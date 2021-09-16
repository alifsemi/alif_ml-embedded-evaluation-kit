# Implementing custom ML application

- [Implementing custom ML application](#implementing-custom-ml-application)
  - [Software project description](#software-project-description)
  - [Hardware Abstraction Layer API](#hardware-abstraction-layer-api)
  - [Main loop function](#main-loop-function)
  - [Application context](#application-context)
  - [Profiler](#profiler)
  - [NN Model API](#nn-model-api)
  - [Adding custom ML use-case](#adding-custom-ml-use_case)
  - [Implementing main loop](#implementing-main-loop)
  - [Implementing custom NN model](#implementing-custom-nn-model)
    - [Define ModelPointer and ModelSize methods](#define-modelpointer-and-modelsize-methods)
  - [Executing inference](#executing-inference)
  - [Printing to console](#printing-to-console)
  - [Reading user input from console](#reading-user-input-from-console)
  - [Output to MPS3 LCD](#output-to-mps3-lcd)
  - [Building custom use-case](#building-custom-use_case)

This section describes how to implement a custom Machine Learning application running on Arm® *Corstone™-300* based FVP
or on the Arm® MPS3 FPGA prototyping board.

the Arm® *Ethos™-U55* code sample software project offers a way to incorporate more use-case code into the existing
infrastructure. It also provides a build system that automatically picks up added functionality and produces
corresponding executable for each use-case. This is achieved by following certain configuration and code implementation
conventions.

The following sign indicates the important conventions to apply:

> **Convention:** The code is developed using `C++11` and `C99` standards. This is then governed by TensorFlow Lite for
> Microcontrollers framework.

## Software project description

As mentioned in the [Repository structure](../documentation.md#repository-structure) section, project sources are:

```tree
├── docs
│ ├── ...
│ └── Documentation.md
├── resources
│ └── img_class
│      └── ...
├── scripts
│ └── ...
├── source
│ ├── application
│ │ ├── hal
│ │ ├── main
│ │ └── tensorflow-lite-micro
│ └── use_case
│     └──img_class
├── CMakeLists.txt
└── Readme.md
```

Where the `source` folder contains C/C++ sources for the platform and ML applications. Common code related to the
*Ethos-U* code samples software framework resides in the `application` sub-folder and ML application-specific logic,
use-cases, sources are in the `use-case` subfolder.

> **Convention**: Separate use-cases must be organized in sub-folders under the use-case folder. The name of the
> directory is used as a name for this use-case and can be provided as a `USE_CASE_BUILD` parameter value. The build
> system expects that sources for the use-case are structured as follows: Headers in an `include` directory and C/C++
> sources in a `src` directory. For example:
>
> ```tree
> use_case
>   └──img_class
>         ├── include
>         │   └── *.hpp
>         └── src
>             └── *.cc
> ```

## Hardware Abstraction Layer API

The HAL is represented by the following interfaces. To access them, include the `hal.h` header.

- `hal_platform` structure: Defines a platform context to be used by the application.

  |  Attribute name    | Description |
  |--------------------|----------------------------------------------------------------------------------------------|
  |  `inited`            |  Initialization flag. Is set after the `platform_init()` function is called.                   |
  |  `plat_name`         |  Platform name. it is set to `mps3-bare` for MPS3 build and `FVP` for Fast Model build.      |
  |  `data_acq`          |  Pointer to data acquisition module responsible for user interaction and other data collection for the application logic.               |
  |  `data_psn`          |  Pointer to data presentation module responsible for data output through components available in the selected platform: `LCD --` for MPS3, `console --` for Fast Model. |
  |  `timer`             |  Pointer to platform timer implementation (see `platform_timer`)                               |
  |  `platform_init`     |  Pointer to platform initialization function.                                                |
  |  `platform_release`  |  Pointer to platform release function                                                        |

- `hal_init` function: Initializes the HAL structure based on the compile time configuration. This must be called before
    any other function in this API.

  |  Parameter name  | Description|
  |------------------|-----------------------------------------------------|
  |  `platform`        | Pointer to a pre-allocated `hal_platform` struct.   |
  |  `data_acq`        | Pointer to a pre-allocated data acquisition module  |
  |  `data_psn`        | Pointer to a pre-allocated data presentation module |
  |  `timer`           | Pointer to a pre-allocated timer module             |
  |  `return`          | Zero returned if successful, an error code is returned if unsuccessful.            |

- `hal_platform_init` function: Initializes the HAL platform and every module on the platform that the application
  requires to run.

  | Parameter name  | Description                                                         |
  | ----------------| ------------------------------------------------------------------- |
  | `platform`        | Pointer to a pre-allocated and initialized `hal_platform` struct.   |
  | `return`          | zero if successful, error code otherwise.                           |

- `hal_platform_release` function Releases the HAL platform and any acquired resources.

  | Parameter name  | Description                                                         |
  | ----------------| ------------------------------------------------------------------- |
  |  `platform`       | Pointer to a pre-allocated and initialized `hal_platform` struct.   |

- `data_acq_module` structure: Structure to encompass the data acquisition module and linked methods.

  | Attribute name | Description                                        |
  |----------------|----------------------------------------------------|
  | `inited`        | Initialization flag. Is set after the `system_init ()` function is called. |
  | `system_name`    | Channel name. It is set to `UART` for MPS3 build and Fast Model builds.   |
  | `system_init`    | Pointer to data acquisition module initialization function. The pointer is set according to the platform selected during the build. This function is called by the platform initialization routines. |
  | `get_input`      | Pointer to a function reading user input. The pointer is set according to the selected platform during the build. For MPS3 and Fast Model environments, the function reads data from UART.   |

- `data_psn_module` structure: Structure to encompass the data presentation module and associated methods.

  | Attribute name     | Description                                    |
  |--------------------|------------------------------------------------|
  | `inited`             | Initialization flag. It is set after the `system_init ()` function is called. |
  | `system_name`        | System component name used to present data. It is set to `lcd` for the MPS3 build and to `log_psn` for the Fast Model build. For Fast Model, the console output of the data summary replaces all pixel drawing functions.  |
  | `system_init`        | Pointer to data presentation module initialization function. The pointer is set according to the platform selected during the build. This function is called by the platform initialization routines. |
  | `present_data_image` | Pointer to a function to draw an image. The pointer is set according to the selected platform during the build. For MPS3, the image is drawn on the LCD. For Fast Model, the image summary is printed in the UART (coordinates, channel info, downsample factor). |
  | `present_data_text`  | Pointer to a function to print a text. The pointer is set according to the selected platform during the build. For MPS3, the text is drawn on the LCD. For Fast Model, the text is printed in the UART. |
  | `present_box`        | Pointer to a function to draw a rectangle. The pointer is set according to the selected platform during the build. For MPS3, the image is drawn on the LCD. For Fast Model, the image summary is printed in the UART. |
  | `clear`              | Pointer to a function to clear the output. The pointer is set according to the selected platform during the build. For MPS3, the function clears the LCD. For Fast Model, nothing happens. |
  | `set_text_color`     | Pointer to a function to set text color for the next call of `present_data_text()` function. The pointer is set according to the selected platform during the build. For MPS3, the function sets the color for the text printed on the LCD. For Fast Model, nothing happens. |

- `platform_timer` structure: The structure to hold a platform-specific timer implementation.

  | Attribute name      | Description                                    |
  |---------------------|------------------------------------------------|
  |  `inited`             |  Initialization flag. It is set after the timer is initialized by the `hal_platform_init` function. |
  |  `reset`              |  Pointer to a function to reset a timer. |
  |  `get_time_counter`   |  Pointer to a function to get current time counter. |
  |  `get_duration_ms`    |  Pointer to a function to calculate duration between two time-counters in milliseconds. |
  |  `get_duration_us`    |  Pointer to a function to calculate duration between two time-counters in microseconds |
  |  `get_cpu_cycle_diff` |  Pointer to a function to calculate duration between two time-counters in *Cortex-M55* cycles. |
  |  `get_npu_cycle_diff` |  Pointer to a function to calculate duration between two time-counters in *Ethos-U* cycles. Available only when project is configured with `ETHOS_U_NPU_ENABLED` set. |
  |  `start_profiling`    |  If necessary, wraps the `get_time_counter` function with another profiling initialization, if necessary. |
  |  `stop_profiling`     |  If necessary, wraps the `get_time_counter` function along with more instructions when profiling ends. |

An example of the API initialization in the main function:

```C++
#include "hal.h"

int main ()

{

  hal_platform platform;
  data_acq_module dataAcq;
  data_psn_module dataPsn;
  platform_timer timer;

  /* Initialise the HAL and platform */
  hal_init(&platform, &dataAcq, &dataPsn, &timer);
  hal_platform_init(&platform);

  ...

  hal_platform_release(&platform);

  return 0;

}
```

## Main loop function

Code samples application main function delegates the use-case logic execution to the main loop function that must be
implemented for each custom ML scenario.

Main loop function takes the initialized `hal_platform` structure pointer as an argument.

The main loop function has external linkage and the main executable for the use-case references the function defined in
the use-case code.

```C++
void main_loop(hal_platform& platform){

...

}
```

## Application context

Application context can be used as a holder for a state between main loop iterations. Include `AppContext.hpp` to use
`ApplicationContext` class.

| Method name  | Description                                                      |
|--------------|------------------------------------------------------------------|
| `Set`         |  Saves given value as a named attribute in the context.          |
|  `Get`         |  Gets the saved attribute from the context by the given name.    |
|  `Has`         |  Checks if an attribute with a given name exists in the context. |

For example:

```C++
#include "hal.h"
#include "AppContext.hpp"

void main_loop(hal_platform& platform) {

    /* Instantiate application context */
    arm::app::ApplicationContext caseContext;
    caseContext.Set<hal_platform&>("platform", platform);
    caseContext.Set<uint32_t>("counter", 0);

    /* loop */
  while (true) {
    // do something, pass application context down the call stack
  }
}
```

## Profiler

The profiler is a helper class that assists with the collection of timings and *Ethos-U* cycle counts for operations.
It uses platform timer to get system timing information.

| Method name             | Description                                                    |
|-------------------------|----------------------------------------------------------------|
|  `StartProfiling`         | Starts profiling and records the starting timing data.         |
|  `StopProfiling`          | Stops profiling and records the ending timing data.            |
|  `StopProfilingAndReset`  | Stops the profiling and internally resets the platform timers. |
|  `Reset`                  | Resets the profiler and clears all collected data.             |
|  `GetAllResultsAndReset`  | Gets all the results as string and resets the profiler.            |
|  `PrintProfilingResult`   | Prints collected profiling results and resets the profiler.    |
|  `SetName`                | Set the profiler name.                                         |

An example of it in use:

```C++
Profiler profiler{&platform, "Inference"};

profiler.StartProfiling();
// Code running inference to profile
profiler.StopProfiling();

profiler.PrintProfilingResult();
```

## NN Model API

The Model, which refers to neural network model, is an abstract class wrapping the underlying TensorFlow Lite Micro API.
It provides methods to perform common operations such as TensorFlow Lite Micro framework initialization, inference
execution, accessing input, and output tensor objects.

To use this abstraction, import the `TensorFlowLiteMicro.hpp` header.

| Method name              | Description                                                                  |
|--------------------------|------------------------------------------------------------------------------|
|  `GetInputTensor`          |  Returns the pointer to the model's input tensor.                            |
|  `GetOutputTensor`         |  Returns the pointer to the model's output tensor                            |
|  `GetType`                 |  Returns the model's data type                                               |
|  `GetInputShape`           |  Return the pointer to the model's input shape                               |
|  `GetOutputShape`          |  Return the pointer to the model's output shape.                             |
|  `GetNumInputs`            |  Return the number of input tensors the model has.                           |
|  `GetNumOutputs`           |  Return the number of output tensors the model has.                          |
|  `LogTensorInfo`           |  Logs the tensor information to `stdout` for the given tensor pointer. Includes: Tensor name, tensor address, tensor type, tensor memory size, and quantization params.  |
|  `LogInterpreterInfo`      |  Logs the interpreter information to stdout.                                 |
|  `Init`                    |  Initializes the TensorFlow Lite Micro framework, allocates require memory for the model. |
|  `GetAllocator`            |  Gets the allocator pointer for the instance.                                |
|  `IsInited`                |  Checks if this model object has been initialized.                           |
|  `IsDataSigned`            |  Checks if the model uses signed data type.                                  |
|  `RunInference`            |  Runs the inference, so invokes the interpreter.                               |
|  `ShowModelInfoHandler`    |  Model information handler common to all models.                             |
|  `GetTensorArena`          |  Returns pointer to memory region to be used for tensors allocations.        |
|  `ModelPointer`            |  Returns the pointer to the NN model data array.                             |
|  `ModelSize`               |  Returns the model size.                                                     |
|  `GetOpResolver`           |  Returns the reference to the TensorFlow Lite Micro operator resolver.       |
|  `EnlistOperations`        |  Registers required operators with TensorFlow Lite Micro operator resolver.  |
|  `GetActivationBufferSize` |  Returns the size of the tensor arena memory region.                         |

> **Convention:**  Each ML use-case must have an extension of this class and an implementation of the protected virtual
> methods:
>
> ```C++
> virtual const uint8_t* ModelPointer() = 0;
> virtual size_t ModelSize() = 0;
> virtual const tflite::MicroOpResolver& GetOpResolver() = 0;
> virtual bool EnlistOperations() = 0;
> virtual size_t GetActivationBufferSize() = 0;
> ```
>
> Network models have different set of operators that must be registered with `tflite::MicroMutableOpResolver` object in
> the `EnlistOperations` method. Network models can require different size of activation buffer that is returned as
> tensor arena memory for TensorFlow Lite Micro framework by the `GetTensorArena` and `GetActivationBufferSize` methods.
>
> **Note:** Please see `MobileNetModel.hpp` and `MobileNetModel.cc` files from the image classification ML application
> use-case as an example of the model base class extension.

## Adding custom ML use-case

This section describes how to implement additional use-case and then compile it into the binary executable to run with
Fast Model or MPS3 FPGA board.

It covers common major steps: The application main loop creation, a description of the NN model, and inference
execution.

In addition, few useful examples are provided: Reading user input, printing into console, and drawing images into MPS3
LCD.

For example:

```tree
use_case
   └──hello_world
      ├── include
      └── src
```

Start with creation of a sub-directory under the `use_case` directory and two additional directories `src` and `include`
as described in the [Software project description](#software-project-description) section.

## Implementing main loop

The use-case main loop is the place to put use-case main logic. It is an infinite loop that reacts on user input,
triggers use-case conditional logic based on the input and present results back to the user.

However, it could also be a simple logic that runs a single inference and then exits.

Main loop has knowledge about the platform and has access to the platform components through the Hardware Abstraction
Layer (HAL).

Start by creating a `MainLoop.cc` file in the `src` directory (the one created under
[Adding custom ML use case](#adding-custom-ml-use-case)).  The name used is not important.

Now define the `main_loop` function with the signature described in [Main loop function](#main-loop-function):

```C++
#include "hal.h"

void main_loop(hal_platform& platform) {
  printf("Hello world!");
}
```

The preceeding code is already a working use-case. If you compile and run it (see [Building custom usecase](#building-custom-use-case)),
then the application starts and prints a message to console and exits straight away.

You can now start filling this function with logic.

## Implementing custom NN model

Before inference could be run with a custom NN model, TensorFlow Lite Micro framework must learn about the operators, or
layers, included in the model. You must register operators using the `MicroMutableOpResolver` API.

The *Ethos-U* code samples project has an abstraction around TensorFlow Lite Micro API (see [NN model API](#nn-model-api)).
Create `HelloWorldModel.hpp` in the use-case include sub-directory, extend Model abstract class,
and then declare the required methods.

For example:

```C++
#ifndef HELLOWORLDMODEL_HPP
#define HELLOWORLDMODEL_HPP

#include "Model.hpp"

namespace arm {
namespace app {

class HelloWorldModel: public Model {
  protected:
    /** @brief   Gets the reference to op resolver interface class. */
    const tflite::MicroOpResolver& GetOpResolver() override;

    /** @brief   Adds operations to the op resolver instance. */
    bool EnlistOperations() override;

    const uint8_t* ModelPointer() override;

    size_t ModelSize() override;

  private:
    /* Maximum number of individual operations that can be enlisted. */
    static constexpr int ms_maxOpCnt = 5;

    /* A mutable op resolver instance. */
    tflite::MicroMutableOpResolver<ms_maxOpCnt> m_opResolver;
  };
} /* namespace app */
} /* namespace arm */

#endif /* HELLOWORLDMODEL_HPP */
```

Create the `HelloWorldModel.cc` file in the `src` sub-directory and define the methods there. Include
`HelloWorldModel.hpp` created earlier.

> **Note:** The `Model.hpp` included in the header provides access to TensorFlow Lite Micro's operation resolver API.

Please refer to `use_case/img_class/src/MobileNetModel.cc` for code examples.

If you are using a TensorFlow Lite model compiled with Vela, it is important to add a custom *Ethos-U* operator to the
operators list.

The following example shows how to add the custom *Ethos-U* operator with the TensorFlow Lite Micro framework. when
defined, `ARM_NPU` excludes the code if the application was built without NPU support.

For example:

```C++
#include "HelloWorldModel.hpp"

bool arm::app::HelloWorldModel::EnlistOperations() {

#if defined(ARM_NPU)
    if (kTfLiteOk == this->m_opResolver.AddEthosU()) {
        info("Added %s support to op resolver\n",
            tflite::GetString_ETHOSU());
    } else {
        printf_err("Failed to add Arm NPU support to op resolver.");
        return false;
    }
#endif /* ARM_NPU */

    return true;
}
```

To minimize the memory footprint of the application, we advise you to only register operators that are used by the NN
model.

### Define ModelPointer and ModelSize methods

These functions are wrappers around the functions generated in the C++ file containing the neural network model as an
array. This generation the C++ array from the `.tflite` file, logic needs to be defined in the `usecase.cmake` file for
this `HelloWorld` example.

For more details on `usecase.cmake`, refer to: [Building options](./building.md#build-options).

For details on code generation flow in general, refer to: [Automatic file generation](./building.md#automatic-file-generation).

The TensorFlow Lite model data is read during the `Model::Init()` method execution. Please refer to
`application/tensorflow-lite-micro/Model.cc` for more details.

Model invokes the `ModelPointer()` function which calls the `GetModelPointer()` function to get the neural network model
data memory address. The `GetModelPointer()` function is generated during the build and can be found in the file
`build/generated/hello_world/src/<model_file_name>.cc`. The file generated is automatically added to the compilation.

Use the `${use-case}_MODEL_TFLITE_PATH` build parameter to include custom model to the generation, or compilation,
process. Please refer to: [Build options](./building.md#build-options) for further information.

## Executing inference

To run an inference successfully, you must use:

- A TensorFlow Lite model file,
- An extended Model class,
- A place to add the code to invoke inference,
- A main loop function,
- And some input data.

For the `hello_world` example below, the input array is not populated. However, for real-world scenarios, and before
compilation and be baked into the application, this data must either be read from an on-board device, or be prepared in
the form of C++ sources.

For example, the image classification application requires extra build steps to generate C++ sources from the provided
images with `generate_images_code` CMake function.

> **Note:** Check that the input data type for your NN model and input array data type are the same. For example,
> generated C++ sources for images store image data as a `uint8` array. For models that were quantized to an `int8` data
> type, convert the image data to `int8` correctly *before* inference execution. Converting asymmetric data to symmetric
> data involves positioning the zero value. In other words, subtracting an offset for `uint8` values. Please check the
> image classification application source for the code example, such as the `ConvertImgToInt8` function.

The following code adds inference invocation to the main loop function:

```C++
#include "hal.h"
#include "HelloWorldModel.hpp"

  void main_loop(hal_platform& platform) {

  /* model wrapper object */
  arm::app::HelloWorldModel model;

  /* Load the model */
  if (!model.Init()) {
    printf_err("failed to initialise model\n");
    return;
  }

  TfLiteTensor *outputTensor = model.GetOutputTensor();
  TfLiteTensor *inputTensor = model.GetInputTensor();

  /* dummy input data*/
  uint8_t inputData[1000];

  memcpy(inputTensor->data.data, inputData, 1000);

  /* run inference */
  model.RunInference();

  const uint32_t tensorSz = outputTensor->bytes;
  const uint8_t * outputData = tflite::GetTensorData<uint8>(outputTensor);
}
```

The code snippet has several important blocks:

- Creating HelloWorldModel object and initializing it.

  ```C++
  arm::app::HelloWorldModel model;

  /* Load the model */
  if (!model.Init()) {
    printf_err(\"failed to initialise model\\n\");
    return;
  }
  ```

- Getting pointers to allocated input and output tensors.

  ```C++
  TfLiteTensor *outputTensor = model.GetOutputTensor();
  TfLiteTensor *inputTensor = model.GetInputTensor();
  ```

- Copying input data to the input tensor. We assume input tensor size to be 1000 `uint8` elements.

  ```C++
  memcpy(inputTensor->data.data, inputData, 1000);
  ```

- Running inference

  ```C++
  model.RunInference();
  ```

- Reading inference results: Data and data size from the output tensor. We assume that the output layer has a `uint8`
  data type.

  ```C++
  Const uint32_t tensorSz = outputTensor->bytes ;

  const uint8_t *outputData = tflite::GetTensorData<uint8>(outputTensor);
  ```

To add profiling for the *Ethos-U*, include a `Profiler.hpp` header and invoke both `StartProfiling` and
`StopProfiling` around inference execution.

For example:

```C++
Profiler profiler{&platform, "Inference"};

profiler.StartProfiling();
model.RunInference();
profiler.StopProfiling();

profiler.PrintProfilingResult();
```

## Printing to console

The preceding examples used some function to print messages to the console.

However, for clarity, here is the full list of available functions:

- `printf`
- `trace` - printf wrapper for tracing messages.
- `debug` - printf wrapper for debug messages.
- `info` - printf wrapper for informational messages.
- `warn` - printf wrapper for warning messages.
- `printf_err` - printf wrapper for error messages.

`printf` wrappers can be switched off with `LOG_LEVEL` define:

`trace (0) < debug (1) < info (2) < warn (3) < error (4)`.

> **Note:** The default output level is `info = level 2`.

## Reading user input from console

The platform data acquisition module uses the `get_input` function to read the keyboard input from the UART. It can be
used as follows:

```C++
char ch_input[128];
platform.data_acq->get_input(ch_input, sizeof(ch_input));
```

The function is blocked until a user provides an input.

## Output to MPS3 LCD

The platform presentation module has functions to print text or an image to the board LCD. For example:

- `present_data_text`
- `present_data_image`

Text presentation function has the following signature:

- `const char* str`: the string to print.
- `const uint32_t str_sz`: The string size.
- `const uint32_t pos_x`: The x coordinate of the first letter in pixels.
- `const uint32_t pos_y`: The y coordinate of the first letter in pixels.
- `const uint32_t alow_multiple_lines`: Signals whether the text is allowed to span multiple lines on the screen, or
  must be truncated to the current line.

This function does not wrap text. If the given string cannot fit on the screen, it goes outside the screen boundary.

Here is an example that prints "Hello world" on the LCD screen:

```C++
std::string hello("Hello world");
platform.data_psn->present_data_text(hello.c_str(), hello.size(), 10, 35, 0);
```

The image presentation function has the following signature:

- `uint8_t* data`: The image data pointer;
- `const uint32_t width`: The image width;
- `const uint32_t height`: The image height;
- `const uint32_t channels`: The number of channels. Only 1 and 3 channels are supported now.
- `const uint32_t pos_x`: The x coordinate of the first pixel.
- `const uint32_t pos_y`: The y coordinate of the first pixel.
- `const uint32_t downsample_factor`: The factor by which the image is to be downsampled.

For example, the following code snippet visualizes an input tensor data for `MobileNet v2 224`, by downsampling it
twice:

```C++
platform.data_psn->present_data_image((uint8_t *) inputTensor->data.data, 224, 224, 3, 10, 35, 2);
```

Please refer to the [Hardware Abstraction Layer API](./customizing.md#hardware-abstraction-layer-api) section for more data presentation functions.

## Building custom use-case

There is one last thing to do before building and running a use-case application. You must create a `usecase.cmake` file
in the root of your use-case. However, the name of the file is not important.

> **Convention:**  The build system searches for CMake file in each use-case directory and includes it into the build
> flow. This file can be used to specify additional application-specific build options, add custom build steps, or
> override standard compilation and linking flags. Use the `USER_OPTION` function to add further build options. Prefix
> the variable name with `${use_case}`, the use-case name, to avoid names collisions with other CMake variables. Here
> are some useful variable names visible in use-case CMake file:
>
> - `DEFAULT_MODEL_PATH` – The default model path to use if use-case specific `${use_case}_MODEL_TFLITE_PATH` is not set
>  in the build arguments.
>- `TARGET_NAME` – The name of the executable.
> - `use_case` – The name of the current use-case.
> - `UC_SRC` – A list of use-case sources.
> - `UC_INCLUDE` – The path to the use-case headers.
> - `ETHOS_U_NPU_ENABLED` – The flag indicating if the current build supports Ethos-U55.
> - `TARGET_PLATFORM` – The target platform being built for.
> - `TARGET_SUBSYSTEM` – If target platform supports multiple subsystems, this is the name of the subsystem.
> - All standard build options.
>   - `CMAKE_CXX_FLAGS` and `CMAKE_C_FLAGS` – The compilation flags.
>   - `CMAKE_EXE_LINKER_FLAGS` – The linker flags.

For the hello world use-case, it is enough to create a `helloworld.cmake` file and set the `DEFAULT_MODEL_PATH`, like
so:

```cmake
if (ETHOS_U_NPU_ENABLED)
  set(DEFAULT_MODEL_PATH  ${DEFAULT_MODEL_DIR}/helloworldmodel_uint8_vela_${DEFAULT_NPU_CONFIG_ID}.tflite)
else()
  set(DEFAULT_MODEL_PATH  ${DEFAULT_MODEL_DIR}/helloworldmodel_uint8.tflite)
endif()
```

This can be used in subsequent section, for example:

```cmake
USER_OPTION(${use_case}_MODEL_TFLITE_PATH "Neural network model in tflite format."
    ${DEFAULT_MODEL_PATH}
    FILEPATH
    )

generate_tflite_code(
    MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH}
    DESTINATION ${SRC_GEN_DIR}
    )
```

This ensures that the model path pointed to by `${use_case}_MODEL_TFLITE_PATH` is converted to a C++ array and is picked
up by the build system. More information on auto-generations is available under section:
[Automatic file generation](./building.md#automatic-file-generation).

To build you application, follow the general instructions from [Add Custom inputs](./building.md#add-custom-inputs) and
then specify the name of the use-case in the build command, like so:

```commandline
cmake .. \
  -DTARGET_PLATFORM=mps3 \
  -DTARGET_SUBSYSTEM=sse-300 \
  -DUSE_CASE_BUILD=hello_world \
  -DCMAKE_TOOLCHAIN_FILE=scripts/cmake/toolchains/bare-metal-armclang.cmake
```

As a result, the file `ethos-u-hello_world.axf` is created. The MPS3 build also produces the `sectors/hello_world`
directory with binaries and the file `sectors/images.txt` to be copied to the MicroSD card on the board.

The next section of the documentation covers: [Testing and benchmarking](testing_benchmarking.md).
