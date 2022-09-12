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

// <e> SILICON_REV_A  (Silicon Revison)
// <i> Select if the Chip Silicon Rev is Ax
#define RTE_SILICON_REV_A 	1
#if RTE_SILICON_REV_A
#define RTE_SILICON_REV_B0	0
#else
#define RTE_SILICON_REV_B0	1
#endif
// </e> SILICON_REV_A  (Silicon Revison)

// <e> FLASH_MRAM (Flash MRAM) [Driver_FLASH_MRAM]
// <i> Configuration settings for Driver_FLASH_MRAM in component ::Drivers:FLASH_MRAM
#define RTE_FLASH_MRAM          1
#if RTE_FLASH_MRAM
#define RTE_FLASH_MRAM_SIZE     0x00580000
#endif
// </e> FLASH_MRAM (Flash MRAM) [Driver_FLASH_MRAM]

// <e> CAMERA0 (Camera) [Driver_CAMERA0]
// <i> Configuration settings for Driver_CAMERA0 in component ::Drivers:CAMERA
#define RTE_CAMERA0   1
#if RTE_CAMERA0
#define RTE_CAMERA0_IRQ_PRI                    0
#define RTE_ARX3A0_CAMERA_SENSOR_ENABLE        1

#if (RTE_ARX3A0_CAMERA_SENSOR_ENABLE)
#define RTE_ARX3A0_CAMERA_SENSOR_INTERFACE_MIPI_ENABLE        1

#if RTE_ARX3A0_CAMERA_SENSOR_INTERFACE_MIPI_ENABLE
#define RTE_ARX3A0_CAMERA_CLOCK_SOURCE                  0
#define RTE_ARX3A0_CAMERA_SENSOR_PIXEL_CLK_POL          0
#define RTE_ARX3A0_CAMERA_SENSOR_CFG_FPS                5
#define RTE_ARX3A0_CAMERA_SENSOR_FREQ                   400000000
#define RTE_ARX3A0_CAMERA_SENSOR_CLK_SCR_DIV            0x14
#define RTE_ARX3A0_CAMERA_SENSOR_HSYNC_POL              0
#define RTE_ARX3A0_CAMERA_SENSOR_VSYNC_POL              0
#define RTE_ARX3A0_CAMERA_SENSOR_HSYNC_MODE             0
#define RTE_ARX3A0_CAMERA_SENSOR_DATA_MODE              0
#define RTE_ARX3A0_CAMERA_SENSOR_DATA_MASK              0
#define RTE_ARX3A0_CAMERA_RESET_PIN_NO                  5
#define RTE_ARX3A0_CAMERA_RESET_GPIO_PORT               4
#endif

#define RTE_ARX3A0_CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE          0
#endif

#define RTE_MT9M114_CAMERA_SENSOR_ENABLE       0

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

// <e> MIPI_CSI2 (mipi csi2) [Driver_MIPI_CSI2]
// <i> Configuration settings for Driver_MIPI_CSI2 in component ::Drivers:MIPI_CSI2
#define RTE_MIPI_CSI2 1
#if RTE_MIPI_CSI2
#define RTE_MIPI_CSI2_NON_CONTINUOUS_CLOCK_MODE 1
#define RTE_MIPI_CSI2_PIXCLK_DIV                0x2
#define RTE_MIPI_CSI2_N_LANES                   02
#define RTE_MIPI_CSI2_VC_ID                     0
#define RTE_MIPI_CSI2_DATA_TYPE                 15
#define RTE_MIPI_CSI2_IPI_MODE                  0
#define RTE_MIPI_CSI2_COLOR_COP                 1
#define RTE_MIPI_CSI2_MEMFLUSH                  0
#define RTE_MIPI_CSI2_IPI_HSA_TIME              0x2
#define RTE_MIPI_CSI2_IPI_HBP_TIME              0x0
#define RTE_MIPI_CSI2_IPI_HSD_TIME              0x118
#define RTE_MIPI_CSI2_IPI_HACTIVE_TIME          0x230
#define RTE_MIPI_CSI2_IPI_VSA_LINE              0x4
#define RTE_MIPI_CSI2_IPI_VBP_LINE              0x4
#define RTE_MIPI_CSI2_IPI_VFP_LINE              0x4
#define RTE_MIPI_CSI2_IPI_VACTIVE_LINE          0x230
#define RTE_MIPI_CSI2_IRQ_PRI                   0
#endif
// </e> MIPI_CSI2 (mipi csi2) [Driver_MIPI_CSI2]

