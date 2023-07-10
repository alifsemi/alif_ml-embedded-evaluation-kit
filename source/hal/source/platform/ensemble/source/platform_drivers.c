/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
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
#include "Driver_PINMUX_AND_PINPAD.h"
#include "Driver_GPIO.h"
#include "uart_tracelib.h"
#include "services_lib_api.h"
#include "services_main.h"

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

static void MHU_msg_received(void* data);
extern ARM_DRIVER_GPIO Driver_GPIO1;
extern ARM_DRIVER_GPIO Driver_GPIO2;
extern ARM_DRIVER_GPIO Driver_GPIO3;

// IPC callback
static void ipc_rx_callback(void *data)
{
    m55_data_payload_t* payload = (m55_data_payload_t*)data;
    char *st = (char*)payload->msg;
    uint16_t id = payload->id;
    printf("****** Got message from other CPU: %s, id: %d\n", st, id);
}

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

int platform_init(void)
{
    /* Turn off PRIVDEFENA - only way to have address 0 unmapped */
    ARM_MPU_Enable(MPU_CTRL_HFNMIENA_Msk);

    copy_vtor_table_to_ram();

    if (0 != Init_SysTick()) {
        printf("Failed to initialise system tick config\n");
    }

    /* Forces retarget code to be included in build */
    extern void _clock_init(void);
    _clock_init();

    tracelib_init(NULL);

    fault_dump_enable(true);

    int err = 0;
    info("Processor internal clock: %" PRIu32 "Hz\n", GetSystemCoreClock());

    info("%s: complete\n", __FUNCTION__);

#if defined(ARM_NPU)

    int state;

    /* If Arm Ethos-U NPU is to be used, we initialise it here */
    if (0 != (state = arm_ethosu_npu_init())) {
        return state;
    }

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
    services_init(MHU_msg_received);

#if TARGET_BOARD == BOARD_AppKit_Alpha1

    Driver_GPIO2.Initialize(PIN_NUMBER_7, NULL);
    Driver_GPIO2.PowerControl(PIN_NUMBER_7, ARM_POWER_FULL);
    Driver_GPIO2.SetValue(PIN_NUMBER_7, GPIO_PIN_OUTPUT_STATE_HIGH);
    Driver_GPIO2.SetDirection(PIN_NUMBER_7, GPIO_PIN_DIRECTION_OUTPUT);
    PINMUX_Config(PORT_NUMBER_2, PIN_NUMBER_7, PINMUX_ALTERNATE_FUNCTION_0);

#elif TARGET_BOARD == BOARD_AppKit_Alpha2

    Driver_GPIO2.Initialize(PIN_NUMBER_26, NULL);
    Driver_GPIO2.PowerControl(PIN_NUMBER_26, ARM_POWER_FULL);
    Driver_GPIO2.SetValue(PIN_NUMBER_26, GPIO_PIN_OUTPUT_STATE_HIGH);
    Driver_GPIO2.SetDirection(PIN_NUMBER_26, GPIO_PIN_DIRECTION_OUTPUT);
    PINMUX_Config(PORT_NUMBER_2, PIN_NUMBER_26, PINMUX_ALTERNATE_FUNCTION_0);

    // Initialize GPIOs to capture the buttons state

    Driver_GPIO2.Initialize(PIN_NUMBER_27, NULL);
    Driver_GPIO2.PowerControl(PIN_NUMBER_27, ARM_POWER_FULL);
    Driver_GPIO2.SetDirection(PIN_NUMBER_27, GPIO_PIN_DIRECTION_INPUT);
    PINMUX_Config(PORT_NUMBER_2, PIN_NUMBER_27, PINMUX_ALTERNATE_FUNCTION_0);
    PINPAD_Config(PORT_NUMBER_2, PIN_NUMBER_27, PAD_FUNCTION_READ_ENABLE | \
                                            PAD_FUNCTION_SCHMITT_TRIGGER_ENABLE | \
                                            PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_HIGH_Z);

    Driver_GPIO3.Initialize(PIN_NUMBER_18, NULL);
    Driver_GPIO3.PowerControl(PIN_NUMBER_18, ARM_POWER_FULL);
    Driver_GPIO3.SetDirection(PIN_NUMBER_18, GPIO_PIN_DIRECTION_INPUT);
    PINMUX_Config(PORT_NUMBER_3, PIN_NUMBER_18, PINMUX_ALTERNATE_FUNCTION_0);
    PINPAD_Config(PORT_NUMBER_3, PIN_NUMBER_18, PAD_FUNCTION_READ_ENABLE | \
                                            PAD_FUNCTION_SCHMITT_TRIGGER_ENABLE | \
                                            PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_HIGH_Z);

#elif TARGET_BOARD == BOARD_DevKit

    // Initialize GPIOs to capture the buttons state

    Driver_GPIO1.Initialize(PIN_NUMBER_12, NULL);
    Driver_GPIO1.PowerControl(PIN_NUMBER_12, ARM_POWER_FULL);
    Driver_GPIO1.SetDirection(PIN_NUMBER_12, GPIO_PIN_DIRECTION_INPUT);
    PINMUX_Config(PORT_NUMBER_1, PIN_NUMBER_12, PINMUX_ALTERNATE_FUNCTION_0);
    PINPAD_Config(PORT_NUMBER_1, PIN_NUMBER_12, PAD_FUNCTION_READ_ENABLE | \
                                            PAD_FUNCTION_SCHMITT_TRIGGER_ENABLE | \
                                            PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_HIGH_Z);

    Driver_GPIO3.Initialize(PIN_NUMBER_4, NULL);
    Driver_GPIO3.PowerControl(PIN_NUMBER_4, ARM_POWER_FULL);
    Driver_GPIO3.SetDirection(PIN_NUMBER_4, GPIO_PIN_DIRECTION_INPUT);
    PINMUX_Config(PORT_NUMBER_3, PIN_NUMBER_4, PINMUX_ALTERNATE_FUNCTION_0);
    PINPAD_Config(PORT_NUMBER_3, PIN_NUMBER_4, PAD_FUNCTION_READ_ENABLE | \
                                            PAD_FUNCTION_SCHMITT_TRIGGER_ENABLE | \
                                            PAD_FUNCTION_DRIVER_DISABLE_STATE_WITH_HIGH_Z);

#else
    #error "TARGET_BOARD does not seem to be defined"
#endif
}

