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
#include "PlatformMath.hpp"
#include <cstdint>
#include <vector>
#include <array>
#include <tuple>

namespace arm {
namespace app {
namespace rnn {

    using vec1D32F = std::vector<float>;
    using vec2D32F = std::vector<vec1D32F>;
    using arrHp = std::array<float, 2>;
    using math::FftInstance;
    using math::FftType;

    class FrameFeatures {
    public:
        bool m_silence{false};        /* If frame contains silence or not. */
        vec1D32F m_featuresVec{};     /* Calculated feature vector to feed to model. */
        vec1D32F m_fftX{};            /* Vector of floats arranged to represent complex numbers. */
        vec1D32F m_fftP{};            /* Vector of floats arranged to represent complex numbers. */
        vec1D32F m_Ex{};              /* Spectral band energy for audio x. */
        vec1D32F m_Ep{};              /* Spectral band energy for pitch p. */
        vec1D32F m_Exp{};             /* Correlated spectral energy between x and p. */
    };

    /**
     * @brief   RNNoise pre and post processing class based on the 2018 paper from
     *          Jan-Marc Valin. Recommended reading:
     *          - https://jmvalin.ca/demo/rnnoise/
     *          - https://arxiv.org/abs/1709.08243
     **/
    class RNNoiseProcess {
    /* Public interface */
    public:
        RNNoiseProcess();
        ~RNNoiseProcess() = default;

        /**
         * @brief        Calculates the features from a given audio buffer ready to be sent to RNNoise model.
         * @param[in]    audioData   Pointer to the floating point vector
         *                           with audio data (within the numerical
         *                           limits of int16_t type).
         * @param[in]    audioLen    Number of elements in the audio window.
         * @param[out]   features    FrameFeatures object reference.
         **/
        void PreprocessFrame(const float*   audioData,
                             size_t   audioLen,
                             FrameFeatures& features);

        /**
         * @brief        Use the RNNoise model output gain values with pre-processing features
         *               to generate audio with noise suppressed.
         * @param[in]    modelOutput   Output gain values from model.
         * @param[in]    features      Calculated features from pre-processing step.
         * @param[out]   outFrame      Output frame to be populated.
         **/
        void PostProcessFrame(vec1D32F& modelOutput, FrameFeatures& features,  vec1D32F& outFrame);


    /* Public constants */
    public:
        static constexpr uint32_t FRAME_SIZE_SHIFT{2};
        static constexpr uint32_t FRAME_SIZE{512};
        static constexpr uint32_t WINDOW_SIZE{2 * FRAME_SIZE};
        static constexpr uint32_t FREQ_SIZE{FRAME_SIZE + 1};

        static constexpr uint32_t PITCH_MIN_PERIOD{64};
        static constexpr uint32_t PITCH_MAX_PERIOD{820};
        static constexpr uint32_t PITCH_FRAME_SIZE{1024};
        static constexpr uint32_t PITCH_BUF_SIZE{PITCH_MAX_PERIOD + PITCH_FRAME_SIZE};

        static constexpr uint32_t NB_BANDS{22};
        static constexpr uint32_t CEPS_MEM{8};
        static constexpr uint32_t NB_DELTA_CEPS{6};

        static constexpr uint32_t NB_FEATURES{NB_BANDS + 3*NB_DELTA_CEPS + 2};

    /* Private functions */
    private:

        /**
         * @brief   Initialises the half window and DCT tables.
         */
        void InitTables();

        /**
         * @brief           Applies a bi-quadratic filter over the audio window.
         * @param[in]       bHp           Constant coefficient set b (arrHp type).
         * @param[in]       aHp           Constant coefficient set a (arrHp type).
         * @param[in,out]   memHpX        Coefficients populated by this function.
         * @param[in,out]   audioWindow   Floating point vector with audio data.
         **/
        void BiQuad(
            const arrHp& bHp,
            const arrHp& aHp,
            arrHp& memHpX,
            vec1D32F& audioWindow);

        /**
         * @brief        Computes features from the "filtered" audio window.
         * @param[in]    audioWindow   Floating point vector with audio data.
         * @param[out]   features      FrameFeatures object reference.
         **/
        void ComputeFrameFeatures(vec1D32F& audioWindow, FrameFeatures& features);

