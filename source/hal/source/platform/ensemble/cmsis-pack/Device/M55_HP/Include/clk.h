/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement 
 *
 * You should have received a copy of the Alif Semiconductor Software 
 * License Agreement with this file. If not, please write to: 
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef CLK_H_
#define CLK_H_

#ifndef AXI_CLOCK
#define AXI_CLOCK 400000000
#endif

#ifndef AHB_CLOCK
#define AHB_CLOCK 200000000
#endif

#ifndef APB_CLOCK
#define APB_CLOCK 100000000
#endif

#define I2C_PERIPHERAL_CLOCK    APB_CLOCK

#define CDC200_PIXCLK           AXI_CLOCK

#endif /* CLK_H_ */

