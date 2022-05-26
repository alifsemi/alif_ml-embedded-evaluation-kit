/* -----------------------------------------------------------------------------
 * Copyright (c) 2021 Alif Semiconductor Inc.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * $Date:        2. Feburary 2021
 * $Revision:    V1.0.0
 * Author:       Sudhir Sreedharan
 *
 * Project:      RTE Device Configuration for ALIF M55_HE
 * -------------------------------------------------------------------------- */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H

// <e> CAMERA0 (Camera) [Driver_CAMERA0]
// <i> Configuration settings for Driver_CAMERA0 in component ::Drivers:CAMERA
#define RTE_CAMERA0   0
#if RTE_CAMERA0
#define RTE_CAMERA0_IRQ_PRI                    0
#define RTE_MT9M114_CAMERA_SENSOR_ENABLE       1

#if RTE_MT9M114_CAMERA_SENSOR_ENABLE
#define RTE_MT9M114_CAMERA_SENSOR_INTERFACE_PARALLEL_ENABLE        1

#if RTE_MT9M114_CAMERA_SENSOR_INTERFACE_PARALLEL_ENABLE
#define RTE_MT9M114_CAMERA_CLOCK_SOURCE                  0
#define RTE_MT9M114_CAMERA_SENSOR_PIXEL_CLK_POL          1
#define RTE_MT9M114_CAMERA_SENSOR_HSYNC_POL              0
#define RTE_MT9M114_CAMERA_SENSOR_VSYNC_POL              0
#define RTE_MT9M114_CAMERA_SENSOR_HSYNC_MODE             1
#define RTE_MT9M114_CAMERA_SENSOR_DATA_MODE              0
#define RTE_MT9M114_CAMERA_SENSOR_DATA_MASK              0
#endif

#define RTE_MT9M114_CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE          0
#endif

#endif
// </e> CAMERA0 (Camera) [Driver_CAMERA0]

// <e> I3C0 (Improved Inter-Integrated Circuit) [Driver_I3C0]
// <i> Configuration settings for Driver_I3C0 in component ::Drivers:I3C
#define RTE_I3C0   1
#if RTE_I3C0
#define RTE_I3C0_IRQ_PRI       0
#endif
// </e> I3C0 (Improved Inter-Integrated Circuit) [Driver_I3C0]

// <e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]
// <i> Configuration settings for Driver_SAI0 in component ::Drivers:SAI
#define RTE_I2S0 1
#ifdef RTE_I2S0
#define RTE_I2S0_WSS_CLOCK_CYCLES 2
#define RTE_I2S0_SCLKG_CLOCK_CYCLES 0
#define RTE_I2S0_RX_TRIG_LVL 8 
#define RTE_I2S0_TX_TRIG_LVL 10
#define RTE_I2S0_IRQ_PRI     0
#define RTE_I2S0_CLK_SOURCE  1
#endif
// </e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]

// <e> I2S1 (Integrated Interchip Sound 1) [Driver_SAI1]
// <i> Configuration settings for Driver_SAI1 in component ::Drivers:SAI
#define RTE_I2S1 0
#ifdef RTE_I2S1
#define RTE_I2S1_WSS_CLOCK_CYCLES 2
#define RTE_I2S1_SCLKG_CLOCK_CYCLES 0
#define RTE_I2S1_RX_TRIG_LVL 8
#define RTE_I2S1_TX_TRIG_LVL 10
#define RTE_I2S1_IRQ_PRI     0
#define RTE_I2S1_CLK_SOURCE  1
#endif
// </e> I2S1 (Integrated Interchip Sound 1) [Driver_SAI1]

// <e> I2S2 (Integrated Interchip Sound 2) [Driver_SAI2]
// <i> Configuration settings for Driver_SAI2 in component ::Drivers:SAI
#define RTE_I2S2 1
#ifdef RTE_I2S2
#define RTE_I2S2_WSS_CLOCK_CYCLES 2
#define RTE_I2S2_SCLKG_CLOCK_CYCLES 0
#define RTE_I2S2_RX_TRIG_LVL 8
#define RTE_I2S2_TX_TRIG_LVL 10
#define RTE_I2S2_IRQ_PRI     10
#define RTE_I2S2_CLK_SOURCE  1
#endif
// </e> I2S2 (Integrated Interchip Sound 2) [Driver_SAI2]

