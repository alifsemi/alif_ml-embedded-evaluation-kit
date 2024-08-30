#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
Classes to represent NPU configurations for Vela
"""
import itertools
import typing
from dataclasses import dataclass

# The internal SRAM size for Corstone-300 implementation on MPS3 specified by AN552
# The internal SRAM size for Corstone-310 implementation on MPS3 specified by AN555
# is 4MB, but we are content with the 2MB specified below.
MPS3_MAX_SRAM_SZ = 2 * 1024 * 1024  # 2 MiB (2 banks of 1 MiB each)


@dataclass(frozen=True)
class NpuConfig:
    """
    Represents a Vela configuration for an NPU
    """
    name_prefix: str
    macs: int
    processor_id: str
    prefix_id: str
    memory_mode: str
    system_config: str
    arena_cache_size: int = 0

    @property
    def config_name(self) -> str:
        """
        Get the name of the configuration

        For example: "ethos-u55-128" would represent the Ethos-U55 NPU
        with a 128 MAC configuration.

        :return:    The NPU configuration name.
        """
        return f"{self.name_prefix}-{self.macs}"

    @property
    def config_id(self) -> str:
        """
        Get the configuration id as a string

        For example: "Y256" would represent the Ethos-U65 NPU
        with a 256 MAC configuration.

        :return:    The NPU configuration id.
        """
        return f"{self.prefix_id}{self.macs}"

    def overwrite_arena_cache_size(self, arena_cache_size):
        """
        Get a new NPU configuration with the specified
        arena cache size.

        By default, we use the `arena_cache_size` value in the
        `default_vela.ini` configuration file.

        :param  arena_cache_size:   The new arena cache size value.
        :return:                    A new NPU configuration with the new
                                    arena cache size value.
        """
        value = arena_cache_size

        if value == 0:
            value = MPS3_MAX_SRAM_SZ if self.memory_mode == "Shared_Sram" else None

        return NpuConfig(
            **{**self.__dict__, **{"arena_cache_size": value}}
        )


@dataclass(frozen=True)
class NpuConfigs:
    """
    Represents a collection of NPU configurations.
    """
    configs: typing.Dict[str, typing.Dict[int, NpuConfig]]

    @staticmethod
    def create(*configs: NpuConfig):
        """
        Create a new collection with the specified NPU configurations.

        :param configs: NPU configuration objects to add to the collection.
        :return:        A new collection of NPU configurations.
        """
        _configs = {}

        # Internal data structure of nested dictionaries based on
        # NPU name and MAC configuration, e.g.:
        #   _configs["ethos-u55"][128]

        for c in configs:
            if c.name_prefix not in _configs:
                _configs[c.name_prefix] = {}
            _configs[c.name_prefix][c.macs] = c
        return NpuConfigs(configs=_configs)

    def get(self, name_prefix: str, macs: typing.Union[int, str]) -> typing.Optional[NpuConfig]:
        """
        Get an NPU configuration by name prefix and MAC configuration.

        :param name_prefix: The name prefix, e.g. "ethos-u55".
        :param macs:        The MAC configuration, e.g. 128.
        :return:            The matching NPU configuration, or None if no such configuration
                            exists in the collection.
        """
        configs_for_name = self.configs.get(name_prefix)
        if not configs_for_name:
            return None
        return configs_for_name.get(int(macs))

    def get_by_name(self, name: str) -> typing.Optional[NpuConfig]:
        """
        Get an NPU configuration by name.

        :param name:    The NPU configuration name, e.g. "ethos-u55-128".
        :return:        The matching NPU configuration, or None if no such configuration
                        exists in the collection.
        """
        name_prefix, macs = name.rsplit("-", 1)
        return self.get(name_prefix, macs)

    @property
    def names(self):
        """
        Return a list of all NPU configuration names in the collection.

        :return:    The list of NPU configuration names.
        """
        return list(itertools.chain.from_iterable([
            [f"{c.name_prefix}-{c.macs}" for c in config.values()]
            for config in self.configs.values()
        ]))
