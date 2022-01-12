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
#include "ExpectedObjectDetectionResults.hpp"


/*
//Reference results
Got 2 boxes
0)  (0.999246) -> Detection box: {x=89,y=17,w=41,h=56}
1)  (0.995367) -> Detection box: {x=27,y=81,w=48,h=53}
Entering TestInference
Got 1 boxes
0)  (0.998107) -> Detection box: {x=87,y=35,w=53,h=64}
Entering TestInference
Got 2 boxes
0)  (0.999244) -> Detection box: {x=105,y=73,w=58,h=66}
1)  (0.985984) -> Detection box: {x=34,y=40,w=70,h=95}
Entering TestInference
Got 2 boxes
0)  (0.993294) -> Detection box: {x=22,y=43,w=39,h=53}
1)  (0.992021) -> Detection box: {x=63,y=60,w=38,h=45}

*/

void get_expected_ut_results(std::vector<std::vector<arm::app::DetectionResult>> &expected_results)
{

    expected_results.resize(4);

    std::vector<arm::app::DetectionResult> img_1(2);
    std::vector<arm::app::DetectionResult> img_2(1);
    std::vector<arm::app::DetectionResult> img_3(2);
    std::vector<arm::app::DetectionResult> img_4(2);

    img_1[0] = arm::app::DetectionResult(0.99,89,17,41,56);
    img_1[1] = arm::app::DetectionResult(0.99,27,81,48,53);

    img_2[0] = arm::app::DetectionResult(0.99,87,35,53,64);

    img_3[0] = arm::app::DetectionResult(0.99,105,73,58,66);
    img_3[1] = arm::app::DetectionResult(0.98,34,40,70,95);

    img_4[0] = arm::app::DetectionResult(0.99,22,43,39,53);
    img_4[1] = arm::app::DetectionResult(0.99,63,60,38,45);

    expected_results[0] = img_1;
    expected_results[1] = img_2;
    expected_results[2] = img_3;
    expected_results[3] = img_4;


}
