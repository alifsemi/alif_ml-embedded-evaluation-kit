#  SPDX-FileCopyrightText:  Copyright 2025-2026 Arm Limited and/or its
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
"""NPU hardware descriptor dataclasses."""
import enum
import itertools
import typing
from dataclasses import dataclass
from types import MappingProxyType


class MemoryMode(str, enum.Enum):
    """Supported Vela memory modes."""

    SRAM_ONLY = "Sram_Only"
    SHARED_SRAM = "Shared_Sram"
    DEDICATED_SRAM = "Dedicated_Sram"

    def __str__(self) -> str:
        """Return the Vela memory-mode string.

        :return:    Memory mode passed to Vela.
        """
        return self.value

    @staticmethod
    def from_name(name: str) -> typing.Optional["MemoryMode"]:
        """Return the memory mode matching *name*.

        :param name: Memory mode name such as ``"Shared_Sram"``.
        :return:     Matching memory mode, or ``None``.
        """
        modes_by_name = {
            memory_mode.value: memory_mode
            for memory_mode in MemoryMode
        }
        return modes_by_name.get(name)


@dataclass(frozen=True)
class NpuMetadata:
    """Static metadata for one Ethos-U NPU family."""

    name_prefix: str
    processor_id: str
    config_prefix_id: str
    memory_modes: typing.Tuple[MemoryMode, ...]
    default_memory_mode: MemoryMode
    system_configs_by_macs: typing.Mapping[int, str]


class Npu(str, enum.Enum):
    """Supported Ethos-U NPU families."""

    ETHOS_U55 = NpuMetadata(
        name_prefix="ethos-u55",
        processor_id="U55",
        config_prefix_id="H",
        memory_modes=(MemoryMode.SRAM_ONLY, MemoryMode.SHARED_SRAM),
        default_memory_mode=MemoryMode.SHARED_SRAM,
        system_configs_by_macs={
            32: "RTSS_HE_SRAM_MRAM",
            64: "RTSS_HE_SRAM_MRAM",
            128: "RTSS_HE_SRAM_MRAM",
            256: "RTSS_HP_SRAM_MRAM",
        },
    )
    ETHOS_U65 = NpuMetadata(
        name_prefix="ethos-u65",
        processor_id="U65",
        config_prefix_id="Y",
        memory_modes=(
            MemoryMode.SRAM_ONLY,
            MemoryMode.SHARED_SRAM,
            MemoryMode.DEDICATED_SRAM,
        ),
        default_memory_mode=MemoryMode.DEDICATED_SRAM,
        system_configs_by_macs={
            256: "Ethos_U65_High_End",
            512: "Ethos_U65_High_End",
        },
    )
    ETHOS_U85 = NpuMetadata(
        name_prefix="ethos-u85",
        processor_id="U85",
        config_prefix_id="Z",
        memory_modes=(
            MemoryMode.SRAM_ONLY,
            MemoryMode.SHARED_SRAM,
            MemoryMode.DEDICATED_SRAM,
        ),
        default_memory_mode=MemoryMode.SHARED_SRAM,
        system_configs_by_macs={
            128: "Ethos_U85_SRAM_MRAM",
            256: "Ethos_U85_SRAM_MRAM",
            512: "Ethos_U85_SRAM_MRAM",
            1024: "Ethos_U85_SRAM_MRAM",
            2048: "Ethos_U85_SRAM_MRAM",
        },
    )

    processor_id: str
    config_prefix_id: str
    memory_modes: typing.Tuple[MemoryMode, ...]
    default_memory_mode: MemoryMode
    system_configs_by_macs: typing.Mapping[int, str]

    def __new__(cls, metadata: NpuMetadata) -> "Npu":
        """Create an NPU enum member with associated metadata.

        :param metadata: Static NPU family metadata.
        :return:         NPU enum member.
        """
        npu = str.__new__(cls, metadata.name_prefix)
        npu._value_ = metadata.name_prefix
        npu.processor_id = metadata.processor_id
        npu.config_prefix_id = metadata.config_prefix_id
        npu.memory_modes = metadata.memory_modes
        npu.default_memory_mode = metadata.default_memory_mode
        npu.system_configs_by_macs = MappingProxyType(dict(
            metadata.system_configs_by_macs
        ))
        return npu

    def __str__(self) -> str:
        """Return the Vela accelerator name prefix.

        :return:    Name prefix used in Vela accelerator configs.
        """
        return self.value

    @staticmethod
    def from_name_prefix(name_prefix: str) -> typing.Optional["Npu"]:
        """Return the NPU matching *name_prefix*.

        :param name_prefix: Name prefix such as ``"ethos-u55"``.
        :return:            Matching NPU, or ``None``.
        """
        npus_by_name = {
            npu.value: npu
            for npu in Npu
        }
        return npus_by_name.get(name_prefix)

    @property
    def supported_macs(self) -> typing.Tuple[int, ...]:
        """Return supported MAC counts.

        :return:    Supported MAC counts.
        """
        return tuple(self.system_configs_by_macs)

    def get_system_config(self, macs: int) -> str:
        """Return the Vela system config for *macs*.

        :param macs:        MAC count.
        :return:            Vela system config.
        :raises ValueError: If *macs* is not supported by this NPU.
        """
        if macs not in self.system_configs_by_macs:
            raise ValueError(
                f"Unsupported MAC configuration '{macs}' for {self}. "
                f"Select one from {self.supported_macs}."
            )
        return self.system_configs_by_macs[macs]


