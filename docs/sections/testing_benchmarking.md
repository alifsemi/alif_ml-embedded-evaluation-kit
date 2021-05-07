# Testing and benchmarking

- [Testing](#testing)
- [Benchmarking](#benchmarking)

## Testing

The `tests` folder has the following structure:

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

Where:

- `common`: contains tests for generic and common appplication functions.
- `use_case`: contains all the use case specific tests in the respective folders.
- `utils`: contains utilities sources used only within the tests.

When [configuring](./building.md#configuring-the-build-native-unit-test) and
[building](./building.md#building-the-configured-project) for `native` target platform results of the build will
be placed under `<build folder>/bin/` folder, for example:

```tree
.
├── arm_ml_embedded_evaluation_kit-<usecase1>-tests
├── arm_ml_embedded_evaluation_kit-<usecase2>-tests
├── ethos-u-<usecase1>
└── ethos-u-<usecase1>
```

To execute unit-tests for a specific use-case in addition to the common tests:

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

Tests output could have `[ERROR]` messages, that's alright - they are coming from negative scenarios tests.

## Benchmarking

Profiling is enabled by default when configuring the project. This will enable displaying:

- the active and idle NPU cycle counts when Arm® Ethos™-U55 is enabled (see `-DETHOS_U55_ENABLED` in
  [Build options](./building.md#build-options).
- CPU cycle counts and/or in milliseconds elapsed for inferences performed if CPU profiling is enabled
  (see `-DCPU_PROFILE_ENABLED` in [Build options](./building.md#build-options). This should be done only
  when running on a physical FPGA board as the FVP does not contain a cycle-approximate or cycle-accurate Cortex-M model.

For example:

- On the FVP:

```log
    Active NPU cycles: 5475412
    Idle NPU cycles:   702
```

- For MPS3 platform, the time duration in milliseconds is also reported when `-DCPU_PROFILE_ENABLED=1` is added to
  CMake configuration command:

```log
    Active NPU cycles: 5629033
    Idle NPU cycles:   1005276
    Active CPU cycles: 993553 (approx)
    Time in ms:        210
```

Next section of the documentation: [Memory Considerations](memory_considerations.md).
