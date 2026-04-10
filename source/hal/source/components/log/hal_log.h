/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or
 * its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef HAL_BASIC_LOGGER_H
#define HAL_BASIC_LOGGER_H

#if !defined(UNUSED)
#define UNUSED(x) ((void)(x))
#endif /* #if !defined(UNUSED) */

#if defined(HAL_LOG_ENABLE)

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdio.h>

#define HAL_LOG_LEVEL_TRACE    (0)
#define HAL_LOG_LEVEL_DEBUG    (1)
#define HAL_LOG_LEVEL_INFO     (2)
#define HAL_LOG_LEVEL_WARN     (3)
#define HAL_LOG_LEVEL_ERROR    (4)

#ifndef HAL_LOG_LEVEL
#define HAL_LOG_LEVEL HAL_LOG_LEVEL_INFO
#endif /* HAL_LOG_LEVEL */


#if (HAL_LOG_LEVEL == HAL_LOG_LEVEL_TRACE)
#define trace(...)      \
    printf("TRACE - "); \
    printf(__VA_ARGS__)
#else
#define trace(...)
#endif /* LOG_LEVEL == HAL_LOG_LEVEL_TRACE */

#if (HAL_LOG_LEVEL <= HAL_LOG_LEVEL_DEBUG)
#define debug(...)      \
    printf("DEBUG - "); \
    printf(__VA_ARGS__)
#else
#define debug(...)
#endif /* LOG_LEVEL > LOG_LEVEL_TRACE */

#if (HAL_LOG_LEVEL <= HAL_LOG_LEVEL_INFO)
#define info(...)      \
    printf("INFO - "); \
    printf(__VA_ARGS__)
#else
#define info(...)
#endif /* LOG_LEVEL > LOG_LEVEL_DEBUG */

#if (HAL_LOG_LEVEL <= HAL_LOG_LEVEL_WARN)
#define warn(...)      \
    printf("WARN - "); \
    printf(__VA_ARGS__)
#else
#define warn(...)
#endif /* LOG_LEVEL > LOG_LEVEL_INFO */

#if (HAL_LOG_LEVEL <= HAL_LOG_LEVEL_ERROR)
#define printf_err(...) \
    printf("ERROR - "); \
    printf(__VA_ARGS__)
#else
#define printf_err(...)
#endif /* LOG_LEVEL > LOG_LEVEL_INFO */

#ifdef __cplusplus
}
#endif

#else /* defined(HAL_LOG_ENABLE) */

/**
 * Logging macros in this file will not be used.
 * Provide stubs if the definitions have not been overridden externally.
 */
#if !defined(trace)
    #define trace(...)
#endif

#if !defined(debug)
    #define debug(...)
#endif

#if !defined(info)
    #define info(...)
#endif

#if !defined(warn)
    #define warn(...)
#endif

#if !defined(printf_err)
    #define printf_err(...)
#endif

#endif /* defined(HAL_LOG_ENABLE) */

#endif /* HAL_BASIC_LOGGER_H */
