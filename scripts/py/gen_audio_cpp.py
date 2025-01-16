#  SPDX-FileCopyrightText:  Copyright 2021, 2023-2024 Arm Limited and/or
#  its affiliates <open-source-office@arm.com>
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
Utility script to convert a set of audio clip in a given location into
corresponding cpp files and a single hpp file referencing the vectors
from the cpp files.
"""
import datetime
import glob
import math
from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path
import typing

import numpy as np
from jinja2 import Environment, FileSystemLoader

from gen_utils import GenUtils

# pylint: disable=duplicate-code
parser = ArgumentParser()

parser.add_argument(
    "--audio_path",
    type=str,
    help="path to audio folder to convert."
)

parser.add_argument(
    "--package_gen_dir",
    type=str,
    help="path to directory to be generated."
)

parser.add_argument(
    "--sampling_rate",
    type=int,
    help="target sampling rate.",
    default=16000
)

parser.add_argument(
    "--mono",
    type=bool,
    help="convert signal to mono.",
    default=True
)

parser.add_argument(
    "--offset",
    type=float,
    help="start reading after this time (in seconds).",
    default=0
)

parser.add_argument(
    "--duration",
    type=float,
    help="only load up to this much audio (in seconds).",
    default=0
)

parser.add_argument(
    "--res_type",
    type=GenUtils.res_data_type,
    help=f"Resample type: {GenUtils.res_type_list()}.",
    default='kaiser_best'
)

parser.add_argument(
    "--min_samples",
    type=int,
    help="Minimum sample number.",
    default=16000
)

parser.add_argument(
    "--license_template",
    type=str,
    help="Header template file",
    default="header_template.txt"
)

parser.add_argument(
    "-v",
    "--verbosity",
    action="store_true"
)

parsed_args = parser.parse_args()

env = Environment(loader=FileSystemLoader(Path(__file__).parent / 'templates'),
                  trim_blocks=True,
                  lstrip_blocks=True)

# pylint: enable=duplicate-code
@dataclass
class AudioParams:
    """
    Parameters for audio data
    """
    num_audio_files: int
    audio_names_sizes: typing.List[tuple]
    audio_filenames: typing.List[str]
    sampling_rate: int
    num_channels: int

def write_hpp_file(
        header_filepath: Path,
        header: str,
        audio_params: AudioParams
):
    """
    Write audio hpp file

    @param header_filepath:         .hpp filepath
    @param header:                  Rendered header
    @param audio_params:            Audio parameters
    """
    print(f"++ Generating {header_filepath}")

    env \
        .get_template('sample-data/audio/audio_clips.h.template') \
        .stream(common_template_header=header,
                clips_count=audio_params.num_audio_files,
                varname_size=audio_params.audio_names_sizes,
                sampling_rate=audio_params.sampling_rate,
                n_channels=audio_params.num_channels) \
        .dump(str(header_filepath))


def write_cc_file(
        cc_filepath: Path,
        header: str,
        audio_params: AudioParams,
        header_filename: str
):
    """
    Write cc file

    @param cc_filepath:             .cc filepath
    @param header:                  Rendered header
    @param audio_params:            Audio parameters
    @param header_filename:         Name of the common header file to be included
    """
    print(f"++ Generating {cc_filepath}")

    env \
        .get_template('sample-data/audio/audio_clips.c.template') \
        .stream(common_template_header=header,
                clips_count=audio_params.num_audio_files,
                var_names=(name for name, _ in audio_params.audio_names_sizes),
                clip_sizes=(size for _, size in audio_params.audio_names_sizes),
                clip_names=audio_params.audio_filenames,
                header_filename=header_filename) \
        .dump(str(cc_filepath))


def create_audio_cc_file(args, filename, array_name, clip_dirpath):
    """
    Create an individual audio cpp file

    @param args:            User-specified args
    @param filename:        Audio filename
    @param array_name:      Name of the array in the audio .cc file
    @param clip_dirpath:    Audio file directory path
    @return:                Array length of the audio data written
    """
    cc_filename = (Path(args.package_gen_dir) /
                   (Path(filename).stem.replace(" ", "_") + ".c"))
    audio_filepath = Path(clip_dirpath) / filename
    audio_sample = GenUtils.read_audio_file(audio_filepath, args.offset, args.duration)
    resampled_audio = GenUtils.resample_audio_clip(
        audio_sample, args.sampling_rate, args.mono, args.res_type, args.min_samples
    )

    print(f"++ Converting {filename} to {Path(cc_filename).name}")

    # Change from [-1, 1] fp32 range to int16 range.
    clip_data = np.clip((resampled_audio.data * (1 << 15)),
                        np.iinfo(np.int16).min,
                        np.iinfo(np.int16).max).flatten().astype(np.int16)

    hdr = GenUtils.gen_header(env, args.license_template, filename)
    hex_line_generator = (', '.join(map(hex, sub_arr))
                          for sub_arr in np.array_split(clip_data, math.ceil(len(clip_data) / 20)))

    env \
        .get_template('sample-data/audio/audio.c.template') \
        .stream(common_template_header=hdr,
                size=len(clip_data),
                var_name=array_name,
                audio_data=hex_line_generator) \
        .dump(str(cc_filename))

    return len(clip_data)


def main(args):
    """
    Convert audio files to .cc + .hpp files
    @param args:    Parsed args
    """
    # Keep the count of the audio files converted
    audioclip_idx = 0
    audioclip_filenames = []
    audioclip_array_names = []
    header_filename = "sample_files.h"
    header_filepath = Path(args.package_gen_dir) / header_filename
    common_cc_filepath = Path(args.package_gen_dir) / "sample_files.c"

    if Path(args.audio_path).is_dir():
        filepaths = sorted(glob.glob(str(Path(args.audio_path) / '**/*.wav'), recursive=True))
    elif Path(args.audio_path).is_file():
        filepaths = [args.audio_path]
    else:
        raise OSError("Directory or file does not exist.")

    for filepath in filepaths:
        filename = Path(filepath).name
        clip_dirpath = Path(filepath).parent
        try:
            audioclip_filenames.append(filename)

            # Save the cc file
            array_name = "audio" + str(audioclip_idx)
            array_size = create_audio_cc_file(args,
                                              filename,
                                              array_name,
                                              clip_dirpath)

            audioclip_array_names.append((array_name, array_size))
            # Increment audio index
            audioclip_idx = audioclip_idx + 1
        except OSError:
            if args.verbosity:
                print(f"Failed to open {filename} as an audio.")

    if len(audioclip_filenames) > 0:
        header = env \
            .get_template(args.license_template) \
            .render(script_name=Path(__file__).name,
                    gen_time=datetime.datetime.now(),
                    year=datetime.datetime.now().year)
        audio_params = AudioParams(num_audio_files=len(audioclip_filenames),
                                   audio_names_sizes=audioclip_array_names,
                                   audio_filenames=audioclip_filenames,
                                   sampling_rate=args.sampling_rate,
                                   num_channels=1 if args.mono else 2)

        write_hpp_file(
            header_filepath,
            header,
            audio_params
        )

        write_cc_file(
            common_cc_filepath,
            header,
            audio_params,
            header_filename
        )

    else:
        raise FileNotFoundError("No valid audio clip files found.")


if __name__ == '__main__':
    main(parsed_args)
