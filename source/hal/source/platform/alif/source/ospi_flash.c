/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#include "ospi_flash.h"

#include "Driver_OSPI.h"

#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header
#include "board_defs.h"

extern ARM_DRIVER_OSPI ARM_Driver_OSPI_(OSPI_CONTROLLER_INSTANCE_CONNECTED_TO_FLASH_DEVICE);
static ARM_DRIVER_OSPI* const ptrDrvOspi = &ARM_Driver_OSPI_(OSPI_CONTROLLER_INSTANCE_CONNECTED_TO_FLASH_DEVICE);

int32_t ospi_flash_switch_clock(const ospi_delay_cfg_t *signal_delay)
{
    if (signal_delay == NULL) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    int32_t ret = ptrDrvOspi->Control(ARM_OSPI_SET_SIGNAL_DELAY_CFG, (uint32_t)signal_delay);
    if (ret != ARM_DRIVER_OK) {
        return ret;
    }

    ret = ptrDrvOspi->Control(ARM_OSPI_SET_BUS_SPEED, signal_delay->sclk_freq);

    return ret;
}
