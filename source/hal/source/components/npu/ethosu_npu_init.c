/*
 * SPDX-FileCopyrightText: Copyright 2022-2024 Arm Limited and/or its affiliates
 * <open-source-office@arm.com> SPDX-License-Identifier: Apache-2.0
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

#include "ethosu_npu_init.h"

#include "RTE_Components.h"         /* For CPU related defintiions */
#include "log_macros.h"             /* Logging functions */

#include "ethosu_mem_config.h"      /* Arm Ethos-U memory config */
#include "ethosu_driver.h"          /* Arm Ethos-U driver header */

/* Mandatory definition checks. */
#if !defined(ETHOS_U_BASE_ADDR)
#error "NPU base address is undefined."
#endif /* defnied(ETHOS_U_BASE_ADDR) */

#if !defined(ETHOS_U_IRQN)
#error "Arm NPU interrupt number is undefined."
#endif /* !defined(ETHOS_U_IRQN) */

#if !defined(ETHOS_U_SEC_ENABLED)
#error "Arm NPU security mode is undefined."
#endif /* !defined(ETHOS_U_SEC_ENABLED) */

#if !defined(ETHOS_U_PRIV_ENABLED)
#error "Arm NPU privilege mode is undefined."
#endif /* !defined(ETHOS_U_PRIV_ENABLED) */

#if defined(ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0)
static uint8_t cache_arena[ETHOS_U_CACHE_BUF_SZ] CACHE_BUF_ATTRIBUTE;
#else  /* defined (ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0) */
static uint8_t *cache_arena = NULL;
#endif /* defined (ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0) */

struct ethosu_driver ethosu_drv; /* Default Ethos-U device driver */

static uint8_t *get_cache_arena()
{
    return cache_arena;
}

static size_t get_cache_arena_size()
{
#if defined(ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0)
    return sizeof(cache_arena);
#else  /* defined (ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0) */
    return 0;
#endif /* defined (ETHOS_U_CACHE_BUF_SZ) && (ETHOS_U_CACHE_BUF_SZ > 0) */
}

/**
 * @brief  Initialises the NPU IRQ
 **/
static void arm_ethosu_npu_irq_init(void)
{
    const IRQn_Type ethosu_irqnum = (IRQn_Type)ETHOS_U_IRQN;

    /* Register the EthosU IRQ handler in our vector table.
     * Note, this handler comes from the EthosU driver */
    NVIC_SetVector(ethosu_irqnum, (uint32_t)arm_ethosu_npu_irq_handler);

    /* Enable the IRQ */
    NVIC_EnableIRQ(ethosu_irqnum);

    debug("EthosU IRQ#: %u, Handler: 0x%p\n",
          ethosu_irqnum, arm_ethosu_npu_irq_handler);
}

/**
 * @brief   Defines the Ethos-U interrupt handler: just a wrapper around the default
 *          implementation.
 **/
void arm_ethosu_npu_irq_handler(void)
{
    /* Call the default interrupt handler from the NPU driver */
    ethosu_irq_handler(&ethosu_drv);
}

int arm_ethosu_npu_init(void)
{
    int err = 0;

    /* Initialise the IRQ */
    arm_ethosu_npu_irq_init();

    /* Initialise Ethos-U device */
    void* const ethosu_base_address = (void *)(ETHOS_U_BASE_ADDR);
    info("Initialising Ethos-U device@0x%" PRIx32 "\n", (uint32_t)(ETHOS_U_BASE_ADDR));

    if (0 != (err = ethosu_init(&ethosu_drv,         /* Ethos-U driver device pointer */
                                ethosu_base_address, /* Ethos-U NPU's base address. */
                                get_cache_arena(),   /* Pointer to fast mem area - NULL for U55. */
                                get_cache_arena_size(), /* Fast mem region size. */
                                ETHOS_U_SEC_ENABLED,    /* Security enable. */
                                ETHOS_U_PRIV_ENABLED))) /* Privilege enable. */ {
        printf_err("Failed to initialise Ethos-U device\n");
        return err;
    }

    info("Ethos-U device initialised\n");

    /* Get Ethos-U version */
    struct ethosu_driver_version driver_version;
    struct ethosu_hw_info hw_info;

    ethosu_get_driver_version(&driver_version);
    ethosu_get_hw_info(&ethosu_drv, &hw_info);

    info("Ethos-U version info:\n");
    info("\tArch:       v%" PRIu32 ".%" PRIu32 ".%" PRIu32 "\n",
         hw_info.version.arch_major_rev,
         hw_info.version.arch_minor_rev,
         hw_info.version.arch_patch_rev);
    info("\tDriver:     v%" PRIu8 ".%" PRIu8 ".%" PRIu8 "\n",
         driver_version.major,
         driver_version.minor,
         driver_version.patch);
    info("\tMACs/cc:    %" PRIu32 "\n", (uint32_t)(1 << hw_info.cfg.macs_per_cc));
    info("\tCmd stream: v%" PRIu32 "\n", hw_info.cfg.cmd_stream_version);

    return 0;
}
