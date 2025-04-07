/*
 * SPDX-FileCopyrightText: Copyright 2022, 2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
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

#include "ethosu_ta_init.h"

#include "log_macros.h"                 /* Logging functions */

#include "timing_adapter.h"             /* Arm Ethos-U timing adapter driver header */
#include "timing_adapter_settings.h"    /* Arm Ethos-U timing adapter settings */

static uint32_t init_ta(uintptr_t base_addr,
                        struct timing_adapter_settings * ta_s,
                        char * name) {
    struct timing_adapter ta;

    if (0 != ta_init(&ta, base_addr)) {
        printf_err("TA_%s initialisation failed\n", name);
        return 1;
    }

    ta_set_all(&ta, ta_s);
    info("Configured TA_%s\t@0x%" PRIx32 "\n", name, (uint32_t)base_addr);
    return 0;
}

int arm_ethosu_timing_adapter_init(void)
{
#if defined(TA_SRAM_BASE)
    struct timing_adapter_settings sram_ta_s = {
        .maxr           = SRAM_MAXR,
        .maxw           = SRAM_MAXW,
        .maxrw          = SRAM_MAXRW,
        .rlatency       = SRAM_RLATENCY,
        .wlatency       = SRAM_WLATENCY,
        .pulse_on       = SRAM_PULSE_ON,
        .pulse_off      = SRAM_PULSE_OFF,
        .bwcap          = SRAM_BWCAP,
        .perfctrl       = SRAM_PERFCTRL,
        .perfcnt        = SRAM_PERFCNT,
        .mode           = SRAM_MODE,
        .maxpending     = 0, /* This is a read-only parameter */
        .histbin        = SRAM_HISTBIN,
        .histcnt        = SRAM_HISTCNT
    };

    if (0 != init_ta(TA_SRAM_BASE, &sram_ta_s, "SRAM")) {
        return 1;
    }
#endif /* defined (TA_SRAM_BASE) */

#if defined(TA_EXT_BASE)
    struct timing_adapter_settings ext_ta_s = {
        .maxr           = EXT_MAXR,
        .maxw           = EXT_MAXW,
        .maxrw          = EXT_MAXRW,
        .rlatency       = EXT_RLATENCY,
        .wlatency       = EXT_WLATENCY,
        .pulse_on       = EXT_PULSE_ON,
        .pulse_off      = EXT_PULSE_OFF,
        .bwcap          = EXT_BWCAP,
        .perfctrl       = EXT_PERFCTRL,
        .perfcnt        = EXT_PERFCNT,
        .mode           = EXT_MODE,
        .maxpending     = 0, /* This is a read-only parameter */
        .histbin        = EXT_HISTBIN,
        .histcnt        = EXT_HISTCNT
    };

    if (0 != init_ta(TA_EXT_BASE, &ext_ta_s, "EXT")) {
        return 1;
    }
#endif /* defined (TA_EXT_BASE) */

    return 0;
}
