/*
 * SPDX-FileCopyrightText: Copyright 2021 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifdef __cplusplus
extern "C"
{
#endif

#include "RTE_Components.h"

#include <stdio.h>
#include <inttypes.h>

/**
 * @brief   Dump core registers on stdout
 */
static void LogCoreCPURegisters(void)
{
    printf("CTRL    : 0x%08" PRIx32 "\n", __get_CONTROL());
    printf("IPSR    : 0x%08" PRIx32 "\n", __get_IPSR());
    printf("APSR    : 0x%08" PRIx32 "\n", __get_APSR());
    printf("xPSR    : 0x%08" PRIx32 "\n", __get_xPSR());
    printf("PSP     : 0x%08" PRIx32 "\n", __get_PSP());
    printf("MSP     : 0x%08" PRIx32 "\n", __get_MSP());
    printf("PRIMASK : 0x%08" PRIx32 "\n", __get_PRIMASK());
    printf("BASEPRI : 0x%08" PRIx32 "\n", __get_BASEPRI());
    printf("FAULTMSK: 0x%08" PRIx32 "\n", __get_FAULTMASK());
}

/**
 * @brief   Default interrupt handler - an infinite loop.
 **/
__attribute__((noreturn)) static void DefaultHandler(void)
{
    LogCoreCPURegisters();
    while (1) {
        /* Without the following line, armclang may optimize away the
         * infinite loop because it'd be without side effects and thus
         * undefined behaviour. */
        __ASM volatile("");
    }
}

#define DEFAULT_HANDLER_CALL(type)              \
    do {                                        \
        printf("\n");                           \
        printf("%s caught by function %s\n",    \
             type, __FUNCTION__);               \
        DefaultHandler();                       \
    } while (0)

#define DEFAULT_ERROR_HANDLER_CALL()            \
            DEFAULT_HANDLER_CALL("Exception")

#define DEFAULT_IRQ_HANDLER_CALL()              \
            DEFAULT_HANDLER_CALL("Interrupt")

/**
 * Placeholder Exception Handlers for core interrupts.
 *
 * Weak definitions provided to be used if the user chooses not
 * to override them.
 **/

/**
 * @brief  Non maskable interrupt handler.
 **/
__attribute__((weak)) void NMI_Handler(void)
{
    DEFAULT_ERROR_HANDLER_CALL();
}

/**
 * @brief  Hardfault interrupt handler.
 **/
__attribute__((weak)) void HardFault_Handler(void)
{
    DEFAULT_ERROR_HANDLER_CALL();
}

/**
 * @brief  Memory management interrupt handler.
 **/
__attribute__((weak)) void MemManage_Handler(void)
{
    DEFAULT_IRQ_HANDLER_CALL();
}

/**
 * @brief  Bus fault interrupt handler.
 **/
__attribute__((weak)) void BusFault_Handler(void)
{
    DEFAULT_ERROR_HANDLER_CALL();
}

/**
 * @brief  Usage fault interrupt handler.
 **/
__attribute__((weak)) void UsageFault_Handler(void)
{
    DEFAULT_ERROR_HANDLER_CALL();
}

/**
 * @brief  Secure access fault interrupt handler.
 **/
__attribute__((weak)) void SecureFault_Handler(void)
{
    DEFAULT_ERROR_HANDLER_CALL();
}

/**
 * @brief  Supervisor call interrupt handler.
 **/
__attribute__((weak)) void SVC_Handler(void)
{
    DEFAULT_IRQ_HANDLER_CALL();
}

/**
 * @brief  Debug monitor interrupt handler.
 **/
__attribute__((weak)) void DebugMon_Handler(void)
{
    DEFAULT_IRQ_HANDLER_CALL();
}

/**
 * @brief  Pending SV call interrupt handler.
 */
__attribute__((weak)) void PendSV_Handler(void)
{
    DEFAULT_IRQ_HANDLER_CALL();
}

#ifdef __cplusplus
}
#endif
