/*
 * SPDX-FileCopyrightText: Copyright 2021-2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#include "UseCaseHandler.hpp"       /* Handlers for different user options. */
#include "UseCaseCommonUtils.hpp"   /* Utils functions. */
#include "RNNoiseModel.hpp"         /* Model class for running inference. */
#include "InputFiles.hpp"           /* For input audio clips. */
#include "log_macros.h"             /* Logging functions */
#include "BufAttributes.hpp"        /* Buffer attributes to be applied */

namespace arm {
namespace app {
    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
    namespace rnn {
        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace rnn */
} /* namespace app */
} /* namespace arm */

enum opcodes
{
    MENU_OPT_RUN_INF_NEXT = 1,       /* Run on next vector. */
    MENU_OPT_RUN_INF_CHOSEN,         /* Run on a user provided vector index. */
    MENU_OPT_RUN_INF_ALL,            /* Run inference on all. */
    MENU_OPT_SHOW_MODEL_INFO,        /* Show model info. */
    MENU_OPT_LIST_AUDIO_CLIPS        /* List the current baked audio clip features. */
};

static void DisplayMenu()
{
    printf("\n\n");
    printf("User input required\n");
    printf("Enter option number from:\n\n");
    printf("  %u. Run noise reduction on the next WAV\n", MENU_OPT_RUN_INF_NEXT);
    printf("  %u. Run noise reduction on a WAV at chosen index\n", MENU_OPT_RUN_INF_CHOSEN);
    printf("  %u. Run noise reduction on all WAVs\n", MENU_OPT_RUN_INF_ALL);
    printf("  %u. Show NN model info\n", MENU_OPT_SHOW_MODEL_INFO);
    printf("  %u. List audio clips\n\n", MENU_OPT_LIST_AUDIO_CLIPS);
    printf("  Choice: ");
    fflush(stdout);
}

static bool SetAppCtxClipIdx(arm::app::ApplicationContext& ctx, uint32_t idx)
{
    if (idx >= NUMBER_OF_FILES) {
        printf_err("Invalid idx %" PRIu32 " (expected less than %u)\n",
                   idx, NUMBER_OF_FILES);
        return false;
    }
    ctx.Set<uint32_t>("clipIndex", idx);
    return true;
}

void main_loop()
{
    arm::app::RNNoiseModel model;  /* Model wrapper object. */

    bool executionSuccessful = true;
    constexpr bool bUseMenu = NUMBER_OF_FILES > 1 ? true : false;

    /* Load the model. */
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::rnn::GetModelPointer(),
                    arm::app::rnn::GetModelLen())) {
        printf_err("Failed to initialise model\n");
        return;
    }

    /* Instantiate application context. */
    arm::app::ApplicationContext caseContext;

    arm::app::Profiler profiler{"noise_reduction"};
    caseContext.Set<arm::app::Profiler&>("profiler", profiler);
    caseContext.Set<uint32_t>("numInputFeatures", arm::app::rnn::g_NumInputFeatures);
    caseContext.Set<uint32_t>("frameLength", arm::app::rnn::g_FrameLength);
    caseContext.Set<uint32_t>("frameStride", arm::app::rnn::g_FrameStride);
    caseContext.Set<arm::app::RNNoiseModel&>("model", model);
    SetAppCtxClipIdx(caseContext, 0);

#if defined(MEM_DUMP_BASE_ADDR)
    /* For this use case, for valid targets, we dump contents
     * of the output tensor to a certain location in memory to
     * allow offline tools to pick this data up. */
    constexpr size_t memDumpMaxLen = MEM_DUMP_LEN;
    uint8_t* memDumpBaseAddr = reinterpret_cast<uint8_t *>(MEM_DUMP_BASE_ADDR);
    size_t memDumpBytesWritten = 0;
    caseContext.Set<size_t>("MEM_DUMP_LEN", memDumpMaxLen);
    caseContext.Set<uint8_t*>("MEM_DUMP_BASE_ADDR", memDumpBaseAddr);
    caseContext.Set<size_t*>("MEM_DUMP_BYTE_WRITTEN", &memDumpBytesWritten);
#endif /* defined(MEM_DUMP_BASE_ADDR) */

    /* Loop. */
    do {
        int menuOption = MENU_OPT_RUN_INF_NEXT;

        if (bUseMenu) {
            DisplayMenu();
            menuOption = arm::app::ReadUserInputAsInt();
            printf("\n");
        }
        switch (menuOption) {
            case MENU_OPT_RUN_INF_NEXT:
                executionSuccessful = NoiseReductionHandler(caseContext, false);
                break;
            case MENU_OPT_RUN_INF_CHOSEN: {
                printf("    Enter the audio clip IFM index [0, %d]: ", NUMBER_OF_FILES-1);
                fflush(stdout);
                auto clipIndex = static_cast<uint32_t>(arm::app::ReadUserInputAsInt());
                SetAppCtxClipIdx(caseContext, clipIndex);
                executionSuccessful = NoiseReductionHandler(caseContext, false);
                break;
            }
            case MENU_OPT_RUN_INF_ALL:
                executionSuccessful = NoiseReductionHandler(caseContext, true);
                break;
            case MENU_OPT_SHOW_MODEL_INFO:
                executionSuccessful = model.ShowModelInfoHandler();
                break;
            case MENU_OPT_LIST_AUDIO_CLIPS:
                executionSuccessful = ListFilesHandler(caseContext);
                break;
            default:
                printf("Incorrect choice, try again.");
                break;
        }
    } while (executionSuccessful && bUseMenu);
    info("Main loop terminated.\n");
}
