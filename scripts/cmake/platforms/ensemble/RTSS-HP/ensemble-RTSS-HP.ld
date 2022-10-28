/* This file was ported to work on Alif Semiconductor Ensemble family of devices. */

/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

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
__HEAP_SIZE  = 0x00010000;
__ROM_BASE = 0x80001000;
__ROM_SIZE = 0x0047F000;

MEMORY
{
  ITCM  (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00040000
  DTCM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00100000
  SRAM0 (rwx) : ORIGIN = 0x02000000, LENGTH = 0x00400000
  SRAM1 (rwx) : ORIGIN = 0x08000000, LENGTH = 0x00280000
  MRAM  (rx)  : ORIGIN = __ROM_BASE, LENGTH = __ROM_SIZE
  TOC   (r)   : ORIGIN = 0x8057FFF0, LENGTH = 16
}

ENTRY(Reset_Handler)

SECTIONS
{
  .text.at_mram :
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

    *(ifm)
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
    LONG (__etext2)
    LONG (__data2_start__)
    LONG ((__data2_end__ - __data2_start__)/4)
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

  . = ALIGN(16);
  __etext = .;

  .data.at_dtcm : AT (__etext)
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

    . = ALIGN(16);
    __data_end__ = .;
  } > DTCM

  . = ALIGN(16);

  /* Place non-zero code/data here if required in SRAM0 */
/*
  __etext2 = __etext + (__data_end__ - __data_start__);

  .data.at_sram : AT (__etext2)
  {
    . = ALIGN(4);
    __data2_start__ = .;
    . = ALIGN(16);
    __data2_end__ = .;
  } > SRAM0
*/

  .data.at_sram0 (NOLOAD) :
  {
    . = ALIGN(4);
    __sram0_start__ = .;
    * (camera_frame_buf)              /* Camera Frame Buffer */
    * (camera_frame_bayer_to_rgb_buf) /* (Optional) Camera Frame Buffer for Bayer to RGB Convertion.*/
    * (lcd_image_buf)

    . = ALIGN(4);
    __sram0_end__ = .;
  } > SRAM0

  .data.at_sram1 (NOLOAD) :
  {
    . = ALIGN(4);
    __sram1_start__ = .;
    * (.bss.NoInit.activation_buf_sram) /* 2MB */
    . = ALIGN(4);
    __sram1_end__ = .;
  } > SRAM1

  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)


    . = ALIGN(4);
    __bss_end__ = .;
  } > DTCM

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

  /* Magic number (0xFFA0A710) + (0 = A32_0, 1 = A32_1, 2 = CM55HP, 3 = CM55HE)
  .mini_toc :
  {
    LONG (0xffffffff)
    LONG (0xFFA0A710 + 2)
    LONG (__ROM_BASE)
    LONG (0)
  } > TOC
  */
}
