/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2022-2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#include "peripheral_memmap.h"
#include "fault_handler.h"
#include <string.h>         /* For strncpy */
#include <time.h>
#include <inttypes.h>
#include <stdatomic.h>

#include "RTE_Device.h"
#include "RTE_Components.h"
#include "Driver_HWSEM.h"
#include "board.h"
#include "tracelib.h"
#include "ospi_flash.h"
#include "soc.h"
#include "core_defines.h"
#include "sys_utils.h"

#include CMSIS_device_header

#if defined(ARM_NPU)
#include "ethosu_driver.h"
#include "ethosu_npu_init.h"

#if defined(ETHOS_U_BASE_ADDR)
    #if (ETHOS_U_NPU_BASE != ETHOS_U_BASE_ADDR)
        #error "NPU component configured with incorrect NPU base address."
    #endif /* (ETHOS_U_NPU_BASE != ETHOS_U_BASE_ADDR) */
#else
    #error "ETHOS_U_BASE_ADDR should have been defined by the NPU component."
#endif /* defined(ETHOS_U_BASE_ADDR) */
#endif /* ARM_NPU */

uint32_t tprof1, tprof2, tprof3, tprof4, tprof5;

/** Platform name */
static const char* s_platform_name = DESIGN_NAME;

#ifdef SE_SERVICES_SUPPORT
extern uint32_t services_handle;

run_profile_t default_runprof;
off_profile_t default_offprof;

static void MHU_msg_received(void* data);

// IPC callback
static void ipc_rx_callback(void *data)
{
    m55_data_payload_t* payload = (m55_data_payload_t*)data;
    char *st = (char*)payload->msg;
    uint16_t id = payload->id;
    printf("****** Got message from other CPU: %s, id: %d\n", st, id);
}
#endif

#ifdef COPY_VECTORS

static VECTOR_TABLE_Type MyVectorTable[496] __attribute__((aligned (2048))) __attribute__((section (".bss.noinit.ram_vectors")));

static void copy_vtor_table_to_ram()
{
    if (SCB->VTOR == (uint32_t) MyVectorTable) {
        return;
    }
    memcpy(MyVectorTable, (const void *) SCB->VTOR, sizeof MyVectorTable);
    __DMB();
    // Set the new vector table into use.
    SCB->VTOR = (uint32_t) MyVectorTable;
    __DSB();
}
#endif

#ifdef SE_SERVICES_SUPPORT

uint32_t set_power_off_profile(off_profile_t offprof)
{
    uint32_t service_error_code;
    uint32_t err = SERVICES_set_off_cfg(services_handle, &offprof, &service_error_code);
    return (err + service_error_code);
}

uint32_t set_power_run_profile(run_profile_t runprof)
{
    uint32_t service_error_code;
    uint32_t err = SERVICES_set_run_cfg(services_handle, &runprof, &service_error_code);
    return (err + service_error_code);
}

