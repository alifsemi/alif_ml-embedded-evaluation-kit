# Implementing custom ML application

- [Implementing custom ML application](./customizing.md#implementing-custom-ml-application)
  - [Software project description](./customizing.md#software-project-description)
  - [Hardware Abstraction Layer API](./customizing.md#hardware-abstraction-layer-api)
  - [Main loop function](./customizing.md#main-loop-function)
  - [Application context](./customizing.md#application-context)
  - [Profiler](./customizing.md#profiler)
  - [NN Model API](./customizing.md#nn-model-api)
  - [Adding custom ML use-case](./customizing.md#adding-custom-ml-use_case)
  - [Implementing main loop](./customizing.md#implementing-main-loop)
  - [Implementing custom NN model](./customizing.md#implementing-custom-nn-model)
    - [Using GetModelPointer and GetModelLen methods](./customizing.md#using-getmodelpointer-and-getmodellen-methods)
  - [Executing inference](./customizing.md#executing-inference)
  - [Printing to console](./customizing.md#printing-to-console)
  - [Reading user input from console](./customizing.md#reading-user-input-from-console)
  - [Output to MPS3 LCD](./customizing.md#output-to-mps3-lcd)
  - [Building custom use-case](./customizing.md#building-custom-use_case)
  - [Adding custom platform support](./customizing.md#adding-custom-platform-support)

This section describes how to implement a custom Machine Learning application running on Arm® *Corstone™-300* based FVP
or on the Arm® MPS3 FPGA prototyping board.

The Arm® *Ethos™-U* code sample software project offers a way to incorporate more use-case code into the existing
infrastructure. It also provides a build system that automatically picks up added functionality and produces
corresponding executable for each use-case. This is achieved by following certain configuration and code implementation
conventions.

The following sign indicates the important conventions to apply:

> **Convention:** The code is developed using `C++14` and `C99` standards.

## Software project description

See [Repository structure](../documentation.md#repository-structure) section for the outline of the repo.

> **Convention**: Separate use-cases must be organized in sub-folders under the `source/use-case` folder. The name of
> the directory is used as a name for this use-case and can be provided as a `USE_CASE_BUILD` parameter value. The build
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
>
> It is important to note that each use case example has at least one associated API that it uses from
> `source/application/api/use_case`. The API sources are **platform-agnostic** by design so the use cases example
> implementations can re-use one or more of these components, and they can be used on any target. However, it
> is not mandatory to use an API, or to implement one if you are adding a use-case.

## Hardware Abstraction Layer API

The HAL is represented by the following interfaces. To access them, include the `hal.h` header.

- `hal_platform_init` function: Initializes the HAL platform and every module on the platform that the application
  requires to run.

  |  Parameter name | Description                             |
  |-----------------|-----------------------------------------|
  | `return`        | true if successful, false otherwise.    |

- `hal_platform_release` function Releases the HAL platform and any acquired resources.

An example of the API initialization in the main function:

```C++
#include "hal.h"

int main ()

{
  /* Initialise the HAL and platform */
  hal_platform_init();

  ...

  hal_platform_release();
  return 0;
}
```

## Main loop function

Code samples application main function delegates the use-case logic execution to the main loop function that must be
implemented for each custom ML scenario.

The main loop function has external linkage and the main executable for the use-case references the function defined in
the use-case code.

```C++
void main_loop()
{

...

}
```

## Application context

Application context can be used as a holder for a state between main loop iterations. Include `AppContext.hpp` to use
`ApplicationContext` class.

| Method name | Description                                                     |
|-------------|-----------------------------------------------------------------|
| `Set`       | Saves given value as a named attribute in the context.          |
| `Get`       | Gets the saved attribute from the context by the given name.    |
| `Has`       | Checks if an attribute with a given name exists in the context. |

For example:

```C++
#include "hal.h"
#include "AppContext.hpp"

void main_loop()
{
    /* Instantiate application context */
    arm::app::ApplicationContext caseContext;
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
| `StartProfiling`        | Starts profiling and records the starting timing data.         |
| `StopProfiling`         | Stops profiling and records the ending timing data.            |
| `StopProfilingAndReset` | Stops the profiling and internally resets the platform timers. |
| `Reset`                 | Resets the profiler and clears all collected data.             |
| `GetAllResultsAndReset` | Gets all the results as string and resets the profiler.        |
| `PrintProfilingResult`  | Prints collected profiling results and resets the profiler.    |
| `SetName`               | Set the profiler name.                                         |

An example of it in use:

```C++
/* A named profiler instance */
Profiler profiler{"Inference"};

profiler.StartProfiling();
// Code running inference to profile
profiler.StopProfiling();

profiler.PrintProfilingResult();
```

## NN Model API

The Model, which refers to neural network model, is an abstract class wrapping the underlying TensorFlow Lite Micro API.
It provides methods to perform common operations such as TensorFlow Lite Micro framework initialization, inference
execution, accessing input, and output tensor objects.

To use this abstraction, import the `Model.hpp` header.

| Method name               | Description                                                                                                                                                            |
|---------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `GetInputTensor`          | Returns the pointer to the model's input tensor.                                                                                                                       |
| `GetOutputTensor`         | Returns the pointer to the model's output tensor                                                                                                                       |
| `GetType`                 | Returns the model's data type                                                                                                                                          |
| `GetInputShape`           | Return the pointer to the model's input shape                                                                                                                          |
| `GetOutputShape`          | Return the pointer to the model's output shape.                                                                                                                        |
| `GetNumInputs`            | Return the number of input tensors the model has.                                                                                                                      |
| `GetNumOutputs`           | Return the number of output tensors the model has.                                                                                                                     |
| `LogTensorInfo`           | Logs the tensor information to `stdout` for the given tensor pointer. Includes: Tensor name, tensor address, tensor type, tensor memory size, and quantization params. |
| `LogInterpreterInfo`      | Logs the interpreter information to stdout.                                                                                                                            |
| `Init`                    | Initializes the TensorFlow Lite Micro framework, allocates require memory for the model.                                                                               |
| `GetAllocator`            | Gets the allocator pointer for the instance.                                                                                                                           |
| `IsInited`                | Checks if this model object has been initialized.                                                                                                                      |
| `IsDataSigned`            | Checks if the model uses signed data type.                                                                                                                             |
| `RunInference`            | Runs the inference, so invokes the interpreter.                                                                                                                        |
| `ShowModelInfoHandler`    | Model information handler common to all models.                                                                                                                        |
| `GetTensorArena`          | Returns pointer to memory region to be used for tensors allocations.                                                                                                   |
| `ModelPointer`            | Returns the pointer to the NN model data array.                                                                                                                        |
| `ModelSize`               | Returns the model size.                                                                                                                                                |
| `GetOpResolver`           | Returns the reference to the TensorFlow Lite Micro operator resolver.                                                                                                  |
| `EnlistOperations`        | Registers required operators with TensorFlow Lite Micro operator resolver.                                                                                             |
| `GetActivationBufferSize` | Returns the size of the tensor arena memory region.                                                                                                                    |

> **Convention:**  Each ML use-case must have an extension of this class and an implementation of the protected virtual
> methods:
>
> ```C++
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
> API as an example of the model base class extension.

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

Start with creation of a subdirectory under the `source/use_case` directory and two additional directories `src` and
`include` as described in the [Software project description](./customizing.md#software-project-description) section.

## Implementing main loop

The use-case main loop is the place to put use-case main logic. It is an infinite loop that reacts on user input,
triggers use-case conditional logic based on the input and present results back to the user.

However, it could also be a simple logic that runs a single inference and then exits.

Main loop has knowledge about the platform and has access to the platform components through the Hardware Abstraction
Layer (HAL).

Start by creating a `MainLoop.cc` file in the `src` directory (the one created under
[Adding custom ML use-case](./customizing.md#adding-custom-ml-use_case)).  The name used is not important.

Now define the `main_loop` function with the signature described in [Main loop function](./customizing.md#main-loop-function):

```C++
#include "hal.h"
#include "log_macros.h"

void main_loop()
{
    printf("Hello world!");
}
```

The preceding code is already a working use-case. If you compile and run it (see [Building custom use-case](./customizing.md#building-custom-use_case)),
then the application starts and prints a message to console and exits straight away.

You can now start filling this function with logic.

## Implementing custom NN model

Before inference could be run with a custom NN model, TensorFlow Lite Micro framework must learn about the operators, or
layers, included in the model. You must register operators using the `MicroMutableOpResolver` API.

The *Ethos-U* code samples project has an abstraction around TensorFlow Lite Micro API (see [NN model API](./customizing.md#nn-model-api)).
Create `HelloWorldModel.hpp` in the use-case include subdirectory, extend Model abstract class,
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

Create the `HelloWorldModel.cc` file in the `src` subdirectory and define the methods there. Include
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
#include "log_macros.h"

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

### Using GetModelPointer and GetModelLen methods

These functions generated in the C++ file containing the neural network model as an array. This logic for generation of
the C++ array from the `.tflite` file needs to be defined in the `usecase.cmake` file for this `HelloWorld` example.
In the root of the `source/use_case/hello_world`, create a file called `usecase.cmake` and add the following lines to
it:

```cmake
# Generate model file
USER_OPTION(${${use_case}_MODEL_TFLITE_PATH}
            "NN model tflite path"
            "Path-to-your-model.tflite"
            FILEPATH)

generate_tflite_code(
        MODEL_PATH ${${use_case}_MODEL_TFLITE_PATH}
        DESTINATION ${SRC_GEN_DIR}
        EXPRESSIONS ${EXTRA_MODEL_CODE}
        NAMESPACE   "arm" "app" "hello_world")
```

Use the `${use-case}_MODEL_TFLITE_PATH` CMake configuration parameter to include custom model in the generation or
compilation process. Please refer to: [Build options](./building.md#build-options) for further information.

For more details on `usecase.cmake`, refer to: [Building options](./building.md#build-options).

For details on code generation flow in general, refer to: [Automatic file generation](./building.md#automatic-file-generation).

The TensorFlow Lite model data is read during the `Model::Init` method execution. Please refer to
`source/application/api/common/source/Model.cc` for more details.

`Model::Init` will need the pointer to the model. The `arm::app::hello_world::GetModelPointer()` function is generated
during the build and can be found in the file `<build>/generated/hello_world/src/<model_file_name>.cc`. The file
generated is automatically added to the compilation.

At the top of `MainLoop.cc`, add:

```c++
namespace arm {
namespace app {
    namespace hello_world {

        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace hello_world */

    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
} /* namespace app */
} /* namespace arm */
```

These functions can now be used in the `Model.Init` call.

## Executing inference

To run an inference successfully, you must use:

- A TensorFlow Lite model file,
- An extended Model class,
- A place to add the code to invoke inference,
- A main loop function,
- And some input data.

For the `hello_world` example below the input array is not populated. However, for real-world deployment this data must
either be read from an on-board device or be prepared in the form of C++ sources and baked into the application before
compilation.

For example, the image classification application requires extra build steps to generate C++ sources from the provided
images with `generate_images_code` CMake function.

> **Note:** Check that the input data type for your NN model and input array data type are the same. For example,
> generated C++ sources for images store image data as a `uint8` array. For models that were quantized to an `int8` data
> type, convert the image data to `int8` correctly *before* inference execution. Converting asymmetric data to symmetric
> data involves positioning the zero value. In other words, subtracting an offset for `uint8` values. Please check the
> image classification application source for the code example, such as the `ConvertImgToInt8` function.

The following code adds inference invocation to the main loop function:

```c++
#include "hal.h"
#include "log_macros.h"
#include "HelloWorldModel.hpp"

namespace arm {
namespace app {
    namespace hello_world {

        extern uint8_t* GetModelPointer();
        extern size_t GetModelLen();
    } /* namespace hello_world */

    static uint8_t tensorArena[ACTIVATION_BUF_SZ] ACTIVATION_BUF_ATTRIBUTE;
} /* namespace app */
} /* namespace arm */

void main_loop()
{
    printf("Hello world!");

    arm::app::HelloWorldModel model;

    /* Load the model. */
    if (!model.Init(arm::app::tensorArena,
                    sizeof(arm::app::tensorArena),
                    arm::app::hello_world::GetModelPointer(),
                    arm::app::hello_world::GetModelLen())) {
        printf_err("failed to initialise model\n");
        return;
    }

    /* Populate input tensors here */
    // Your-custom-code;

    /* Run inference */
    model.RunInference();

    /* Read or post-process output here */
    const uint32_t tensorSz = outputTensor->bytes;
    const uint8_t * outputData = tflite::GetTensorData<uint8>(outputTensor);
    // Your-custom-code;
}
```

The code snippet has several important blocks:

- Creating HelloWorldModel object and initializing it.

```C++
  arm::app::HelloWorldModel model;

  /* Load the model */
  if (!model.Init(arm::app::tensorArena,
                  sizeof(arm::app::tensorArena),
                  arm::app::hello_world::GetModelPointer(),
                  arm::app::hello_world::GetModelLen())) {
      printf_err("failed to initialise model\n");
      return;
  }
```

- Getting pointers to allocated input and output tensors.

  ```C++
  TfLiteTensor *outputTensor = model.GetOutputTensor();
  TfLiteTensor *inputTensor = model.GetInputTensor();
  ```
- Running inference

  ```C++
  model.RunInference();
  ```

- Reading inference results: Data and data size from the output tensor. We assume that the output layer has a `uint8`
  data type.

  ```C++
  Const uint32_t tensorSz = outputTensor->bytes;

  const uint8_t *outputData = tflite::GetTensorData<uint8>(outputTensor);
  ```

To add profiling for the *Ethos-U*, include a `Profiler.hpp` header and invoke both `StartProfiling` and
`StopProfiling` around inference execution.

For example:

```C++
Profiler profiler{"Inference"};

profiler.StartProfiling();
model.RunInference();
profiler.StopProfiling();

profiler.PrintProfilingResult();
```

## Printing to console

The preceding examples used some function to print messages to the console. To use them, include `log_macros.h` header.

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

The platform package under HAL must provide an implementation for a function `GetLine`. This is then wrapped by HAL to 
expose a function called `hal_get_user_input`. 

```C++
char ch_input[128];
hal_get_user_input(ch_input, sizeof(ch_input));
```

The function intends to block until a line has been provided. For embedded targets, this call might be redirected to get
input from a UART block. For the host targets, this will just be a call to the C standard library instead.

## Output to MPS3 LCD

The HAL exposes LCD functions to print text or an image to the board LCD. For example:

- `hal_lcd_display_text`
- `hal_lcd_display_image`

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
hal_lcd_display_text(hello.c_str(), hello.size(), 10, 35, 0);
```

The image presentation function has the following signature:

- `uint8_t* data`: The image data pointer;
- `const uint32_t width`: The image width;
- `const uint32_t height`: The image height;
- `const uint32_t channels`: The number of channels. Only 1 and 3 channels are supported now.
- `const uint32_t pos_x`: The x coordinate of the first pixel.
- `const uint32_t pos_y`: The y coordinate of the first pixel.
- `const uint32_t downsample_factor`: The factor by which the image is to be downsampled.

For example, the following code snippet visualizes an input tensor data for `MobileNet v2 224`, by down-sampling it
by a factor of two:

```C++
hal_lcd_display_image((uint8_t *) inputTensor->data.data, 224, 224, 3, 10, 35, 2);
```

Please refer to the [Hardware Abstraction Layer API](./customizing.md#hardware-abstraction-layer-api) section for more
LCD related functions.

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
> - `ETHOS_U_NPU_ENABLED` – The flag indicating if the current build supports *Ethos™-U* NPU.
> - `TARGET_PLATFORM` – The target platform being built for.
> - `TARGET_SUBSYSTEM` – If target platform supports multiple subsystems, this is the name of the subsystem.
> - All standard build options.
>   - `CMAKE_CXX_FLAGS` and `CMAKE_C_FLAGS` – The compilation flags.
>   - `CMAKE_EXE_LINKER_FLAGS` – The linker flags.

For the hello world use-case, it is enough to create a `helloworld.cmake` file and set the `DEFAULT_MODEL_PATH`, like:

```cmake
if (ETHOS_U_NPU_ENABLED)
  set(DEFAULT_MODEL_PATH  ${DEFAULT_MODEL_DIR}/helloworldmodel_vela_${DEFAULT_NPU_CONFIG_ID}.tflite)
else()
  set(DEFAULT_MODEL_PATH  ${DEFAULT_MODEL_DIR}/helloworldmodel.tflite)
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

## Adding custom platform support

Platform build configuration script `build_configuration.cmake` is the main build entry point for platform sources.
It is used by top level CMakeLists.txt script to add a platform into the public build stream.
Platform build configuration script must have 2 functions:

- `set_platform_global_defaults` - to set platform source locations and other build options.
- `platform_custom_post_build` - to execute specific post build steps.

The function `set_platform_global_defaults` must set `PLATFORM_DRIVERS_DIR` variable

```cmake
    set(PLATFORM_DRIVERS_DIR "${HAL_PLATFORM_DIR}/mps3" PARENT_SCOPE)
```

location of the platform library sources.

> **Convention:**  The default search path for platform build configuration scripts is in `scripts/cmake/platforms`.
> The directory name for a platform configuration script is also used as a value for `TARGET_PLATFORM`
> build option. For example:
> `scripts/cmake/platforms/my_platform` results in having `my_platform` as a `TARGET_PLATFORM` option for the build.

The function `platform_custom_post_build` could be used to add platform specific post use-case application build steps.

Repository's root level CMakeLists.txt calls common utility function

```cmake
add_platform_build_configuration(TARGET_PLATFORM ${TARGET_PLATFORM})
```
to add given target platform to the build stream. The function finds the script and includes `build_configuration.cmake`
file. After that public build can invoke:

- `set_platform_global_defaults`
- `platform_custom_post_build`

for a specified platform.

New platform sources, that are pointed to by `PLATFORM_DRIVERS_DIR` variable, could be placed anywhere, conventional location
is `source/hal/platform`. Platform must be a separate CMake project with CMakeLists.txt script and build into a static
library `libplatform-drivers.a`.
HAL expects platform to have `platfrom_drivers.h`  header file with required interfaces for included peripherals.

If the new platform uses existing cmsis device project then it should be linked with it like this:

```cmake
    target_link_libraries(${PLATFORM_DRIVERS_TARGET} PUBLIC cmsis_device)
```

Cmsis device exposes an entry point `--entry Reset_Handler` as a link interface.

If the new platform defines custom cmsis device and has custom application entry point,
it must tell linker about it like this:

```cmake
    target_link_options(${PLATFORM_DRIVERS_TARGET} INTERFACE --entry <custom handler name>)
```

Most of the ML use-case applications use UART driven standard output (stdout), and LCD. It is therefore a hard
requirement to implement at least stubs for those. LCD driver must provide implementation for functions declared in
`lcd_img.h` header. The LCD component under HAL sources provides an implementation for MPS3's LCD peripheral as well
as a stub. The application linking to the library produced by LCD has a choice of picking either target. For example,
the MPS3 platform implementation has:

```cmake
target_link_libraries(${PLATFORM_DRIVERS_TARGET} PUBLIC
    <other libs>
    lcd_mps3)
```
The implementation for simple platform on the other hand has:
```cmake
target_link_libraries(${PLATFORM_DRIVERS_TARGET} PUBLIC
    <other libs>
    lcd_stubs)
```

The standard output (stdout) component follows the same convention. It can expose three targets:

- `stdout_retarget_cmsdk`
- `stdout_retarget_pl011`
- `stdout`

The first two targets use the UART (pulling in `CMSDK UART` and `PL011 UART` drivers respectively). The third
implementation relies on standard C or overridden implementation of `fgets` function and can be thought of as stubs
for platforms that do not need to use UART driven stdout (and stderr) streams. It is also possible to run applications
with semi-hosting enabled - `printf` statements will be shown in the host machine console, typically via a debugger.
To facilitate this, the CMake toolchain files expose a function called `configure_semihosting`.  For supported targets
semi-hosting is disabled by default, as mentioned in the `cmsis_device` target CMake file.

```cmake
configure_semihosting(${CMSIS_DEVICE_TARGET} OFF)
```

Other re-usable component is the NPU. It wraps the Arm Ethos-U NPU driver sources with functions that can be called
from the platform initialisation routine. In addition to general utility functions, this component also provides
**cache invalidation overrides** for the `weak` implementations available in the driver. This is useful for custom
targets that use the data cache for the target CPU (typically enabled somewhere in the start-up code for the system).
The driver will call these functions for invalidating data cache (memory regions the NPU has written to).

> **Note**: To do the Arm Ethos-U interrupt set up, the NPU component should be provided with the base address of the
> NPU as well as the IRQ number wired to the CPU. To set this up, the NPU component relies on the system specific
> header files made available by the CMake interface target provided by the `cmsis_device` CMake project. If this
> target is not used by the new custom platform implementation, the NPU component target must be given access to an
> "RTE_Components.h" header in some way. This header should pull in the required definitions for functions like
> `SCB_InvalidateDCache` and `NVIC_SetVector`. The pre-processor definitions like `__DCACHE_PRESENT` are also expected
> to be exposed via the same header.

Examples of the standard output, LCD and NPU components can be found here: `source/hal/source/components`.

Linker scripts for Arm Compiler and GNU embedded toolchain should be added. The location of the files is
on your discretion. The new platform build configuration script must add it in the `platform_custom_post_build`
function like this:

```cmake
    add_linker_script(
            ${PARSED_TARGET_NAME}                  # Target
            ${CMAKE_SCRIPTS_DIR}/platforms/mps3    # linker scripts directory path
            ${LINKER_SCRIPT_NAME})                 # Name of the file without suffix
```

Please see existing platforms sources and build scripts for more details.
