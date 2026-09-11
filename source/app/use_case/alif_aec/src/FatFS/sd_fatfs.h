/* Copyright (C) Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef SD_FATFS_H
#define SD_FATFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

int init_fs();
int close_file();
int write_file(const int16_t* data, uint32_t numSamples);
int deinit_fs();

#ifdef __cplusplus
}
#endif

#endif // SD_FATFS_H
