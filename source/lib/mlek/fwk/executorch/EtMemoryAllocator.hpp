/**
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its affiliates
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

#ifndef MLEK_EXECUTORCH_MEMORY_ALLOCATOR_HPP
#define MLEK_EXECUTORCH_MEMORY_ALLOCATOR_HPP

#include "mlek/fwk/executorch/ExecuTorch.hpp"

namespace arm::app::fwk::et {

/**
 * Arm allocator to help show associated metrics easily.
 **/
class EtMemoryAllocator : public executorch::runtime::MemoryAllocator {
public:
    /**
     * @brief       Constructor
     * @param[in]   size        Size of memory pool in bytes.
     * @param[in]   baseAddress Base address of the memory pool to be used by this allocator.
     */
    EtMemoryAllocator(uint32_t size, uint8_t* baseAddress);

    /**
     * @brief   Gets the total numbers of bytes allocated by this allocator.
     * @return  Number of bytes as `size_t`
     */
    size_t UsedSizeCurrent() const;

    /**
     * @brief   Gets the peak usage, in bytes, allocated by this allocator.
     * @return  Number of bytes as `size_t`
     */
    size_t UsedSizePeak() const;

    /**
     * @brief   Gets the total numbers of free space in this allocator's buffer.
     * @return  Number of bytes as `size_t`
     */
    size_t FreeSize() const;

    /**
     * @brief   Allocate a chunk of memory. This is just a wrapper override around the base
     *          class implementation.
     * @param[in]   size        Size (in bytes) of memory to be allocated.
     * @param[in]   alignment   Alignment (in bytes) of the allocated region.
     * @return      Pointer to the allocated memory or `nullptr` on failure.
     */
    virtual void* allocate(size_t size, size_t alignment = kDefaultAlignment) override;

    /**
     * @brief   Resets the allocator.
     */
    virtual void reset() override;

private:
    size_t m_used; /**< Number of bytes used (allocated). */
    size_t m_peak; /**< Peak bytes used by this allocator */
};
} /* namespace arm::app::fwk::et */

#endif /* MLEK_EXECUTORCH_MEMORY_ALLOCATOR_HPP */
