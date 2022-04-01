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

#include "ethosu_ta_init.h"

#include "log_macros.h"                 /* Logging functions */

#include "timing_adapter.h"             /* Arm Ethos-U timing adapter driver header */
#include "timing_adapter_settings.h"    /* Arm Ethos-U timing adapter settings */

int arm_ethosu_timing_adapter_init(void)
{
#if defined(TA0_BASE)
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
        .histcnt = TA0_HISTCNT};

    if (0 != ta_init(&ta_0, TA0_BASE)) {
        printf_err("TA0 initialisation failed\n");
        return 1;
    }

    ta_set_all(&ta_0, &ta_0_settings);
    info("TA0 values set\n");
#endif /* defined (TA0_BASE) */

#if defined(TA1_BASE)
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
        .histcnt = TA1_HISTCNT};

    if (0 != ta_init(&ta_1, TA1_BASE))
    {
        printf_err("TA1 initialisation failed\n");
        return 1;
    }

    ta_set_all(&ta_1, &ta_1_settings);
    info("TA1 values set\n");
#endif /* defined (TA1_BASE) */

    return 0;
}
