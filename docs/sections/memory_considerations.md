# Memory considerations

- [Memory considerations](#memory-considerations)
  - [Introduction](#introduction)
  - [Memory available on the target platform](#memory-available-on-the-target-platform)
    - [Parameters linked to SRAM size definitions](#parameters-linked-to-sram-size-definitions)
  - [Understanding memory usage from Vela output](#understanding-memory-usage-from-vela-output)
    - [Total SRAM used](#total-sram-used)
    - [Total Off-chip Flash used](#total-off_chip-flash-used)
  - [Non-default configurations](#non-default-configurations)
  - [Tensor arena and neural network model memory placement](#tensor-arena-and-neural-network-model-memory-placement)
  - [Memory usage for ML use-cases](#memory-usage-for-ml-use_cases)
  - [Memory constraints](#memory-constraints)

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

Embedded target platforms supported have a description in the form of CMake files. These files
have definitions that describe the memory regions and the peripheral base addresses.

See the example for Arm® *Corstone™-300* description file [corstone-sse-300.cmake](../../scripts/cmake/subsystem-profiles/corstone-sse-300.cmake). For the discussion on this page, it is useful to note the following definitions:

```
set(ISRAM0_SIZE           "0x00200000" CACHE STRING "ISRAM0 size:       2 MiB")
set(ISRAM1_SIZE           "0x00200000" CACHE STRING "ISRAM1 size:       2 MiB")
...
# SRAM size reserved for activation buffers
math(EXPR ACTIVATION_BUF_SRAM_SZ "${ISRAM0_SIZE} + ${ISRAM1_SIZE}" OUTPUT_FORMAT HEXADECIMAL)
```
This will set `ACTIVATION_BUF_SRAM_SZ` to be **4 MiB** for Arm® *Corstone™-300* target platform.
As mentioned in the comments within the file, this size is directly linked to the size mentioned
in the linker scripts, and therefore, it should not be changed without corresponding changes
in the linker script too. For example, a snippet from the scatter file for Corstone™-300 shows:

```
;-----------------------------------------------------
; SSE-300's internal SRAM of 4MiB - reserved for
; activation buffers.
; This region should have 3 cycle read latency from
; both Cortex-M55 and Ethos-U NPU
;-----------------------------------------------------
isram.bin       0x31000000  UNINIT ALIGN 16 0x00400000
{
  ...
}
```
If the usable size of the internal SRAM was to be increased/decreased, the change should be
made in both the linker script as well as the `corstone-300.cmake` definition.

### Parameters linked to SRAM size definitions

Other than the obvious link between the linker script and the target profile description in
CMake files, there are other parameters linked to what the reserved space for activation
buffers is. These are:

- The file [default_vela.ini](../../scripts/vela/default_vela.ini) contains a parameter called
  `arena_cache_size` under `Shared_Sram` memory mode. For example:
  ```
  [Memory_Mode.Shared_Sram]
  const_mem_area=Axi1
  arena_mem_area=Axi0
  cache_mem_area=Axi0
  arena_cache_size=4194304
  ```
  This size of **4 MiB** here is provided here to allow the default vela optimisation process to
  use this size as a hint for the available SRAM size for use by the CPU and the NPU.

- In every `usecase.cmake` file (present within each use case's source directory), there is
  a parameter called `${use_case}_ACTIVATION_BUF_SZ` set to a fixed value by default. This
  default value should be less than the `ACTIVATION_BUF_SRAM_SZ` if the activation buffer needs
  to be reserved in the target platform's SRAM region.

## Understanding memory usage from Vela output

### Total SRAM used

When the neural network model is compiled with Vela, a summary report that includes memory usage is generated. For
example, compiling the keyword spotting model
[ds_cnn_clustered_int8](https://github.com/ARM-software/ML-zoo/blob/master/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/ds_cnn_clustered_int8.tflite)
with Vela produces, among others, the following output:

```log
Total SRAM used                                 70.77 KiB
Total Off-chip Flash used                      430.78 KiB
```

The `Total SRAM used` here shows the required memory to store the `tensor arena` for the TensorFlow Lite Micro
framework. This is the amount of memory required to store the input, output, and intermediate buffers. In the preceding
example, the tensor arena requires 70.77 KiB of available SRAM.

> **Note:** Vela can only estimate the SRAM required for graph execution. It has no way of estimating the memory used by
> internal structures from TensorFlow Lite Micro framework.

Therefore, we recommend that you top this memory size by at least 2KiB. We also recoomend that you also carve out the
`tensor arena` of this size, and then place it on the SRAM of the target system.

### Total Off-chip Flash used

The `Total Off-chip Flash` parameter indicates the minimum amount of flash required to store the neural network model.
In the preceding example, the system must have a minimum of 430.78 KiB of available flash memory to store the `.tflite`
file contents.

> **Note:** The Arm® *Corstone™-300* system uses the DDR region as a flash memory. The timing adapter sets up the AXI
> bus that is wired to the DDR to mimic both bandwidth and latency characteristics of a flash memory device.

## Non-default configurations

The preceding example outlines a typical configuration, and this corresponds to the default Vela setting. However, the
system SRAM can also be used to store the neural network model along with the `tensor arena`. Vela supports optimizing
the model for this configuration with its `Sram_Only` memory mode.

For further information, please refer to: [vela.ini](../../scripts/vela/vela.ini).

To make use of a neural network model that is optimized for this configuration, the linker script for the target
platform must be changed. By default, the linker scripts are set up to support the default configuration only.

For script snippets, please refer to: [Memory constraints](./memory_considerations.md#memory-constraints).

> **Note:**
>
> 1. The the `Shared_Sram` memory mode represents the default configuration.
> 2. The `Dedicated_Sram` mode is only applicable for the Arm® *Ethos™-U65*.

## Tensor arena and neural network model memory placement

The evaluation kit uses the name `activation buffer` for the `tensor arena` in the TensorFlow Lite Micro framework.
Every use-case application has a corresponding `<use_case_name>_ACTIVATION_BUF_SZ` parameter that governs the maximum
available size of the `activation buffer` for that particular use-case.

The linker script is set up to place this memory region in SRAM. However, if the memory required is more than what the
target platform supports, this buffer needs to be placed on flash instead. Every target platform has a profile
definition in the form of a `CMake` file.

For further information and an example, please refer to: [Corstone-300 profile](../../scripts/cmake/subsystem-profiles/corstone-sse-300.cmake).

The parameter `ACTIVATION_BUF_SRAM_SZ` defines the maximum SRAM size available for the platform. This is propagated
through the build system. If the `<use_case_name>_ACTIVATION_BUF_SZ` for a given use-case is *more* than the
`ACTIVATION_BUF_SRAM_SZ` for the target build platform, then the `activation buffer` is placed on the flash memory
instead.

The neural network model is always placed in the flash region. However, this can be changed in the linker script.

## Memory usage for ML use-cases

The following numbers have been obtained from Vela for the `Shared_Sram` memory mode, along with the SRAM and flash
memory requirements for the different use-cases of the evaluation kit.

> **Note:** The SRAM usage does not include memory used by TensorFlow Lite Micro and must be topped up as explained
> under [Total SRAM used](#total-sram-used).

- [Keyword spotting model](https://github.com/ARM-software/ML-zoo/tree/master/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8)
  requires
  - 70.7 KiB of SRAM
  - 430.7 KiB of flash memory.

- [Image classification model](https://github.com/ARM-software/ML-zoo/tree/master/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8)
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
    ;-----------------------------------------------------
    ; First part of code mem - 512kiB
    ;-----------------------------------------------------
    itcm.bin        0x00000000                  0x00080000
    {
        *.o (RESET, +First)
        * (InRoot$$Sections)

        ; Essentially only RO-CODE, RO-DATA is in a
        ; different region.
        .ANY (+RO)
    }

    ;-----------------------------------------------------
    ; 128kiB of 512kiB DTCM is used for any other RW or ZI
    ; data. Note: this region is internal to the Cortex-M
    ; CPU.
    ;-----------------------------------------------------
    dtcm.bin        0x20000000                  0x00020000
    {
        ; Any R/W and/or zero initialised data
        .ANY(+RW +ZI)
    }

    ;-----------------------------------------------------
    ; 384kiB of stack space within the DTCM region. See
    ; `dtcm.bin` for the first section. Note: by virtue of
    ; being part of DTCM, this region is only accessible
    ; from Cortex-M55.
    ;-----------------------------------------------------
    ARM_LIB_STACK   0x20020000 EMPTY ALIGN 8    0x00060000
    {}

    ;-----------------------------------------------------
    ; SSE-300's internal SRAM of 4MiB - reserved for
    ; activation buffers.
    ; This region should have 3 cycle read latency from
    ; both Cortex-M55 and Ethos-U NPU
    ;-----------------------------------------------------
    isram.bin       0x31000000  UNINIT ALIGN 16 0x00400000
    {
        ; activation buffers a.k.a tensor arena
        *.o (.bss.NoInit.activation_buf)
    }
}

;---------------------------------------------------------
; Second load region (DDR)
;---------------------------------------------------------
LOAD_REGION_1       0x70000000                  0x02000000
{
    ;-----------------------------------------------------
    ; 32 MiB of DRAM space for neural network model,
    ; input vectors and labels. If the activation buffer
    ; size required by the network is bigger than the
    ; SRAM size available, it is accommodated here.
    ;-----------------------------------------------------
    ddr.bin        0x70000000 ALIGN 16         0x02000000
    {
        ; nn model's baked in input matrices
        *.o (ifm)

        ; nn model
        *.o (nn_model)

        ; labels
        *.o (labels)

        ; if the activation buffer (tensor arena) doesn't
        ; fit in the SRAM region, we accommodate it here
        *.o (activation_buf)
    }

    ;-----------------------------------------------------
    ; First 256kiB of BRAM (FPGA SRAM) used for RO data.
    ; Note: Total BRAM size available is 2MiB.
    ;-----------------------------------------------------
    bram.bin        0x11000000          ALIGN 8 0x00040000
    {
        ; RO data (incl. unwinding tables for debugging)
        .ANY (+RO-DATA)
    }

    ;-----------------------------------------------------
    ; Remaining part of the 2MiB BRAM used as heap space.
    ; 0x00200000 - 0x00040000 = 0x001C0000 (1.75 MiB)
    ;-----------------------------------------------------
    ARM_LIB_HEAP    0x11040000 EMPTY ALIGN 8    0x001C0000
    {}
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
