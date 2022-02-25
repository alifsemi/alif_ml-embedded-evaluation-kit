# Object Detection Code Sample

- [Object Detection Code Sample](./object_detection.md#object-detection-code-sample)
  - [Introduction](./object_detection.md#introduction)
    - [Prerequisites](./object_detection.md#prerequisites)
  - [Building the code sample application from sources](./object_detection.md#building-the-code-sample-application-from-sources)
    - [Build options](./object_detection.md#build-options)
    - [Build process](./object_detection.md#build-process)
    - [Add custom input](./object_detection.md#add-custom-input)
    - [Add custom model](./object_detection.md#add-custom-model)
  - [Setting up and running Ethos-U NPU code sample](./object_detection.md#setting-up-and-running-ethos_u-npu-code-sample)
    - [Setting up the Ethos-U NPU Fast Model](./object_detection.md#setting-up-the-ethos_u-npu-fast-model)
    - [Starting Fast Model simulation](./object_detection.md#starting-fast-model-simulation)
    - [Running Object Detection](./object_detection.md#running-object-detection)

## Introduction

This document describes the process of setting up and running the Arm® *Ethos™-U* NPU Object Detection example.
Object Detection is a classical computer vision use case in which specific objects need to be identified and located
within a full frame. In this specific example the model was trained for face detection. The ML sample was developed
using the *YOLO Fastest* model.  To adopt the model for low power / low memory systems the input images to the model
are monochrome images. The model was trained on the *Wider* dataset (after conversion from RGB to monochrome)
and on *Emza Visual-Sense* dataset <www.emza-vs.com>.
The model makes detection faces in size of 20x20 pixels and above.

Use-case code could be found in the following directory:[source/use_case/object_detection](../../source/use_case/object_detection).

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the code sample application from sources

### Build options

In addition to the already specified build option in the main documentation, the Object Detection use-case
specifies:

- `object_detection_MODEL_TFLITE_PATH` - The path to the NN model file in the *TFLite* format. The model is then processed and
  included in the application `axf` file. The default value points to one of the delivered set of models.
  Note that the parameters `TARGET_PLATFORM`, and `ETHOS_U_NPU_ENABLED` must be aligned with
  the chosen model. In other words:
  - If `ETHOS_U_NPU_ENABLED` is set to `On` or `1`, then the NN model is assumed to be optimized. The model naturally
    falls back to the Arm® *Cortex®-M* CPU if an unoptimized model is supplied.
  - if `ETHOS_U_NPU_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized
    model in this case results in a runtime error.

- `object_detection_FILE_PATH`: The path to the directory containing the images, or a path to a single image file, that is to
   be used in the application. The default value points to the `resources/object_detection/samples` folder containing the
   delivered set of images.
   For further information, please refer to: [Add custom input data section](./object_detection.md#add-custom-input).

- `object_detection_IMAGE_SIZE`: The NN model requires input images to be of a specific size. This parameter defines the size
  of the image side in pixels. Images are considered squared. The default value is `192`, which is what the supplied
  *YOLO Fastest* model expects.

- `object_detection_ANCHOR_1`: First anchor  array estimated during *YOLO Fastest* model training.

- `object_detection_ANCHOR_2`: Second anchor array estimated during *YOLO Fastest* model training.

- `object_detection_CHANNELS_IMAGE_DISPLAYED`: The user can decide if display the image on the LCD screen in grayscale or RGB.
  This parameter defines the number of channel to use: 1 for grayscale, 3 for RGB. The default value is `3`.

- `object_detection_ACTIVATION_BUF_SZ`: The intermediate, or activation, buffer size reserved for the NN model.
  By default, it is set to 2MiB and is enough for most models.

To build **ONLY** the Object Detection example application, add `-DUSE_CASE_BUILD=object_detection` to the `cmake` command
line, as specified in: [Building](../documentation.md#Building).

### Build process

> **Note:** This section describes the process for configuring the build for the *MPS3: SSE-300*. To build for a
> different target platform, please refer to: [Building](../documentation.md#Building).

To **only** build the `object_detection` example, create a build directory, and then navigate inside.

```commandline
mkdir build_object_detection && cd build_object_detection
```

On Linux, when providing only the mandatory arguments for the CMake configuration, execute the following command to
build **only** Object Detection application to run on the *Ethos-U55* Fast Model:

```commandline
cmake ../ -DUSE_CASE_BUILD=object_detection
```

To configure a build that can be debugged using Arm DS, we specify the build type as `Debug` and use the `Arm Compiler`
toolchain file:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_CASE_BUILD=object_detection
```

For further information, please refer to:

- [Configuring with custom TPIP dependencies](../sections/building.md#configuring-with-custom-tpip-dependencies)
- [Using Arm Compiler](../sections/building.md#using-arm-compiler)
- [Configuring the build for simple-platform](../sections/building.md#configuring-the-build-for-simple_platform)
- [Building for different Ethos-U NPU variants](../sections/building.md#building-for-different-ethos_u-npu-variants)

> **Note:** If re-building with changed parameters values, we recommend that you clean the build directory and re-run
> the CMake command.

If the CMake command succeeds, build the application as follows:

```commandline
make -j4
```

To see compilation and link details, add `VERBOSE=1`.

Results of the build are placed under the `build/bin` folder, like so:

```tree
bin
 ├── ethos-u-object_detection.axf
 ├── ethos-u-object_detection.htm
 ├── ethos-u-object_detection.map
 └── sectors
      ├── images.txt
      └── object_detection
           ├── ddr.bin
           └── itcm.bin
```

The `bin` folder contains the following files:

- `ethos-u-object_detection.axf`: The built application binary for the Object Detection use-case.

- `ethos-u-object_detection.map`: Information from building the application. For example: The libraries used, what was
  optimized, and the location of objects.

- `ethos-u-object_detection.htm`: Human readable file containing the call graph of application functions.

- `sectors/object_detection`: Folder containing the built application. It is split into files for loading into different FPGA memory
  regions.

- `Images-object_detection.txt`: Tells the FPGA which memory regions to use for loading the binaries
  in the `sectors/...` folder.

### Add custom input

The application object detection is set up to perform inferences on data found in the folder, or an individual file,
that is pointed to by the parameter `object_detection_FILE_PATH`.

To run the application with your own images, first create a folder to hold them and then copy the custom images into the
following folder:

```commandline
mkdir /tmp/custom_images

cp custom_image1.bmp /tmp/custom_images/
```

> **Note:** Clean the build directory before re-running the CMake command.

Next, set `object_detection_FILE_PATH` to the location of this folder when building:

```commandline
cmake .. \
    -Dobject_detection_FILE_PATH=/tmp/custom_images/ \
    -DUSE_CASE_BUILD=object_detection
```

The images found in the `object_detection_FILE_PATH` folder are picked up and automatically converted to C++ files during the
CMake configuration stage. They are then compiled into the application during the build phase for performing inference
with.

The log from the configuration stage tells you what image directory path has been used:

```log
-- User option object_detection_FILE_PATH is set to /tmp/custom_images
-- User option object_detection_IMAGE_SIZE is set to 192
...
-- Generating image files from /tmp/custom_images
++ Converting custom_image1.bmp to custom_image1.cc
...
-- Defined build user options:
...
-- object_detection_FILE_PATH=/tmp/custom_images
-- object_detection_IMAGE_SIZE=192
```

After compiling, your custom images have now replaced the default ones in the application.

> **Note:** The CMake parameter `IMAGE_SIZE` must match the model input size. When building the application, if the size
of any image does not match `IMAGE_SIZE`, then it is rescaled and padded so that it does.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter `object_detection_MODEL_TFLITE_PATH`.

> **Note:** If you want to run the model using an *Ethos-U*, ensure that your custom model has been successfully run
> through the Vela compiler *before* continuing.

For further information: [Optimize model with Vela compiler](../sections/building.md#Optimize-custom-model-with-Vela-compiler).

For example:

```commandline
cmake .. \
    -Dobject_detection_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
    -DUSE_CASE_BUILD=object_detection
```

> **Note:** Clean the build directory before re-running the CMake command.

The `.tflite` model file pointed to by `object_detection_MODEL_TFLITE_PATH` is converted to
C++ files during the CMake configuration stage. They are then compiled into
the application for performing inference with.

The log from the configuration stage tells you what model path and labels file have been used, for example:

```log
-- User option object_detection_MODEL_TFLITE_PATH is set to <path/to/custom_model_after_vela.tflite>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to custom_model_after_vela.tflite.cc
...
```

After compiling, your custom model has now replaced the default one in the application.

## Setting up and running Ethos-U NPU code sample

### Setting up the Ethos-U NPU Fast Model

The FVP is available publicly from
[Arm Ecosystem FVP downloads](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For the *Ethos-U* evaluation, please download the MPS3 based version of the Arm® *Corstone™-300* model that contains *Cortex-M55*
and offers a choice of the *Ethos-U55* and *Ethos-U65* processors.

To install the FVP:

- Unpack the archive.

- Run the install script in the extracted package:

```commandline
./FVP_Corstone_SSE-300.sh
```

- Follow the instructions to install the FVP to the required location.

### Starting Fast Model simulation

The pre-built application binary `ethos-u-object_detection.axf` can be
found in the `bin/mps3-sse-300` folder of the delivery package.

Assuming that the install location of the FVP was set to `~/FVP_install_location`, then the simulation can be started by
using:

```commandline
~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55 ./bin/mps3-sse-300/ethos-u-object_detection.axf
```

A log output appears on the terminal:

```log
telnetterminal0: Listening for serial connection on port 5000
telnetterminal1: Listening for serial connection on port 5001
telnetterminal2: Listening for serial connection on port 5002
telnetterminal5: Listening for serial connection on port 5003
```

This also launches a telnet window with the standard output of the sample application. It also includes error log
entries containing information about the pre-built application version, TensorFlow Lite Micro library version used, and
data types. The log also includes the input and output tensor sizes of the model compiled into the executable binary.

After the application has started, if `object_detection_FILE_PATH` points to a single file, or even a folder that contains a
single image, then the inference starts immediately. If there are multiple inputs, it outputs a menu and then waits for
input from the user:

```log
User input required
Enter option number from:

  1. Run detection on next ifm
  2. Run detection ifm at chosen index
  3. Run detection on all ifm
  4. Show NN model info
  5. List ifm

Choice:

```

What the preceding choices do:

1. Run detection on next ifm: Runs a single inference on the next in line image from the collection of the compiled images.

2. Run detection ifm at chosen index: Runs inference on the chosen image.

    > **Note:** Please make sure to select image index from within the range of supplied audio clips during application
    > build. By default, a pre-built application has four images, with indexes from `0` to `3`.

3. Run detection on all ifm: Triggers sequential inference executions on all built-in images.

4. Show NN model info: Prints information about the model data type, input, and output, tensor sizes. For example:

    ```log
    INFO - Allocating tensors
    INFO - Model INPUT tensors:
    INFO -  tensor type is INT8
    INFO -  tensor occupies 36864 bytes with dimensions
    INFO -    0:   1
    INFO -    1: 192
    INFO -    2: 192
    INFO -    3:   1
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.003921
    INFO - ZeroPoint[0] = -128
    INFO - Model OUTPUT tensors:
    INFO -  tensor type is INT8
    INFO -  tensor occupies 648 bytes with dimensions
    INFO -    0:   1
    INFO -    1:   6
    INFO -    2:   6
    INFO -    3:  18
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.134084
    INFO - ZeroPoint[0] = 47
    INFO -  tensor type is INT8
    INFO -  tensor occupies 2592 bytes with dimensions
    INFO -    0:   1
    INFO -    1:  12
    INFO -    2:  12
    INFO -    3:  18
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.185359
    INFO - ZeroPoint[0] = 10
    INFO - Activation buffer (a.k.a tensor arena) size used: 443992
    INFO - Number of operators: 3
    INFO -  Operator 0: ethos-u
    INFO -  Operator 1: RESIZE_NEAREST_NEIGHBOR
    INFO -  Operator 2: ethos-u
    ```

5. List Images: Prints a list of pair image indexes. The original filenames are embedded in the application, like so:

    ```log
    INFO - List of Files:
    INFO - 0 => couple.bmp
    INFO - 1 => glasses.bmp
    INFO - 2 => man_and_baby.bmp
    INFO - 3 => pitch_and_roll.bmp
    ```

### Running Object Detection

Please select the first menu option to execute Object Detection.

The following example illustrates an application output for detection:

```log
INFO - Running inference on image 0 => couple.bmp
INFO - Final results:
INFO - Total number of inferences: 1
INFO - 0)  (0.999246) -> Detection box: {x=89,y=17,w=41,h=56}
INFO - 0)  (0.995367) -> Detection box: {x=27,y=81,w=48,h=53}
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 678955
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 467545
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 46895
INFO - NPU ACTIVE cycles: 2050339
INFO - NPU IDLE cycles: 855
INFO - NPU TOTAL cycles: 2051194
```

It can take several minutes to complete one inference run. The average time is around 10-20 seconds.

The log shows the inference results for `image 0`, so `0` - `index`, that corresponds to `couple.bmp` in the sample image
resource folder.

The profiling section of the log shows that for this inference:

- *Ethos-U* PMU report:

  - 2,051,194 total cycle: The number of NPU cycles.

  - 2,050,339 active cycles: The number of NPU cycles that were used for computation.

  - 855 idle cycles: The number of cycles for which the NPU was idle.

  - 678,955 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus. AXI0 is the bus where the
    *Ethos-U* NPU reads and writes to the computation buffers, activation buf, or tensor arenas.

  - 467,545 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 46,895 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus. AXI1 is the bus where the
    *Ethos-U* NPU reads the model. So, read-only.

- For FPGA platforms, a CPU cycle count can also be enabled. However, do not use cycle counters for FVP, as the CPU
  model is not cycle-approximate or cycle-accurate

The application prints the detected bounding boxes for faces in image. The FVP window also shows the output on its LCD section.
