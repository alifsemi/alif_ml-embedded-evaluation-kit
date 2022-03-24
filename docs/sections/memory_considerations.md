# Memory considerations

- [Memory considerations](./memory_considerations.md#memory-considerations)
  - [Introduction](./memory_considerations.md#introduction)
  - [Memory available on the target platform](./memory_considerations.md#memory-available-on-the-target-platform)
    - [Parameters linked to SRAM size definitions](./memory_considerations.md#parameters-linked-to-sram-size-definitions)
  - [Understanding memory usage from Vela output](./memory_considerations.md#understanding-memory-usage-from-vela-output)
    - [Total SRAM used](./memory_considerations.md#total-sram-used)
    - [Total Off-chip Flash used](./memory_considerations.md#total-off_chip-flash-used)
  - [Memory mode configurations](./memory_considerations.md#memory-mode-configurations)
  - [Tensor arena and neural network model memory placement](./memory_considerations.md#tensor-arena-and-neural-network-model-memory-placement)
  - [Memory usage for ML use-cases](./memory_considerations.md#memory-usage-for-ml-use_cases)
  - [Memory constraints](./memory_considerations.md#memory-constraints)

## Introduction

This section provides useful details on how the Machine Learning use-cases of the evaluation kit use the system memory.

Although the guidance provided here is concerning the Arm® *Corstone™-300* system, it is fairly generic and is
applicable for other platforms too. The Arm® *Corstone™-300* is composed of both the Arm® *Cortex™-M55* and the Arm®
*Ethos™-U* NPU. The memory map for the Arm® *Cortex™-M55* core can be found in the [Appendix](./appendix.md#appendix).

The Arm® *Ethos™-U* NPU interacts with the system through two AXI interfaces. The first one, is envisaged to be the
higher-bandwidth, lower-latency, interface. In a typical system, this is wired to an SRAM as it is required to service
frequent read and write traffic.

The second interface is expected to have a higher-latency, lower-bandwidth characteristic, and is typically wired to a
flash device servicing read-only traffic. In this configuration, the Arm® *Cortex™-M55* CPU and Arm® *Ethos™-U* NPU
read the contents of the neural network model, or the `.tflite` file, from the flash memory region. With the Arm®
*Ethos™-U* NPU requesting these read transactions over its second AXI bus.

The input and output tensors, along with any intermediate computation buffers, are placed on SRAM. Therefore, both the
Arm® *Cortex™-M55* CPU and Arm® *Ethos™-U* NPU would be reading, or writing, to this region when running an inference.
The Arm® *Ethos™-U* NPU requests these Read and Write transactions over the first AXI bus.

## Memory available on the target platform

All embedded target platforms supported have platform specific header files that contain memory and other definitions.
For example, MPS3 target header files can be found under the HAL sources' platform subdirectory and contains the
following definitions:

```c
#define ISRAM0_SIZE           (0x00100000)     /* ISRAM0 size */
#define ISRAM1_SIZE           (0x00100000)     /* ISRAM1 size */
```

As these SRAM banks are being used for holding the tensor arena, the total size should be limited to **2 MiB** for
Arm® *Corstone™-300* target platform. This size is directly linked to the size mentioned in the linker scripts. See
snippet from the scatter file for Arm® *Corstone™-300* below:

```log
;-----------------------------------------------------
; FPGA internal SRAM of 2MiB - reserved for activation
; buffers.
; This region should have 3 cycle read latency from
; both Cortex-M55 and Ethos-U NPU
;-----------------------------------------------------
isram.bin       0x31000000  UNINIT ALIGN 16 0x00200000
{
    ...
}
```

When not using the Arm Ethos-U NPU at all or using only Arm Ethos-U55 in `Shared_Sram` or `Sram_Only` modes, the limit
in the linker script (and the platform memory definition header file) is applicable.

However, for `Dedicated_Sram` memory mode (applies only for Arm Ethos-U65), the tensor arena will not sit in this
SRAM space and therefore, the use case can be allowed to have larger `${use_case}_ACTIVATION_BUF_SZ`. See next section
for more details on this.

For custom requirements (like always placing the tensor arena in DDR), the user can change the `BufAttributes.hpp` file
along with the linker scripts.

### Parameters linked to SRAM size definitions

Other than the obvious link between the linker script and the target profile description in
CMake files, there are other parameters linked to what the reserved space for activation
buffers is. These are:

- The file [set_up_default_resources.py](../../set_up_default_resources.py) contains a
  parameter called `mps3_max_sram_sz`:

  ```python
  # The internal SRAM size for Corstone-300 implementation on MPS3 specified by AN552
  mps3_max_sram_sz = 2 * 1024 * 1024 # 2 MiB (2 banks of 1 MiB each)
  ```

  This size of **2 MiB** here is provided here to allow the default vela optimisation process to
  use this size as a hint for the available SRAM size for use by the CPU and the NPU.

- In every `usecase.cmake` file (present within each use case's source directory), there is
  a parameter called `${use_case}_ACTIVATION_BUF_SZ` set to a fixed value by default. This
  default value should be less than, or equal to, the available SRAM on the target platform if
  the activation buffer needs to be reserved in the target platform's SRAM region.

## Understanding memory usage from Vela output

### Total SRAM used

When the neural network model is compiled with Vela, a summary report that includes memory usage is generated.
For example, compiling the keyword spotting model
[ds_cnn_clustered_int8](https://github.com/ARM-software/ML-zoo/blob/master/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/ds_cnn_clustered_int8.tflite)
with the Vela command:

```commandline
vela \
  --accelerator-config=ethos-u55-128 \
  --optimise Performance \
  --config scripts/vela/default_vela.ini
  --memory-mode=Shared_Sram
  --system-config=Ethos_U55_High_End_Embedded
  ds_cnn_clustered_int8.tflite
```

It produces, among others, the following output:

```log
Total SRAM used                                146.31 KiB
Total Off-chip Flash used                      452.42 KiB
```

The `Total SRAM used` here shows the required memory to store the `tensor arena` for the TensorFlow Lite Micro
framework. This is the amount of memory required to store the input, output, and intermediate buffers. In the preceding
example, the tensor arena requires 146.31 KiB of available SRAM.

> **Note:** Vela can only estimate the SRAM required for graph execution. It has no way of estimating the memory used by
> internal structures from TensorFlow Lite Micro framework.

Therefore, we recommend that you top this memory size by at least 2KiB. We also recommend that you also carve out the
`tensor arena` of this size, and then place it on the SRAM of the target system.

### Total Off-chip Flash used

The `Total Off-chip Flash` parameter indicates the minimum amount of flash required to store the neural network model.
In the preceding example, the system must have a minimum of 452.42 KiB of available flash memory to store the `.tflite`
file contents.

> **Note:** The Arm® *Corstone™-300* system uses the DDR region as a flash memory. The timing adapter sets up the AXI
> bus that is wired to the DDR to mimic both bandwidth and latency characteristics of a flash memory device.

## Memory mode configurations

The preceding example outlines a typical configuration for *Ethos-U55* NPU, and this corresponds to the default
Vela memory mode setting.
Evaluation kit supports all the *Ethos-U* NPU memory modes:

|  *Ethos™-U* NPU  |   Default Memory Mode  |  Other Memory Modes supported  |
|------------------|------------------------|--------------------------------|
|   *Ethos™-U55*   |     `Shared_Sram`      |          `Sram_Only`           |
|   *Ethos™-U65*   |    `Dedicated_Sram`    |         `Shared_Sram`          |

For further information on the default settings, please refer to: [default_vela.ini](../../scripts/vela/default_vela.ini).
To use the other supported memory modes refer to [vela.ini](https://review.mlplatform.org/plugins/gitiles/ml/ethos-u/ethos-u-vela/+/refs/tags/3.2.0/vela.ini).
For *Ethos-U55* NPU, the system SRAM can also be used to store the neural network model along with the `tensor arena`.
Vela supports optimizing the model for this configuration with its `Sram_Only` memory mode.
Although the Vela settings for this configurations suggests that only AXI0 bus is used, when compiling the model
a warning is generated, for example:

```log
vela \
  --accelerator-config=ethos-u55-128 \
  --optimise Performance \
  --config scripts/vela/vela.ini
  --memory-mode=Sram_Only
  --system-config=Ethos_U55_High_End_Embedded
  ds_cnn_clustered_int8.tflite

Info: Changing const_mem_area from Sram to OnChipFlash. This will use the same characteristics as Sram.
```

This means that the  neural network model is always placed in the flash region. In this case, timing adapters for the
AXI buses are set the same values to mimic both bandwidth and latency characteristics of a SRAM memory device.
See [Ethos-U55 NPU timing adapter default configuration](../../scripts/cmake/timing_adapter/ta_config_u55_high_end.cmake).

For script snippets, please refer to: [Memory constraints](./memory_considerations.md#memory-constraints).

> **Note:**
>
> 1. The `Shared_Sram` memory mode represents the default configuration.
> 2. The `Dedicated_Sram` memory mode is only applicable for the Arm® *Ethos™-U65*.
> 3. The `Sram_only` memory mode is only applicable for the Arm® *Ethos™-U55*.

## Tensor arena and neural network model memory placement

The evaluation kit uses the name `activation buffer` for the `tensor arena` in the TensorFlow Lite Micro framework.
Every use-case application has a corresponding `<use_case_name>_ACTIVATION_BUF_SZ` parameter that governs the maximum
available size of the `activation buffer` for that particular use-case.

The linker script is set up to place this memory region in SRAM for *Ethos-U55* or in flash for *Ethos-U65* (following
the default memory modes of `Shared_Sram` and `Dedicated_Sram` respectively).

The neural network model is always placed in the flash region (even in case of `Sram_Only` memory mode as mentioned earlier).

## Memory usage for ML use-cases

The following numbers have been obtained from Vela for the `Shared_Sram` memory mode, along with the SRAM and flash
memory requirements for the different use-cases of the evaluation kit.

> **Note:** The SRAM usage does not include memory used by TensorFlow Lite Micro and must be topped up as explained
> under [Total SRAM used](./memory_considerations.md#total-sram-used).

- [Keyword spotting model](https://github.com/ARM-software/ML-zoo/tree/68b5fbc77ed28e67b2efc915997ea4477c1d9d5b//models/keyword_spotting/ds_cnn_large/tflite_clustered_int8)
  requires
  - 70.7 KiB of SRAM
  - 430.7 KiB of flash memory.

- [Image classification model](https://github.com/ARM-software/ML-zoo/tree/e0aa361b03c738047b9147d1a50e3f2dcb13dbcb/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8)
  requires
  - 638.6 KiB of SRAM
  - 3.1 MB of flash memory.

- [Automated speech recognition](https://github.com/ARM-software/ML-zoo/tree/1a92aa08c0de49a7304e0a7f3f59df6f4fd33ac8/models/speech_recognition/wav2letter/tflite_pruned_int8)
  requires
  - 655.16 KiB of SRAM
  - 13.42 MB of flash memory.

## Memory constraints

Both the MPS3 Fixed Virtual Platform (FVP) and the MPS3 FPGA platform share the linker script for Arm® *Corstone™-300*
design. The CMake configuration parameter `TARGET_SUBSYSTEM` sets the design, which is described in:
[build options](./building.md#build-options).

The memory map exposed by this design is located in the [Appendix](./appendix.md#appendix), and can be used as a reference when
editing the linker script. This is useful to make sure that the region boundaries are respected. The snippet from the
scatter file is as follows:

```log
;---------------------------------------------------------
; First load region (ITCM)
;---------------------------------------------------------
LOAD_REGION_0       0x00000000                  0x00080000
{

...

    ;-----------------------------------------------------
    ; SSE-300's internal SRAM of 4MiB - reserved for
    ; activation buffers.
    ; This region should have 3 cycle read latency from
    ; both Cortex-M55 and Ethos-U NPU
    ;-----------------------------------------------------
    isram.bin       0x31000000  UNINIT ALIGN 16 0x00400000
    {
        ; Cache area (if used)
        *.o (.bss.NoInit.ethos_u_cache)

        ; activation buffers a.k.a tensor arena when memory mode sram only
        *.o (.bss.NoInit.activation_buf_sram)
    }
}

;---------------------------------------------------------
; Second load region (DDR)
;---------------------------------------------------------
LOAD_REGION_1       0x70000000                  0x02000000
{
    ;-----------------------------------------------------
    ; 32 MiB of DDR space for neural network model,
    ; input vectors and labels. If the activation buffer
    ; size required by the network is bigger than the
    ; SRAM size available, it is accommodated here.
    ;-----------------------------------------------------
    ddr.bin        0x70000000 ALIGN 16         0x02000000
    {
        ; nn model's baked in input matrices
        *.o (ifm)

        ; nn model's default space
        *.o (nn_model)

        ; labels
        *.o (labels)

        ; activation buffers a.k.a tensor arena when memory mode dedicated sram
        *.o (activation_buf_dram)
    }

...

}

```

> **Note:** With Arm® *Corstone™-300* FPGA and FVP implementations, only the BRAM, internal SRAM, and DDR memory regions
> are accessible to the Arm® Ethos™-U NPU block.

In the preceding snippet, the internal SRAM region memory is utilized by the activation buffers with a limit of 4MiB. If
used by a Vela optimized neural network model, then the Arm® *Ethos™-U* NPU writes to this block frequently.

A bigger region of memory for storing the neural network model is placed in the DDR, or flash, region under
`LOAD_REGION_1`. The two load regions are necessary as the motherboard configuration controller of the MPS3 limits the
load size at address `0x00000000` to 1MiB. This has implications on how the application **is deployed** on MPS3, as
explained under the following section: [Deployment on MPS3](./deployment.md#mps3-board).

The next section of the documentation covers: [Troubleshooting](troubleshooting.md).
