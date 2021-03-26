#  Copyright (c) 2021 Arm Limited. All rights reserved.
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
"""
Utility functions related to data and models that are common to all the model conditioning examples.
"""
import tensorflow as tf
import numpy as np


def get_data():
    """Downloads and returns the pre-processed data and labels for training and testing.

    Returns:
        Tuple of: (train data, train labels, test data, test labels)
    """

    # To save time we use the MNIST dataset for this example.
    (x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data()

    # Convolution operations require data to have 4 dimensions.
    # We divide by 255 to help training and cast to float32 for TensorFlow.
    x_train = (x_train[..., np.newaxis] / 255.0).astype(np.float32)
    x_test = (x_test[..., np.newaxis] / 255.0).astype(np.float32)

    return x_train, y_train, x_test, y_test


def create_model():
    """Create and returns a simple Keras model for training MNIST.

    We will use a simple convolutional neural network for this example,
    but the model optimization methods employed should be compatible with a
    wide variety of CNN architectures such as Mobilenet and Inception etc.

    Returns:
        Uncompiled Keras model.
    """

    keras_model = tf.keras.models.Sequential([
        tf.keras.layers.Conv2D(32, 3, padding='same', input_shape=(28, 28, 1), activation=tf.nn.relu),
        tf.keras.layers.Conv2D(32, 3, padding='same', activation=tf.nn.relu),
        tf.keras.layers.MaxPool2D(),
        tf.keras.layers.Conv2D(32, 3, padding='same', activation=tf.nn.relu),
        tf.keras.layers.MaxPool2D(),
        tf.keras.layers.Flatten(),
        tf.keras.layers.Dense(units=10, activation=tf.nn.softmax)
    ])

    return keras_model
