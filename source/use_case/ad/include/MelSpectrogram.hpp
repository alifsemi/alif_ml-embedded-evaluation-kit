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
#ifndef MELSPECTROGRAM_HPP
#define MELSPECTROGRAM_HPP

#include "PlatformMath.hpp"

#include <vector>
#include <cstdint>
#include <cmath>
#include <limits>
#include <string>

namespace arm {
namespace app {
namespace audio {

    /* Mel Spectrogram consolidated parameters */
    class MelSpecParams {
    public:
        float       m_samplingFreq;
        uint32_t    m_numFbankBins;
        float       m_melLoFreq;
        float       m_melHiFreq;
        uint32_t    m_frameLen;
        uint32_t    m_frameLenPadded;
        bool        m_useHtkMethod;

        /** @brief  Constructor */
        MelSpecParams(const float samplingFreq, const uint32_t numFbankBins,
                      const float melLoFreq, const float melHiFreq,
                      const uint32_t frameLen, const bool useHtkMethod);

        MelSpecParams()  = delete;
        ~MelSpecParams() = default;

        /** @brief  String representation of parameters */
        std::string Str() const;
    };

    /**
     * @brief   Class for Mel Spectrogram feature extraction.
     *          Based on https://github.com/ARM-software/ML-KWS-for-MCU/blob/master/Deployment/Source/MFCC/mfcc.cpp
     *          This class is designed to be generic and self-sufficient but
     *          certain calculation routines can be overridden to accommodate
     *          use-case specific requirements.
     */
    class MelSpectrogram {

    public:
        /**
        * @brief        Extract Mel Spectrogram for one single small frame of
        *               audio data e.g. 640 samples.
        * @param[in]    audioData       Vector of audio samples to calculate
        *               features for.
        * @param[in]    trainingMean    Value to subtract from the the computed mel spectrogram, default 0.
        * @return       Vector of extracted Mel Spectrogram features.
        **/
        std::vector<float> ComputeMelSpec(const std::vector<int16_t>& audioData, float trainingMean = 0);

        /**
         * @brief       Constructor
         * @param[in]   params   Mel Spectrogram parameters
        */
        explicit MelSpectrogram(const MelSpecParams& params);

        MelSpectrogram() = delete;
        ~MelSpectrogram() = default;

        /** @brief  Initialise */
        void Init();

        /**
         * @brief        Extract Mel Spectrogram features and quantise for one single small
         *               frame of audio data e.g. 640 samples.
         * @param[in]    audioData      Vector of audio samples to calculate
         *               features for.
         * @param[in]    quantScale     quantisation scale.
         * @param[in]    quantOffset    quantisation offset.
         * @param[in]    trainingMean   training mean.
         * @return       Vector of extracted quantised Mel Spectrogram features.
         **/
        template<typename T>
        std::vector<T> MelSpecComputeQuant(const std::vector<int16_t>& audioData,
                                           const float quantScale,
                                           const int quantOffset,
                                           float trainingMean = 0)
        {
            this->ComputeMelSpec(audioData, trainingMean);
            float minVal = std::numeric_limits<T>::min();
            float maxVal = std::numeric_limits<T>::max();

            std::vector<T> melSpecOut(this->m_params.m_numFbankBins);
            const size_t numFbankBins = this->m_params.m_numFbankBins;

            /* Quantize to T. */
            for (size_t k = 0; k < numFbankBins; ++k) {
                auto quantizedEnergy = std::round(((this->m_melEnergies[k]) / quantScale) + quantOffset);
                melSpecOut[k] = static_cast<T>(std::min<float>(std::max<float>(quantizedEnergy, minVal), maxVal));
            }

            return melSpecOut;
        }

        /* Constants */
        static constexpr float ms_logStep = /*logf(6.4)*/ 1.8562979903656 / 27.0;
        static constexpr float ms_freqStep = 200.0 / 3;
        static constexpr float ms_minLogHz = 1000.0;
        static constexpr float ms_minLogMel = ms_minLogHz / ms_freqStep;