@dataclass(frozen=True)
class NpuConfig:
    """Hardware descriptor for a concrete Ethos-U NPU configuration."""

    npu: Npu
    macs: int
    memory_mode: MemoryMode

    def __post_init__(self):
        """Validate that this config is supported by its NPU."""
        if self.macs not in self.npu.supported_macs:
            raise ValueError(
                f"Unsupported MAC configuration '{self.macs}' for {self.npu}. "
                f"Select one from {self.npu.supported_macs}."
            )
        if self.memory_mode not in self.npu.memory_modes:
            raise ValueError(
                f"Unsupported memory mode '{self.memory_mode}' for {self.npu}. "
                f"Select one from {self.npu.memory_modes}."
            )

    @classmethod
    def from_name(cls: typing.Type["NpuConfig"], name: str) -> "NpuConfig":
        """Parse an unqualified or memory-qualified config name.

        :param name:        Config name such as ``"ethos-u55-128"`` or
                            ``"ethos-u55-128:Shared_Sram"``.
        :return:            Concrete NPU config.
        :raises ValueError: If *name* is not recognised.
        """
        invalid_name_message = (
            f"Invalid Ethos-U NPU configuration '{name}'. "
            f"Select one from {supported_npu_config_names(memory_mode=True)}; "
            f"unqualified defaults are {supported_npu_config_names()}."
        )

        # Split off an optional memory-mode qualifier.  Unqualified names use
        # the selected NPU's default memory mode after the NPU has been parsed.
        config_name = name
        memory_mode = None
        if ":" in config_name:
            config_name, memory_mode_name = config_name.rsplit(":", 1)
            memory_mode = MemoryMode.from_name(memory_mode_name)
            if memory_mode is None:
                raise ValueError(invalid_name_message)

        # Parse the accelerator family and MAC count from the remaining name.
        name_parts = config_name.rsplit("-", 1)
        if len(name_parts) != 2:
            raise ValueError(invalid_name_message)

        name_prefix, macs = name_parts
        npu = Npu.from_name_prefix(name_prefix)
        if npu is None or not macs.isdigit():
            raise ValueError(invalid_name_message)

        # Resolve an omitted memory mode to the documented family default.
        selected_memory_mode = memory_mode
        if selected_memory_mode is None:
            selected_memory_mode = npu.default_memory_mode

        # Reject unsupported combinations before constructing the frozen value.
        macs_value = int(macs)
        if macs_value not in npu.supported_macs:
            raise ValueError(invalid_name_message)
        if selected_memory_mode not in npu.memory_modes:
            raise ValueError(invalid_name_message)

        return cls(
            npu=npu,
            macs=macs_value,
            memory_mode=selected_memory_mode,
        )

    @property
    def name_prefix(self) -> str:
        """Return the Vela accelerator name prefix.

        :return:    Name prefix, for example ``"ethos-u55"``.
        """
        return self.npu.value

    @property
    def processor_id(self) -> str:
        """Return the NPU processor ID.

        :return:    Processor ID, for example ``"U55"``.
        """
        return self.npu.processor_id

    @property
    def prefix_id(self) -> str:
        """Return the short config prefix ID.

        :return:    Prefix ID, for example ``"H"``.
        """
        return self.npu.config_prefix_id

    @property
    def system_config(self) -> str:
        """Return the Vela system config for this concrete config.

        :return:    Vela system config.
        """
        return self.npu.get_system_config(self.macs)

    @property
    def config_name(self) -> str:
        """Return the Vela accelerator-config string.

        :return:    Config name, for example ``"ethos-u55-128"``.
        """
        return f"{self.npu}-{self.macs}"

    @property
    def config_id(self) -> str:
        """Return the short config ID used in output file names.

        :return:    Short config ID, for example ``"H128"``.
        """
        return f"{self.prefix_id}{self.macs}"

    @property
    def qualified_name(self) -> str:
        """Return the memory-qualified config name.

        :return:    Qualified name, for example ``"ethos-u55-128:Shared_Sram"``.
        """
        return f"{self.config_name}:{self.memory_mode}"

    @property
    def filename_suffix(self) -> str:
        """Return the stable file-name suffix for this concrete config.

        :return:    File-name suffix, for example ``"H128_Shared_Sram"``.
        """
        return f"{self.config_id}_{self.memory_mode}"

    @property
    def vela_output_suffix(self) -> str:
        """Return the TFLite Vela output suffix for this concrete config.

        :return:    Vela output suffix, for example ``"_vela_H128_Shared_Sram"``.
        """
        return f"_vela_{self.filename_suffix}"

    @property
    def delegate_target_suffix(self) -> str:
        """Return the ExecuTorch delegated output suffix for this config.

        :return:    Delegate suffix, for example
                    ``"arm_delegate_ethos-u55-128_Shared_Sram"``.
        """
        return f"arm_delegate_{self.config_name}_{self.memory_mode}"

    def to_metadata_dict(self) -> typing.Dict[str, typing.Union[int, str]]:
        """Return a stable plain-string metadata representation.

        :return:    Dictionary matching the optimization metadata schema.
        """
        return {
            "name_prefix": self.name_prefix,
            "macs": self.macs,
            "processor_id": self.processor_id,
            "prefix_id": self.prefix_id,
            "memory_mode": self.memory_mode.value,
            "system_config": self.system_config,
        }


def iter_npu_configs(
        npus: typing.Iterable[Npu] = tuple(Npu),
) -> typing.Iterator[NpuConfig]:
    """Yield every concrete config supported by *npus*.

    :param npus: NPU descriptors.
    :return:     Iterator over concrete configs in NPU, MAC, memory-mode order.
    """
    for npu in npus:
        for macs, memory_mode in itertools.product(
                npu.supported_macs,
                npu.memory_modes,
        ):
            yield NpuConfig(
                npu=npu,
                macs=macs,
                memory_mode=memory_mode,
            )


def supported_npu_config_names(memory_mode: bool = False) -> typing.List[str]:
    """Return supported accelerator config names.

    :param memory_mode: Include memory-mode qualifiers in returned names.
    :return:            Supported config names.
    """
    if memory_mode:
        return [
            config.qualified_name
            for config in iter_npu_configs()
        ]

    names = []
    for config in iter_npu_configs():
        if config.config_name not in names:
            names.append(config.config_name)
    return names
