# ----------------------------------------------------------------------------
#  SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
#  affiliates <open-source-office@arm.com>
#  SPDX-License-Identifier: Apache-2.0
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# ----------------------------------------------------------------------------
"""
Utility script to supply DeiT model for ExecuTorch.
"""
import numpy as np
from timm.data import IMAGENET_INCEPTION_MEAN, IMAGENET_INCEPTION_STD
from timm.models import deit
from torch import from_numpy, rand
from torchvision import transforms


def get_deit_tiny(option: str):
    """
    Utility function to be used alongside AOT Arm Compiler in ExecuTorch
    to get the DeiT tiny model and its reprentative input in a tuple.

    :return     Tuple of model as nn.Module and input tensor.
    """
    deit_tiny = deit.deit_tiny_patch16_224(pretrained=True)
    deit_tiny.eval()

    if option == 'random':
        # Option 1: Random input:
        normalize = transforms.Normalize(mean=IMAGENET_INCEPTION_MEAN,
                                         std=IMAGENET_INCEPTION_STD)
        model_inputs = (normalize(rand((1, 3, 224, 224))),)
    elif option == 'linspace':
        # Option 2: linearly spaced input covering the span:
        # This works better as a representative dataset.
        shape = (1, 3, 224, 224)
        num_elements = np.prod(shape)  # = 150528
        values = np.linspace(-1.0, 1.0, num=256, endpoint=True)
        repeated_values = np.resize(values, num_elements)
        array = repeated_values.reshape(shape).astype(np.float32)
        model_inputs = (from_numpy(array),)
    else:
        raise ValueError(f'Unsupported option: {option}')

    return deit_tiny, model_inputs


ModelUnderTest, ModelInputs = get_deit_tiny(option='linspace')
