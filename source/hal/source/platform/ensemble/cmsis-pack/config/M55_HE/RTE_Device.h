/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H

#include "board.h"

// <e> SILICON_REV_A  (Silicon Revison)
// <i> Select if the Chip Silicon Rev is Ax
#define RTE_SILICON_REV_A	1
#if RTE_SILICON_REV_A
#define RTE_SILICON_REV_A0      0
#define RTE_SILICON_REV_A1      1
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
#define RTE_ARX3A0_CAMERA_SENSOR_CFG_FPS                40
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

// <o> select clock mode
//     <1=> non continuous clock mode
//     <0=> continuous clock mode
// <i> defines clock mode for mipi csi2
// <i> default: non continuous clock mode
#define RTE_MIPI_CSI2_NON_CONTINUOUS_CLOCK_MODE 1

// <o> select CSI2 pixel clock divider
// <i> defines select CSI2 pixel clock divider value.
// <i> default: 2
#define RTE_MIPI_CSI2_PIXCLK_DIV                2

// <o> select number of lanes in DPHY
//     <1=> one lane
//     <2=> two lane
// <i> defines select number of lanes in DPHY.
// <i> default: two lane
#define RTE_MIPI_CSI2_N_LANES                   2

// <o> select number of virtual channel ID
//     <0=> one virtual channel
// <i> defines select number of virtual channel IDs.
// <i> default: one virtual channel
#define RTE_MIPI_CSI2_VC_ID                     0

// <o> select CSI2 Data type
//     <0=> YUV420_8
//     <1=> YUV420_10
//     <2=> YUV420_8_LEGACY
//     <3=> YUV420_8_SHIFT
//     <4=> YUV420_10_SHIFT
//     <5=> YUV422_8
//     <6=> YUV422_10
//     <7=> RGB444
//     <8=> RGB555
//     <9=> RGB565
//     <10=> RGB666
//     <11=> RGB888
//     <12=> RAW6
//     <13=> RAW7
//     <14=> RAW8
//     <15=> RAW10
//     <16=> RAW12
//     <17=> RAW14
//     <18=> RAW16
//     <19=> USER_DEFINED_1
//     <20=> USER_DEFINED_2
//     <21=> USER_DEFINED_3
//     <22=> USER_DEFINED_4
//     <23=> USER_DEFINED_5
//     <24=> USER_DEFINED_6
//     <25=> USER_DEFINED_7
//     <26=> USER_DEFINED_8
// <i> defines select CSI2 Data type
// <i> default: RAW10
#define RTE_MIPI_CSI2_DATA_TYPE                 15

// <o> select IPI mode
//     <0=> camera mode
//     <1=> controller mode
// <i> defines select IPI mode
// <i> default: camera mode
#define RTE_MIPI_CSI2_IPI_MODE                  0

// <o> select color component
//     <0=> 48 bit interface
//     <1=> 16 bit interface
// <i> defines select color component
// <i> default: 16 bit interface
#define RTE_MIPI_CSI2_COLOR_COP                 1

// <o> select memory flush
//     <0=> auto
//     <1=> manual
// <i> defines select memory flush
// <i> default: auto
#define RTE_MIPI_CSI2_MEMFLUSH                  0

// <o> select sync event mode
//     <0=> not trigger by frame start
//     <1=> trigger by frame start
// <i> defines select sync event mode
// <i> default : not trigger by frame start
#define RTE_MIPI_CSI2_SYNC_ET_MODE              0

// <o> select sync event select
//     <0=> auto
//     <1=> programmed
// <i> defines select sync event select
// <i> default : programmed
#define RTE_MIPI_CSI2_SYNC_ET_SEL               1

// <o> embedded packets for IPI synchronization events
//     <0=> disable
//     <1=> enable
// <i> define if want to use embedded packets for IPI synchronization events
// <i> default: disable
#define RTE_MIPI_CSI2_EN_EMBEDDED               0

// <o> blanking packets for IPI synchronization events
//     <0=> disable
//     <1=> enable
// <i> define if want to use blanking packets for IPI synchronization events
// <i> default: disable
#define RTE_MIPI_CSI2_EN_BLANKING               0

// <o> null packets for IPI synchronization events
//     <0=> disable
//     <1=> enable
// <i> define if want to use null packets for IPI synchronization events
// <i> default: disable
#define RTE_MIPI_CSI2_EN_NULL                   0

// <o> line start packets for IPI synchronization events
//     <0=> disable
//     <1=> enable
// <i> define if want to use line start packets for IPI synchronization events
// <i> default: disable
#define RTE_MIPI_CSI2_EN_LINE_START             0

