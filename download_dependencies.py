#!/usr/bin/env python3

#  Copyright (c) 2021-2022 Arm Limited. All rights reserved.
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

"""This script does effectively the same as "git submodule update --init" command."""
import logging
import sys
import tarfile
import tempfile
from urllib.request import urlopen
from zipfile import ZipFile
from pathlib import Path

TF = "https://github.com/tensorflow/tflite-micro/archive/02715237c1fc0a23f465226364d206277f54ebce.zip"
CMSIS = "https://github.com/ARM-software/CMSIS_5/archive/29615088b12e3ba8ce50d316cf7f38c1bd7fc620.zip"
ETHOS_U_CORE_DRIVER = "https://git.mlplatform.org/ml/ethos-u/ethos-u-core-driver.git/snapshot/ethos-u-core-driver-22.05.tar.gz"
ETHOS_U_CORE_PLATFORM = "https://git.mlplatform.org/ml/ethos-u/ethos-u-core-platform.git/snapshot/ethos-u-core-platform-22.05.tar.gz"


def download(url_file: str, post_process=None):
    with urlopen(url_file) as response, tempfile.NamedTemporaryFile() as temp:
        logging.info(f"Downloading {url_file} ...")
        temp.write(response.read())
        temp.seek(0)
        logging.info(f"Finished downloading {url_file}.")
        if post_process:
            post_process(temp)


def unzip(file, to_path):
    with ZipFile(file) as z:
        for archive_path in z.infolist():
            archive_path.filename = archive_path.filename[archive_path.filename.find("/") + 1:]
            if archive_path.filename:
                z.extract(archive_path, to_path)
                target_path = to_path / archive_path.filename
                attr = archive_path.external_attr >> 16
                if attr != 0:
                    target_path.chmod(attr)


def untar(file, to_path):
    with tarfile.open(file) as z:
        for archive_path in z.getmembers():
            index = archive_path.name.find("/")
            if index < 0:
                continue
            archive_path.name = archive_path.name[index + 1:]
            if archive_path.name:
                z.extract(archive_path, to_path)


def main(dependencies_path: Path):

    download(CMSIS,
             lambda file: unzip(file.name, to_path=dependencies_path / "cmsis"))
    download(ETHOS_U_CORE_DRIVER,
             lambda file: untar(file.name, to_path=dependencies_path / "core-driver"))
    download(ETHOS_U_CORE_PLATFORM,
             lambda file: untar(file.name, to_path=dependencies_path / "core-platform"))
    download(TF,
             lambda file: unzip(file.name, to_path=dependencies_path / "tensorflow"))


if __name__ == '__main__':
    logging.basicConfig(filename='download_dependencies.log', level=logging.DEBUG, filemode='w')
    logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

    download_dir = Path(__file__).parent.resolve() / "dependencies"

    if download_dir.is_dir():
        logging.info(f'{download_dir} exists. Skipping download.')
    else:
        main(download_dir)
