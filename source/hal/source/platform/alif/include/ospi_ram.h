/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef OSPI_RAM_H_
#define OSPI_RAM_H_

#include <stdint.h>
#include "ospi_delay.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize OSPI driver and RAM chip. Enables XIP mode.
 *
 */
int32_t ospi_ram_init(const ospi_delay_cfg_t *signal_delay);

#ifdef __cplusplus
}
#endif
#endif
