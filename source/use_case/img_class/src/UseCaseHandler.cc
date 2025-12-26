/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2024-2025 Arm Limited and/or its
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
#include "UseCaseHandler.hpp"

#include "mlek/common/Classifier.hpp"
#include "mlek/common/ImageUtils.hpp"
#include "mlek/use_case/img_class/ImgClassProcessing.hpp"
#include "UseCaseCommonUtils.hpp"
#include "hal.h"
#include "mlek/log/log_macros.h"

#if defined(MLEK_FWK_TFLM)
#include "mlek/fwk/tflm/MobileNetModel.hpp"
#elif defined(MLEK_FWK_EXECUTORCH)
#include "mlek/fwk/executorch/MobileNetModel.hpp"
#endif

#include <cinttypes>

using ImgClassClassifier = arm::app::Classifier;

namespace arm {
namespace app {

    /** Based on ML framework, set up the model namespace. */
#if defined(MLEK_FWK_TFLM)
    using fwk::tflm::MobileNetModel;
#elif defined(MLEK_FWK_EXECUTORCH)
    using fwk::et::MobileNetModel;
    namespace img_class {
        extern const unsigned int g_numChannels;
        extern const float g_normMean[];
        extern const float g_normStddev[];
    } /* namespace img_class */
#endif /** MLEK_FWK_TFLM or MLEK_FWK_EXECUTORCH */

    /* Image classification inference handler. */
    bool ClassifyImageHandler(ApplicationContext& ctx)
    {
        auto& profiler = ctx.Get<Profiler&>("profiler");
        auto& model    = ctx.Get<fwk::iface::Model&>("model");

        constexpr uint32_t dataPsnImgDownscaleFactor = 2;
        constexpr uint32_t dataPsnImgStartX          = 10;
        constexpr uint32_t dataPsnImgStartY          = 35;

        constexpr uint32_t dataPsnTxtInfStartX = 150;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto inputTensor  = model.GetInputTensor(0);
        auto outputTensor = model.GetOutputTensor(0);
        auto inputShape   = inputTensor->Shape();
        if (inputShape.size() < 4) {
            printf_err("Input tensor dimension should be = 4\n");
            return false;
        }

        /* Get input shape for displaying the image. */
        const uint32_t nCols     = inputShape[MobileNetModel::ms_inputColsIdx];
        const uint32_t nRows     = inputShape[MobileNetModel::ms_inputRowsIdx];
        const uint32_t nChannels = inputShape[MobileNetModel::ms_inputChannelsIdx];

        if (nChannels != ImgClassPreProcess::kNumChannels) {
            printf_err("Number of channels mismatch\n");
            return false;
        }

#if defined(MLEK_FWK_EXECUTORCH)
        /**
         * For ExecuTorch we typically have input tensors in floating point.
         * In this case, normalisation parameters should be made available
         * with the model. We try to use these here.
         */
        /* Set up pre- and post-processing. */
        std::array<float, ImgClassPreProcess::kNumChannels> normMean{0.f, 0.f, 0.f};
        std::array<float, ImgClassPreProcess::kNumChannels> normStddev{1.f, 1.f, 1.f};

        if (inputTensor->Type() == fwk::iface::TensorType::FP32 ||
            inputTensor->Type() == fwk::iface::TensorType::FP16) {

            /* Check for mismatch in size of channels. */
            if (ImgClassPreProcess::kNumChannels != img_class::g_numChannels) {
                printf_err("Number of channels mismatch in norm parameters\n");
                return false;
            }

            /* Assign values. */
            for (size_t i = 0; i < ImgClassPreProcess::kNumChannels; ++i) {
                normMean[i] = img_class::g_normMean[i];
                normStddev[i] = img_class::g_normStddev[i];

                if (0.f == normStddev[i]) {
                    printf_err("Invalid std value: %f\n", normStddev[i]);
                    return false;
                }
            }
        }
        auto preProcess = ImgClassPreProcess(inputTensor, normMean, normStddev);
#else /* defined(MLEK_FWK_EXECUTORCH) */
        auto preProcess = ImgClassPreProcess(inputTensor);
#endif /* defined(MLEK_FWK_EXECUTORCH) */

        std::vector<ClassificationResult> results;
        ImgClassPostProcess postProcess =
            ImgClassPostProcess(outputTensor,
                                ctx.Get<ImgClassClassifier&>("classifier"),
                                ctx.Get<std::vector<std::string>&>("labels"),
                                results);
        hal_camera_init();
        auto bCamera = hal_camera_configure(nCols,
            nRows,
            HAL_CAMERA_MODE_SINGLE_FRAME,
            HAL_CAMERA_COLOUR_FORMAT_RGB888);
        if (!bCamera) {
            printf_err("Failed to configure camera.\n");
            return false;
        }

        while(true) {
#ifdef INTERACTIVE_MODE
            AwaitUserInput(); // Wait for user input before moving forward.
#endif /* INTERACTIVE_MODE */

            hal_display_clear(COLOR_BLACK);
            hal_camera_start();

            /* Strings for presentation/logging. */
            std::string str_inf{"Running inference... "};

            uint32_t capturedFrameSize = 0;
            const uint8_t* imgSrc = hal_camera_get_captured_frame(&capturedFrameSize);
            if (!imgSrc || !capturedFrameSize) {
                break;
            }

            /* Display this image on the LCD. */
            hal_display_show_image(imgSrc,
                                   nCols,
                                   nRows,
                                   nChannels,
                                   dataPsnImgStartX,
                                   dataPsnImgStartY,
                                   dataPsnImgDownscaleFactor);

            /* Display message on the LCD - inference running. */
            hal_display_show_text(
                str_inf.c_str(), str_inf.size(), dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

            const size_t imgSz =
                inputTensor->Bytes() < capturedFrameSize ? inputTensor->Bytes() : capturedFrameSize;

            /* Run the pre-processing, inference and post-processing. */
            if (!preProcess.DoPreProcess(imgSrc, imgSz)) {
                printf_err("Pre-processing failed.");
                return false;
            }

            if (!RunInference(model, profiler)) {
                printf_err("Inference failed.");
                return false;
            }

            if (!postProcess.DoPostProcess()) {
                printf_err("Post-processing failed.\n");
                return false;
            }

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            hal_display_show_text(
                str_inf.c_str(), str_inf.size(), dataPsnTxtInfStartX, dataPsnTxtInfStartY, false);

            /* Add results to context for access outside handler. */
            ctx.Set<std::vector<ClassificationResult>>("results", results);

#if VERIFY_TEST_OUTPUT
            arm::app::DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */

            if (!PresentInferenceResult(results)) {
                return false;
            }

            profiler.PrintProfilingResult();
        }

        return true;
    }

} /* namespace app */
} /* namespace arm */
