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

    inline tflite::MicroMutableOpResolver<kNumberOperators> get_resolver() {
        tflite::MicroMutableOpResolver<kNumberOperators> micro_op_resolver;

        micro_op_resolver.AddAbs();
        micro_op_resolver.AddAdd();
        micro_op_resolver.AddAddN();
        micro_op_resolver.AddArgMax();
        micro_op_resolver.AddArgMin();
        micro_op_resolver.AddAssignVariable();
        micro_op_resolver.AddAveragePool2D();
        micro_op_resolver.AddBatchToSpaceNd();
        micro_op_resolver.AddBroadcastArgs();
        micro_op_resolver.AddBroadcastTo();
        micro_op_resolver.AddCallOnce();
        micro_op_resolver.AddCast();
        micro_op_resolver.AddCeil();
        micro_op_resolver.AddCircularBuffer();
        micro_op_resolver.AddConcatenation();
        micro_op_resolver.AddConv2D();
        micro_op_resolver.AddCos();
        micro_op_resolver.AddCumSum();
        micro_op_resolver.AddDepthToSpace();
        micro_op_resolver.AddDepthwiseConv2D();
        micro_op_resolver.AddDequantize();
        micro_op_resolver.AddDetectionPostprocess();
        micro_op_resolver.AddDiv();
        micro_op_resolver.AddElu();
        micro_op_resolver.AddEqual();
        micro_op_resolver.AddEthosU();
        micro_op_resolver.AddExp();
        micro_op_resolver.AddExpandDims();
        micro_op_resolver.AddFill();
        micro_op_resolver.AddFloor();
        micro_op_resolver.AddFloorDiv();
        micro_op_resolver.AddFloorMod();
        micro_op_resolver.AddFullyConnected();
        micro_op_resolver.AddGather();
        micro_op_resolver.AddGatherNd();
        micro_op_resolver.AddGreater();
        micro_op_resolver.AddGreaterEqual();
        micro_op_resolver.AddHardSwish();
        micro_op_resolver.AddIf();
        micro_op_resolver.AddL2Normalization();
        micro_op_resolver.AddL2Pool2D();
        micro_op_resolver.AddLeakyRelu();
        micro_op_resolver.AddLess();
        micro_op_resolver.AddLessEqual();
        micro_op_resolver.AddLog();
        micro_op_resolver.AddLogicalAnd();
        micro_op_resolver.AddLogicalNot();
        micro_op_resolver.AddLogicalOr();
        micro_op_resolver.AddLogistic();
        micro_op_resolver.AddLogSoftmax();
        micro_op_resolver.AddMaxPool2D();
        micro_op_resolver.AddMaximum();
        micro_op_resolver.AddMean();
        micro_op_resolver.AddMinimum();
        micro_op_resolver.AddMirrorPad();
        micro_op_resolver.AddMul();
        micro_op_resolver.AddNeg();
        micro_op_resolver.AddNotEqual();
        micro_op_resolver.AddPack();
        micro_op_resolver.AddPad();
        micro_op_resolver.AddPadV2();
        micro_op_resolver.AddPrelu();
        micro_op_resolver.AddQuantize();
        micro_op_resolver.AddReadVariable();
        micro_op_resolver.AddReduceMax();
        micro_op_resolver.AddRelu();
        micro_op_resolver.AddRelu6();
        micro_op_resolver.AddReshape();
        micro_op_resolver.AddResizeBilinear();
        micro_op_resolver.AddResizeNearestNeighbor();
        micro_op_resolver.AddRound();
        micro_op_resolver.AddRsqrt();
        micro_op_resolver.AddSelectV2();
        micro_op_resolver.AddShape();
        micro_op_resolver.AddSin();
        micro_op_resolver.AddSlice();
        micro_op_resolver.AddSoftmax();
        micro_op_resolver.AddSpaceToBatchNd();
        micro_op_resolver.AddSpaceToDepth();
        micro_op_resolver.AddSplit();
        micro_op_resolver.AddSplitV();
        micro_op_resolver.AddSqrt();
        micro_op_resolver.AddSquare();
        micro_op_resolver.AddSquaredDifference();
        micro_op_resolver.AddSqueeze();
        micro_op_resolver.AddStridedSlice();
        micro_op_resolver.AddSub();
        micro_op_resolver.AddSum();
        micro_op_resolver.AddSvdf();
        micro_op_resolver.AddTanh();
        micro_op_resolver.AddTranspose();
        micro_op_resolver.AddTransposeConv();
        micro_op_resolver.AddUnidirectionalSequenceLSTM();
        micro_op_resolver.AddUnpack();
        micro_op_resolver.AddVarHandle();
        micro_op_resolver.AddWhile();
        micro_op_resolver.AddZerosLike();

        return micro_op_resolver;
    }

} /* namespace app */
} /* namespace arm */

#endif /* INF_RUNNER_MICRO_MUTABLE_ALLOPS_RESOLVER_HPP */