// <e> MIPI_DSI (mipi dsi) [Driver_MIPI_DSI]
// <i> Configuration settings for Driver_MIPI_DSI in component ::Drivers:MIPI_DSI
#define RTE_MIPI_DSI 0

#if RTE_MIPI_DSI
#define RTE_MIPI_DSI_FREQUENCY                  125000000
#define RTE_MIPI_DSI_N_LANES                    0x2
#define RTE_MIPI_DSI_VC_ID                      0
#define RTE_MIPI_DSI_NON_CONTINUOUS_CLOCK_MODE  0x1
#define RTE_MIPI_DSI_PLL_INPUT_DIV_FACTOR_N     0x3
#define RTE_MIPI_DSI_PHY_CLKHS2LP_TIME          0x42
#define RTE_MIPI_DSI_PHY_CLKLP2HS_TIME          0xB5
#define RTE_MIPI_DSI_PHY_HS2LP_TIME             0x32
#define RTE_MIPI_DSI_PHY_LP2HS_TIME             0x99
#define RTE_MIPI_DSI_TX_ESC_CLK_DIVISION        0x2
#define RTE_MIPI_DSI_VID_MODE_TYPE              0x2
#define RTE_MIPI_DSI_VID_NUM_CHUNKS             0
#define RTE_MIPI_DSI_VID_NLL_SIZE               0
#define RTE_MIPI_DSI_DPI_COLOR_CODE             0x5
#define RTE_MIPI_DSI_DPI_DATAEN_ACTIVE_LOW      0
#define RTE_MIPI_DSI_DPI_VSYNC_ACTIVE_LOW       0x1
#define RTE_MIPI_DSI_DPI_HSYNC_ACTIVE_LOW       0x1
#define RTE_MIPI_DSI_DPI_SHUTD_ACTIVE_LOW       0
#define RTE_MIPI_DSI_DPI_COLORM_ACTIVE_LOW      0
#define RTE_MIPI_DSI_DPI_HSA_TIME               0x40
#define RTE_MIPI_DSI_DPI_HBP_TIME               0x5E
#define RTE_MIPI_DSI_DPI_HFP_TIME               0x41A
#define RTE_MIPI_DSI_DPI_HACTIVE_TIME           0x1E0
#define RTE_MIPI_DSI_DPI_VSA_LINE               0xA
#define RTE_MIPI_DSI_DPI_VBP_LINE               0x14
#define RTE_MIPI_DSI_DPI_VFP_LINE               0x14
#define RTE_MIPI_DSI_DPI_VACTIVE_LINE           0x320
#define RTE_MIPI_DSI_IRQ_PRI                    0

#define RTE_MIPI_DSI_ILI9806E_PANEL  1

#if RTE_MIPI_DSI_ILI9806E_PANEL
#define RTE_ILI9806E_PANEL_RESET_PIN_NO                  6
#define RTE_ILI9806E_PANEL_RESET_GPIO_PORT               4
#define RTE_ILI9806E_PANEL_BL_LED_PIN_NO                 4
#define RTE_ILI9806E_PANEL_BL_LED_GPIO_PORT              4
#define RTE_ILI9806E_PANEL_BPP                           24
#endif

#endif

// </e> MIPI_DSI (mipi dsi) [Driver_MIPI_DSI]

// <e> CDC200 (cdc200) [Driver_CDC200]
// <i> Configuration settings for Driver_CDC200 in component ::Drivers:CDC200
#define RTE_CDC200 1

#if RTE_CDC200
#define RTE_CDC200_PIXCLK_DIV                0x8
#define RTE_CDC200_PIXEL_FORMAT              0x1
#define RTE_CDC200_DPI_HSYNC_TIME            0x40
#define RTE_CDC200_DPI_HBP_TIME              0x5E
#define RTE_CDC200_DPI_HFP_TIME              0x48B
#define RTE_CDC200_DPI_HACTIVE_TIME          0x1E0
#define RTE_CDC200_DPI_VSYNC_LINE            0xA
#define RTE_CDC200_DPI_VBP_LINE              0x14
#define RTE_CDC200_DPI_VFP_LINE              0x14
#define RTE_CDC200_DPI_VACTIVE_LINE          0x320

