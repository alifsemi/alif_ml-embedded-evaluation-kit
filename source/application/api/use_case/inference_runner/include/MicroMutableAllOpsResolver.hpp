/*
 * SPDX-FileCopyrightText: Copyright 2023 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
#ifndef INF_RUNNER_MICRO_MUTABLE_ALLOPS_RESOLVER_HPP
#define INF_RUNNER_MICRO_MUTABLE_ALLOPS_RESOLVER_HPP

#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>

constexpr int kNumberOperators = 97;

namespace arm {
namespace app {

    /* Create our own AllOpsResolver by adding all Ops to MicroMutableOpResolver. */
    inline tflite::MicroMutableOpResolver<kNumberOperators> CreateAllOpsResolver() {
        tflite::MicroMutableOpResolver<kNumberOperators> mutableAllOpResolver;

        mutableAllOpResolver.AddAbs();
        mutableAllOpResolver.AddAdd();
        mutableAllOpResolver.AddAddN();
        mutableAllOpResolver.AddArgMax();
        mutableAllOpResolver.AddArgMin();
        mutableAllOpResolver.AddAssignVariable();
        mutableAllOpResolver.AddAveragePool2D();
        mutableAllOpResolver.AddBatchToSpaceNd();
        mutableAllOpResolver.AddBroadcastArgs();
        mutableAllOpResolver.AddBroadcastTo();
        mutableAllOpResolver.AddCallOnce();
        mutableAllOpResolver.AddCast();
        mutableAllOpResolver.AddCeil();
        mutableAllOpResolver.AddCircularBuffer();
        mutableAllOpResolver.AddConcatenation();
        mutableAllOpResolver.AddConv2D();
        mutableAllOpResolver.AddCos();
        mutableAllOpResolver.AddCumSum();
        mutableAllOpResolver.AddDepthToSpace();
        mutableAllOpResolver.AddDepthwiseConv2D();
        mutableAllOpResolver.AddDequantize();
        mutableAllOpResolver.AddDetectionPostprocess();
        mutableAllOpResolver.AddDiv();
        mutableAllOpResolver.AddElu();
        mutableAllOpResolver.AddEqual();
        mutableAllOpResolver.AddEthosU();
        mutableAllOpResolver.AddExp();
        mutableAllOpResolver.AddExpandDims();
        mutableAllOpResolver.AddFill();
        mutableAllOpResolver.AddFloor();
        mutableAllOpResolver.AddFloorDiv();
        mutableAllOpResolver.AddFloorMod();
        mutableAllOpResolver.AddFullyConnected();
        mutableAllOpResolver.AddGather();
        mutableAllOpResolver.AddGatherNd();
        mutableAllOpResolver.AddGreater();
        mutableAllOpResolver.AddGreaterEqual();
        mutableAllOpResolver.AddHardSwish();
        mutableAllOpResolver.AddIf();
        mutableAllOpResolver.AddL2Normalization();
        mutableAllOpResolver.AddL2Pool2D();
        mutableAllOpResolver.AddLeakyRelu();
        mutableAllOpResolver.AddLess();
        mutableAllOpResolver.AddLessEqual();
        mutableAllOpResolver.AddLog();
        mutableAllOpResolver.AddLogicalAnd();
        mutableAllOpResolver.AddLogicalNot();
        mutableAllOpResolver.AddLogicalOr();
        mutableAllOpResolver.AddLogistic();
        mutableAllOpResolver.AddLogSoftmax();
        mutableAllOpResolver.AddMaxPool2D();
        mutableAllOpResolver.AddMaximum();
        mutableAllOpResolver.AddMean();
        mutableAllOpResolver.AddMinimum();
        mutableAllOpResolver.AddMirrorPad();
        mutableAllOpResolver.AddMul();
        mutableAllOpResolver.AddNeg();
        mutableAllOpResolver.AddNotEqual();
        mutableAllOpResolver.AddPack();
        mutableAllOpResolver.AddPad();
        mutableAllOpResolver.AddPadV2();
        mutableAllOpResolver.AddPrelu();
        mutableAllOpResolver.AddQuantize();
        mutableAllOpResolver.AddReadVariable();
        mutableAllOpResolver.AddReduceMax();
        mutableAllOpResolver.AddRelu();
        mutableAllOpResolver.AddRelu6();
        mutableAllOpResolver.AddReshape();
        mutableAllOpResolver.AddResizeBilinear();
        mutableAllOpResolver.AddResizeNearestNeighbor();
        mutableAllOpResolver.AddRound();
        mutableAllOpResolver.AddRsqrt();
        mutableAllOpResolver.AddSelectV2();
        mutableAllOpResolver.AddShape();
        mutableAllOpResolver.AddSin();
        mutableAllOpResolver.AddSlice();
        mutableAllOpResolver.AddSoftmax();
        mutableAllOpResolver.AddSpaceToBatchNd();
        mutableAllOpResolver.AddSpaceToDepth();
        mutableAllOpResolver.AddSplit();
        mutableAllOpResolver.AddSplitV();
        mutableAllOpResolver.AddSqrt();
        mutableAllOpResolver.AddSquare();
        mutableAllOpResolver.AddSquaredDifference();
        mutableAllOpResolver.AddSqueeze();
        mutableAllOpResolver.AddStridedSlice();
        mutableAllOpResolver.AddSub();
        mutableAllOpResolver.AddSum();
        mutableAllOpResolver.AddSvdf();
        mutableAllOpResolver.AddTanh();
        mutableAllOpResolver.AddTranspose();
        mutableAllOpResolver.AddTransposeConv();
        mutableAllOpResolver.AddUnidirectionalSequenceLSTM();
        mutableAllOpResolver.AddUnpack();
        mutableAllOpResolver.AddVarHandle();
        mutableAllOpResolver.AddWhile();
        mutableAllOpResolver.AddZerosLike();

        return mutableAllOpResolver;
    }

} /* namespace app */
} /* namespace arm */

#endif /* INF_RUNNER_MICRO_MUTABLE_ALLOPS_RESOLVER_HPP */
