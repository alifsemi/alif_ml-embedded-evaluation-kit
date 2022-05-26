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

__STACK_SIZE = 0x00060000;
__HEAP_SIZE  = 0x000f0000;

/* System memory brief */
MEMORY
{
  ITCM  (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00040000
  DTCM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000
  SRAM0 (rwx) : ORIGIN = 0x02000000, LENGTH = 0x00400000
  SRAM1 (rwx) : ORIGIN = 0x08000000, LENGTH = 0x00280000
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
  .text.at_itcm :
  {
    KEEP(*(.vectors))
    *(.text*)

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
  } > ITCM

  .ARM.extab.at_itcm :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > ITCM

  __exidx_start = .;
  .ARM.exidx.at_itcm :
  {
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
  } > ITCM
  __exidx_end = .;

  .zero.table.at_itcm :
  {
    . = ALIGN(4);
    __zero_table_start__ = .;

    LONG (__bss_start__)
    LONG ((__bss_end__ - __bss_start__)/4) /* Size is in 32-bit words */

    __zero_table_end__ = .;
  } > ITCM

  .copy.table.at_itcm :
  {
    . = ALIGN(4);
    __copy_table_start__ = .;

    /* Section to be copied - part 1: any data to be placed in BRAM */
    LONG (__etext)
    LONG (__data_start__)
    LONG ((__data_end__ - __data_start__)/4) /* Size is in 32-bit words */

    /* Section to be copied - part 2: RO data for for DTCM */
    LONG (__etext2)
    LONG (__ro_data_start__)
    LONG ((__ro_data_end__ - __ro_data_start__)/4) /* Size is in 32-bit words */

    __copy_table_end__ = .;
  } > ITCM

  __itcm_total = ALIGN(4);

  ASSERT( __itcm_total < (ORIGIN(ITCM) + LENGTH(ITCM)), "ITCM overflow")

  .sram :
  {
    . = ALIGN(16);
    *(.bss.NoInit.activation_buf)
    . = ALIGN(16);
  } > SRAM0 AT > SRAM0

  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > DTCM AT > DTCM

  .stack (ORIGIN(DTCM) + LENGTH(DTCM) - __STACK_SIZE) (COPY) :
  {
    . = ALIGN(8);
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
  } > DTCM
  PROVIDE(__stack = __StackTop);
  ASSERT(
    (__STACK_SIZE + __bss_end__ - __bss_start__) <= LENGTH(DTCM),
    "DTCM overflow")

  .ddr.at_ddr :
  {
    /* __attribute__((aligned(16))) is not handled by the CMSIS startup code.
     * Force the alignment here as a workaround */
    . = ALIGN(16);
    *(ifm)
    . = ALIGN(16);
    *(nn_model)
    . = ALIGN (16);
    *(labels)
    . = ALIGN (16);
    *(activation_buf)
    . = ALIGN (16);
  } > SRAM0 AT > SRAM0

  /**
   * Location counter can end up 2byte aligned with narrow Thumb code but
   * __etext is assumed by startup code to be the LMA of a section in DTCM
   * which must be 4byte aligned
   */
  __etext = ALIGN (4);

  .bram.at_ddr :  AT (__etext)
  {
    __data_start__ = .;
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

    __data_end__ = .;
  } > SRAM1

  __etext2 = __etext + (__data_end__ - __data_start__);

  .data.at_ddr : AT (__etext2)
  {
    . = ALIGN(4);
    __ro_data_start__ = .;

    *(.rodata*)
    . = ALIGN(4);
    * (npu_driver_version)
    . = ALIGN(4);
    * (npu_driver_arch_version)
    . = ALIGN(4);

    __ro_data_end__ = .;
  } > SRAM1

  .heap (COPY) :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > SRAM1

  ASSERT (
      (__ro_data_end__ - __ro_data_start__)
    + (__data_end__  - __data_start__)
    + __HEAP_SIZE <= LENGTH(SRAM1),
    "SRAM1 overflow")
}
