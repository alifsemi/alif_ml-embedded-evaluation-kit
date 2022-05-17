/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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
  DTCM  (rwx) : ORIGIN = 0x30000000, LENGTH = 0x00008000
  BRAM  (rwx) : ORIGIN = 0x11000000, LENGTH = 0x00200000
  SRAM  (rwx) : ORIGIN = 0x31000000, LENGTH = 0x00400000
  DDR   (rwx) : ORIGIN = 0x70000000, LENGTH = 0x02000000

  /* Dynamic load regions declared for use by FVP only
   * These regions are mentioned in the CMake subsystem profile.
   * Do not change the addresses here in isolation. */
  DDR_dynamic_model (rx) : ORIGIN = 0x90000000, LENGTH = 0x02000000
  DDR_dynamic_ifm   (rx) : ORIGIN = 0x92000000, LENGTH = 0x01000000
  DDR_dynamic_ofm   (rx) : ORIGIN = 0x93000000, LENGTH = 0x01000000
}

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions ITCM and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */
ENTRY(Reset_Handler)

SECTIONS
{
  .text.at_bram :
  {
    KEEP(*(.vectors))

    KEEP(*(.init))
    KEEP(*(.fini))

    /* .ctors */
    *crtbegin.o(.ctors)
    *crtbegin?.o(.ctors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
    *(SORT(.ctors.*))
    *(.ctors)

    /* .dtors */
    *crtbegin.o(.dtors)
    *crtbegin?.o(.dtors)
    *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
    *(SORT(.dtors.*))
    *(.dtors)

    KEEP(*(.eh_frame*))
    
    *(vtable)
    *(.data)
    *(.data.*)
    . = ALIGN(4);
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
    . = ALIGN(4);
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);
    . = ALIGN(4);
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);
    KEEP(*(.jcr*))
    . = ALIGN(4);

    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);

    *(.rodata*)
    . = ALIGN(4);
    * (npu_driver_version)
    . = ALIGN(4);
    * (npu_driver_arch_version)
    . = ALIGN(4);

    __copy_table_start__ = .;
    . = ALIGN(4);
    __copy_table_end__ = .;
  } > BRAM

  __exidx_start = .;
  .ARM.exidx.at_bram :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > BRAM
  __exidx_end = .;

  .sram :
  {
    . = ALIGN(16);
    /* Cache area (if used) */
    *(.bss.NoInit.ethos_u_cache)
    . = ALIGN (16);
    /* activation buffers a.k.a tensor arena when memory mode sram only or shared sram */
    *(.bss.NoInit.activation_buf_sram)
    . = ALIGN(16);
  } > SRAM AT > SRAM

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > BRAM

  .zero.table.at_bram :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;

    LONG (__bss_start__)
    LONG ((__bss_end__ - __bss_start__)/4) /* Size is in 32-bit words */

    __zero_table_end__ = .;
  } > BRAM
  
  .heap (COPY) :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > BRAM

  __bram_total = ALIGN(4);

  ASSERT( __bram_total < (ORIGIN(BRAM) + LENGTH(BRAM)), "BRAM overflow")

  .stack (ORIGIN(DTCM) + LENGTH(DTCM) - __STACK_SIZE) (COPY) :
  {
    . = ALIGN(8);
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
  } > DTCM
  PROVIDE(__stack = __StackTop);
  ASSERT(__STACK_SIZE <= LENGTH(DTCM), "DTCM overflow")

  .ddr.at_ddr :
  {
    /* __attribute__((aligned(16))) is not handled by the CMSIS startup code.
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
    /* activation buffers a.k.a tensor arena when memory mode dedicated sram */
    *(activation_buf_dram)
    . = ALIGN (16);
  } > DDR AT > DDR

}
