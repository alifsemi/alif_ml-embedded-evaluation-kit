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
#include "RNNoiseFeatureProcessor.hpp"
#include "log_macros.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace arm {
namespace app {
namespace rnn {

#define VERIFY(x)                                   \
do {                                                \
    if (!(x)) {                                     \
        printf_err("Assert failed:" #x "\n");       \
        exit(1);                                    \
    }                                               \
} while(0)

RNNoiseFeatureProcessor::RNNoiseFeatureProcessor() :
        m_halfWindow(FRAME_SIZE, 0),
        m_dctTable(NB_BANDS * NB_BANDS),
        m_analysisMem(FRAME_SIZE, 0),
        m_cepstralMem(CEPS_MEM, vec1D32F(NB_BANDS, 0)),
        m_memId{0},
        m_synthesisMem(FRAME_SIZE, 0),
        m_pitchBuf(PITCH_BUF_SIZE, 0),
        m_lastGain{0.0},
        m_lastPeriod{0},
        m_memHpX{},
        m_lastGVec(NB_BANDS, 0)
{
    constexpr uint32_t numFFt = 2 * FRAME_SIZE;
    static_assert(numFFt != 0, "Num FFT can't be 0");

    math::MathUtils::FftInitF32(numFFt, this->m_fftInstReal, FftType::real);
    math::MathUtils::FftInitF32(numFFt, this->m_fftInstCmplx, FftType::complex);
    this->InitTables();
}

void RNNoiseFeatureProcessor::PreprocessFrame(const float*   audioData,
                                              const size_t   audioLen,
                                              FrameFeatures& features)
{
    /* Note audioWindow is modified in place */
    const arrHp aHp {-1.99599, 0.99600 };
    const arrHp bHp {-2.00000, 1.00000 };

    vec1D32F audioWindow{audioData, audioData + audioLen};

    this->BiQuad(bHp, aHp, this->m_memHpX, audioWindow);
    this->ComputeFrameFeatures(audioWindow, features);
}

void RNNoiseFeatureProcessor::PostProcessFrame(vec1D32F& modelOutput, FrameFeatures& features, vec1D32F& outFrame)
{
    std::vector<float> outputBands = modelOutput;
    std::vector<float> gain(FREQ_SIZE, 0);

    if (!features.m_silence) {
        PitchFilter(features, outputBands);
        for (size_t i = 0; i < NB_BANDS; i++) {
            float alpha = .6f;
            outputBands[i] = std::max(outputBands[i], alpha * m_lastGVec[i]);
            m_lastGVec[i] = outputBands[i];
        }
        InterpBandGain(gain, outputBands);
        for (size_t i = 0; i < FREQ_SIZE; i++) {
            features.m_fftX[2 * i] *= gain[i];  /* Real. */
            features.m_fftX[2 * i + 1] *= gain[i];  /*imaginary. */

        }

    }

    FrameSynthesis(outFrame, features.m_fftX);
}

void RNNoiseFeatureProcessor::InitTables()
{
    constexpr float pi = M_PI;
    constexpr float halfPi = M_PI / 2;
    constexpr float halfPiOverFrameSz = halfPi/FRAME_SIZE;

    for (uint32_t i = 0; i < FRAME_SIZE; i++) {
        const float sinVal = math::MathUtils::SineF32(halfPiOverFrameSz * (i + 0.5f));
        m_halfWindow[i] = math::MathUtils::SineF32(halfPi * sinVal * sinVal);
    }

    for (uint32_t i = 0; i < NB_BANDS; i++) {
        for (uint32_t j = 0; j < NB_BANDS; j++) {
            m_dctTable[i * NB_BANDS + j] = math::MathUtils::CosineF32((i + 0.5f) * j * pi / NB_BANDS);
        }
        m_dctTable[i * NB_BANDS] *= math::MathUtils::SqrtF32(0.5f);
    }
}

void RNNoiseFeatureProcessor::BiQuad(
        const arrHp& bHp,
        const arrHp& aHp,
        arrHp& memHpX,
        vec1D32F& audioWindow)
{
    for (float& audioElement : audioWindow) {
        const auto xi = audioElement;
        const auto yi = audioElement + memHpX[0];
        memHpX[0] = memHpX[1] + (bHp[0] * xi - aHp[0] * yi);
        memHpX[1] = (bHp[1] * xi - aHp[1] * yi);
        audioElement = yi;
    }
}

void RNNoiseFeatureProcessor::ComputeFrameFeatures(vec1D32F& audioWindow,
                                                   FrameFeatures& features)
{
    this->FrameAnalysis(audioWindow,
                        features.m_fftX,
                        features.m_Ex,
                        this->m_analysisMem);

    float energy = 0.0;

    vec1D32F Ly(NB_BANDS, 0);
    vec1D32F p(WINDOW_SIZE, 0);
    vec1D32F pitchBuf(PITCH_BUF_SIZE >> 1, 0);

    VERIFY(PITCH_BUF_SIZE >= this->m_pitchBuf.size());
    std::copy_n(this->m_pitchBuf.begin() + FRAME_SIZE,
                PITCH_BUF_SIZE - FRAME_SIZE,
                this->m_pitchBuf.begin());

    VERIFY(FRAME_SIZE <= audioWindow.size() && PITCH_BUF_SIZE > FRAME_SIZE);
    std::copy_n(audioWindow.begin(),
                FRAME_SIZE,
                this->m_pitchBuf.begin() + PITCH_BUF_SIZE - FRAME_SIZE);

    this->PitchDownsample(pitchBuf, PITCH_BUF_SIZE);

    VERIFY(pitchBuf.size() > PITCH_MAX_PERIOD/2);
    vec1D32F xLp(pitchBuf.size() - PITCH_MAX_PERIOD/2, 0);
    std::copy_n(pitchBuf.begin() + PITCH_MAX_PERIOD/2, xLp.size(), xLp.begin());

    int pitchIdx = this->PitchSearch(xLp, pitchBuf,
            PITCH_FRAME_SIZE, (PITCH_MAX_PERIOD - (3*PITCH_MIN_PERIOD)));

    pitchIdx = this->RemoveDoubling(
                pitchBuf,
                PITCH_MAX_PERIOD,
                PITCH_MIN_PERIOD,
                PITCH_FRAME_SIZE,
                PITCH_MAX_PERIOD - pitchIdx);

    size_t stIdx = PITCH_BUF_SIZE - WINDOW_SIZE - pitchIdx;
    VERIFY((static_cast<int>(PITCH_BUF_SIZE) - static_cast<int>(WINDOW_SIZE) - pitchIdx) >= 0);
    std::copy_n(this->m_pitchBuf.begin() + stIdx, WINDOW_SIZE, p.begin());

    this->ApplyWindow(p);
    this->ForwardTransform(p, features.m_fftP);
    this->ComputeBandEnergy(features.m_fftP, features.m_Ep);
    this->ComputeBandCorr(features.m_fftX, features.m_fftP, features.m_Exp);

    for (uint32_t i = 0 ; i < NB_BANDS; ++i) {
        features.m_Exp[i] /= math::MathUtils::SqrtF32(
            0.001f + features.m_Ex[i] * features.m_Ep[i]);
    }

    vec1D32F dctVec(NB_BANDS, 0);
    this->DCT(features.m_Exp, dctVec);

    features.m_featuresVec = vec1D32F (NB_FEATURES, 0);
    for (uint32_t i = 0; i < NB_DELTA_CEPS; ++i) {
        features.m_featuresVec[NB_BANDS + 2*NB_DELTA_CEPS + i] = dctVec[i];
    }

    features.m_featuresVec[NB_BANDS + 2*NB_DELTA_CEPS] -= 1.3;
    features.m_featuresVec[NB_BANDS + 2*NB_DELTA_CEPS + 1] -= 0.9;
    features.m_featuresVec[NB_BANDS + 3*NB_DELTA_CEPS] = 0.01 * (static_cast<int>(pitchIdx) - 300);

    float logMax = -2.f;
    float follow = -2.f;
    for (uint32_t i = 0; i < NB_BANDS; ++i) {
        Ly[i] = log10f(1e-2f + features.m_Ex[i]);
        Ly[i] = std::max<float>(logMax - 7, std::max<float>(follow - 1.5, Ly[i]));
        logMax = std::max<float>(logMax, Ly[i]);
        follow = std::max<float>(follow - 1.5, Ly[i]);
        energy += features.m_Ex[i];
    }

    /* If there's no audio avoid messing up the state. */
    features.m_silence = true;
    if (energy < 0.04) {
        return;
    } else {
        features.m_silence = false;
    }

    this->DCT(Ly, features.m_featuresVec);
    features.m_featuresVec[0] -= 12.0;
    features.m_featuresVec[1] -= 4.0;

    VERIFY(CEPS_MEM > 2);
    uint32_t stIdx1 = this->m_memId < 1 ? CEPS_MEM + this->m_memId - 1 : this->m_memId - 1;
    uint32_t stIdx2 = this->m_memId < 2 ? CEPS_MEM + this->m_memId - 2 : this->m_memId - 2;
    VERIFY(stIdx1 < this->m_cepstralMem.size());
    VERIFY(stIdx2 < this->m_cepstralMem.size());
    auto ceps1 = this->m_cepstralMem[stIdx1];
    auto ceps2 = this->m_cepstralMem[stIdx2];

    /* Ceps 0 */
    for (uint32_t i = 0; i < NB_BANDS; ++i) {
        this->m_cepstralMem[this->m_memId][i] = features.m_featuresVec[i];
    }

    for (uint32_t i = 0; i < NB_DELTA_CEPS; ++i) {
        features.m_featuresVec[i] = this->m_cepstralMem[this->m_memId][i] + ceps1[i] + ceps2[i];
        features.m_featuresVec[NB_BANDS + i] = this->m_cepstralMem[this->m_memId][i] - ceps2[i];
        features.m_featuresVec[NB_BANDS + NB_DELTA_CEPS + i] =
                this->m_cepstralMem[this->m_memId][i] - 2 * ceps1[i] + ceps2[i];
    }

    /* Spectral variability features. */
    this->m_memId += 1;
    if (this->m_memId == CEPS_MEM) {
        this->m_memId = 0;
    }

    float specVariability = 0.f;

    VERIFY(this->m_cepstralMem.size() >= CEPS_MEM);
    for (size_t i = 0; i < CEPS_MEM; ++i) {
        float minDist = 1e15;
        for (size_t j = 0; j < CEPS_MEM; ++j) {
            float dist = 0.f;
            for (size_t k = 0; k < NB_BANDS; ++k) {
                VERIFY(this->m_cepstralMem[i].size() >= NB_BANDS);
                auto tmp = this->m_cepstralMem[i][k] - this->m_cepstralMem[j][k];
                dist += tmp * tmp;
            }

            if (j != i) {
                minDist = std::min<float>(minDist, dist);
            }
        }
        specVariability += minDist;
    }

    VERIFY(features.m_featuresVec.size() >= NB_BANDS + 3 * NB_DELTA_CEPS + 1);
    features.m_featuresVec[NB_BANDS + 3 * NB_DELTA_CEPS + 1] = specVariability / CEPS_MEM - 2.1;
}

void RNNoiseFeatureProcessor::FrameAnalysis(
    const vec1D32F& audioWindow,
    vec1D32F& fft,
    vec1D32F& energy,
    vec1D32F& analysisMem)
{
    vec1D32F x(WINDOW_SIZE, 0);

    /* Move old audio down and populate end with latest audio window. */
    VERIFY(x.size() >= FRAME_SIZE && analysisMem.size() >= FRAME_SIZE);
    VERIFY(audioWindow.size() >= FRAME_SIZE);

    std::copy_n(analysisMem.begin(), FRAME_SIZE, x.begin());
    std::copy_n(audioWindow.begin(), x.size() - FRAME_SIZE, x.begin() + FRAME_SIZE);
    std::copy_n(audioWindow.begin(), FRAME_SIZE, analysisMem.begin());

    this->ApplyWindow(x);

    /* Calculate FFT. */
    ForwardTransform(x, fft);

    /* Compute band energy. */
    ComputeBandEnergy(fft, energy);
}

void RNNoiseFeatureProcessor::ApplyWindow(vec1D32F& x)
{
    if (WINDOW_SIZE != x.size()) {
        printf_err("Invalid size for vector to be windowed\n");
        return;
    }

    VERIFY(this->m_halfWindow.size() >= FRAME_SIZE);

    /* Multiply input by sinusoidal function. */
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        x[i] *= this->m_halfWindow[i];
        x[WINDOW_SIZE - 1 - i] *= this->m_halfWindow[i];
    }
}

void RNNoiseFeatureProcessor::ForwardTransform(
    vec1D32F& x,
    vec1D32F& fft)
{
    /* The input vector can be modified by the fft function. */
    fft.reserve(x.size() + 2);
    fft.resize(x.size() + 2, 0);
    math::MathUtils::FftF32(x, fft, this->m_fftInstReal);

    /* Normalise. */
    for (auto& f : fft) {
        f /= this->m_fftInstReal.m_fftLen;
    }

    /* Place the last freq element correctly */
    fft[fft.size()-2] = fft[1];
    fft[1] = 0;

    /* NOTE: We don't truncate out FFT vector as it already contains only the
     * first half of the FFT's. The conjugates are not present. */
}

void RNNoiseFeatureProcessor::ComputeBandEnergy(const vec1D32F& fftX, vec1D32F& bandE)
{
    bandE = vec1D32F(NB_BANDS, 0);

    VERIFY(this->m_eband5ms.size() >= NB_BANDS);
    for (uint32_t i = 0; i < NB_BANDS - 1; i++) {
        const auto bandSize = (this->m_eband5ms[i + 1] - this->m_eband5ms[i])
                              << FRAME_SIZE_SHIFT;

        for (uint32_t j = 0; j < bandSize; j++) {
            const auto frac = static_cast<float>(j) / bandSize;
            const auto idx = (this->m_eband5ms[i] << FRAME_SIZE_SHIFT) + j;

            auto tmp = fftX[2 * idx] * fftX[2 * idx]; /* Real part */
            tmp += fftX[2 * idx + 1] * fftX[2 * idx + 1]; /* Imaginary part */

            bandE[i] += (1 - frac) * tmp;
            bandE[i + 1] += frac * tmp;
        }
    }
    bandE[0] *= 2;
    bandE[NB_BANDS - 1] *= 2;
}

void RNNoiseFeatureProcessor::ComputeBandCorr(const vec1D32F& X, const vec1D32F& P, vec1D32F& bandC)
{
    bandC = vec1D32F(NB_BANDS, 0);
    VERIFY(this->m_eband5ms.size() >= NB_BANDS);

    for (uint32_t i = 0; i < NB_BANDS - 1; i++) {
        const auto bandSize = (this->m_eband5ms[i + 1] - this->m_eband5ms[i]) << FRAME_SIZE_SHIFT;

        for (uint32_t j = 0; j < bandSize; j++) {
            const auto frac = static_cast<float>(j) / bandSize;
            const auto idx = (this->m_eband5ms[i] << FRAME_SIZE_SHIFT) + j;

            auto tmp = X[2 * idx] * P[2 * idx]; /* Real part */
            tmp += X[2 * idx + 1] * P[2 * idx + 1]; /* Imaginary part */

            bandC[i] += (1 - frac) * tmp;
            bandC[i + 1] += frac * tmp;
        }
    }
    bandC[0] *= 2;
    bandC[NB_BANDS - 1] *= 2;
}

void RNNoiseFeatureProcessor::DCT(vec1D32F& input, vec1D32F& output)
{
    VERIFY(this->m_dctTable.size() >= NB_BANDS * NB_BANDS);
    for (uint32_t i = 0; i < NB_BANDS; ++i) {
        float sum = 0;

        for (uint32_t j = 0, k = 0; j < NB_BANDS; ++j, k += NB_BANDS) {
            sum += input[j] * this->m_dctTable[k + i];
        }
        output[i] = sum * math::MathUtils::SqrtF32(2.0/22);
    }
}

void RNNoiseFeatureProcessor::PitchDownsample(vec1D32F& pitchBuf, size_t pitchBufSz) {
    for (size_t i = 1; i < (pitchBufSz >> 1); ++i) {
        pitchBuf[i] = 0.5 * (
                        0.5 * (this->m_pitchBuf[2 * i - 1] + this->m_pitchBuf[2 * i + 1])
                            + this->m_pitchBuf[2 * i]);
    }

    pitchBuf[0] = 0.5*(0.5*(this->m_pitchBuf[1]) + this->m_pitchBuf[0]);

    vec1D32F ac(5, 0);
    size_t numLags = 4;

    this->AutoCorr(pitchBuf, ac, numLags, pitchBufSz >> 1);

    /* Noise floor -40db */
    ac[0] *= 1.0001;

    /* Lag windowing. */
    for (size_t i = 1; i < numLags + 1; ++i) {
        ac[i] -= ac[i] * (0.008 * i) * (0.008 * i);
    }

    vec1D32F lpc(numLags, 0);
    this->LPC(ac, numLags, lpc);

    float tmp = 1.0;
    for (size_t i = 0; i < numLags; ++i) {
        tmp = 0.9f * tmp;
        lpc[i] = lpc[i] * tmp;
    }

    vec1D32F lpc2(numLags + 1, 0);
    float c1 = 0.8;

    /* Add a zero. */
    lpc2[0] = lpc[0] + 0.8;
    lpc2[1] = lpc[1] + (c1 * lpc[0]);
    lpc2[2] = lpc[2] + (c1 * lpc[1]);
    lpc2[3] = lpc[3] + (c1 * lpc[2]);
    lpc2[4] = (c1 * lpc[3]);

    this->Fir5(lpc2, pitchBufSz >> 1, pitchBuf);
}

int RNNoiseFeatureProcessor::PitchSearch(vec1D32F& xLp, vec1D32F& y, uint32_t len, uint32_t maxPitch) {
    uint32_t lag = len + maxPitch;
    vec1D32F xLp4(len >> 2, 0);
    vec1D32F yLp4(lag >> 2, 0);
    vec1D32F xCorr(maxPitch >> 1, 0);

    /* Downsample by 2 again. */
    for (size_t j = 0; j < (len >> 2); ++j) {
        xLp4[j] = xLp[2*j];
    }
    for (size_t j = 0; j < (lag >> 2); ++j) {
        yLp4[j] = y[2*j];
    }

    this->PitchXCorr(xLp4, yLp4, xCorr, len >> 2, maxPitch >> 2);

    /* Coarse search with 4x decimation. */
    arrHp bestPitch = this->FindBestPitch(xCorr, yLp4, len >> 2, maxPitch >> 2);

    /* Finer search with 2x decimation. */
    const int maxIdx = (maxPitch >> 1);
    for (int i = 0; i < maxIdx; ++i) {
        xCorr[i] = 0;
        if (std::abs(i - 2*bestPitch[0]) > 2 and std::abs(i - 2*bestPitch[1]) > 2) {
            continue;
        }
        float sum = 0;
        for (size_t j = 0; j < len >> 1; ++j) {
            sum += xLp[j] * y[i+j];
        }

        xCorr[i] = std::max(-1.0f, sum);
    }

    bestPitch = this->FindBestPitch(xCorr, y, len >> 1, maxPitch >> 1);

    int offset;
    /* Refine by pseudo-interpolation. */
    if ( 0 < bestPitch[0] && bestPitch[0] < ((maxPitch >> 1) - 1)) {
        float a = xCorr[bestPitch[0] - 1];
        float b = xCorr[bestPitch[0]];
        float c = xCorr[bestPitch[0] + 1];

        if ( (c-a) > 0.7*(b-a) ) {
            offset = 1;
        } else if ( (a-c) > 0.7*(b-c) ) {
            offset = -1;
        } else {
            offset = 0;
        }
    } else {
        offset = 0;
    }

    return 2*bestPitch[0] - offset;
}

arrHp RNNoiseFeatureProcessor::FindBestPitch(vec1D32F& xCorr, vec1D32F& y, uint32_t len, uint32_t maxPitch)
{
    float Syy = 1;
    arrHp bestNum {-1, -1};
    arrHp bestDen {0, 0};
    arrHp bestPitch {0, 1};

    for (size_t j = 0; j < len; ++j) {
        Syy += (y[j] * y[j]);
    }

    for (size_t i = 0; i < maxPitch; ++i ) {
        if (xCorr[i] > 0) {
            float xCorr16 = xCorr[i] * 1e-12f;  /* Avoid problems when squaring. */

            float num = xCorr16 * xCorr16;
            if (num*bestDen[1] > bestNum[1]*Syy) {
                if (num*bestDen[0] > bestNum[0]*Syy) {
                    bestNum[1] = bestNum[0];
                    bestDen[1] = bestDen[0];
                    bestPitch[1] = bestPitch[0];
                    bestNum[0] = num;
                    bestDen[0] = Syy;
                    bestPitch[0] = i;
                } else {
                    bestNum[1] = num;
                    bestDen[1] = Syy;
                    bestPitch[1] = i;
                }
            }
        }

        Syy += (y[i+len]*y[i+len]) - (y[i]*y[i]);
        Syy = std::max(1.0f, Syy);
    }

    return bestPitch;
}

int RNNoiseFeatureProcessor::RemoveDoubling(
    vec1D32F& pitchBuf,
    uint32_t maxPeriod,
    uint32_t minPeriod,
    uint32_t frameSize,
    size_t pitchIdx0_)
{
    constexpr std::array<size_t, 16> secondCheck {0, 0, 3, 2, 3, 2, 5, 2, 3, 2, 3, 2, 5, 2, 3, 2};
    uint32_t minPeriod0 = minPeriod;
    float lastPeriod = static_cast<float>(this->m_lastPeriod)/2;
    float lastGain = static_cast<float>(this->m_lastGain);

    maxPeriod /= 2;
    minPeriod /= 2;
    pitchIdx0_ /= 2;
    frameSize /= 2;
    uint32_t xStart = maxPeriod;

    if (pitchIdx0_ >= maxPeriod) {
        pitchIdx0_ = maxPeriod - 1;
    }

    size_t pitchIdx  = pitchIdx0_;
    const size_t pitchIdx0 = pitchIdx0_;

    float xx = 0;
    for ( size_t i = xStart; i < xStart+frameSize; ++i) {
        xx += (pitchBuf[i] * pitchBuf[i]);
    }

    float xy = 0;
    for ( size_t i = xStart; i < xStart+frameSize; ++i) {
        xy += (pitchBuf[i] * pitchBuf[i-pitchIdx0]);
    }

    vec1D32F yyLookup (maxPeriod+1, 0);
    yyLookup[0] = xx;
    float yy = xx;

    for ( size_t i = 1; i < yyLookup.size(); ++i) {
        yy = yy + (pitchBuf[xStart-i] * pitchBuf[xStart-i]) -
                (pitchBuf[xStart+frameSize-i] * pitchBuf[xStart+frameSize-i]);
        yyLookup[i] = std::max(0.0f, yy);
    }

    yy = yyLookup[pitchIdx0];
    float bestXy = xy;
    float bestYy = yy;

    float g = this->ComputePitchGain(xy, xx, yy);
    float g0 = g;

    /* Look for any pitch at pitchIndex/k. */
    for ( size_t k = 2; k < 16; ++k) {
        size_t pitchIdx1 = (2*pitchIdx0+k) / (2*k);
        if (pitchIdx1 < minPeriod) {
            break;
        }

        size_t pitchIdx1b;
        /* Look for another strong correlation at T1b. */
        if (k == 2) {
            if ((pitchIdx1 + pitchIdx0) > maxPeriod) {
                pitchIdx1b = pitchIdx0;
            } else {
                pitchIdx1b = pitchIdx0 + pitchIdx1;
            }
        } else {
            pitchIdx1b = (2*(secondCheck[k])*pitchIdx0 + k) / (2*k);
        }

        xy = 0;
        for ( size_t i = xStart; i < xStart+frameSize; ++i) {
            xy += (pitchBuf[i] * pitchBuf[i-pitchIdx1]);
        }

        float xy2 = 0;
        for ( size_t i = xStart; i < xStart+frameSize; ++i) {
            xy2 += (pitchBuf[i] * pitchBuf[i-pitchIdx1b]);
        }
        xy = 0.5f * (xy + xy2);
        VERIFY(pitchIdx1b < maxPeriod+1);
        yy = 0.5f * (yyLookup[pitchIdx1] + yyLookup[pitchIdx1b]);

        float g1 = this->ComputePitchGain(xy, xx, yy);

        float cont;
        if (std::abs(pitchIdx1-lastPeriod) <= 1) {
            cont = lastGain;
        } else if (std::abs(pitchIdx1-lastPeriod) <= 2 and 5*k*k < pitchIdx0) {
            cont = 0.5f*lastGain;
        } else {
            cont = 0.0f;
        }

        float thresh = std::max(0.3, 0.7*g0-cont);

        /* Bias against very high pitch (very short period) to avoid false-positives
         * due to short-term correlation */
        if (pitchIdx1 < 3*minPeriod) {
            thresh = std::max(0.4, 0.85*g0-cont);
        } else if (pitchIdx1 < 2*minPeriod) {
            thresh = std::max(0.5, 0.9*g0-cont);
        }
        if (g1 > thresh) {
            bestXy = xy;
            bestYy = yy;
            pitchIdx = pitchIdx1;
            g = g1;
        }
    }

    bestXy = std::max(0.0f, bestXy);
    float pg;
    if (bestYy <= bestXy) {
        pg = 1.0;
    } else {
        pg = bestXy/(bestYy+1);
    }

    std::array<float, 3> xCorr {0};
    for ( size_t k = 0; k < 3; ++k ) {
        for ( size_t i = xStart; i < xStart+frameSize; ++i) {
            xCorr[k] += (pitchBuf[i] * pitchBuf[i-(pitchIdx+k-1)]);
        }
    }

    size_t offset;
    if ((xCorr[2]-xCorr[0]) > 0.7*(xCorr[1]-xCorr[0])) {
        offset = 1;
    } else if ((xCorr[0]-xCorr[2]) > 0.7*(xCorr[1]-xCorr[2])) {
        offset = -1;
    } else {
        offset = 0;
    }

    if (pg > g) {
        pg = g;
    }

    pitchIdx0_ = 2*pitchIdx + offset;

    if (pitchIdx0_ < minPeriod0) {
        pitchIdx0_ = minPeriod0;
    }

    this->m_lastPeriod = pitchIdx0_;
    this->m_lastGain = pg;

    return this->m_lastPeriod;
}

float RNNoiseFeatureProcessor::ComputePitchGain(float xy, float xx, float yy)
{
    return xy / math::MathUtils::SqrtF32(1+xx*yy);
}

void RNNoiseFeatureProcessor::AutoCorr(
    const vec1D32F& x,
    vec1D32F& ac,
    size_t lag,
    size_t n)
{
    if (n < lag) {
        printf_err("Invalid parameters for AutoCorr\n");
        return;
    }

    auto fastN = n - lag;

    /* Auto-correlation - can be done by PlatformMath functions */
    this->PitchXCorr(x, x, ac, fastN, lag + 1);

    /* Modify auto-correlation by summing with auto-correlation for different lags. */
    for (size_t k = 0; k < lag + 1; k++) {
        float d = 0;
        for (size_t i = k + fastN; i < n; i++) {
            d += x[i] * x[i - k];
        }
        ac[k] += d;
    }
}


void RNNoiseFeatureProcessor::PitchXCorr(
    const vec1D32F& x,
    const vec1D32F& y,
    vec1D32F& xCorr,
    size_t len,
    size_t maxPitch)
{
    for (size_t i = 0; i < maxPitch; i++) {
        float sum = 0;
        for (size_t j = 0; j < len; j++) {
            sum += x[j] * y[i + j];
        }
        xCorr[i] = sum;
    }
}

/* Linear predictor coefficients */
void RNNoiseFeatureProcessor::LPC(
    const vec1D32F& correlation,
    int32_t p,
    vec1D32F& lpc)
{
    auto error = correlation[0];

    if (error != 0) {
        for (int i = 0; i < p; i++) {

            /* Sum up this iteration's reflection coefficient */
            float rr = 0;
            for (int j = 0; j < i; j++) {
                rr += lpc[j] * correlation[i - j];
            }

            rr += correlation[i + 1];
            auto r = -rr / error;

            /* Update LP coefficients and total error */
            lpc[i] = r;
            for (int j = 0; j < ((i + 1) >> 1); j++) {
                auto tmp1 = lpc[j];
                auto tmp2 = lpc[i - 1 - j];
                lpc[j] = tmp1 + (r * tmp2);
                lpc[i - 1 - j] = tmp2 + (r * tmp1);
            }

            error = error - (r * r * error);

            /* Bail out once we get 30dB gain */
            if (error < (0.001 * correlation[0])) {
                break;
            }
        }
    }
}

void RNNoiseFeatureProcessor::Fir5(
    const vec1D32F &num,
    uint32_t N,
    vec1D32F &x)
{
    auto num0 = num[0];
    auto num1 = num[1];
    auto num2 = num[2];
    auto num3 = num[3];
    auto num4 = num[4];
    auto mem0 = 0;
    auto mem1 = 0;
    auto mem2 = 0;
    auto mem3 = 0;
    auto mem4 = 0;
    for (uint32_t i = 0; i < N; i++)
    {
        auto sum_ = x[i] +  (num0 * mem0) + (num1 * mem1) +
                    (num2 * mem2) + (num3 * mem3) + (num4 * mem4);
        mem4 = mem3;
        mem3 = mem2;
        mem2 = mem1;
        mem1 = mem0;
        mem0 = x[i];
        x[i] = sum_;
    }
}

void RNNoiseFeatureProcessor::PitchFilter(FrameFeatures &features, vec1D32F &gain) {
    std::vector<float> r(NB_BANDS, 0);
    std::vector<float> rf(FREQ_SIZE, 0);
    std::vector<float> newE(NB_BANDS);

    for (size_t i = 0; i < NB_BANDS; i++) {
        if (features.m_Exp[i] > gain[i]) {
            r[i] = 1;
        } else {


            r[i] = std::pow(features.m_Exp[i], 2) * (1 - std::pow(gain[i], 2)) /
                   (.001 + std::pow(gain[i], 2) * (1 - std::pow(features.m_Exp[i], 2)));
        }


        r[i] = math::MathUtils::SqrtF32(std::min(1.0f, std::max(0.0f, r[i])));
        r[i] *= math::MathUtils::SqrtF32(features.m_Ex[i] / (1e-8f + features.m_Ep[i]));
    }

    InterpBandGain(rf, r);
    for (size_t i = 0; i < FREQ_SIZE - 1; i++) {
        features.m_fftX[2 * i] += rf[i] * features.m_fftP[2 * i];  /* Real. */
        features.m_fftX[2 * i + 1] += rf[i] * features.m_fftP[2 * i + 1];  /* Imaginary. */

    }
    ComputeBandEnergy(features.m_fftX, newE);
    std::vector<float> norm(NB_BANDS);
    std::vector<float> normf(FRAME_SIZE, 0);
    for (size_t i = 0; i < NB_BANDS; i++) {
        norm[i] = math::MathUtils::SqrtF32(features.m_Ex[i] / (1e-8f + newE[i]));
    }

    InterpBandGain(normf, norm);
    for (size_t i = 0; i < FREQ_SIZE - 1; i++) {
        features.m_fftX[2 * i] *= normf[i];  /* Real. */
        features.m_fftX[2 * i + 1] *= normf[i];  /* Imaginary. */

    }
}

void RNNoiseFeatureProcessor::FrameSynthesis(vec1D32F& outFrame, vec1D32F& fftY) {
    std::vector<float> x(WINDOW_SIZE, 0);
    InverseTransform(x, fftY);
    ApplyWindow(x);
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        outFrame[i] = x[i] + m_synthesisMem[i];
    }
    memcpy((m_synthesisMem.data()), &x[FRAME_SIZE], FRAME_SIZE*sizeof(float));
}

void RNNoiseFeatureProcessor::InterpBandGain(vec1D32F& g, vec1D32F& bandE) {
    for (size_t i = 0; i < NB_BANDS - 1; i++) {
        int bandSize = (m_eband5ms[i + 1] - m_eband5ms[i]) << FRAME_SIZE_SHIFT;
        for (int j = 0; j < bandSize; j++) {
            float frac = static_cast<float>(j) / bandSize;
            g[(m_eband5ms[i] << FRAME_SIZE_SHIFT) + j] = (1 - frac) * bandE[i] + frac * bandE[i + 1];
        }
    }
}

void RNNoiseFeatureProcessor::InverseTransform(vec1D32F& out, vec1D32F& fftXIn) {

    std::vector<float> x(WINDOW_SIZE * 2);  /* This is complex. */
    vec1D32F newFFT;  /* This is complex. */

    size_t i;
    for (i = 0; i < FREQ_SIZE * 2; i++) {
        x[i] = fftXIn[i];
    }
    for (i = FREQ_SIZE; i < WINDOW_SIZE; i++) {
        x[2 * i] = x[2 * (WINDOW_SIZE - i)];  /* Real. */
        x[2 * i + 1] = -x[2 * (WINDOW_SIZE - i) + 1];  /* Imaginary. */
    }

    constexpr uint32_t numFFt = 2 * FRAME_SIZE;
    static_assert(numFFt != 0, "numFFt cannot be 0!");

    vec1D32F fftOut = vec1D32F(x.size(), 0);
    math::MathUtils::FftF32(x,fftOut, m_fftInstCmplx);

    /* Normalize. */
    for (auto &f: fftOut) {
        f /= numFFt;
    }

    out[0] = WINDOW_SIZE * fftOut[0];  /* Real. */
    for (i = 1; i < WINDOW_SIZE; i++) {
        out[i] = WINDOW_SIZE * fftOut[(WINDOW_SIZE * 2) - (2 * i)];  /* Real. */
    }
}


} /* namespace rnn */
} /* namespace app */
} /* namspace arm */
