/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/**************************************************************************//**
 * @file     mpu_M55_HP.c
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

#if defined (M55_HP)
  #include "M55_HP.h"
#else
  #error device not specified!
#endif


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
    static const ARM_MPU_Region_t mpu_table[] __STARTUP_RO_DATA_ATTRIBUTE = {
    {
    .RBAR = ARM_MPU_RBAR(0x80000000UL, ARM_MPU_SH_NON, 1UL, 1UL, 0UL),  // RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x8057FFFFUL, 1UL)     // MRAM
    },
    {
    .RBAR = ARM_MPU_RBAR(0x02000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL),	// RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x023FFFFFUL, 2UL)     // SRAM0
    },
    {
    .RBAR = ARM_MPU_RBAR(0x08000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL),	// RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x0827FFFFUL, 2UL)     // SRAM1
    },
    {
    .RBAR = ARM_MPU_RBAR(0x70000000UL, ARM_MPU_SH_NON, 0UL, 1UL, 1UL),
    .RLAR = ARM_MPU_RLAR(0x71FFFFFFUL, 0UL)     // LP- Peripheral & PINMUX Regions */
    },
    {
    .RBAR = ARM_MPU_RBAR(0x60000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 0UL),	// RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x60FFFFFFUL, 1UL)     // HE TCM (SRAM4 + SRAM5)
    },
    {
    .RBAR = ARM_MPU_RBAR(0x62000000UL, ARM_MPU_SH_OUTER, 0UL, 1UL, 0UL),	// RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x621FFFFFUL, 1UL)     // SRAM6
    },
    {
    .RBAR = ARM_MPU_RBAR(0x63100000UL, ARM_MPU_SH_NON, 0UL, 1UL, 0UL),	// RO, NP, XN
    .RLAR = ARM_MPU_RLAR(0x632FFFFFUL, 1UL)     // SRAM8
    },
    };

    /* Define the possible Attribute regions */
    ARM_MPU_SetMemAttr(0UL, ARM_MPU_ATTR(   /* Attr0, Device Memory */
                            ARM_MPU_ATTR_DEVICE,
                            ARM_MPU_ATTR_DEVICE_nGnRE));
    ARM_MPU_SetMemAttr(1UL, ARM_MPU_ATTR(	/* Attr1, Normal Memory, Write-Back, Read-Write-Allocate */
                            ARM_MPU_ATTR_MEMORY_(1,1,1,1),
                            ARM_MPU_ATTR_MEMORY_(1,1,1,1)));
    ARM_MPU_SetMemAttr(2UL, ARM_MPU_ATTR(   /* Attr2, Normal Memory, Transient, Write Through, Read Allocate */
                            ARM_MPU_ATTR_MEMORY_(0,0,1,0),
                            ARM_MPU_ATTR_MEMORY_(0,0,1,0)));

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

