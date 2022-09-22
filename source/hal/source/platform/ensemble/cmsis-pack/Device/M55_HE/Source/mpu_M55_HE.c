/* Copyright (c) 2021 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

/**************************************************************************//**
 * @file     mpu_M55_HE.c
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     14-December-2021
 * @brief    MPU Configuration Source
 *
 *           This file load the MPU regions from the table and provides the function to enable it.
 * @bug      None.
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/

#if defined (M55_HE)
  #include "M55_HE.h"
#else
  #error device not specified!
#endif

const ARM_MPU_Region_t mpu_table[] __attribute__((section("startup_ro_data"))) = {
    {
    .RBAR = ARM_MPU_RBAR(0x02000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL),	// RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x023FFFFFUL, 1UL)     // SRAM0
    },
    {
    .RBAR = ARM_MPU_RBAR(0x08000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL),	// RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x0827FFFFUL, 1UL)     // SRAM1
    },
    {
    .RBAR = ARM_MPU_RBAR(0x70000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 1UL),
    .RLAR = ARM_MPU_RLAR(0x71FFFFFFUL, 0UL)     // LP- Peripheral & PINMUX Regions */
    },
    {
    .RBAR = ARM_MPU_RBAR(0x62000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL),	// RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x621FFFFFUL, 1UL)     // SRAM6
    },
    {
    .RBAR = ARM_MPU_RBAR(0x63100000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL),	// RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x632FFFFFUL, 1UL)     // SRAM8
    },
};

/* Public functions ----------------------------------------------------------*/
/**
 * @brief  Load the MPU regions from the given table
 * @note   This function loads the region and also sets the
 *         attrbutes for the regions.
 * @param  None
 * @retval None
 */
static void MPU_Load_Regions(void)
{

    /* Define the possible Attribute regions */
    ARM_MPU_SetMemAttr(0UL, ARM_MPU_ATTR_DEVICE);	/* Attr0, Device Memory */
    ARM_MPU_SetMemAttr(1UL, ARM_MPU_ATTR(	/* Attr1, Normal Memory, Cached, Write-through */
                            ARM_MPU_ATTR_MEMORY_(1,0,1,1),
                            ARM_MPU_ATTR_MEMORY_(1,0,1,1)));

    /* Load the regions from the table */
    ARM_MPU_Load(0U, &mpu_table[0], sizeof(mpu_table)/sizeof(ARM_MPU_Region_t));
}

/**
 * @brief  Clear all the MPU registers
 * @note   This function disables the MPU and clear
 *         all the existing regions.
 * @param  None
 * @retval None
 */
static void MPU_Clear_All_Regions(void)
{
    MPU_Type* mpu = MPU;
    /* Retrieve the number of regions */
    uint32_t num_regions = (mpu->TYPE >> 8);
    uint32_t cnt;

    ARM_MPU_Disable();

    for(cnt = 0U; cnt < num_regions; cnt++)
            ARM_MPU_ClrRegion(cnt);
}

/**
 * @brief  Configure the MPU.
 * @note   This function disables the MPU and loads the regions
 *         from the table. Once it is loaded, MPU is enabled.
 * @param  None
 * @retval None
 */
void MPU_Setup(void)
{
#define MPU_CONTROL (MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk)

    MPU_Clear_All_Regions();
    MPU_Load_Regions();
    ARM_MPU_Enable(MPU_CONTROL);
}

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
