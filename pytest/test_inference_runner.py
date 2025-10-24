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
import pykush
import time

def test_inference_runner(dut: Dut):
    # we need to reset board as the test is run only once and it already over once pytest is started. Use ykush for reset.
    try:
        pykush.YKUSH().get_serial_number_string()
        print("Ykush connected")
        tYKDevice = pykush.YKUSH()
    except pykush.pykush.YKUSHNotFound:
        print("Ykush not connected")

    tYKDevice.set_port_state(1, pykush.YKUSH_PORT_STATE_DOWN)
    time.sleep(8)
    tYKDevice.set_port_state(1, pykush.YKUSH_PORT_STATE_UP)
    time.sleep(10)

    dut.expect('Total number of inferences: 1')
    dut.expect('NPU ACTIVE: ([0-9]+) cycles')
    dut.expect('Inference completed.')
