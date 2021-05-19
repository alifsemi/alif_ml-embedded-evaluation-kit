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
#include "system_init.h"

#include "uart_stdout.h"

#include <string.h>
#include <inttypes.h>

#if defined(MPS3_PLATFORM)
#define CREATE_MASK(msb, lsb)           (int)(((1U << ((msb) - (lsb) + 1)) - 1) << (lsb))
#define MASK_BITS(arg, msb, lsb)        (int)((arg) & CREATE_MASK(msb, lsb))
#define EXTRACT_BITS(arg, msb, lsb)     (int)(MASK_BITS(arg, msb, lsb) >> (lsb))
#endif /* MPS3_PLATFORM */

int system_init(void)
{
#if defined(MPS3_PLATFORM)
    uint32_t id = 0;
    uint32_t fpgaid = 0;
    uint32_t apnote = 0;
    uint32_t rev = 0;
    uint32_t aid = 0;
    uint32_t fpga_clk = 0;
    const uint32_t ascii_A = (uint32_t)('A');

    /* Initialise the LEDs as the switches are */
    MPS3_FPGAIO->LED = MPS3_FPGAIO->SWITCHES & 0xFF;
#endif

    /* UART init - will enable valid use of printf (stdout
     * re-directed at this UART (UART0) */
    UartStdOutInit();
    info("Processor internal clock: %" PRIu32 "Hz\n", GetSystemCoreClock());

#if defined(MPS3_PLATFORM)
    /* Get revision information from various registers */
    rev = MPS3_SCC->CFG_REG4;
    fpgaid = MPS3_SCC->SCC_ID;
    aid = MPS3_SCC->SCC_AID;
    apnote = EXTRACT_BITS(fpgaid, 15, 4);
    fpga_clk = GetMPS3CoreClock();

    info("V2M-MPS3 revision %c\n\n", (char)(rev + ascii_A));
    info("Application Note AN%" PRIx32 ", Revision %c\n", apnote,
        (char)(EXTRACT_BITS(aid, 23, 20) + ascii_A));
    info("MPS3 build %d\n", EXTRACT_BITS(aid, 31, 24));
    info("MPS3 core clock has been set to: %" PRIu32 "Hz\n", fpga_clk);

    /* Display CPU ID */
    id = SCB->CPUID;
    info("CPU ID: 0x%08" PRIx32 "\n", id);

    if(EXTRACT_BITS(id, 15, 8) == 0xD2) {
        if (EXTRACT_BITS(id, 7, 4) == 2) {
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
#else /* MPS3_PLATFORM */

    info("%s: complete\n", __FUNCTION__);
    return 0;
#endif /* MPS3_PLATFORM */

    /* If the CPU is anything other than M33 or M55, we return 1 */
    printf_err("CPU mismatch!\n");
    return 1;
}

void system_release(void)
{
    __disable_irq();
}

void system_name(char* name, size_t size)
{
    strncpy(name, DESIGN_NAME, size);
}