static uint32_t set_power_profiles()
{
    uint32_t service_error_code;
    uint32_t err;

    // By default all the needed memories, power domains and gatings are enabled.
    // Use case can easily change these for more suitable settings extern default_runprof/default_offprof and change only
    // the needed values and call set_power_run_profile/set_power_off_profile
    default_runprof.power_domains   = PD_VBAT_AON_MASK | PD_SSE700_AON_MASK | PD_SYST_MASK | PD_SESS_MASK | PD_DBSS_MASK;
    default_runprof.dcdc_mode       = DCDC_MODE_PWM;
    default_runprof.dcdc_voltage    = DCDC_VOUT_0825;
    default_runprof.aon_clk_src     = CLK_SRC_LFXO;
    default_runprof.run_clk_src     = CLK_SRC_PLL;
    default_runprof.scaled_clk_freq = SCALED_FREQ_XO_HIGH_DIV_38_4_MHZ;
    default_runprof.memory_blocks   = SERAM_MASK | SRAM0_MASK | SRAM1_MASK | MRAM_MASK | FWRAM_MASK | BACKUP4K_MASK;
    default_runprof.phy_pwr_gating  = LDO_PHY_MASK | MIPI_PLL_DPHY_MASK | MIPI_TX_DPHY_MASK | MIPI_RX_DPHY_MASK;
    default_runprof.ip_clock_gating = MIPI_DSI_MASK | CDC200_MASK | MIPI_CSI_MASK | CAMERA_MASK | LP_PERIPH_MASK | NPU_HE_MASK | NPU_HP_MASK;
#ifdef OSPI_FLASH_SUPPORT
    default_runprof.ip_clock_gating |= OSPI_1_MASK;
#endif
#if defined(M55_HE) || defined(RTSS_HE)
    default_runprof.cpu_clk_freq    = CLOCK_FREQUENCY_160MHZ;
#elif defined(M55_HP) || defined(RTSS_HP)
    default_runprof.cpu_clk_freq    = CLOCK_FREQUENCY_400MHZ;
#endif
    default_runprof.vdd_ioflex_3V3  = IOFLEX_LEVEL_1V8;
    err = SERVICES_set_run_cfg(services_handle, &default_runprof, &service_error_code);

    if ((err + service_error_code) == 0) {
        // No power domains on off profile -> device can go to chip STOP mode which is the most least power consumption state
        default_offprof.power_domains   = 0;
        default_offprof.dcdc_voltage    = DCDC_VOUT_0825;
        default_offprof.dcdc_mode       = DCDC_MODE_PWM;
        default_offprof.aon_clk_src     = CLK_SRC_LFXO;
        default_offprof.stby_clk_src    = CLK_SRC_HFRC;
        default_offprof.stby_clk_freq   = SCALED_FREQ_RC_STDBY_38_4_MHZ;
        // default_offprof.sysref_clk_src = /* SoC Reference Clock shared with all subsystems */
        default_offprof.memory_blocks   = SERAM_MASK;
        default_offprof.ip_clock_gating = 0;
        default_offprof.phy_pwr_gating  = 0;
        default_offprof.vdd_ioflex_3V3  = IOFLEX_LEVEL_1V8;
        default_offprof.wakeup_events   = WE_LPGPIO;
        default_offprof.ewic_cfg        = EWIC_VBAT_GPIO;
#if defined(M55_HE) || defined(RTSS_HE)
        default_offprof.vtor_address    = 0x80480000;
        default_offprof.vtor_address_ns = 0x80480000;
#elif defined(M55_HP) || defined(RTSS_HP)
        default_offprof.vtor_address    = 0x80008000;
        default_offprof.vtor_address_ns = 0x80008000;
#else
#error "Only M55_HE or M55_HP core is supported!"
#endif
        err = SERVICES_set_off_cfg(services_handle, &default_offprof, &service_error_code);
    }

    return (err + service_error_code);
}
#endif

void enable_mipi_power(void)
{
    // if SE_SERVICES_SUPPORT is defined, then in run profile dphy is already enabled.
#ifndef SE_SERVICES_SUPPORT
    /* Enable MIPI power */
    enable_mipi_dphy_power();
    disable_mipi_dphy_isolation();
#endif
}

uint32_t enable_peripheral_clocks(void)
{
    uint32_t err = 0;
    uint32_t service_error_code = 0;
#if SE_SERVICES_SUPPORT
    // Enable peripheral clocks, dphy_tx_clk
    err = SERVICES_clocks_enable_clock(services_handle,
                                    CLKEN_HFOSC,
                                    true,
                                    &service_error_code);
    if ((err + service_error_code) == 0) {
        // CSI, DSI...
        err = SERVICES_clocks_enable_clock(services_handle,
                                        CLKEN_CLK_100M,
                                        true,
                                        &service_error_code);
    }
#else // SE_SERVICES_SUPPORT
    enable_cgu_clk38p4m();
    enable_cgu_clk100m();
#endif // SE_SERVICES_SUPPORT

    return (err + service_error_code);
}