        /**
         * @brief        Runs analysis on the audio buffer.
         * @param[in]    audioWindow   Floating point vector with audio data.
         * @param[out]   fft           Floating point FFT vector containing real and
         *                             imaginary pairs of elements. NOTE: this vector
         *                             does not contain the mirror image (conjugates)
         *                             part of the spectrum.
         * @param[out]   energy        Computed energy for each band in the Bark scale.
         * @param[out]   analysisMem   Buffer sequentially, but partially,
         *                             populated with new audio data.
         **/
        void FrameAnalysis(
            const vec1D32F& audioWindow,
            vec1D32F& fft,
            vec1D32F& energy,
            vec1D32F& analysisMem);

        /**
         * @brief               Applies the window function, in-place, over the given
         *                      floating point buffer.
         * @param[in,out]   x   Buffer the window will be applied to.
         **/
        void ApplyWindow(vec1D32F& x);

        /**
         * @brief        Computes the FFT for a given vector.
         * @param[in]    x     Vector to compute the FFT from.
         * @param[out]   fft   Floating point FFT vector containing real and
         *                     imaginary pairs of elements. NOTE: this vector
         *                     does not contain the mirror image (conjugates)
         *                     part of the spectrum.
         **/
        void ForwardTransform(
            vec1D32F& x,
            vec1D32F& fft);

        /**
         * @brief        Computes band energy for each of the 22 Bark scale bands.
         * @param[in]    fft_X   FFT spectrum (as computed by ForwardTransform).
         * @param[out]   bandE   Vector with 22 elements populated with energy for
         *                       each band.
         **/
        void ComputeBandEnergy(const vec1D32F& fft_X, vec1D32F& bandE);

        /**
         * @brief        Computes band energy correlation.
         * @param[in]    X       FFT vector X.
         * @param[in]    P       FFT vector P.
         * @param[out]   bandC   Vector with 22 elements populated with band energy
         *                       correlation for the two input FFT vectors.
         **/
        void ComputeBandCorr(const vec1D32F& X, const vec1D32F& P, vec1D32F& bandC);

        /**
         * @brief        Performs pitch auto-correlation for a given vector for
         *               given lag.
         * @param[in]    x     Input vector.
         * @param[out]   ac    Auto-correlation output vector.
         * @param[in]    lag   Lag value.
         * @param[in]    n     Number of elements to consider for correlation
         *                     computation.
         **/
        void AutoCorr(const vec1D32F &x,
                     vec1D32F &ac,
                     size_t lag,
                     size_t n);

        /**
         * @brief       Computes pitch cross-correlation.
         * @param[in]   x          Input vector 1.
         * @param[in]   y          Input vector 2.
         * @param[out]  xCorr         Cross-correlation output vector.
         * @param[in]   len        Number of elements to consider for correlation.
         *                         computation.
         * @param[in]   maxPitch   Maximum pitch.
         **/
        void PitchXCorr(
            const vec1D32F& x,
            const vec1D32F& y,
            vec1D32F& xCorr,
            size_t len,
            size_t maxPitch);

        /**
         * @brief        Computes "Linear Predictor Coefficients".
         * @param[in]    ac    Correlation vector.
         * @param[in]    p     Number of elements of input vector to consider.
         * @param[out]   lpc   Output coefficients vector.
         **/
        void LPC(const vec1D32F& ac, int32_t p, vec1D32F& lpc);

        /**
         * @brief        Custom FIR implementation.
         * @param[in]    num   FIR coefficient vector.
         * @param[in]    N     Number of elements.
         * @param[out]   x     Vector to be be processed.
         **/
        void Fir5(const vec1D32F& num, uint32_t N, vec1D32F& x);

        /**
         * @brief           Down-sample the pitch buffer.
         * @param[in,out]   pitchBuf     Pitch buffer.
         * @param[in]       pitchBufSz   Buffer size.
         **/
        void PitchDownsample(vec1D32F& pitchBuf, size_t pitchBufSz);

        /**
         * @brief       Pitch search function.
         * @param[in]   xLP        Shifted pitch buffer input.
         * @param[in]   y          Pitch buffer input.
         * @param[in]   len        Length to search for.
         * @param[in]   maxPitch   Maximum pitch.
         * @return      pitch index.
         **/
        int PitchSearch(vec1D32F& xLp, vec1D32F& y, uint32_t len, uint32_t maxPitch);