#endif
// </e> CDC200 (cdc200) [Driver_CDC200]

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
#define RTE_I2S0_RX_TRIG_LVL 7
#define RTE_I2S0_TX_TRIG_LVL 8
#define RTE_I2S0_IRQ_PRI     0
#define RTE_I2S0_CLK_SOURCE  1
#define RTE_I2S0_DMA_ENABLE  1
#endif
// </e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]

// <e> I2S1 (Integrated Interchip Sound 1) [Driver_SAI1]
// <i> Configuration settings for Driver_SAI1 in component ::Drivers:SAI
#define RTE_I2S1 0
#ifdef RTE_I2S1
#define RTE_I2S1_WSS_CLOCK_CYCLES 2
#define RTE_I2S1_SCLKG_CLOCK_CYCLES 0
#define RTE_I2S1_RX_TRIG_LVL 7
#define RTE_I2S1_TX_TRIG_LVL 8
#define RTE_I2S1_IRQ_PRI     0
#define RTE_I2S1_CLK_SOURCE  1
#define RTE_I2S1_DMA_ENABLE  1
#endif
// </e> I2S1 (Integrated Interchip Sound 1) [Driver_SAI1]

// <e> I2S2 (Integrated Interchip Sound 2) [Driver_SAI2]
// <i> Configuration settings for Driver_SAI2 in component ::Drivers:SAI
#define RTE_I2S2 1
#ifdef RTE_I2S2
#define RTE_I2S2_WSS_CLOCK_CYCLES 2
#define RTE_I2S2_SCLKG_CLOCK_CYCLES 0
#define RTE_I2S2_RX_TRIG_LVL 7
#define RTE_I2S2_TX_TRIG_LVL 8
#define RTE_I2S2_IRQ_PRI     10
#define RTE_I2S2_CLK_SOURCE  1
#define RTE_I2S2_DMA_ENABLE  1
#endif
// </e> I2S2 (Integrated Interchip Sound 2) [Driver_SAI2]

// <e> I2S3 (Integrated Interchip Sound 3) [Driver_SAI3]
// <i> Configuration settings for Driver_SAI3 in component ::Drivers:SAI
#define RTE_I2S3 0
#ifdef RTE_I2S3
#define RTE_I2S3_WSS_CLOCK_CYCLES 2
#define RTE_I2S3_SCLKG_CLOCK_CYCLES 0
#define RTE_I2S3_RX_TRIG_LVL 7
#define RTE_I2S3_TX_TRIG_LVL 8
#define RTE_I2S3_IRQ_PRI     0
#define RTE_I2S3_CLK_SOURCE  1
#define RTE_I2S3_DMA_ENABLE  1
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
#define RTE_UART2_TX_TRIG_LVL   0
#define RTE_UART2_IRQ_PRI       0
#define RTE_UART2_CLK_SOURCE    1
#endif
// </e> UART2 (Universal asynchronous receiver transmitter) [Driver_USART2]


// <e> UART3 (Universal asynchronous receiver transmitter) [Driver_USART3]
// <i> Configuration settings for Driver_USART3 in component ::Drivers:USART
#define RTE_UART3   1
#if RTE_UART3
#define RTE_UART3_RX_TRIG_LVL   0
#define RTE_UART3_TX_TRIG_LVL   0
#define RTE_UART3_IRQ_PRI       0
#define RTE_UART3_CLK_SOURCE    1
#endif
// </e> UART3 (Universal asynchronous receiver transmitter) [Driver_USART3]


// <e> UART4 (Universal asynchronous receiver transmitter) [Driver_USART4]
// <i> Configuration settings for Driver_USART4 in component ::Drivers:USART
#define RTE_UART4   1
#if RTE_UART4
#define RTE_UART4_RX_TRIG_LVL   0
#define RTE_UART4_TX_TRIG_LVL   0
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
#define RTE_UART5_RX_TRIG_LVL   0
#define RTE_UART5_TX_TRIG_LVL   0
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
// <i> Configuration settings for Driver_GPIO4 in component ::Drivers:GPIO
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
#define RTE_LPTIMER			    1
#if RTE_LPTIMER

#define RTE_LPTIMER_CHANNEL0_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL0_PWM            (0)
#define RTE_LPTIMER_CHANNEL0_FREE_RUN_MODE  (0)
#define RTE_LPTIMER_CHANNEL0_CLK_SRC        (0)

