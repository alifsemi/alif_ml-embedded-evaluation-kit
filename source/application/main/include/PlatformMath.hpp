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
#ifndef PLATFORM_MATH_HPP
#define PLATFORM_MATH_HPP

#include "hal.h"

/* See if ARM DSP functions can be used. */
#if PLATFORM_HAL == PLATFORM_CORTEX_M_BAREMETAL
    #if defined(__DSP_PRESENT) && (__DSP_PRESENT == 1U)

        #define ARM_DSP_AVAILABLE   (1U)
        #include "arm_math.h"
        #define M_PI    (PI)

    #endif /* defined(__DSP_PRESENT) && (__DSP_PRESENT == 1U) */
#endif /* PLATFORM_HAL == PLATFORM_CORTEX_M_BAREMETAL */

#include <vector>

namespace arm {
namespace app {
namespace math {

    struct FftInstance {
#if ARM_DSP_AVAILABLE
        arm_rfft_fast_instance_f32 instance;
#endif
        bool initialised = false;
    };

    /* Class to provide Math functions like FFT, mean, stddev etc.
     * This will allow other classes, functions to be independent of
     * #if definition checks and provide a cleaner API. Also, it will
     * consolidate all arm math functions used in one place and make
     * them easier to test. */
    class MathUtils {

    public:
        /**
         * @brief       Get the cosine value of the argument in floating point.
         * @param[in]   radians   Angle in radians.
         * @return      Cosine value (floating point).
         */
        static float CosineF32(float radians);

        /**
         * @brief       Get the square root of the argument in floating point.
         * @param[in]   input   Value to compute square root of.
         * @return      Square root (floating point) value.
         */
        static float SqrtF32(float input);

        /**
         * @brief       Gets the mean of a floating point array of elements.
         * @param[in]   ptrSrc   Pointer to the first element.
         * @param[in]   srcLen   Number of elements in the array/vector.
         * @return      Average value.
         */
        static float MeanF32(float* ptrSrc, uint32_t srcLen);

        /**
         * @brief       Gets the standard deviation of a floating point array
         *              of elements.
         * @param[in]   ptrSrc   Pointer to the first element.
         * @param[in]   srcLen   Number of elements in the array/vector.
         * @param[in]   mean     Pre-computed mean value.
         * @return      Standard deviation value.
         */
        static float StdDevF32(float* ptrSrc, uint32_t srcLen,
                               float mean);

        /**
         * @brief       Initialises the internal FFT structures (if available
         *              for the platform). This function should be called
         *              prior to Fft32 function call if built with ARM DSP functions.
         * @param[in]   fftLen        Requested length of the FFT.
         * @param[in]   fftInstance   FFT instance struct to use.
         * @return      true if successful, false otherwise.
         */
        static bool FftInitF32(const uint16_t fftLen, arm::app::math::FftInstance& fftInstance);

        /**
         * @brief       Computes the FFT for the input vector.
         * @param[in]   input       Floating point vector of input elements
         * @param[out]  fftOutput   Output buffer to be populated by computed FFTs.
         * @param[in]   fftInstance FFT instance struct to use.
         */
        static void FftF32(std::vector<float>& input,
                           std::vector<float>& fftOutput,
                           arm::app::math::FftInstance& fftInstance);

        /**
         * @brief       Computes the natural logarithms of input floating point
         *              vector
         * @param[in]   input    Floating point input vector
         * @param[out]  output   Pre-allocated buffer to be populated with
         *                       natural log values of each input element.
         */
        static void VecLogarithmF32(std::vector <float>& input,
                                    std::vector <float>& output);

        /**
         * @brief       Computes the dot product of two 1D floating point
         *              vectors.
         *              result = sum(srcA[0]*srcB[0] + srcA[1]*srcB[1] + ..)
         * @param[in]   srcPtrA   Pointer to the first element of first
         *                        array.
         * @param[in]   srcPtrB   Pointer to the first element of second
         *                        array.
         * @param[in]   srcLen    Number of elements in the array/vector.
         * @return      Dot product.
         */
        static float DotProductF32(float* srcPtrA, float* srcPtrB,
                                   const uint32_t srcLen);

        /**
         * @brief       Computes the squared magnitude of floating point
         *              complex number array.
         * @param[in]   ptrSrc   Pointer to the first element of input
         *                       array.
         * @param[in]   srcLen   Number of elements in the array/vector.
         * @param[out]  ptrDst   Output buffer to be populated.
         * @param[in]   dstLen   Output buffer len (for sanity check only).
         * @return      true if successful, false otherwise.
         */
        static bool ComplexMagnitudeSquaredF32(float* ptrSrc,
                                               const uint32_t srcLen,
                                               float* ptrDst,
                                               const uint32_t dstLen);

    };
} /* namespace math */
} /* namespace app */
} /* namespace arm */

#endif /* PLATFORM_MATH_HPP */