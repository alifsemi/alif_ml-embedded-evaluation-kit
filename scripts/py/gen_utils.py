#!env/bin/python3

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

import soundfile as sf
import resampy
import numpy as np


class AudioUtils:
    @staticmethod
    def res_data_type(res_type_value):
        """
        Returns the input string if is one of the valid resample type
        """
        import argparse
        if res_type_value not in AudioUtils.res_type_list():
            raise argparse.ArgumentTypeError(f"{res_type_value} not valid. Supported only {AudioUtils.res_type_list()}")
        return res_type_value

    @staticmethod
    def res_type_list():
        """
        Returns the resample type list
        """
        return ['kaiser_best', 'kaiser_fast']

    @staticmethod
    def load_resample_audio_clip(path, target_sr=16000, mono=True, offset=0.0, duration=0, res_type='kaiser_best',
                                 min_len=16000):
        """
        Load and resample an audio clip with the given desired specs.

        Parameters:
        ----------
        path (string):             Path to the input audio clip.
        target_sr (int, optional): Target sampling rate. Positive number are considered valid, 
                                    if zero or negative the native sampling rate of the file will be preserved. Default is 16000. 
        mono (bool, optional):     Specify if the audio file needs to be converted to mono. Default is True.
        offset (float, optional):  Target sampling rate. Default is 0.0.
        duration (int, optional):  Target duration. Positive number are considered valid, 
                                    if zero or negative the duration of the file will be preserved. Default is 0.
        res_type (int, optional):  Resample type to use,  Default is 'kaiser_best'.
        min_len (int, optional):   Minimun lenght of the output audio time series. Default is 16000.

        Returns:
        ----------
        y (np.ndarray): Output audio time series of shape shape=(n,) or (2, n).
        sr (int):       A scalar number > 0 that represent the sampling rate of `y`
        """
        try:
            with sf.SoundFile(path) as audio_file:
                origin_sr = audio_file.samplerate

                if offset:
                    # Seek to the start of the target read
                    audio_file.seek(int(offset * origin_sr))

                if duration > 0:
                    num_frame_duration = int(duration * origin_sr)
                else:
                    num_frame_duration = -1

                # Load the target number of frames
                y = audio_file.read(frames=num_frame_duration, dtype=np.float32, always_2d=False).T

        except:
            print(f"Failed to open {path} as an audio.")

        # Convert to mono if requested and if audio has more than one dimension
        if mono and (y.ndim > 1):
            y = np.mean(y, axis=0)

        if not (origin_sr == target_sr) and (target_sr > 0):
            ratio = float(target_sr) / origin_sr
            axis = -1
            n_samples = int(np.ceil(y.shape[axis] * ratio))

            # Resample using resampy
            y_rs = resampy.resample(y, origin_sr, target_sr, filter=res_type, axis=axis)
            n_rs_samples = y_rs.shape[axis]

            # Adjust the size
            if n_rs_samples > n_samples:
                slices = [slice(None)] * y_rs.ndim
                slices[axis] = slice(0, n_samples)
                y = y_rs[tuple(slices)]
            elif n_rs_samples < n_samples:
                lengths = [(0, 0)] * y_rs.ndim
                lengths[axis] = (0, n_samples - n_rs_samples)
                y = np.pad(y_rs, lengths, 'constant', constant_values=(0))

            sr = target_sr
        else:
            sr = origin_sr

        # Pad if necessary and min lenght is setted (min_len> 0)
        if (y.shape[0] < min_len) and (min_len > 0):
            sample_to_pad = min_len - y.shape[0]
            y = np.pad(y, (0, sample_to_pad), 'constant', constant_values=(0))

        return y, sr
