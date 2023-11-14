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
#ifndef APP_PROFILER_HPP
#define APP_PROFILER_HPP

#include "hal.h"

#include <cstdint>
#include <string>
#include <map>
#include <vector>

namespace arm {
namespace app {

    /** Statistics for a profiling metric. */
    struct Statistics {
        std::string name;
        std::string unit;
        std::uint64_t total;
        double avrg;
        std::uint64_t min;
        std::uint64_t max;
        std::uint32_t samplesNum = 0;
    };

    /** Profiling results with calculated statistics. */
    struct ProfileResult {
        std::string name;
        std::uint32_t samplesNum;
        std::vector<Statistics> data;
    };

    /** A single profiling unit definition. */
    struct ProfilingUnit {
        pmu_counters counters;
    };

    /* A map for string identifiable profiling statistics. */
    using ProfilingStats = std::map<std::string, std::vector<Statistics>>;

    /**
     * @brief   A very simple profiler example using the platform timer
     *          implementation.
     */
    class Profiler {
    public:
        /**
         * @brief       Constructor for profiler.
         * @param[in]   name       A friendly name for this profiler.
         **/
        Profiler(const char* name);

        /** Default constructor. */
        Profiler();

        /** Default destructor. */
        ~Profiler() = default;

        /** @brief  Start profiling => get starting time-stamp. */
        bool StartProfiling(const char* name = nullptr);

        /** @brief  Stop profiling => get the ending time-stamp. */
        bool StopProfiling();

        /** @brief  Stops the profiling and internally resets the
         *          platform timers. */
        bool StopProfilingAndReset();

        /** @brief  Reset the platform timers. */
        void Reset();

        /**
         * @brief   Collects profiling results statistics and resets the profiler.
         **/
        void GetAllResultsAndReset(std::vector<ProfileResult>& results);

        /**
         * @brief   Prints collected profiling results and resets the profiler.
         **/
        void PrintProfilingResult(bool printFullStat = false);

        /** @brief Set the profiler name. */
        void SetName(const char* str);

    private:
        ProfilingStats     m_profStats;             /* Profiling stats map. */
        pmu_counters       m_tstampSt{};            /* Container for a current starting timestamp. */
        pmu_counters       m_tstampEnd{};           /* Container for a current ending timestamp. */
        bool               m_started = false;       /* Indicates profiler has been started. */
        std::string        m_name;                  /* Name given to this profiler. */


        /**
         * @brief       Updates the running average stats with those computed
         *              by the "start" and "end" timestamps for the profiling
         *              stats identified by the name provided.
         * @param[in]   start   Starting time-stamp.
         * @param[in]   end     Ending time-stamp.
         * @param[in]   name    Name for the profiling running stats series to be
         *                      updated.
         **/
        void UpdateRunningStats(pmu_counters start, pmu_counters end,
                                const std::string& name);
    };

} /* namespace app */
} /* namespace arm */

#endif /* APP_PROFILER_HPP */
