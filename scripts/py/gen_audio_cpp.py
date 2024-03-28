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
Utility script to convert a set of audio clip in a given location into
corresponding cpp files and a single hpp file referencing the vectors
from the cpp files.
"""
import datetime
import glob
import math
from argparse import ArgumentParser
from pathlib import Path

import numpy as np
from jinja2 import Environment, FileSystemLoader

from gen_utils import GenUtils, AudioSample

# pylint: disable=duplicate-code
parser = ArgumentParser()

parser.add_argument(
    "--audio_path",
    type=str,
    help="path to audio folder to convert."
)

parser.add_argument(
    "--source_folder_path",
    type=str,
    help="path to source folder to be generated."
)

parser.add_argument(
    "--header_folder_path",
    type=str,
    help="path to header folder to be generated."
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
def write_hpp_file(
        header_filepath,
        header,
        num_audios,
        audio_array_namesizes
):
    """
    Write audio hpp file

    @param header_filepath:         .hpp filepath
    @param header:                  Rendered header
    @param num_audios:              Audio file index
    @param audio_array_namesizes:   Audio array name sizes
    """
    print(f"++ Generating {header_filepath}")

    env \
        .get_template('AudioClips.hpp.template') \
        .stream(common_template_header=header,
                clips_count=num_audios,
                varname_size=audio_array_namesizes) \
        .dump(str(header_filepath))


def write_cc_file(
        cc_filepath,
        header,
        num_audios,
        audio_filenames,
        audio_array_namesizes
):
    """
    Write cc file

    @param cc_filepath:             .cc filepath
    @param header:                  Rendered header
    @param num_audios:              Audio file index
    @param audio_filenames:         Audio filenames
    @param audio_array_namesizes:   Audio array name sizes
    """
    print(f"++ Generating {cc_filepath}")

    env \
        .get_template('AudioClips.cc.template') \
        .stream(common_template_header=header,
                clips_count=num_audios,
                var_names=(name for name, _ in audio_array_namesizes),
                clip_sizes=(size for _, size in audio_array_namesizes),
                clip_names=audio_filenames) \
        .dump(str(cc_filepath))


def write_individual_audio_cc_file(
        resampled_audio: AudioSample,
        clip_filename,
        cc_filename,
        header_template_file,
        array_name
):
    """
    Writes the provided audio sample to a .cc file

    @param resampled_audio:         Audio sample to write
    @param clip_filename:           File name of the clip
    @param cc_filename:             File name of the .cc file
    @param header_template_file:    Header template
    @param array_name:              Name of the array to write
    @return:                        Array length of the audio data written
    """
    print(f"++ Converting {clip_filename} to {Path(cc_filename).name}")

    # Change from [-1, 1] fp32 range to int16 range.
    clip_data = np.clip((resampled_audio.data * (1 << 15)),
                        np.iinfo(np.int16).min,
                        np.iinfo(np.int16).max).flatten().astype(np.int16)

    hdr = GenUtils.gen_header(env, header_template_file, clip_filename)

    hex_line_generator = (', '.join(map(hex, sub_arr))
                          for sub_arr in np.array_split(clip_data, math.ceil(len(clip_data) / 20)))

    env \
        .get_template('audio.cc.template') \
        .stream(common_template_header=hdr,
                size=len(clip_data),
                var_name=array_name,
                audio_data=hex_line_generator) \
        .dump(str(cc_filename))

    return len(clip_data)


def create_audio_cc_file(args, filename, array_name, clip_dirpath):
    """
    Create an individual audio cpp file

    @param args:            User-specified args
    @param filename:        Audio filename
    @param array_name:      Name of the array in the audio .cc file
    @param clip_dirpath:    Audio file directory path
    @return:                Array length of the audio data written
    """
    cc_filename = (Path(args.source_folder_path) /
                   (Path(filename).stem.replace(" ", "_") + ".cc"))
    audio_filepath = Path(clip_dirpath) / filename
    audio_sample = GenUtils.read_audio_file(audio_filepath, args.offset, args.duration)
    resampled_audio = GenUtils.resample_audio_clip(
        audio_sample, args.sampling_rate, args.mono, args.res_type, args.min_samples
    )
    return write_individual_audio_cc_file(
        resampled_audio, filename, cc_filename, args.license_template, array_name,
    )


def main(args):
    """
    Convert audio files to .cc + .hpp files
    @param args:    Parsed args
    """
    # Keep the count of the audio files converted
    audioclip_idx = 0
    audioclip_filenames = []
    audioclip_array_names = []
    header_filename = "InputFiles.hpp"
    common_cc_filename = "InputFiles.cc"
    header_filepath = Path(args.header_folder_path) / header_filename
    common_cc_filepath = Path(args.source_folder_path) / common_cc_filename

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
            array_size = create_audio_cc_file(args, filename, array_name, clip_dirpath)

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

        write_hpp_file(
            header_filepath,
            header,
            audioclip_idx,
            audioclip_array_names
        )

        write_cc_file(
            common_cc_filepath,
            header,
            audioclip_idx,
            audioclip_filenames,
            audioclip_array_names
        )

    else:
        raise FileNotFoundError("No valid audio clip files found.")


if __name__ == '__main__':
    main(parsed_args)
