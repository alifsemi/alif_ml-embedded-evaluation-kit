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
#include "hal.h"            /* API */

#include "hal_config.h"     /* HAL configuration */
#include "system_init.h"

#include <stdio.h>
#include <assert.h>

#if defined(ARM_NPU)

#include "ethosu_driver.h"              /* Arm Ethos-U55 driver header */
#include "timing_adapter.h"             /* Arm Ethos-U55 timing adapter driver header */
#include "timing_adapter_settings.h"    /* Arm Ethos-U55 timing adapter settings */

/**
 * @brief   Initialises the Arm Ethos-U55 NPU
 * @return  0 if successful, error code otherwise
 **/
static int _arm_npu_init(void);

#endif /* ARM_NPU */

int hal_init(hal_platform* platform, data_acq_module* data_acq,
    data_psn_module* data_psn, platform_timer* timer)
{
    assert(platform && data_acq && data_psn);

    platform->data_acq  = data_acq;
    platform->data_psn  = data_psn;
    platform->timer     = timer;
    platform->platform_init     = system_init;
    platform->platform_release  = system_release;
    system_name(platform->plat_name, sizeof(platform->plat_name));

    return 0;
}

/**
 * @brief  Local helper function to clean the slate for current platform.
 **/
static void _hal_platform_clear(hal_platform* platform)
{
    assert(platform);
    platform->inited = 0;
}

int hal_platform_init(hal_platform* platform)
{
    int state;
    assert(platform && platform->platform_init);
    _hal_platform_clear(platform);

    /* Initialise platform */
    if (0 != (state = platform->platform_init())) {
        printf_err("failed to initialise platform %s\n", platform->plat_name);
        return state;
    }

    /* Initialise the data acquisition module */
    if (0 != (state = data_acq_channel_init(platform->data_acq))) {
        if (!platform->data_acq->inited) {
            printf_err("failed to initialise data acq module: %s\n",
                platform->data_acq->system_name);
        }
        hal_platform_release(platform);
        return state;
    }

    /* Initialise the presentation module */
    if (0 != (state = data_psn_system_init(platform->data_psn))) {
        printf_err("failed to initialise data psn module: %s\n",
            platform->data_psn->system_name);
        data_acq_channel_release(platform->data_acq);
        hal_platform_release(platform);
        return state;
    }

#if defined(ARM_NPU)

    /* If Arm Ethos-U55 NPU is to be used, we initialise it here */
    if (0 != (state = _arm_npu_init())) {
        return state;
    }

#endif /* ARM_NPU */

    /* followed by the timer module */
    init_timer(platform->timer);

    info("%s platform initialised\n", platform->plat_name);
    debug("using %s module for data acquisition\n",
            platform->data_acq->system_name);
    debug("using %s module for data presentation\n",
        platform->data_psn->system_name);

    platform->inited = !state;

    return state;
}

void hal_platform_release(hal_platform *platform)
{
    assert(platform && platform->platform_release);
    data_acq_channel_release(platform->data_acq);
    data_psn_system_release(platform->data_psn);

    _hal_platform_clear(platform);
    info("releasing platform %s\n", platform->plat_name);
    platform->platform_release();
}

#if defined(ARM_NPU)
/**
 * @brief   Defines the Ethos-U interrupt handler: just a wrapper around the default
 *          implementation.
 **/
static void _arm_npu_irq_handler(void)
{
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler();
}

/**
 * @brief  Initialises the NPU IRQ
 **/
static void _arm_npu_irq_init(void)
{
    const IRQn_Type ethosu_irqnum = (IRQn_Type)EthosU_IRQn;

    /* Register the EthosU IRQ handler in our vector table.
     * Note, this handler comes from the EthosU driver */
    NVIC_SetVector(ethosu_irqnum, (uint32_t)_arm_npu_irq_handler);

    /* Enable the IRQ */
    NVIC_EnableIRQ(ethosu_irqnum);

    debug("EthosU IRQ#: %u, Handler: 0x%p\n",
            ethosu_irqnum, _arm_npu_irq_handler);
}

