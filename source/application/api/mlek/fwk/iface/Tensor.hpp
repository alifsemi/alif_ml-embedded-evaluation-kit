/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or
 * its affiliates <open-source-office@arm.com>
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

#ifndef MLEK_TENSOR_HPP
#define MLEK_TENSOR_HPP

#include <cstddef>
#include <vector>

namespace arm::app::fwk::iface {

/** Struct for quantization parameters. */
struct QuantParams {
    float scale{0.0};
    int offset{0};
};

/** Enum representing tensor data types supported. */
enum class TensorType { INT8 = 0, UINT8 = 1, INT16 = 2, INT32 = 3, FP16 = 4, FP32 = 5, INVALID = 6 };

/** Enum representing tensor layout. */
enum class TensorLayout { NHWC = 0, NCHW = 1, INVALID = 2 };

/**
 * @brief       Gets the tensor data type name as character array.
 * @param[in]   type Tensor data type object.
 * @return      String represented as const char pointer.
 */
const inline char* GetTensorDataTypeName(const TensorType type)
{
    switch (type) {
    case TensorType::INT8:
        return "int8";
    case TensorType::UINT8:
        return "uint8";
    case TensorType::INT16:
        return "int16";
    case TensorType::INT32:
        return "int32";
    case TensorType::FP16:
        return "fp16";
    case TensorType::FP32:
        return "fp32";
    default:
        return "unknown";
    }
}

/**
 * @brief       Gets the tensor data type name as character array.
 * @param[in]   type Tensor data type object.
 * @return      String represented as const char pointer.
 */
const inline char* GetTensorLayoutName(const TensorLayout layout)
{
    switch (layout) {
    case TensorLayout::NHWC:
        return "NHWC";
    case TensorLayout::NCHW:
        return "NCHW";
    default:
        return "unknown";
    }
}

/**
 * @brief       Gets the tensor element size in bytes.
 * @param[in]   type Tensor data type object.
 * @return      Number of bytes as a signed integer.
 */
inline int GetTensorDataTypeSize(const TensorType type)
{
    switch (type) {
    case TensorType::INT8:
        [[fallthrough]];
    case TensorType::UINT8:
        return 1;
    case TensorType::INT16:
        [[fallthrough]];
    case TensorType::FP16:
        return 2;
    case TensorType::INT32:
        [[fallthrough]];
    case TensorType::FP32:
        return 4;
    default:
        return 0;
    }
}

/**
 * @brief Tensor interface class.
 */
class TensorIface {
public:
    TensorIface()                        = default; /**< Constructor */
    virtual ~TensorIface()               = default; /** Destructor */

    /** Gets pointer to actual tensor data. */
    virtual void* GetData()              = 0;

    /** Gets number of bytes occupied by tensor data. */
    virtual size_t Bytes()               = 0;

    /** Gets number of elements in the tensor (all dimensions combined). */
    virtual size_t GetNumElements()      = 0;

    /** Gets the tensor data type. */
    virtual TensorType Type()            = 0;

    /** Gets the tensor layout. */
    virtual TensorLayout Layout()        = 0;

    /** Gets the tensor shape, expressed as a std::vector */
    virtual std::vector<size_t> Shape()  = 0;

    /** Gets the quantization parameters for this tensor. */
    virtual QuantParams GetQuantParams() = 0;

    /** Utility function to get the tensor data pointer in required pointer format. */
    template <typename T>
    T* GetData()
    {
        return reinterpret_cast<T*>(this->GetData());
    }
};
} // namespace arm::app::fwk::iface

#endif /* MLEK_TENSOR_HPP */
