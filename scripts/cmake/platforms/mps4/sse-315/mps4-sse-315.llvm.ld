/*
 * SPDX-FileCopyrightText: Copyright 2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

__STACK_SIZE = 0x00008000;
__HEAP_SIZE  = 0x000C0000;

/* System memory brief */
MEMORY
{
    ITCM  (rx)  : ORIGIN = 0x10000000, LENGTH = 0x00008000
    BOOT  (rx)  : ORIGIN = 0x11000000, LENGTH = 0x00010000
    BRAM  (rwx) : ORIGIN = 0x12000000, LENGTH = 0x00200000
    DTCM  (rwx) : ORIGIN = 0x30000000, LENGTH = 0x00008000
    SRAM  (rwx) : ORIGIN = 0x31000000, LENGTH = 0x00400000
    DDR   (rwx) : ORIGIN = 0x70000000, LENGTH = 0x02000000

    /* Dynamic load regions declared for use by FVP only
     * These regions are mentioned in the CMake subsystem profile.
     * Do not change the addresses here in isolation. */
    DDR_dynamic_model (rx) : ORIGIN = 0x90000000, LENGTH = 0x02000000
    DDR_dynamic_ifm   (rx) : ORIGIN = 0x92000000, LENGTH = 0x01000000
    DDR_dynamic_ofm   (rx) : ORIGIN = 0x93000000, LENGTH = 0x01000000
}

