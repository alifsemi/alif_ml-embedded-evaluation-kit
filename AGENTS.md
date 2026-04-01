# AGENTS.md

## Project Overview

The **Arm ML Embedded Evaluation Kit** provides ready-to-use ML applications for the Arm Ethos-U NPU and Cortex-M CPUs. It supports TensorFlow Lite for Microcontrollers and ExecuTorch frameworks, targeting platforms like MPS3/MPS4 FPGA boards and Corstone Fixed Virtual Platforms (FVPs).

## Repository Structure

```
source/
  app/           # Application code: main entry, use cases, profiler
    use_case/    # ML use cases: img_class, kws, asr, kws_asr, ad, vww, noise_reduction, object_detection, inference_runner
    main/        # Main application entry point
  hal/           # Hardware Abstraction Layer (C boundary)
  lib/           # Platform-agnostic libraries (ML API, ports)
tests/
  common/        # Common test utilities
  use_case/      # Per-use-case tests (ad, asr, img_class, kws, etc.)
  fwk/           # Framework-specific tests (tflm)
dependencies/    # Git submodules: tensorflow, executorch, cmsis-*, core-driver, core-platform, cortex-dfp
scripts/
  cmake/         # CMake toolchain files and helper modules
  py/            # Python tooling
    mlek_tools/  # Installable package: resource setup, code generation, model validation
  vela/          # Vela NPU compiler configuration files
resources/       # Input samples and labels for each use case
docs/            # Full documentation: building, deployment, use cases, coding guidelines
```

### Architecture Guardrail

- Keep `source/lib/` fully platform agnostic as designed.
- Do not introduce direct usage or dependencies on `source/hal/` from `source/lib/`.

### Python tooling (`scripts/py/mlek_tools`)

`mlek_tools` is the project's Python support package. It can be installed directly with:

```sh
pip install scripts/py/
```

The `set_up_default_resources.py` script installs it as an editable package into the shared venv at `resources_downloaded/env/`, making the following entry-point scripts available after activation:

| Command | Description |
|---------|-------------|
| `gen-audio` | Generate audio C arrays from WAV files |
| `gen-audio-cpp` | Generate audio C++ source files |
| `gen-rgb-cpp` | Generate RGB image C++ source files |
| `gen-model-cpp` | Generate model weight C++ source files |
| `gen-labels-cpp` | Generate labels C++ source files |
| `gen-test-data-cpp` | Generate test data C++ source files |
| `pte-ops-dump` | Dump operator list from an ExecuTorch `.pte` file |

Optional extras for model validation:

```sh
pip install "scripts/py/[tflite]"     # adds ethos-u-vela
pip install "scripts/py/[executorch]" # adds executorch
```

### `pyproject.toml` files

`scripts/py/pyproject.toml` defines the `mlek-tools` pip package.

## Languages and Standards

- **C++17** (`.cc`, `.cpp` files, `.hpp` headers)
- **C11** (`.c` files, `.h` headers)
- **CMake** (build system, minimum 3.21)
- **Python 3.10 - 3.12** (build helpers, resource setup, code generation)

## Build System

CMake-based. The quickest way to build:

```sh
git submodule update --init --recursive -j $(nproc)
python3 ./set_up_default_resources.py --parallel $(nproc)
python3 ./build_default.py --make-jobs $(nproc)
```

Manual CMake configuration:

```sh
cmake -B build -DTARGET_PLATFORM=mps3 -DTARGET_SUBSYSTEM=sse-300
cmake --build build -j$(nproc)
```