#define RTE_LPTIMER_CHANNEL1_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL1_PWM            (0)
#define RTE_LPTIMER_CHANNEL1_FREE_RUN_MODE  (0)
#define RTE_LPTIMER_CHANNEL1_CLK_SRC        (0)

#define RTE_LPTIMER_CHANNEL2_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL2_PWM            (0)
#define RTE_LPTIMER_CHANNEL2_FREE_RUN_MODE  (0)
#define RTE_LPTIMER_CHANNEL2_CLK_SRC        (0)

#define RTE_LPTIMER_CHANNEL3_IRQ_PRIORITY   (0)
#define RTE_LPTIMER_CHANNEL3_PWM            (0)
#define RTE_LPTIMER_CHANNEL3_FREE_RUN_MODE  (0)
#define RTE_LPTIMER_CHANNEL3_CLK_SRC        (0)
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
#define RTE_UTIMER_CHANNEL0_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL0_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL0_DRIVER_A                0
#define RTE_UTIMER_CHANNEL0_DRIVER_B                0
#define RTE_UTIMER_CHANNEL0_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL0_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL0_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL0_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL0_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL0_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL0_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL0_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL0_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL0_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL0_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL0_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL1_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL1_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL1_DRIVER_A                0
#define RTE_UTIMER_CHANNEL1_DRIVER_B                0
#define RTE_UTIMER_CHANNEL1_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL1_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL1_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL1_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL1_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL1_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL1_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL1_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL1_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL1_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL1_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL1_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL2_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL2_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL2_DRIVER_A                0
#define RTE_UTIMER_CHANNEL2_DRIVER_B                0
#define RTE_UTIMER_CHANNEL2_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL2_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL2_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL2_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL2_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL2_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL2_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL2_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL2_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL2_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL2_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL2_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL3_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL3_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL3_DRIVER_A                0
#define RTE_UTIMER_CHANNEL3_DRIVER_B                0
#define RTE_UTIMER_CHANNEL3_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL3_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL3_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL3_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL3_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL3_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL3_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL3_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL3_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL3_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL3_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL3_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL4_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL4_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL4_DRIVER_A                1
#define RTE_UTIMER_CHANNEL4_DRIVER_B                0
#define RTE_UTIMER_CHANNEL4_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL4_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL4_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL4_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL4_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL4_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL4_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL4_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL4_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL4_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL4_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL4_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL5_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL5_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL5_DRIVER_A                1
#define RTE_UTIMER_CHANNEL5_DRIVER_B                0
#define RTE_UTIMER_CHANNEL5_CMP_START_STATE         1
#define RTE_UTIMER_CHANNEL5_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL5_DRV_OP_AT_MATCH_COUNT   3
#define RTE_UTIMER_CHANNEL5_DRV_OP_AT_CYCLE_END     3
#define RTE_UTIMER_CHANNEL5_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL5_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL5_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL5_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL5_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL5_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL5_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL5_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL6_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL6_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL6_DRIVER_A                0
#define RTE_UTIMER_CHANNEL6_DRIVER_B                0
#define RTE_UTIMER_CHANNEL6_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL6_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL6_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL6_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL6_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL6_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL6_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL6_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL6_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL7_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL7_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL7_DRIVER_A                0
#define RTE_UTIMER_CHANNEL7_DRIVER_B                0
#define RTE_UTIMER_CHANNEL7_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL7_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL7_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL7_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL7_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL7_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL7_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL7_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL7_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL7_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL7_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL7_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL8_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL8_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL8_DRIVER_A                0
#define RTE_UTIMER_CHANNEL8_DRIVER_B                0
#define RTE_UTIMER_CHANNEL8_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL8_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL8_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL8_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL8_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL8_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL8_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL8_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL8_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL8_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL8_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL8_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL9_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL9_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL9_DRIVER_A                0
#define RTE_UTIMER_CHANNEL9_DRIVER_B                0
#define RTE_UTIMER_CHANNEL9_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL9_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL9_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL9_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL9_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL9_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL9_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL9_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL9_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL9_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL9_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL9_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL10_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL10_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL10_DRIVER_A                0
#define RTE_UTIMER_CHANNEL10_DRIVER_B                0
#define RTE_UTIMER_CHANNEL10_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL10_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL10_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL10_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL10_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL10_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL10_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL10_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL10_BUFFERING_TYPE_A        2
#define RTE_UTIMER_CHANNEL10_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL10_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL10_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL11_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL11_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL11_DRIVER_A                0
#define RTE_UTIMER_CHANNEL11_DRIVER_B                0
#define RTE_UTIMER_CHANNEL11_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL11_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL11_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL11_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL11_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL11_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL11_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL11_BUFFER_OPERATION        1
#define RTE_UTIMER_CHANNEL11_BUFFERING_TYPE_A        2
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

