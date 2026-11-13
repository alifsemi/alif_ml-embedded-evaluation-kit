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
"""Download and operational configuration."""
import typing
from dataclasses import dataclass, field

from mlek_tools.setup.util import HttpHeadersType


@dataclass(frozen=True)
class DownloadConfig:
    """Controls which use cases are downloaded and how downloads are performed."""
    use_case_names: typing.List[str] = field(default_factory=list)
    parallel: int = 1
    http_headers: HttpHeadersType = field(default_factory=dict)
    check_clean_folder: bool = False
