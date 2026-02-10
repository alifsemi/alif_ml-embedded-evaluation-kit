#!/usr/bin/env python3
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
"""
Use case domain object definitions
"""
import itertools
import json
import re
import typing
from dataclasses import dataclass, field
from enum import IntEnum
from pathlib import Path

model_file_extensions = re.compile(r'^.*\.pt2?$')


class ExecutorchResourceType(IntEnum):
    """
    Denotes the type of ExecuTorch resource
    Built-in models (e.g. "mv2") require no additional setup
    Local projects may have requirements that need installing
    """
    BUILT_IN = 0
    LOCAL_PROJECT = 1
    CHECKPOINT_DOWNLOAD = 2


@dataclass(frozen=True)
class UseCaseResource:
    """
    Represent a use case's resource
    """
    name: str
    url: str
    sub_folder: typing.Optional[str] = None


@dataclass(frozen=True)
class ExecutorchResource:
    """
    Represent a use case's ExecuTorch project
    """
    type: ExecutorchResourceType = field(init=False)
    resources_dir: Path
    model: str
    path: typing.Optional[Path] = None
    requirements: typing.Optional[str] = None
    lowering: typing.Optional[Path] = None

    def __post_init__(self):
        is_model_file = model_file_extensions.match(self.model)
        if self.path and not is_model_file:
            object.__setattr__(self, "type", ExecutorchResourceType.LOCAL_PROJECT)
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
            object.__setattr__(self, "type", ExecutorchResourceType.CHECKPOINT_DOWNLOAD)
        else:
            object.__setattr__(self, "type", ExecutorchResourceType.BUILT_IN)

    @property
    def project_path(self) -> typing.Optional[Path]:
        """
        Get the full path to the local project directory
        :return:    The full path to the local project directory,
                    or None for built-in models
        """
        return self.resources_dir / self.path if self.path else None

    @property
    def model_path(self) -> typing.Optional[Path]:
        """
        Get the full path to the local model.
        :return:    The full path to the local model,
                    or None for built-in models
        """
        if self.is_local_project():
            return self.project_path / self.model if self.model else None

        return None

    @property
    def requirements_path(self) -> typing.Optional[Path]:
        """
        Get the full path to the requirements file if specified
        :return:    The full path to the requirements file
                    if it has been specified
        """
        if not self.path:
            return None

        return self.project_path / self.requirements if self.requirements else None

    @property
    def lowering_script(self) -> typing.Optional[Path]:
        """
        Get the full path to the lowering script if specified
        :return:    The full path to the lowering script
                    if it has been specified
        """
        if not self.path:
            return None

        return self.project_path / self.lowering if self.lowering else None

    @property
    def model_name(self) -> str:
        """
        Get the name of the model to be provided to the aot_arm_compiler.py script
        This will either be a name for built-in models or a full path to a local model file
        :return:    The model name
        """
        return self.model_path \
            if self.type is ExecutorchResourceType.LOCAL_PROJECT \
            else self.model

    def is_local_project(self) -> bool:
        """
        Convenience function to denote if this resource is a local project
        :return:    True if this resource is a local project
        """
        return self.type == ExecutorchResourceType.LOCAL_PROJECT

    def is_built_in(self) -> bool:
        """
        Convenience function to denote if this resource is a built-in-model
        :return:    True if this resource is a built-in-model
        """
        return self.type == ExecutorchResourceType.BUILT_IN

    def is_checkpoint_download(self) -> bool:
        """
        Convenience function to denote if this resource is a downloaded checkpoint model
        :return:    True if this resource is a checkpoint
        """
        return self.type == ExecutorchResourceType.CHECKPOINT_DOWNLOAD

    def has_requirements(self) -> bool:
        """
        Convenience function to denote if this resource has requirements that need to be installed
        :return:    True if this resource has requirements that need to be installed
        """
        return self.requirements is not None


@dataclass(frozen=True)
class UseCase:
    """
    Represent a use case
    """
    name: str
    url_prefix: typing.List[str]
    resources: typing.List[UseCaseResource]
    executorch_resources: typing.List[ExecutorchResource] = field(default_factory=lambda: [])


def load_use_case_resources_file(file_path: Path) -> typing.List[typing.Dict[str, typing.Any]]:
    """
    Load a use case resources file from the specified Path
    :param file_path:   Path to the use case resources file
    :return:            The file contents parsed to a Dictionary
    """
    with open(file_path, encoding="utf-8") as f:
        return json.load(f)


def to_use_case(
        use_case_data: typing.Dict[str, typing.Any],
        resources_dir: Path
) -> UseCase:
    """
    Create a UseCase
    :param use_case_data:   Dictionary of values read from use_case_resources.json
    :param resources_dir:   Resource directory path
    :return:                UseCase object
    """
    use_case_resources = [
        UseCaseResource(**resource)
        for resource in use_case_data.get("resources", [])
    ]

    executorch_resources = [
        ExecutorchResource(resources_dir, **resource)
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
        use_case_names: typing.List[str] = ()
) -> typing.List[UseCase]:
    """
    Load use case metadata resources

    Parameters
    ----------
    use_case_resources_files :  Paths to JSON files containing the use case
                                metadata resources.
    use_case_names          :   List of named use cases to restrict
                                resource loading to.
    Returns
    -------
    The use cases resources object parsed to a dict
    """
    use_cases = itertools.chain(*(
        [
            to_use_case(use_case_data, file.parent)
            for use_case_data in load_use_case_resources_file(file)
        ]
        for file in use_case_resources_files
    ))

    if len(use_case_names) == 0:
        return list(use_cases)

    return [uc for uc in use_cases if uc.name in use_case_names]
