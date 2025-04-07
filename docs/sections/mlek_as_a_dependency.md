# Using MLEK as a dependency

<!-- TOC -->
* [Using MLEK as a dependency](#using-mlek-as-a-dependency)
  * [Introduction](#introduction)
  * [Guidelines](#guidelines)
    * [Using Use Case APIs](#using-use-case-apis)
    * [Wrapping the complete project](#wrapping-the-complete-project)
<!-- TOC -->

## Introduction

It may be desirable to wrap ML-Embedded-Evaluation-Kit (MLEK) as a source dependency to use the example pipelines.
The use cases have [platform-agnostic API](../../source/application/api/use_case/readme.md) that can be used as
a stand-alone CMake project or be consumed in CMSIS-Pack form. However, there may be scenarios where other parts of
this project are required as dependencies. This guide provides some general recommendations on wrapping MLEK sources.

Other useful readings:
* [Repository Structure](../../docs/documentation.md#repository-structure)
* [Reusable Software](../../Readme.md#reusable-software).

## Guidelines

### Using Use Case APIs

The use case APIs consist of:
- `common` component - required for all use cases. This is available as a
  [CMake project](../../source/application/api/common/CMakeLists.txt) that depends on:
  - `TENSORFLOW_SRC_PATH` variable being defined
  - Following CMake target libraries:
    - [log](../../source/log/readme.md)
    - [arm_math](../../source/math/readme.md)
    - [tensorflow-lite-micro](../../scripts/cmake/tensorflow_lite_micro.cmake): this is a third-party dependency and the
      project wrapping MLEK could provide its own variant.

- `use case specific APIs` are individual CMake projects in
  [source/application/api/use_case](../../source/application/api/use_case) subdirectories. These will only depend on
  the `common` target, which also brings in its dependencies listed above.

### Wrapping the complete project

The top level [CMakeLists.txt](../../CMakeLists.txt) file expects certain common options to have been defined and the
toolchain file to be set. For the default behaviour, the top level project could add this as a CMake subdirectory
without issues. However, if certain options need to be overridden or environment needs to be set up differently, we
recommend to include [MlekModule.cmake](../../MlekModule.cmake) file first and modifying the required cache variables
before adding this project using `add_subdirectory` function.

Let's say there is a new project that wants to add support for a new hardware target. This new project contains the
required `build_configuration.cmake` file along with associated source files providing the `platform_drivers` API to
HAL.

> **NOTE**: Please read [Repository Structure](../../docs/documentation.md#repository-structure) and
> [Adding custom platform support](../../docs/sections/customizing.md#adding-custom-platform-support) for actual
> guidance on adding a new target platform. This section just provides an example from a wrapping perspective.

For a directory structure like:
```tree
NewProject
  ├── LICENSE
  ├── CMakeLists.txt
  ├── platforms-scripts
  │      └── new-target
  │            ├── ...
  │            └── build_configuration.cmake
  ├── source
  │      ├── ...
  │      └── new-target-platform
  ├── third-party
  │      └── MLEK
  │            ├── ...
  │            └── CMakeLists.txt
  └── tests
```

The top-level `CMakeLists.txt` snippet may look something like:
```cmake
# Import the MLEK module CMake file to set up the environment and default options.
include(${CMAKE_CURRENT_SOURCE_DIR}/third-party/MLEK/MlekModule.cmake)

# Append the list of directories where "platform build configurations" are searched for
list(APPEND MLEK_PLATFORM_BUILD_CONFIG_DIRS "platform-scripts/new-target")

# Any additional changes can go here
# ...

# Define the new project
project(NewProject DESCRIPTION "Example")

# NewProject set up if needed
# ...

# Add MLEK subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third-party/MLEK)

# Add tests wrapper, if any
add_subdirectory(tests)
```

Another scenario, for wrapping the complete project, is prototyping or adding new use-cases and re-use platform support
this repository provides. In such cases or generally where downloading additional (or different) resources may be
needed, the top level [set_up_default_resources.py](../../set_up_default_resources.py) provides an optional
`--use-case-resources-file` argument that can be used to point to a new json file that lists the required files.
See more on this script's arguments [here](./building.md#fetching-resource-files).
