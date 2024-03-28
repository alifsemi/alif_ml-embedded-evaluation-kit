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
Utility functions for .cc + .hpp file generation
"""
import argparse
import datetime
from dataclasses import dataclass
from pathlib import Path

import jinja2
import numpy as np
import resampy
import soundfile as sf


@dataclass
class AudioSample:
    """
    Represents an audio sample with its sample rate
    """
    data: np.ndarray
    sample_rate: int


class GenUtils:
    """
    Class with utility functions for audio and other .cc + .hpp file generation
    """

    @staticmethod
    def res_data_type(res_type_value):
        """
        Returns the input string if is one of the valid resample type
        """
        if res_type_value not in GenUtils.res_type_list():
            raise argparse.ArgumentTypeError(
                f"{res_type_value} not valid. Supported only {GenUtils.res_type_list()}"
            )
        return res_type_value

    @staticmethod
    def res_type_list():
        """
        Returns the resample type list
        """
        return ['kaiser_best', 'kaiser_fast']

    @staticmethod
    def read_audio_file(
            path,
            offset,
            duration
    ) -> AudioSample:
        """
        Reads an audio file to an array

        @param path:        Path to audio file
        @param offset:      Offset to read from
        @param duration:    Duration to read
        @return:            The audio data and the sample rate
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
        except OSError as err:
            print(f"Failed to open {path} as an audio.")
            raise err

        return AudioSample(y, origin_sr)

    @staticmethod
    def _resample_audio(
            y,
            target_sr,
            origin_sr,
            res_type
    ):
        """
        Resamples audio to a different sample rate

        @param y:           Audio to resample
        @param target_sr:   Target sample rate
        @param origin_sr:   Original sample rate
        @param res_type:    Resample type
        @return:            The resampled audio
        """
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
            y = np.pad(y_rs, lengths, 'constant', constant_values=0)

        return y

    @staticmethod
    def resample_audio_clip(
            audio_sample: AudioSample,
            target_sr=16000,
            mono=True,
            res_type='kaiser_best',
            min_len=16000
    ) -> AudioSample:
        """
        Load and resample an audio clip with the given desired specs.

        Parameters:
        ----------
        path (string):              Path to the input audio clip.
        target_sr (int, optional):  Target sampling rate. Positive number are considered valid,
                                    if zero or negative the native sampling rate of the file
                                    will be preserved. Default is 16000.
        mono (bool, optional):      Specify if the audio file needs to be converted to mono.
                                    Default is True.
        offset (float, optional):   Target sampling rate. Default is 0.0.
        duration (int, optional):   Target duration. Positive number are considered valid,
                                    if zero or negative the duration of the file
                                    will be preserved. Default is 0.
        res_type (int, optional):   Resample type to use,  Default is 'kaiser_best'.
        min_len (int, optional):    Minimum length of the output audio time series.
                                    Default is 16000.

        Returns:
        ----------
        y (np.ndarray):     Output audio time series of shape=(n,) or (2, n).
        sample_rate (int):  A scalar number > 0 that represent the sampling rate of `y`
        """
        y = audio_sample.data.copy()

        # Convert to mono if requested and if audio has more than one dimension
        if mono and (audio_sample.data.ndim > 1):
            y = np.mean(y, axis=0)

        if not (audio_sample.sample_rate == target_sr) and (target_sr > 0):
            y = GenUtils._resample_audio(y, target_sr, audio_sample.sample_rate, res_type)
            sample_rate = target_sr
        else:
            sample_rate = audio_sample.sample_rate

        # Pad if necessary and min lenght is setted (min_len> 0)
        if (y.shape[0] < min_len) and (min_len > 0):
            sample_to_pad = min_len - y.shape[0]
            y = np.pad(y, (0, sample_to_pad), 'constant', constant_values=0)

        return AudioSample(data=y, sample_rate=sample_rate)

    @staticmethod
    def gen_header(
            env: jinja2.Environment,
            header_template_file: str,
            file_name: str = None
    ) -> str:
        """
        Generate common licence header

        :param env:                     Jinja2 environment
        :param header_template_file:    Path to the licence header template
        :param file_name:               Optional generating script file name
        :return:                        Generated licence header as a string
        """
        header_template = env.get_template(header_template_file)
        return header_template.render(script_name=Path(__file__).name,
                                      gen_time=datetime.datetime.now(),
                                      file_name=file_name,
                                      year=datetime.datetime.now().year)