// <e> I2S3 (Integrated Interchip Sound 3) [Driver_SAI3]
// <i> Configuration settings for Driver_SAI3 in component ::Drivers:SAI
#define RTE_I2S3 0
#ifdef RTE_I2S3
#define RTE_I2S3_WSS_CLOCK_CYCLES 2
#define RTE_I2S3_SCLKG_CLOCK_CYCLES 0
#define RTE_I2S3_RX_TRIG_LVL 8
#define RTE_I2S3_TX_TRIG_LVL 10
#define RTE_I2S3_IRQ_PRI     0
#define RTE_I2S3_CLK_SOURCE  1
#endif
// </e> I2S3 (Integrated Interchip Sound 3) [Driver_SAI3]


// <e> UART0 (Universal asynchronous receiver transmitter) [Driver_USART0]
// <i> Configuration settings for Driver_USART0 in component ::Drivers:USART
#define RTE_UART0   1
#if RTE_UART0
#define RTE_UART0_RX_TRIG_LVL   0
#define RTE_UART0_TX_TRIG_LVL   0
#define RTE_UART0_IRQ_PRI       0
#define RTE_UART0_CLK_SOURCE    1
#endif
// </e> UART0 (Universal asynchronous receiver transmitter) [Driver_USART0]


// <e> UART1 (Universal asynchronous receiver transmitter) [Driver_USART1]
// <i> Configuration settings for Driver_USART1 in component ::Drivers:USART
#define RTE_UART1   1
#if RTE_UART1
#define RTE_UART1_RX_TRIG_LVL   0
#define RTE_UART1_TX_TRIG_LVL   0
#define RTE_UART1_IRQ_PRI       0
#define RTE_UART1_CLK_SOURCE    1
#endif
// </e> UART1 (Universal asynchronous receiver transmitter) [Driver_USART1]


// <e> UART2 (Universal asynchronous receiver transmitter) [Driver_USART2]
// <i> Configuration settings for Driver_USART2 in component ::Drivers:USART
#define RTE_UART2   1
#if RTE_UART2
#define RTE_UART2_RX_TRIG_LVL   0
#define RTE_UART2_TX_TRIG_LVL   3
#define RTE_UART2_IRQ_PRI       0
#define RTE_UART2_CLK_SOURCE    1
#endif
// </e> UART2 (Universal asynchronous receiver transmitter) [Driver_USART2]


// <e> UART3 (Universal asynchronous receiver transmitter) [Driver_USART3]
// <i> Configuration settings for Driver_USART3 in component ::Drivers:USART
#define RTE_UART3   1
#if RTE_UART3
#define RTE_UART3_RX_TRIG_LVL   2
#define RTE_UART3_TX_TRIG_LVL   3
#define RTE_UART3_IRQ_PRI       0
#define RTE_UART3_CLK_SOURCE    1
#endif
// </e> UART3 (Universal asynchronous receiver transmitter) [Driver_USART3]


// <e> UART4 (Universal asynchronous receiver transmitter) [Driver_USART4]
// <i> Configuration settings for Driver_USART4 in component ::Drivers:USART
#define RTE_UART4   1
#if RTE_UART4
#define RTE_UART4_RX_TRIG_LVL   0
#define RTE_UART4_TX_TRIG_LVL   3
#define RTE_UART4_IRQ_PRI       0
#define RTE_UART4_CLK_SOURCE    1

#define RTE_UART4_RS485_ENABLE                              0
#if RTE_UART4_RS485_ENABLE
#define RTE_UART4_RS485_TRANSFER_MODE                       2
#define RTE_UART4_RS485_DE_ASSERTION_TIME_8BIT              (0x01)
#define RTE_UART4_RS485_DE_DEASSERTION_TIME_8BIT            (0x01)
#define RTE_UART4_RS485_DE_TO_RE_TURN_AROUND_TIME_16BIT     (0x01)
#define RTE_UART4_RS485_RE_TO_DE_TURN_AROUND_TIME_16BIT     (0x03)
#endif
#endif
// </e> UART4 (Universal asynchronous receiver transmitter) [Driver_USART4]


