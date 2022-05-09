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
#include <catch.hpp>
#include "MicroNetKwsModel.hpp"
#include "hal.h"

#include "KwsResult.hpp"
#include "Labels.hpp"
#include "UseCaseHandler.hpp"
#include "Classifier.hpp"
#include "UseCaseCommonUtils.hpp"

namespace arm {
    namespace app {
        static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
        namespace kws {
            extern uint8_t* GetModelPointer();
            extern size_t GetModelLen();
        } /* namespace kws */
    } /* namespace app */
} /* namespace arm */

TEST_CASE("Model info")
{
    /* Model wrapper object. */
    arm::app::MicroNetKwsModel model;

    /* Load the model. */
    REQUIRE(model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::kws::GetModelPointer(),
                    arm::app::kws::GetModelLen()));

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::Model&>("model", model);

    REQUIRE(model.ShowModelInfoHandler());
}


TEST_CASE("Inference by index")
{
    /* Initialise the HAL and platform. */
    hal_platform_init();

    /* Model wrapper object. */
    arm::app::MicroNetKwsModel model;

    /* Load the model. */
    REQUIRE(model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::kws::GetModelPointer(),
                    arm::app::kws::GetModelLen()));

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"kws"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<int>("frameLength", arm::app::kws::g_FrameLength);  /* 640 sample length for MicroNetKws. */
    caseContext.Set<int>("frameStride", arm::app::kws::g_FrameStride);  /* 320 sample stride for MicroNetKws. */
    caseContext.Set<float>("scoreThreshold", 0.5);       /* Normalised score threshold. */

    arm::app::Classifier classifier;                     /* classifier wrapper object. */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    auto checker = [&](uint32_t audioIndex, std::vector<uint32_t> labelIndex)
    {
        caseContext.Set<uint32_t>("clipIndex", audioIndex);

        std::vector<std::string> labels;
        GetLabelsVector(labels);
        caseContext.Set<const std::vector<std::string> &>("labels", labels);

        REQUIRE(arm::app::ClassifyAudioHandler(caseContext, audioIndex, false));
        REQUIRE(caseContext.Has("results"));

        auto results = caseContext.Get<std::vector<arm::app::kws::KwsResult>>("results");

        REQUIRE(results.size() == labelIndex.size());

        for (size_t i = 0; i < results.size(); i++ ) {
            REQUIRE(results[i].m_resultVec.size());
            REQUIRE(results[i].m_resultVec[0].m_labelIdx == labelIndex[i]);
        }

    };

    SECTION("Index = 0, short clip down")
    {
        /* Result: down. */
        checker(0, {0});
    }

    SECTION("Index = 1, long clip right->left->up")
    {
        /* Result: right->right->left->up->up. */
        checker(1, {6, 6, 2, 8, 8});
    }

    SECTION("Index = 2, short clip yes")
    {
        /* Result: yes. */
        checker(2, {9});
    }

    SECTION("Index = 3, long clip yes->no->go->stop")
    {
        /* Result: yes->no->no->go->go->stop->stop. */
        checker(3, {9, 3, 3, 1, 1, 7, 7});
    }
}


TEST_CASE("Inference run all clips")
{
    /* Initialise the HAL and platform. */
    hal_platform_init();

    /* Model wrapper object. */
    arm::app::MicroNetKwsModel model;

    /* Load the model. */
    REQUIRE(model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::kws::GetModelPointer(),
                    arm::app::kws::GetModelLen()));

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"kws"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<arm::app::Model&>("model", model);
    caseContext.Set<uint32_t>("clipIndex", 0);
    caseContext.Set<int>("frameLength", arm::app::kws::g_FrameLength);  /* 640 sample length for MicroNet. */
    caseContext.Set<int>("frameStride", arm::app::kws::g_FrameStride);  /* 320 sample stride for MicroNet. */
    caseContext.Set<float>("scoreThreshold", 0.7);       /* Normalised score threshold. */
    arm::app::Classifier classifier;                     /* classifier wrapper object. */
    caseContext.Set<arm::app::Classifier&>("classifier", classifier);

    std::vector <std::string> labels;
    GetLabelsVector(labels);
    caseContext.Set<const std::vector <std::string>&>("labels", labels);
    REQUIRE(arm::app::ClassifyAudioHandler(caseContext, 0, true));
}


TEST_CASE("List all audio clips")
{
   /* Initialise the HAL and platform. */
    hal_platform_init();

    /* Model wrapper object. */
    arm::app::MicroNetKwsModel model;

    /* Load the model. */
    REQUIRE(model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::kws::GetModelPointer(),
                    arm::app::kws::GetModelLen()));

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    caseContext.Set<arm::app::Model&>("model", model);

    REQUIRE(arm::app::ListFilesHandler(caseContext));
}
