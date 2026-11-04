# SPDX-FileCopyrightText: Copyright 2026 Arm Limited and/or its affiliates <open-source-office@arm.com>
# SPDX-License-Identifier: Apache-2.0
#
# Reduce the progress output of the SE tools (e.g. app-write-mram) for a CI log.
#
# The tools redraw a progress line for each transferred chunk (separated with carriage returns):
#   build/AppTocPackage.bin         [############        ]62%: 12720/20352 bytes
# which fills the console with hundreds of lines per file. This filter prints the progress of each
# file only every STEP percent (and the first and the last line), other lines are passed as they
# are.
#
# Usage: <command> 2>&1 | awk -f filter_setools_progress.awk [-v step=25]

BEGIN {
    RS = "\r|\n"
    if (step == "") {
        step = 25
    }
}

/\[[# ]*\] *[0-9]+%: *[0-9]+\/[0-9]+ bytes/ {
    name = $0
    sub(/ *\[.*$/, "", name)
    pct = $0
    sub(/^.*\] */, "", pct)
    sub(/%.*$/, "", pct)
    bucket = int((pct + 0) / step)
    if (!(name in last) || bucket > last[name]) {
        print
        fflush()
        last[name] = bucket
    }
    next
}

NF {
    print
    fflush()
}