// <e> IPC_MODEM_SS (InterProcessor Communication Module for Modem Subsystem) [Driver_IPC_MODEM_SS]
// <i> Configuration settings for Driver_IPC_MODEM_SS in component ::Drivers:IPC_MODEM_SS
#define RTE_IPC_INTGEN_BLOCK0 1
#define RTE_IPC_MDM_ATCMD_IRQ_PRI 0
#define RTE_IPC_INTGEN_BLOCK1 1
#define RTE_IPC_MDM_DATA_IRQ_PRI 0
#define RTE_IPC_INTGEN_BLOCK4 1
#define RTE_IPC_GNSS_DATA_IRQ_PRI 0
#define RTE_IPC_INTGEN_BLOCK5 1
#define RTE_IPC_GNSS_ATCMD_IRQ_PRI 0

// </e> IPC_MODEM_SS (Interprocessor Communication Module for Modem Subsystem) [Driver_IPC_MODEM_SS]

// </e> Analog configuration [vbat analog register2 and comparator register2]
#define RTE_ANALOG_CONFIG          1
#if RTE_ANALOG_CONFIG
#define RTE_VBAT_ANA_REG2_VAL      (0x388C4230)
#define RTE_COMP_REG2_VAL          (0x10200000 | 0x14240100)
#endif
// </e> Analog configuration [vbat analog register2 and comparator register2]

// <e> DAC0 (Digital to analog converter ) [Driver_DAC0]
// <i> Configuration settings for Driver_DAC0 in component ::Drivers:DAC
#define RTE_DAC0                   1
#if RTE_DAC0
#define RTE_DAC0_INPUT_BYP_MUX_EN  0
#define RTE_DAC0_BYP_VAL           (0x1FF)
#define RTE_DAC0_CAP_CONT          (0X07)
#define RTE_DAC0_RES_CONT          (0X0C)
#define RTE_DAC0_TWOSCOMP_EN        0
#define RTE_DAC0_IBIAS             (0X0C)
#endif
// </e> DAC0 (Digital to analog converter) [Driver_DAC0]

// <e> DAC1 (Digital to analog converter ) [Driver_DAC1]
// <i> Configuration settings for Driver_DAC1 in component ::Drivers:DAC
#define RTE_DAC1                   1
#if RTE_DAC1
#define RTE_DAC1_INPUT_BYP_MUX_EN  0
#define RTE_DAC1_BYP_VAL           (0x1FF)
#define RTE_DAC1_CAP_CONT          (0X07)
#define RTE_DAC1_RES_CONT          (0X0C)
#define RTE_DAC1_TWOSCOMP_EN        0
#define RTE_DAC1_IBIAS             (0X0C)
#endif
// </e> DAC1 (Digital to Analog converter) [Driver_DAC1]

// <e> ADC0 (Analog to Digital Converter) [Driver_ADC0]
// <i> Configuration settings for Driver_ADC0 in component ::Drivers:ADC
#define RTE_ADC0     1
#if RTE_ADC0
#define RTE_ADC0_IRQ_PRIORITY        0

#define RTE_ADC0_INPUT_NUM               (0)
#define RTE_ADC0_CLOCK_DIV               (2)
#define RTE_ADC0_SAMPLE_WIDTH            (16)
#define RTE_ADC0_AVG_SAMPLE_NUM          (256)
#define RTE_ADC0_SHIFT_N_BIT             (8)
#define RTE_ADC0_SHIFT_LEFT_OR_RIGHT     (1)

#define RTE_ADC0_TEST_EN                  0
#define RTE_ADC0_DIFFERENTIAL_EN          0
#define RTE_ADC0_COMPARATOR_EN            1
#define RTE_ADC0_COMPARATOR_BIAS          2
#define RTE_ADC0_VCM_RDIV_EN              1