        /**
         * @brief       Finds the "best" pitch from the buffer.
         * @param[in]   xCorr      Pitch correlation vector.
         * @param[in]   y          Pitch buffer input.
         * @param[in]   len        Length to search for.
         * @param[in]   maxPitch   Maximum pitch.
         * @return      pitch array (2 elements).
         **/
        arrHp FindBestPitch(vec1D32F& xCorr, vec1D32F& y, uint32_t len, uint32_t maxPitch);

        /**
         * @brief           Remove pitch period doubling errors.
         * @param[in,out]   pitchBuf     Pitch buffer vector.
         * @param[in]       maxPeriod    Maximum period.
         * @param[in]       minPeriod    Minimum period.
         * @param[in]       frameSize    Frame size.
         * @param[in]       pitchIdx0_   Pitch index 0.
         * @return          pitch index.
         **/
        int RemoveDoubling(
                vec1D32F& pitchBuf,
                uint32_t maxPeriod,
                uint32_t minPeriod,
                uint32_t frameSize,
                size_t pitchIdx0_);

        /**
         * @brief       Computes pitch gain.
         * @param[in]   xy   Single xy cross correlation value.
         * @param[in]   xx   Single xx auto correlation value.
         * @param[in]   yy   Single yy auto correlation value.
         * @return      Calculated pitch gain.
         **/
        float ComputePitchGain(float xy, float xx, float yy);

        /**
         * @brief        Computes DCT vector from the given input.
         * @param[in]    input    Input vector.
         * @param[out]   output   Output vector with DCT coefficients.
         **/
        void DCT(vec1D32F& input, vec1D32F& output);

        /**
         * @brief        Perform inverse fourier transform on complex spectral vector.
         * @param[out]   out      Output vector.
         * @param[in]    fftXIn   Vector of floats arranged to represent complex numbers interleaved.
         **/
        void InverseTransform(vec1D32F& out, vec1D32F& fftXIn);

        /**
         * @brief       Perform pitch filtering.
         * @param[in]   features   Object with pre-processing calculated frame features.
         * @param[in]   g          Gain values.
         **/
        void PitchFilter(FrameFeatures& features, vec1D32F& g);

        /**
         * @brief        Interpolate the band gain values.
         * @param[out]   g       Gain values.
         * @param[in]    bandE   Vector with 22 elements populated with energy for
         *                       each band.
         **/
        void InterpBandGain(vec1D32F& g, vec1D32F& bandE);

        /**
         * @brief        Create de-noised frame.
         * @param[out]   outFrame   Output vector for storing the created audio frame.
         * @param[in]    fftY       Gain adjusted complex spectral vector.
         */
        void FrameSynthesis(vec1D32F& outFrame, vec1D32F& fftY);

    /* Private objects */
    private:
        FftInstance m_fftInstReal;  /* FFT instance for real numbers */
        FftInstance m_fftInstCmplx; /* FFT instance for complex numbers */
        vec1D32F m_halfWindow;      /* Window coefficients */
        vec1D32F m_dctTable;        /* DCT table */
        vec1D32F m_analysisMem;     /* Buffer used for frame analysis */
        vec2D32F m_cepstralMem;     /* Cepstral coefficients */
        size_t m_memId;             /* memory ID */
        vec1D32F m_synthesisMem;    /* Synthesis mem (used by post-processing) */
        vec1D32F m_pitchBuf;        /* Pitch buffer */
        float m_lastGain;           /* Last gain calculated */
        int m_lastPeriod;           /* Last period calculated */
        arrHp m_memHpX;             /* HpX coefficients. */
        vec1D32F m_lastGVec;        /* Last gain vector (used by post-processing) */

        /* Constants */
        const std::array <uint32_t, NB_BANDS> m_eband5ms {
            0,  1,  2,  3,  4,  5,  6,  7,  8, 10,  12,
            14, 16, 20, 24, 28, 34, 40, 48, 60, 78, 100};

    };


} /* namespace rnn */
} /* namspace app */
} /* namespace arm */
