#  Copyright (c) 2021 Arm Limited. All rights reserved.
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
from pathlib import Path
from argparse import ArgumentParser

import numpy as np
from jinja2 import Environment, FileSystemLoader

from gen_utils import AudioUtils

parser = ArgumentParser()
parser.add_argument("--audio_path", type=str, help="path to audio folder to convert.")
parser.add_argument("--source_folder_path", type=str, help="path to source folder to be generated.")
parser.add_argument("--header_folder_path", type=str, help="path to header folder to be generated.")
parser.add_argument("--sampling_rate", type=int, help="target sampling rate.", default=16000)
parser.add_argument("--mono", type=bool, help="convert signal to mono.", default=True)
parser.add_argument("--offset", type=float, help="start reading after this time (in seconds).", default=0)
parser.add_argument("--duration", type=float, help="only load up to this much audio (in seconds).", default=0)
parser.add_argument("--res_type", type=AudioUtils.res_data_type, help=f"Resample type: {AudioUtils.res_type_list()}.",
                    default='kaiser_best')
parser.add_argument("--min_samples", type=int, help="Minimum sample number.", default=16000)
parser.add_argument("--license_template", type=str, help="Header template file",
                    default="header_template.txt")
parser.add_argument("-v", "--verbosity", action="store_true")
args = parser.parse_args()

env = Environment(loader=FileSystemLoader(Path(__file__).parent / 'templates'),
                  trim_blocks=True,
                  lstrip_blocks=True)


def write_hpp_file(header_filepath, cc_filepath, header_template_file, num_audios, audio_filenames, audio_array_namesizes):
    print(f"++ Generating {header_filepath}")

    header_template = env.get_template(header_template_file)
    hdr = header_template.render(script_name=Path(__file__).name,
                                 gen_time=datetime.datetime.now(),
                                 year=datetime.datetime.now().year)
    env.get_template('AudioClips.hpp.template').stream(common_template_header=hdr,
                                                       clips_count=num_audios,
                                                       varname_size=audio_array_namesizes
                                                       ) \
        .dump(str(header_filepath))

    print(f"++ Generating {cc_filepath}")

    env.get_template('AudioClips.cc.template').stream(common_template_header=hdr,
                                                       clips_count=num_audios,
                                                       var_names=(name for name, _ in audio_array_namesizes),
                                                       clip_sizes=(size for _, size in audio_array_namesizes),
                                                       clip_names=audio_filenames) \
        .dump(str(cc_filepath))


def write_individual_audio_cc_file(clip_dirpath, clip_filename,
                                   cc_filename, header_template_file, array_name,
                                   sampling_rate_value, mono_value, offset_value,
                                   duration_value, res_type_value, min_len):
    print(f"++ Converting {clip_filename} to {Path(cc_filename).name}")
    audio_filepath = Path(clip_dirpath) / clip_filename
    clip_data, samplerate = AudioUtils.load_resample_audio_clip(audio_filepath,
                                                                sampling_rate_value, mono_value,
                                                                offset_value, duration_value,
                                                                res_type_value, min_len)

    # Change from [-1, 1] fp32 range to int16 range.
    clip_data = np.clip((clip_data * (1 << 15)),
                        np.iinfo(np.int16).min,
                        np.iinfo(np.int16).max).flatten().astype(np.int16)

    header_template = env.get_template(header_template_file)
    hdr = header_template.render(script_name=Path(__file__).name,
                                 gen_time=datetime.datetime.now(),
                                 file_name=clip_filename,
                                 year=datetime.datetime.now().year)

    hex_line_generator = (', '.join(map(hex, sub_arr))
                          for sub_arr in np.array_split(clip_data, math.ceil(len(clip_data)/20)))

    env.get_template('audio.cc.template').stream(common_template_header=hdr,
                                                 size=len(clip_data),
                                                 var_name=array_name,
                                                 audio_data=hex_line_generator) \
        .dump(str(cc_filename))

    return len(clip_data)


def main(args):
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
            cc_filename = Path(args.source_folder_path) / (Path(filename).stem.replace(" ", "_") + ".cc")
            array_name = "audio" + str(audioclip_idx)
            array_size = write_individual_audio_cc_file(clip_dirpath, filename, cc_filename, args.license_template, array_name,
                                                        args.sampling_rate, args.mono, args.offset,
                                                        args.duration, args.res_type, args.min_samples)

            audioclip_array_names.append((array_name, array_size))
            # Increment audio index
            audioclip_idx = audioclip_idx + 1
        except:
            if args.verbosity:
                print(f"Failed to open {filename} as an audio.")

    if len(audioclip_filenames) > 0:
        write_hpp_file(header_filepath, common_cc_filepath, args.license_template,
                    audioclip_idx, audioclip_filenames, audioclip_array_names)
    else:
        raise FileNotFoundError("No valid audio clip files found.")


if __name__ == '__main__':
    main(args)
