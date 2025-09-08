#/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

from pytest_embedded import Dut


def test_object_detection(dut: Dut):

    # 'Detection box:' text comes up when face was recognized with object detection
    # CI has an image in front of the camera which has one face.
    dut.expect('Detection box:', timeout=5)