SECTIONS
{
    /**< Bare minimum boot code lives here. */
    .text.at_boot :
    {
        /* vector table */
        KEEP(*(.vectors))
        KEEP (*(.text.init.enter))
        KEEP (*(.data.init.enter))
        KEEP (*(SORT_BY_NAME(.init) SORT_BY_NAME(.init.*)))
    } > BOOT AT > BOOT

    /**< Most of the RO code lives in this space. */
    .text.at_bram :
    {
        *(.text*)
        *(.opd .opd.*)
        *(.gnu.linkonce.t.*)
        KEEP(*(.fini .fini.*))
        __text_end = .;
        PROVIDE (__etext = __text_end);
        PROVIDE (_etext = __text_end);
        PROVIDE (etext = __text_end);

        . = ALIGN(8);

        /* contructors and destructors */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*) SORT(.ctors.*)))
        KEEP(*(.init_array .ctors))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*) SORT(.dtors.*)))
        KEEP(*(.fini_array .dtors))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > BRAM AT > BRAM

    .lcxx_override.at_bram :
    {
        . = ALIGN(8);
        *(__lcxx_override*)
        . = ALIGN(8);
    } > BRAM AT > BRAM

    /* Read-only data */
    .rodata.at_bram :
    {
        *(.rdata)
        *(.rodata .rodata.*)
        *(.gnu.linkonce.r.*)

        *(.srodata.cst16)
        *(.srodata.cst8)
        *(.srodata.cst4)
        *(.srodata.cst2)
        *(.srodata .srodata.*)
        KEEP(*(.eh_frame*))
    } > BRAM AT > BRAM

    /* Read-write data */
    .data.at_bram :
    {
        . = ALIGN(16);
        *(.data)
        *(.data.*)
        . = ALIGN(16);
        *(.sdata .sdata.* .sdata2.*)
        *(.gnu.linkonce.s.*)
    } > BRAM AT > BRAM

    PROVIDE(__data_start = ADDR(.data.at_bram));
    PROVIDE(__data_source = LOADADDR(.data.at_bram));

    /* Dedicated section for global offset table */
    .got.at_bram :
    {
        . = ALIGN(8);
        *(.got*)
        . = ALIGN(8);
    } > BRAM AT > BRAM

    .tdata.at_bram : ALIGN(__tls_align)
    {
        *(.tdata .tdata.* .gnu.linkonce.td.*)
        PROVIDE(__data_end = .);
        PROVIDE(__tdata_end = .);
    } > BRAM AT > BRAM

    PROVIDE( __tls_base = ADDR(.tdata.at_bram));
    PROVIDE( __tdata_start = ADDR(.tdata.at_bram));
    PROVIDE( __tdata_source = LOADADDR(.tdata.at_bram) );
    PROVIDE( __tdata_source_end = LOADADDR(.tdata.at_bram) + SIZEOF(.tdata.at_bram) );
    PROVIDE( __data_source_end = __tdata_source_end );
    PROVIDE( __tdata_size = SIZEOF(.tdata.at_bram) );

    PROVIDE( __edata = __data_end );
    PROVIDE( _edata = __data_end );
    PROVIDE( edata = __data_end );
    PROVIDE( __data_size = __data_end - __data_start );
    PROVIDE( __data_source_size = __data_source_end - __data_source );

    .tbss.at_bram (NOLOAD) :
    {
        *(.tbss .tbss.* .gnu.linkonce.tb.*)
        *(.tcommon)
        PROVIDE( __tls_end = . );
        PROVIDE( __tbss_end = . );
    } > BRAM AT > BRAM

    PROVIDE( __bss_start = ADDR(.tbss.at_bram));
    PROVIDE( __tbss_start = ADDR(.tbss.at_bram));
    PROVIDE( __tbss_offset = ADDR(.tbss.at_bram) - ADDR(.tdata.at_bram) );
    PROVIDE( __tbss_size = SIZEOF(.tbss.at_bram) );
    PROVIDE( __tls_size = __tls_end - __tls_base );
    PROVIDE( __tls_align = MAX(ALIGNOF(.tdata.at_bram), ALIGNOF(.tbss.at_bram)) );
    PROVIDE( __tls_size_align = (__tls_size + __tls_align - 1) & ~(__tls_align - 1));
    PROVIDE( __arm32_tls_tcb_offset = MAX(8, __tls_align) );
    PROVIDE( __arm64_tls_tcb_offset = MAX(16, __tls_align) );

    /**
     * FPGA internal SRAM region used to hold activation buffers.
     * We declare it before to place it prior to other bss section.
     */
    .sram (NOLOAD):
    {
        . = ALIGN(16);
        /* Cache area (if used) */
        *(.bss.NoInit.ethos_u_cache)
        . = ALIGN (16);
        /* activation buffers a.k.a tensor arena when memory mode sram only or shared sram */
        *(.bss.NoInit.activation_buf_sram)
        . = ALIGN(16);
    } > SRAM AT > SRAM

    .bss.at_bram (NOLOAD) :
    {
        *(.sbss*)
        *(.gnu.linkonce.sb.*)
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)

        /* Align the heap */
        . = ALIGN(8);
        __bss_end = .;
    } > BRAM AT > BRAM

    PROVIDE( __non_tls_bss_start = ADDR(.bss.at_bram) );
    PROVIDE( __end = __bss_end );
    _end = __bss_end;
    PROVIDE( end = __bss_end );
    PROVIDE( __bss_size = __bss_end - __bss_start );

    /* Heap */
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        PROVIDE(__heap_start = .);
        . = . + __HEAP_SIZE;
        . = ALIGN(8);
        PROVIDE(__heap_end = .);
    } > BRAM

    PROVIDE(__head_size = __heap_end - __heap_start);

    /* Stack */
    .stack (ORIGIN(DTCM) + LENGTH(DTCM) - __STACK_SIZE) (NOLOAD) :
    {
        . = ALIGN(8);
        PROVIDE(__stack_limit = .);
        . = . + __STACK_SIZE;
        . = ALIGN(8);
        PROVIDE(__stack = .);
    } > DTCM
    ASSERT(__STACK_SIZE <= LENGTH(DTCM), "DTCM overflow")
    PROVIDE(__stack_size = __STACK_SIZE);

    .ddr.at_ddr :
    {
        /* __attribute__((aligned(16))) may not handled by the CMSIS startup code.
         * Force the alignment here as a workaround */
        . = ALIGN(16);
        /* nn model's baked in input matrices */
        *(ifm)
        . = ALIGN(16);
        /* nn model's default space */
        *(nn_model)
        . = ALIGN (16);
        /* labels */
        *(labels)
        . = ALIGN (16);
        *Labels*.obj (*.rodata*)
        . = ALIGN (16);
        /* activation buffers a.k.a tensor arena when memory mode dedicated sram */
        *(activation_buf_dram)
        . = ALIGN (16);
    } > DDR AT > DDR
} /* SECTIONS */