void init_trigger_tx(void)
{
    services_init(ipc_rx_callback);
}

#if TARGET_BOARD == BOARD_AppKit_Alpha1
static atomic_int do_inference_once = false;
#else
static atomic_int do_inference_once = true;
static bool last_btn0 = false;
static bool last_btn1 = false;
#endif

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

bool run_requested(void)
{
#if TARGET_BOARD == BOARD_AppKit_Alpha1

    bool ret = true;
    if (do_inference_once)
    {
        ret = false;
    }
    return ret;

#elif TARGET_BOARD == BOARD_AppKit_Alpha2

    bool ret = true;
    bool new_btn0, new_btn1;
    uint32_t pin_state0, pin_state1;

    // Get new button state (active low)
    Driver_GPIO3.GetValue(PIN_NUMBER_18, &pin_state0);
    new_btn0 = pin_state0 == 0;
    Driver_GPIO2.GetValue(PIN_NUMBER_27, &pin_state1);
    new_btn1 = pin_state1 == 0;

    if (do_inference_once)
    {
        // Edge detector - run inference on the positive edge of the button pressed signal
        ret = !last_btn0 && new_btn0;
    }
    if (new_btn1 && last_btn1 != new_btn1)
    {
        // Switch single shot and continuous inference mode
        do_inference_once ^= 1;
    }
    last_btn0 = new_btn0;
    last_btn1 = new_btn1;
    return ret;

#elif TARGET_BOARD == BOARD_DevKit

    bool ret = true;
    bool new_btn0, new_btn1;
    uint32_t pin_state0, pin_state1;

    // Get new button state (active low)
    Driver_GPIO1.GetValue(PIN_NUMBER_12, &pin_state0);
    new_btn0 = pin_state0 == 0;
    Driver_GPIO3.GetValue(PIN_NUMBER_4, &pin_state1);
    new_btn1 = pin_state1 == 0;

    if (do_inference_once)
    {
        // Edge detector - run inference on the positive edge of the button pressed signal
        ret = !last_btn0 && new_btn0;
    }
    if (new_btn1 && last_btn1 != new_btn1)
    {
        // Switch single shot and continuous inference mode
        do_inference_once ^= 1;
    }
    last_btn0 = new_btn0;
    last_btn1 = new_btn1;
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
        .base = MRAM_BASE,
        .limit = MRAM_BASE + MRAM_SIZE - 1,
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
    /* This is a complete map - the startup code enables PRIVDEFENA that falls back
     * to the system default, but we will turn it off later.
     */
    static const ARM_MPU_Region_t mpu_table[] __STARTUP_RO_DATA_ATTRIBUTE = {
    { // ITCM (alias) at 01000000, SRAM0 at 02000000, SRAM1 at 08000000
    .RBAR = ARM_MPU_RBAR(0x01000000, ARM_MPU_SH_NON, 0, 1, 0),  // RW, NP, XA
    .RLAR = ARM_MPU_RLAR(0x0FFFFFFF, 2)
    },
    { // LP- Peripheral & PINMUX Regions
    .RBAR = ARM_MPU_RBAR(LP_PERIPHERAL_BASE, ARM_MPU_SH_NON, 0, 1, 1),  // RW, NP, XN
    .RLAR = ARM_MPU_RLAR(LP_PERIPHERAL_BASE + 0x01FFFFFF, 0)
    },
    { // DTCM
    .RBAR = ARM_MPU_RBAR(DTCM_BASE, ARM_MPU_SH_NON, 0, 1, 1),  // RW, NP, XN
    .RLAR = ARM_MPU_RLAR(DTCM_BASE + DTCM_SIZE - 1, 2)
    },
    { // Combined RAM (30000000) - firewall translation with some system packages
    .RBAR = ARM_MPU_RBAR(0x30000000, ARM_MPU_SH_NON, 0, 1, 0),  // RW, NP, XN
    .RLAR = ARM_MPU_RLAR(0x3067FFFF, 2)
    },
    { // General peripherals
    .RBAR = ARM_MPU_RBAR(0x40000000, ARM_MPU_SH_NON, 0, 0, 1),  // RW, P, XN
    .RLAR = ARM_MPU_RLAR(0x4FFFFFFF, 0)
    },
    { // Other core's DTCM
#if defined(M55_HE)
    .RBAR = ARM_MPU_RBAR(SRAM2_BASE, ARM_MPU_SH_OUTER, 0, 1, 0),  // RW, NP, XA
    .RLAR = ARM_MPU_RLAR(SRAM3_BASE + SRAM3_SIZE - 1, 1)  // HP TCM (SRAM2 + SRAM3)
#elif defined(M55_HP)
    .RBAR = ARM_MPU_RBAR(SRAM4_BASE, ARM_MPU_SH_OUTER, 0, 1, 0),  // RW, NP, XA
    .RLAR = ARM_MPU_RLAR(SRAM5_BASE + SRAM5_SIZE - 1, 1)  // HE TCM (SRAM4 + SRAM5)
#else
  #error device not specified!
#endif
    },
    { // MRAM
    .RBAR = ARM_MPU_RBAR(MRAM_BASE, ARM_MPU_SH_NON, 1, 1, 0),  // RO, NP, XA
    .RLAR = ARM_MPU_RLAR(MRAM_BASE + MRAM_SIZE - 1, 1)
    },
    { // System PPB
    .RBAR = ARM_MPU_RBAR(0xE0000000, ARM_MPU_SH_NON, 1, 0, 1),  // RW, P, XN
    .RLAR = ARM_MPU_RLAR(0xE00FFFFF, 7)
    },
    };

    /* Define the possible Attribute regions */
    ARM_MPU_SetMemAttr(0, ARM_MPU_ATTR(   /* Attr0, Device Memory */
                          ARM_MPU_ATTR_DEVICE,
                          ARM_MPU_ATTR_DEVICE_nGnRE));
    ARM_MPU_SetMemAttr(1, ARM_MPU_ATTR(   /* Attr1, Normal Memory, Write-Back, Read-Write-Allocate */
                          ARM_MPU_ATTR_MEMORY_(1,1,1,1),
                          ARM_MPU_ATTR_MEMORY_(1,1,1,1)));
    ARM_MPU_SetMemAttr(2, ARM_MPU_ATTR(   /* Attr2, Normal Memory, Transient, Write Through, Read Allocate */
                          ARM_MPU_ATTR_MEMORY_(0,0,1,0),
                          ARM_MPU_ATTR_MEMORY_(0,0,1,0)));
    ARM_MPU_SetMemAttr(7, ARM_MPU_ATTR(   /* Attr7, Device Memory nGnRnE*/
                          ARM_MPU_ATTR_DEVICE,
                          ARM_MPU_ATTR_DEVICE_nGnRnE));

    /* Load the regions from the table */
    ARM_MPU_Load(0, &mpu_table[0], sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
}
