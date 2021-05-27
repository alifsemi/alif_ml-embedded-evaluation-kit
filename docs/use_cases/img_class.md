# Image Classification Code Sample

- [Image Classification Code Sample](#image-classification-code-sample)
  - [Introduction](#introduction)
    - [Prerequisites](#prerequisites)
  - [Building the code sample application from sources](#building-the-code-sample-application-from-sources)
    - [Build options](#build-options)
    - [Build process](#build-process)
    - [Add custom input](#add-custom-input)
    - [Add custom model](#add-custom-model)
  - [Setting up and running Ethos-U55 code sample](#setting-up-and-running-ethos-u55-code-sample)
    - [Setting up the Ethos-U55 Fast Model](#setting-up-the-ethos-u55-fast-model)
    - [Starting Fast Model simulation](#starting-fast-model-simulation)
    - [Running Image Classification](#running-image-classification)

## Introduction

This document describes the process of setting up and running the Arm® Ethos™-U55 Image Classification
example.

Use case solves classical computer vision problem: image classification. The ML sample was developed using MobileNet v2
model trained on ImageNet dataset.

Use case code could be found in [source/use_case/img_class](../../source/use_case/img_class]) directory.

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the code sample application from sources

### Build options

In addition to the already specified build option in the main documentation, Image Classification use case specifies:

- `img_class_MODEL_TFLITE_PATH` - Path to the NN model file in TFLite format. Model will be processed and included into
    the application axf file. The default value points to one of the delivered set of models. Note that the parameters
    `img_class_LABELS_TXT_FILE`,`TARGET_PLATFORM` and `ETHOS_U55_ENABLED` should be aligned with the chosen model, i.e.:
  - if `ETHOS_U55_ENABLED` is set to `On` or `1`, the NN model is assumed to be optimized. The model will naturally
    fall back to the Arm® Cortex®-M CPU if an unoptimized model is supplied.
  - if `ETHOS_U55_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized
    model in this case will result in a runtime error.

- `img_class_FILE_PATH`: Path to the directory containing images, or path to a single image file, to be used file(s) in
    the application. The default value points to the resources/img_class/samples folder containing the delivered
    set of images. See more in the [Add custom input data section](#add-custom-input).

- `img_class_IMAGE_SIZE`: The NN model requires input images to be of a specific size. This parameter defines the
    size of the image side in pixels. Images are considered squared. Default value is 224, which is what the supplied
    MobilenetV2-1.0 model expects.

- `img_class_LABELS_TXT_FILE`: Path to the labels' text file to be baked into the application. The file is used to
    map classified classes index to the text label. Change this parameter to point to the custom labels file to map
    custom NN model output correctly.\
    The default value points to the delivered labels.txt file inside the delivery package.

- `img_class_ACTIVATION_BUF_SZ`: The intermediate/activation buffer size reserved for the NN model. By default, it
    is set to 2MiB and should be enough for most models.

- `USE_CASE_BUILD`: set to img_class to build only this example.

In order to build **ONLY** Image Classification example application add to the `cmake` command line specified in
[Building](../documentation.md#Building) `-DUSE_CASE_BUILD=img_class`.

### Build process

> **Note:** This section describes the process for configuring the build for `MPS3: SSE-300` for different target platform
see [Building](../documentation.md#Building).

Create a build directory folder and navigate inside:

```commandline
mkdir build_img_class && cd build_img_class
```

On Linux, execute the following command to build **only** Image Classification application to run on the Ethos-U55 Fast
Model when providing only the mandatory arguments for CMake configuration:

```commandline
cmake ../ -DUSE_CASE_BUILD=img_class
```

To configure a build that can be debugged using Arm-DS, we can just specify
the build type as `Debug` and use the `Arm Compiler` toolchain file:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_CASE_BUILD=img_class
```

Also see:

- [Configuring with custom TPIP dependencies](../sections/building.md#configuring-with-custom-tpip-dependencies)
- [Using Arm Compiler](../sections/building.md#using-arm-compiler)
- [Configuring the build for simple_platform](../sections/building.md#configuring-the-build-for-simple_platform)
- [Working with model debugger from Arm FastModel Tools](../sections/building.md#working-with-model-debugger-from-arm-fastmodel-tools)

> **Note:** If re-building with changed parameters values, it is highly advised to clean the build directory and re-run
>the CMake command.

If the CMake command succeeds, build the application as follows:

```commandline
make -j4
```

Add VERBOSE=1 to see compilation and link details.

Results of the build will be placed under `build/bin` folder:

```tree
bin
 ├── ethos-u-img_class.axf
 ├── ethos-u-img_class.htm
 ├── ethos-u-img_class.map
 └── sectors
      ├── images.txt
      └── img_class
           ├── dram.bin
           └── itcm.bin
```

Where:

- `ethos-u-img_class.axf`: The built application binary for the Image Classification use case.

- `ethos-u-img_class.map`: Information from building the application (e.g. libraries used, what was optimized, location
    of objects)

- `ethos-u-img_class.htm`: Human readable file containing the call graph of application functions.

- `sectors/img_class`: Folder containing the built application, split into files for loading into different FPGA memory regions.

- `sectors/images.txt`: Tells the FPGA which memory regions to use for loading the binaries in sectors/** folder.

### Add custom input

The application performs inference on input data found in the folder, or an individual file set by the CMake parameter
img_class_FILE_PATH.

To run the application with your own images, first create a folder to hold them and then copy the custom images into
this folder, for example:

```commandline
mkdir /tmp/custom_images

cp custom_image1.bmp /tmp/custom_images/
```

> **Note:** Clean the build directory before re-running the CMake command.

Next set `img_class_FILE_PATH` to the location of this folder when building:

```commandline
cmake .. \
    -Dimg_class_FILE_PATH=/tmp/custom_images/ \
    -DUSE_CASE_BUILD=img_class
```

The images found in the `img_class_FILE_PATH` folder will be picked up and automatically converted to C++ files during
the CMake configuration stage and then compiled into the application during the build phase for performing inference
with.

The log from the configuration stage should tell you what image directory path has been used:

```log
-- User option img_class_FILE_PATH is set to /tmp/custom_images
-- User option img_class_IMAGE_SIZE is set to 224
...
-- Generating image files from /tmp/custom_images
++ Converting custom_image1.bmp to custom_image1.cc
...
-- Defined build user options:
...
-- img_class_FILE_PATH=/tmp/custom_images
-- img_class_IMAGE_SIZE=224
```

After compiling, your custom images will have now replaced the default ones in the application.

> **Note:** The CMake parameter IMAGE_SIZE should match the model input size. When building the application,
if the size of any image does not match IMAGE_SIZE then it will be rescaled and padded so that it does.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter MODEL_TFLITE_PATH.

> **Note:** If you want to run the model using Ethos-U55, ensure your custom model has been run through the Vela compiler
>successfully before continuing. See [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

To run the application with a custom model you will need to provide a labels_<model_name>.txt file of labels
associated with the model. Each line of the file should correspond to one of the outputs in your model. See the provided
labels_mobilenet_v2_1.0_224.txt file for an example.

Then, you must set `img_class_MODEL_TFLITE_PATH` to the location of the Vela processed model file and
`img_class_LABELS_TXT_FILE` to the location of the associated labels file.

An example:

```commandline
cmake .. \
    -Dimg_class_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
    -Dimg_class_LABELS_TXT_FILE=<path/to/labels_custom_model.txt> \
    -DUSE_CASE_BUILD=img_class
```

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` model file pointed to by `img_class_MODEL_TFLITE_PATH` and labels text file pointed to by
`img_class_LABELS_TXT_FILE` will be converted to C++ files during the CMake configuration stage and then compiled into
the application for performing inference with.

The log from the configuration stage should tell you what model path and labels file have been used:

```log
-- User option img_class_MODEL_TFLITE_PATH is set to <path/to/custom_model_after_vela.tflite>
...
-- User option img_class_LABELS_TXT_FILE is set to <path/to/labels_custom_model.txt>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to\
custom_model_after_vela.tflite.cc
-- Generating labels file from <path/to/labels_custom_model.txt>
-- writing to <path/to/build/generated/src/Labels.cc>
...
```

After compiling, your custom model will have now replaced the default one in the application.

## Setting up and running Ethos-U55 code sample

### Setting up the Ethos-U55 Fast Model

The FVP is available publicly from [Arm Ecosystem FVP downloads](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For Ethos-U55 evaluation, please download the MPS3 version of the Arm® Corstone™-300 model that contains Ethos-U55 and
Cortex-M55. The model is currently only supported on Linux based machines. To install the FVP:

- Unpack the archive

- Run the install script in the extracted package

```commandline
$./FVP_Corstone_SSE-300_Ethos-U55.sh
```

- Follow the instructions to install the FVP to your desired location

### Starting Fast Model simulation

Pre-built application binary ethos-u-img_class.axf can be found in the bin/mps3-sse-300 folder of the delivery package.
Assuming the install location of the FVP was set to ~/FVP_install_location, the simulation can be started by:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55
./bin/mps3-sse-300/ethos-u-img_class.axf
```

A log output should appear on the terminal:

```log
telnetterminal0: Listening for serial connection on port 5000
telnetterminal1: Listening for serial connection on port 5001
telnetterminal2: Listening for serial connection on port 5002
telnetterminal5: Listening for serial connection on port 5003
```

This will also launch a telnet window with the sample application's standard output and error log entries containing
information about the pre-built application version, TensorFlow Lite Micro library version used, data type as well as
the input and output tensor sizes of the model compiled into the executable binary.

After the application has started if `img_class_FILE_PATH` pointed to a single file (or a folder containing a single image)
the inference starts immediately. In case of multiple inputs choice, it outputs a menu and waits for the user input from
telnet terminal:

```log
User input required
Enter option number from:

1. Classify next image
2. Classify image at chosen index
3. Run classification on all images
4. Show NN model info
5. List images

Choice:

```

1. “Classify next image” menu option will run single inference on the next in line image from the collection of the
    compiled images.

2. “Classify image at chosen index” menu option will run single inference on the chosen image.

    > **Note:** Please make sure to select image index in the range of supplied images during application build.
    By default, pre-built application has 4 images, indexes from 0 to 3.

3. “Run classification on all images” menu option triggers sequential inference executions on all built-in images.

4. “Show NN model info” menu option prints information about model data type, input and output tensor sizes:

    ```log
    INFO - uTFL version: 2.5.0
    INFO - Model info:
    INFO - Model INPUT tensors:
    INFO - 	tensor type is UINT8
    INFO - 	tensor occupies 150528 bytes with dimensions
    INFO - 		0:   1
    INFO - 		1: 224
    INFO - 		2: 224
    INFO - 		3:   3
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.007812
    INFO - ZeroPoint[0] = 128
    INFO - Model OUTPUT tensors:
    INFO - 	tensor type is UINT8
    INFO - 	tensor occupies 1001 bytes with dimensions
    INFO - 		0:   1
    INFO - 		1: 1001
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.098893
    INFO - ZeroPoint[0] = 58
    INFO - Activation buffer (a.k.a tensor arena) size used: 521760
    INFO - Number of operators: 1
    INFO - 	Operator 0: ethos-u
    ```

5. “List Images” menu option prints a list of pair image indexes - the original filenames embedded in the application:

    ```log
    INFO - List of Files:
    INFO - 0 => cat.bmp
    INFO - 1 => dog.bmp
    INFO - 2 => kimono.bmp
    INFO - 3 => tiger.bmp
    ```

### Running Image Classification

Please select the first menu option to execute Image Classification.

The following example illustrates application output for classification:

```log
INFO - Running inference on image 0 => cat.bmp
INFO - Final results:
INFO - Total number of inferences: 1
INFO - 0) 282 (14.636096) -> tabby, tabby cat
INFO - 1) 286 (14.537203) -> Egyptian cat
INFO - 2) 283 (12.757138) -> tiger cat
INFO - 3) 458 (7.021370) -> bow tie, bow-tie, bowtie
INFO - 4) 288 (7.021370) -> lynx, catamount
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 2489726
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 1098726
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 471129
INFO - NPU ACTIVE cycles: 7489258
INFO - NPU IDLE cycles: 914
INFO - NPU total cycles: 7490172
```

It could take several minutes to complete one inference run (average time is 2-3 minutes).

The log shows the inference results for “image 0” (0 - index) that corresponds to “cat.bmp” in the sample image resource
folder.

The profiling section of the log shows that for this inference:

- Ethos-U55's PMU report:

  - 7,490,172 total cycle: The number of NPU cycles

  - 7,489,258 active cycles: number of NPU cycles that were used for computation

  - 914 idle cycles: number of cycles for which the NPU was idle

  - 2,489,726 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus.
    AXI0 is the bus where Ethos-U55 NPU reads and writes to the computation buffers (activation buf/tensor arenas).

  - 1,098,726 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 471,129 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus.
    AXI1 is the bus where Ethos-U55 NPU reads the model (read only)

- For FPGA platforms, CPU cycle count can also be enabled. For FVP, however, CPU cycle counters should not be used as
    the CPU model is not cycle-approximate or cycle-accurate.

The application prints the top 5 classes with indexes, confidence score and labels from associated
labels_mobilenet_v2_1.0_224.txt file. The FVP window also shows the output on its LCD section.