// <o> video packets for IPI synchronization events
//     <0=> disable
//     <1=> enable
// <i> define if want to use video packets for IPI synchronization events
// <i> default: enable
#define RTE_MIPI_CSI2_EN_VIDEO                  1

// <o> datatype to overwrite
//     <0=> disable
//     <1=> enable
// <i> define if want to overwrite datatype
// <i> default: disable
#define RTE_MIPI_CSI2_EN_DT                     0

// <o> datatype to overwrite with programmed datatype
// <i> define if want to use programmed datatype ignoring datatype of the header
// <i> default: 0
#define RTE_MIPI_CSI2_EN_DT_OVERWRITE           0

// <o> Horizontal Synchronism Active Time range <0-2047>
// <i> Defines possible range for selecting horizontal sync active time
// <i> Default: 0
#define RTE_MIPI_CSI2_IPI_HSA_TIME              0

// <o> Horizontal Synchronism back porch Time range <0-2047>
// <i> Defines possible range for selecting horizontal sync back porch time
// <i> Default: 0
#define RTE_MIPI_CSI2_IPI_HBP_TIME              0

// <o> Horizontal sync delay Time range <0-2047>
// <i> Defines possible range for selecting horizontal sync delay time
// <i> Default: 560
#define RTE_MIPI_CSI2_IPI_HSD_TIME              560

// <o> Horizontal Active Time range <0-2047>
// <i> Defines possible range for selecting horizontal active time
// <i> Default: 560
#define RTE_MIPI_CSI2_IPI_HACTIVE_TIME          560

// <o> Vertical sync active period range <0-511>
// <i> Defines possible range for selecting vertical sync active period
// <i> Default: 0
#define RTE_MIPI_CSI2_IPI_VSA_LINE              0

// <o> Vertical back porch period range <0-511>
// <i> Defines possible range for selecting vertical back porch period
// <i> Default: 0
#define RTE_MIPI_CSI2_IPI_VBP_LINE              0

// <o> Vertical front porch period range <0-511>
// <i> Defines possible range for selecting vertical front porch period
// <i> Default: 0
#define RTE_MIPI_CSI2_IPI_VFP_LINE              0

// <o> Vertical active period range <0-8191>
// <i> Defines possible range for selecting vertical active period
// <i> Default: 560
#define RTE_MIPI_CSI2_IPI_VACTIVE_LINE          560

// <o> CSI2 interrupt priority.
// <i> Defines CSI2 interrupt priority.
// <i> Default: 0
#define RTE_MIPI_CSI2_IRQ_PRI                   0
#endif
// </e> MIPI_CSI2 (mipi csi2) [Driver_MIPI_CSI2]

// <e> MIPI_DSI (mipi dsi) [Driver_MIPI_DSI]
// <i> Configuration settings for Driver_MIPI_DSI in component ::Drivers:MIPI_DSI
#define RTE_MIPI_DSI 1

#if RTE_MIPI_DSI

// <o> Number of data lanes
//     <1=> ONE
//     <2=> TWO
// <i> Defines Number of data lanes
// <i> Default: TWO
#define RTE_MIPI_DSI_N_LANES                    0x2

// <o> Virtual channel ID
// <i> Defines Virtual Channel ID
// <i> Default: 0
#define RTE_MIPI_DSI_VC_ID                      0

// <o> DPHY Clock Mode
//     <0=> CONTINUOUS CLOCK_MODE
//     <1=> NON CONTINUOUS CLOCK MODE
// <i> Defines DPHY Clock Mode
// <i> Default: NON CONTINUOUS CLOCK MODE
#define RTE_MIPI_DSI_NON_CONTINUOUS_CLOCK_MODE  1

// <o> DPHY PLL input division factor
// <i> Defines DPHY PLL input division factor
// <i> Default: 3
#define RTE_MIPI_DSI_PLL_INPUT_DIV_FACTOR_N     3

// <o> DPHY clock lane HS to LP transition time.
// <i> Defines DPHY clock lane HS to LP transition time.
// <i> Default: 35
#define RTE_MIPI_DSI_PHY_CLKHS2LP_TIME          35

// <o> DPHY clock lane LP to HS transition time.
// <i> Defines DPHY clock lane LP to HS transition time.
// <i> Default: 51
#define RTE_MIPI_DSI_PHY_CLKLP2HS_TIME          51

// <o> DPHY data lane HS to LP transition time.
// <i> Defines DPHY data lane HS to LP transition time.
// <i> Default: 20
#define RTE_MIPI_DSI_PHY_HS2LP_TIME             20

// <o> DPHY data lane LP to HS transition time.
// <i> Defines DPHY data lane LP to HS transition time.
// <i> Default: 40
#define RTE_MIPI_DSI_PHY_LP2HS_TIME             40

