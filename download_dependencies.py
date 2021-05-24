#!/usr/bin/env python3

#  Copyright (c) 2021 Arm Limited. All rights reserved.
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

# The script does effectively the same as "git submodule update --init" command.

import logging
import os
import sys
import tarfile
import tempfile
from urllib.request import urlopen
from zipfile import ZipFile

logging.basicConfig(filename='download_dependencies.log', level=logging.DEBUG)
logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))

tf = "https://github.com/tensorflow/tensorflow/archive/6cff09aee1f832d495b3cae40cab0de58155a0af.zip"
cmsis = "https://github.com/ARM-software/CMSIS_5/archive/0d7e4fa7131241a17e23dfae18140e0b2e77728f.zip"
ethos_u_core_sw = "https://git.mlplatform.org/ml/ethos-u/ethos-u-core-software.git/snapshot/ethos-u-core-software-7f3c1c92732b611a53968b14e70a2b116e43b980.tar.gz"
ethos_u_core_driver = "https://git.mlplatform.org/ml/ethos-u/ethos-u-core-driver.git/snapshot/ethos-u-core-driver-effc7aa8b9272fb20cdd1a7d1097818af70acc93.tar.gz"


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
                target_path = os.path.join(to_path, archive_path.filename)
                attr = archive_path.external_attr >> 16
                if attr != 0:
                    os.chmod(target_path, attr)


def untar(file, to_path):
    with tarfile.open(file) as z:
        for archive_path in z.getmembers():
            index = archive_path.name.find("/")
            if index < 0:
                continue
            archive_path.name = archive_path.name[index + 1:]
            if archive_path.name:
                z.extract(archive_path, to_path)


def main(dependencies_path: str):

    download(cmsis,
             lambda file: unzip(file.name,
                                to_path=os.path.join(dependencies_path, "cmsis")))
    download(ethos_u_core_sw,
             lambda file: untar(file.name,
                                to_path=os.path.join(dependencies_path, "core-software")))
    download(ethos_u_core_driver,
             lambda file: untar(file.name,
                                to_path=os.path.join(dependencies_path, "core-driver")))
    download(tf,
             lambda file: unzip(file.name,
                                to_path=os.path.join(dependencies_path, "tensorflow")))


if __name__ == '__main__':
    download_dir = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "dependencies"))

    if os.path.isdir(download_dir):
        logging.info(f'{download_dir} exists. Skipping download.')
    else:
        main(download_dir)
