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

#if 0 == ARM_DSP_AVAILABLE
    #include <cmath>
    #include <numeric>
#endif /* 0 == ARM_DSP_AVAILABLE */

namespace arm {
namespace app {
namespace math {

    float MathUtils::CosineF32(float radians)
    {
#if ARM_DSP_AVAILABLE
        return arm_cos_f32(radians);
#else /* ARM_DSP_AVAILABLE */
        return cos(radians);
#endif /* ARM_DSP_AVAILABLE */
    }

    float MathUtils::SqrtF32(float input)
    {
#if ARM_DSP_AVAILABLE
        float output = 0.f;
        arm_sqrt_f32(input, &output);
        return output;
#else /* ARM_DSP_AVAILABLE */
        return sqrtf(input);
#endif /* ARM_DSP_AVAILABLE */
    }

    float MathUtils::MeanF32(float* ptrSrc, const uint32_t srcLen)
    {
        if (!srcLen) {
            return 0.f;
        }

#if ARM_DSP_AVAILABLE
        float result = 0.f;
        arm_mean_f32(ptrSrc, srcLen, &result);
        return result;
#else /* ARM_DSP_AVAILABLE */
        float acc = std::accumulate(ptrSrc, ptrSrc + srcLen, 0.0);
        return acc/srcLen;
#endif /* ARM_DSP_AVAILABLE */
    }

    float MathUtils::StdDevF32(float* ptrSrc, const uint32_t srcLen,
                               const float mean)
    {
        if (!srcLen) {
            return 0.f;
        }
#if ARM_DSP_AVAILABLE
        /**
         * Note Standard deviation calculation can be off
         * by > 0.01 but less than < 0.1, according to
         * preliminary findings.
         **/
        UNUSED(mean);
        float stdDev = 0;
        arm_std_f32(ptrSrc, srcLen, &stdDev);
        return stdDev;
#else /* ARM_DSP_AVAILABLE */
        auto VarianceFunction = [=](float acc, const float value) {
            return acc + (((value - mean) * (value - mean))/ srcLen);
        };

        float acc = std::accumulate(ptrSrc, ptrSrc + srcLen, 0.0,
                                    VarianceFunction);

        return sqrtf(acc);
#endif /* ARM_DSP_AVAILABLE */
    }

    bool MathUtils::FftInitF32(const uint16_t fftLen, arm::app::math::FftInstance& fftInstance)
    {
#if ARM_DSP_AVAILABLE
        if (!fftInstance.initialised) {
            arm_status status = arm_rfft_fast_init_f32(&fftInstance.instance, fftLen);

            if (ARM_MATH_SUCCESS != status) {
                return false;
            }
            fftInstance.initialised = true;
        }
#else
        UNUSED(fftLen);
        UNUSED(fftInstance);
#endif /* ARM_DSP_AVAILABLE */
        return true;
    }

    void MathUtils::FftF32(std::vector<float>& input,
                           std::vector<float>& fftOutput,
                           arm::app::math::FftInstance& fftInstance)
    {
#if ARM_DSP_AVAILABLE
        arm_rfft_fast_f32(&fftInstance.instance, input.data(), fftOutput.data(), 0);
#else
        UNUSED(fftInstance);
        const int inputLength = input.size();

        for (int k = 0; k <= inputLength / 2; k++) {
            float sumReal = 0, sumImag = 0;

            for (int t = 0; t < inputLength; t++) {
                float angle = 2 * M_PI * t * k / inputLength;
                sumReal += input[t] * cosf(angle);
                sumImag += -input[t] * sinf(angle);
            }

            /* Arrange output to [real0, realN/2, real1, im1, real2, im2, ...] */
            if (k == 0) {
                fftOutput[0] = sumReal;
            } else if (k == inputLength / 2) {
                fftOutput[1] = sumReal;
            } else {
                fftOutput[k*2] = sumReal;
                fftOutput[k*2 + 1] = sumImag;
            };
        }
#endif /* ARM_DSP_AVAILABLE */
    }

    void MathUtils::VecLogarithmF32(std::vector <float>& input,
                                    std::vector <float>& output)
    {
#if ARM_DSP_AVAILABLE
        arm_vlog_f32(input.data(), output.data(),
                     output.size());
#else /* ARM_DSP_AVAILABLE */
        for (auto in = input.begin(), out = output.begin();
                in != input.end(); ++in, ++out) {
            *out = logf(*in);
        }
#endif /* ARM_DSP_AVAILABLE */
    }

    float MathUtils::DotProductF32(float* srcPtrA, float* srcPtrB,
                                   const uint32_t srcLen)
    {
        float output = 0.f;

#if ARM_DSP_AVAILABLE
        arm_dot_prod_f32(srcPtrA, srcPtrB, srcLen, &output);
#else /* ARM_DSP_AVAILABLE */
        for (uint32_t i = 0; i < srcLen; ++i) {
            output += *srcPtrA++ * *srcPtrB++;
        }
#endif /* ARM_DSP_AVAILABLE */

        return output;
    }

    bool MathUtils::ComplexMagnitudeSquaredF32(float* ptrSrc,
                                               const uint32_t srcLen,
                                               float* ptrDst,
                                               const uint32_t dstLen)
    {
        if (dstLen < srcLen/2) {
            printf_err("dstLen must be greater than srcLen/2");
            return false;
        }

#if ARM_DSP_AVAILABLE
        arm_cmplx_mag_squared_f32(ptrSrc, ptrDst, srcLen/2);
#else /* ARM_DSP_AVAILABLE */
        for (uint32_t j = 0; j < srcLen; ++j) {
            const float real = *ptrSrc++;
            const float im = *ptrSrc++;
            *ptrDst++ = real*real + im*im;
        }
#endif /* ARM_DSP_AVAILABLE */
        return true;
    }

} /* namespace math */
} /* namespace app */
} /* namespace arm */