#endif
// </e> ADC0 (Analog to Digital Converter) [Driver_ADC0]

// <e> ADC1 (Analog to Digital Converter) [Driver_ADC1]
// <i> Configuration settings for Driver_ADC1 in component ::Drivers:ADC
#define RTE_ADC1   1
#if RTE_ADC1

#define RTE_ADC1_INPUT_NUM               (0)
#define RTE_ADC1_CLOCK_DIV               (2)
#define RTE_ADC1_SAMPLE_WIDTH            (16)
#define RTE_ADC1_AVG_SAMPLE_NUM          (256)
#define RTE_ADC1_SHIFT_N_BIT             (8)
#define RTE_ADC1_SHIFT_LEFT_OR_RIGHT     (1)

#define RTE_ADC1_TEST_EN                  0
#define RTE_ADC1_DIFFERENTIAL_EN          0
#define RTE_ADC1_COMPARATOR_EN            1
#define RTE_ADC1_COMPARATOR_BIAS          2
#define RTE_ADC1_VCM_RDIV_EN              1

#endif
// </e> ADC1 (Analog to Digital Converter) [Driver_ADC1]

// <e> ADC2 (Analog to Digital Converter) [Driver_ADC2]
// <i> Configuration settings for Driver_ADC2 in component ::Drivers:ADC
#define RTE_ADC2   1
#if RTE_ADC2

#define RTE_ADC2_INPUT_NUM               (0)
#define RTE_ADC2_CLOCK_DIV               (2)
#define RTE_ADC2_SAMPLE_WIDTH            (16)
#define RTE_ADC2_AVG_SAMPLE_NUM          (256)
#define RTE_ADC2_SHIFT_N_BIT             (8)
#define RTE_ADC2_SHIFT_LEFT_OR_RIGHT     (1)

#define RTE_ADC2_TEST_EN                  0
#define RTE_ADC2_DIFFERENTIAL_EN          0
#define RTE_ADC2_COMPARATOR_EN            1
#define RTE_ADC2_COMPARATOR_BIAS          2
#define RTE_ADC2_VCM_RDIV_EN              1

#endif
// <e> ADC2 (Analog to Digital Converter) [Driver_ADC2]

// </e> ADC common bits for each instance
// <i> Configuration settings for ADC instances ::Drivers:ADC
#if (RTE_ADC0 | RTE_ADC1 | RTE_ADC2)
#define RTE_ADC_CONFG_RESERVED_bits_18_23           7
#define RTE_ADC_CONFG_amux_cont                     0
#endif
// </e> ADC commmon bit for each instance

// <e> CRC0 (Cyclic Redundancy Check) [Driver_CRC0]
// <i> Configuration settings for Driver_CRC0 in component ::Drivers:CRC
#define RTE_CRC0      1
// </e> CRC0 (Cyclic Redundancy Check) [Driver_CRC0]

// <e> CRC1 (Cyclic Redundancy Check) [Driver_CRC1]
// <i> Configuration settings for Driver_CRC1 in component ::Drivers:CRC
#define RTE_CRC1      1
// </e> CRC1 (Cyclic Redundancy Check) [Driver_CRC1]

// </e> HWSEM0 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM0 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM0                                     1
#if RTE_HWSEM0
#define RTE_HWSEM0_IRQPRIORITY                         0
#endif

// </e> HWSEM1 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM1 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM1                                     0
#if RTE_HWSEM1
#define RTE_HWSEM1_IRQPRIORITY                         0
#endif

// </e> HWSEM2 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM2 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM2                                     0
#if RTE_HWSEM2
#define RTE_HWSEM2_IRQPRIORITY                         0
#endif

// </e> HWSEM3 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM3 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM3                                     0
#if RTE_HWSEM3
#define RTE_HWSEM3_IRQPRIORITY                         0
#endif

// </e> HWSEM4 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM4 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM4                                     0
#if RTE_HWSEM4
#define RTE_HWSEM4_IRQPRIORITY                         0
#endif

// </e> HWSEM5 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM5 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM5                                     0
#if RTE_HWSEM5
#define RTE_HWSEM5_IRQPRIORITY                         0
#endif

// </e> HWSEM6 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM6 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM6                                     0
#if RTE_HWSEM6
#define RTE_HWSEM6_IRQPRIORITY                         0
#endif

