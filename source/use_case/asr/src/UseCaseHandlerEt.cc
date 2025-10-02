/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
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
#if defined(MLEK_FWK_EXECUTORCH)

#include "UseCaseHandler.hpp"

#include "ConformerProcessing.hpp"
#include "ImageUtils.hpp"
#include "UseCaseCommonUtils.hpp"
#include "Wav2LetterPreprocess.hpp"
#include "hal.h"
#include "log_macros.h"

#include <cstring>

namespace arm {
namespace app {
    bool ClassifyAudioHandler(ApplicationContext& ctx)
    {
        auto& model            = ctx.Get<fwk::iface::Model&>("model");
        auto& profiler         = ctx.Get<Profiler&>("profiler");
        auto& labels           = ctx.Get<std::vector<std::string>&>("labels");
        auto melSpecWindowSize = ctx.Get<uint32_t>("melSpecWindowSize");
        auto melSpecHopSize    = ctx.Get<uint32_t>("melSpecHopSize");
        auto chunkSize         = ctx.Get<uint32_t>("chunkSize");

        // LCD text position
        constexpr uint32_t dataPsnTxtInfStartX = 20;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto inputTensorMelSpec  = model.GetInputTensor(0);
        auto inputTensorChunkSize = model.GetInputTensor(1);
        auto outputTensorLogits  = model.GetOutputTensor(0);
        auto outputTensorChunkSize = model.GetOutputTensor(1);

        auto preProcess = ConformerPreProcess<int16_t>(
            inputTensorMelSpec,
            inputTensorChunkSize,
            melSpecWindowSize,
            melSpecHopSize,
            chunkSize
        );

        std::string decodedResult;

        auto postProcess = ConformerPostProcess(
            outputTensorLogits,
            outputTensorChunkSize,
            labels,
            decodedResult
        );

        hal_audio_init();
        if (!hal_audio_configure(HAL_AUDIO_MODE_SINGLE_BURST,
                                 HAL_AUDIO_FORMAT_16KHZ_MONO_16BIT)) {
            printf_err("Failed to configure audio\n");
            return false;
        }

        /* Loop to process audio clips. */
        while (true) {
#ifdef INTERACTIVE_MODE
            AwaitUserInput(); // Wait for user input before moving forward.
#endif /* INTERACTIVE_MODE */

            hal_display_clear(COLOR_BLACK);

            /* Get the current audio buffer and respective size. */
            uint32_t audioArrSize = 0;
            hal_audio_start();
            auto audioArr = hal_audio_get_captured_frame(&audioArrSize);
            if (!audioArrSize || !audioArr) {
                debug("End of stream\n");
                break;
            }

            /* Display message on the LCD - inference running. */
            std::string str_inf{"Running inference... "};
            hal_display_set_text_color(COLOR_WHITE);
            hal_display_show_text(
                str_inf.c_str(),
                str_inf.size(),
                dataPsnTxtInfStartX,
                dataPsnTxtInfStartY,
                false
            );

            /* Run the pre-processing, inference and post-processing. */
            if (!preProcess.DoPreProcess(audioArr, audioArrSize)) {
                printf_err("Pre-processing failed.");
                return false;
            }

            if (!RunInference(model, profiler)) {
                printf_err("Inference failed.");
                return false;
            }

            if (!postProcess.DoPostProcess()) {
                printf_err("Post-processing failed.");
                return false;
            }

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            hal_display_show_text(
                str_inf.c_str(),
                str_inf.size(),
                dataPsnTxtInfStartX,
                dataPsnTxtInfStartY,
                false
            );
            str_inf = decodedResult;
            hal_display_set_text_color(COLOR_GREEN);
            hal_display_show_text(
                str_inf.c_str(),
                str_inf.size(),
                dataPsnTxtInfStartX,
                dataPsnTxtInfStartY,
                true
            );

            profiler.PrintProfilingResult();
            info("Decoded output: %s\n", decodedResult.c_str());
        }
        return true;
    }

} /* namespace app */
} /* namespace arm */

#endif /* defined(MLEK_FWK_EXECUTORCH) */
