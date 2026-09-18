/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef OSPI_FLASH_H_
#define OSPI_FLASH_H_

#include <stdint.h>
#include "ospi_delay.h"

#define OSPI_CONTROLLER_INSTANCE_CONNECTED_TO_FLASH_DEVICE RTE_ISSI_FLASH_OSPI_DRV_NUM

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize OSPI driver and flash chip. Enables XIP mode.
 *
 */
int32_t ospi_flash_init(void);

int32_t ospi_flash_set_linear(void);
int32_t ospi_flash_set_wrap32(void);

int32_t ospi_flash_switch_clock(const ospi_delay_cfg_t *signal_delay);

#ifdef __cplusplus
}
#endif
#endif
