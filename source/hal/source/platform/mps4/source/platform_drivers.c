/*
 * SPDX-FileCopyrightText: Copyright 2024-2025 Arm Limited and/or its
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

#include "platform_drivers.h"

#include "log_macros.h"     /* Logging functions */
#include "smm_mps4.h"       /* Memory map for MPS4. */
#include "hdlcd_drv.h"      /* HDLCD driver */
#include "peripheral_irqs.h"    /* Interrupt numbers */

#include <string.h>

#if defined(ARM_NPU)
#include "ethosu_npu_init.h"

#if defined(ETHOS_U_NPU_TIMING_ADAPTER_ENABLED)
#include "ethosu_ta_init.h"
#endif /* ETHOS_U_NPU_TIMING_ADAPTER_ENABLED */

#endif /* ARM_NPU */

#if !defined(USE_SEMIHOSTING)
#include "uart_stdout.h"    /* stdout over UART. */
#endif /* !defined(USE_SEMIHOSTING) */

/**
 * @brief   Checks if the platform is valid by checking
 *          the CPU ID for the FPGA implementation against
 *          the register from the CPU core.
 * @return  0 if successful, 1 otherwise
 */
static int verify_platform(void);

/**
 * @brief   Initialises the HDLCD for MPS4 platform
 * @return  0 if successful, error code otherwise.
 */
static int platform_hdlcd_init(void);

/** Platform name */
static const char* s_platform_name = DESIGN_NAME;

