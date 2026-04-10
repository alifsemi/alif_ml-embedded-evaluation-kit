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
    Represents audio sample data with its sample rate.
    """
    data: np.ndarray
    sample_rate: int


def res_data_type(res_type_value):
    """
    Validate and return a supported resample type string.

    :param res_type_value:    Candidate resample type value.
    :returns:                 The validated resample type value.
    :raises argparse.ArgumentTypeError:
                              Raised if ``res_type_value`` is not supported.
    """
    if res_type_value not in res_type_list():
        raise argparse.ArgumentTypeError(
            f"{res_type_value} not valid. Supported only {res_type_list()}"
        )
    return res_type_value


def res_type_list():
    """
    Return the supported resample type names.

    :returns:    List of supported resample filter names.
    """
    return ['kaiser_best', 'kaiser_fast']


def read_audio_file(
        path,
        offset,
        duration
) -> AudioSample:
    """
    Read an audio file into an :class:`AudioSample`.

    :param path:        Path to the audio file.
    :param offset:      Offset, in seconds, from which to start reading.
    :param duration:    Duration, in seconds, to read. Non-positive values
                        read to the end of the file.
    :returns:           Audio data and its sample rate.
    :raises OSError:    Raised if the audio file cannot be opened.
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


def _resample_audio(
        y,
        target_sr,
        origin_sr,
        res_type
):
    """
    Resample audio to a different sample rate.

    :param y:           Audio array to resample.
    :param target_sr:   Target sample rate.
    :param origin_sr:   Original sample rate.
    :param res_type:    Resampling filter name.
    :returns:           The resampled audio array.
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


def resample_audio_clip(
        audio_sample: AudioSample,
        target_sr=16000,
        mono=True,
        res_type='kaiser_best',
        min_len=16000
) -> AudioSample:
    """
    Load and resample an audio clip with the given desired specs.

    :param audio_sample:    Audio sample to transform.
    :param target_sr:       Target sampling rate. Non-positive values preserve
                            the native sample rate.
    :param mono:            Whether to convert multi-channel audio to mono.
    :param res_type:        Resample filter name to use.
    :param min_len:         Minimum output length in samples. Shorter outputs
                            are zero-padded to this size.
    :returns:               Resampled audio data and the resulting sample rate.
    """
    y = audio_sample.data.copy()

    # Convert to mono if requested and if audio has more than one dimension
    if mono and (audio_sample.data.ndim > 1):
        y = np.mean(y, axis=0)

    if not (audio_sample.sample_rate == target_sr) and (target_sr > 0):
        y = _resample_audio(y, target_sr, audio_sample.sample_rate, res_type)
        sample_rate = target_sr
    else:
        sample_rate = audio_sample.sample_rate

    # Pad if necessary and min lenght is setted (min_len> 0)
    if (y.shape[0] < min_len) and (min_len > 0):
        sample_to_pad = min_len - y.shape[0]
        y = np.pad(y, (0, sample_to_pad), 'constant', constant_values=0)

    return AudioSample(data=y, sample_rate=sample_rate)


def gen_header(
        env: jinja2.Environment,
        header_template_file: str,
        file_name: str = None
) -> str:
    """
    Generate the common licence header text.

    :param env:                     Jinja2 environment.
    :param header_template_file:    Path to the licence header template.
    :param file_name:               Optional generated file name to include.
    :returns:                       Rendered licence header string.
    """
    header_template = env.get_template(header_template_file)
    return header_template.render(script_name=Path(__file__).name,
                                  gen_time=datetime.datetime.now(),
                                  file_name=file_name,
                                  year=datetime.datetime.now().year)
