/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef OSPI_CALIB_H_
#define OSPI_CALIB_H_

#include <stdint.h>
#include "ospi_delay.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OSPI_DELAY_CAL_FLASH_SECTOR_SIZE        (4U * 1024U)

/* On-flash container for the calibration results: one entry per calibrated
 * controller, identified by its `idx` field. Written to the last flash sector
 * (CAL_RESULT_BASE_OFFSET) so another application can read the values back and
 * run the bus at the calibrated clock. */
#define OSPI_DELAY_BLOB_MAGIC   0xFA57C10CU

#pragma pack(push, 1)
typedef struct {
    uint32_t         magic;   /* OSPI_DELAY_BLOB_MAGIC                  */
    uint32_t         count;   /* number of delay configurations stored  */
    ospi_delay_cfg_t cfg[];   /* `count` entries                        */
} ospi_delay_blob_t;
#pragma pack(pop)

/**
 * @brief Initialize the OSPI calibration repository with the provided calibration data.
 *        See: https://github.com/alifsemi/alif_xspi-calibrator for more details on the calibration process.
 * 
 * @param calib_data Pointer to the calibration data blob.
 * @return 0 on success, -1 on failure.
 */
int32_t ospi_calib_repo_init(ospi_delay_blob_t *calib_data);

/**
 * @brief Retrieve the OSPI delay configuration for the specified instance.
 *
 * @param instance The OSPI instance index.
 * @return Pointer to the delay configuration if found, NULL otherwise.
 */
const ospi_delay_cfg_t *ospi_calib_repo_get_cfg(uint32_t instance);

#ifdef __cplusplus
}
#endif
#endif
