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


def test_vww(dut: Dut):

    # CI has an image in front of the camera which has one person so it should be detected.
    dut.expect('Person detected: Yes', timeout=15)
