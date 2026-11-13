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
"""Resource download flow: URL resolution, directory init, and download execution."""
import concurrent.futures
import errno
import json
import logging
import os
import re
import typing
from dataclasses import dataclass
from pathlib import Path

from mlek_tools.setup.util import (
    HttpHeadersType,
    download_file,
    remove_tree_dir,
    verify_file_sha256,
)
from mlek_tools.use_case.model import UseCase, UseCaseResource


@dataclass(frozen=True)
class ResourceDownload:
    """A pending resource download with its destination and expected digest."""
    url: str
    dest: Path
    expected_sha256: typing.Optional[str]


def get_downloaded_resources_directory(use_case: UseCase, downloads_dir: Path) -> Path:
    """Return the download directory for a specific use case."""
    return downloads_dir / use_case.name


def get_downloaded_resource_path(
        use_case: UseCase,
        resource: UseCaseResource,
        downloads_dir: Path,
) -> Path:
    """Return the final local path for a downloaded resource.

    :param use_case:       Use case that owns the resource.
    :param resource:       Resource entry from the use case manifest.
    :param downloads_dir:  Root downloads directory.
    :return:               Final resource file path.
    """
    dest_dir = get_downloaded_resources_directory(use_case, downloads_dir)
    if resource.sub_folder is not None:
        dest_dir = dest_dir / resource.sub_folder
    return dest_dir / resource.name


def initialize_use_case_resources_directory(
        use_case: UseCase,
        metadata: typing.Dict,
        downloads_dir: Path,
        check_clean_folder: bool,
        setup_script_hash_verified: bool,
) -> None:
    """Create (or conditionally clean) the per-use-case download directory."""
    use_case_resources_dir = get_downloaded_resources_directory(use_case, downloads_dir)
    try:
        use_case_resources_dir.mkdir()
    except OSError as err:
        if err.errno == errno.EEXIST:
            if check_clean_folder and not setup_script_hash_verified:
                use_case_info = next(
                    (
                        info for info in metadata.get("resources_info", [])
                        if isinstance(info, dict) and info.get("name") == use_case.name
                    ),
                    {},
                )
                url_prefixes = use_case_info.get("url_prefix", [])
                for i, url_prefix in enumerate(url_prefixes):
                    if i >= len(use_case.url_prefix) or url_prefix != use_case.url_prefix[i]:
                        logging.info("Removing %s resources.", use_case.name)
                        remove_tree_dir(use_case_resources_dir)
                        break
        else:
            logging.error("Error creating %s directory.", use_case.name)
            raise


def get_resources_to_download(
        use_case: UseCase,
        downloads_dir: Path,
) -> typing.List[ResourceDownload]:
    """Build the list of resources to download and verify already-present files."""
    reg_expr_str = r"{url_prefix:(.*\d)}"
    reg_expr_pattern = re.compile(reg_expr_str)
    to_download = []
    for resource in use_case.resources:
        url_prefix_idx = int(reg_expr_pattern.search(resource.url).group(1))
        url = use_case.url_prefix[url_prefix_idx] + re.sub(reg_expr_str, "", resource.url)

        dest = get_downloaded_resource_path(use_case, resource, downloads_dir)
        dest_dir = dest.parent
        os.makedirs(dest_dir, exist_ok=True)

        if resource.sha256 is None:
            logging.warning(
                "No SHA-256 digest configured for %s resource %s.",
                use_case.name,
                dest,
            )

        if dest.is_file():
            logging.info("File %s exists, skipping download.", dest)
            verify_resource_sha256(resource, dest)
        else:
            to_download.append(
                ResourceDownload(
                    url=url,
                    dest=dest,
                    expected_sha256=resource.sha256,
                )
            )
    return to_download


def verify_resource_sha256(
        resource: UseCaseResource,
        resource_path: Path,
):
    """Verify one resource SHA-256 digest when the manifest provides it.

    :param resource:       Resource entry from the use case manifest.
    :param resource_path:  Local resource path to verify.
    :raises RuntimeError:  If a referenced file is missing or has an unexpected digest.
    """
    if not resource_path.is_file():
        raise RuntimeError(f"Downloaded resource is missing: {resource_path}")

    if resource.sha256 is None:
        return

    verify_file_sha256(resource_path, resource.sha256)


def initialize_resources_directory(
        downloads_dir: Path,
        check_clean_folder: bool,
        metadata_file_path: Path,
        setup_script_hash: str,
        vela_version: str,
) -> typing.Tuple[typing.Dict, bool]:
    """Set up the top-level downloads directory and check whether it is still valid.

    :param downloads_dir:       Path to the downloads directory.
    :param check_clean_folder:  When True, checks metadata and may wipe stale content.
    :param metadata_file_path:  Path to the JSON metadata file.
    :param setup_script_hash:   SHA-256 of the calling setup script.
    :param vela_version:        Expected Vela version string.
    :return:                    ``(metadata_dict, setup_script_hash_verified)``
    """
    metadata_dict: typing.Dict = {}
    setup_script_hash_verified = False

    if downloads_dir.is_dir():
        logging.info("'resources_downloaded' directory exists.")
        if check_clean_folder and metadata_file_path.is_file():
            with open(metadata_file_path, encoding="utf8") as metadata_file:
                metadata_dict = json.load(metadata_file)

            vela_in_metadata = metadata_dict.get("ethosu_vela_version", "")
            if vela_in_metadata != vela_version:
                logging.info(
                    "Vela version in metadata is %s, current %s."
                    " Removing the resources and re-downloading them.",
                    vela_in_metadata,
                    vela_version,
                )
                remove_tree_dir(downloads_dir)
                metadata_dict = {}
            else:
                setup_script_hash_verified = (
                    metadata_dict.get("set_up_script_sha256sum") == setup_script_hash
                )
    else:
        downloads_dir.mkdir()

    return metadata_dict, setup_script_hash_verified


def download_resources(
        to_download: typing.List[ResourceDownload],
        http_headers: HttpHeadersType,
        parallel: int = 1,
) -> None:
    """Download all pending resources, in parallel or serially.

    :param to_download:     Resources pending download.
    :param http_headers:    Per-domain HTTP headers for authentication.
    :param parallel:        Number of worker threads. ``1`` means serial.
    """
    if parallel > 1:
        with concurrent.futures.ThreadPoolExecutor(
                max_workers=parallel,
                thread_name_prefix="download",
        ) as executor:
            futures = [
                executor.submit(
                    download_file,
                    download.url,
                    download.dest,
                    http_headers,
                    download.expected_sha256,
                )
                for download in to_download
            ]
            for future in concurrent.futures.as_completed(futures):
                future.result()
    else:
        for download in to_download:
            download_file(
                download.url,
                download.dest,
                http_headers,
                download.expected_sha256,
            )
