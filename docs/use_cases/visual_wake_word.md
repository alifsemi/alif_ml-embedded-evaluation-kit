# Visual Wake Word Code Sample

- [Visual Wake Word Code Sample](./visual_wake_word.md#visual-wake-word-code-sample)
  - [Introduction](./visual_wake_word.md#introduction)
    - [Prerequisites](./visual_wake_word.md#prerequisites)
  - [Building the Code Samples application from sources](./visual_wake_word.md#building-the-code-samples-application-from-sources)
    - [Build options](./visual_wake_word.md#build-options)
    - [Build process](./visual_wake_word.md#build-process)
    - [Add custom input](./visual_wake_word.md#add-custom-input)
    - [Add custom model](./visual_wake_word.md#add-custom-model)
  - [Setting up and running Ethos-U NPU code sample](./visual_wake_word.md#setting-up-and-running-ethos_u-npu-code-sample)
    - [Setting up the Ethos-U NPU Fast Model](./visual_wake_word.md#setting-up-the-ethos_u-npu-fast-model)
    - [Starting Fast Model simulation](./visual_wake_word.md#starting-fast-model-simulation)
    - [Running Visual Wake Word](./visual_wake_word.md#running-visual-wake-word)

## Introduction

This document describes the process of setting up and running the Arm® Ethos™-U NPU Visual Wake Word example.
Visual Wake Words is a common vision use-case to detect if a the provided image contains a person.

Use case code could be found in [source/use_case/vww](../../source/use_case/vww) directory.

### Prerequisites

See [Prerequisites](../documentation.md#prerequisites)

## Building the Code Samples application from sources

### Build options

In addition to the already specified build option in the main reference manual, Visual Wake Word use case specifies:

- `vww_MODEL_TFLITE_PATH` - Path to the NN model file in the `TFLite` format. The model is then processed and included in
  the application `axf` file. The default value points to one of the delivered set of models.
  Note that the parameters `vww_LABELS_TXT_FILE`, `TARGET_PLATFORM`, and `ETHOS_U_NPU_ENABLED` must be aligned with the
  chosen model. In other words:
  - If `ETHOS_U_NPU_ENABLED` is set to `On` or `1`, then the NN model is assumed to be optimized. The model naturally
      falls back to the Arm® *Cortex®-M* CPU if an unoptimized model is supplied.
  - if `ETHOS_U_NPU_ENABLED` is set to `Off` or `0`, the NN model is assumed to be unoptimized. Supplying an optimized
      model in this case results in a runtime error.

- `vww_FILE_PATH`: Path to directory or file to be used as custom image file(s) to use in the evaluation
    application. The default value points to the resources/vww/samples folder containing the delivered set
    of images. See more in the Running custom input data section.

- `vww_IMAGE_SIZE`: The NN model requires input images to be of a specific size. This parameter defines the
    size of the image side in pixels. Images are considered squared. Default value is 128, which is what the supplied
    visual wake word model expects.

- `vww_LABELS_TXT_FILE`: Path to the labels' text file to be baked into the application. The file is used
    to map classified classes index to the text label. Change this parameter to point to the custom labels file to map
    custom NN model output correctly.\
    The default value points to the delivered labels.txt file inside the delivery package.

- `vww_ACTIVATION_BUF_SZ`: The intermediate/activation buffer size reserved for the NN model. By default,
    it is set to 2MiB and should be enough for most models.

### Build process

> **Note:** This section describes the process for configuring the build for `MPS3: SSE-300` for different target
> platform see [Building](../documentation.md#Building) section.

Create a build directory and navigate inside:

```commandline
mkdir build_visual_wake_word && cd build_visual_wake_word
```

On Linux, execute the following command to build **only** Visual Wake Word application to run on the Ethos-U55 Fast
Model when providing only the mandatory arguments for CMake configuration:

```commandline
cmake ../ -DUSE_CASE_BUILD=vww
```

To configure a build that can be debugged using Arm-DS, we can just specify the build type as `Debug` and use the `Arm
Compiler` toolchain file:

```commandline
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DUSE_CASE_BUILD=vww
```

Also see:

- [Configuring with custom TPIP dependencies](../sections/building.md#configuring-with-custom-tpip-dependencies)
- [Using Arm Compiler](../sections/building.md#using-arm-compiler)
- [Configuring the build for simple-platform](../sections/building.md#configuring-the-build-for-simple_platform)
- [Building for different Ethos-U NPU variants](../sections/building.md#building-for-different-ethos-u-npu-variants)

> **Note:** If re-building with changed parameters values, it is highly advised to clean the build directory and re-run
>the CMake command.

If the CMake command succeeded, build the application as follows:

```commandline
make -j4
```

Add VERBOSE=1 to see compilation and link details.

Results of the build will be placed under `build/bin` folder:

```tree
bin
 ├── ethos-u-vww.axf
 ├── ethos-u-vww.htm
 ├── ethos-u-vww.map
 ├── images-vww.txt
 └── sectors
      └── vww
           ├── ddr.bin
           └── itcm.bin
```

Where:

- `ethos-u-vww.axf`: The built application binary for the Visual Wake Word use case.

- `ethos-u-vww.map`: Information from building the application (e.g. libraries used, what was optimized,
    location of objects)

- `ethos-u-vww.htm`: Human readable file containing the call graph of application functions.

- `sectors/`: Folder containing the built application, split into files for loading into different FPGA memory regions.

- `Images-vww.txt`: Tells the FPGA which memory regions to use for loading the binaries in sectors/**
  folder.

### Add custom input

The application performs inference on image data found in the folder set by the CMake parameter
`vww_FILE_PATH`.

To run the application with your own images first create a folder to hold them and then copy the custom images into this
folder:

```commandline
mkdir /tmp/custom_images

cp custom_image1.bmp /tmp/custom_images/
```

> **Note:** Clean the build directory before re-running the cmake command.

Next set `vww_FILE_PATH` to the location of this folder when building:

```commandline
cmake .. \
    -Dvww_FILE_PATH=/tmp/custom_images/ \
    -DUSE_CASE_BUILD=vww
```

The images found in the `vww_FILE_PATH` folder will be picked up and automatically converted to C++ files
during the CMake configuration stage and then compiled into the application during the build phase for performing
inference with.

The log from the configuration stage should tell you what image directory path has been used:

```log
-- User option vww_FILE_PATH is set to /tmp/custom_images
-- User option vww_IMAGE_SIZE is set to 128
...
-- Generating image files from /tmp/custom_images
++ Converting custom_image1.bmp to custom_image1.cc
...
-- Defined build user options:
...
-- vww_FILE_PATH=/tmp/custom_images
-- vww_IMAGE_SIZE=128
```

After compiling, your custom images will have now replaced the default ones in the application.

> **Note:** The CMake parameter vww_IMAGE_SIZE should match the model input size. When building the
> application, if the size of any image does not match IMAGE_SIZE then it will be rescaled and padded so that it does.

### Add custom model

The application performs inference using the model pointed to by the CMake parameter
`vww_MODEL_TFLITE_PATH`.

> **Note:** If you want to run the model using Ethos-U, ensure your custom model has been run through the Vela compiler
> successfully before continuing.

To run the application with a custom model you will need to provide a labels_<model_name>.txt file of labels associated
with the model. Each line of the file should correspond to one of the outputs in your model. See the provided
visual_wake_word_labels.txt file for an example.

Then, you must set `vww_MODEL_TFLITE_PATH` to the location of the Vela processed model file and
`vww_LABELS_TXT_FILE` to the location of the associated labels file.

An example:

```commandline
cmake \
    -Dvww_MODEL_TFLITE_PATH=<path/to/custom_model_after_vela.tflite> \
    -Dvww_LABELS_TXT_FILE=<path/to/labels_custom_model.txt> \
    -DUSE_CASE_BUILD=vww ..
```

> **Note:** Clean the build directory before re-running the cmake command.

The TFLite model pointed to by `vww_MODEL_TFLITE_PATH` and labels text file pointed to by
`vww_LABELS_TXT_FILE` will be converted to C++ files during the CMake configuration stage and then compiled
into the application for performing inference with.

The log from the configuration stage should tell you what model path and labels file have been used:

```log
-- User option vww_MODEL_TFLITE_PATH is set to <path/to/custom_model_after_vela.tflite>
...
-- User option vww_LABELS_TXT_FILE is set to <path/to/labels_custom_model.txt>
...
-- Using <path/to/custom_model_after_vela.tflite>
++ Converting custom_model_after_vela.tflite to custom_model_after_vela.tflite.cc
-- Generating labels file from <path/to/labels_custom_model.txt>
-- writing to <path/to/build/generated/src/Labels.cc>
...
```

After compiling, your custom model will have now replaced the default one in the application.

## Setting up and running Ethos-U NPU code sample

### Setting up the Ethos-U NPU Fast Model

The FVP is available publicly from
[Arm Ecosystem FVP downloads](https://developer.arm.com/tools-and-software/open-source-software/arm-platforms-software/arm-ecosystem-fvps).

For the *Ethos-U* evaluation, please download the MPS3 based version of the Arm® *Corstone™-300* model that contains *Cortex-M55*
and offers a choice of the *Ethos-U55* and *Ethos-U65* processors.

- Unpack the archive

- Run the install script in the extracted package

```commandline
$./FVP_Corstone_SSE-300.sh
```

- Follow the instructions to install the FVP to your desired location

### Starting Fast Model simulation

Pre-built application binary ethos-u-vww.axf can be found in the bin/mps3-sse-300 folder of the delivery
package. Assuming the install location of the FVP was set to ~/FVP_install_location, the simulation can be started by:

```commandline
$ ~/FVP_install_location/models/Linux64_GCC-6.4/FVP_Corstone_SSE-300_Ethos-U55 ./bin/mps3-sse-300/ethos-u-vww.axf
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

After the application has started if `vww_FILE_PATH` pointed to a single file (or a folder containing a
single image) the inference starts immediately. In case of multiple inputs choice, it outputs a menu and waits for the
user input from telnet terminal:

```log
User input required
Enter option number from:

  1. Classify next ifm
  2. Classify ifm at chosen index
  3. Run classification on all ifm
  4. Show NN model info
  5. List ifm

Choice:

```

1. “Classify next image” menu option will run single inference on the next in line image from the collection of the
    compiled images.

2. “Classify image at chosen index” menu option will run single inference on the chosen image.

    > **Note:** Please make sure to select image index in the range of supplied images during application build. By
    default, pre-built application has 2 images, index should 0 or 1.

3. “Run classification on all images” menu option triggers sequential inference executions on all built-in images.

4. “Show NN model info” menu option prints information about model data type, input and output tensor sizes:

    ```log
    INFO - Added ethos-u support to op resolver
    INFO - Creating allocator using tensor arena in SRAM
    INFO - Allocating tensors
    INFO - Model INPUT tensors:
    INFO -  tensor type is INT8
    INFO -  tensor occupies 16384 bytes with dimensions
    INFO -          0:   1
    INFO -          1: 128
    INFO -          2: 128
    INFO -          3:   1
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.008138
    INFO - ZeroPoint[0] = -70
    INFO - Model OUTPUT tensors:
    INFO -  tensor type is INT8
    INFO -  tensor occupies 2 bytes with dimensions
    INFO -          0:   1
    INFO -          1:   2
    INFO - Quant dimension: 0
    INFO - Scale[0] = 0.022299
    INFO - ZeroPoint[0] = -17
    INFO - Activation buffer (a.k.a tensor arena) size used: 133716
    INFO - Number of operators: 19
    INFO -  Operator 0: ethos-u
    INFO -  Operator 1: PAD
    INFO -  Operator 2: ethos-u
    INFO -  Operator 3: PAD
    INFO -  Operator 4: ethos-u
    INFO -  Operator 5: PAD
    INFO -  Operator 6: ethos-u
    INFO -  Operator 7: PAD
    INFO -  Operator 8: ethos-u
    INFO -  Operator 9: PAD
    INFO -  Operator 10: ethos-u
    INFO -  Operator 11: PAD
    INFO -  Operator 12: ethos-u
    INFO -  Operator 13: PAD
    INFO -  Operator 14: ethos-u
    INFO -  Operator 15: PAD
    INFO -  Operator 16: ethos-u
    INFO -  Operator 17: AVERAGE_POOL_2D
    INFO -  Operator 18: ethos-u
    ```

5. “List Images” menu option prints a list of pair image indexes - the original filenames embedded in the application:

    ```log
    INFO - List of Files:
    INFO -  0 => man_in_red_jacket.png
    INFO -  1 => st_paul_s_cathedral.png
    ```

### Running Visual Wake Word

Please select the first menu option to execute Visual Wake Word.

The following example illustrates application output for classification:

```log
INFO - Running inference on image 0 => man_in_red_jacket.png
INFO - Final results:
INFO - Total number of inferences: 1
INFO - 0) 1 (3.211100) -> person
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 228679
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 153031
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 40625
INFO - NPU ACTIVE cycles: 706754
INFO - NPU IDLE cycles: 10954
INFO - NPU TOTAL cycles: 717708
```

It could take several minutes to complete one inference run (average time is 2-3 minutes).

The log shows the inference results for “image 1” (1 - index) that corresponds to man_in_red_jacket.png” in the sample
image resource folder.

The profiling section of the log shows that for this inference:

- Ethos-U's PMU report:

  - 717,708 total cycle: The number of NPU cycles

  - 706,754 active cycles: number of NPU cycles that were used for computation

  - 10,954 idle cycles: number of cycles for which the NPU was idle

  - 228,679 AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus. AXI0 is the bus where
    Ethos-U NPU reads and writes to the computation buffers (activation buf/tensor arenas).

  - 153,031 AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - 40,625 AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus. AXI1 is the bus where
    Ethos-U NPU reads the model (read only)

- For FPGA platforms, CPU cycle count can also be enabled. For FVP, however, CPU cycle counters should not be used as
    the CPU model is not cycle-approximate or cycle-accurate.

The application prints the detection with label index, confidence score and labels from associated pd_labels.txt file.