// </e> HWSEM7 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM77 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM7                                     0
#if RTE_HWSEM7
#define RTE_HWSEM7_IRQPRIORITY                         0
#endif

// </e> HWSEM8 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM8 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM8                                     0
#if RTE_HWSEM8
#define RTE_HWSEM8_IRQPRIORITY                         0
#endif

// </e> HWSEM9 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM9 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM9                                     0
#if RTE_HWSEM9
#define RTE_HWSEM9_IRQPRIORITY                         0
#endif

// </e> HWSEM10 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM10 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM10                                     0
#if RTE_HWSEM10
#define RTE_HWSEM10_IRQPRIORITY                         0
#endif

// </e> HWSEM11 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM11 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM11                                     0
#if RTE_HWSEM11
#define RTE_HWSEM11_IRQPRIORITY                         0
#endif

// </e> HWSEM12 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM12 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM12                                     0
#if RTE_HWSEM12
#define RTE_HWSEM12_IRQPRIORITY                         0
#endif

// </e> HWSEM13 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM13 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM13                                     0
#if RTE_HWSEM13
#define RTE_HWSEM13_IRQPRIORITY                         0
#endif

// </e> HWSEM14 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM14 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM14                                     0
#if RTE_HWSEM14
#define RTE_HWSEM14_IRQPRIORITY                         0
#endif

// </e> HWSEM15 (Hardware Semaphore) [Driver_HWSEM]


// <e> HWSEM15 (Hardware Semaphore) [Driver_HWSEM]
// <i> Configuration settings for Driver_HWSEM in component ::Drivers:HWSEM
#define RTE_HWSEM15                                     0
#if RTE_HWSEM15
#define RTE_HWSEM15_IRQPRIORITY                         0
#endif

// <e> CMP0 (Analog Comparator ) [Driver_CMP0]
// <i> Configuration settings for Driver_CMP0 in component ::Drivers:Comparator
#define RTE_CMP0    1
#if RTE_CMP0

#define RTE_CMP0_IRQ_PRIORITY        0

#define RTE_CMP0_POSITIVE_PIN_PO_00  0
#define RTE_CMP0_POSITIVE_PIN_PO_06  1
#define RTE_CMP0_POSITIVE_PIN_PO_12  2
#define RTE_CMP0_POSITIVE_PIN_PO_04  3

#define RTE_CMP0_7MV_HYSTERISIS      7
#define RTE_CMP0_0MV_HYSTERISIS      0

#define RTE_CMP0_SEL_POSITIVE        RTE_CMP0_POSITIVE_PIN_PO_00
#define RTE_CMP0_SEL_NEGATIVE        RTE_CMP_NEGATIVE_PO_16
#define RTE_CMP0_SEL_HYSTERISIS      RTE_CMP0_7MV_HYSTERISIS

#define RTE_CMP0_PIN_P0_17           0xB2000000
#define RTE_CMP0_OUTPUT_MUX_SEL      RTE_CMP0_PIN_P0_17

#endif
// </e> CMP0 (Analog Comparator) [Driver_CMP0]

// <e> CMP1 (Analog Comparator ) [Driver_CMP1]
// <i> Configuration settings for Driver_CMP1 in component ::Drivers:Comparator
#define RTE_CMP1    1
#if RTE_CMP1

#define RTE_CMP1_IRQ_PRIORITY        0

#define RTE_CMP1_POSITIVE_PIN_PO_01  0
#define RTE_CMP1_POSITIVE_PIN_PO_07  1
#define RTE_CMP1_POSITIVE_PIN_PO_13  2
#define RTE_CMP1_POSITIVE_PIN_PO_05  3

#define RTE_CMP1_7MV_HYSTERISIS      7
#define RTE_CMP1_0MV_HYSTERISIS      0

#define RTE_CMP1_SEL_POSITIVE        RTE_CMP1_POSITIVE_PIN_PO_01
#define RTE_CMP1_SEL_NEGATIVE        RTE_CMP_NEGATIVE_PO_16
#define RTE_CMP1_SEL_HYSTERISIS      RTE_CMP0_7MV_HYSTERISIS

#define RTE_CMP1_PIN_P0_17           0xB3000000
#define RTE_CMP1_OUTPUT_MUX_SEL      RTE_CMP1_PIN_P0_17

