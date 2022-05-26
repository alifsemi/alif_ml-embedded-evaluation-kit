/*
 * tgu_M55_HP.c Contains routine(s) to program the TGU configuration registers
 * and look up table registers.
 *
 * Author   : Silesh C V <silesh@alifsemi.com>
 *
 * Copyright (C) 2021 ALIF SEMICONDUCTOR
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "tgu_M55_HP.h"
/*
 * Array of NS memory regions. Add new regions to this list.
 */
static const struct mem_region ns_regions[] = {
    {NS_REGION_0_BASE, NS_REGION_0_END, DTCM},
    };

/*----------------------------------------------------------------------------
  TGU setup function. Marks single memory region starting at NS_REGION_1_BASE
  and ending at NS_REGION_1_END as Non Secure in the TGU look up tables. The
  region should be in the DTCM area. Can be extended to include the ITCM region
  and multiple discontigous regions if needed.
 *----------------------------------------------------------------------------*/
void TGU_Setup(void)
{
    uint32_t start_block, end_block, start_lut, end_lut;
    uint32_t start_offset, end_offset, lut_val_l, lut_val_h;
    uint32_t base, itcm_blksize, dtcm_blksize, blksize;
    int32_t i;

    /* Find out the TGU block size for ITCM */
    blksize = *((volatile unsigned int *) ITGU_CFG) & ITGU_CFG_BLKSZ;
    /* ITCM blksize is 2^(blksize + 5)*/
    itcm_blksize = 1 << (blksize + 5);

    /* Find out the TGU block size for DTCM */
    blksize = *((volatile unsigned int *) DTGU_CFG) & DTGU_CFG_BLKSZ;
    /* DTCM block size is 2^(blksize + 5) */
    dtcm_blksize = 1 << (blksize + 5);

    for (i = 0; i < (sizeof(ns_regions) / sizeof(ns_regions[0])); i++) {
        if (ns_regions[i].type == DTCM) {
            start_block =  (ns_regions[i].start - DTCM_BASE) / dtcm_blksize;
            end_block = (ns_regions[i].end - DTCM_BASE) / dtcm_blksize;
            base = DTGU_BASE;
        } else {
            start_block =  (ns_regions[i].start - ITCM_BASE) / itcm_blksize;
            end_block = (ns_regions[i].end - ITCM_BASE) / itcm_blksize;
            base = ITGU_BASE;
        }

        start_lut = start_block / 32;
        end_lut = end_block / 32;

        start_offset = start_block % 32;
        end_offset = end_block % 32;

        if (start_lut == end_lut) {
            /* same LUT register */
            lut_val_l = SET_BIT_RANGE(start_offset, end_offset);
            *((volatile uint32_t *) TGU_LUT(base, start_lut)) |= lut_val_l;
        } else {
            /* the range spans multiple LUT registers */
            lut_val_l = SET_BIT_RANGE(start_offset, 31);
            lut_val_h = SET_BIT_RANGE(0, end_offset);

            /* Write into the first LUT register */
            *((volatile uint32_t *) TGU_LUT(base, start_lut)) = lut_val_l;

            /* Now write to all the intermediate LUT registers */
            while (start_lut != (end_lut - 1)) {
                start_lut++;
                *((volatile uint32_t *) TGU_LUT(base, start_lut)) = 0xFFFFFFFFU;
            }

            /* Write into the last LUT register*/
            *((volatile uint32_t *) TGU_LUT(base, end_lut)) |= lut_val_h;
        }
    }
}
