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


def test_img_find_correct_classification(dut: Dut):

    dut.expect('bassoon', timeout=5)
