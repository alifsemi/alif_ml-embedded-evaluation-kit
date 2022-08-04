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

__STACK_SIZE = 0x00004000;
__HEAP_SIZE  = 0x00008000;
__ROM_BASE = 0x80480000;
__ROM_SIZE = 0x000FF000;

MEMORY
{
  ITCM  (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00040000
  DTCM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000
  SRAM0 (rwx) : ORIGIN = 0x02280000, LENGTH = 0x00180000
  SRAM1 (rwx) : ORIGIN = 0x08000000, LENGTH = 0x00280000
  MRAM  (rx)  : ORIGIN = __ROM_BASE, LENGTH = __ROM_SIZE
  TOC   (r)   : ORIGIN = 0x8057FFF0, LENGTH = 16
}

ENTRY(Reset_Handler)

SECTIONS
{
  .text :
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

    *(.rodata*)
    *(startup_ro_data)

    *(npu_driver_version)
    *(npu_driver_arch_version)

    *(nn_model)
    *(labels)

    KEEP(*(.eh_frame*))
    . = ALIGN(16);
  } > MRAM

  .ARM.extab :
  {
    . = ALIGN(16);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(16);
  } > MRAM

  __exidx_start = .;
  .ARM.exidx :
  {
    . = ALIGN(16);
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    . = ALIGN(16);
  } > MRAM
  __exidx_end = .;

  .copy.table :
  {
    . = ALIGN(16);
    __copy_table_start__ = .;
    LONG (__etext)
    LONG (__data_start__)
    LONG ((__data_end__ - __data_start__)/4)
/*
    LONG (__edata)
    LONG (__itcm_start__)
    LONG ((__itcm_end__ - __itcm_start__)/4)
*/
    __copy_table_end__ = .;
    . = ALIGN(16);
  } > MRAM

  .zero.table :
  {
    . = ALIGN(16);
    __zero_table_start__ = .;
    LONG (__bss_start__)
    LONG ((__bss_end__ - __bss_start__)/4)
    LONG (__sram0_start__)
    LONG ((__sram0_end__ - __sram0_start__)/4)
    LONG (__sram1_start__)
    LONG ((__sram1_end__ - __sram1_start__)/4)
    __zero_table_end__ = .;
    . = ALIGN(16);
  } > MRAM

  . = ALIGN (16);
  __etext = .;

  .data :
  {
    __data_start__ = .;
    *(vtable)
    *(.data)
    *(.data*)

    . = ALIGN(4);
    /* preinit data */
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);

    . = ALIGN(4);
    /* init data */
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);

    . = ALIGN(4);
    /* finit data */
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);

    KEEP(*(.jcr*))

    __data_end__ = .;
    . = ALIGN(16);
  } > DTCM AT >MRAM
/*
  __edata = __etext + (__data_end__ - __data_start__);
  . = ALIGN (16);

  .text.at_itcm :
  {
    __itcm_start__ = .;
    __itcm_end__ = .;
  } > ITCM AT >MRAM
*/
  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
  } > SRAM0

  .heap (NOLOAD) :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > DTCM

  .stack (ORIGIN(DTCM) + LENGTH(DTCM) - __STACK_SIZE) (NOLOAD) :
  {
    . = ALIGN(8);
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
  } > DTCM
  PROVIDE(__stack = __StackTop);

  .data.at_sram0 (NOLOAD) :
  {
    __sram0_start__ = .;
    * (.bss.NoInit.activation_buf_sram)
    * (activation_buf_dram)
    * (ifm)
    __sram0_end__ = .;
  } > SRAM0

  .data.at_sram1 (NOLOAD) :
  {
    __sram1_start__ = .;
    __sram1_end__ = .;
  } > SRAM1

  /* Magic number (0xFFA0A710) + (0 = A32_0, 1 = A32_1, 2 = CM55HP, 3 = CM55HE)
  .mini_toc :
  {
    LONG (0xffffffff)
    LONG (0xFFA0A710 + 3)
    LONG (__ROM_BASE)
    LONG (0)
  } > TOC
  */
}
