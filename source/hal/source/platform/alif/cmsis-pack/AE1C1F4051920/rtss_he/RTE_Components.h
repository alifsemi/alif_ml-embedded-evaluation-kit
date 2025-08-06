/* Copyright (C) 2025 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H

#define CMSIS_device_header "alif.h"
#include CMSIS_device_header

#define RTE_Drivers_IO

#define RTE_Drivers_PINCONF

#define RTE_Drivers_USART

#define RTE_Drivers_SAI
#define RTE_Drivers_SPI
#define RTE_Drivers_PDM

#define RTE_Drivers_ISSI_FLASH
#define RTE_Drivers_OSPI

#endif /* RTE_COMPONENTS_H */
