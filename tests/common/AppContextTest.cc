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
#include "AppContext.hpp"

#include <catch.hpp>

TEST_CASE("Common: Application context")
{
    SECTION("Add primitive type Parameter")
    {
        arm::app::ApplicationContext context;
        context.Set<uint32_t>("imgIndex", 0);
        auto data = context.Get<uint32_t>("imgIndex");

        REQUIRE(0 == data);

    }

    SECTION("Add object parameter")
    {
        arm::app::ApplicationContext context;
        std::vector <std::string> vect{"a"};
        context.Set<std::vector <std::string>>("vect", vect);
        auto data = context.Get<std::vector <std::string>>("vect");

        REQUIRE(vect == data);
    }

    SECTION("Add reference object parameter")
    {
        arm::app::ApplicationContext context;
        std::vector <std::string> vect{"a"};
        context.Set<std::vector <std::string>&>("vect", vect);
        auto data = context.Get<std::vector <std::string>&>("vect");

        REQUIRE(vect == data);
    }

    SECTION("Add object pointer parameter")
    {
        arm::app::ApplicationContext context;
        std::vector <std::string>* vect = new std::vector <std::string>{"a"};
        context.Set<std::vector <std::string>*>("vect", vect);
        auto data = context.Get<std::vector <std::string>*>("vect");

        REQUIRE(vect == data);
        delete(vect);
    }
}