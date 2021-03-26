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
#include "OutputDecode.hpp"

#include "catch.hpp"

TEST_CASE("Running output decode on test vector") {

    std::vector<arm::app::ClassificationResult> vecResult(20);
    /* Number of test inputs. */
    const size_t numStrings = 8; 
    
    /* The test inputs. */
    std::string testText[numStrings][20] 
    {
        {"a", "b", "c", "d", "e", "f", "g", "g", "g", " ", "h", "h", "i", " ", " ", "j", "k", "\'", "\'", "l"},  /* initial */
        {" ", "b", "c", "d", "e", "f", "g", "g", "g", " ", "h", "h", "i", " ", " ", "j", "k", "\'", "\'", " "},  /* space start and end */
        {"\'", "b", "c", "d", "e", "f", "g", "g", "g", " ", "h", "h", "i", " ", " ", "j", "k", "\'", "l", "\'"}, /* apostrophe start and end */
        {"a", "a", "c", "d", "e", "f", "g", "g", "g", " ", "h", "h", "i", " ", " ", "j", "k", "\'", "l", "l"},   /* Double start and end */
        {"a", "b", "c", "d", "e", "f", "g", "g", "o", "$", "o", "h", "i", " ", " ", "j", "k", "\'", "\'", "l"},  /* Legit double character */
        {"a", "$", "a", "d", "e", "f", "g", "g", "o", "$", "o", "h", "i", " ", " ", "j", "k", "l", "$", "l"},    /* Legit double character start and end */
        {"$", "a", "b", "d", "e", "f", "g", "g", "o", "$", "o", "h", "i", " ", " ", "j", "k", "l", "$", "$"},    /* $$ */
        {"$", "a", "b", "d", "e", "f", "g", "g", "o", "$", "o", "h", "i", " ", " ", "j", "k", "l", "l", "l"}
    };

    /* The golden outputs for the above test inputs. */
    std::string expectedOutput[numStrings] =
    {
        {"abcdefg hi jk\'l"},
        {" bcdefg hi jk\' "},
        {"\'bcdefg hi jk\'l\'"},
        {"acdefg hi jk\'l"},
        {"abcdefgoohi jk\'l"},
        {"aadefgoohi jkll"},
        {"abdefgoohi jkl"},
        {"abdefgoohi jkl"}
    };

    /*For each test input. */
    for (size_t h = 0; h < numStrings; ++h)
    {
        /* Generate fake vecResults.m_label to mimic AsrClassifier output containing the testText. */
        for (size_t i = 0; i < 20; i++)
        {
            vecResult[i].m_label = testText[h][i];
        }
        /* Call function with fake vecResults and save returned string into 'buff'. */
        std::string buff = arm::app::audio::asr::DecodeOutput(vecResult); 

        /* Check that the string returned from the function matches the expected output given above. */
        REQUIRE(buff.compare(expectedOutput[h]) == 0); 
    }
}