#endif
// </e> CMP1 (Analog Comparator) [Driver_CMP1]

// <e> CMP2 (Analog Comparator ) [Driver_CMP2]
// <i> Configuration settings for Driver_CMP2 in component ::Drivers:Comparator
#define RTE_CMP2    1
#if RTE_CMP2

#define RTE_CMP2_IRQ_PRIORITY        0

#define RTE_CMP2_POSITIVE_PIN_PO_02  0
#define RTE_CMP2_POSITIVE_PIN_PO_08  1
#define RTE_CMP2_POSITIVE_PIN_PO_14  2
#define RTE_CMP2_POSITIVE_PIN_PO_10  3

#define RTE_CMP2_7MV_HYSTERISIS      7
#define RTE_CMP2_0MV_HYSTERISIS      0

#define RTE_CMP2_SEL_POSITIVE        RTE_CMP2_POSITIVE_PIN_PO_02
#define RTE_CMP2_SEL_NEGATIVE        RTE_CMP_NEGATIVE_PO_16
#define RTE_CMP2_SEL_HYSTERISIS      RTE_CMP2_7MV_HYSTERISIS

#define RTE_CMP2_OUTPUT_PIN_P0_17    0xB4000000
#define RTE_CMP2_OUTPUT_MUX_SEL      RTE_CMP2_OUTPUT_PIN_P0_17

#endif
// </e> CMP2 (Analog Comparator) [Driver_CMP2]

// <e> CMP3 (Analog Comparator ) [Driver_CMP3]
// <i> Configuration settings for Driver_CMP3 in component ::Drivers:Comparator
#define RTE_CMP3    1
#if RTE_CMP3

#define RTE_CMP3_IRQ_PRIORITY        0

#define RTE_CMP3_POSITIVE_PIN_PO_03  0
#define RTE_CMP3_POSITIVE_PIN_PO_09  1
#define RTE_CMP3_POSITIVE_PIN_PO_15  2
#define RTE_CMP3_POSITIVE_PIN_PO_11  3

#define RTE_CMP3_7MV_HYSTERISIS      7
#define RTE_CMP3_0MV_HYSTERISIS      0

#define RTE_CMP3_SEL_POSITIVE        RTE_CMP3_POSITIVE_PIN_PO_03
#define RTE_CMP3_SEL_NEGATIVE        RTE_CMP_NEGATIVE_PO_16
#define RTE_CMP3_SEL_HYSTERISIS      RTE_CMP3_7MV_HYSTERISIS


#define RTE_CMP3_PIN_P0_17           0xB5000000
#define RTE_CMP3_OUTPUT_MUX_SEL      RTE_CMP3_PIN_P0_17

#endif
// </e> CMP3 (Analog Comparator) [Driver_CMP0]

// </e> Comparator common bits for each instance
// <i> Configuration settings for Comparator instances ::Drivers:Comparator
#if (RTE_CMP0 |RTE_CMP1 |RTE_CMP2 |RTE_CMP3 )

#define RTE_CMP_NEGATIVE_PO_16       0
#define RTE_CMP_NEGATIVE_PO_17       1
#define RTE_CMP_NEGATIVE_INT_VREF    2
#define RTE_CMP_NEGATIVE_DAC6        3

#endif
// </e> Comparator commmon bit for each instance

// <e> DMA0 (Direct Memory Access Controller) [Driver_DMA0]
// <i> Configuration settings for Driver_DMA0 in component ::Drivers:DMA
#define RTE_DMA0      1
#if RTE_DMA0
//   DMA APB Interface to be used <0-1>
//   0 - Secure APB interface, 1 - Non-Secure
//   Default: 0
#define RTE_DMA0_APB_INTERFACE 0
#endif
// </e> DMA0 (Direct Memory Access Controller) [Driver_DMA0]

// <e> DMA2 (Direct Memory Access Controller) [Driver_DMA2]
// <i> Configuration settings for Driver_DMA2 in component ::Drivers:DMA
#define RTE_DMA2      1
#if RTE_DMA2
//   DMA APB Interface to be used <0-1>
//   0 - Secure APB interface, 1 - Non-Secure
//   Default: 0
#define RTE_DMA2_APB_INTERFACE 0
#endif
// </e> DMA2 (Direct Memory Access Controller) [Driver_DMA2]


#endif  /* __RTE_DEVICE_H */