int platform_init(void)
{
    int err = 0;

    SystemCoreClockUpdate();    /* From start up code */

#if !defined(USE_SEMIHOSTING)
    /* UART init - will enable valid use of printf (stdout
     * re-directed at this UART (UART0) */
    UartStdOutInit();
#endif /* !defined(USE_SEMIHOSTING) */

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

    /* Initialise HDLCD device. */
    if (0 != (state = platform_hdlcd_init())) {
        return state;
    }

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
    MPS4_FPGAIO->LED = MPS4_FPGAIO->SWITCHES & 0xFF;

    info("Processor internal clock: %" PRIu32 "Hz\n", get_mps4_core_clock());

    /* Get revision information from various registers */
    rev = MPS4_SCC->CFG_REG4;
    fpgaid = MPS4_SCC->SCC_ID;
    aid = MPS4_SCC->SCC_AID;
    apnote = EXTRACT_BITS(fpgaid, 15, 4);
    fpga_clk = get_mps4_core_clock();

    info("V2M-MPS4 revision %c\n\n", (char)(rev + ascii_A));
    info("Application Note AN%" PRIx32 ", Revision %c\n", apnote,
        (char)(EXTRACT_BITS(aid, 23, 20) + ascii_A));
    info("MPS4 build %d\n", EXTRACT_BITS(aid, 31, 24));
    info("MPS4 core clock has been set to: %" PRIu32 "Hz\n", fpga_clk);

    /* Display CPU ID */
    id = SCB->CPUID;
    info("CPU ID: 0x%08" PRIx32 "\n", id);

    if(EXTRACT_BITS(id, 15, 8) == 0xD2) {
        if (EXTRACT_BITS(id, 7, 4) == 3) {
            info ("CPU: Cortex-M85 r%dp%d\n\n",
                  EXTRACT_BITS(id, 23, 20),EXTRACT_BITS(id, 3, 0));
#if defined (ARMv81MML_DSP_DP_MVE_FP) || defined (CPU_CORTEX_M85)
            /* CPU ID should be "0x_41_0f_d2_30" for Cortex-M85 */
            return 0;
#endif /* (ARMv81MML_DSP_DP_MVE_FP) || (CPU_CORTEX_M85) */
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

    /* If the CPU is anything other than Arm Cortex-M33, Arm Cortex-M55 or
     * Arm Cortex-M85, we return 1 */
    printf_err("CPU mismatch!\n");
    return 1;
}

static const struct hdlcd_dev_cfg_t HDLCD_DEV_CFG_S = {
    .base = HDLCD_BASE_S,
    .polarities = (POLARITIES_DATA_POLARITY_Msk |
                   POLARITIES_DATAEN_POLARITY_Msk |
                   POLARITIES_HSYNC_POLARITY_Pos),
    .bus_options = ((4 << BUS_OPTIONS_MAX_OUTSTANDING_Pos) |
                    BUS_OPTIONS_BURST_16_Msk)};

static struct hdlcd_dev_data_t HDLCD_DEV_DATA_S = {.is_initialized = false};

static struct hdlcd_dev_t HDLCD_DEV_S = {
    &(HDLCD_DEV_CFG_S),
    &(HDLCD_DEV_DATA_S)};

static struct hdlcd_dev_t* platform_get_hdlcd_dev()
{
    return &HDLCD_DEV_S;
}

static void HDLCD_Handler(void)
{
    /* Clear IRQ */
    hdlcd_clear_irq(platform_get_hdlcd_dev(), INT_DMA_END_Msk);
    __NVIC_ClearPendingIRQ(HDLCD_IRQn);
}

static void enable_hdlcd_irq(void)
{
    NVIC_ClearPendingIRQ(HDLCD_IRQn);
    NVIC_SetVector(HDLCD_IRQn, (uint32_t)HDLCD_Handler);
    NVIC_EnableIRQ(HDLCD_IRQn);
    hdlcd_enable_irq(platform_get_hdlcd_dev(), INT_DMA_END_Msk);
}

static int platform_hdlcd_init(void)
{
    /* Initialise HDLCD */
    struct hdlcd_dev_t* hdlcd_dev = platform_get_hdlcd_dev();
    enum hdlcd_error_t hdlcd_err = hdlcd_init(hdlcd_dev);

    if (hdlcd_err != HDLCD_ERR_NONE) {
        printf_err("HDLCD initialization failed.\n");
        return hdlcd_err;
    }
    debug("HDLCD device init\n");

    hdlcd_err = hdlcd_static_config(hdlcd_dev);
    if (hdlcd_err != HDLCD_ERR_NONE) {
        printf_err("Failed to set HDLCD resolution.\n");
        return hdlcd_err;
    }

#if (HDLCD_RES_HEIGHT == 240) && (HDLCD_RES_WIDTH == 320)
    const enum hdlcd_resolution_t res = HDLCD_RES_320x240;
    hdlcd_err = hdlcd_set_resolution(hdlcd_dev, res);
#elif (HDLCD_RES_HEIGHT == 480) && (HDLCD_RES_WIDTH == 640)
    const enum hdlcd_resolution_t res = HDLCD_RES_VGA;
    hdlcd_err = hdlcd_set_resolution(hdlcd_dev, res);
#else  /* resolutions */
    #error "Resolution not supported!"
#endif /* resolutions */

    if (hdlcd_err != HDLCD_ERR_NONE) {
        printf_err("Failed to set HDLCD resolution.\n");
        return hdlcd_err;
    }
    info("HDLCD resolution id: %d\n", res);
    enable_hdlcd_irq();

    struct hdlcd_buffer_cfg_t cfg = {
        .base_address = HDLCD_FRAME_BUFFER_BASE_ADDRESS,
        .line_length = HDLCD_RES_WIDTH * HDLCD_BYTES_PER_PIXEL,
        .line_count = HDLCD_RES_HEIGHT - 1,
        .line_pitch = HDLCD_RES_WIDTH * HDLCD_BYTES_PER_PIXEL,
        .pixel_format = ((HDLCD_BYTES_PER_PIXEL - 1) << PIXEL_FORMAT_BYTES_PER_PIXEL_Pos)
    };

    hdlcd_err = hdlcd_buffer_config(hdlcd_dev, &cfg);
    if (hdlcd_err != HDLCD_ERR_NONE) {
        printf_err("Failed to set HDLCD config.\n");
        return hdlcd_err;
    }
    trace("HDLCD base address set\n");

    const struct hdlcd_pixel_cfg_t hdlcd_pixel_cfg_rgb888 = {
        .red.default_color = 0x00,
        .red.bit_size = 0x8,
        .red.offset = 0x10,
        .green.default_color = 0x00,
        .green.bit_size = 0x8,
        .green.offset = 0x8,
        .blue.default_color = 0x00,
        .blue.bit_size = 0x8,
        .blue.offset = 0x0};

    hdlcd_err = hdlcd_pixel_config(hdlcd_dev, &hdlcd_pixel_cfg_rgb888);
    if (hdlcd_err != HDLCD_ERR_NONE) {
        printf_err("Failed to set HDLCD pixel config.\n");
        return hdlcd_err;
    }

    memset((uint8_t*)HDLCD_FRAME_BUFFER_BASE_ADDRESS, 0, cfg.line_pitch * cfg.line_count + 1);

    hdlcd_enable(hdlcd_dev);
    hdlcd_err = hdlcd_enable(hdlcd_dev);
    if (hdlcd_err != HDLCD_ERR_NONE) {
        printf_err("Failed to enable HDLCD.\n");
    }

    debug("HDLCD device initialised.\n");
    return 0;
}
