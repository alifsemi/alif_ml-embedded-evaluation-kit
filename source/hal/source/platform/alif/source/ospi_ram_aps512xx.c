/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include "board_defs.h"

#if BOARD_APS512XXN_PSRAM_PRESENT

#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "Driver_Common.h"
#include "ospi_psram_xip.h"
#include "APS512XXN_PSRAM.h"
#include "ospi_ram.h"
#include "hal_log.h"

static ospi_psram_xip_config ram_config = {
    .instance       = BOARD_PSRAM_OSPI_INSTANCE,
    .ram_init       = aps512xxn_psram_init,
    .ram_type       = RAM_TYPE_PSRAM
};

int32_t ospi_ram_init(void)
{
    if (ospi_psram_xip_init(&ram_config) < 0)
    {
        printf_err("PSRAM XIP init failed\n");
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

#endif // BOARD_APS512XXN_PSRAM_PRESENT
