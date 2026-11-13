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
"""Use case domain object definitions."""
import itertools
import json
import re
import typing
from dataclasses import dataclass, field
from enum import IntEnum
from pathlib import Path

model_file_extensions = re.compile(r'^.*\.pt2?$')


class ExecuTorchResourceType(IntEnum):
    """Denotes the type of ExecuTorch resource."""
    BUILT_IN = 0
    LOCAL_PROJECT = 1
    CHECKPOINT_DOWNLOAD = 2


@dataclass(frozen=True)
class UseCaseResource:
    """A single downloadable resource belonging to a use case."""
    name: str
    url: str
    sub_folder: typing.Optional[str] = None
    sha256: typing.Optional[str] = None

    def __post_init__(self):
        """Validate and normalize the optional SHA-256 digest."""
        if self.sha256 is None:
            return

        normalized_sha256 = self.sha256.lower()
        if not re.fullmatch(r"[0-9a-f]{64}", normalized_sha256):
            raise ValueError(
                f"SHA-256 for resource {self.name} must be 64 hexadecimal characters"
            )
        object.__setattr__(self, "sha256", normalized_sha256)


@dataclass(frozen=True)
class ExecuTorchResource:
    """An ExecuTorch model resource associated with a use case."""
    type: ExecuTorchResourceType = field(init=False)
    resources_dir: Path
    model: str
    path: typing.Optional[Path] = None
    requirements: typing.Optional[str] = None
    lowering: typing.Optional[Path] = None
    excluded_npu_processor_ids: typing.Tuple[str, ...] = field(default_factory=tuple)

    def __post_init__(self):
        is_model_file = model_file_extensions.match(self.model)
        if self.path and not is_model_file:
            object.__setattr__(self, "type", ExecuTorchResourceType.LOCAL_PROJECT)
            if self.resources_dir and not self.resources_dir.exists():
                raise ValueError(f"Resources directory {self.resources_dir} does not exist")
            if self.project_path and not self.project_path.exists():
                raise ValueError(f"Project path {self.project_path} does not exist")
            if self.model_path and not self.model_path.is_file():
                raise ValueError(f"Model file {self.model_path} does not exist")
            if self.requirements_path and not self.requirements_path.is_file():
                raise ValueError(f"Requirements file {self.requirements_path} does not exist")
            if self.lowering_script and not self.lowering_script.is_file():
                raise ValueError(f"Lowering script {self.lowering_script} does not exist")
        elif is_model_file:
            object.__setattr__(self, "type", ExecuTorchResourceType.CHECKPOINT_DOWNLOAD)
        else:
            object.__setattr__(self, "type", ExecuTorchResourceType.BUILT_IN)

    @property
    def project_path(self) -> typing.Optional[Path]:
        """Full path to the local project directory, or ``None`` for built-in models."""
        return self.resources_dir / self.path if self.path else None

    @property
    def model_path(self) -> typing.Optional[Path]:
        """Full path to the local model file, or ``None`` for built-in models."""
        if self.is_local_project():
            return self.project_path / self.model if self.model else None
        return None

    @property
    def requirements_path(self) -> typing.Optional[Path]:
        """Full path to the requirements file, or ``None`` if not specified."""
        if not self.path:
            return None
        return self.project_path / self.requirements if self.requirements else None

    @property
    def lowering_script(self) -> typing.Optional[Path]:
        """Full path to a custom lowering script, or ``None`` if not specified."""
        if not self.path:
            return None
        return self.project_path / self.lowering if self.lowering else None

    @property
    def model_name(self) -> typing.Union[str, Path]:
        """Model identifier passed to the lowering script."""
        return self.model_path \
            if self.type is ExecuTorchResourceType.LOCAL_PROJECT \
            else self.model

    def is_local_project(self) -> bool:
        """Return ``True`` if this resource is a local project."""
        return self.type == ExecuTorchResourceType.LOCAL_PROJECT

    def is_built_in(self) -> bool:
        """Return ``True`` if this resource uses a built-in model."""
        return self.type == ExecuTorchResourceType.BUILT_IN

    def is_checkpoint_download(self) -> bool:
        """Return ``True`` if this resource is a downloaded checkpoint."""
        return self.type == ExecuTorchResourceType.CHECKPOINT_DOWNLOAD

    def has_requirements(self) -> bool:
        """Return ``True`` if this resource has pip requirements to install."""
        return self.requirements is not None


@dataclass(frozen=True)
class UseCase:
    """A named use case grouping downloadable and ExecuTorch resources."""
    name: str
    url_prefix: typing.List[str]
    resources: typing.List[UseCaseResource]
    executorch_resources: typing.List[ExecuTorchResource] = field(default_factory=list)


def _load_use_case_resources_file(
        file_path: Path
) -> typing.List[typing.Dict[str, typing.Any]]:
    with open(file_path, encoding="utf-8") as f:
        return json.load(f)


def _to_use_case(
        use_case_data: typing.Dict[str, typing.Any],
        resources_dir: Path,
) -> UseCase:
    use_case_resources = [
        UseCaseResource(**resource)
        for resource in use_case_data.get("resources", [])
    ]
    executorch_resources = [
        ExecuTorchResource(
            resources_dir,
            **{
                **resource,
                "excluded_npu_processor_ids": tuple(
                    resource.get("excluded_npu_processor_ids", [])
                ),
            }
        )
        for resource in use_case_data.get("executorch_resources", [])
    ]
    return UseCase(
        name=use_case_data["name"],
        url_prefix=use_case_data.get("url_prefix", []),
        resources=use_case_resources,
        executorch_resources=executorch_resources,
    )


def load_use_case_resources(
        use_case_resources_files: typing.List[Path],
        use_case_names: typing.List[str] = (),
) -> typing.List[UseCase]:
    """Load use case metadata from one or more JSON resource files.

    :param use_case_resources_files:    Paths to JSON files.
    :param use_case_names:              If non-empty, only include these use cases.
    :return:                            List of :class:`UseCase` objects.
    """
    use_cases = itertools.chain(*(
        [
            _to_use_case(use_case_data, file.parent)
            for use_case_data in _load_use_case_resources_file(file)
        ]
        for file in use_case_resources_files
    ))

    if len(use_case_names) == 0:
        return list(use_cases)
    return [uc for uc in use_cases if uc.name in use_case_names]
