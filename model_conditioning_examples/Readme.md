# Model conditioning examples

- [Introduction](#introduction)
  - [How to run](#how-to-run)
- [Quantization](#quantization)
  - [Post-training quantization](#post-training-quantization)
  - [Quantization aware training](#quantization-aware-training)
- [Weight pruning](#weight-pruning)
- [Weight clustering](#weight-clustering)
- [References](#references)

## Introduction

This folder contains short example scripts that demonstrate some methods available in TensorFlow to condition your model
in preparation for deployment on Arm® Ethos™ NPU.

These scripts will cover three main topics:

- Quantization
- Weight clustering
- Weight pruning

The objective of these scripts is not to be a single source of knowledge on everything related to model conditioning.
Instead the aim is to provide the reader with a quick starting point that demonstrates some commonly used tools that
will enable models to run on Arm® Ethos-U NPU and also optimize them to enable maximum performance from the Arm® Ethos-U NPU.

Links to more in-depth guides available on the TensorFlow website are provided in the [references](#references) section
in this Readme.

### How to run

From the `model_conditioning_examples` folder run the following command:

```commandline
./setup.sh
```

This will create a Python virtual environment and install the required versions of TensorFlow and TensorFlow model
optimization toolkit to run the examples scripts.

If the virtual environment has not been activated you can do so by running:

```commandline
source ./env/bin/activate
```

You can then run the examples from the command line. For example to run the post-training quantization example:

```commandline
python ./post_training_quantization.py
```

The produced TensorFlow Lite model files will be saved in a `conditioned_models` sub-folder.

## Quantization

Most machine learning models are trained using 32bit floating point precision. However, Arm® Ethos-U NPU performs
calculations in 8bit integer precision. As a result, it is required that any model you wish to deploy on Arm® Ethos-U NPU is
first fully quantized to 8bits.

TensorFlow provides two methods of quantization and the scripts in this folder will demonstrate these:

- [Post-training quantization](./post_training_quantization.py)
- [Quantization aware training](./quantization_aware_training.py)

Both of these techniques will not only quantize weights of the the model but also the variable tensors such as model
input and output, and the activations of each intermediate layer.

For details on the quantization specification used by TensorFlow please see
[here](https://www.tensorflow.org/lite/performance/quantization_spec).

In both methods scale and zero point values are chosen to allow the floating point weights to be maximally
represented in this reduced precision. Quantization is performed per-axis, meaning a different scale and zero point
is used for each channel of a layer's weights.

### Post-training quantization

The first of the quantization methods that will be covered is called post-training quantization. As the name suggests
this form of quantization takes place after training of your model is complete. It is also the simpler of the methods
we will show to quantize a model.

In post-training quantization, first the weights of the model are quantized to 8bit integer values. After this we
quantize the variable tensors, such as layer activations. To do this we need to calculate the potential range of values
that all these tensors can take.

Calculating these ranges requires a small dataset that is representative of what you expect your model to see when
it is deployed. Model inference is then performed using this representative dataset and the resulting minimum and
maximum values for variable tensors are calculated.

Only a small number of samples need to be used in this calibration dataset (around 100-500 should be enough). These
samples can be taken from the training or validation sets.

Quantizing your model can result in accuracy drops depending on your model. However for a lot of use cases the accuracy
drop when using post-training quantization is usually minimal. After post-training quantization is complete you will
have a fully quantized TensorFlow Lite model.

If you are targetting an Arm® Ethos-U NPU then the output TensorFlow Lite file will also need to be passed through the Vela
compiler for further optimizations before it can be used.

### Quantization aware training

Depending on the model, the use of post-training quantization can result in an accuracy drop that is too large to be
considered suitable. This is where quantization aware training can be used to improve things. Quantization aware
training simulates the quantization of weights and activations during the inference stage of training using fake
quantization nodes.

By simulating quantization during training, the model weights will be adjusted in the backward pass so that they are
better suited for the reduced precision of quantization. It is this simulating of quantization and adjusting of weights
that can minimize accuracy loss incurred when quantizing. Note that quantization is only simulated
at this stage and backward passes of training are still performed in full floating point precision.

Importantly, with quantization aware training you do not have to train your model from scratch to use it. Instead, you
can train it normally (not quantization aware) and after training is complete you can then fine-tune it using
quantization aware training. By only fine-tuning you can save a lot of training time.

As well as simulating quantization and adjusting weights, the ranges for variable tensors are captured so that the
model can be fully quantized afterwards. Once you have finished quantization aware training the TensorFlow Lite converter is
used to produce a fully quantized TensorFlow Lite model.

If you are targetting an Arm® Ethos-U NPU then the output TensorFlow Lite file will also need to be passed through the Vela
compiler for further optimizations before it can be used.

## Weight pruning

After you have trained your deep learning model it is common to see that many of the weights in the model
have the value of 0, and also have many values very close to 0. These weights have very little effect in network
calculations so are safe to be removed or 'pruned' from the model. This is accomplished by setting all these weight
values to 0, resulting in a sparse model.

Compression algorithms can then take advantage of this to reduce model size in memory, which can be very important when
deploying on small embedded systems. Moreover, Arm® Ethos-U NPU can take advantage of model sparsity to further accelerate
execution of a model.

Training with weight pruning will force your model to have a certain percentage of its weights set (or 'pruned') to 0
during the training phase. This is done by forcing those that are closest to 0 to become 0. Doing it during training
guarantees your model will have a certain level of sparsity and the weights of your model can also be better adapted
to the sparsity level chosen. This means, accuracy loss will hopefully be minimized if a large pruning percentage
is desired.

Weight pruning can be further combined with quantization so you have a model that is both pruned and quantized, meaning
that the memory saving affects of both can be combined. Quantization then allows the model to be used with
Arm® Ethos-U NPU.

If you are targetting an Arm® Ethos-U NPU then the output TensorFlow Lite file will also need to be passed through the Vela
compiler for further optimizations before it can be used.

## Weight clustering

Another method of model conditioning is weight clustering (also called weight sharing). With this technique, a fixed
number of values (cluster centers) are used in each layer of a model to represent all the possible values that the
layer's weights take. The weights in a layer will then use the value of their closest cluster center. By restricting
the number of possible clusters, weight clustering reduces the amount of memory needed to store all the weight values
in a model.

Depending on the model and number of clusters chosen, using this kind of technique can have a negative effect on
accuracy. To reduce the impact on accuracy you can introduce clustering during training so the models weights can be
better adjusted to the reduced precision.

Weight clustering can be further combined with quantization so you have a model that is both clustered and quantized,
meaning that the memory saving affects of both can be combined. Quantization then allows the model to be used with
Arm® Ethos-U NPU.

If you are targetting an Arm® Ethos-U NPU then the output TensorFlow Lite file will also need to be passed through the Vela
compiler for further optimizations before it can be used (see [Optimize model with Vela compiler](./building.md#optimize-custom-model-with-vela-compiler)).

## References

- [TensorFlow Model Optimization Toolkit](https://www.tensorflow.org/model_optimization)
- [Post-training quantization](https://www.tensorflow.org/lite/performance/post_training_integer_quant)
- [Quantization aware training](https://www.tensorflow.org/model_optimization/guide/quantization/training)
- [Weight pruning](https://www.tensorflow.org/model_optimization/guide/pruning)
- [Weight clustering](https://www.tensorflow.org/model_optimization/guide/clustering)
- [Vela](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela)