// <o> DSI TX escape clock division value.
// <i> Defines DSI TX escape clock division value.
// <i> Default: 2
#define RTE_MIPI_DSI_TX_ESC_CLK_DIVISION        2

// <o> DPI Video Mode
//     <0=> NON_BURST SYNC_PULSES
//     <1=> NON_BURST SYNC_EVENTS
//     <2=> BURST MODE
// <i> Defines DPI Video Mode
// <i> Default:BURST MODE
#define RTE_MIPI_DSI_VID_MODE_TYPE              2

// <o> DSI Video number of Chunks
// <i> Defines Number of chunks used to transfer single video packet .
// <i> Default: 0
#define RTE_MIPI_DSI_VID_NUM_CHUNKS             0

// <o> DSI Size of NULL packet
// <i> Defines Size of the NULL packet.
// <i> Default: 0
#define RTE_MIPI_DSI_VID_NULL_SIZE              0

// <o> DPI interface color code
//     <1=> 16bit
//     <4=> 18bit
//     <5=> 24bit
// <i> Defines Color code for DPI Interface
// <i> Default: 24bit
#define RTE_MIPI_DSI_DPI_COLOR_CODE             5

// <o> DPI DATAEN pin active state
//     <0=> ACTIVE HIGH
//     <1=> ACTIVE LOW
// <i> Defines DPI DATAEN pin active state
// <i> Default: ACTIVE HIGH
#define RTE_MIPI_DSI_DPI_DATAEN_ACTIVE_LOW      0

// <o> DPI VSYNC pin active state
//     <0=> ACTIVE HIGH
//     <1=> ACTIVE LOW
// <i> Defines DPI VSYNC pin active state
// <i> Default: ACTIVE LOW
#define RTE_MIPI_DSI_DPI_VSYNC_ACTIVE_LOW       1

// <o> DPI HSYNC pin active state
//     <0=> ACTIVE HIGH
//     <1=> ACTIVE LOW
// <i> Defines DPI HSYNC pin active state
// <i> Default: ACTIVE LOW
#define RTE_MIPI_DSI_DPI_HSYNC_ACTIVE_LOW       1

// <o> DPI SHUTD pin active state
//     <0=> ACTIVE HIGH
//     <1=> ACTIVE LOW
// <i> Defines DPI SHUTD pin active state
// <i> Default: ACTIVE HIGH
#define RTE_MIPI_DSI_DPI_SHUTD_ACTIVE_LOW       0

// <o> DPI COLORM pin active state
//     <0=> ACTIVE HIGH
//     <1=> ACTIVE LOW
// <i> Defines DPI COLORM pin active state
// <i> Default: ACTIVE HIGH
#define RTE_MIPI_DSI_DPI_COLORM_ACTIVE_LOW      0

// <o> DSI IRQ priority
// <i> Defines Interrupt priority.
// <i> Default: 0
#define RTE_MIPI_DSI_IRQ_PRI                    0

// <o> DSI ILI9806E LCD PANEL
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines DSI ILI9806E LCD PANEL
// <i> Default: ENABLE
#define RTE_MIPI_DSI_ILI9806E_PANEL  1

#if RTE_MIPI_DSI_ILI9806E_PANEL

// <o> DSI ILI9806E panel variant
//     <0=> E43RB_FW405
//     <1=> E43GB_MW405
//     <2=> E50RA_MW550
// <i> Defines ILI9806E panel variant
// <i> Default: E43RB_FW405
#define RTE_ILI9806E_PANEL_VARIANT             1

#if (RTE_ILI9806E_PANEL_VARIANT == 0)
#define RTE_ILI9806E_PANEL_E43RB_FW405_EN      1
#elif (RTE_ILI9806E_PANEL_VARIANT == 1)
#define RTE_ILI9806E_PANEL_E43GB_MW405_EN      1
#elif (RTE_ILI9806E_PANEL_VARIANT == 2)
#define RTE_ILI9806E_PANEL_E50RA_MW550_EN      1
#endif

// <o> ILI9806 LCD panel maximum bitrate in mbps
// <i> Defines ILI9806 LCD panel maximum bitrate in mbps.
// <i> Default: 500
#define RTE_ILI9806E_PANEL_MAX_BITRATE_MBPS              500

// <o> ILI9806 LCD panel reset pin number
// <i> Defines ILI9806 LCD panel reset pin number.
// <i> Default: 6
#define RTE_ILI9806E_PANEL_RESET_PIN_NO                  6

// <o> ILI9806 LCD panel reset pin GPIO port number
// <i> Defines ILI9806 LCD panel reset pin GPIO port number.
// <i> Default: 4
#define RTE_ILI9806E_PANEL_RESET_GPIO_PORT               4

