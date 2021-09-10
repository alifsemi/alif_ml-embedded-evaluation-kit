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

- `common`: The tests for generic and common application functions.
- `use_case`: Every use-case specific test in their respective folders.
- `utils`: Utility sources that are only used within the tests.

When [configuring](./building.md#configuring-native-unit_test-build) and
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
...
===============================================================================
   All tests passed (37 assertions in 7 test cases)
```

> **Note:** Test outputs could contain `[ERROR]` messages. This is OK as they are coming from negative scenarios tests.

## Benchmarking

Profiling is enabled by default when configuring the project. Profiling enables you to display:

- NPU event counters when the Arm® *Ethos™-U* NPU is enabled (see `ETHOS_U_NPU_ENABLED` in [Build options](./building.md#build-options) )

  - Total cycle: The number of NPU cycles

  - Active cycles: number of NPU cycles that were used for computation

  - Idle cycles: number of cycles for which the NPU was idle

  - AXI0 read beats: The number of AXI beats with read transactions from AXI0 bus. AXI0 is the bus where
    *Ethos-U* NPU reads and writes to the computation buffers, activation buf, or tensor arenas.

  - AXI0 write beats: The number of AXI beats with write transactions to AXI0 bus.

  - AXI1 read beats: The number of AXI beats with read transactions from AXI1 bus. AXI1 is the bus where
   *Ethos-U* NPU reads the model. So, read-only.

- If CPU profiling is enabled, the CPU cycle counts and the time elapsed, in milliseconds, for inferences performed. For
  further information, please refer to the `CPU_PROFILE_ENABLED` in [Build options](./building.md#build-options).

> **Note:** Only do this when running on a physical FPGA board as the FVP does not contain a cycle-approximate or
> cycle-accurate *Cortex-M* model.

For example:

- On the FVP:

```log
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 628122
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 135087
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 62870
INFO - NPU ACTIVE cycles: 1081007
INFO - NPU IDLE cycles: 626
INFO - NPU TOTAL cycles: 1081634
```

- For the MPS3 platform, the time duration in milliseconds is also reported when `-DCPU_PROFILE_ENABLED=1` is added to
  CMake configuration command, like so:

```log
INFO - Profile for Inference:
INFO - NPU AXI0_RD_DATA_BEAT_RECEIVED beats: 628122
INFO - NPU AXI0_WR_DATA_BEAT_WRITTEN beats: 135087
INFO - NPU AXI1_RD_DATA_BEAT_RECEIVED beats: 62870
INFO - NPU ACTIVE cycles: 1081007
INFO - NPU IDLE cycles: 626
INFO - NPU TOTAL cycles: 1081634
INFO - CPU ACTIVE cycles (approx): 993553
INFO - Time ms: 210
```

The next section of the documentation refers to: [Memory Considerations](memory_considerations.md).