    protected:
        /**
         * @brief       Project input frequency to Mel Scale.
         * @param[in]   freq          input frequency in floating point
         * @param[in]   useHTKMethod  bool to signal if HTK method is to be
         *                            used for calculation
         * @return      Mel transformed frequency in floating point
         **/
        static float MelScale(const float    freq,
                              const bool     useHTKMethod = true);

        /**
         * @brief       Inverse Mel transform - convert MEL warped frequency
         *              back to normal frequency
         * @param[in]   melFreq          Mel frequency in floating point
         * @param[in]   useHTKMethod  bool to signal if HTK method is to be
         *                            used for calculation
         * @return      Real world frequency in floating point
         **/
        static float InverseMelScale(const float melFreq,
                                     const bool  useHTKMethod = true);

        /**
         * @brief       Populates MEL energies after applying the MEL filter
         *              bank weights and adding them up to be placed into
         *              bins, according to the filter bank's first and last
         *              indices (pre-computed for each filter bank element
         *              by CreateMelFilterBank function).
         * @param[in]   fftVec                  Vector populated with FFT magnitudes
         * @param[in]   melFilterBank           2D Vector with filter bank weights
         * @param[in]   filterBankFilterFirst   Vector containing the first indices of filter bank
         *                                      to be used for each bin.
         * @param[in]   filterBankFilterLast    Vector containing the last indices of filter bank
         *                                      to be used for each bin.
         * @param[out]  melEnergies             Pre-allocated vector of MEL energies to be
         *                                      populated.
         * @return      true if successful, false otherwise
         */
        virtual bool ApplyMelFilterBank(
                std::vector<float>&                 fftVec,
                std::vector<std::vector<float>>&    melFilterBank,
                std::vector<uint32_t>&               filterBankFilterFirst,
                std::vector<uint32_t>&               filterBankFilterLast,
                std::vector<float>&                 melEnergies);

        /**
         * @brief           Converts the Mel energies for logarithmic scale
         * @param[in,out]   melEnergies 1D vector of Mel energies
         **/
        virtual void ConvertToLogarithmicScale(std::vector<float>& melEnergies);

        /**
         * @brief       Given the low and high Mel values, get the normaliser
         *              for weights to be applied when populating the filter
         *              bank.
         * @param[in]   leftMel      low Mel frequency value
         * @param[in]   rightMel     high Mel frequency value
         * @param[in]   useHTKMethod bool to signal if HTK method is to be
         *                           used for calculation
         * @return      Return float value to be applied 
         *              when populating the filter bank.
         */
        virtual float GetMelFilterBankNormaliser(
                const float&   leftMel,
                const float&   rightMel,
                const bool     useHTKMethod);

    private:
        MelSpecParams                   m_params;
        std::vector<float>              m_frame;
        std::vector<float>              m_buffer;
        std::vector<float>              m_melEnergies;
        std::vector<float>              m_windowFunc;
        std::vector<std::vector<float>> m_melFilterBank;
        std::vector<uint32_t>            m_filterBankFilterFirst;
        std::vector<uint32_t>            m_filterBankFilterLast;
        bool                            m_filterBankInitialised;
        arm::app::math::FftInstance     m_fftInstance;

        /**
         * @brief       Initialises the filter banks.
         **/
        void InitMelFilterBank();

        /**
         * @brief       Signals whether the instance of MelSpectrogram has had its
         *              required buffers initialised
         * @return      True if initialised, false otherwise
         **/
        bool IsMelFilterBankInited() const;

        /**
         * @brief       Create mel filter banks for Mel Spectrogram calculation.
         * @return      2D vector of floats
         **/
        std::vector<std::vector<float>> CreateMelFilterBank();

        /**
         * @brief       Computes the magnitude from an interleaved complex array
         **/
        void ConvertToPowerSpectrum();

    };

} /* namespace audio */
} /* namespace app */
} /* namespace arm */


#endif /* MELSPECTROGRAM_HPP */