#if TARGET_BOARD == BOARD_AppKit_Alpha2
// <o> ILI9806 LCD panel back light pin number
// <i> Defines ILI9806 LCD panel back light pin number.
// <i> Default: 4
#define RTE_ILI9806E_PANEL_BL_LED_PIN_NO                 19

// <o> ILI9806 LCD panel back light pin GPIO port number
// <i> Defines ILI9806 LCD panel back light pin GPIO port number.
// <i> Default: 4
#define RTE_ILI9806E_PANEL_BL_LED_GPIO_PORT              3
#else
// <o> ILI9806 LCD panel back light pin number
// <i> Defines ILI9806 LCD panel back light pin number.
// <i> Default: 4
#define RTE_ILI9806E_PANEL_BL_LED_PIN_NO                 4

// <o> ILI9806 LCD panel back light pin GPIO port number
// <i> Defines ILI9806 LCD panel back light pin GPIO port number.
// <i> Default: 4
#define RTE_ILI9806E_PANEL_BL_LED_GPIO_PORT              4
#endif

#if (RTE_ILI9806E_PANEL_E43RB_FW405_EN || RTE_ILI9806E_PANEL_E43GB_MW405_EN)

// <o> Panel hsync time in pixels
// <i> Defines ILI9806 LCD panel hsync time in pixels.
// <i> Default: 4
#define RTE_PANEL_HSYNC_TIME            4

// <o> Panel FW405/MW405 hbp time in pixels
// <i> Defines ILI9806 LCD panel hbp time in pixels.
// <i> Default: 5
#define RTE_PANEL_HBP_TIME              5

// <o> Panel FW405/MW405 hfp time in pixels
// <i> Defines ILI9806 LCD panel hfp time in pixels.
// <i> Default: 5
#define RTE_PANEL_HFP_TIME              5

// <o> Panel FW405/MW405 hactive pixels
// <i> Defines ILI9806 LCD panel hactive pixels.
// <i> Default: 480
#define RTE_PANEL_HACTIVE_TIME          480

// <o> Panel FW405/MW405 vsync time in lines
// <i> Defines ILI9806 LCD panel vsync time in lines.
// <i> Default: 2
#define RTE_PANEL_VSYNC_LINE            2

// <o> Panel FW405/MW405 vbp time in lines
// <i> Defines ILI9806 LCD panel vbp time in lines.
// <i> Default: 10
#define RTE_PANEL_VBP_LINE              10

// <o> Panel FW405/MW405 vfp time in lines
// <i> Defines ILI9806 LCD panel vfp time in lines.
// <i> Default: 10
#define RTE_PANEL_VFP_LINE              10

// <o> Panel FW405/MW405 vactive lines
// <i> Defines ILI9806 LCD panel vactive lines.
// <i> Default: 800
#define RTE_PANEL_VACTIVE_LINE          800

#elif RTE_ILI9806E_PANEL_E50RA_MW550_EN

// <o> Panel MW550 hsync time in pixels
// <i> Defines ILI9806 LCD panel hsync time in pixels.
// <i> Default: 4
#define RTE_PANEL_HSYNC_TIME            4

// <o> Panel MW550 hbp time in pixels
// <i> Defines ILI9806 LCD panel hbp time in pixels.
// <i> Default: 30
#define RTE_PANEL_HBP_TIME              30

// <o> Panel MW550 hfp time in pixels
// <i> Defines ILI9806 LCD panel hfp time in pixels.
// <i> Default: 18
#define RTE_PANEL_HFP_TIME              18

// <o> Panel MW550 hactive pixels
// <i> Defines ILI9806 LCD panel hactive pixels.
// <i> Default: 480
#define RTE_PANEL_HACTIVE_TIME          480

// <o> Panel MW550 vsync time in lines
// <i> Defines ILI9806 LCD panel vsync time in lines.
// <i> Default: 4
#define RTE_PANEL_VSYNC_LINE            4

// <o> Panel MW550 vbp time in lines
// <i> Defines ILI9806 LCD panel vbp time in lines.
// <i> Default: 30
#define RTE_PANEL_VBP_LINE              30

// <o> Panel MW550 vfp time in lines
// <i> Defines ILI9806 LCD panel vfp time in lines.
// <i> Default: 20
#define RTE_PANEL_VFP_LINE              20

// <o> Panel MW550 vactive lines
// <i> Defines ILI9806 LCD panel vactive lines.
// <i> Default: 854
#define RTE_PANEL_VACTIVE_LINE          854
#endif
#endif