int platform_init(void)
{
    /* Turn off PRIVDEFENA - only way to have address 0 unmapped */
    ARM_MPU_Enable(MPU_CTRL_HFNMIENA_Msk);

#ifdef COPY_VECTORS
    copy_vtor_table_to_ram();
#endif

    if (0 != Init_SysTick()) {
        printf("Failed to initialise system tick config\n");
    }

#ifndef USE_SEMIHOSTING
    /* Forces retarget code to be included in build */
    extern void _clock_init(void);
    _clock_init();
#endif

#ifdef SE_SERVICES_SUPPORT

    uint32_t se_err;
    // Init SE. Use case MainLoop.cc will do the re-init if needed but only updates the callback function
    services_init(ipc_rx_callback);

    SERVICES_synchronize_with_se(services_handle);

    uint32_t service_error_code;

    SERVICES_system_set_services_debug(services_handle, false, &service_error_code);

    se_err = (int)set_power_profiles();
#endif // SE_SERVICES_SUPPORT

    extern ARM_DRIVER_HWSEM ARM_Driver_HWSEM_(0);
    ARM_DRIVER_HWSEM *HWSEMdrv = &ARM_Driver_HWSEM_(0);

    HWSEMdrv->Initialize(NULL);

    int err = 0;
    /* Only 1 core will do the pinmux */
    if (HWSEMdrv->TryLock() == ARM_DRIVER_OK) {
        /* We're first to acquire the lock - we do it */
        BOARD_Power_Init();
        BOARD_Clock_Init();
        BOARD_Pinmux_Init();

#ifdef OSPI_FLASH_SUPPORT /* OSPI drivers compiled in, check if OSPI flash support is enabled */
        err = ospi_flash_init();
        if (err) {
            printf_err("Failed initializing OSPI flash. err=%d\n", err);
        }
#endif

        /* Lock a second time to raise the count to 2 - the signal that we've finished */
        HWSEMdrv->Lock();
    } else {
        /* Someone else got there first - they did it or are doing it. Wait until we see count 2 indicating they've finished */
        while (HWSEMdrv->GetCount() < 2);
    }

    HWSEMdrv->Uninitialize();

    // tracelib init here after pinmux is done.
    tracelib_init(NULL);
    fault_dump_enable(true);

    info("Processor internal clock: %" PRIu32 "Hz\n", GetSystemCoreClock());
    info("%s: complete\n", __FUNCTION__);

#ifdef SE_SERVICES_SUPPORT
    if (se_err) {
        printf_err("Failed to set power profiles!\n");
        // return here as if power profiles did not succeed, NPU init will fail.
        return se_err;
    }
#endif

#if defined(ARM_NPU)

    int state;

    /* If Arm Ethos-U NPU is to be used, we initialise it here */
    if (0 != (state = arm_ethosu_npu_init())) {
        return state;
    }
    NVIC_SetPriority((IRQn_Type)ETHOS_U_IRQN, 0x60);

#endif /* ARM_NPU */

    /* Print target design info */
    info("Target system design: %s\n", s_platform_name);
    return err;
}

void platform_release(void)
{
    __disable_irq();
}

const char* platform_name(void)
{
    return s_platform_name;
}

void init_trigger_rx(void)
{
#ifdef SE_SERVICES_SUPPORT
    services_init(MHU_msg_received);
#endif

    BOARD_BUTTON1_Init(NULL);
    BOARD_BUTTON2_Init(NULL);
}

void init_trigger_tx(void)
{
#ifdef SE_SERVICES_SUPPORT
    services_init(ipc_rx_callback);
#endif
}

#if BOARD_BUTTON_COUNT < 2
static atomic_int do_inference_once = false;
#else
static atomic_int do_inference_once = true;
static bool last_btn1 = false;
static bool last_btn2 = false;
#endif

#ifdef SE_SERVICES_SUPPORT
static void MHU_msg_received(void* data)
{
    m55_data_payload_t* payload = data;

    __DMB();

    switch(payload->id)
    {
        case 2:
            if (!strcmp(payload->msg, "go")) {
                // Enter continuous inference mode
               do_inference_once = false;
            }
            if (!strcmp(payload->msg, "stop")) {
                // Enter single shot inference mode
               do_inference_once = true;
            }
            break;
        case 3:
            break;
        default:
            break;
    }
}
#endif

bool run_requested(void)
{
#if BOARD_BUTTON_COUNT < 2

    bool ret = true;
    if (do_inference_once)
    {
        ret = false;
    }
    return ret;

#else

    bool ret = true;
    bool new_btn1, new_btn2;
    BOARD_BUTTON_STATE btn_state1, btn_state2;

    // Get new button state (active low)
    BOARD_BUTTON1_GetState(&btn_state1);
    BOARD_BUTTON2_GetState(&btn_state2);
    new_btn1 = btn_state1 == BOARD_BUTTON_STATE_LOW;
    new_btn2 = btn_state2 == BOARD_BUTTON_STATE_LOW;

    if (do_inference_once)
    {
        // Edge detector - run inference on the positive edge of the button pressed signal
        ret = !last_btn1 && new_btn1;
    }
    if (new_btn2 && last_btn2 != new_btn2)
    {
        // Switch single shot and continuous inference mode
        do_inference_once ^= 1;
    }
    last_btn1 = new_btn1;
    last_btn2 = new_btn2;
    return ret;

#endif
}

uint64_t ethosu_address_remap(uint64_t address, int index)
{
    UNUSED(index);
    /* Double cast to avoid build warning about pointer/integer size mismatch */
    return LocalToGlobal((void *) (uint32_t) address);
}
typedef struct address_range
{
    uintptr_t base;
    uintptr_t limit;
} address_range_t;

