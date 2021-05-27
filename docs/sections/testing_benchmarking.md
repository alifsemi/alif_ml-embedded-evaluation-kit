# Testing and benchmarking

- [Testing and benchmarking](#testing-and-benchmarking)
  - [Testing](#testing)
  - [Benchmarking](#benchmarking)

## Testing

The `tests` folder uses the following structure:

```tree
.
├── common
│   └── ...
├── use_case
│   ├── <usecase1>
│   │   └── ...
│   ├── <usecase2>
│   │   └── ...
└── utils
    └── ...
```

The folders contain the following information:

- `common`: The tests for generic and common appplication functions.
- `use_case`: Every use-case specific test in their respective folders.
- `utils`: Utility sources that are only used within the tests.

When [configuring](./building.md#configuring-the-build-native-unit-test) and
[building](./building.md#building-the-configured-project) for your `native` target platform, the results of the build is
placed under `<build folder>/bin/` folder. For example:

```tree
.
├── arm_ml_embedded_evaluation_kit-<usecase1>-tests
├── arm_ml_embedded_evaluation_kit-<usecase2>-tests
├── ethos-u-<usecase1>
└── ethos-u-<usecase1>
```

To execute unit-tests for a specific use-case, in addition to the common tests, use:

```commandline
arm_ml_embedded_evaluation_kit-<use_case>-tests
```

```log
INFO - native platform initialised
INFO - ARM Ethos-U55 Evaluation application for MPS3 FPGA Prototyping Board and FastModel

...
===============================================================================
   All tests passed (37 assertions in 7 test cases)
```

> **Note:** Test outputs could contain `[ERROR]` messages. This is OK as they are coming from negative scenarios tests.

## Benchmarking

Profiling is enabled by default when configuring the project. Profiling enables you to display:

- The active and idle NPU cycle counts when the Arm® *Ethos™-U55* is enabled. For more information, refer to the
  `-DETHOS_U55_ENABLED` section in: [Build options](./building.md#build-options).
- If CPU profiling is enabled, the CPU cycle counts and the time elapsed, in milliseconds, for inferences performed. For
  further information, please refer to the `-DCPU_PROFILE_ENABLED` section in:
  [Build options](./building.md#build-options).

> **Note:** Only do this when running on a physical FPGA board as the FVP does not contain a cycle-approximate or
> cycle-accurate *Cortex-M* model.

For example:

- On the FVP:

```log
    Active NPU cycles: 5475412
    Idle NPU cycles:   702
```

- For the MPS3 platform, the time duration in milliseconds is also reported when `-DCPU_PROFILE_ENABLED=1` is added to
  CMake configuration command, like so:

```log
    Active NPU cycles: 5629033
    Idle NPU cycles:   1005276
    Active CPU cycles: 993553 (approx)
    Time in ms:        210
```

The next section of the documentation refers to: [Memory Considerations](memory_considerations.md).
