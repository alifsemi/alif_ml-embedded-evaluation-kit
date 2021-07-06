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
#include "AudioUtils.hpp"
#include "catch.hpp"

TEST_CASE("Common: Slide long data")
{
    std::vector<int> test{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    SECTION("Fit the data")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 1, 1);

        for (int i = 0 ; i < 10; ++i) {
            REQUIRE(slider.HasNext());
            REQUIRE(*slider.Next() == i + 1);
        }

        REQUIRE(!slider.HasNext());
        REQUIRE(nullptr == slider.Next());
    }

    SECTION("Fit the data stride> window")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 2, 3);

        REQUIRE(slider.HasNext());
        REQUIRE(*slider.Next() == 1);

        REQUIRE(slider.HasNext());
        REQUIRE(*slider.Next() == 4);

        REQUIRE(slider.HasNext());
        REQUIRE(*slider.Next() == 7);

        REQUIRE(!slider.HasNext());
        REQUIRE(nullptr == slider.Next());
    }

    SECTION("Fit the data stride < window")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 5, 1);

        for (int i = 0 ; i < 6; i++) {
            REQUIRE(slider.HasNext());
            REQUIRE(*slider.Next() == i + 1);
        }

        REQUIRE(!slider.HasNext());
        REQUIRE(nullptr == slider.Next());
    }
}


TEST_CASE("Common: Slide data size 1")
{
    std::vector<int> test{1};

    SECTION("Fit the data")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 1, 1);

        REQUIRE(slider.HasNext());
        REQUIRE(*slider.Next() == 1);
        REQUIRE(!slider.HasNext());
        REQUIRE(nullptr == slider.Next());
    }

    SECTION("Does not Fit the data because of big window")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 2, 1);

        REQUIRE(!slider.HasNext());
        REQUIRE(nullptr == slider.Next());
    }

    SECTION("Does not Fit the data because of big stride")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 1, 2);

        REQUIRE(slider.HasNext());
        REQUIRE(*slider.Next() == 1);
        REQUIRE(!slider.HasNext());
        REQUIRE(nullptr == slider.Next());
    }

}


TEST_CASE("Common: Slide reset")
{
    SECTION("current range")
    {
        std::vector<int> test{1};
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 1, 1);
        int *saved = slider.Next();
        slider.Reset();

        REQUIRE(slider.Next() == saved);
    }

    SECTION("new range")
    {
        std::vector<int> test{1};
        std::vector<int> test2{100};
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 1, 1);
        slider.Next();
        slider.Reset(test2.data());

        REQUIRE(*slider.Next() == 100);
    }
}


TEST_CASE("Common: Slide fast forward")
{
    std::vector<int> test{1, 2, 3, 4, 5};

    auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 1, 1);
    SECTION("at the beginning") {
        slider.FastForward(3);

        REQUIRE(slider.HasNext());
        REQUIRE(*slider.Next() == 4);
    }

    SECTION("in the middle")
    {
        slider.Next();
        slider.FastForward(3);

        REQUIRE(slider.HasNext());
        REQUIRE(*slider.Next() == 4);
    }

    SECTION("at the end")
    {
        while(slider.HasNext()) {
            slider.Next();
        }
        slider.FastForward(3);

        REQUIRE(slider.HasNext());
        REQUIRE(*slider.Next() == 4);
    }

    SECTION("out of the range")
    {
        slider.FastForward(100);

        REQUIRE(!slider.HasNext());
        REQUIRE(slider.Next() == nullptr);
    }
}


TEST_CASE("Common: Slide Index")
{
    std::vector<int> test{1, 2, 3, 4, 5};
    auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 1, 1);
    REQUIRE(slider.Index() == 0);
    for (int i = 0; i < 5; i++) {
        slider.Next();
        REQUIRE(slider.Index() == i);
    }
}


TEST_CASE("Common: Total strides") 
{
    std::vector<int> test{1, 2, 3, 4, 5};

    SECTION("Element by element")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 1, 1);
        REQUIRE(slider.TotalStrides() == 4 );
    }

    SECTION("Step through element")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 1, 2);
        REQUIRE(slider.TotalStrides() == 2 );
    }

    SECTION("Window = data")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 5, 2);
        REQUIRE(slider.TotalStrides() == 0 );
    }

    SECTION("Window > data")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 6, 2);
        REQUIRE(slider.TotalStrides() == 0 );
    }

    SECTION("Window < data, not enough for the next stride")
    {
        auto slider = arm::app::audio::SlidingWindow<int>(test.data(), test.size(), 4, 2);
        REQUIRE(slider.TotalStrides() == 0 );
    }
}


TEST_CASE("Common: Next window data index")
{
    std::vector<int> test{1, 2, 3, 4, 5};

    /* Check we get the correct index returned */
    SECTION("Stride 1")
    {
        auto slider = arm::app::audio::FractionalSlidingWindow<int>(test.data(), test.size(), 1, 1);
        REQUIRE(slider.NextWindowStartIndex() == 0);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 1);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 2);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 3);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 4);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 5);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 5);
    }

    SECTION("Stride 2")
    {
        auto slider = arm::app::audio::FractionalSlidingWindow<int>(test.data(), test.size(), 1, 2);
        REQUIRE(slider.NextWindowStartIndex() == 0);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 2);
        REQUIRE(slider.NextWindowStartIndex() == 2);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 4);
    }

    SECTION("Stride 3")
    {
        auto slider = arm::app::audio::FractionalSlidingWindow<int>(test.data(), test.size(), 1, 3);
        REQUIRE(slider.NextWindowStartIndex() == 0);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 3);
        REQUIRE(slider.NextWindowStartIndex() == 3);
        slider.Next();
        REQUIRE(slider.NextWindowStartIndex() == 6);
        REQUIRE(!slider.HasNext());
        REQUIRE(slider.Next() == nullptr);
        REQUIRE(slider.NextWindowStartIndex() == 6);
    }
}
