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
 * @file     Camera_Common.h
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     06-Sep-2021
 * @brief    Common Camera definitions.
 ******************************************************************************/

#ifndef CAMERA_COMMON_H_
#define CAMERA_COMMON_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/**
\brief Camera Resolution
*/
typedef enum _ARM_CAMERA_RESOLUTION {
  CAMERA_RESOLUTION_VGA_640x480,         /* VGA Resolution 480P */
  CAMERA_RESOLUTION_560x560,             /* Resolution 560x560 */
  CAMERA_RESOLUTION_480x480,             /* Resolution 480x480 */
} ARM_CAMERA_RESOLUTION;

#ifdef  __cplusplus
}
#endif

#endif /* CAMERA_COMMON_H_ */

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
