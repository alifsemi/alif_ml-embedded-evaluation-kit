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
#include <iomanip>

namespace arm {
namespace app {
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

            this->AddProfilingUnit(this->_m_tstampSt, this->_m_tstampEnd, this->_m_name);

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
        this->_m_series.clear();
        memset(&this->_m_tstampSt, 0, sizeof(this->_m_tstampSt));
        memset(&this->_m_tstampEnd, 0, sizeof(this->_m_tstampEnd));
    }

    void calcProfilingStat(uint64_t currentValue,
                           Statistics& data,
                           uint32_t samples)
    {
        data.total += currentValue;
        data.min = std::min(data.min, currentValue);
        data.max = std::max(data.max, currentValue);
        data.avrg = ((double)data.total / samples);
    }

    void Profiler::GetAllResultsAndReset(std::vector<ProfileResult>& results)
    {
        for (const auto& item: this->_m_series) {
            auto name = item.first;
            ProfilingSeries series = item.second;
            ProfileResult result{};
            result.name = item.first;
            result.samplesNum = series.size();

            Statistics AXI0_RD {
                .name = "NPU AXI0_RD_DATA_BEAT_RECEIVED",
                .unit = "beats",
                .total = 0,
                .avrg = 0.0,
                .min = series[0].axi0writes,
                .max = 0
            };
            Statistics AXI0_WR {
                    .name = "NPU AXI0_WR_DATA_BEAT_WRITTEN",
                    .unit = "beats",
                    .total = 0,
                    .avrg = 0.0,
                    .min = series[0].axi0reads,
                    .max = 0
            };
            Statistics AXI1_RD {
                    .name = "NPU AXI1_RD_DATA_BEAT_RECEIVED",
                    .unit = "beats",
                    .total = 0,
                    .avrg = 0.0,
                    .min = series[0].axi1reads,
                    .max = 0
            };
            Statistics NPU_ACTIVE {
                    .name = "NPU ACTIVE",
                    .unit = "cycles",
                    .total = 0,
                    .avrg = 0.0,
                    .min = series[0].activeNpuCycles,
                    .max = 0
            };
            Statistics NPU_IDLE {
                    .name = "NPU IDLE",
                    .unit = "cycles",
                    .total = 0,
                    .avrg = 0.0,
                    .min = series[0].idleNpuCycles,
                    .max = 0
            };
            Statistics NPU_Total {
                    .name = "NPU total",
                    .unit = "cycles",
                    .total = 0,
                    .avrg = 0.0,
                    .min = series[0].npuCycles,
                    .max = 0,
            };
#if defined(CPU_PROFILE_ENABLED)
            Statistics CPU_ACTIVE {
                    .name = "CPU ACTIVE",
                    .unit = "cycles (approx)",
                    .total = 0,
                    .avrg = 0.0,
                    .min = series[0].cpuCycles - NPU_ACTIVE.min,
                    .max = 0
            };
            Statistics TIME {
                    .name = "Time",
                    .unit = "ms",
                    .total = 0,
                    .avrg = 0.0,
                    .min = static_cast<uint64_t>(series[0].time),
                    .max = 0
            };
#endif
            for(ProfilingUnit& unit: series){

                calcProfilingStat(unit.npuCycles,
                                  NPU_Total, result.samplesNum);

                calcProfilingStat(unit.activeNpuCycles,
                                  NPU_ACTIVE, result.samplesNum);

                calcProfilingStat(unit.idleNpuCycles,
                                  NPU_IDLE, result.samplesNum);

                calcProfilingStat(unit.axi0writes,
                                  AXI0_WR, result.samplesNum);

                calcProfilingStat(unit.axi0reads,
                                  AXI0_RD, result.samplesNum);

                calcProfilingStat(unit.axi1reads,
                                  AXI1_RD, result.samplesNum);
#if defined(CPU_PROFILE_ENABLED)
                calcProfilingStat(static_cast<uint64_t>(unit.time),
                                  TIME, result.samplesNum);

                calcProfilingStat(unit.cpuCycles - unit.activeNpuCycles,
                                  CPU_ACTIVE, result.samplesNum);
#endif
            }
            result.data.emplace_back(AXI0_RD);
            result.data.emplace_back(AXI0_WR);
            result.data.emplace_back(AXI1_RD);
            result.data.emplace_back(NPU_ACTIVE);
            result.data.emplace_back(NPU_IDLE);
            result.data.emplace_back(NPU_Total);
#if defined(CPU_PROFILE_ENABLED)
            result.data.emplace_back(CPU_ACTIVE);
            result.data.emplace_back(TIME);
#endif
        results.emplace_back(result);
        }
        this->Reset();
    }

    void printStatisticsHeader(uint32_t samplesNum) {
        info("Number of samples: %i\n", samplesNum);
        info("%s\n", "Total / Avg./ Min / Max");
    }

    void Profiler::PrintProfilingResult(bool printFullStat) {
        std::vector<ProfileResult> results{};
        GetAllResultsAndReset(results);
        for(ProfileResult& result: results) {
            info("Profile for %s:\n", result.name.c_str());

            if (printFullStat) {
                printStatisticsHeader(result.samplesNum);
            }

            for (Statistics &stat: result.data) {
                if (printFullStat) {
                    info("%s %s: %llu / %.0f / %llu / %llu \n", stat.name.c_str(), stat.unit.c_str(),
                         stat.total, stat.avrg, stat.min, stat.max);
                } else {
                    info("%s %s: %.0f\n", stat.name.c_str(), stat.unit.c_str(), stat.avrg);
                }
            }
        }
    }

    void Profiler::SetName(const char* str)
    {
        this->_m_name = std::string(str);
    }

    void Profiler::AddProfilingUnit(time_counter start, time_counter end,
                                    const std::string& name)
    {
        platform_timer * timer = this->_m_pPlatform->timer;

        struct ProfilingUnit unit;

        if (timer->cap.npu_cycles && timer->get_npu_cycles_diff)
        {
            const size_t size = 6;
            uint64_t pmuCounters[size] = {0};
            /* 6 values: total cc, active cc, idle cc, axi0 read, axi0 write, axi1 read*/
            if (0 == timer->get_npu_cycles_diff(&start, &end, pmuCounters, size)) {
                unit.npuCycles = pmuCounters[0];
                unit.activeNpuCycles = pmuCounters[1];
                unit.idleNpuCycles = pmuCounters[2];
                unit.axi0reads = pmuCounters[3];
                unit.axi0writes = pmuCounters[4];
                unit.axi1reads = pmuCounters[5];
            }
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