// <e> UART5 (Universal asynchronous receiver transmitter) [Driver_USART5]
// <i> Configuration settings for Driver_USART5 in component ::Drivers:USART
#define RTE_UART5   1
#if RTE_UART5
#define RTE_UART5_RX_TRIG_LVL   2
#define RTE_UART5_TX_TRIG_LVL   3
#define RTE_UART5_IRQ_PRI       0
#define RTE_UART5_CLK_SOURCE    1

#define RTE_UART5_RS485_ENABLE                              0
#if RTE_UART5_RS485_ENABLE
#define RTE_UART5_RS485_TRANSFER_MODE                       2
#define RTE_UART5_RS485_DE_ASSERTION_TIME_8BIT              (0x01)
#define RTE_UART5_RS485_DE_DEASSERTION_TIME_8BIT            (0x01)
#define RTE_UART5_RS485_DE_TO_RE_TURN_AROUND_TIME_16BIT     (0x01)
#define RTE_UART5_RS485_RE_TO_DE_TURN_AROUND_TIME_16BIT     (0x03)
#endif
#endif
// </e> UART5 (Universal asynchronous receiver transmitter) [Driver_USART5]


// <e> UART6 (Universal asynchronous receiver transmitter) [Driver_USART6]
// <i> Configuration settings for Driver_USART6 in component ::Drivers:USART
#define RTE_UART6   1
#if RTE_UART6
#define RTE_UART6_RX_TRIG_LVL   0
#define RTE_UART6_TX_TRIG_LVL   0
#define RTE_UART6_IRQ_PRI       0
#define RTE_UART6_CLK_SOURCE    1

#define RTE_UART6_RS485_ENABLE                              0
#if RTE_UART6_RS485_ENABLE
#define RTE_UART6_RS485_TRANSFER_MODE                       2
#define RTE_UART6_RS485_DE_ASSERTION_TIME_8BIT              (0x01)
#define RTE_UART6_RS485_DE_DEASSERTION_TIME_8BIT            (0x01)
#define RTE_UART6_RS485_DE_TO_RE_TURN_AROUND_TIME_16BIT     (0x01)
#define RTE_UART6_RS485_RE_TO_DE_TURN_AROUND_TIME_16BIT     (0x03)
#endif
#endif
// </e> UART6 (Universal asynchronous receiver transmitter) [Driver_USART6]


// <e> UART7 (Universal asynchronous receiver transmitter) [Driver_USART7]
// <i> Configuration settings for Driver_USART7 in component ::Drivers:USART
#define RTE_UART7   1
#if RTE_UART7
#define RTE_UART7_RX_TRIG_LVL   0
#define RTE_UART7_TX_TRIG_LVL   0
#define RTE_UART7_IRQ_PRI       0
#define RTE_UART7_CLK_SOURCE    1

#define RTE_UART7_RS485_ENABLE                              0
#if RTE_UART7_RS485_ENABLE
#define RTE_UART7_RS485_TRANSFER_MODE                       2
#define RTE_UART7_RS485_DE_ASSERTION_TIME_8BIT              (0x01)
#define RTE_UART7_RS485_DE_DEASSERTION_TIME_8BIT            (0x01)
#define RTE_UART7_RS485_DE_TO_RE_TURN_AROUND_TIME_16BIT     (0x01)
#define RTE_UART7_RS485_RE_TO_DE_TURN_AROUND_TIME_16BIT     (0x03)
#endif
#endif
// </e> UART7 (Universal asynchronous receiver transmitter) [Driver_USART7]

// <e> SPI0 (Serial Peripheral Interface 0) [Driver_SPI]
// <i> Configuration settings for Driver_SPI in component ::Drivers:SPI
#define RTE_SPI0 1
#ifdef RTE_SPI0
#define RTE_SPI0_IRQ_PRIORITY                   0
#define RTE_SPI0_SPI_FRAME_FORMAT               0
#define RTE_SPI0_TX_FIFO_LEVEL_TO_START_TRANSFER 0
#define RTE_SPI0_TX_LOAD_DUMMY_TO_START_LEVEL   0
#define RTE_SPI0_TX_FIFO_THRESHOLD              0
#define RTE_SPI0_RX_FIFO_THRESHOLD              0
#define RTE_SPI0_CHIP_SELECTION_PIN             0
#endif
// </e> SPI0 (Serial Peripheral Interface 0) [Driver_SPI]

