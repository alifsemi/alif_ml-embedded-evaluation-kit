/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#include "ospi_calib.h"

#include <stddef.h>

#define OSPI_CALIB_REPO_SIZE 4

static uint32_t cfg_count;   /* number of delay configurations stored  */
static ospi_delay_cfg_t cfg[OSPI_CALIB_REPO_SIZE];

int32_t ospi_calib_repo_init(ospi_delay_blob_t *calib_data)
{
    if (!calib_data || calib_data->magic != OSPI_DELAY_BLOB_MAGIC) {
        return -1;
    }

    cfg_count = calib_data->count < OSPI_CALIB_REPO_SIZE ? calib_data->count : OSPI_CALIB_REPO_SIZE;
    for (uint32_t i = 0; i < cfg_count; i++) {
        cfg[i] = calib_data->cfg[i];
    }

    return 0;
}

const ospi_delay_cfg_t *ospi_calib_repo_get_cfg(uint32_t instance, uint32_t sclk_freq)
{
    for (uint32_t i = 0; i < cfg_count; i++) {
        if (cfg[i].idx == instance && cfg[i].sclk_freq == sclk_freq) {
            return &cfg[i];
        }
    }
    return NULL;
}
