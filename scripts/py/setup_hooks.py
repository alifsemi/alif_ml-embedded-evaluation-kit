#!/usr/bin/env python3
#  Copyright (c) 2022 Arm Limited. All rights reserved.
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

import os
import sys
import argparse
import subprocess
import stat

def set_hooks_dir(hooks_dir):
    command = 'git config core.hooksPath {}'.format(hooks_dir)
    subprocess.Popen(command.split(), stdout=subprocess.PIPE)

def add_pre_push_hooks(hooks_dir):
    pre_push = "pre-push"
    file_path = os.path.join(hooks_dir, pre_push)
    file_exists = os.path.exists(file_path)
    if file_exists:
        os.remove(file_path)
    f = open(file_path, "a")
    f.write(
'''#!/bin/sh
#  Copyright (c) 2022 Arm Limited. All rights reserved.
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

# Called by "git push" with no arguments.  The hook should
# exit with non-zero status after issuing an appropriate message if
# it wants to stop the push.

while read local_ref local_sha remote_ref remote_sha
do
    # We should pass only added or modified C/C++ source files to cppcheck.
    changed_files=$(git diff --name-only HEAD~1 HEAD | grep -E "*\.(c|cpp|cc|cxx)" | cut -f 2)
                if [ -n "$changed_files" ]; then
                                    clang-format -style=file --dry-run --Werror $changed_files

                                    exitcode1=$?
                                    if [ $exitcode1 -ne 0 ]; then
                                        echo "Formatting errors found in file: $changed_files.
                                        \nPlease run:\n\    \"clang-format -style=file -i $changed_files\"
                                        \nto correct these errors"
                                        exit $exitcode1
                                    fi

                    cppcheck --enable=performance,portability  --error-exitcode=1 $changed_files
                                    exitcode2=$?
                                    if [ $exitcode2 -ne 0 ]; then
                                        exit $exitcode2
                                    fi
                fi
            exit 0
done

exit 0'''
)

    f.close()
    s = os.stat(file_path)
    os.chmod(file_path, s.st_mode | stat.S_IEXEC)

parser = argparse.ArgumentParser()
parser.add_argument("git_hooks_path")
args = parser.parse_args()

dir_exists = os.path.exists(args.git_hooks_path)
if not dir_exists:
    print('Error! The Git hooks directory you supplied does not exist.')
    sys.exit()

add_pre_push_hooks(args.git_hooks_path)
set_hooks_dir(args.git_hooks_path)
