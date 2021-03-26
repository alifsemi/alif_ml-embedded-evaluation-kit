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
This script will provide you with a short example of how to perform clustering of weights (weight sharing) in
TensorFlow using the TensorFlow Model Optimization Toolkit.

The output from this example will be a TensorFlow Lite model file where weights in each layer have been 'clustered' into
16 clusters during training - quantization has then been applied on top of this.

By clustering the model we can improve compression of the model file. This can be essential for deploying certain
models on systems with limited resources - such as embedded systems using an Arm Ethos NPU.

After performing clustering we do post-training quantization to quantize the model and then generate a TensorFlow Lite file.

If you are targetting an Arm Ethos-U55 NPU then the output TensorFlow Lite file will also need to be passed through the Vela
compiler for further optimizations before it can be used.

For more information on using Vela see: https://git.mlplatform.org/ml/ethos-u/ethos-u-vela.git/about/
For more information on clustering see: https://www.tensorflow.org/model_optimization/guide/clustering
"""
import pathlib

import tensorflow as tf
import tensorflow_model_optimization as tfmot

from training_utils import get_data, create_model
from post_training_quantization import post_training_quantize, evaluate_tflite_model


def prepare_for_clustering(keras_model):
    """Prepares a Keras model for clustering."""

    # Choose the number of clusters to use and how to initialize them. Using more clusters will generally
    # reduce accuracy so you will need to find the optimal number for your use-case.
    number_of_clusters = 16
    cluster_centroids_init = tfmot.clustering.keras.CentroidInitialization.LINEAR

    # Apply the clustering wrapper to the whole model so weights in every layer will get clustered. You may find that
    # to avoid too much accuracy loss only certain non-critical layers in your model should be clustered.
    clustering_ready_model = tfmot.clustering.keras.cluster_weights(keras_model,
                                                                    number_of_clusters=number_of_clusters,
                                                                    cluster_centroids_init=cluster_centroids_init)

    # We must recompile the model after making it ready for clustering.
    clustering_ready_model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
                                   loss=tf.keras.losses.sparse_categorical_crossentropy,
                                   metrics=['accuracy'])

    return clustering_ready_model


def main():
    x_train, y_train, x_test, y_test = get_data()
    model = create_model()

    # Compile and train the model first.
    # In general it is easier to do clustering as a fine-tuning step after the model is fully trained.
    model.compile(optimizer=tf.keras.optimizers.Adam(learning_rate=0.001),
                  loss=tf.keras.losses.sparse_categorical_crossentropy,
                  metrics=['accuracy'])

    model.fit(x=x_train, y=y_train, batch_size=128, epochs=5, verbose=1, shuffle=True)

    # Test the trained model accuracy.
    test_loss, test_acc = model.evaluate(x_test, y_test)
    print(f"Test accuracy before clustering: {test_acc:.3f}")

    # Prepare the model for clustering.
    clustered_model = prepare_for_clustering(model)

    # Continue training the model but now with clustering applied.
    clustered_model.fit(x=x_train, y=y_train, batch_size=128, epochs=1, verbose=1, shuffle=True)
    test_loss, test_acc = clustered_model.evaluate(x_test, y_test)
    print(f"Test accuracy after clustering: {test_acc:.3f}")

    # Remove all variables that clustering only needed in the training phase.
    model_for_export = tfmot.clustering.keras.strip_clustering(clustered_model)

    # Apply post-training quantization on top of the clustering and save the resulting TensorFlow Lite model to file.
    tflite_model = post_training_quantize(model_for_export, x_train)

    tflite_models_dir = pathlib.Path('./conditioned_models/')
    tflite_models_dir.mkdir(exist_ok=True, parents=True)

    clustered_quant_model_save_path = tflite_models_dir / 'clustered_post_training_quant_model.tflite'
    with open(clustered_quant_model_save_path, 'wb') as f:
        f.write(tflite_model)

    # Test the clustered quantized model accuracy. Save time by only testing a subset of the whole data.
    num_test_samples = 1000
    evaluate_tflite_model(clustered_quant_model_save_path, x_test[0:num_test_samples], y_test[0:num_test_samples])


if __name__ == "__main__":
    main()
