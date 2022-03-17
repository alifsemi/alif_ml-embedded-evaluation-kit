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
This script can be used with the noise reduction use case to save
the dumped noise reduced audio to a wav file.

Example use:
python rnnoise_dump_extractor.py --dump_file output.bin --output_dir ./denoised_wavs/
"""

import soundfile as sf
import numpy as np

import argparse
from os import path
import struct


def extract(fp, output_dir, export_npy):
    while True:
        filename_length = struct.unpack("i", fp.read(4))[0]

        if filename_length == -1:
            return

        filename = struct.unpack("{}s".format(filename_length), fp.read(filename_length))[0].decode('ascii')
        audio_clip_length = struct.unpack("I", fp.read(4))[0]
        output_file_name = path.join(output_dir, "denoised_{}".format(filename))
        audio_clip = fp.read(audio_clip_length)
        
        with sf.SoundFile(output_file_name, 'w', channels=1, samplerate=48000, subtype="PCM_16", endian="LITTLE") as wav_file:
            wav_file.buffer_write(audio_clip, dtype='int16')
            print("{} written to disk".format(output_file_name))

        if export_npy:
            output_file_name += ".npy"
            pack_format = "{}h".format(int(audio_clip_length/2))
            npdata = np.array(struct.unpack(pack_format, audio_clip)).astype(np.int16)
            np.save(output_file_name, npdata)
            print("{} written to disk".format(output_file_name))


def main(args):
    extract(args.dump_file, args.output_dir, args.export_npy)


parser = argparse.ArgumentParser()
parser.add_argument("--dump_file", type=argparse.FileType('rb'), help="Dump file with audio files to extract.", required=True)
parser.add_argument("--output_dir", help="Output directory, Warning: Duplicated file names will be overwritten.", required=True)
parser.add_argument("--export_npy", help="Export the audio buffer in NumPy format", action="store_true")
args = parser.parse_args()

if __name__ == "__main__":
    main(args)
