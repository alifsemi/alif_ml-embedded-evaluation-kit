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
This script will provide you with a short example of how to perform quantization aware training in TensorFlow using the
TensorFlow Model Optimization Toolkit.

The output from this example will be a TensorFlow Lite model file where weights and activations are quantized to 8bit
integer values.

Quantization helps reduce the size of your models and is necessary for running models on certain hardware such as Arm
Ethos NPU.

In quantization aware training (QAT), the error introduced with quantizing from fp32 to int8 is simulated using
fake quantization nodes. By simulating this quantization error when training, the model can learn better adapted
weights and minimize accuracy losses caused by the reduced precision.

Minimum and maximum values for activations are also captured during training so activations for every layer can be
quantized along with the weights later.

Quantization is only simulated during training and the training backward passes are still performed in full float
precision. Actual quantization happens when generating a TensorFlow Lite model.

If you are targetting an Arm Ethos-U55 NPU then the output TensorFlow Lite file will also need to be passed through the Vela
compiler for further optimizations before it can be used.

For more information on using vela see: https://git.mlplatform.org/ml/ethos-u/ethos-u-vela.git/about/
For more information on quantization aware training
see: https://www.tensorflow.org/model_optimization/guide/quantization/training
"""
import pathlib

import numpy as np
import tensorflow as tf
import tensorflow_model_optimization as tfmot

from training_utils import get_data, create_model


def quantize_and_convert_to_tflite(keras_model):
    """Quantize and convert Keras model trained with QAT to TensorFlow Lite.

    TensorFlow Lite will have fp32 inputs/outputs and the model will handle quantizing/dequantizing.

    Args:
        keras_model: Keras model trained with quantization aware training.

    Returns:
        Quantized TensorFlow Lite model.
    """

    converter = tf.lite.TFLiteConverter.from_keras_model(keras_model)

    # After doing quantization aware training all the information for creating a fully quantized
    # TensorFlow Lite model is already within the quantization aware Keras model.
    # This means we only need to call convert with default optimizations to generate the quantized TensorFlow Lite model.
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
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

    # When working with the TensorFlow Keras API and the TF Model Optimization Toolkit we can make our
    # model quantization aware in one line. Once this is done we compile the model and train as normal.
    # It is important to note that the model is only quantization aware and is not quantized yet. The weights are
    # still floating point and will only be converted to int8 when we generate the TensorFlow Lite model later on.
    quant_aware_model = tfmot.quantization.keras.quantize_model(model)

    quant_aware_model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
                              loss=tf.keras.losses.sparse_categorical_crossentropy,
                              metrics=['accuracy'])

    quant_aware_model.fit(x=x_train, y=y_train, batch_size=128, epochs=5, verbose=1, shuffle=True)

    # Test the quantization aware model accuracy.
    test_loss, test_acc = quant_aware_model.evaluate(x_test, y_test)
    print(f"Test accuracy quant aware: {test_acc:.3f}")

    # Quantize and save the resulting TensorFlow Lite model to file.
    tflite_model = quantize_and_convert_to_tflite(quant_aware_model)

    tflite_models_dir = pathlib.Path('./conditioned_models/')
    tflite_models_dir.mkdir(exist_ok=True, parents=True)

    quant_model_save_path = tflite_models_dir / 'qat_quant_model.tflite'
    with open(quant_model_save_path, 'wb') as f:
        f.write(tflite_model)

    # Test quantized model accuracy. Save time by only testing a subset of the whole data.
    num_test_samples = 1000
    evaluate_tflite_model(quant_model_save_path, x_test[0:num_test_samples], y_test[0:num_test_samples])


if __name__ == "__main__":
    main()