const address_range_t no_need_to_invalidate_areas[] = {
    /* TCM is never cached */
    {
        .base = 0x00000000,
        .limit = 0x01FFFFFF,
    },
    {
        .base = 0x20000000,
        .limit = 0x21FFFFFF,
    },
    /* MRAM should never change while running */
    {
        .base = SOC_FEAT_MRAM_BASE,
        .limit = SOC_FEAT_MRAM_BASE + SOC_FEAT_MRAM_SIZE - 1,
    }
};

static bool check_need_to_invalidate(const void *p, size_t bytes)
{
    uintptr_t base = (uintptr_t) p;
    if (bytes == 0) {
        return false;
    }
    uintptr_t limit = base + bytes - 1;
    for (unsigned int i = 0; i < sizeof no_need_to_invalidate_areas / sizeof no_need_to_invalidate_areas[0]; i++) {
        if (base >= no_need_to_invalidate_areas[i].base && limit <= no_need_to_invalidate_areas[i].limit) {
            return false;
        }
    }
    return true;
}

bool ethosu_area_needs_invalidate_dcache(const void *p, size_t bytes)
{
    /* API says null pointer can be passed */
    if (!p) {
        return true;
    }
    /* We know we have a cache and assume the cache is on */
    return check_need_to_invalidate(p, bytes);
}

bool ethosu_area_needs_flush_dcache(const void *p, size_t bytes)
{
    /* We're not using writeback for any Ethos areas */
    UNUSED(p);
    UNUSED(bytes);
    return false;
}

