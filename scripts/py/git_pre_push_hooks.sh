#!/bin/sh
#  SPDX-FileCopyrightText: Copyright 2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

# shellcheck disable=SC2034,SC2162
while read local_ref local_sha remote_ref remote_sha; do
  # We should pass only added or modified C/C++ source files to cppcheck.
  changed_files=$(git diff --name-only HEAD~1 HEAD | grep -iE "\.(c|cpp|cxx|cc|h|hpp|hxx)$" | cut -f 2)
  if [ -n "$changed_files" ]; then
    # shellcheck disable=SC2086
    clang-format -style=file --dry-run --Werror $changed_files

    exitcode1=$?
    if [ $exitcode1 -ne 0 ]; then
      echo "Formatting errors found in file: $changed_files. \
                Please run:
                    \"clang-format -style=file -i $changed_files\"
                to correct these errors"
      exit $exitcode1
    fi

    # shellcheck disable=SC2086
    cppcheck --enable=performance,portability --error-exitcode=1 --suppress=*:tests* $changed_files
    exitcode2=$?
    if [ $exitcode2 -ne 0 ]; then
      exit $exitcode2
    fi
  fi
  exit 0
done

exit 0
