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
 * @file     mpu_M55_HP.h
 * @author   Sudhir Sreedharan
 * @emial    sudhir@alifsemi.com
 * @version  V1.0.0
 * @date     14-December-2021
 * @brief    MPU header file
 ******************************************************************************/

/* Include Guard */
#ifndef MPU_M55_HP_H
#define MPU_M55_HP_H

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* Public function prototypes ------------------------------------------------*/
/**
 * @brief  Configure the MPU.
 * @note   This function disables the MPU and loads the regions
 *         from the table. Once it is loaded, MPU is enabled.
 * @param  None
 * @retval None
 */
void MPU_Setup(void);


#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* End Include Guard */
/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