void MPU_Load_Regions(void)
{
    // Hopefully we can remove these memory area defined. But for now the file app_mem_regions.h is
    // in same folder as CMSIS RTE_Device.h so we cannot use it. I have propsed to move file another location.

#define APP_SRAM2_BASE 0x50000000
#define APP_SRAM2_SIZE 0x00040000
#define APP_SRAM3_BASE 0x50800000
#define APP_SRAM3_SIZE 0x00100000
#define APP_SRAM4_BASE 0x58000000
#define APP_SRAM4_SIZE 0x00040000
#define APP_SRAM5_BASE 0x58800000
#define APP_SRAM5_SIZE 0x00040000


/* Define the memory attribute index with the below properties */
#define MEMATTRIDX_NORMAL_WT_RA_TRANSIENT    0
#define MEMATTRIDX_DEVICE_nGnRE              1
#define MEMATTRIDX_NORMAL_WB_RA_WA           2
#define MEMATTRIDX_NORMAL_NON_CACHEABLE      3
#define MEMATTRIDX_DEVICE_nGnRnE             7
    /* This is a complete map - the startup code enables PRIVDEFENA that falls back
     * to the system default, but we will turn it off later.
     */
    static const ARM_MPU_Region_t mpu_table[] = {
    { // ITCM (alias) at 01000000, SRAM0 at 02000000, SRAM1 at 08000000
    .RBAR = ARM_MPU_RBAR(0x01000000, ARM_MPU_SH_NON, 0, 1, 0),  // RW, NP, XA
    .RLAR = ARM_MPU_RLAR(0x0FFFFFFF, MEMATTRIDX_NORMAL_WT_RA_TRANSIENT)
    },
    { // SSE-700 Host Peripheral Region (1A000000)
    .RBAR = ARM_MPU_RBAR(0x1A000000, ARM_MPU_SH_NON, 0, 0, 1),  // RW, P, XN
    .RLAR = ARM_MPU_RLAR(0x1FFFFFFF, MEMATTRIDX_DEVICE_nGnRE)
    },
    { // DTCM (20000000)
    .RBAR = ARM_MPU_RBAR(DTCM_BASE, ARM_MPU_SH_NON, 0, 1, 1),  // RW, NP, XN
    .RLAR = ARM_MPU_RLAR(DTCM_BASE + DTCM_SIZE - 1, MEMATTRIDX_NORMAL_WT_RA_TRANSIENT)
    },
    { // Combined RAM (30000000) - firewall translation with some system packages
    .RBAR = ARM_MPU_RBAR(0x30000000, ARM_MPU_SH_NON, 0, 1, 0),  // RW, NP, XN
    .RLAR = ARM_MPU_RLAR(0x3067FFFF, MEMATTRIDX_NORMAL_WT_RA_TRANSIENT)
    },
    { // General peripherals
    .RBAR = ARM_MPU_RBAR(0x40000000, ARM_MPU_SH_NON, 0, 0, 1),  // RW, P, XN
    .RLAR = ARM_MPU_RLAR(0x4FFFFFFF, MEMATTRIDX_DEVICE_nGnRE)
    },
    { // Other core's DTCM
#if defined(M55_HE) || defined(RTSS_HE)
    .RBAR = ARM_MPU_RBAR(APP_SRAM2_BASE, ARM_MPU_SH_OUTER, 0, 1, 0),  // RW, NP, XA
    .RLAR = ARM_MPU_RLAR(APP_SRAM3_BASE + APP_SRAM3_SIZE - 1, MEMATTRIDX_NORMAL_WB_RA_WA)  // HP TCM (SRAM2 + SRAM3)
#elif defined(M55_HP) || defined(RTSS_HP)
    .RBAR = ARM_MPU_RBAR(APP_SRAM4_BASE, ARM_MPU_SH_OUTER, 0, 1, 0),  // RW, NP, XA
    .RLAR = ARM_MPU_RLAR(APP_SRAM5_BASE + APP_SRAM5_SIZE - 1, MEMATTRIDX_NORMAL_WB_RA_WA)  // HE TCM (SRAM4 + SRAM5)
#else
  #error device not specified!
#endif
    },
    { // MRAM (80000000)
    .RBAR = ARM_MPU_RBAR(SOC_FEAT_MRAM_BASE, ARM_MPU_SH_NON, 1, 1, 0),  // RO, NP, XA
    .RLAR = ARM_MPU_RLAR(SOC_FEAT_MRAM_BASE + SOC_FEAT_MRAM_SIZE - 1, MEMATTRIDX_NORMAL_WB_RA_WA)
    },
#ifdef OSPI_FLASH_SUPPORT
    {   /* OSPI Regs - 16MB : RO-0, NP-0, XN-1  */
    .RBAR = ARM_MPU_RBAR(0x83000000, ARM_MPU_SH_NON, 0, 0, 1),
    .RLAR = ARM_MPU_RLAR(0x83FFFFFF, MEMATTRIDX_DEVICE_nGnRE)
    },
    {   /* OSPI1 XIP(eg:flash) - 512MB */
    .RBAR = ARM_MPU_RBAR(0xC0000000, ARM_MPU_SH_NON, 1, 1, 0),
    .RLAR = ARM_MPU_RLAR(0xDFFFFFFF, MEMATTRIDX_NORMAL_NON_CACHEABLE)
    },
#endif
    { // System PPB
    .RBAR = ARM_MPU_RBAR(0xE0000000, ARM_MPU_SH_NON, 1, 0, 1),  // RW, P, XN
    .RLAR = ARM_MPU_RLAR(0xE00FFFFF, MEMATTRIDX_DEVICE_nGnRnE)
    },
    };

    /* Mem Attribute for 0th index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_NORMAL_WT_RA_TRANSIENT, ARM_MPU_ATTR(
                                         /* NT=0, WB=0, RA=1, WA=0 */
                                         ARM_MPU_ATTR_MEMORY_(0,0,1,0),
                                         ARM_MPU_ATTR_MEMORY_(0,0,1,0)));

    /* Mem Attribute for 1st index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_DEVICE_nGnRE, ARM_MPU_ATTR(
                                         /* Device Memory */
                                         ARM_MPU_ATTR_DEVICE,
                                         ARM_MPU_ATTR_DEVICE_nGnRE));

    /* Mem Attribute for 2nd index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_NORMAL_WB_RA_WA, ARM_MPU_ATTR(
                                         /* NT=1, WB=1, RA=1, WA=1 */
                                         ARM_MPU_ATTR_MEMORY_(1,1,1,1),
                                         ARM_MPU_ATTR_MEMORY_(1,1,1,1)));

    /* Mem Attribute for 3th index */
    ARM_MPU_SetMemAttr(MEMATTRIDX_NORMAL_NON_CACHEABLE, ARM_MPU_ATTR(
                                         ARM_MPU_ATTR_NON_CACHEABLE,
                                         ARM_MPU_ATTR_NON_CACHEABLE));


    ARM_MPU_SetMemAttr(MEMATTRIDX_DEVICE_nGnRnE, ARM_MPU_ATTR(   /* Attr7, Device Memory nGnRnE*/
                                         ARM_MPU_ATTR_DEVICE,
                                         ARM_MPU_ATTR_DEVICE_nGnRnE));

    /* Load the regions from the table */
    ARM_MPU_Load(0, &mpu_table[0], sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
}