#endif
// </e> MIPI_DSI (mipi dsi) [Driver_MIPI_DSI]

// <e> TOUCH_SCREEN (touch screen) [Driver_Touch_Screen]
// <i> Configuration settings for Driver_Touch_Screen in component ::Drivers:touch screen
#define RTE_TOUCH_SCREEN 1

#if RTE_TOUCH_SCREEN

// <o> GT911 Touch screen
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines GT911 Touch screen
// <i> Default: ENABLE
#define RTE_GT911 1

#if RTE_GT911

// <o> select active touch points
//     <1=> 1
//     <2=> 2
//     <3=> 3
//     <4=> 4
//     <5=> 5
// <i> defines select active touch points
// <i> default: 5
#define RTE_ACTIVE_TOUCH_POINTS     5

// <o> GT911 Touch screen reset pin GPIO port number
// <i> Defines GT911 Touch screen reset pin GPIO port number.
// <i> Default: 4
#define RTE_GT911_TOUCH_RESET_GPIO_PORT   4

// <o> GT911 Touch screen reset pin number
// <i> Defines GT911 Touch screen reset pin number.
// <i> Default: 2
#define RTE_GT911_TOUCH_RESET_PIN_NO      2

#if TARGET_BOARD == BOARD_AppKit_Alpha2
// <o> GT911 Touch screen INT pin GPIO port number
// <i> Defines GT911 Touch screen INT pin GPIO port number.
// <i> Default: 2
#define RTE_GT911_TOUCH_INT_GPIO_PORT     2

// <o> GT911 Touch screen INT pin number
// <i> Defines GT911 Touch screen INT pin number.
// <i> Default: 20
#define RTE_GT911_TOUCH_INT_PIN_NO        21
#else
// <o> GT911 Touch screen INT pin GPIO port number
// <i> Defines GT911 Touch screen INT pin GPIO port number.
// <i> Default: 2
#define RTE_GT911_TOUCH_INT_GPIO_PORT     2

// <o> GT911 Touch screen INT pin number
// <i> Defines GT911 Touch screen INT pin number.
// <i> Default: 20
#define RTE_GT911_TOUCH_INT_PIN_NO        20
#endif

#endif

#endif
// </e> TOUCH_SCREEN (touch screen) [Driver_Touch_Screen]

// <e> CDC200 (cdc200) [Driver_CDC200]
// <i> Configuration settings for Driver_CDC200 in component ::Drivers:CDC200
#define RTE_CDC200 1

#if RTE_CDC200

// <o> CDC200 pixel format
//     <0=> ARGB8888
//     <1=> RGB888
//     <2=> RGB565
// <i> Defines CDC200 pixel format
// <i> Default: RGB888
#define RTE_CDC200_PIXEL_FORMAT              2

// <o> CDC200 DPI interface FPS
// <i> Defines CDC200 DPI interface Framrate per second.
// <i> Default: 60
#define RTE_CDC200_DPI_FPS                   60
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
#define RTE_I2S0_IRQ_PRI     10
#define RTE_I2S0_CLK_SOURCE  1
#define RTE_I2S0_DMA_ENABLE  1
#define RTE_I2S0_DMA_IRQ_PRI RTE_I2S0_IRQ_PRI
#endif
// </e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]

// <e> I2S1 (Integrated Interchip Sound 1) [Driver_SAI1]
// <i> Configuration settings for Driver_SAI1 in component ::Drivers:SAI
#define RTE_I2S1 1
#ifdef RTE_I2S1
#define RTE_I2S1_WSS_CLOCK_CYCLES 2
#define RTE_I2S1_SCLKG_CLOCK_CYCLES 0
#define RTE_I2S1_RX_TRIG_LVL 7
#define RTE_I2S1_TX_TRIG_LVL 8
#define RTE_I2S1_IRQ_PRI     10
#define RTE_I2S1_CLK_SOURCE  1
#define RTE_I2S1_DMA_ENABLE  1
#define RTE_I2S1_DMA_IRQ_PRI RTE_I2S1_IRQ_PRI
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
#define RTE_I2S2_DMA_IRQ_PRI RTE_I2S2_IRQ_PRI
#endif
// </e> I2S2 (Integrated Interchip Sound 2) [Driver_SAI2]

// <e> I2S3 (Integrated Interchip Sound 3) [Driver_SAI3]
// <i> Configuration settings for Driver_SAI3 in component ::Drivers:SAI
#define RTE_I2S3 1
#ifdef RTE_I2S3
#define RTE_I2S3_WSS_CLOCK_CYCLES 2
#define RTE_I2S3_SCLKG_CLOCK_CYCLES 0
#define RTE_I2S3_RX_TRIG_LVL 7
#define RTE_I2S3_TX_TRIG_LVL 8
#define RTE_I2S3_IRQ_PRI     10
#define RTE_I2S3_CLK_SOURCE  1
#define RTE_I2S3_DMA_ENABLE  1
#define RTE_I2S3_DMA_IRQ_PRI RTE_I2S3_IRQ_PRI
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

