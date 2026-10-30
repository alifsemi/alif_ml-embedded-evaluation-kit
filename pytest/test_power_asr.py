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
import pytest
from jscope_helper import RoundCapture

def test_asr_power(dut: Dut, JScope, record_power):
    """Per-phase power of one ASR round. Needs a GPIO_PROFILING and GPIO_PROFILING_CI build and GPI0 from JScope IN0 wired to the DUT.
       See Alif_benchmark.md for more details about the Joulescope connection."""
    if JScope is None:
        pytest.skip('Joulescope not connected')

    # Arm the capture before the DUT is touched. The first rising edge seen starts the round,
    # the capture ends at the next rising edge, so the base (idle) phase is included.
    with RoundCapture(None, JScope) as capture:
        capture.start()
        result = capture.wait_for_round(timeout=260) # 260 seconds is the timeout needed for without Helium and NPU builds

    record_power(result)
    print(result.summary())
    for name, phase in result.phases.items():
        print(f"{name}: {phase.time_ms:.4f} ms, {phase.current_ma.mean:.3f} mA, "
              f"{phase.voltage_v.mean:.4f} V, {phase.power_mw.mean:.3f} mW, "
              f"{phase.energy_mj:.5f} mJ")
        assert phase.time_us > 0
        assert phase.voltage_v.mean > 0
        assert phase.energy_mj > 0
