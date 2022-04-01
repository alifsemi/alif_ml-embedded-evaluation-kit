/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#include "log_macros.h"

#include <cstring>

namespace arm {
namespace app {
    Profiler::Profiler(hal_platform* platform, const char* name = "Unknown")
    : m_name(name)
    {
        if (platform && platform->inited) {
            this->m_pPlatform = platform;
            this->Reset();
        } else {
            printf_err("Profiler %s initialised with invalid platform\n",
                this->m_name.c_str());
        }
    }

    bool Profiler::StartProfiling(const char* name)
    {
        if (name) {
            this->SetName(name);
        }
        if (this->m_pPlatform && !this->m_started) {
            this->m_pPlatform->timer->reset();
            this->m_tstampSt = this->m_pPlatform->timer->get_counters();
            this->m_started = true;
            return true;
        }
        printf_err("Failed to start profiler %s\n", this->m_name.c_str());
        return false;
    }

    bool Profiler::StopProfiling()
    {
        if (this->m_pPlatform && this->m_started) {
            this->m_tstampEnd = this->m_pPlatform->timer->get_counters();
            this->m_started = false;

            this->AddProfilingUnit(this->m_tstampSt, this->m_tstampEnd, this->m_name);

            return true;
        }
        printf_err("Failed to stop profiler %s\n", this->m_name.c_str());
        return false;
    }

    bool Profiler::StopProfilingAndReset()
    {
        if (this->StopProfiling()) {
            this->Reset();
            return true;
        }
        printf_err("Failed to stop profiler %s\n", this->m_name.c_str());
        return false;
    }

    void Profiler::Reset()
    {
        this->m_started = false;
        this->m_series.clear();
        memset(&this->m_tstampSt, 0, sizeof(this->m_tstampSt));
        memset(&this->m_tstampEnd, 0, sizeof(this->m_tstampEnd));
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
        for (const auto& item: this->m_series) {
            auto name = item.first;
            ProfilingSeries series = item.second;
            ProfileResult result{};
            result.name = item.first;
            result.samplesNum = series.size();

            std::vector<Statistics> stats(series[0].counters.num_counters);
            for (size_t i = 0; i < stats.size(); ++i) {
                stats[i].name = series[0].counters.counters[i].name;
                stats[i].unit = series[0].counters.counters[i].unit;
            }

            for(ProfilingUnit& unit: series) {
                for (size_t i = 0; i < stats.size(); ++i) {
                    calcProfilingStat(
                        unit.counters.counters[i].value,
                        stats[i],
                        result.samplesNum);
                }
            }

            for (Statistics& stat : stats) {
                result.data.emplace_back(stat);
            }

            results.emplace_back(result);
        }

        this->Reset();
    }

    void printStatisticsHeader(uint32_t samplesNum) {
        info("Number of samples: %" PRIu32 "\n", samplesNum);
        info("%s\n", "Total / Avg./ Min / Max");
    }

    void Profiler::PrintProfilingResult(bool printFullStat) {
        std::vector<ProfileResult> results{};
        GetAllResultsAndReset(results);
        for(ProfileResult& result: results) {
            if (result.data.size()) {
                info("Profile for %s:\n", result.name.c_str());
                if (printFullStat) {
                    printStatisticsHeader(result.samplesNum);
                }
            }

            for (Statistics &stat: result.data) {
                if (printFullStat) {
                    info("%s %s: %" PRIu64 "/ %.0f / %" PRIu64 " / %" PRIu64 " \n",
                         stat.name.c_str(), stat.unit.c_str(),
                         stat.total, stat.avrg, stat.min, stat.max);
                } else {
                    info("%s: %.0f %s\n", stat.name.c_str(), stat.avrg, stat.unit.c_str());
                }
            }
        }
    }

    void Profiler::SetName(const char* str)
    {
        this->m_name = std::string(str);
    }

    void Profiler::AddProfilingUnit(pmu_counters start, pmu_counters end,
                                    const std::string& name)
    {
        if (!this->m_pPlatform) {
            printf_err("Invalid platform\n");
            return;
        }

        struct ProfilingUnit unit = {
            .counters = end
        };

        if (end.num_counters != start.num_counters ||
                true != end.initialised || true != start.initialised) {
            printf_err("Invalid start or end counters\n");
            return;
        }

        for (size_t i = 0; i < unit.counters.num_counters; ++i) {
            if (unit.counters.counters[i].value < start.counters[i].value) {
                warn("Overflow detected for %s\n", unit.counters.counters[i].name);
                unit.counters.counters[i].value = 0;
            } else {
                unit.counters.counters[i].value -= start.counters[i].value;
            }
        }

        this->m_series[name].emplace_back(unit);
    }

} /* namespace app */
} /* namespace arm */
