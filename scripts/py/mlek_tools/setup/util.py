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
Utility functions for setup
"""
import hashlib
import logging
import os
import shutil
import subprocess
import typing
import urllib
import urllib.parse
import urllib.request
from netrc import netrc
from pathlib import Path
from urllib.error import URLError

default_netrc_path = Path.home() / ".netrc"
HttpHeadersType = typing.Dict[str, typing.List[typing.Tuple[str, str]]]


def create_basic_auth_handler(
        domain: str,
        username: str,
        password: str
) -> urllib.request.BaseHandler:
    """
    Create an HTTPBasicAuthHandler for the specified domain and credentials
    :param domain:      The top-level domain for which to provide credentials
    :param username:    The username
    :param password:    The password
    :return:            The HTTPBasicAuthHandler
    """
    password_mgr = urllib.request.HTTPPasswordMgrWithDefaultRealm()
    password_mgr.add_password(None, domain, username, password)
    return urllib.request.HTTPBasicAuthHandler(password_mgr)


def build_opener(domain: str) -> urllib.request.OpenerDirector:
    """
    Build an opener for the specified domain.
    HTTP Basic Auth will be used if credentials for the
    provided domain are found in the local ~/.netrc file.
    :param domain:  The domain for which credentials will be searched in ~/.netrc
    :return:        The opener with HTTP Basic Auth credentials added if found
    """
    handlers = []
    if default_netrc_path.is_file():
        netrc_entry = netrc().authenticators(domain)
        if netrc_entry:
            login, _, password = netrc_entry
            handlers.append(create_basic_auth_handler(domain, login, password))
    return urllib.request.build_opener(*handlers)


def download_file(
        url: str,
        dest: Path,
        http_headers: HttpHeadersType,
) -> Path:
    """
    Download a file

    @param url:     The URL of the file to download
    @param dest:    The destination of downloaded file
    """
    parsed_url = urllib.parse.urlparse(url)
    opener = build_opener(parsed_url.netloc)
    request = urllib.request.Request(url)
    headers_for_domain = http_headers.get(parsed_url.netloc, [])
    for key, value in headers_for_domain:
        request.add_header(key, value)
    try:
        with opener.open(request) as g:
            with open(dest, "b+w") as f:
                f.write(g.read())
                logging.info("- Downloaded %s to %s.", url, dest)
    except URLError:
        logging.error("URLError while downloading %s.", url)
        raise
    return dest


def call_command(
        command: str,
        buffer_logs: bool = False,
        capture_output: bool = True,
        **kwargs
) -> typing.Optional[str]:
    """
    Helpers function that call subprocess and return the output.

    Parameters:
    ----------
    command (string):       Specifies the command to run.
    buffer_logs (bool):     When True, output will be buffered and written to the log
                            when the command has finished running.
                            When False, output will be written to the log
                            line-by-line with no buffering.
    capture_output (bool)   When True, capture the command output and return it.
    """
    logging.debug(command)
    log = ""
    with subprocess.Popen(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        shell=True,
        text=True,
        **kwargs
    ) as proc:
        for line in proc.stdout:
            if capture_output:
                log += line
            if not buffer_logs:
                logging.info(line.rstrip("\n"))
    if buffer_logs:
        logging.info(log)
    if proc.returncode != 0:
        raise subprocess.CalledProcessError(proc.returncode, proc.args,
                                 output=proc.stdout, stderr=proc.stderr)
    return log if capture_output else None


def get_md5sum_for_file(filepath: Path) -> str:
    """
    Compute the MD5 hex digest of a file's contents.

    :param filepath:    Path to the file.
    :return:            Hex string of the MD5 digest.
    """
    md5 = hashlib.md5()
    with open(filepath, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            md5.update(chunk)
    return md5.hexdigest()



def remove_tree_dir(dir_path: Path):
    """
    Delete and re-create a directory

    Parameters
    ----------
    dir_path    : The directory path
    """
    try:
        # Remove the full directory.
        shutil.rmtree(dir_path)
        # Re-create an empty one.
        os.mkdir(dir_path)
    except OSError:
        logging.error("Failed to delete %s.", dir_path)
