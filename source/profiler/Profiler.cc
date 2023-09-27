/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
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
    Profiler::Profiler()
        : Profiler("Unknown")
    {}

    Profiler::Profiler(const char* name)
        : m_name(name)
    {}

    bool Profiler::StartProfiling(const char* name)
    {
        if (name) {
            this->SetName(name);
        }

        if (!this->m_started) {
            hal_pmu_reset();
            this->m_tstampSt.initialised = false;
            hal_pmu_get_counters(&this->m_tstampSt);
            if (this->m_tstampSt.initialised) {
                if (this->m_profStats.count(this->m_name) == 0) {
                    this->m_profStats.insert(
                            std::pair<std::string, std::vector<Statistics>>(
                                    this->m_name, std::vector<Statistics>(this->m_tstampSt.num_counters))
                                    );
                }
                this->m_started = true;
                return true;
            }
        }
        printf_err("Failed to start profiler %s\n", this->m_name.c_str());
        return false;
    }

    bool Profiler::StopProfiling()
    {
        if (this->m_started) {
            this->m_tstampEnd.initialised = false;
            hal_pmu_get_counters(&this->m_tstampEnd);
            this->m_started = false;
            if (this->m_tstampEnd.initialised) {
                this->UpdateRunningStats(
                    this->m_tstampSt,
                    this->m_tstampEnd,
                    this->m_name);
                return true;
            }
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
        this->m_profStats.clear();
        memset(&this->m_tstampSt, 0, sizeof(this->m_tstampSt));
        memset(&this->m_tstampEnd, 0, sizeof(this->m_tstampEnd));
    }

    void calcProfilingStat(uint64_t currentValue,
                           Statistics& data)
    {
        data.total += currentValue;
        data.min = std::min(data.min, currentValue);
        data.max = std::max(data.max, currentValue);
        data.avrg = (static_cast<double>(data.total) / data.samplesNum);
    }

    void Profiler::GetAllResultsAndReset(std::vector<ProfileResult>& results)
    {
        for (const auto& item: this->m_profStats) {
            auto name = item.first;
            std::vector<Statistics> series = item.second;
            ProfileResult result{};
            result.name = item.first;

            for (Statistics& stat : series) {
                result.samplesNum = stat.samplesNum;
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
            if (!result.data.empty()) {
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

    void Profiler::UpdateRunningStats(pmu_counters start, pmu_counters end,
                                      const std::string& name)
    {
        struct ProfilingUnit unit = {
            .counters = end
        };

        if (end.num_counters != start.num_counters ||
            !end.initialised || !start.initialised) {
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

        for (size_t i = 0; i < this->m_profStats[name].size(); ++i) {
            this->m_profStats[name][i].name = unit.counters.counters[i].name;
            this->m_profStats[name][i].unit = unit.counters.counters[i].unit;
            ++this->m_profStats[name][i].samplesNum;
            calcProfilingStat(
                    unit.counters.counters[i].value,
                    this->m_profStats[name][i]);
        }

        ProfileResult result{};
        result.name = this->m_name;
        if (!this->m_profStats[name].empty()) {
            result.samplesNum = this->m_profStats[name][0].samplesNum;
        }

        for (Statistics& stat : this->m_profStats[name]) {
            result.data.emplace_back(stat);
        }
    }

} /* namespace app */
} /* namespace arm */
