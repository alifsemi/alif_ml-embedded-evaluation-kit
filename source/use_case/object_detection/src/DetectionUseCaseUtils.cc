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
#include "DetectionUseCaseUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "InputFiles.hpp"
#include <inttypes.h>


void DisplayDetectionMenu()
{
    printf("\n\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Run detection on next ifm\n", common::MENU_OPT_RUN_INF_NEXT);
    printf("  %u. Run detection ifm at chosen index\n", common::MENU_OPT_RUN_INF_CHOSEN);
    printf("  %u. Run detection on all ifm\n", common::MENU_OPT_RUN_INF_ALL);
    printf("  %u. Show NN model info\n", common::MENU_OPT_SHOW_MODEL_INFO);
    printf("  %u. List ifm\n\n", common::MENU_OPT_LIST_IFM);
    printf("  Choice: ");
    fflush(stdout);
}


bool image::PresentInferenceResult(hal_platform& platform,
                                   const std::vector<arm::app::DetectionResult>& results)
{
    return PresentInferenceResult(platform, results, false);
}

bool image::PresentInferenceResult(hal_platform &platform,
                                   const std::vector<arm::app::DetectionResult> &results,
                                   const time_t infTimeMs)
{
    return PresentInferenceResult(platform, results, true, infTimeMs);
}


bool image::PresentInferenceResult(hal_platform &platform,
                                   const std::vector<arm::app::DetectionResult> &results,
                                   bool profilingEnabled,
                                   const time_t infTimeMs)
{
    constexpr uint32_t dataPsnTxtStartX1 = 150;
    constexpr uint32_t dataPsnTxtStartY1 = 30;


    if(profilingEnabled)
    {
        platform.data_psn->set_text_color(COLOR_YELLOW);

        /* If profiling is enabled, and the time is valid. */
        info("Final results:\n");
        info("Total number of inferences: 1\n");
        if (infTimeMs)
        {
            std::string strInf =
                    std::string{"Inference: "} +
                    std::to_string(infTimeMs) +
                    std::string{"ms"};
            platform.data_psn->present_data_text(
                    strInf.c_str(), strInf.size(),
                    dataPsnTxtStartX1, dataPsnTxtStartY1, 0);
        }
    }
    platform.data_psn->set_text_color(COLOR_GREEN);

    if(!profilingEnabled) {
        info("Final results:\n");
        info("Total number of inferences: 1\n");
    }

    for (uint32_t i = 0; i < results.size(); ++i) {
        
        if(profilingEnabled) {
            info("%" PRIu32 ")  (%f) -> %s {x=%d,y=%d,w=%d,h=%d}\n", i, 
                 results[i].m_normalisedVal, "Detection box:",
                 results[i].m_x0, results[i].m_y0, results[i].m_w, results[i].m_h );
        }
        else
        {
            info("%" PRIu32 ")  (%f) -> %s {x=%d,y=%d,w=%d,h=%d}\n", i, 
                 results[i].m_normalisedVal, "Detection box:",
                 results[i].m_x0, results[i].m_y0, results[i].m_w, results[i].m_h );
        }
    }

    return true;
}



