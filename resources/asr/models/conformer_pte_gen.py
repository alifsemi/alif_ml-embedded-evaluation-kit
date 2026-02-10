# ----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025-2026 Arm Limited and/or its
#  affiliates <open-source-office@arm.com>
#  SPDX-License-Identifier: Apache-2.0
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ----------------------------------------------------------------------------
"""
Utility script to optimize Conformer model for ExecuTorch.
"""

import argparse
import logging
import typing
from dataclasses import dataclass
from pathlib import Path

import soundfile as sf
import torch
from conformer import Conformer
from executorch.backends.arm.ethosu import EthosUPartitioner, EthosUCompileSpec
from executorch.backends.arm.quantizer import (
    EthosUQuantizer,
)
from executorch.backends.arm.quantizer import (
    get_symmetric_quantization_config,
)
from executorch.exir import (
    EdgeCompileConfig,
    ExecutorchBackendConfig,
    to_edge_transform_and_lower,
)
from executorch.exir.serde.schema import GraphModule
from executorch.extension.export_util.utils import save_pte_program
from torch.export import export
from torchao.quantization.pt2e.quantize_pt2e import convert_pt2e, prepare_pt2e
from torchaudio.transforms import MelSpectrogram

# pylint: disable=fixme
# TODO: this is brittle and shouldn't be the long-term solution
current_dir = Path(__file__).parent
samples_dir = current_dir.parent / "samples"

SAMPLE_RATE = 16000
CHUNK_SIZE = 700
NUM_MELS = 80

mel_spectrogram = MelSpectrogram(
    sample_rate=SAMPLE_RATE,
    n_fft=512,
    win_length=512,
    hop_length=160,
    n_mels=NUM_MELS,
    center=False
)

device = torch.device("cpu")

logger = logging.getLogger(__name__)
if not logger.handlers:
    logging.basicConfig(level=logging.INFO)


@dataclass(frozen=True)
class TargetConfig:
    """
    Utility class to store target configuration.
    """
    target_name: str
    system_config: str
    memory_mode: str


def preprocess_audio(audio: Path | str) -> torch.Tensor:
    """
    Preprocess audio to produce mel spectrogram
    :param audio:   Audio data
    :return:        Generated mel spectrogram data
    """

    # torchaudio.load(audio) will also work if torchcodec + ffmpeg are installed
    with sf.SoundFile(audio) as f:
        waveform = torch.Tensor(f.read())
        sr = f.samplerate

    assert sr == SAMPLE_RATE

    spec = mel_spectrogram(waveform).squeeze(0).transpose(0, 1)  # (T, 80)
    spec = spec.add_(1e-6).log()

    audio_length = spec.shape[0]

    # If the sample is longer than CHUNK_SIZE, take the first CHUNK_SIZE numbers
    # Because the log MelSpectrogram has a limited input range, a single sample is enough
    # to obtain model with good int8 accuracy
    if audio_length >= CHUNK_SIZE:
        spec = spec[:CHUNK_SIZE]
    else:
        # pad with -20s in case of audio clip shorter than CHUNK_SIZE
        pad_len = CHUNK_SIZE - audio_length
        pad = -20 * torch.ones(pad_len, spec.size(1), dtype=spec.dtype)
        spec = torch.cat((spec, pad), dim=0)
    spec = spec.unsqueeze(0)
    return spec


def create_example_inputs() -> typing.Tuple[torch.Tensor, torch.Tensor]:
    """
    Create sample inputs to Conformer
    :return:    A tuple of sample inputs for Conformer
    """
    return (
        torch.rand(1, CHUNK_SIZE, NUM_MELS),
        torch.tensor([CHUNK_SIZE], dtype=torch.int32),
    )


def load_model(checkpoint_path: Path | str) -> Conformer:
    """
    Load the Conformer model from the specified checkpoint path
    :param checkpoint_path:     Path to the checkpoint
    :return:                    The Conformer model in eval mode
    """
    # Conformer model with the same hyperparameters used for training
    model = Conformer(
        num_classes=129,
        input_dim=NUM_MELS,
        encoder_dim=144,
        num_encoder_layers=16,
        num_attention_heads=4,
        feed_forward_expansion_factor=4,
        conv_expansion_factor=2,
        input_dropout_p=0.1,
        feed_forward_dropout_p=0.1,
        attention_dropout_p=0.1,
        conv_dropout_p=0.1,
        conv_kernel_size=31,
        half_step_residual=True,
    ).to(device)

    # Load the checkpoint, state dict of the trained FP32 model and put the model in eval mode
    checkpoint = torch.load(checkpoint_path, map_location=device, weights_only=True)
    model.load_state_dict(checkpoint["model"])
    model.eval()
    return model


