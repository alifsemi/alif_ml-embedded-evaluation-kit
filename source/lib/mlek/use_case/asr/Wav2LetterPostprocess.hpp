/*
 * SPDX-FileCopyrightText: Copyright 2021-2022, 2025 Arm Limited and/or its affiliates
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
#ifndef ASR_WAV2LETTER_POSTPROCESS_HPP
#define ASR_WAV2LETTER_POSTPROCESS_HPP

#include "mlek/use_case/asr/AsrClassifier.hpp"
#include "mlek/use_case/asr/AsrResult.hpp"
#include "mlek/common/BaseProcessing.hpp"
#include "mlek/fwk/iface/Model.hpp"
#include "mlek/fwk/iface/Tensor.hpp"
#include "mlek/log/log_macros.h"

namespace arm {
namespace app {

    /**
     * @brief   Helper class to manage tensor post-processing for "wav2letter"
     *          output.
     */
    class AsrPostProcess : public BasePostProcess {
    public:
        bool m_lastIteration = false;   /* Flag to set if processing the last set of data for a clip. */

        /**
         * @brief           Constructor
         * @param[in]       model              Model object reference.
         * @param[in]       classifier         Object used to get top N results from classification.
         * @param[in]       labels             Vector of string labels to identify each output of
         *the model.
         * @param[in/out]   results            Vector of classification results to store decoded
         *outputs.
         * @param[in]       inputContextLen    Left/right context length for input tensor.
         * @param[in]       blankTokenIdx      Index in the labels that the "Blank token" takes.
         * @param[in]       reductionAxisIdx   The axis that the logits of each time step is on.
         **/
        AsrPostProcess(const fwk::iface::Model& model,
                       AsrClassifier& classifier,
                       const std::vector<std::string>& labels,
                       asr::ResultVec& results,
                       uint32_t inputContextLen,
                       uint32_t blankTokenIdx,
                       uint32_t reductionAxisIdx);

        /**
         * @brief    Should perform post-processing of the result of inference then
         *           populate ASR result data for any later use.
         * @return   true if successful, false otherwise.
         **/
        bool DoPostProcess() override;

        /** @brief   Gets the output context length (left/right) for post-processing. */
        uint32_t GetOutputContextLen() const;

        /** @brief   Gets the number of feature vectors to be computed. */
        uint32_t GetNumFeatureVectors() const;

    private:
        AsrClassifier& m_classifier;                /* ASR Classifier object. */
        const fwk::iface::Model& m_model;           /* Model reference */
        std::shared_ptr<fwk::iface::TensorIface> m_outputTensor; /* Model output tensor. */
        const std::vector<std::string>& m_labels;   /* ASR Labels. */
        asr::ResultVec & m_results;                 /* Results vector for a single inference. */
        uint32_t m_inputContextLen;                 /* length of left/right contexts for input */
        uint32_t m_outputContextLen;                /* lengths of left/right contexts for output. */
        uint32_t m_outputInnerLen;                  /* Length of output inner context. */
        uint32_t m_totalLen;                        /* Total length of the required axis. */
        uint32_t m_countIterations;                 /* Current number of iterations. */
        uint32_t m_blankTokenIdx;                   /* Index of the labels blank token. */
        uint32_t m_reductionAxisIdx;                /* Axis containing output logits for a single step. */

        /** @brief   Gets the output inner length for post-processing. */
        uint32_t GetOutputInnerLen() const;

        /**
         * @brief    Checks if the tensor and axis index are valid
         *           inputs to the object - based on how it has been initialised.
         * @return   true if valid, false otherwise.
         */
        bool IsInputValid(const std::shared_ptr<fwk::iface::TensorIface> tensor,
                          uint32_t axisIdx) const;

        /**
         * @brief    Erases sections from the data assuming row-wise
         *           arrangement along the context axis.
         * @return   true if successful, false otherwise.
         */
        bool EraseSectionsRowWise(uint8_t* ptrData,
                                  uint32_t strideSzBytes,
                                  bool lastIteration);
    };

} /* namespace app */
} /* namespace arm */

#endif /* ASR_WAV2LETTER_POSTPROCESS_HPP */
