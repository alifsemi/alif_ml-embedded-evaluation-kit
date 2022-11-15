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
#include "timer_mps3.h"

#include "log_macros.h"
#include "smm_mps3.h"   /* Memory map for MPS3. */

static uint64_t cpu_cycle_count = 0;    /* 64-bit cpu cycle counter */
static const char* unit_cycles = "cycles";
static const char* unit_ms = "milliseconds";

/**
 * @brief   Gets the system tick triggered cycle counter for the CPU.
 * @return  64-bit counter value.
 **/
static uint64_t Get_SysTick_Cycle_Count(void);

/**
 * SysTick initialisation
 */
static int Init_SysTick(void);

/**
 * @brief Adds one PMU counter to the counters' array
 * @param value Value of the counter
 * @param name  Name for the given counter
 * @param unit  Unit for the "value"
 * @param counters Pointer to the counter struct - the one to be populated.
 * @return true if successfully added, false otherwise
 */
static bool add_pmu_counter(
        uint64_t value,
        const char* name,
        const char* unit,
        pmu_counters* counters);

/**
 * @brief Gets the evaluated millisecond timestamp from the given MPS3 counter struct.
 * @param mps3_counters     Pointer to the MPS3 counters.
 * @return microseconds timestamp as 32 bit unsigned integer.
 */
static uint32_t get_tstamp_milliseconds(mps3_pmu_counters* mps3_counters);

void platform_reset_counters(void)
{
    MPS3_FPGAIO->CLK1HZ   = 0;
    MPS3_FPGAIO->CLK100HZ = 0;
    MPS3_FPGAIO->COUNTER  = 0;

    if (0 != Init_SysTick()) {
        printf_err("Failed to initialise system tick config\n");
    }
    debug("system tick config ready\n");

#if defined (ARM_NPU)
    ethosu_pmu_init();
#endif /* defined (ARM_NPU) */
}

void platform_get_counters(pmu_counters* counters)
{
    counters->num_counters = 0;
    counters->initialised = true;
    uint32_t i = 0;

#if defined (ARM_NPU)
    ethosu_pmu_counters npu_counters = ethosu_get_pmu_counters();
    for (i = 0; i < ETHOSU_PMU_NCOUNTERS; ++i) {
        add_pmu_counter(
            npu_counters.npu_evt_counters[i].counter_value,
            npu_counters.npu_evt_counters[i].name,
            npu_counters.npu_evt_counters[i].unit,
            counters);
    }
    for (i = 0; i < ETHOSU_DERIVED_NCOUNTERS; ++i) {
        add_pmu_counter(
            npu_counters.npu_derived_counters[i].counter_value,
            npu_counters.npu_derived_counters[i].name,
            npu_counters.npu_derived_counters[i].unit,
            counters);
    }
    add_pmu_counter(
        npu_counters.npu_total_ccnt,
        "NPU TOTAL",
        unit_cycles,
        counters);
#else
    UNUSED(i);
#endif /* defined (ARM_NPU) */

#if defined(CPU_PROFILE_ENABLED)
    mps3_pmu_counters mps3_counters = {
            .counter_1Hz        = MPS3_FPGAIO->CLK1HZ,
            .counter_100Hz      = MPS3_FPGAIO->CLK100HZ,
            .counter_fpga       = MPS3_FPGAIO->COUNTER,
            .counter_systick    = Get_SysTick_Cycle_Count()
    };

    add_pmu_counter(
            mps3_counters.counter_systick,
            "CPU TOTAL",
            unit_cycles,
            counters);

    add_pmu_counter(
            get_tstamp_milliseconds(&mps3_counters),
            "DURATION",
            unit_ms,
            counters);
#endif /* defined(CPU_PROFILE_ENABLED) */

#if !defined(CPU_PROFILE_ENABLED)
    UNUSED(get_tstamp_milliseconds);
    UNUSED(Get_SysTick_Cycle_Count);
    UNUSED(unit_ms);
#if !defined(ARM_NPU)
    UNUSED(unit_cycles);
    UNUSED(add_pmu_counter);
#endif /* !defined(ARM_NPU) */
#endif /* !defined(CPU_PROFILE_ENABLED) */
}

uint32_t get_mps3_core_clock(void)
{
    const uint32_t default_clock = 32000000 /* 32 MHz clock */;
    static int warned_once = 0;
    if (0 != MPS3_SCC->CFG_ACLK) {
        return MPS3_SCC->CFG_ACLK;
    }

    if (!warned_once) {
        warn("MPS3_SCC->CFG_ACLK reads 0. Assuming default clock of %" PRIu32 "\n",
             default_clock);
        warned_once = 1;
    }
    return default_clock;
}

void SysTick_Handler(void)
{
    /* Increment the cycle counter based on load value. */
    cpu_cycle_count += SysTick->LOAD + 1;
}

/**
 * Gets the current SysTick derived counter value
 */
static uint64_t Get_SysTick_Cycle_Count(void)
{
    uint32_t systick_val;

    NVIC_DisableIRQ(SysTick_IRQn);
    systick_val = SysTick->VAL & SysTick_VAL_CURRENT_Msk;
    NVIC_EnableIRQ(SysTick_IRQn);

    return cpu_cycle_count + (SysTick->LOAD - systick_val);
}


/**
 * SysTick initialisation
 */
static int Init_SysTick(void)
{
    const uint32_t ticks_10ms = get_mps3_core_clock()/100 + 1;
    int err = 0;

    /* Reset CPU cycle count value. */
    cpu_cycle_count = 0;

    /* Changing configuration for sys tick => guard from being
     * interrupted. */
    NVIC_DisableIRQ(SysTick_IRQn);

    /* SysTick init - this will enable interrupt too. */
    err = SysTick_Config(ticks_10ms);

    /* Enable interrupt again. */
    NVIC_EnableIRQ(SysTick_IRQn);

    /* Wait for SysTick to kick off */
    while (!err && !SysTick->VAL) {
        __NOP();
    }

    return err;
}

static bool add_pmu_counter(uint64_t value,
                            const char* name,
                            const char* unit,
                            pmu_counters* counters)
{
    const uint32_t idx = counters->num_counters;
    if (idx < NUM_PMU_COUNTERS) {
        counters->counters[idx].value = value;
        counters->counters[idx].name = name;
        counters->counters[idx].unit = unit;
        ++counters->num_counters;

        debug("%s: %" PRIu64 " %s\n", name, value, unit);
        return true;
    }
    printf_err("Failed to add PMU counter!\n");
    return false;
}

static uint32_t get_tstamp_milliseconds(mps3_pmu_counters* mps3_counters)
{
    const uint32_t divisor = get_mps3_core_clock() / 1000;
    if (mps3_counters->counter_100Hz > 100) {
        return (mps3_counters->counter_100Hz * 10);
    }
    return (mps3_counters->counter_systick/divisor);
}