static int _arm_npu_timing_adapter_init(void)
{
#if defined (TA0_BASE)
    struct timing_adapter ta_0;
    struct timing_adapter_settings ta_0_settings = {
        .maxr = TA0_MAXR,
        .maxw = TA0_MAXW,
        .maxrw = TA0_MAXRW,
        .rlatency = TA0_RLATENCY,
        .wlatency = TA0_WLATENCY,
        .pulse_on = TA0_PULSE_ON,
        .pulse_off = TA0_PULSE_OFF,
        .bwcap = TA0_BWCAP,
        .perfctrl = TA0_PERFCTRL,
        .perfcnt = TA0_PERFCNT,
        .mode = TA0_MODE,
        .maxpending = 0, /* This is a read-only parameter */
        .histbin = TA0_HISTBIN,
        .histcnt = TA0_HISTCNT
    };

    if (0 != ta_init(&ta_0, TA0_BASE)) {
        printf_err("TA0 initialisation failed\n");
        return 1;
    }

    ta_set_all(&ta_0, &ta_0_settings);
#endif /* defined (TA0_BASE) */

#if defined (TA1_BASE)
    struct timing_adapter ta_1;
    struct timing_adapter_settings ta_1_settings = {
        .maxr = TA1_MAXR,
        .maxw = TA1_MAXW,
        .maxrw = TA1_MAXRW,
        .rlatency = TA1_RLATENCY,
        .wlatency = TA1_WLATENCY,
        .pulse_on = TA1_PULSE_ON,
        .pulse_off = TA1_PULSE_OFF,
        .bwcap = TA1_BWCAP,
        .perfctrl = TA1_PERFCTRL,
        .perfcnt = TA1_PERFCNT,
        .mode = TA1_MODE,
        .maxpending = 0, /* This is a read-only parameter */
        .histbin = TA1_HISTBIN,
        .histcnt = TA1_HISTCNT
    };

    if (0 != ta_init(&ta_1, TA1_BASE)) {
        printf_err("TA1 initialisation failed\n");
        return 1;
    }

    ta_set_all(&ta_1, &ta_1_settings);
#endif /* defined (TA1_BASE) */

    return 0;
}

static int _arm_npu_init(void)
{
    int err = 0;

    /* If the platform has timing adapter blocks along with Ethos-U55 core
     * block, initialise them here. */
    if (0 != (err = _arm_npu_timing_adapter_init())) {
        return err;
    }

    /* Initialise the IRQ */
    _arm_npu_irq_init();

    /* Initialise Ethos-U55 device */
    const void * ethosu_base_address = (void *)(SEC_ETHOS_U55_BASE);

    if (0 != (err = ethosu_init_v3(
                        ethosu_base_address,    /* Ethos-U55's base address. */
                        NULL,                   /* Pointer to fast mem area - NULL for U55. */
                        0,                      /* Fast mem region size. */
                        1,                      /* Security enable. */
                        1))) {                  /* Privilege enable. */
        printf_err("failed to initalise Ethos-U55 device\n");
        return err;
    }

    info("Ethos-U55 device initialised\n");

    /* Get Ethos-U55 version */
    struct ethosu_version version;
    if (0 != (err = ethosu_get_version(&version))) {
        printf_err("failed to fetch Ethos-U55 version info\n");
        return err;
    }

    info("Ethos-U55 version info:\n");
    info("\tArch:       v%u.%u.%u\n", version.id.arch_major_rev,
                                    version.id.arch_minor_rev,
                                    version.id.arch_patch_rev);
    info("\tDriver:     v%u.%u.%u\n", version.id.driver_major_rev,
                                    version.id.driver_minor_rev,
                                    version.id.driver_patch_rev);
    info("\tMACs/cc:    %u\n", (1 << version.cfg.macs_per_cc));
    info("\tCmd stream: v%u\n", version.cfg.cmd_stream_version);
    info("\tSHRAM size: %u\n", version.cfg.shram_size);

    return 0;
}
#endif /* ARM_NPU */
