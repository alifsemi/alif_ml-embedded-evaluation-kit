/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef ASR_WAV2LETTER_POSTPROCESS_HPP
#define ASR_WAV2LETTER_POSTPROCESS_HPP

#include "TensorFlowLiteMicro.hpp"   /* TensorFlow headers. */
#include "BaseProcessing.hpp"
#include "Model.hpp"
#include "AsrClassifier.hpp"
#include "AsrResult.hpp"
#include "log_macros.h"

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
         * @param[in]       outputTensor       Pointer to the TFLite Micro output Tensor.
         * @param[in]       classifier         Object used to get top N results from classification.
         * @param[in]       labels             Vector of string labels to identify each output of the model.
         * @param[in/out]   result             Vector of classification results to store decoded outputs.
         * @param[in]       outputContextLen   Left/right context length for output tensor.
         * @param[in]       blankTokenIdx      Index in the labels that the "Blank token" takes.
         * @param[in]       reductionAxis      The axis that the logits of each time step is on.
         **/
        AsrPostProcess(TfLiteTensor* outputTensor, AsrClassifier& classifier,
                       const std::vector<std::string>& labels, asr::ResultVec& result,
                       uint32_t outputContextLen,
                       uint32_t blankTokenIdx, uint32_t reductionAxis);

        /**
         * @brief    Should perform post-processing of the result of inference then
         *           populate ASR result data for any later use.
         * @return   true if successful, false otherwise.
         **/
        bool DoPostProcess() override;

        /** @brief   Gets the output inner length for post-processing. */
        static uint32_t GetOutputInnerLen(const TfLiteTensor*, uint32_t outputCtxLen);

        /** @brief   Gets the output context length (left/right) for post-processing. */
        static uint32_t GetOutputContextLen(const Model& model, uint32_t inputCtxLen);

        /** @brief   Gets the number of feature vectors to be computed. */
        static uint32_t GetNumFeatureVectors(const Model& model);

    private:
        AsrClassifier& m_classifier;                /* ASR Classifier object. */
        TfLiteTensor* m_outputTensor;               /* Model output tensor. */
        const std::vector<std::string>& m_labels;   /* ASR Labels. */
        asr::ResultVec & m_results;                 /* Results vector for a single inference. */
        uint32_t m_outputContextLen;                /* lengths of left/right contexts for output. */
        uint32_t m_outputInnerLen;                  /* Length of output inner context. */
        uint32_t m_totalLen;                        /* Total length of the required axis. */
        uint32_t m_countIterations;                 /* Current number of iterations. */
        uint32_t m_blankTokenIdx;                   /* Index of the labels blank token. */
        uint32_t m_reductionAxisIdx;                /* Axis containing output logits for a single step. */

        /**
         * @brief    Checks if the tensor and axis index are valid
         *           inputs to the object - based on how it has been initialised.
         * @return   true if valid, false otherwise.
         */
        bool IsInputValid(TfLiteTensor*  tensor,
                          uint32_t axisIdx) const;

        /**
         * @brief    Gets the tensor data element size in bytes based
         *           on the tensor type.
         * @return   Size in bytes, 0 if not supported.
         */
        static uint32_t GetTensorElementSize(TfLiteTensor* tensor);

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