// <e> OSPI0 (Octal Serial Peripheral Interface 0) [Driver_OSPI]
// <i> Configuration settings for Driver_OSPI in component ::Drivers:OSPI
#define RTE_OSPI0                               1
#ifdef RTE_OSPI0
#define RTE_OSPI0_IRQ_PRIORITY                    0
#define RTE_OSPI0_SPI_FRAME_FORMAT                3
#define RTE_OSPI0_TX_FIFO_LEVEL_TO_START_TRANSFER 0
#define RTE_OSPI0_TX_LOAD_DUMMY_TO_START_LEVEL    0
#define RTE_OSPI0_TX_FIFO_THRESHOLD               64
#define RTE_OSPI0_RX_FIFO_THRESHOLD               0
#define RTE_OSPI0_CHIP_SELECTION_PIN              0
#endif
// </e> OSPI0 (Octal Serial Peripheral Interface 0) [Driver_OSPI]

// <e> OSPI1 (Octal Serial Peripheral Interface 1) [Driver_OSPI]
// <i> Configuration settings for Driver_OSPI in component ::Drivers:OSPI
#define RTE_OSPI1                               0
#ifdef RTE_OSPI1
#define RTE_OSPI1_IRQ_PRIORITY                    0
#define RTE_OSPI1_SPI_FRAME_FORMAT                3
#define RTE_OSPI1_TX_FIFO_LEVEL_TO_START_TRANSFER 0
#define RTE_OSPI1_TX_LOAD_DUMMY_TO_START_LEVEL    0
#define RTE_OSPI1_TX_FIFO_THRESHOLD               64
#define RTE_OSPI1_RX_FIFO_THRESHOLD               0
#define RTE_OSPI1_CHIP_SELECTION_PIN              0
#endif
// </e> OSPI1 (Octal Serial Peripheral Interface 1) [Driver_OSPI]

// <e> FLASH (OSPI ISSI FLASH) [Driver_Flash]
// <i> Configuration settings for Driver_Flash in component ::Drivers:Flash
#define RTE_OSPI_ISSI_FLASH               1
// <e> FLASH (OSPI ISSI FLASH) [Driver_Flash]

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
#define RTE_UTIMER_CHANNEL6_DRIVER_A                1
#define RTE_UTIMER_CHANNEL6_DRIVER_B                1
#define RTE_UTIMER_CHANNEL6_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL6_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL6_DRV_OP_AT_MATCH_COUNT   3
#define RTE_UTIMER_CHANNEL6_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL6_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL6_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE          1
#define RTE_UTIMER_CHANNEL6_BUFFER_OPERATION        1
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

#define RTE_UTIMER_CHANNEL12_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL12_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL12_DRIVER_A                1
#define RTE_UTIMER_CHANNEL12_DRIVER_B                0
#define RTE_UTIMER_CHANNEL12_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL12_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL12_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL12_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL12_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL12_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL12_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL12_BUFFER_OPERATION        0
#define RTE_UTIMER_CHANNEL12_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL12_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL12_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL12_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL12_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL12_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL12_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL12_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL12_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL12_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL13_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL13_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL13_DRIVER_A                1
#define RTE_UTIMER_CHANNEL13_DRIVER_B                0
#define RTE_UTIMER_CHANNEL13_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL13_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL13_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL13_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL13_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL13_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL13_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL13_BUFFER_OPERATION        0
#define RTE_UTIMER_CHANNEL13_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL13_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL13_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL13_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL13_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL13_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL13_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL13_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL13_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL13_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL14_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL14_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL14_DRIVER_A                1
#define RTE_UTIMER_CHANNEL14_DRIVER_B                0
#define RTE_UTIMER_CHANNEL14_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL14_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL14_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL14_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL14_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL14_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL14_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL14_BUFFER_OPERATION        0
#define RTE_UTIMER_CHANNEL14_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL14_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL14_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL14_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL14_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL14_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL14_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL14_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL14_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL14_UNDER_FLOW_IRQ_PRIORITY 0