// <e> SPI1 (Serial Peripheral Interface 1) [Driver_SPI]
// <i> Configuration settings for Driver_SPI in component ::Drivers:SPI
#define RTE_SPI1 1
#ifdef RTE_SPI1
#define RTE_SPI1_IRQ_PRIORITY                   0
#define RTE_SPI1_SPI_FRAME_FORMAT               0
#define RTE_SPI1_TX_FIFO_LEVEL_TO_START_TRANSFER 0
#define RTE_SPI1_TX_LOAD_DUMMY_TO_START_LEVEL   0
#define RTE_SPI1_TX_FIFO_THRESHOLD              0
#define RTE_SPI1_RX_FIFO_THRESHOLD              0
#define RTE_SPI1_CHIP_SELECTION_PIN             0
#endif
// </e> SPI1 (Serial Peripheral Interface 1) [Driver_SPI]

// <e> SPI2 (Serial Peripheral Interface 2) [Driver_SPI]
// <i> Configuration settings for Driver_SPI in component ::Drivers:SPI
#define RTE_SPI2 1
#ifdef RTE_SPI2
#define RTE_SPI2_IRQ_PRIORITY                   0
#define RTE_SPI2_SPI_FRAME_FORMAT               0
#define RTE_SPI2_TX_FIFO_LEVEL_TO_START_TRANSFER 0
#define RTE_SPI2_TX_LOAD_DUMMY_TO_START_LEVEL   0
#define RTE_SPI2_TX_FIFO_THRESHOLD              0
#define RTE_SPI2_RX_FIFO_THRESHOLD              0
#define RTE_SPI2_CHIP_SELECTION_PIN             0
#endif
// </e> SPI2 (Serial Peripheral Interface 2) [Driver_SPI]

// <e> SPI3 (Serial Peripheral Interface 3) [Driver_SPI]
// <i> Configuration settings for Driver_SPI in component ::Drivers:SPI
#define RTE_SPI3 1
#ifdef RTE_SPI3
#define RTE_SPI3_IRQ_PRIORITY                   0
#define RTE_SPI3_SPI_FRAME_FORMAT               0
#define RTE_SPI3_TX_FIFO_LEVEL_TO_START_TRANSFER 0
#define RTE_SPI3_TX_LOAD_DUMMY_TO_START_LEVEL   0
#define RTE_SPI3_TX_FIFO_THRESHOLD              0
#define RTE_SPI3_RX_FIFO_THRESHOLD              0
#define RTE_SPI3_CHIP_SELECTION_PIN             1
#endif
// </e> SPI3 (Serial Peripheral Interface 3) [Driver_SPI]


// <e> GPIO1 (General purpose input or output) [Driver_GPIO1]
// <i> Configuration settings for Driver_GPIO1 in component ::Drivers:GPIO
#define RTE_GPIO1   1
#if RTE_GPIO1
#define RTE_GPIO1_PIN0_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN1_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN2_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN3_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN4_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN5_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN6_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN7_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN8_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN9_IRQ_PRIORITY     (0)
#define RTE_GPIO1_PIN10_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN11_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN12_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN13_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN14_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN15_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN16_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN17_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN18_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN19_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN20_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN21_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN22_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN23_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN24_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN25_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN26_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN27_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN28_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN29_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN30_IRQ_PRIORITY    (0)
#define RTE_GPIO1_PIN31_IRQ_PRIORITY    (0)
#endif /*RTE_GPIO1 */
// </e> GPIO1 (General purpose input or output) [Driver_GPIO1]

// <e> GPIO2 (General purpose input or output) [Driver_GPIO2]
// <i> Configuration settings for Driver_GPIO2 in component ::Drivers:GPIO
#define RTE_GPIO2   1
#if RTE_GPIO2
#define RTE_GPIO2_PIN0_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN1_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN2_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN3_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN4_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN5_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN6_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN7_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN8_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN9_IRQ_PRIORITY     (0)
#define RTE_GPIO2_PIN10_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN11_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN12_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN13_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN14_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN15_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN16_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN17_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN18_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN19_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN20_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN21_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN22_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN23_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN24_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN25_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN26_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN27_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN28_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN29_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN30_IRQ_PRIORITY    (0)
#define RTE_GPIO2_PIN31_IRQ_PRIORITY    (0)
#endif /*RTE_GPIO2 */
// </e> GPIO2 (General purpose input or output) [Driver_GPIO2]

