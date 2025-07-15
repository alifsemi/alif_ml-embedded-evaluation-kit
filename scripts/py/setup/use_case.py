#!/usr/bin/env python3
#  SPDX-FileCopyrightText:  Copyright 2025 Arm Limited and/or its
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
import typing
from dataclasses import dataclass, field
from pathlib import Path


@dataclass(frozen=True)
class UseCaseResource:
    """
    Represent a use case's resource
    """
    name: str
    url: str
    sub_folder: typing.Optional[str] = None


@dataclass(frozen=True)
class UseCase:
    """
    Represent a use case
    """
    name: str
    url_prefix: str
    resources: typing.List[UseCaseResource]
    executorch_models: typing.Optional[typing.List[str]] = field(default_factory=lambda: [])


def load_use_case_resources_file(file_path: Path) -> typing.List[typing.Dict[str, typing.Any]]:
    """
    Load a use case resources file from the specified Path
    :param file_path:   Path to the use case resources file
    :return:            The file contents parsed to a Dictionary
    """
    with open(file_path, encoding="utf-8") as f:
        return json.load(f)


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
    use_case_resources = list(
        itertools.chain(*(
            load_use_case_resources_file(file) for file in use_case_resources_files
        ))
    )

    use_cases = (
        UseCase(
            name=u["name"],
            url_prefix=u["url_prefix"],
            resources=[UseCaseResource(**r) for r in u["resources"]],
            executorch_models=u.get("executorch_models", []),
        )
        for u in use_case_resources
    )

    if len(use_case_names) == 0:
        return list(use_cases)

    return [uc for uc in use_cases if uc.name in use_case_names]