#define RTE_UTIMER_CHANNEL15_FIXED_BUFFER            0
#define RTE_UTIMER_CHANNEL15_BUF_TROUGH_N_CREST      4
#define RTE_UTIMER_CHANNEL15_DRIVER_A                1
#define RTE_UTIMER_CHANNEL15_DRIVER_B                0
#define RTE_UTIMER_CHANNEL15_CMP_START_STATE         0
#define RTE_UTIMER_CHANNEL15_CMP_STOP_STATE          0
#define RTE_UTIMER_CHANNEL15_DRV_OP_AT_MATCH_COUNT   0
#define RTE_UTIMER_CHANNEL15_DRV_OP_AT_CYCLE_END     0
#define RTE_UTIMER_CHANNEL15_EVENT_AT_CREST          1
#define RTE_UTIMER_CHANNEL15_EVENT_AT_TROUGH         0
#define RTE_UTIMER_CHANNEL15_BUFFERING_TYPE          0
#define RTE_UTIMER_CHANNEL15_BUFFER_OPERATION        0
#define RTE_UTIMER_CHANNEL15_BUFFERING_TYPE_A        0
#define RTE_UTIMER_CHANNEL15_BUFFERING_TYPE_B        0
#define RTE_UTIMER_CHANNEL15_CAPTURE_A_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL15_CAPTURE_B_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL15_CAPTURE_C_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL15_CAPTURE_D_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL15_CAPTURE_E_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL15_CAPTURE_F_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL15_OVER_FLOW_IRQ_PRIORITY  0
#define RTE_UTIMER_CHANNEL15_UNDER_FLOW_IRQ_PRIORITY 0
#endif /*RTE_UTIMER*/
// </e> UTIMER (Universal timer) [Driver_UTIMER]


// </e> Analog configuration [vbat analog register2 and comparator register2]
#define RTE_ANALOG_CONFIG          1
#if RTE_ANALOG_CONFIG
#define RTE_VBAT_ANA_REG2_VAL      (0x388C4230)
#define RTE_COMP_REG2_VAL          (0x10200000 | 0x1C240100)
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

// <e> HWSEM0 (Hardware Semaphore) [Driver_HWSEM0]
// <i> Configuration settings for Driver_HWSEM0 in component ::Drivers:HWSEM
#define RTE_HWSEM0                                     1
#if RTE_HWSEM0
#define RTE_HWSEM0_IRQPRIORITY                         0
#endif

// </e> HWSEM0 (Hardware Semaphore) [Driver_HWSEM0]


// <e> HWSEM1 (Hardware Semaphore) [Driver_HWSEM1]
// <i> Configuration settings for Driver_HWSEM1 in component ::Drivers:HWSEM
#define RTE_HWSEM1                                     0
#if RTE_HWSEM1
#define RTE_HWSEM1_IRQPRIORITY                         0
#endif

// </e> HWSEM1 (Hardware Semaphore) [Driver_HWSEM1]


// <e> HWSEM2 (Hardware Semaphore) [Driver_HWSEM2]
// <i> Configuration settings for Driver_HWSEM2 in component ::Drivers:HWSEM
#define RTE_HWSEM2                                     0
#if RTE_HWSEM2
#define RTE_HWSEM2_IRQPRIORITY                         0
#endif

// </e> HWSEM2 (Hardware Semaphore) [Driver_HWSEM2]


// <e> HWSEM3 (Hardware Semaphore) [Driver_HWSEM3]
// <i> Configuration settings for Driver_HWSEM3 in component ::Drivers:HWSEM
#define RTE_HWSEM3                                     0
#if RTE_HWSEM3
#define RTE_HWSEM3_IRQPRIORITY                         0
#endif

// </e> HWSEM3 (Hardware Semaphore) [Driver_HWSEM3]


// <e> HWSEM4 (Hardware Semaphore) [Driver_HWSEM4]
// <i> Configuration settings for Driver_HWSEM4 in component ::Drivers:HWSEM
#define RTE_HWSEM4                                     0
#if RTE_HWSEM4
#define RTE_HWSEM4_IRQPRIORITY                         0
#endif

// </e> HWSEM4 (Hardware Semaphore) [Driver_HWSEM4]


// <e> HWSEM5 (Hardware Semaphore) [Driver_HWSEM5]
// <i> Configuration settings for Driver_HWSEM5 in component ::Drivers:HWSEM
#define RTE_HWSEM5                                     0
#if RTE_HWSEM5
#define RTE_HWSEM5_IRQPRIORITY                         0
#endif

// </e> HWSEM5 (Hardware Semaphore) [Driver_HWSEM5]


// <e> HWSEM6 (Hardware Semaphore) [Driver_HWSEM6]
// <i> Configuration settings for Driver_HWSEM6 in component ::Drivers:HWSEM
#define RTE_HWSEM6                                     0
#if RTE_HWSEM6
#define RTE_HWSEM6_IRQPRIORITY                         0
#endif

