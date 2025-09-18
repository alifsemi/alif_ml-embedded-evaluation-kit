#  SPDX-FileCopyrightText:  Copyright 2025 Arm Limited
#  and/or its affiliates <open-source-office@arm.com>
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
"""
Script to generate a placeholder Conformer model.
"""
import typing
import torch
from conformer import model as conformer_model

# Audio chunk size
CHUNK_SIZE = 1500
VOCAB_SIZE =  128
NUM_MELS   =   80

def create_model(vocab_size: int, num_mels: int) -> torch.nn.Module:
    """
    Creates a Conformer model given the vocabulary size and
    number of MEL coefficients
    :param vocab_size:  Vocabulary size
    :param num_mels:    Number of MEL coefficients per window.
    :retrun:            Model object expressed as a `torch.nn.Module`.
    """
    model = conformer_model.Conformer(
        num_classes=vocab_size,
        input_dim=num_mels,
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
    )

    return model.eval()


def model_with_inputs() -> typing.Tuple[torch.nn.Module, typing.Tuple | torch.tensor]:
    """
    Gets a tuple of model and its corresponding representative input. The input
    can either be a torch.tensor or a tuple of tensors if the model accepts more
    than one input.
    """
    model = create_model(VOCAB_SIZE, NUM_MELS)
    inputs = (
        torch.rand(1, CHUNK_SIZE, NUM_MELS),
        torch.tensor([CHUNK_SIZE], dtype=torch.int32),
    )
    return model, inputs


ModelUnderTest, ModelInputs = model_with_inputs()
