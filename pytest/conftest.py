#/* Copyright (C) Alif Semiconductor - All Rights Reserved.
# * Use, distribution and modification of this code is permitted under the
# * terms stated in the Alif Semiconductor Software License Agreement
# *
# * You should have received a copy of the Alif Semiconductor Software
# * License Agreement with this file. If not, please write to:
# * contact@alifsemi.com, or visit: https://alifsemi.com/license
# *
# */

from pytest import fixture, skip, exit
import joulescope
import pykush
import time
import subprocess
import pexpect
import power_report

# --- Add timestamps to a per-DUT log file written by pytest-embedded ---
# pytest-embedded's built-in --with-timestamp only prefixes stdout, not the
# log file. We can't add timestamps to the original log file because
# PexpectProcess reads from it for expect() pattern matching - any prefix
# would corrupt the input stream.
#
# Instead, patch the listener to write a *separate* timestamped log file
# alongside the original (suffix ".ts.log"), leaving the pexpect log file
# completely untouched.
import datetime as _dt
from pytest_embedded import dut_factory as _df
from pytest_embedded.utils import to_str as _to_str

tJSDevice = None

@fixture(scope="function")
def JScope():
    """A fixture to measure power consumption from dut using JouleScope JS220"""
    global tJSDevice
    if tJSDevice is None:
        print("Joulescope not connected. Power measurements skipped")
    return tJSDevice

@fixture(scope="function")
def record_power(request):
    """A fixture returning a function that records the RoundResult of a power test.

    The recorded results of all power tests are written to power_results.md at the end of the
    pytest session."""
    def record(result):
        power_report.record(power_report.use_case_name(request.node.name), result)
    return record

def pytest_configure(config):
    # Joulescope measures the current between pins J20_9 and J20_10. But the Joulescope does not
    # let the power through unless the measurement has been started at least once.
    # Therefore we need to run at least once measurement during the start up to enable DUT to
    # start running the SW.
    print("Detecting Joulescope...")
    global tJSDevice
    if config.getoption("--jscope"):
        serial_number = config.getoption("--jscope")
        if tJSDevice == None:
            try:
                devices = joulescope.scan(config='auto')
                for device in devices:
                    if serial_number == device.device_serial_number:
                        tJSDevice = device
                        print(f'Joulescope connected with serial number {serial_number}')
                        with tJSDevice:
                            data = tJSDevice.read(contiguous_duration=0.5)
            except:
                print(f'Joulescope not connected with serial number {serial_number}')

    if tJSDevice is None:
        try:
            tJSDevice = joulescope.scan_require_one(config='auto')
            serial_number = tJSDevice.device_serial_number
            print(f'Joulescope auto-connected with serial number {serial_number}')
            with tJSDevice:
                data = tJSDevice.read(contiguous_duration=0.5)
        except:
            print("Joulescope not connected")

    global tJLDevice
    if config.getoption("--jlink"):
        if tJLDevice == None:
            tJLDevice = ['JLinkExe','-if','jtag','-jtagconf','-1,-1','-speed','15000','-autoconnect','1','-Device','CORTEX-M0+','-CommandFile','reset_cm0.jlink']

    config.addinivalue_line(
        "markers", "--jlink"
    )

def pytest_sessionfinish(session, exitstatus):
    # After all the power tests have run, collect their results to one table
    config = session.config
    path = config.getoption("--power_results")
    if power_report.write_report(path, config.getoption("--power_board"),
                                 config.getoption("--power_core")):
        print(f"\nPower results written to {path}")

def pytest_unconfigure(config):
     # recover cursor to terminal, in case pytest was run with '-s', SE
     # spamming the cursor hiding will cause it to hide from running terminal
    print("\x1b[?25h")

def pytest_addoption(parser):
    parser.addoption(
        "--ykush",
        action="store",
        default=False,
        help="Use ykush to reset board between test cases. Default is False.",
    )
    parser.addoption(
        "--jlink",
        action="store",
        default=False,
        help="Use JLinkExe to reset board between test cases. Default is False.",
    )
    parser.addoption(
        "--jscope",
        action="store",
        default="",
        help="Override Joulescope serial number (5 digits). Default is to auto detect.",
    )
    parser.addoption(
        "--power_results",
        action="store",
        default="power_results.md",
        help="File where the results of the power tests are written. Default is power_results.md.",
    )
    parser.addoption(
        "--power_board",
        action="store",
        default="Unknown board",
        help="Board / SE version heading of the power results table, e.g. 'DevKit-e8 rev. B, SE 112'.",
    )
    parser.addoption(
        "--power_core",
        action="store",
        default="",
        help="Core / NPU label added to the power results rows, e.g. HE_U55.",
    )
    parser.addoption(
        "--se_magic_baudrate",
        action="store",
        default=57600,
        help="Override magic baud rate used to find SE UART from the device config",
    )
    parser.addoption(
        "--write_register",
        metavar="ADDR=VALUE",
        nargs="+",
        help="Set register addresses and their desired values as key-value pairs. These will be written to memory before test execution starts and after every SoC reset via SE."
    )
