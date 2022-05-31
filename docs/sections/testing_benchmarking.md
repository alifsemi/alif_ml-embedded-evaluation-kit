# Testing and benchmarking

- [Testing and benchmarking](./testing_benchmarking.md#testing-and-benchmarking)
  - [Testing](./testing_benchmarking.md#testing)
  - [Benchmarking](./testing_benchmarking.md#benchmarking)

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
To view all the available tests to run, use the following command in the `<build folder>`:
```commandline
ctest -N
```
Sample output:
```commandline
Test #1: ad-tests
Test #2: asr-tests
Test #3: img_class-tests
Test #4: kws-tests
Test #5: kws_asr-tests
Test #6: noise_reduction-tests
Test #7: object_detection-tests
Test #8: vww-tests

Total Tests: 8
```

To execute a specific unit-test from the above list, in addition to the common tests, run the following command in the `<build folder>`:

```commandline
ctest -R <test_name>
```

To run every test that has been built, run the following command in the `<build folder>`:

```commandline
ctest
```

Sample output:
```log
Start 1: ad-tests
1/8 Test #1: ad-tests .........................   Passed    0.17 sec
    Start 2: asr-tests
2/8 Test #2: asr-tests ........................   Passed    3.04 sec
    Start 3: img_class-tests
3/8 Test #3: img_class-tests ..................   Passed    0.49 sec
    Start 4: kws-tests
4/8 Test #4: kws-tests ........................   Passed    7.52 sec
    Start 5: kws_asr-tests
5/8 Test #5: kws_asr-tests ....................   Passed    2.85 sec
    Start 6: noise_reduction-tests
6/8 Test #6: noise_reduction-tests ............   Passed   16.41 sec
    Start 7: object_detection-tests
7/8 Test #7: object_detection-tests ...........   Passed    0.58 sec
    Start 8: vww-tests
8/8 Test #8: vww-tests ........................   Passed    0.07 sec


Total Test time (real) =  34.71 sec

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
...
INFO - Time ms: 210
```

The next section of the documentation refers to: [Memory Considerations](memory_considerations.md).