// </e> HWSEM6 (Hardware Semaphore) [Driver_HWSEM6]


// <e> HWSEM7 (Hardware Semaphore) [Driver_HWSEM7]
// <i> Configuration settings for Driver_HWSEM7 in component ::Drivers:HWSEM
#define RTE_HWSEM7                                     0
#if RTE_HWSEM7
#define RTE_HWSEM7_IRQPRIORITY                         0
#endif

// </e> HWSEM7 (Hardware Semaphore) [Driver_HWSEM7]


// <e> HWSEM8 (Hardware Semaphore) [Driver_HWSEM8]
// <i> Configuration settings for Driver_HWSEM8 in component ::Drivers:HWSEM
#define RTE_HWSEM8                                     0
#if RTE_HWSEM8
#define RTE_HWSEM8_IRQPRIORITY                         0
#endif

// </e> HWSEM8 (Hardware Semaphore) [Driver_HWSEM8]


// <e> HWSEM9 (Hardware Semaphore) [Driver_HWSEM9]
// <i> Configuration settings for Driver_HWSEM9 in component ::Drivers:HWSEM
#define RTE_HWSEM9                                     0
#if RTE_HWSEM9
#define RTE_HWSEM9_IRQPRIORITY                         0
#endif

// </e> HWSEM9 (Hardware Semaphore) [Driver_HWSEM9]


// <e> HWSEM10 (Hardware Semaphore) [Driver_HWSEM10]
// <i> Configuration settings for Driver_HWSEM10 in component ::Drivers:HWSEM
#define RTE_HWSEM10                                     0
#if RTE_HWSEM10
#define RTE_HWSEM10_IRQPRIORITY                         0
#endif

// </e> HWSEM10 (Hardware Semaphore) [Driver_HWSEM10]


// <e> HWSEM11 (Hardware Semaphore) [Driver_HWSEM11]
// <i> Configuration settings for Driver_HWSEM11 in component ::Drivers:HWSEM
#define RTE_HWSEM11                                     0
#if RTE_HWSEM11
#define RTE_HWSEM11_IRQPRIORITY                         0
#endif

// </e> HWSEM11 (Hardware Semaphore) [Driver_HWSEM11]


// <e> HWSEM12 (Hardware Semaphore) [Driver_HWSEM12]
// <i> Configuration settings for Driver_HWSEM12 in component ::Drivers:HWSEM
#define RTE_HWSEM12                                     0
#if RTE_HWSEM12
#define RTE_HWSEM12_IRQPRIORITY                         0
#endif

// </e> HWSEM12 (Hardware Semaphore) [Driver_HWSEM12]


// <e> HWSEM13 (Hardware Semaphore) [Driver_HWSEM13]
// <i> Configuration settings for Driver_HWSEM13 in component ::Drivers:HWSEM
#define RTE_HWSEM13                                     0
#if RTE_HWSEM13
#define RTE_HWSEM13_IRQPRIORITY                         0
#endif

// </e> HWSEM13 (Hardware Semaphore) [Driver_HWSEM13]


// <e> HWSEM14 (Hardware Semaphore) [Driver_HWSEM14]
// <i> Configuration settings for Driver_HWSEM14 in component ::Drivers:HWSEM
#define RTE_HWSEM14                                     0
#if RTE_HWSEM14
#define RTE_HWSEM14_IRQPRIORITY                         0
#endif

// </e> HWSEM14 (Hardware Semaphore) [Driver_HWSEM14]


// <e> HWSEM15 (Hardware Semaphore) [Driver_HWSEM15]
// <i> Configuration settings for Driver_HWSEM15 in component ::Drivers:HWSEM
#define RTE_HWSEM15                                     0
#if RTE_HWSEM15
#define RTE_HWSEM15_IRQPRIORITY                         0
#endif

// </e> HWSEM15 (Hardware Semaphore) [Driver_HWSEM15]

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

// <e> Comparator common bits for each instance
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
#define RTE_DMA0_ABORT_IRQ_PRI 0
#endif
// </e> DMA0 (Direct Memory Access Controller) [Driver_DMA0]

// <e> DMA2 (Direct Memory Access Controller) [Driver_DMA2]
// <i> Configuration settings for Driver_DMA2 in component ::Drivers:DMA
#define RTE_DMA2      0
#if RTE_DMA2
//   DMA APB Interface to be used <0-1>
//   0 - Secure APB interface, 1 - Non-Secure
//   Default: 0
#define RTE_DMA2_APB_INTERFACE 0
#define RTE_DMA2_ABORT_IRQ_PRI 0
#endif
// </e> DMA2 (Direct Memory Access Controller) [Driver_DMA2]


#endif  /* __RTE_DEVICE_H */
