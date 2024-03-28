#!env/bin/python3

#  SPDX-FileCopyrightText:  Copyright 2021, 2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
Utility script to convert an audio clip into eval platform desired spec.
"""
from argparse import ArgumentParser
from os import path

import soundfile as sf

from gen_utils import GenUtils

parser = ArgumentParser()

# pylint: disable=duplicate-code
parser.add_argument(
    "--audio_path",
    help="Audio file path",
    required=True
)

parser.add_argument(
    "--output_dir",
    help="Output directory",
    required=True
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
    "-v",
    "--verbosity",
    action="store_true"
)
# pylint: enable=duplicate-code

parsed_args = parser.parse_args()


def main(args):
    """
    Generate the new audio file
    @param args:    Parsed args
    """
    audio_sample = GenUtils.read_audio_file(
        args.audio_path, args.offset, args.duration
    )

    resampled_audio = GenUtils.resample_audio_clip(
        audio_sample, args.sampling_rate, args.mono, args.res_type, args.min_samples
    )

    sf.write(
        path.join(args.output_dir, path.basename(args.audio_path)),
        resampled_audio.data,
        resampled_audio.sample_rate
    )


if __name__ == '__main__':
    main(parsed_args)
