#  SPDX-FileCopyrightText:  Copyright 2026 Arm Limited and/or its
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
"""ML framework domain descriptors."""
import typing
from argparse import ArgumentTypeError
from enum import Enum
from types import MappingProxyType


class MLFramework(str, Enum):
    """Supported ML inference frameworks."""

    TENSORFLOW_LITE_MICRO = "tflm"
    EXECUTORCH = "executorch"

    def __str__(self) -> str:
        """
        Return the canonical framework value.

        :return:    Canonical framework value.
        """
        return self.value

    @staticmethod
    def accepted_values() -> typing.FrozenSet[str]:
        """
        Return all supported framework selector values.

        :return:    Framework selector values.
        """
        return _ML_FRAMEWORK_VALUES

    @staticmethod
    def from_value(value: str) -> typing.Optional["MLFramework"]:
        """
        Return the framework matching a framework selector.

        :param value: Framework selector value.
        :return:      Matching framework, or ``None``.
        """
        return _ML_FRAMEWORKS_BY_VALUE.get(value.casefold())


def parse_ml_framework(value: str) -> MLFramework:
    """
    Parse an ML framework selector.

    :param value:                ML framework selector value.
    :return:                     Matching ML framework.
    :raises ArgumentTypeError:   If the value does not match a framework.
    """
    ml_framework = MLFramework.from_value(value)
    if ml_framework is None:
        accepted_values = ", ".join(sorted(MLFramework.accepted_values()))
        raise ArgumentTypeError(
            f"Invalid ML framework: {value}. Valid values are: {accepted_values}"
        )

    return ml_framework


_ML_FRAMEWORKS_BY_VALUE: typing.Mapping[str, MLFramework] = MappingProxyType({
    MLFramework.TENSORFLOW_LITE_MICRO.value: MLFramework.TENSORFLOW_LITE_MICRO,
    "tensorflowlitemicro": MLFramework.TENSORFLOW_LITE_MICRO,
    MLFramework.EXECUTORCH.value: MLFramework.EXECUTORCH,
    "et": MLFramework.EXECUTORCH,
})

_ML_FRAMEWORK_VALUES: typing.FrozenSet[str] = frozenset(_ML_FRAMEWORKS_BY_VALUE)
