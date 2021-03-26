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
This script will provide you with an example of how to perform post-training quantization in TensorFlow.

The output from this example will be a TensorFlow Lite model file where weights and activations are quantized to 8bit
integer values.

Quantization helps reduce the size of your models and is necessary for running models on certain hardware such as Arm
Ethos NPU.

In addition to quantizing weights, post-training quantization uses a calibration dataset to
capture the minimum and maximum values of all variable tensors in your model.
By capturing these ranges it is possible to fully quantize not just the weights of the model but also the activations.

Depending on the model you are quantizing there may be some accuracy loss, but for a lot of models the loss should
be minimal.

If you are targetting an Arm Ethos-U55 NPU then the output TensorFlow Lite file will also need to be passed through the Vela
compiler for further optimizations before it can be used.

For more information on using Vela see: https://git.mlplatform.org/ml/ethos-u/ethos-u-vela.git/about/
For more information on post-training quantization
see: https://www.tensorflow.org/lite/performance/post_training_integer_quant
"""
import pathlib

import numpy as np
import tensorflow as tf

from training_utils import get_data, create_model


def post_training_quantize(keras_model, sample_data):
    """Quantize Keras model using post-training quantization with some sample data.

    TensorFlow Lite will have fp32 inputs/outputs and the model will handle quantizing/dequantizing.

    Args:
        keras_model: Keras model to quantize.
        sample_data: A numpy array of data to use as a representative dataset.

    Returns:
        Quantized TensorFlow Lite model.
    """

    converter = tf.lite.TFLiteConverter.from_keras_model(keras_model)

    # We set the following converter options to ensure our model is fully quantized.
    # An error should get thrown if there is any ops that can't be quantized.
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]

    # To use post training quantization we must provide some sample data that will be used to
    # calculate activation ranges for quantization. This data should be representative of the data
    # we expect to feed the model and must be provided by a generator function.
    def generate_repr_dataset():
        for i in range(100):  # 100 samples is all we should need in this example.
            yield [np.expand_dims(sample_data[i], axis=0)]

    converter.representative_dataset = generate_repr_dataset
    tflite_model = converter.convert()

    return tflite_model


def evaluate_tflite_model(tflite_save_path, x_test, y_test):
    """Calculate the accuracy of a TensorFlow Lite model using TensorFlow Lite interpreter.

    Args:
        tflite_save_path: Path to TensorFlow Lite model to test.
        x_test: numpy array of testing data.
        y_test: numpy array of testing labels (sparse categorical).
    """

    interpreter = tf.lite.Interpreter(model_path=str(tflite_save_path))

    interpreter.allocate_tensors()
    input_details = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    accuracy_count = 0
    num_test_images = len(y_test)

    for i in range(num_test_images):
        interpreter.set_tensor(input_details[0]['index'], x_test[i][np.newaxis, ...])
        interpreter.invoke()
        output_data = interpreter.get_tensor(output_details[0]['index'])

        if np.argmax(output_data) == y_test[i]:
            accuracy_count += 1

    print(f"Test accuracy quantized: {accuracy_count / num_test_images:.3f}")


def main():
    x_train, y_train, x_test, y_test = get_data()
    model = create_model()

    # Compile and train the model in fp32 as normal.
    model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
                  loss=tf.keras.losses.sparse_categorical_crossentropy,
                  metrics=['accuracy'])

    model.fit(x=x_train, y=y_train, batch_size=128, epochs=5, verbose=1, shuffle=True)

    # Test the fp32 model accuracy.
    test_loss, test_acc = model.evaluate(x_test, y_test)
    print(f"Test accuracy float: {test_acc:.3f}")

    # Quantize and export the resulting TensorFlow Lite model to file.
    tflite_model = post_training_quantize(model, x_train)

    tflite_models_dir = pathlib.Path('./conditioned_models/')
    tflite_models_dir.mkdir(exist_ok=True, parents=True)

    quant_model_save_path = tflite_models_dir / 'post_training_quant_model.tflite'
    with open(quant_model_save_path, 'wb') as f:
        f.write(tflite_model)

    # Test the quantized model accuracy. Save time by only testing a subset of the whole data.
    num_test_samples = 1000
    evaluate_tflite_model(quant_model_save_path, x_test[0:num_test_samples], y_test[0:num_test_samples])


if __name__ == "__main__":
    main()
