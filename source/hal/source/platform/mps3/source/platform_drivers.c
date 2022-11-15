/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

#include "platform_drivers.h"

#include "log_macros.h"     /* Logging functions */
#include "uart_stdout.h"    /* stdout over UART. */
#include "smm_mps3.h"       /* Memory map for MPS3. */

#include <string.h>         /* For strncpy */

#if defined(ARM_NPU)
#include "ethosu_npu_init.h"

#if defined(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
#include "ethosu_ta_init.h"
#endif /* ETHOS_U_NPU_TIMING_ADAPTER_ENABLED */

#if defined(ETHOS_U_BASE_ADDR)
    #if (ETHOS_U_NPU_BASE != ETHOS_U_BASE_ADDR) && (SEC_ETHOS_U_NPU_BASE != ETHOS_U_BASE_ADDR)
        #error "NPU component configured with incorrect NPU base address."
    #endif /* (ETHOS_U_NPU_BASE != ETHOS_U_BASE_ADDR) && (SEC_ETHOS_U_NPU_BASE == ETHOS_U_BASE_ADDR) */
#else
    #error "ETHOS_U_BASE_ADDR should have been defined by the NPU component."
#endif /* defined(ETHOS_U_BASE_ADDR) */

#endif /* ARM_NPU */

/**
 * @brief   Checks if the platform is valid by checking
 *          the CPU ID for the FPGA implementation against
 *          the register from the CPU core.
 * @return  0 if successful, 1 otherwise
 */
static int verify_platform(void);

/** Platform name */
static const char* s_platform_name = DESIGN_NAME;

int platform_init(void)
{
    int err = 0;

    SystemCoreClockUpdate();    /* From start up code */

    /* UART init - will enable valid use of printf (stdout
     * re-directed at this UART (UART0) */
    UartStdOutInit();

    if (0 != (err = verify_platform())) {
        return err;
    }

#if defined(__ICACHE_PRESENT) && (__ICACHE_PRESENT == 1U)
    info("Enabling I-cache.\n");
    SCB_EnableICache();
#endif /* __ICACHE_PRESENT */

#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    info("Enabling D-cache.\n");
    SCB_EnableDCache();
#endif /* __DCACHE_PRESENT */

#if defined(ARM_NPU)

#if defined(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
    /* If the platform has timing adapter blocks along with Ethos-U core
     * block, initialise them here. */
    if (0 != (err = arm_ethosu_timing_adapter_init())) {
        return err;
    }
#endif /* ETHOS_U_NPU_TIMING_ADAPTER_ENABLED */

    int state;

    /* If Arm Ethos-U NPU is to be used, we initialise it here */
    if (0 != (state = arm_ethosu_npu_init())) {
        return state;
    }

#endif /* ARM_NPU */

    /* Print target design info */
    info("Target system design: %s\n", s_platform_name);
    return 0;
}

void platform_release(void)
{
    __disable_irq();
}

const char* platform_name(void)
{
    return s_platform_name;
}

#define CREATE_MASK(msb, lsb)           (int)(((1U << ((msb) - (lsb) + 1)) - 1) << (lsb))
#define MASK_BITS(arg, msb, lsb)        (int)((arg) & CREATE_MASK(msb, lsb))
#define EXTRACT_BITS(arg, msb, lsb)     (int)(MASK_BITS(arg, msb, lsb) >> (lsb))

static int verify_platform(void)
{
    uint32_t id = 0;
    uint32_t fpgaid = 0;
    uint32_t apnote = 0;
    uint32_t rev = 0;
    uint32_t aid = 0;
    uint32_t fpga_clk = 0;
    const uint32_t ascii_A = (uint32_t)('A');

    /* Initialise the LEDs as the switches are */
    MPS3_FPGAIO->LED = MPS3_FPGAIO->SWITCHES & 0xFF;

    info("Processor internal clock: %" PRIu32 "Hz\n", get_mps3_core_clock());

    /* Get revision information from various registers */
    rev = MPS3_SCC->CFG_REG4;
    fpgaid = MPS3_SCC->SCC_ID;
    aid = MPS3_SCC->SCC_AID;
    apnote = EXTRACT_BITS(fpgaid, 15, 4);
    fpga_clk = get_mps3_core_clock();

    info("V2M-MPS3 revision %c\n\n", (char)(rev + ascii_A));
    info("Application Note AN%" PRIx32 ", Revision %c\n", apnote,
        (char)(EXTRACT_BITS(aid, 23, 20) + ascii_A));
    info("MPS3 build %d\n", EXTRACT_BITS(aid, 31, 24));
    info("MPS3 core clock has been set to: %" PRIu32 "Hz\n", fpga_clk);

    /* Display CPU ID */
    id = SCB->CPUID;
    info("CPU ID: 0x%08" PRIx32 "\n", id);

    if(EXTRACT_BITS(id, 15, 8) == 0xD2) {
        if (EXTRACT_BITS(id, 7, 4) == 3) {
            info ("CPU: Cortex-M85 r%dp%d\n\n",
                  EXTRACT_BITS(id, 23, 20),EXTRACT_BITS(id, 3, 0));
            /* @TODO: Remove CPU_CORTEX_M55 from here once CMake min version is > 3.21.0 or when
             * toolchains officially support Cortex-M85. Currently, for CMake versions older than
             * this, Cortex-M85 is built using Cortex-M55 flags. */
#if defined (CPU_CORTEX_M55) || defined (ARMv81MML_DSP_DP_MVE_FP) || defined(CPU_CORTEX_M85)
            /* CPU ID should be "0x_41_0f_d2_30" for Cortex-M85 */
            return 0;
#endif /* (CPU_CORTEX_M55) || (ARMv81MML_DSP_DP_MVE_FP) || (CPU_CORTEX_M85) */
        } else if (EXTRACT_BITS(id, 7, 4) == 2) {
            info ("CPU: Cortex-M55 r%dp%d\n\n",
                EXTRACT_BITS(id, 23, 20),EXTRACT_BITS(id, 3, 0));
#if defined (CPU_CORTEX_M55)
            /* CPU ID should be "0x_41_0f_d2_20" for Cortex-M55 */
            return 0;
#endif /* CPU_CORTEX_M55 */
        } else if (EXTRACT_BITS(id, 7, 4) == 1) {
            info ("CPU: Cortex-M33 r%dp%d\n\n",
                EXTRACT_BITS(id, 23, 20),EXTRACT_BITS(id, 3, 0));
#if defined (CPU_CORTEX_M33)
            return 0;
#endif /* CPU_CORTEX_M33 */
        } else if (EXTRACT_BITS(id, 7, 4) == 0) {
            info ("CPU: Cortex-M23 r%dp%d\n\n",
                EXTRACT_BITS(id, 23, 20),EXTRACT_BITS(id, 3, 0));
        } else {
            info ("CPU: Cortex-M processor family");
        }
    } else if (EXTRACT_BITS(id, 15, 8) == 0xC6) {
        info ("CPU: Cortex-M%d+ r%dp%d\n\n",
            EXTRACT_BITS(id, 7, 4), EXTRACT_BITS(id, 23, 20),
            EXTRACT_BITS(id, 3, 0));
    } else {
        info ("CPU: Cortex-M%d r%dp%d\n\n",
            EXTRACT_BITS(id, 7, 4), EXTRACT_BITS(id, 23, 20),
            EXTRACT_BITS(id, 3, 0));
    }

    /* If the CPU is anything other than M33 or M55, we return 1 */
    printf_err("CPU mismatch!\n");
    return 1;
}