def quantize(
        graph_module: GraphModule,
        example_inputs: typing.Tuple[torch.Tensor, torch.Tensor],
        audio_samples_dir: Path,
        cfg: TargetConfig,
        config_ini: typing.Optional[Path | str] = None
):
    """
    Quantize the Conformer graph for the specified Ethos-U target.
    :param graph_module:       Graph module exported from the FP32 model.
    :param example_inputs:     Example inputs used for exporting the program.
    :param audio_samples_dir:  Directory containing calibration audio samples.
    :param cfg:                Target configuration specifying the Ethos-U variant.
    :param config_ini:         Optional path to a Vela configuration file.
    :return:                   The exported program and the Ethos-U partitioner.
    """
    compile_spec = EthosUCompileSpec(
        target=cfg.target_name,
        system_config=cfg.system_config,
        memory_mode=cfg.memory_mode,
        config_ini=config_ini,
        extra_flags=["--output-format=raw", "--debug-force-regor"],
    )
    quantizer = EthosUQuantizer(compile_spec)
    partitioner = EthosUPartitioner(compile_spec)
    quantizer.set_global(get_symmetric_quantization_config(is_per_channel=True))
    quantized_graph_module = prepare_pt2e(graph_module, quantizer)
    logger.info("Calibrating...")
    chunk_tensor = torch.tensor([CHUNK_SIZE], dtype=torch.int32)
    for sample in list(audio_samples_dir.glob("*.wav")):
        audio_tensor = preprocess_audio(sample)
        quantized_graph_module(audio_tensor, chunk_tensor)
    quantized_graph_module = convert_pt2e(quantized_graph_module)
    exported_program = export(quantized_graph_module, example_inputs, strict=True)
    return exported_program, partitioner


def generate_pte(
        checkpoint_path: Path | str,
        cfg: TargetConfig,
        config_ini: Path | str,
        output: Path | str
):
    """
    Generate a PTE file given the target config.
    :param checkpoint_path: Path to the checkpoint to be loaded.
    :param cfg:             Target configuration object.
    :param config_ini:      Vela configuration file path.
    :param output:          Output directory to save the pte file.
    """
    # Pre-processing as per the Conformer paper and how the model was trained
    example_inputs = create_example_inputs()
    exported_program = export(
        load_model(checkpoint_path), example_inputs, strict=True
    )
    graph_module = exported_program.module(check_guards=False)
    model_name = Path(checkpoint_path).name.split('.')[0]

    if cfg.target_name.startswith("ethos-u"):
        output_model_name = f"{model_name}_arm_delegate_{cfg.target_name}.pte"
        exported_program, partitioner = quantize(
            graph_module,
            example_inputs,
            samples_dir,
            cfg,
            config_ini
        )
    else:
        output_model_name = f"{model_name}_arm_{cfg.target_name}.pte"
        partitioner = None

    # Lower the exported program to the target backend
    logger.info("Calling to_edge_transform_and_lower")
    edge_program_manager = to_edge_transform_and_lower(
        exported_program,
        partitioner=[partitioner] if partitioner else None,
        compile_config=EdgeCompileConfig(
            _check_ir_validity=False,
        ),
    )
    if Path(output).is_dir():
        output = Path(output) / output_model_name

    save_pte_program(
        edge_program_manager.to_executorch(
            config=ExecutorchBackendConfig(extract_delegate_segments=False)
        ),
        str(output),
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-m",
        "--model_name",
        required=True,
        help="Path to the model as an exported program."
    )
    parser.add_argument(
        "-t",
        "--target",
        action="store",
        required=False,
        default="ethos-u85-256",
        help="For ArmBackend delegated models."
    )
    parser.add_argument(
        "-o",
        "--output",
        action="store",
        required=False,
        help="Output directory to save the model in."
    )
    parser.add_argument(
        "--system_config",
        required=False,
        default=None,
        help="System configuration to select from the Vela configuration file (see vela.ini)."
    )
    parser.add_argument(
        "--memory_mode",
        required=False,
        default=None,
        help="Memory mode to select from the Vela configuration file (see vela.ini)."
    )
    parser.add_argument(
        "--config",
        required=False,
        default="Arm/vela.ini",
        help="Specify custom vela configuration file (vela.ini)",
    )
    args, _ = parser.parse_known_args()

    if not str(args.model_name).endswith(".pt"):
        raise ValueError(f"Model name {args.model_name} should be a pt2 file")

    generate_pte(checkpoint_path=args.model_name,
                 cfg=TargetConfig(target_name=args.target,
                                  system_config=args.system_config,
                                  memory_mode=args.memory_mode),
                 config_ini=args.config,
                 output=args.output)