// <e> GPIO3 (General purpose input or output) [Driver_GPIO3]
// <i> Configuration settings for Driver_GPIO3 in component ::Drivers:GPIO
#define RTE_GPIO3   1
#if RTE_GPIO3
#define RTE_GPIO3_PIN0_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN1_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN2_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN3_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN4_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN5_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN6_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN7_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN8_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN9_IRQ_PRIORITY     (0)
#define RTE_GPIO3_PIN10_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN11_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN12_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN13_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN14_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN15_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN16_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN17_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN18_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN19_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN20_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN21_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN22_IRQ_PRIORITY    (0)
#define RTE_GPIO3_PIN23_IRQ_PRIORITY    (0)
#endif /* RTE_GPIO3 */
// </e> GPIO3 (General purpose input or output) [Driver_GPIO3]

// <e> GPIO4 (General purpose input or output) [Driver_GPIO4]
// <i> Configuration settings for Driver_GPIO3 in component ::Drivers:GPIO
#define RTE_GPIO4   1
#if RTE_GPIO4
#define RTE_GPIO4_PIN0_IRQ_PRIORITY     (0)
#define RTE_GPIO4_PIN1_IRQ_PRIORITY     (0)
#define RTE_GPIO4_PIN2_IRQ_PRIORITY     (0)
#define RTE_GPIO4_PIN3_IRQ_PRIORITY     (0)
#define RTE_GPIO4_PIN4_IRQ_PRIORITY     (0)
#define RTE_GPIO4_PIN5_IRQ_PRIORITY     (0)
#define RTE_GPIO4_PIN6_IRQ_PRIORITY     (0)
#define RTE_GPIO4_PIN7_IRQ_PRIORITY     (0)
#endif /* RTE_GPIO4 */
// </e> GPIO4 (General purpose input or output) [Driver_GPIO4]

// <e> LPTIMER (Low power timer) [Driver_LPTIMER]
// <i> Configuration settings for Driver_LPTIMER in component ::Drivers:LPTIMER
#define RTE_LPTIMER             1
#if RTE_LPTIMER

#define RTE_LPTIMER_CHANNEL0_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL0_PWM            (0)
#define RTE_LPTIMER_CHANNEL0_FREE_RUN_MODE  (0)

#define RTE_LPTIMER_CHANNEL1_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL1_PWM            (0)
#define RTE_LPTIMER_CHANNEL1_FREE_RUN_MODE  (0)

#define RTE_LPTIMER_CHANNEL2_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL2_PWM            (0)
#define RTE_LPTIMER_CHANNEL2_FREE_RUN_MODE  (0)

#define RTE_LPTIMER_CHANNEL3_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL3_PWM            (0)
#define RTE_LPTIMER_CHANNEL3_FREE_RUN_MODE  (0)

#define RTE_LPTIMER_CHANNEL4_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL4_PWM            (0)
#define RTE_LPTIMER_CHANNEL4_FREE_RUN_MODE  (0)

#define RTE_LPTIMER_CHANNEL5_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL5_PWM            (0)
#define RTE_LPTIMER_CHANNEL5_FREE_RUN_MODE  (0)

#define RTE_LPTIMER_CHANNEL6_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL6_PWM            (0)
#define RTE_LPTIMER_CHANNEL6_FREE_RUN_MODE  (0)

#define RTE_LPTIMER_CHANNEL7_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL7_PWM            (0)
#define RTE_LPTIMER_CHANNEL7_FREE_RUN_MODE  (0)
#endif /* RTE_LPTIMER */
// </e> LPTIMER (Low power timer) [Driver_LPTIMER]

// <e> RTC0 (Real time clock) [Driver_RTC0]
// <i> Configuration settings for Driver_RTC0 in component ::Drivers:RTC
#define RTE_RTC0                1
#if RTE_RTC0
#define RTE_RTC0_DEFAULT_PRESCALER_VALUE    32768
#define RTE_RTC0_IRQ_PRI                    0
#endif
// </e> RTC0 (Real time clock) [Driver_RTC0]

