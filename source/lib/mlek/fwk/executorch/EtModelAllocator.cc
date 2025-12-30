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

#include "mlek/fwk/executorch/EtMemoryAllocator.hpp"

namespace arm::app::fwk::et {

EtMemoryAllocator::EtMemoryAllocator(uint32_t size, uint8_t* baseAddress) :
    MemoryAllocator(size, baseAddress), m_used(0), m_peak(0)
{}

void* EtMemoryAllocator::allocate(size_t size, size_t alignment)
{
    void* ret = MemoryAllocator::allocate(size, alignment);
    if (ret != nullptr) {
        /* Align with the same code as in MemoryAllocator::allocate() to keep
         * m_used "in sync". */
        if (!(size & (alignment - 1))) {
            this->m_used += size;
        } else {
            this->m_used = (this->m_used | (alignment - 1)) + 1 + size;
        }

        if (this->m_used > this->m_peak) {
            this->m_peak = this->m_used;
        }
    }
    return ret;
}

size_t EtMemoryAllocator::UsedSizeCurrent() const
{
    return this->m_used;
}

size_t EtMemoryAllocator::UsedSizePeak() const
{
    return this->m_peak;
}

size_t EtMemoryAllocator::FreeSize() const
{
    return (this->size() - this->m_used);
}

void EtMemoryAllocator::reset()
{
    MemoryAllocator::reset();
    this->m_used = 0;
}

} /* namespace arm::app::fwk::et */