Key CMake options (see [build options](docs/sections/building.md#build-options) for full list):

| Option | Values | Description |
|--------|--------|-------------|
| `TARGET_PLATFORM` | `mps3` (default), `mps4`, `native`, `simple_platform` | Target platform |
| `TARGET_SUBSYSTEM` | `sse-300` (default), `sse-310`, `sse-315`, `sse-320` | Platform subsystem/design |
| `ML_FRAMEWORK` | `TensorFlowLiteMicro` (default), `ExecuTorch` | ML inference framework |
| `ETHOS_U_NPU_ID` | `U55` (default), `U65`, `U85` | Ethos-U NPU variant |
| `ETHOS_U_NPU_ENABLED` | `ON` (default), `OFF` | Enable/disable NPU support |
| `ETHOS_U_NPU_TIMING_ADAPTER_ENABLED` | `ON` (default), `OFF` | Enable/disable timing adapter (simulates memory bandwidth/latency constraints; disable for faster builds) |
| `USE_CASE_BUILD` | e.g. `img_class`, `"img_class;kws"` | Build only specific use cases |
| `USE_SINGLE_INPUT` | `ON`, `OFF` (default) | Use single input sample per use case (faster for testing) |

## Testing

Build native unit tests:

```sh
cmake -B build-native -DTARGET_PLATFORM=native
cmake --build build-native -j$(nproc)
```

Run all tests with CTest from the build directory:

```sh
ctest --test-dir build-native
```

Run a specific test:

```sh
ctest --test-dir build-native -R <test_name>
```

List available tests without running them:

```sh
ctest --test-dir build-native -N
```

Test binaries are output to `<build-dir>/bin/arm_ml_embedded_evaluation_kit-<usecase>-tests`.

For FVP-based integration tests, configure with `-DBUILD_FVP_TESTS=ON -DUSE_SINGLE_INPUT=ON -DFVP_PATH=<path_to_FVP>` and run `ctest --test-dir <build-dir>`.

See [testing and benchmarking](docs/sections/testing_benchmarking.md) for more details.

## Coding Standards

Full guidelines: [coding_guidelines.md](docs/sections/coding_guidelines.md). Key rules:

### C++ Conventions
- **Types/functions**: `PascalCase` (`SomeClass`, `SomeFunction`)
- **Variables/parameters**: `camelCase` (`someVariable`)
- **Macros/enums**: `UPPER_SNAKE_CASE`
- **Namespaces**: `lowercase`
- **Hungarian notation** for member/scope prefixes: `m_` (member), `g_` (global), `s_` (static), `p` (pointer), `k` (constant)

### C Conventions (HAL layer)
- Linux K&R style: `snake_case` for functions and variables
- `UPPER_SNAKE_CASE` for macros and enum values

### Formatting
- 4-space indentation, no tabs
- 100-character column limit (enforced by `.clang-format`)
- Function braces on next line; control statement braces on same line
- Pointers/references attach to the type: `char* ptr`, not `char *ptr`
- No trailing whitespace

### Python Notes
- All functions must include type hints for parameters and return values.
- For functions that return nothing, do not add `-> None`; leave the return annotation blank.
- All functions must have docstrings in reStructuredText style.
- In docstring fields (`:param`, `:returns`, `:raises`), description text must be vertically aligned.
- The alignment column for docstring descriptions must be a multiple of 4 spaces.
- Use the smallest multiple-of-4 alignment that still leaves at least one space after the field label.
- Within a single docstring, all field descriptions (`:param`, `:returns`, `:raises`) must start at the same column.
- Unit tests should not rely on files on disk outside of the project.
- If PyLint reports a file is too long (for example, `too-many-lines` at over 1000 lines), prefer
  moving code into new helper or module files rather than vertically compacting the existing file.

### Static Analysis
- **clang-format**: Configuration in `.clang-format` (LLVM-based style). Run: `clang-format -style=file -i <file>`
- **cppcheck**: Used via pre-push git hook. Setup: `python scripts/py/setup_hooks.py <hooks-dir>`
- **PyLint**: Python code must satisfy PyLint `3.3.8`.
- Avoid PyLint `disable` statements where practical; prefer refactoring to satisfy checks.

## Licensing and Copyright

Apache 2.0. All source files must include the SPDX header:

```
SPDX-FileCopyrightText: Copyright <year> Arm Limited and/or its affiliates <open-source-office@arm.com>
SPDX-License-Identifier: Apache-2.0
```

When updating an existing file, update the copyright year segment to include the
current year using these rules:

- Do not rewrite or compress historical years/ranges; only extend by adding the
  new year to the existing sequence.
- If a file is moved/renamed without content changes (for example, `git` rename
  score `R100`), do not update copyright years.
- If the latest listed year is not the immediately previous year, append the
  current year as a comma-separated year.
  Example: `2021-2023` -> `2021-2023, 2026`
  Example: `2021, 2023` -> `2021, 2023, 2026`
  Example: `2024` -> `2024, 2026`
- If the latest listed year is the immediately previous year, extend that entry
  as a range with a hyphen.
  Example: `2023, 2024-2025` -> `2023, 2024-2026`
  Example: `2025` -> `2025-2026`
  Example: `2021, 2025` -> `2021, 2025-2026`

## Contribution Requirements

- DCO sign-off required on all commits (`git commit -s`)
- Code review via [Arm's GitLab](https://git.gitlab.arm.com/artificial-intelligence/ethos-u/ml-embedded-evaluation-kit)
- All build variants and unit tests must pass before submission
- Run clang-format and cppcheck before pushing

## Key Dependencies (Git Submodules)

All under `dependencies/`:
- `tensorflow` - TensorFlow Lite for Microcontrollers
- `executorch` - PyTorch ExecuTorch
- `core-driver` - Ethos-U NPU core driver
- `core-platform` - Ethos-U NPU core platform
- `cmsis-6`, `cmsis-dsp`, `cmsis-nn`, `cortex-dfp` - ARM CMSIS libraries

Pull with: `git submodule update --init --recursive -j $(nproc)`

## Useful Documentation

- [Building from sources](docs/sections/building.md) - prerequisites, build options, full CMake configuration guide
- [Testing and benchmarking](docs/sections/testing_benchmarking.md) - running unit tests and profiling
- [Coding standards and guidelines](docs/sections/coding_guidelines.md) - naming conventions, formatting, language usage
- [Contributing](docs/sections/contributing.md) - DCO, copyright headers, code review process
- [Customizing](docs/sections/customizing.md) - implementing custom ML applications
- [Deployment](docs/sections/deployment.md) - running on FVP and MPS3/MPS4 hardware
- [Memory considerations](docs/sections/memory_considerations.md) - memory layout and constraints
- [Timing adapters](docs/sections/timing_adapters.md) - configuring memory bandwidth/latency simulation
- [CMake presets](docs/sections/cmake_presets.md) - using CMakePresets.json for build configurations
- [Troubleshooting](docs/sections/troubleshooting.md) - common issues and solutions
- [FAQ](docs/sections/faq.md)
