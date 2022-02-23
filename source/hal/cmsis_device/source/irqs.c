/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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

#include "irqs.h"
#include "cmsis.h"

#include <stdio.h>
#include <inttypes.h>

/**
 * External references
 */
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    extern uint32_t __STACK_SEAL;
#endif

extern __NO_RETURN void __PROGRAM_START(void);

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
 * Dummy Exception Handlers for core interrupts.
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

/**
 * @brief   System tick interrupt handler.
 **/
__attribute__((weak)) void SysTick_Handler(void)
{
    DEFAULT_IRQ_HANDLER_CALL();
}

/**
 * Interrupt vector table.
 */
irq_vec_type __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
    (irq_vec_type)(&__INITIAL_SP),  /*     Initial Stack Pointer */
    Reset_Handler      , /* 1 Initial PC, set to entry point */

    NMI_Handler        , /* 2 (-14) NMI Handler            */
    HardFault_Handler  , /* 3 (-13) Hard Fault Handler     */
    MemManage_Handler  , /* 4 (-12) MPU Fault Handler      */
    BusFault_Handler   , /* 5 (-11) Bus Fault Handler      */
    UsageFault_Handler , /* 6 (-10) Usage Fault Handler    */
    SecureFault_Handler, /* 7 ( -9) Secure Fault Handler   */
    0                   , /* 8 ( -8) Reserved               */
    0                   , /* 9 ( -7) Reserved               */
    0                   , /* 10 ( -6) Reserved              */
    SVC_Handler        , /* 11 ( -5) SVCall Handler        */
    DebugMon_Handler   , /* 12 ( -4) Debug Monitor Handler */
    0                   , /* 13 ( -3) Reserved              */
    PendSV_Handler     , /* 14 ( -2) PendSV Handler        */
    SysTick_Handler    , /* 15 ( -1) SysTick Handler       */

    /* External sources to be populated by user. */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*   0 -  16 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  16 -  32 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  32 -  48 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  48 -  64 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  64 -  80 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  80 -  96 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*  96 -  112 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 112 -  128 */
};

/* Reset handler - starting point of our application. */
__attribute__((used)) void Reset_Handler(void)
{
    /* Initialise system. */
    SystemInit();

    /* cmsis supplied entry point. */
    __PROGRAM_START();
}

#ifdef __cplusplus
}
#endif
