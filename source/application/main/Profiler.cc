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
#include "Profiler.hpp"

#include <cstring>
#include <string>
#include <sstream>

namespace arm {
namespace app {

    template<class T>
    static void writeStatLine(std::ostringstream& s,
                              const char* desc,
                              T total,
                              uint32_t samples,
                              T min,
                              T max)
    {
        s << "\t" << desc << total << " / "
          << ((double)total / samples) << " / "
          << min << " / " << max << std::endl;
    }

    Profiler::Profiler(hal_platform* platform, const char* name = "Unknown")
    : _m_name(name)
    {
        if (platform && platform->inited) {
            this->_m_pPlatform = platform;
            this->Reset();
        } else {
            printf_err("Profiler %s initialised with invalid platform\n",
                this->_m_name.c_str());
        }
    }

    bool Profiler::StartProfiling(const char* name)
    {
        if (name) {
            this->SetName(name);
        }
        if (this->_m_pPlatform && !this->_m_started) {
            this->_m_pPlatform->timer->reset();
            this->_m_tstampSt = this->_m_pPlatform->timer->start_profiling();
            this->_m_started = true;
            return true;
        }
        printf_err("Failed to start profiler %s\n", this->_m_name.c_str());
        return false;
    }

    bool Profiler::StopProfiling()
    {
        if (this->_m_pPlatform && this->_m_started) {
            this->_m_tstampEnd = this->_m_pPlatform->timer->stop_profiling();
            this->_m_started = false;

            this->_AddProfilingUnit(this->_m_tstampSt, this->_m_tstampEnd, this->_m_name);

            return true;
        }
        printf_err("Failed to stop profiler %s\n", this->_m_name.c_str());
        return false;
    }

    bool Profiler::StopProfilingAndReset()
    {
        if (this->StopProfiling()) {
            this->Reset();
            return true;
        }
        printf_err("Failed to stop profiler %s\n", this->_m_name.c_str());
        return false;
    }

    void Profiler::Reset()
    {
        this->_m_started = false;
        memset(&this->_m_tstampSt, 0, sizeof(this->_m_tstampSt));
        memset(&this->_m_tstampEnd, 0, sizeof(this->_m_tstampEnd));
    }

    std::string Profiler::GetResultsAndReset()
    {
        std::ostringstream strResults;

        for (const auto& item: this->_m_series) {
            auto name = item.first;
            ProfilingSeries series = item.second;

            uint32_t samplesNum = series.size();

            uint64_t totalNpuCycles = 0;        /* Total NPU cycles (idle + active). */
            uint64_t totalActiveNpuCycles = 0;  /* Active NPU cycles. */
            uint64_t totalCpuCycles = 0;        /* Total CPU cycles. */
            time_t totalTimeMs = 0;

            uint64_t minActiveNpuCycles = series[0].activeNpuCycles;
            uint64_t minIdleNpuCycles = series[0].npuCycles - minActiveNpuCycles;
            uint64_t minActiveCpuCycles = series[0].cpuCycles - minActiveNpuCycles;
            time_t minTimeMs = series[0].time;

            uint64_t maxIdleNpuCycles = 0;
            uint64_t maxActiveNpuCycles = 0;
            uint64_t maxActiveCpuCycles = 0;
            time_t maxTimeMs = 0;

            for(ProfilingUnit& unit: series){
                totalNpuCycles += unit.npuCycles;
                totalActiveNpuCycles += unit.activeNpuCycles;
                totalCpuCycles += unit.cpuCycles;
                totalTimeMs += unit.time;

                maxActiveNpuCycles = std::max(maxActiveNpuCycles,
                                              unit.activeNpuCycles);
                maxIdleNpuCycles = std::max(maxIdleNpuCycles,
                                            unit.npuCycles - maxActiveNpuCycles);
                maxActiveCpuCycles = std::max(maxActiveCpuCycles,
                                              unit.cpuCycles - maxActiveNpuCycles);
                maxTimeMs = std::max(maxTimeMs, unit.time);

                minActiveNpuCycles = std::min(minActiveNpuCycles,
                                              unit.activeNpuCycles);
                minIdleNpuCycles = std::min(minIdleNpuCycles,
                                            unit.npuCycles - minActiveNpuCycles);
                minActiveCpuCycles = std::min(minActiveCpuCycles,
                                              unit.cpuCycles - minActiveNpuCycles);
                minTimeMs = std::min(minTimeMs, unit.time);
            }

            strResults << "Profile for " << name << ": " << std::endl;

            if (samplesNum > 1) {
                strResults << "\tSamples: " << samplesNum << std::endl;
                strResults << "\t                            Total / Avg./ Min / Max"
                           << std::endl;

                writeStatLine<uint64_t>(strResults, "Active NPU cycles:          ",
                                        totalActiveNpuCycles, samplesNum,
                                        minActiveNpuCycles, maxActiveNpuCycles);

                writeStatLine<uint64_t>(strResults, "Idle NPU cycles:            ",
                                        (totalNpuCycles - totalActiveNpuCycles),
                                        samplesNum, minIdleNpuCycles, maxIdleNpuCycles);

#if defined(CPU_PROFILE_ENABLED)
                writeStatLine<uint64_t>(strResults, "Active CPU cycles (approx): ",
                                        (totalCpuCycles - totalActiveNpuCycles),
                                        samplesNum, minActiveCpuCycles,
                                        maxActiveCpuCycles);

                writeStatLine<time_t>(strResults, "Time in ms:                 ",
                                        totalTimeMs, samplesNum, minTimeMs, maxTimeMs);
#endif
            } else {
                strResults << "\tActive NPU cycles: " << totalActiveNpuCycles
                           << std::endl;
                strResults << "\tIdle NPU cycles:   "
                           << (totalNpuCycles - totalActiveNpuCycles)
                           << std::endl;
#if defined(CPU_PROFILE_ENABLED)
                strResults << "\tActive CPU cycles: "
                           << (totalCpuCycles - totalActiveNpuCycles)
                           << " (approx)" << std::endl;

                strResults << "\tTime in ms:        " << totalTimeMs << std::endl;
#endif
            }
        }
        this->Reset();
        return strResults.str();
    }

    void Profiler::SetName(const char* str)
    {
        this->_m_name = std::string(str);
    }

    void Profiler::_AddProfilingUnit(time_counter start, time_counter end,
                                     const std::string& name)
    {
        platform_timer * timer = this->_m_pPlatform->timer;

        struct ProfilingUnit unit;

        if (timer->cap.npu_cycles && timer->get_npu_total_cycle_diff &&
            timer->get_npu_active_cycle_diff)
        {
            unit.npuCycles = timer->get_npu_total_cycle_diff(&start, &end);
            unit.activeNpuCycles = timer->get_npu_active_cycle_diff(&start, &end);
        }

        if (timer->cap.cpu_cycles && timer->get_cpu_cycle_diff) {
            unit.cpuCycles = timer->get_cpu_cycle_diff(&start, &end);
        }

        if (timer->cap.duration_ms && timer->get_duration_ms) {
            unit.time = timer->get_duration_ms(&start, &end);
        }

        this->_m_series[name].emplace_back(unit);
    }

} /* namespace app */
} /* namespace arm */