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

__STACK_SIZE = 0x00001800;
__HEAP_SIZE  = 0x00008000;
__ROM_BASE = 0x80008000;
__ROM_SIZE = 0x0047F000;
__OSPI_FLASH_BASE = 0xC0000000;
__OSPI_FLASH_SIZE = 0x02000000;

MEMORY
{
  ITCM  (rwx) : ORIGIN = 0x01000000, LENGTH = 0x00040000
  DTCM  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000
  SRAM0 (rwx) : ORIGIN = 0x02000000, LENGTH = 0x00400000
  SRAM1 (rwx) : ORIGIN = 0x08000000, LENGTH = 0x00280000
  MRAM  (rx)  : ORIGIN = __ROM_BASE, LENGTH = __ROM_SIZE
  OSPI_FLASH (rx) : ORIGIN = __OSPI_FLASH_BASE, LENGTH = __OSPI_FLASH_SIZE
}

ENTRY(Reset_Handler)

SECTIONS
{
  .startup.at_mram : ALIGN(16)
  {
    /* MRAM has all read-only code and data except fast LVGL code and some maths tables
     * This memory layout is a safe default for high-memory builds like the inference runner
     * where the usual attempts to put inference code into ITCM dont fit
     */
    /* Only more-specific stuff here. General wildcards in
     * a later-declared section, giving a chance for other
     * regions to get in before-hand
     */
    KEEP(*(.vectors))
    *(startup_ro_data)
  } > MRAM

  .copy.table.at_mram : ALIGN(4)
  {
    __copy_table_start__ = .;
    LONG ( LOADADDR(.text.itcm.at_mram) )
    LONG ( ADDR(.text.itcm.at_mram) )
    LONG ( SIZEOF(.text.itcm.at_mram)/4 )
    LONG ( LOADADDR(.data.dtcm.at_mram) )
    LONG ( ADDR(.data.dtcm.at_mram) )
    LONG ( SIZEOF(.data.dtcm.at_mram)/4 )
    LONG ( LOADADDR(.data.sram0.at_mram) )
    LONG ( ADDR(.data.sram0.at_mram) )
    LONG ( SIZEOF(.data.sram0.at_mram)/4 )
    __copy_table_end__ = .;
  } > MRAM

  .zero.table.at_mram : ALIGN(4)
  {
    __zero_table_start__ = .;
    LONG (ADDR(.bss.itcm))
    LONG (SIZEOF(.bss.itcm)/4)
    LONG (ADDR(.bss))
    LONG (SIZEOF(.bss)/4)
    LONG (ADDR(.bss.sram0))
    LONG (SIZEOF(.bss.sram0)/4)
    LONG (ADDR(.bss.sram1))
    LONG (SIZEOF(.bss.sram1)/4)
    __zero_table_end__ = .;
  } > MRAM

  .vectors.itcm (NOLOAD) : ALIGN(2048)
  {
    *(.bss.noinit.ram_vectors)
  } > ITCM

  .text.itcm.at_mram : ALIGN(8)
  {
    /* ITCM has only fast LVGL and RAM vectors */
    *(fast_code)
  } > ITCM AT> MRAM

  .data.dtcm.at_mram : ALIGN(8)
  {
    /* DTCM has all read-write data, including NPU activation buffer and audio processing buffers */
    *(vtable)
    *(.data)
    *(.data*)

    KEEP(*(.jcr*))

  } > DTCM AT> MRAM 

  /* Place non-zero code/data here if required in SRAM0 */
  .data.sram0.at_mram : ALIGN(8)
  {
    * (ifm)
  } > SRAM0 AT> MRAM

  .bss.sram0 (NOLOAD) : ALIGN(8)
  {
    * (.bss.large_ram)                     /* Large LVGL buffers */
    * (.bss.camera_frame_buf)              /* Camera Frame Buffer */
    * (.bss.camera_frame_bayer_to_rgb_buf) /* (Optional) Camera Frame Buffer for Bayer to RGB Convertion.*/
    * (.bss.lcd_image_buf)
  } > SRAM0

  .bss.sram1 (NOLOAD) : ALIGN(8)
  {
    * (.bss.NoInit.activation_buf_sram) /* 2MB */
  } > SRAM1

  .bss.itcm (NOLOAD) : ALIGN(8)
  {
  } > ITCM

  .bss (NOLOAD) : ALIGN(8)
  {
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(8);
    __bss_end__ = .;
  } > DTCM

  .heap (NOLOAD) : ALIGN(8)
  {
    __end__ = .;
    PROVIDE(end = .);
    . = . + __HEAP_SIZE;
    . = ALIGN(8);
    __HeapLimit = .;
  } > DTCM

  .stack (ORIGIN(DTCM) + LENGTH(DTCM) - __STACK_SIZE) (NOLOAD) : ALIGN(8)
  {
    __StackLimit = .;
    . = . + __STACK_SIZE;
    . = ALIGN(8);
    __StackTop = .;
  } > DTCM
  PROVIDE(__stack = __StackTop);

  .readonly.at_mram : ALIGN(8)
  {
    /* Use wildcards to mop up any read-only not directed to TCM */ 
    KEEP(*(.init))
    KEEP(*(.fini))

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

    . = ALIGN(8);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(8);
    __exidx_start = .;
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    __exidx_end = .;

    . = ALIGN(4);
    *(.text*)
    *(.rodata*)

    *(nn_model)
    *(labels)
    *(ifm)

    KEEP(*(.eh_frame*))
  } > MRAM

  .readonly.at_ext_flash : ALIGN(8)
  {
    *(nn_model_ext_flash)
  } > OSPI_FLASH
}
