#/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

from pytest_embedded import Dut
import re

# Menu entries
MENU_OPT_RUN_ONCE       = "1"
MENU_OPT_RUN_CONTINUOUS = "2"

def get_time(dut, timestring) -> float:
    ret = dut.expect(timestring + r' time = [-+]?(?:\d*\.*\d+) ms', timeout = 2)
    value = re.findall(r"[-+]?(?:\d*\.*\d+)", ret.group(0).decode())
    return float(value[0])

def expect_in_range(min, max, value, timestring):
    if value < min or value > max:
        raise Exception(timestring + ' time %f out of range [%f, %f] !' % (value, min, max))


def test_max_kws_inference_times(dut: Dut):
    dut.write("")
    # Start one-shot test
    dut.write(MENU_OPT_RUN_ONCE)

    # Check durations
    expect_in_range(14.0, 16.7, get_time(dut, 'Preprocessing'), 'Preprocessing')
    expect_in_range(2.6, 2.8, get_time(dut, 'Inference'), 'Inference')
    expect_in_range(0.02, 0.055, get_time(dut, 'Postprocessing'), 'Postprocessing')
