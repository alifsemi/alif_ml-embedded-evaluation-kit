# Memory considerations

## Table of Contents

- [Memory considerations](#memory-considerations)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Understanding memory usage from Vela output](#understanding-memory-usage-from-vela-output)
    - [Total SRAM used](#total-sram-used)
    - [Total Off-chip Flash used](#total-off-chip-flash-used)
  - [Non-default configurations](#non-default-configurations)
  - [Tensor arena and neural network model memory placement](#tensor-arena-and-neural-network-model-memory-placement)
  - [Memory usage for ML use cases](#memory-usage-for-ml-use-cases)
  - [Memory constraints](#memory-constraints)

## Introduction

This section provides useful details on how the Machine Learning use cases of the
evaluation kit use the system memory. Although the guidance provided here is with
respect to the Arm® Corstone™-300 system, it is fairly generic and is applicable
for other platforms too. Arm® Corstone™-300 is composed of Arm® Cortex™-M55 and
Arm® Ethos™-U55 and the memory map for the Arm® Cortex™-M55 core can be found in the
[Appendix 1](./appendix.md).

The Arm® Ethos™-U55 NPU interacts with the system via two AXI interfaces. The first one
is envisaged to be the higher bandwidth and/or lower latency interface. In a typical
system would this be wired to an SRAM as it would be required to service frequent R/W
traffic. The second interface is expected to have higher latency and/or lower bandwidth
characteristics and would typically be wired to a flash device servicing read-only
traffic. In this configuration, the Arm® Cortex™-M55 CPU and Arm® Ethos™-U55 NPU read the contents
of the neural network model (`.tflite file`) from the flash memory region with Arm® Ethos™-U55
requesting these read transactions over its second AXI bus.
The input and output tensors, along with any intermediate computation buffers, would be
placed on SRAM and both the Arm® Cortex™-M55 CPU and Arm® Ethos™-U55 NPU would be reading/writing
to this region when running an inference. The Arm® Ethos™-U55 NPU will be requesting these R/W
transactions over the first AXI bus.

## Understanding memory usage from Vela output

### Total SRAM used

When the neural network model is compiled with Vela, a summary report that includes memory
usage is generated. For example, compiling the keyword spotting model [ds_cnn_clustered_int8](https://github.com/ARM-software/ML-zoo/blob/master/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8/ds_cnn_clustered_int8.tflite)
with Vela produces, among others, the following output:

```
Total SRAM used                                 70.77 KiB
Total Off-chip Flash used                      430.78 KiB
```

The `Total SRAM used` here indicates the required memory to store the `tensor arena` for the
TensorFlow Lite Micro framework. This is the amount of memory required to store the input,
output and intermediate buffers. In the example above, the tensor arena requires 70.77 KiB
of available SRAM. Note that Vela can only estimate the SRAM required for graph execution.
It has no way of estimating the memory used by internal structures from TensorFlow Lite
Micro framework. Therefore, it is recommended to top this memory size by at least 2KiB and
carve out the `tensor arena` of this size and place it on the target system's SRAM.

### Total Off-chip Flash used

The `Total Off-chip Flash` parameter indicates the minimum amount of flash required to store
the neural network model. In the example above, the system needs to have a minimum of 430.78
KiB of available flash memory to store `.tflite` file contents.

> Note: For Arm® Corstone™-300 system we use the DDR region as a flash memory. The timing
> adapter sets up AXI bus wired to the DDR to mimic bandwidth and latency
> characteristics of a flash memory device.

## Non-default configurations

The above example outlines a typical configuration, and this corresponds to the default Vela
setting. However, the system SRAM can also be used to store the neural network model along with
the `tensor arena`. Vela supports optimizing the model for this configuration with its `Sram_Only`
memory mode. See [vela.ini](../../scripts/vela/vela.ini). To make use of a neural network model
optimised for this configuration, the linker script for the target platform would need to be
changed. By default, the linker scripts are set up to support the default configuration only. See
[Memory constraints](#Memory-constraints) for snippet of a script.

> Note
> 1. The default configuration is represented by `Shared_Sram` memory mode.
> 2. `Dedicated_Sram` mode is only applicable for Arm® Ethos™-U65.

## Tensor arena and neural network model memory placement

The evaluation kit uses the name `activation buffer` for what is called `tensor arena` in the
TensorFlow Lite Micro framework. Every use case application has a corresponding
`<use_case_name>_ACTIVATION_BUF_SZ` parameter that governs the maximum available size of the
`activation buffer` for that particular use case.

The linker script is set up to place this memory region in SRAM. However, if the memory required
is more than what the target platform supports, this buffer needs to be placed on flash instead.
Every target platform has a profile definition in the form of a CMake file. See [Corstone-300
profile](../../scripts/cmake/subsystem-profiles/corstone-sse-300.cmake) for example. The parameter
`ACTIVATION_BUF_SRAM_SZ` defines the maximum SRAM size available for the platform. This is
propagated through the build system and if the `<use_case_name>_ACTIVATION_BUF_SZ` for a given
use case is more than the `ACTIVATION_BUF_SRAM_SZ` for the target build platform, the `activation buffer`
is placed on the flash instead.

The neural network model is always placed in the flash region. However, this can be changed easily
in the linker script.

## Memory usage for ML use cases

The following numbers have been obtained from Vela for `Shared_Sram` memory mode and the SRAM and
flash memory requirements for the different use cases of the evaluation kit. Note that the SRAM usage
does not include memory used by TensorFlow Lite Micro and this will need to be topped up as explained
under [Total SRAM used](#Total-SRAM-used).

- [Keyword spotting model](https://github.com/ARM-software/ML-zoo/tree/master/models/keyword_spotting/ds_cnn_large/tflite_clustered_int8) requires
  -  70.7 KiB of SRAM
  -  430.7 KiB of flash memory.

- [Image classification model](https://github.com/ARM-software/ML-zoo/tree/master/models/image_classification/mobilenet_v2_1.0_224/tflite_uint8) requires
  - 638.6 KiB of SRAM
  - 3.1 MB of flash memory.

- [Automated speech recognition](https://github.com/ARM-software/ML-zoo/tree/master/models/speech_recognition/wav2letter/tflite_int8) requires
  - 635.3 KiB of SRAM
  - 21.1 MB of flash memory.

## Memory constraints

Both the MPS3 Fixed Virtual Platform and the MPS3 FPGA platform share the linker script for Arm® Corstone™-300
design. The design is set by the CMake configuration parameter `TARGET_SUBSYSTEM` as described in
[build options](./building.md#Build-options).

The memory map exposed by this design is presented in [Appendix 1](./appendix.md). This can be used as a reference
when editing the linker script, especially to make sure that region boundaries are respected. The snippet from the
scatter file is presented below:

```
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
    ; both Cortex-M55 and Ethos-U55
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
    dram.bin        0x70000000 ALIGN 16         0x02000000
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

It is worth noting that for the Arm® Corstone™-300 FPGA and FVP implementations, only the BRAM,
internal SRAM and DDR memory regions are accessible to the Arm® Ethos™-U55 NPU block. In the above
snippet, the internal SRAM region memory can be seen to be utilized by activation buffers with
a limit of 4MiB. If used by a Vela optimised neural network model, this region will be written to
by the Arm® Ethos™-U55 NPU block frequently. A bigger region of memory for storing the neural
network model is placed in the DDR/flash region under LOAD_REGION_1. The two load regions are necessary
as the MPS3's motherboard configuration controller limits the load size at address 0x00000000 to 1MiB.
This has implications on how the application **is deployed** on MPS3 as explained under the section
[Deployment on MPS3](./deployment.md#MPS3-board).
