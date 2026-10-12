/* Copyright (C) 2026 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
#ifndef POWER_MEASUREMENT_H_
#define POWER_MEASUREMENT_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Average supply current/power over a measurement window, e.g. one inference. */
typedef struct {
    int32_t  avg_mA;        /* Average current */
    int32_t  avg_mW;        /* Average power */
    uint32_t elapsed_us;    /* Length of the measured window */
} power_measurement_result_t;

/**
 * @brief Probe and configure the power measurement sensor. Called from platform_init().
 *        A missing sensor is not an error for the platform: measurement is then reported
 *        as unavailable and the window functions fail without touching the bus.
 *
 * @return 0 if a sensor was found and configured, -1 otherwise.
 */
int32_t power_measurement_init(void);

/**
 * @brief Whether power_measurement_init() found and configured a sensor.
 */
bool power_measurement_is_available(void);

/**
 * @brief Open a measurement window. One window can be open at a time.
 *        Mask anything that should not be measured (e.g. hold the LVGL lock) until
 *        power_measurement_end() returns.
 *
 * @return 0 on success, -1 if no sensor is available or the sensor could not be started.
 */
int32_t power_measurement_begin(void);

/**
 * @brief Close the window opened by power_measurement_begin() and return the averages
 *        over it. The window is limited to ~10 s.
 *
 * @param[out] result Averages over the window.
 * @return 0 on success, -1 if no sensor is available or the readout failed.
 */
int32_t power_measurement_end(power_measurement_result_t *result);

#ifdef __cplusplus
}
#endif
#endif