// <e> WDT0 (Watchdog Timer) [Driver_WDT0]
// <i> Configuration settings for Driver_WDT0 in component ::Drivers:WDT
#define RTE_WDT0                1
// </e> WDT0 (Watchdog Timer) [Driver_WDT0]

// <e> UTIMER (Universal Timer) [Driver_UTIMER]
// <i> Configuration settings for Driver_UTIMER in component ::Drivers:UTIMER
#define RTE_UTIMER   1
#if RTE_UTIMER
#define RTE_UTIMER_CHANNEL0_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL0_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL0_DRIVER_A                0
#define RTE_UTIMER_CHANNEL0_DRIVER_B                0
#define RTE_UTIMER_CHANNEL0_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL0_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL0_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL0_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL0_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL0_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL0_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL0_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL1_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL1_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL1_DRIVER_A                0
#define RTE_UTIMER_CHANNEL1_DRIVER_B                0
#define RTE_UTIMER_CHANNEL1_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL1_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL1_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL1_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL1_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL1_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL1_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL1_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL2_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL2_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL2_DRIVER_A                0
#define RTE_UTIMER_CHANNEL2_DRIVER_B                0
#define RTE_UTIMER_CHANNEL2_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL2_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL2_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL2_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL2_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL2_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL2_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL2_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL3_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL3_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL3_DRIVER_A                0
#define RTE_UTIMER_CHANNEL3_DRIVER_B                0
#define RTE_UTIMER_CHANNEL3_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL3_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL3_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL3_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL3_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL3_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL3_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL3_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL4_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL4_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL4_DRIVER_A                0
#define RTE_UTIMER_CHANNEL4_DRIVER_B                0
#define RTE_UTIMER_CHANNEL4_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL4_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL4_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL4_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL4_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL4_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL4_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL4_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL5_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL5_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL5_DRIVER_A                0
#define RTE_UTIMER_CHANNEL5_DRIVER_B                0
#define RTE_UTIMER_CHANNEL5_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL5_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL5_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL5_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL5_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL5_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL5_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL5_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL6_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL6_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL6_DRIVER_A                0
#define RTE_UTIMER_CHANNEL6_DRIVER_B                0
#define RTE_UTIMER_CHANNEL6_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL6_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL6_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL6_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL6_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL7_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL7_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL7_DRIVER_A                0
#define RTE_UTIMER_CHANNEL7_DRIVER_B                0
#define RTE_UTIMER_CHANNEL7_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL7_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL7_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL7_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL7_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL7_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL7_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL7_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL8_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL8_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL8_DRIVER_A                0
#define RTE_UTIMER_CHANNEL8_DRIVER_B                0
#define RTE_UTIMER_CHANNEL8_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL8_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL8_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL8_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL8_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL8_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL8_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL8_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL9_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL9_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL9_DRIVER_A                0
#define RTE_UTIMER_CHANNEL9_DRIVER_B                0
#define RTE_UTIMER_CHANNEL9_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL9_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL9_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL9_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL9_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL9_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL9_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL9_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL10_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL10_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL10_DRIVER_A                0
#define RTE_UTIMER_CHANNEL10_DRIVER_B                0
#define RTE_UTIMER_CHANNEL10_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL10_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL10_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL10_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL10_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL10_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL10_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL10_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL11_SINGLE_CYCLE_ENABLE     0
#define RTE_UTIMER_CHANNEL11_BUF_TROUGH_N_CREST      0
#define RTE_UTIMER_CHANNEL11_DRIVER_A                0
#define RTE_UTIMER_CHANNEL11_DRIVER_B                0
#define RTE_UTIMER_CHANNEL11_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL11_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL11_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL11_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL11_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL11_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL11_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL11_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL11_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL11_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL11_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL11_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL11_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL11_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL11_UNDER_FLOW_IRQ_PRIORITY 0
#endif /*RTE_UTIMER*/
// </e> UTIMER (Universal timer) [Driver_UTIMER]

#endif  /* __RTE_DEVICE_H */
