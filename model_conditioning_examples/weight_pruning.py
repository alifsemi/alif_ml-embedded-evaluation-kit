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
This script will provide you with a short example of how to perform magnitude-based weight pruning in TensorFlow
using the TensorFlow Model Optimization Toolkit.

The output from this example will be a TensorFlow Lite model file where ~75% percent of the weights have been 'pruned' to the
value 0 during training - quantization has then been applied on top of this.

By pruning the model we can improve compression of the model file. This can be essential for deploying certain models
on systems with limited resources - such as embedded systems using Arm Ethos NPU. Also, if the pruned model is run
on an Arm Ethos NPU then this pruning can improve the execution time of the model.

After pruning is complete we do post-training quantization to quantize the model and then generate a TensorFlow Lite file.

If you are targetting an Arm Ethos-U55 NPU then the output TensorFlow Lite file will also need to be passed through the Vela
compiler for further optimizations before it can be used.

For more information on using Vela see: https://git.mlplatform.org/ml/ethos-u/ethos-u-vela.git/about/
For more information on weight pruning see: https://www.tensorflow.org/model_optimization/guide/pruning
"""
import pathlib

import tensorflow as tf
import tensorflow_model_optimization as tfmot

from training_utils import get_data, create_model
from post_training_quantization import post_training_quantize, evaluate_tflite_model


def prepare_for_pruning(keras_model):
    """Prepares a Keras model for pruning."""

    # We use a constant sparsity schedule so the amount of sparsity in the model is kept at the same percent throughout
    # training. An alternative is PolynomialDecay where sparsity can be gradually increased during training.
    pruning_schedule = tfmot.sparsity.keras.ConstantSparsity(target_sparsity=0.75, begin_step=0)

    # Apply the pruning wrapper to the whole model so weights in every layer will get pruned. You may find that to avoid
    # too much accuracy loss only certain non-critical layers in your model should be pruned.
    pruning_ready_model = tfmot.sparsity.keras.prune_low_magnitude(keras_model, pruning_schedule=pruning_schedule)

    # We must recompile the model after making it ready for pruning.
    pruning_ready_model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
                                loss=tf.keras.losses.sparse_categorical_crossentropy,
                                metrics=['accuracy'])

    return pruning_ready_model


def main():
    x_train, y_train, x_test, y_test = get_data()
    model = create_model()

    # Compile and train the model first.
    # In general it is easier to do pruning as a fine-tuning step after the model is fully trained.
    model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
                  loss=tf.keras.losses.sparse_categorical_crossentropy,
                  metrics=['accuracy'])

    model.fit(x=x_train, y=y_train, batch_size=128, epochs=5, verbose=1, shuffle=True)

    # Test the trained model accuracy.
    test_loss, test_acc = model.evaluate(x_test, y_test)
    print(f"Test accuracy before pruning: {test_acc:.3f}")

    # Prepare the model for pruning and add the pruning update callback needed in training.
    pruned_model = prepare_for_pruning(model)
    callbacks = [tfmot.sparsity.keras.UpdatePruningStep()]

    # Continue training the model but now with pruning applied - remember to pass in the callbacks!
    pruned_model.fit(x=x_train, y=y_train, batch_size=128, epochs=1, verbose=1, shuffle=True, callbacks=callbacks)
    test_loss, test_acc = pruned_model.evaluate(x_test, y_test)
    print(f"Test accuracy after pruning: {test_acc:.3f}")

    # Remove all variables that pruning only needed in the training phase.
    model_for_export = tfmot.sparsity.keras.strip_pruning(pruned_model)

    # Apply post-training quantization on top of the pruning and save the resulting TensorFlow Lite model to file.
    tflite_model = post_training_quantize(model_for_export, x_train)

    tflite_models_dir = pathlib.Path('./conditioned_models/')
    tflite_models_dir.mkdir(exist_ok=True, parents=True)

    pruned_quant_model_save_path = tflite_models_dir / 'pruned_post_training_quant_model.tflite'
    with open(pruned_quant_model_save_path, 'wb') as f:
        f.write(tflite_model)

    # Test the pruned quantized model accuracy. Save time by only testing a subset of the whole data.
    num_test_samples = 1000
    evaluate_tflite_model(pruned_quant_model_save_path, x_test[0:num_test_samples], y_test[0:num_test_samples])


if __name__ == "__main__":
    main()
