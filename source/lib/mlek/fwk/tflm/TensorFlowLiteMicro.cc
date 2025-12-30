/*
 * SPDX-FileCopyrightText: Copyright 2021, 2024-2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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
#include "mlek/fwk/tflm/TensorFlowLiteMicro.hpp"
#include <cstdio>

/* If target is arm-none-eabi and arch profile is 'M', wire the logging for
 * TensorFlow Lite Micro */
#if defined(__arm__) && (__ARM_ARCH_PROFILE == 77) && !defined(TFLM_EXTERNAL_TARGET)
#include "tensorflow/lite/micro/cortex_m_generic/debug_log_callback.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
static void TFLMLog(const char* s)
{
    printf("TFLM - %s\n", s);
}
#ifdef __cplusplus
}
#endif // __cplusplus

void arm::app::fwk::tflm::EnableTFLMLog()
{
    RegisterDebugLogCallback(TFLMLog);
}

#else /* defined(__arm__) && (__ARM_ARCH_PROFILE == 77) && !defined(TFLM_EXTERNAL_TARGET) */

void arm::app::fwk::tflm::EnableTFLMLog() {}

#endif /* defined(__arm__) && (__ARM_ARCH_PROFILE == 77) && !defined(TFLM_EXTERNAL_TARGET) */
