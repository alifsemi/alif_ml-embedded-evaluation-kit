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

// <e> RTSS_M55 (Core Selection)
// <i> Select if the Configured core is M55_HP or M55_HE
#if defined(M55_HP)
#define RTE_RTSS_M55_HP	1
#define RTE_RTSS_M55_HE	0
#elif defined(M55_HE)
#define RTE_RTSS_M55_HP	0
#define RTE_RTSS_M55_HE	1
#elif defined(A32)
#define RTE_RTSS_M55_HP	0
#define RTE_RTSS_M55_HE	0
#else
#error "Undefined M55 CPU!"
#endif

// </e> RTSS_M55  (Core Selection)

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

// <o> camera controller interrupt priority <0-255>
// <i> Defines camera controller interrupt priority.
// <i> Default: 0
#define RTE_CAMERA0_IRQ_PRI                    0

// <e> ARX3A0 [Driver_ARX3A0]
// <o> Enable/Disable ARX3A0 camera sensor
//     <0=> disable
//     <1=> enable
// <i> define if to enable or disable ARX3A0 camera sensor
// <i> default: enable
#define RTE_ARX3A0_CAMERA_SENSOR_ENABLE        1

#if (RTE_ARX3A0_CAMERA_SENSOR_ENABLE)

// <o> Select camera ARX3A0 clock source
//     <0=> internal generated camera clock
//     <1=> camera clock from external camera device
// <i> Defines camera ARX3A0 clock source
// <i> Default: internal generated camera clock
#define RTE_ARX3A0_CAMERA_CLOCK_SOURCE                  0

// <o> Select camera ARX3A0 pixel clock polarity
//     <0=> not invert camera pixclk
//     <1=> invert camera pixclk
// <i> Defines camera pixel ARX3A0 clock polarity
// <i> Default: not invert camera pixclk
#define RTE_ARX3A0_CAMERA_SENSOR_PIXEL_CLK_POL          0

// <o> Select camera ARX3A0 frame per second
//     <5=>   5 FPS
//     <40=> 40 FPS
//     <60=> 60 FPS
//     <90=> 90 FPS
// <i> Defines camera ARX3A0 frame per second
// <i> Default: 5 FPS
#define RTE_ARX3A0_CAMERA_SENSOR_CFG_FPS                40

// <o> Select camera ARX3A0 frequency
// <i> Defines camera ARX3A0 frequency
// <i> Default: 400000000
#define RTE_ARX3A0_CAMERA_SENSOR_FREQ                   400000000

// <o> Select camera sensor ARX3A0 clock source division [Divisor] <2-511>
// <i> Defines camera sensor ARX3A0 clock source division
// <i> Default: 0x14
#define RTE_ARX3A0_CAMERA_SENSOR_CLK_SCR_DIV            20

// <o> Select camera ARX3A0 HSYNC polarity
//     <0=>  not invert HSYNC input
//     <1=>  invert HSYNC input
// <i> Defines camera ARX3A0 HSYNC polarity
// <i> Default:  not invert VSYNC input
#define RTE_ARX3A0_CAMERA_SENSOR_HSYNC_POL              0

// <o> Select camera ARX3A0 VSYNC polarity
//     <0=> not invert VSYNC input
//     <1=> invert VSYNC input
// <i> Defines camera ARX3A0 VSYNC polarity
// <i> Default:  not invert VSYNC input
#define RTE_ARX3A0_CAMERA_SENSOR_VSYNC_POL              0

// <o> Select camera ARX3A0 HSYNC mode
//     <0=> sync enable
//     <1=> data enable
// <i> Defines camera ARX3A0 HSYNC mode
// <i> Default: sync enable
#define RTE_ARX3A0_CAMERA_SENSOR_HSYNC_MODE             0

// <o> Select video data mode
//     <0=> 8 bit
//     <1=> 16 bit
//     <2=> 32 bit
//     <3=> 64 bit
// <i> Defines video data mode
// <i> Default: 8 bit
#define RTE_ARX3A0_CAMERA_SENSOR_DATA_MODE              0

// <o> Select camera ARX3A0 data mask
//     <0=> 10 bit
//     <1=> 12 bit
//     <2=> 14 bit
//     <3=> 16 bit
// <i> Defines camera ARX3A0 data mask
// <i> no impact on 8-bit mode
// <i> Default: 10 bit
#define RTE_ARX3A0_CAMERA_SENSOR_DATA_MASK              0

// <o> Select camera sensor ARX3A0 reset pin number
// <i> Defines camera sensor ARX3A0 reset pin number
// <i> Default: 5
#define RTE_ARX3A0_CAMERA_RESET_PIN_NO                  BOARD_CAMERA_RESET_PIN_NO

// <o> Select camera sensor ARX3A0 reset GPIO port
// <i> Defines camera sensor ARX3A0 reset GPIO port
// <i> Default: 4
#define RTE_ARX3A0_CAMERA_RESET_GPIO_PORT               BOARD_CAMERA_RESET_GPIO_PORT

// <o> Select I3C instance for ARX3A0 camera sensor communication
// <i> Define I3C instance for ARX3A0 camera sensor communication
// <i> Default: 0
#define RTE_ARX3A0_CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE          0
#endif
// </e> ARX3A0 [Driver_ARX3A0]

// <e> MT9M114 [Driver_MT9M114]
// <o> Enable/Disable MT9M114 camera sensor
//     <0=> disable
//     <1=> enable
// <i> define if to enable or disable MT9M114 camera sensor
// <i> default: disable
#define RTE_MT9M114_CAMERA_SENSOR_ENABLE       0

#if RTE_MT9M114_CAMERA_SENSOR_ENABLE

// <o> Select camera MT9M114 clock source
//     <0=> internal generated camera clock
//     <1=> camera clock from external camera device
// <i> Defines camera MT9M114 clock source
// <i> Default: internal generated camera clock
#define RTE_MT9M114_CAMERA_CLOCK_SOURCE                  0

// <o> Select camera MT9M114 pixel clock polarity
//     <0=> not invert camera pixclk
//     <1=> invert camera pixclk
// <i> Defines camera MT9M114 pixel clock polarity
// <i> Default: invert camera pixclk
#define RTE_MT9M114_CAMERA_SENSOR_PIXEL_CLK_POL          1

// <o> Select camera MT9M114 HSYNC polarity
//     <0=>  not invert HSYNC input
//     <1=>  invert HSYNC input
// <i> Defines camera MT9M114 HSYNC polarity
// <i> Default:  not invert HSYNC input
#define RTE_MT9M114_CAMERA_SENSOR_HSYNC_POL              0

// <o> Select camera MT9M114 VSYNC polarity
//     <0=> not invert VSYNC input
//     <1=> invert VSYNC input
// <i> Defines camera MT9M114 VSYNC polarity
// <i> Default:  not invert VSYNC input
#define RTE_MT9M114_CAMERA_SENSOR_VSYNC_POL              0

// <o> Select camera MT9M114 HSYNC mode
//     <0=> sync enable
//     <1=> data enable
// <i> Defines camera MT9M114 HSYNC mode
// <i> Default: data enable
#define RTE_MT9M114_CAMERA_SENSOR_HSYNC_MODE             1

// <o> Select video data mode
//     <0=> 8 bit
//     <1=> 16 bit
//     <2=> 32 bit
//     <3=> 64 bit
// <i> Defines video data mode
// <i> Default: 8 bit
#define RTE_MT9M114_CAMERA_SENSOR_DATA_MODE              0

// <o> Select camera MT9M114 data mask
//     <0=> 10 bit
//     <1=> 12 bit
//     <2=> 14 bit
//     <3=> 16 bit
// <i> Defines camera MT9M114 data mask
// <i> Default: 10 bit
#define RTE_MT9M114_CAMERA_SENSOR_DATA_MASK              0

// <o> Select I3C instance for MT9M114 camera sensor communication
// <i> Define I3C instance for MT9M114 camera sensor communication
// <i> Default: 0
#define RTE_MT9M114_CAMERA_SENSOR_I2C_USING_I3Cx_INSTANCE          0
#endif
// </e> MT9M114 [Driver_MT9M114]

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

// <o> select CSI2 pixel clock divider [Divisor] <2-511>
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

// <o> CSI2 interrupt priority <0-255>
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

// <o> DSI IRQ priority <0-255>
// <i> Defines Interrupt priority.
// <i> Default: 0
#define RTE_MIPI_DSI_IRQ_PRI                    0

// <e> MIPI_DSI (ILI9806E_PANEL) [Driver_ILI9806E_PANEL]
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
#define RTE_ILI9806E_PANEL_VARIANT             BOARD_ILI9806E_PANEL_VARIANT

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
#define RTE_ILI9806E_PANEL_RESET_PIN_NO                  BOARD_LCD_RESET_PIN_NO

// <o> ILI9806 LCD panel reset pin GPIO port number
// <i> Defines ILI9806 LCD panel reset pin GPIO port number.
// <i> Default: 4
#define RTE_ILI9806E_PANEL_RESET_GPIO_PORT               BOARD_LCD_RESET_GPIO_PORT

// <o> ILI9806 LCD panel back light pin number
// <i> Defines ILI9806 LCD panel back light pin number.
// <i> Default: 4
#define RTE_ILI9806E_PANEL_BL_LED_PIN_NO                 BOARD_LCD_BACKLIGHT_PIN_NO

// <o> ILI9806 LCD panel back light pin GPIO port number
// <i> Defines ILI9806 LCD panel back light pin GPIO port number.
// <i> Default: 4
#define RTE_ILI9806E_PANEL_BL_LED_GPIO_PORT              BOARD_LCD_BACKLIGHT_GPIO_PORT

// <e> MIPI_DSI (ILI9806E_PANEL_E43RB_FW405 | ILI9806E_PANEL_E43GB_MW405) [Driver_ILI9806E_PANEL]
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
// </e> MIPI_DSI (ILI9806E_PANEL_E43RB_FW405 | ILI9806E_PANEL_E43GB_MW405) [Driver_ILI9806E_PANEL]

// <e> MIPI_DSI (ILI9806E_PANEL_E50RA_MW550) [Driver_ILI9806E_PANEL]
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
// </e> MIPI_DSI (ILI9806E_PANEL_E50RA_MW550) [Driver_ILI9806E_PANEL]

#endif
// </e> MIPI_DSI (ILI9806E_PANEL) [Driver_ILI9806E_PANEL]

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
#define RTE_GT911_TOUCH_RESET_GPIO_PORT   BOARD_TOUCH_RESET_GPIO_PORT

// <o> GT911 Touch screen reset pin number
// <i> Defines GT911 Touch screen reset pin number.
// <i> Default: 2
#define RTE_GT911_TOUCH_RESET_PIN_NO      BOARD_TOUCH_RESET_PIN_NO

// <o> GT911 Touch screen INT pin GPIO port number
// <i> Defines GT911 Touch screen INT pin GPIO port number.
// <i> Default: 2
#define RTE_GT911_TOUCH_INT_GPIO_PORT     BOARD_TOUCH_INT_GPIO_PORT

// <o> GT911 Touch screen INT pin number
// <i> Defines GT911 Touch screen INT pin number.
// <i> Default: 20
#define RTE_GT911_TOUCH_INT_PIN_NO        BOARD_TOUCH_INT_PIN_NO
#endif

#endif
// </e> TOUCH_SCREEN (touch screen) [Driver_Touch_Screen]

// <e> CDC200 (cdc200) [Driver_CDC200]
// <i> Configuration settings for Driver_CDC200 in component ::Drivers:CDC200
#define RTE_CDC200 1

#if RTE_CDC200

// <o> CDC200 IRQ priority <0-255>
// <i> Defines Interrupt priority.
// <i> Default: 0
#define RTE_CDC200_IRQ_PRI                   0

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

// <o> Parallel ILI6122 LCD PANEL
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines Parallel ILI6122 LCD PANEL
// <i> Default: ENABLE
#define RTE_ILI6122_PANEL  0

#if RTE_ILI6122_PANEL

// <o> Panel hsync time in pixels
// <i> Defines ILI6122 LCD panel hsync time in pixels.
// <i> Default: 1
#define RTE_PANEL_HSYNC_TIME            1

// <o> Panel hbp time in pixels
// <i> Defines ILI6122 LCD panel hbp time in pixels.
// <i> Default: 46
#define RTE_PANEL_HBP_TIME              46

// <o> Panel hfp time in pixels
// <i> Defines ILI6122 LCD panel hfp time in pixels.
// <i> Default: 210
#define RTE_PANEL_HFP_TIME              210

// <o> Panel hactive pixels
// <i> Defines ILI6122 LCD panel hactive pixels.
// <i> Default: 800
#define RTE_PANEL_HACTIVE_TIME          800

// <o> Panel vsync time in lines
// <i> Defines ILI6122 LCD panel vsync time in lines.
// <i> Default: 1
#define RTE_PANEL_VSYNC_LINE            1

// <o> Panel vbp time in lines
// <i> Defines ILI6122 LCD panel vbp time in lines.
// <i> Default: 23
#define RTE_PANEL_VBP_LINE              23

// <o> Panel vfp time in lines
// <i> Defines ILI6122 LCD panel vfp time in lines.
// <i> Default: 22
#define RTE_PANEL_VFP_LINE              22

// <o> Panel vactive lines
// <i> Defines ILI6122 LCD panel vactive lines.
// <i> Default: 480
#define RTE_PANEL_VACTIVE_LINE          480
#endif

#endif
// </e> CDC200 (cdc200) [Driver_CDC200]

// <e> I3C0 (Improved Inter-Integrated Circuit) [Driver_I3C0]
// <i> Configuration settings for Driver_I3C0 in component ::Drivers:I3C
#define RTE_I3C0   1
#if RTE_I3C0
#define RTE_I3C0_IRQ_PRI       0
#endif
// </e> I3C0 (Improved Inter-Integrated Circuit) [Driver_I3C0]


// <h> SPI (Serial Peripheral Interface)
// <e> SPI0 (Serial Peripheral Interface 0) [Driver_SPI0]
// <i> Configuration settings for Driver_SPI0 in component ::Drivers:SPI
#define RTE_SPI0 1

#ifdef RTE_SPI0

// <o> SPI0 IRQ priority <0-255>
// <i> Defines Interrupt priority for SPI0.
// <i> Default: 0
#define RTE_SPI0_IRQ_PRIORITY                   0

// <o> SPI0 frame format
//     <0=> SPI
//     <1=> SSP
//     <2=> MicroWire
// <i> Defines frame format for SPI0.
// <i> Default: SPI
#define RTE_SPI0_SPI_FRAME_FORMAT               0

// <o> SPI0 TX FIFO Start level <0-15>
// <i> Defines TX FIFO transfer start level for SPI0.
// <i> Default: 0
#define RTE_SPI0_TX_FIFO_LEVEL_TO_START_TRANSFER 0

// <o> SPI0 load dummy data
//    <0=> DISABLE
//    <1=> ENABLE
// <i> load dummy data to start transfer for SPI0.
// <i> Default: 0
#define RTE_SPI0_TX_LOAD_DUMMY_TO_START_LEVEL   0

// <o> SPI0 TX FIFO threshold <0-15>
// <i> Defines TX FIFO threshold for SPI0.
// <i> Default: 0
#define RTE_SPI0_TX_FIFO_THRESHOLD              0

// <o> SPI0 RX FIFO threshold <0-15>
// <i> Defines RX FIFO threshold for SPI0.
// <i> Default: 0
#define RTE_SPI0_RX_FIFO_THRESHOLD              0

// <o> SPI0 slave selection
//    <0=> SS_0
//    <1=> SS_1
//    <2=> SS_2
//    <3=> SS_3
// <i> Defines slave selection for SPI0.
// <i> Default: 0
#define RTE_SPI0_CHIP_SELECTION_PIN             0

// <o> SPI0 port of SS pin <1-4>
// <i> Defines SPI0 port of SS pin in software controlled mode.
// <i> Default: 1
#define RTE_SPI0_SW_SPI_PORT                    3

// <o> SPI0 SW SS pin number <0-31>
// <i> Defines SPI0 SS pin number in software controlled mode.
// <i> Default: 23
#define RTE_SPI0_SW_SPI_PIN                     23

// <o> SPI0 SW SS Polarity
//    <0=> ACTIVE LOW
//    <1=> ACTIVE HIGH
// <i> Defines SW controlled slave select polarity for SPI0.
// <i> Default: 0
#define RTE_SPI0_SW_SPI_SS_POLARITY             0
#endif
// </e> SPI0 (Serial Peripheral Interface 0) [Driver_SPI0]


// <e> SPI1 (Serial Peripheral Interface 1) [Driver_SPI1]
// <i> Configuration settings for Driver_SPI1 in component ::Drivers:SPI
#define RTE_SPI1 1

#ifdef RTE_SPI1

// <o> SPI1 IRQ priority <0-255>
// <i> Defines Interrupt priority for SPI1.
// <i> Default: 0
#define RTE_SPI1_IRQ_PRIORITY                   0

// <o> SPI1 frame format
//     <0=> SPI
//     <1=> SSP
//     <2=> MicroWire
// <i> Defines frame format for SPI1.
// <i> Default: SPI
#define RTE_SPI1_SPI_FRAME_FORMAT               0

// <o> SPI1 TX FIFO Start level <0-15>
// <i> Defines TX FIFO transfer start level for SPI1.
// <i> Default: 0
#define RTE_SPI1_TX_FIFO_LEVEL_TO_START_TRANSFER 0

// <o> SPI1 load dummy data
//    <0=> DISABLE
//    <1=> ENABLE
// <i> load dummy data to start transfer for SPI1.
// <i> Default: 0
#define RTE_SPI1_TX_LOAD_DUMMY_TO_START_LEVEL   0

// <o> SPI1 TX FIFO threshold <0-15>
// <i> Defines TX FIFO threshold for SPI1.
// <i> Default: 0
#define RTE_SPI1_TX_FIFO_THRESHOLD              0

// <o> SPI1 RX FIFO threshold <0-15>
// <i> Defines RX FIFO threshold for SPI1.
// <i> Default: 0
#define RTE_SPI1_RX_FIFO_THRESHOLD              0

// <o> SPI1 slave selection
//    <0=> SS_0
//    <1=> SS_1
//    <2=> SS_2
//    <3=> SS_3
// <i> Defines slave selection for SPI1.
// <i> Default: 0
#define RTE_SPI1_CHIP_SELECTION_PIN             0

// <o> SPI1 port of SS pin <1-4>
// <i> Defines SPI1 port of SS pin in software controlled mode.
// <i> Default: 1
#define RTE_SPI1_SW_SPI_PORT                    1

// <o> SPI1 SW SS pin number <0-31>
// <i> Defines SPI1 SS pin number in software controlled mode.
// <i> Default: 6
#define RTE_SPI1_SW_SPI_PIN                     6

// <o> SPI1 SW SS Polarity
//    <0=> ACTIVE LOW
//    <1=> ACTIVE HIGH
// <i> Defines SW controlled slave select polarity for SPI1.
// <i> Default: 0
#define RTE_SPI1_SW_SPI_SS_POLARITY             0
#endif
// </e> SPI1 (Serial Peripheral Interface 1) [Driver_SPI1]

// <e> SPI2 (Serial Peripheral Interface 2) [Driver_SPI2]
// <i> Configuration settings for Driver_SPI2 in component ::Drivers:SPI
#define RTE_SPI2 1

#ifdef RTE_SPI2

// <o> SPI2 IRQ priority <0-255>
// <i> Defines Interrupt priority for SPI2.
// <i> Default: 0
#define RTE_SPI2_IRQ_PRIORITY                   0

// <o> SPI2 frame format
//     <0=> SPI
//     <1=> SSP
//     <2=> MicroWire
// <i> Defines frame format for SPI2.
// <i> Default: SPI
#define RTE_SPI2_SPI_FRAME_FORMAT               0

// <o> SPI2 TX FIFO Start level <0-15>
// <i> Defines TX FIFO transfer start level for SPI2.
// <i> Default: 0
#define RTE_SPI2_TX_FIFO_LEVEL_TO_START_TRANSFER 0

// <o> SPI2 load dummy data
//    <0=> DISABLE
//    <1=> ENABLE
// <i> load dummy data to start transfer for SPI2.
// <i> Default: 0
#define RTE_SPI2_TX_LOAD_DUMMY_TO_START_LEVEL   0

// <o> SPI2 TX FIFO threshold <0-15>
// <i> Defines TX FIFO threshold for SPI2.
// <i> Default: 0
#define RTE_SPI2_TX_FIFO_THRESHOLD              0

// <o> SPI2 RX FIFO threshold <0-15>
// <i> Defines RX FIFO threshold for SPI2.
// <i> Default: 0
#define RTE_SPI2_RX_FIFO_THRESHOLD              0

// <o> SPI2 slave selection
//    <0=> SS_0
//    <1=> SS_1
//    <2=> SS_2
//    <3=> SS_3
// <i> Defines slave selection for SPI2.
// <i> Default: 0
#define RTE_SPI2_CHIP_SELECTION_PIN             0

// <o> SPI2 port of SS pin <1-4>
// <i> Defines SPI2 port of SS pin in software controlled mode
// <i> Default: 1
#define RTE_SPI2_SW_SPI_PORT                    1

// <o> SPI2 SW SS pin number <0-31>
// <i> Defines SPI2 SS pin number in software controlled mode
// <i> Default: 13
#define RTE_SPI2_SW_SPI_PIN                     13

// <o> SPI2 SW SS Polarity
//    <0=> ACTIVE LOW
//    <1=> ACTIVE HIGH
// <i> Defines SW controlled slave select polarity for SPI2.
// <i> Default: 0
#define RTE_SPI2_SW_SPI_SS_POLARITY             0
#endif
// </e> SPI2 (Serial Peripheral Interface 2) [Driver_SPI2]


// <e> SPI3 (Serial Peripheral Interface 3) [Driver_SPI3]
// <i> Configuration settings for Driver_SPI3 in component ::Drivers:SPI
#define RTE_SPI3 1

#ifdef RTE_SPI3

// <o> SPI3 IRQ priority <0-255>
// <i> Defines Interrupt priority for SPI3.
// <i> Default: 0
#define RTE_SPI3_IRQ_PRIORITY                   0

// <o> SPI3 frame format
//     <0=> SPI
//     <1=> SSP
//     <2=> MicroWire
// <i> Defines frame format for SPI3.
// <i> Default: SPI
#define RTE_SPI3_SPI_FRAME_FORMAT               0

// <o> SPI3 TX FIFO Start level <0-15>
// <i> Defines TX FIFO transfer start level for SPI3.
// <i> Default: 0
#define RTE_SPI3_TX_FIFO_LEVEL_TO_START_TRANSFER 0

// <o> SPI3 load dummy data
//    <0=> DISABLE
//    <1=> ENABLE
// <i> load dummy data to start transfer for SPI3.
// <i> Default: 0
#define RTE_SPI3_TX_LOAD_DUMMY_TO_START_LEVEL   0

// <o> SPI3 TX FIFO threshold <0-15>
// <i> Defines TX FIFO threshold for SPI3.
// <i> Default: 0
#define RTE_SPI3_TX_FIFO_THRESHOLD              0

// <o> SPI3 RX FIFO threshold <0-15>
// <i> Defines RX FIFO threshold for SPI3.
// <i> Default: 0
#define RTE_SPI3_RX_FIFO_THRESHOLD              0

// <o> SPI3 slave selection
//    <0=> SS_0
//    <1=> SS_1
//    <2=> SS_2
//    <3=> SS_3
// <i> Defines slave selection for SPI3.
// <i> Default: 0
#define RTE_SPI3_CHIP_SELECTION_PIN             1

// <o> SPI3 port of SS pin <1-4>
// <i> Defines SPI3 port of SS pin in software controlled mode
// <i> Default: 2
#define RTE_SPI3_SW_SPI_PORT                    2

// <o> SPI3 SW SS pin number <0-31>
// <i> Defines SPI3 SS pin number in software controlled mode
// <i> Default: 24
#define RTE_SPI3_SW_SPI_PIN                     24

// <o> SPI3 SW SS Polarity
//    <0=> ACTIVE LOW
//    <1=> ACTIVE HIGH
// <i> Defines SW controlled slave select polarity for SPI3.
// <i> Default: 1
#define RTE_SPI3_SW_SPI_SS_POLARITY             1
#endif
// </e> SPI3 (Serial Peripheral Interface 3) [Driver_SPI3]
// </h> SPI (Serial Peripheral Interface)


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
#define RTE_OSPI1                               1
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
// </e> FLASH (OSPI ISSI FLASH) [Driver_Flash]


// <h> I2S (Integrated Interchip Sound)
// <e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]
// <i> Configuration settings for Driver_SAI0 in component ::Drivers:SAI
#define RTE_I2S0 1

#ifdef RTE_I2S0

// <o> I2S0 WORD SELECT SIZE
//    <0=> WSS_CLOCK_CYCLES_16
//    <1=> WSS_CLOCK_CYCLES_24
//    <2=> WSS_CLOCK_CYCLES_32
// <i> Defines I2S 0 size of word.
// <i> Default: WSS_CLOCK_CYCLES_32
#define RTE_I2S0_WSS_CLOCK_CYCLES 2

// <o> I2S0 SCLK GATING
//    <0=> NO_CLOCK_GATING
//    <1=> SCLKG_CLOCK_CYCLES_12
//    <2=> SCLKG_CLOCK_CYCLES_16
//    <3=> SCLKG_CLOCK_CYCLES_20
//    <4=> SCLKG_CLOCK_CYCLES_24
// <i> Defines I2S 0 SCLK Gating.
// <i> Default: NO_CLOCK_GATING
#define RTE_I2S0_SCLKG_CLOCK_CYCLES 0

// <o> I2S0 RX FIFO TRIGGER LEVEL
//    <0=>  TRIGGER_LEVEL_1
//    <1=>  TRIGGER_LEVEL_2
//    <2=>  TRIGGER_LEVEL_3
//    <3=>  TRIGGER_LEVEL_4
//    <4=>  TRIGGER_LEVEL_5
//    <5=>  TRIGGER_LEVEL_6
//    <6=>  TRIGGER_LEVEL_7
//    <7=>  TRIGGER_LEVEL_8
//    <8=>  TRIGGER_LEVEL_9
//    <9=>  TRIGGER_LEVEL_10
//    <10=> TRIGGER_LEVEL_11
//    <11=> TRIGGER_LEVEL_12
//    <12=> TRIGGER_LEVEL_13
//    <13=> TRIGGER_LEVEL_14
//    <14=> TRIGGER_LEVEL_15
//    <15=> TRIGGER_LEVEL_16
// <i> Defines I2S 0 receive fifo trigger level.
// <i> Default: TRIGGER_LEVEL_8
#define RTE_I2S0_RX_TRIG_LVL 7

// <o> I2S0 TX FIFO TRIGGER LEVEL
//    <0=>  TRIGGER_LEVEL_1
//    <1=>  TRIGGER_LEVEL_2
//    <2=>  TRIGGER_LEVEL_3
//    <3=>  TRIGGER_LEVEL_4
//    <4=>  TRIGGER_LEVEL_5
//    <5=>  TRIGGER_LEVEL_6
//    <6=>  TRIGGER_LEVEL_7
//    <7=>  TRIGGER_LEVEL_8
//    <8=>  TRIGGER_LEVEL_9
//    <9=>  TRIGGER_LEVEL_10
//    <10=> TRIGGER_LEVEL_11
//    <11=> TRIGGER_LEVEL_12
//    <12=> TRIGGER_LEVEL_13
//    <13=> TRIGGER_LEVEL_14
//    <14=> TRIGGER_LEVEL_15
//    <15=> TRIGGER_LEVEL_16
// <i> Defines I2S 0 transfer fifo trigger level.
// <i> Default: TRIGGER_LEVEL_9
#define RTE_I2S0_TX_TRIG_LVL 8

// <o> I2S0 IRQ priority <0-255>
// <i> Defines I2S 0 Interrupt priority.
// <i> Default: 1
#define RTE_I2S0_IRQ_PRI     0

// <o> I2S0 CLK SOURCE
//    <0=> CLK_76.8MHz
//    <1=> CLK_EXTERNAL
// <i> Defines I2S 0 clock source.
// <i> Default: CLK_76.8MHz
#define RTE_I2S0_CLK_SOURCE  0

// <o> I2S0 DMA ENABLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines DMA feature for I2S 0.
// <i> Default: ENABLE
#define RTE_I2S0_DMA_ENABLE  1

// <i> Defines DMA Interrupt priority for I2S 0.
#define RTE_I2S0_DMA_IRQ_PRI  RTE_I2S0_IRQ_PRI
#endif
// </e> I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]


// <e> I2S1 (Integrated Interchip Sound 1) [Driver_SAI1]
// <i> Configuration settings for Driver_SAI1 in component ::Drivers:SAI
#define RTE_I2S1   1

#ifdef RTE_I2S1

// <o> I2S1 WORD SELECT SIZE
//    <0=> WSS_CLOCK_CYCLES_16
//    <1=> WSS_CLOCK_CYCLES_24
//    <2=> WSS_CLOCK_CYCLES_32
// <i> Defines I2S 1 size of word.
// <i> Default: WSS_CLOCK_CYCLES_32
#define RTE_I2S1_WSS_CLOCK_CYCLES 2

// <o> I2S1 SCLK GATING
//    <0=> NO_CLOCK_GATING
//    <1=> SCLKG_CLOCK_CYCLES_12
//    <2=> SCLKG_CLOCK_CYCLES_16
//    <3=> SCLKG_CLOCK_CYCLES_20
//    <4=> SCLKG_CLOCK_CYCLES_24
// <i> Defines I2S 1 SCLK Gating.
// <i> Default: NO_CLOCK_GATING
#define RTE_I2S1_SCLKG_CLOCK_CYCLES 0

// <o> I2S1 RX FIFO TRIGGER LEVEL
//    <0=>  TRIGGER_LEVEL_1
//    <1=>  TRIGGER_LEVEL_2
//    <2=>  TRIGGER_LEVEL_3
//    <3=>  TRIGGER_LEVEL_4
//    <4=>  TRIGGER_LEVEL_5
//    <5=>  TRIGGER_LEVEL_6
//    <6=>  TRIGGER_LEVEL_7
//    <7=>  TRIGGER_LEVEL_8
//    <8=>  TRIGGER_LEVEL_9
//    <9=>  TRIGGER_LEVEL_10
//    <10=> TRIGGER_LEVEL_11
//    <11=> TRIGGER_LEVEL_12
//    <12=> TRIGGER_LEVEL_13
//    <13=> TRIGGER_LEVEL_14
//    <14=> TRIGGER_LEVEL_15
//    <15=> TRIGGER_LEVEL_16
// <i> Defines I2S 1 receive fifo trigger level.
// <i> Default: TRIGGER_LEVEL_8
#define RTE_I2S1_RX_TRIG_LVL 7

// <o> I2S1 TX FIFO TRIGGER LEVEL
//    <0=>  TRIGGER_LEVEL_1
//    <1=>  TRIGGER_LEVEL_2
//    <2=>  TRIGGER_LEVEL_3
//    <3=>  TRIGGER_LEVEL_4
//    <4=>  TRIGGER_LEVEL_5
//    <5=>  TRIGGER_LEVEL_6
//    <6=>  TRIGGER_LEVEL_7
//    <7=>  TRIGGER_LEVEL_8
//    <8=>  TRIGGER_LEVEL_9
//    <9=>  TRIGGER_LEVEL_10
//    <10=> TRIGGER_LEVEL_11
//    <11=> TRIGGER_LEVEL_12
//    <12=> TRIGGER_LEVEL_13
//    <13=> TRIGGER_LEVEL_14
//    <14=> TRIGGER_LEVEL_15
//    <15=> TRIGGER_LEVEL_16
// <i> Defines I2S 1 transfer fifo trigger level.
// <i> Default: TRIGGER_LEVEL_9
#define RTE_I2S1_TX_TRIG_LVL 8

// <o> I2S1 IRQ priority <0-255>
// <i> Defines I2S 1 Interrupt priority.
// <i> Default: 0
#define RTE_I2S1_IRQ_PRI     0

// <o> I2S1 CLK SOURCE
//    <0=> CLK_76.8MHz
//    <1=> CLK_EXTERNAL
// <i> Defines I2S 1 clock source.
// <i> Default: CLK_76.8MHz
#define RTE_I2S1_CLK_SOURCE  0

// <o> I2S1 DMA ENABLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines DMA feature for I2S 1.
// <i> Default: ENABLE
#define RTE_I2S1_DMA_ENABLE  1

// <i> Defines DMA Interrupt priority for I2S 1.
#define RTE_I2S1_DMA_IRQ_PRI  RTE_I2S1_IRQ_PRI
#endif
// </e> I2S1 (Integrated Interchip Sound 1) [Driver_SAI1]


// <e> I2S2 (Integrated Interchip Sound 2) [Driver_SAI2]
// <i> Configuration settings for Driver_SAI2 in component ::Drivers:SAI
#define RTE_I2S2 1

#ifdef RTE_I2S2

// <o> I2S2 WORD SELECT SIZE
//    <0=> WSS_CLOCK_CYCLES_16
//    <1=> WSS_CLOCK_CYCLES_24
//    <2=> WSS_CLOCK_CYCLES_32
// <i> Defines I2S 2 size of word.
// <i> Default: WSS_CLOCK_CYCLES_32
#define RTE_I2S2_WSS_CLOCK_CYCLES 2

// <o> I2S2 SCLK GATING
//    <0=> NO_CLOCK_GATING
//    <1=> SCLKG_CLOCK_CYCLES_12
//    <2=> SCLKG_CLOCK_CYCLES_16
//    <3=> SCLKG_CLOCK_CYCLES_20
//    <4=> SCLKG_CLOCK_CYCLES_24
// <i> Defines I2S 2 SCLK Gating.
// <i> Default: NO_CLOCK_GATING
#define RTE_I2S2_SCLKG_CLOCK_CYCLES 0

// <o> I2S2 RX FIFO TRIGGER LEVEL
//    <0=>  TRIGGER_LEVEL_1
//    <1=>  TRIGGER_LEVEL_2
//    <2=>  TRIGGER_LEVEL_3
//    <3=>  TRIGGER_LEVEL_4
//    <4=>  TRIGGER_LEVEL_5
//    <5=>  TRIGGER_LEVEL_6
//    <6=>  TRIGGER_LEVEL_7
//    <7=>  TRIGGER_LEVEL_8
//    <8=>  TRIGGER_LEVEL_9
//    <9=>  TRIGGER_LEVEL_10
//    <10=> TRIGGER_LEVEL_11
//    <11=> TRIGGER_LEVEL_12
//    <12=> TRIGGER_LEVEL_13
//    <13=> TRIGGER_LEVEL_14
//    <14=> TRIGGER_LEVEL_15
//    <15=> TRIGGER_LEVEL_16
// <i> Defines I2S 2 receive fifo trigger level.
// <i> Default: TRIGGER_LEVEL_8
#define RTE_I2S2_RX_TRIG_LVL 7

// <o> I2S2 TX FIFO TRIGGER LEVEL
//    <0=>  TRIGGER_LEVEL_1
//    <1=>  TRIGGER_LEVEL_2
//    <2=>  TRIGGER_LEVEL_3
//    <3=>  TRIGGER_LEVEL_4
//    <4=>  TRIGGER_LEVEL_5
//    <5=>  TRIGGER_LEVEL_6
//    <6=>  TRIGGER_LEVEL_7
//    <7=>  TRIGGER_LEVEL_8
//    <8=>  TRIGGER_LEVEL_9
//    <9=>  TRIGGER_LEVEL_10
//    <10=> TRIGGER_LEVEL_11
//    <11=> TRIGGER_LEVEL_12
//    <12=> TRIGGER_LEVEL_13
//    <13=> TRIGGER_LEVEL_14
//    <14=> TRIGGER_LEVEL_15
//    <15=> TRIGGER_LEVEL_16
// <i> Defines I2S 2 transfer fifo trigger level.
// <i> Default: TRIGGER_LEVEL_9
#define RTE_I2S2_TX_TRIG_LVL 8

// <o> I2S2 IRQ priority <0-255>
// <i> Defines I2S 2 Interrupt priority.
// <i> Default: 0
#define RTE_I2S2_IRQ_PRI     10

// <o> I2S2 CLK SOURCE
//    <0=> CLK_76.8MHz
//    <1=> CLK_EXTERNAL
// <i> Defines I2S 2 clock source.
// <i> Default: CLK_76.8MHz
#define RTE_I2S2_CLK_SOURCE  0

// <o> I2S2 DMA ENABLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines DMA feature for I2S 2.
// <i> Default: ENABLE
#define RTE_I2S2_DMA_ENABLE  1

// <i> Defines DMA Interrupt priority for I2S 2.
#define RTE_I2S2_DMA_IRQ_PRI  RTE_I2S2_IRQ_PRI
#endif
// </e> I2S2 (Integrated Interchip Sound 2) [Driver_SAI2]


// <e> I2S3 (Integrated Interchip Sound 3) [Driver_SAI3]
// <i> Configuration settings for Driver_SAI3 in component ::Drivers:SAI
#define RTE_I2S3   1

#ifdef RTE_I2S3

// <o> I2S3 WORD SELECT SIZE
//    <0=> WSS_CLOCK_CYCLES_16
//    <1=> WSS_CLOCK_CYCLES_24
//    <2=> WSS_CLOCK_CYCLES_32
// <i> Defines I2S 3 size of word.
// <i> Default: WSS_CLOCK_CYCLES_32
#define RTE_I2S3_WSS_CLOCK_CYCLES 2

// <o> I2S3 SCLK GATING
//    <0=> NO_CLOCK_GATING
//    <1=> SCLKG_CLOCK_CYCLES_12
//    <2=> SCLKG_CLOCK_CYCLES_16
//    <3=> SCLKG_CLOCK_CYCLES_20
//    <4=> SCLKG_CLOCK_CYCLES_24
// <i> Defines I2S 3 SCLK Gating.
// <i> Default: NO_CLOCK_GATING
#define RTE_I2S3_SCLKG_CLOCK_CYCLES 0

// <o> I2S3 RX FIFO TRIGGER LEVEL
//    <0=>  TRIGGER_LEVEL_1
//    <1=>  TRIGGER_LEVEL_2
//    <2=>  TRIGGER_LEVEL_3
//    <3=>  TRIGGER_LEVEL_4
//    <4=>  TRIGGER_LEVEL_5
//    <5=>  TRIGGER_LEVEL_6
//    <6=>  TRIGGER_LEVEL_7
//    <7=>  TRIGGER_LEVEL_8
//    <8=>  TRIGGER_LEVEL_9
//    <9=>  TRIGGER_LEVEL_10
//    <10=> TRIGGER_LEVEL_11
//    <11=> TRIGGER_LEVEL_12
//    <12=> TRIGGER_LEVEL_13
//    <13=> TRIGGER_LEVEL_14
//    <14=> TRIGGER_LEVEL_15
//    <15=> TRIGGER_LEVEL_16
// <i> Defines I2S 3 receive fifo trigger level.
// <i> Default: TRIGGER_LEVEL_8
#define RTE_I2S3_RX_TRIG_LVL 7

// <o> I2S3 TX FIFO TRIGGER LEVEL
//    <0=>  TRIGGER_LEVEL_1
//    <1=>  TRIGGER_LEVEL_2
//    <2=>  TRIGGER_LEVEL_3
//    <3=>  TRIGGER_LEVEL_4
//    <4=>  TRIGGER_LEVEL_5
//    <5=>  TRIGGER_LEVEL_6
//    <6=>  TRIGGER_LEVEL_7
//    <7=>  TRIGGER_LEVEL_8
//    <8=>  TRIGGER_LEVEL_9
//    <9=>  TRIGGER_LEVEL_10
//    <10=> TRIGGER_LEVEL_11
//    <11=> TRIGGER_LEVEL_12
//    <12=> TRIGGER_LEVEL_13
//    <13=> TRIGGER_LEVEL_14
//    <14=> TRIGGER_LEVEL_15
//    <15=> TRIGGER_LEVEL_16
// <i> Defines I2S 3 transfer fifo trigger level.
// <i> Default: TRIGGER_LEVEL_9
#define RTE_I2S3_TX_TRIG_LVL 8

// <o> I2S3 IRQ priority <0-255>
// <i> Defines I2S 3 Interrupt priority.
// <i> Default: 1
#define RTE_I2S3_IRQ_PRI     0

// <o> I2S3 CLK SOURCE
//    <0=> CLK_76.8MHz
//    <1=> CLK_EXTERNAL
// <i> Defines I2S 3 clock source.
// <i> Default: CLK_76.8MHz
#define RTE_I2S3_CLK_SOURCE  0

// <o> I2S3 DMA ENABLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines DMA feature for I2S 3.
// <i> Default: ENABLE
#define RTE_I2S3_DMA_ENABLE  1

// <i> Defines DMA Interrupt priority for I2S 3.
#define RTE_I2S3_DMA_IRQ_PRI  RTE_I2S3_IRQ_PRI
#endif
// </e> I2S3 (Integrated Interchip Sound 3) [Driver_SAI3]
// </h> I2S (Integrated Interchip Sound)

// <h> UART (Universal asynchronous receiver transmitter)
// <e> UART0 (Universal asynchronous receiver transmitter) [Driver_USART0]
// <i> Configuration settings for Driver_USART0 in component ::Drivers:USART
#define RTE_UART0   1

#if RTE_UART0

// <o> UART0 RX FIFO TRIGGER LEVEL
//    <0=>  UART_RX_ONE_CHAR_IN_FIFO
//    <1=>  UART_RX_FIFO_QUARTER_FULL
//    <2=>  UART_RX_FIFO_HALF_FULL
//    <3=>  UART_RX_FIFO_TWO_LESS_FULL
// <i> Defines UART0 receive fifo trigger level.
// <i> Default: UART_RX_ONE_CHAR_IN_FIFO
#define RTE_UART0_RX_TRIG_LVL   0

// <o> UART0 TX FIFO TRIGGER LEVEL
//    <0=>  UART_TX_FIFO_EMPTY
//    <1=>  UART_TX_FIFO_CHAR_2
//    <2=>  UART_TX_FIFO_QUARTER_FULL
//    <3=>  UART_TX_FIFO_HALF_FULL
// <i> Defines UART0 transmit fifo trigger level.
// <i> Default: UART_TX_FIFO_EMPTY
#define RTE_UART0_TX_TRIG_LVL   0

// <o> UART0 IRQ priority <0-255>
// <i> Defines Interrupt priority for UART0.
// <i> Default: 0
#define RTE_UART0_IRQ_PRI       0

// <o> UART0 CLK SOURCE
//    <0=> CLK_38.4MHz
//    <1=> CLK_100MHz
// <i> Defines UART0 clock source.
// <i> Default: CLK_100MHz
#define RTE_UART0_CLK_SOURCE    1
#endif
// </e> UART0 (Universal asynchronous receiver transmitter) [Driver_USART0]


// <e> UART1 (Universal asynchronous receiver transmitter) [Driver_USART1]
// <i> Configuration settings for Driver_USART1 in component ::Drivers:USART
#define RTE_UART1   1

#if RTE_UART1

// <o> UART1 RX FIFO TRIGGER LEVEL
//    <0=>  UART_RX_ONE_CHAR_IN_FIFO
//    <1=>  UART_RX_FIFO_QUARTER_FULL
//    <2=>  UART_RX_FIFO_HALF_FULL
//    <3=>  UART_RX_FIFO_TWO_LESS_FULL
// <i> Defines UART1 receive fifo trigger level.
// <i> Default: UART_RX_ONE_CHAR_IN_FIFO
#define RTE_UART1_RX_TRIG_LVL   0

// <o> UART1 TX FIFO TRIGGER LEVEL
//    <0=>  UART_TX_FIFO_EMPTY
//    <1=>  UART_TX_FIFO_CHAR_2
//    <2=>  UART_TX_FIFO_QUARTER_FULL
//    <3=>  UART_TX_FIFO_HALF_FULL
// <i> Defines UART1 transmit fifo trigger level.
// <i> Default: UART_TX_FIFO_EMPTY
#define RTE_UART1_TX_TRIG_LVL   0

// <o> UART1 IRQ priority <0-255>
// <i> Defines Interrupt priority for UART1.
// <i> Default: 0
#define RTE_UART1_IRQ_PRI       0

// <o> UART1 CLK SOURCE
//    <0=> CLK_38.4MHz
//    <1=> CLK_100MHz
// <i> Defines UART1 clock source.
// <i> Default: CLK_100MHz
#define RTE_UART1_CLK_SOURCE    1
#endif
// </e> UART1 (Universal asynchronous receiver transmitter) [Driver_USART1]


// <e> UART2 (Universal asynchronous receiver transmitter) [Driver_USART2]
// <i> Configuration settings for Driver_USART2 in component ::Drivers:USART
#define RTE_UART2   1

#if RTE_UART2

// <o> UART2 RX FIFO TRIGGER LEVEL
//    <0=>  UART_RX_ONE_CHAR_IN_FIFO
//    <1=>  UART_RX_FIFO_QUARTER_FULL
//    <2=>  UART_RX_FIFO_HALF_FULL
//    <3=>  UART_RX_FIFO_TWO_LESS_FULL
// <i> Defines UART2 receive fifo trigger level.
// <i> Default: UART_RX_ONE_CHAR_IN_FIFO
#define RTE_UART2_RX_TRIG_LVL   0

// <o> UART2 TX FIFO TRIGGER LEVEL
//    <0=>  UART_TX_FIFO_EMPTY
//    <1=>  UART_TX_FIFO_CHAR_2
//    <2=>  UART_TX_FIFO_QUARTER_FULL
//    <3=>  UART_TX_FIFO_HALF_FULL
// <i> Defines UART2 transmit fifo trigger level.
// <i> Default: UART_TX_FIFO_EMPTY
#define RTE_UART2_TX_TRIG_LVL   0

// <o> UART2 IRQ priority <0-255>
// <i> Defines Interrupt priority for UART2.
// <i> Default: 0
#define RTE_UART2_IRQ_PRI       0

// <o> UART2 CLK SOURCE
//    <0=> CLK_38.4MHz
//    <1=> CLK_100MHz
// <i> Defines UART2 clock source.
// <i> Default: CLK_100MHz
#define RTE_UART2_CLK_SOURCE    1
#endif
// </e> UART2 (Universal asynchronous receiver transmitter) [Driver_USART2]


// <e> UART3 (Universal asynchronous receiver transmitter) [Driver_USART3]
// <i> Configuration settings for Driver_USART3 in component ::Drivers:USART
#define RTE_UART3   1

#if RTE_UART3

// <o> UART3 RX FIFO TRIGGER LEVEL
//    <0=>  UART_RX_ONE_CHAR_IN_FIFO
//    <1=>  UART_RX_FIFO_QUARTER_FULL
//    <2=>  UART_RX_FIFO_HALF_FULL
//    <3=>  UART_RX_FIFO_TWO_LESS_FULL
// <i> Defines UART3 receive fifo trigger level.
// <i> Default: UART_RX_ONE_CHAR_IN_FIFO
#define RTE_UART3_RX_TRIG_LVL   0

// <o> UART3 TX FIFO TRIGGER LEVEL
//    <0=>  UART_TX_FIFO_EMPTY
//    <1=>  UART_TX_FIFO_CHAR_2
//    <2=>  UART_TX_FIFO_QUARTER_FULL
//    <3=>  UART_TX_FIFO_HALF_FULL
// <i> Defines UART3 transmit fifo trigger level.
// <i> Default: UART_TX_FIFO_EMPTY
#define RTE_UART3_TX_TRIG_LVL   0

// <o> UART3 IRQ priority <0-255>
// <i> Defines Interrupt priority for UART3.
// <i> Default: 0
#define RTE_UART3_IRQ_PRI       0

// <o> UART3 CLK SOURCE
//    <0=> CLK_38.4MHz
//    <1=> CLK_100MHz
// <i> Defines UART3 clock source.
// <i> Default: CLK_100MHz
#define RTE_UART3_CLK_SOURCE    1
#endif
// </e> UART3 (Universal asynchronous receiver transmitter) [Driver_USART3]


// <e> UART4 (Universal asynchronous receiver transmitter) [Driver_USART4]
// <i> Configuration settings for Driver_USART4 in component ::Drivers:USART
#define RTE_UART4   1

#if RTE_UART4

// <o> UART4 RX FIFO TRIGGER LEVEL
//    <0=>  UART_RX_ONE_CHAR_IN_FIFO
//    <1=>  UART_RX_FIFO_QUARTER_FULL
//    <2=>  UART_RX_FIFO_HALF_FULL
//    <3=>  UART_RX_FIFO_TWO_LESS_FULL
// <i> Defines UART4 receive fifo trigger level.
// <i> Default: UART_RX_ONE_CHAR_IN_FIFO
#define RTE_UART4_RX_TRIG_LVL   0

// <o> UART4 TX FIFO TRIGGER LEVEL
//    <0=>  UART_TX_FIFO_EMPTY
//    <1=>  UART_TX_FIFO_CHAR_2
//    <2=>  UART_TX_FIFO_QUARTER_FULL
//    <3=>  UART_TX_FIFO_HALF_FULL
// <i> Defines UART4 transmit fifo trigger level.
// <i> Default: UART_TX_FIFO_EMPTY
#define RTE_UART4_TX_TRIG_LVL   0

// <o> UART4 IRQ priority <0-255>
// <i> Defines Interrupt priority for UART4.
// <i> Default: 0
#define RTE_UART4_IRQ_PRI       0

// <o> UART4 CLK SOURCE
//    <0=> CLK_38.4MHz
//    <1=> CLK_100MHz
// <i> Defines UART4 clock source.
// <i> Default: CLK_100MHz
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

// <o> UART5 RX FIFO TRIGGER LEVEL
//    <0=>  UART_RX_ONE_CHAR_IN_FIFO
//    <1=>  UART_RX_FIFO_QUARTER_FULL
//    <2=>  UART_RX_FIFO_HALF_FULL
//    <3=>  UART_RX_FIFO_TWO_LESS_FULL
// <i> Defines UART5 receive fifo trigger level.
// <i> Default: UART_RX_ONE_CHAR_IN_FIFO
#define RTE_UART5_RX_TRIG_LVL   0

// <o> UART5 TX FIFO TRIGGER LEVEL
//    <0=>  UART_TX_FIFO_EMPTY
//    <1=>  UART_TX_FIFO_CHAR_2
//    <2=>  UART_TX_FIFO_QUARTER_FULL
//    <3=>  UART_TX_FIFO_HALF_FULL
// <i> Defines UART5 transmit fifo trigger level.
// <i> Default: UART_TX_FIFO_EMPTY
#define RTE_UART5_TX_TRIG_LVL   0

// <o> UART5 IRQ priority <0-255>
// <i> Defines Interrupt priority for UART5.
// <i> Default: 0
#define RTE_UART5_IRQ_PRI       0

// <o> UART5 CLK SOURCE
//    <0=> CLK_38.4MHz
//    <1=> CLK_100MHz
// <i> Defines UART5 clock source.
// <i> Default: CLK_100MHz
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

// <o> UART6 RX FIFO TRIGGER LEVEL
//    <0=>  UART_RX_ONE_CHAR_IN_FIFO
//    <1=>  UART_RX_FIFO_QUARTER_FULL
//    <2=>  UART_RX_FIFO_HALF_FULL
//    <3=>  UART_RX_FIFO_TWO_LESS_FULL
// <i> Defines UART6 receive fifo trigger level.
// <i> Default: UART_RX_ONE_CHAR_IN_FIFO
#define RTE_UART6_RX_TRIG_LVL   0

// <o> UART6 TX FIFO TRIGGER LEVEL
//    <0=>  UART_TX_FIFO_EMPTY
//    <1=>  UART_TX_FIFO_CHAR_2
//    <2=>  UART_TX_FIFO_QUARTER_FULL
//    <3=>  UART_TX_FIFO_HALF_FULL
// <i> Defines UART6 transmit fifo trigger level.
// <i> Default: UART_TX_FIFO_EMPTY
#define RTE_UART6_TX_TRIG_LVL   0

// <o> UART6 IRQ priority <0-255>
// <i> Defines Interrupt priority for UART6.
// <i> Default: 0
#define RTE_UART6_IRQ_PRI       0

// <o> UART6 CLK SOURCE
//    <0=> CLK_38.4MHz
//    <1=> CLK_100MHz
// <i> Defines UART6 clock source.
// <i> Default: CLK_100MHz
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

// <o> UART7 RX FIFO TRIGGER LEVEL
//    <0=>  UART_RX_ONE_CHAR_IN_FIFO
//    <1=>  UART_RX_FIFO_QUARTER_FULL
//    <2=>  UART_RX_FIFO_HALF_FULL
//    <3=>  UART_RX_FIFO_TWO_LESS_FULL
// <i> Defines UART7 receive fifo trigger level.
// <i> Default: UART_RX_ONE_CHAR_IN_FIFO
#define RTE_UART7_RX_TRIG_LVL   0

// <o> UART7 TX FIFO TRIGGER LEVEL
//    <0=>  UART_TX_FIFO_EMPTY
//    <1=>  UART_TX_FIFO_CHAR_2
//    <2=>  UART_TX_FIFO_QUARTER_FULL
//    <3=>  UART_TX_FIFO_HALF_FULL
// <i> Defines UART7 transmit fifo trigger level.
// <i> Default: UART_TX_FIFO_EMPTY
#define RTE_UART7_TX_TRIG_LVL   0

// <o> UART7 IRQ priority <0-255>
// <i> Defines Interrupt priority for UART7.
// <i> Default: 0
#define RTE_UART7_IRQ_PRI       0

// <o> UART7 CLK SOURCE
//    <0=> CLK_38.4MHz
//    <1=> CLK_100MHz
// <i> Defines UART7 clock source.
// <i> Default: CLK_100MHz
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
// </h> UART (Universal asynchronous receiver transmitter)

// <h> LPTIMER (Low power timer)
// <e> LPTIMER (Low power timer) [Driver_LPTIMER]
// <i> Configuration settings for Driver_LPTIMER in component ::Drivers:LPTIMER
#define RTE_LPTIMER			    1

#if RTE_LPTIMER

// <e> LPTIMER CHANNEL 0
// <i> Configuration settings for Driver_LPTIMER channel 0 in component ::Drivers:LPTIMER
#define RTE_LPTIMER_CHANNEL0   1

// <o> CHANNEL0 IRQ priority <0-255>
// <i> Defines Interrupt priority for LPTIMER channel 0.
// <i> Default: 0
#define RTE_LPTIMER_CHANNEL0_IRQ_PRIORITY   (0)

// <o> CHANNEL0 PWM feature
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines whether PWM feature is not enabled/disabled for channel0.
// <i> Default: DISABLED
#define RTE_LPTIMER_CHANNEL0_PWM            (0)

// <o> CHANNEL0 FREE RUN feature
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines whether free run feature is not enabled/disabled for channel0.
// <i> Default: DISABLED
#define RTE_LPTIMER_CHANNEL0_FREE_RUN_MODE  (0)

// <o> CHANNEL0 clock source
//    <0=> CLK_32768Hz
//    <1=> CLK_128KHz
//    <2=> EXT_CLK
//    <3=> CLK_CASCADE
// <i> Defines clock source for channel0.
// <i> Default: CLK_32768Hz
#define RTE_LPTIMER_CHANNEL0_CLK_SRC        (0)
// </e> LPTIMER CHANNEL 0

// <e> LPTIMER CHANNEL 1
// <i> Configuration settings for Driver_LPTIMER channel 1 in component ::Drivers:LPTIMER
#define RTE_LPTIMER_CHANNEL1   1

// <o> CHANNEL1 IRQ priority <0-255>
// <i> Defines Interrupt priority for LPTIMER channel 1.
// <i> Default: 0
#define RTE_LPTIMER_CHANNEL1_IRQ_PRIORITY   (0)

// <o> CHANNEL1 PWM feature
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines whether PWM feature is not enabled/disabled for channel1.
// <i> Default: DISABLED
#define RTE_LPTIMER_CHANNEL1_PWM            (0)

// <o> CHANNEL1 FREE RUN feature
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines whether free run feature is not enabled/disabled for channel1.
// <i> Default: DISABLED
#define RTE_LPTIMER_CHANNEL1_FREE_RUN_MODE  (0)

// <o> CHANNEL1 clock source
//    <0=> CLK_32768Hz
//    <1=> CLK_128KHz
//    <2=> EXT_CLK
//    <3=> CLK_CASCADE
// <i> Defines clock source for channel1.
// <i> Default: CLK_32768Hz
#define RTE_LPTIMER_CHANNEL1_CLK_SRC        (0)
// </e> LPTIMER CHANNEL 1

// <e> LPTIMER CHANNEL 2
// <i> Configuration settings for Driver_LPTIMER channel 2 in component ::Drivers:LPTIMER
#define RTE_LPTIMER_CHANNEL2   1

// <o> CHANNEL2 IRQ priority <0-255>
// <i> Defines Interrupt priority for LPTIMER channel 2.
// <i> Default: 0
#define RTE_LPTIMER_CHANNEL2_IRQ_PRIORITY   (0)

// <o> CHANNEL2 PWM feature
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines whether PWM feature is not enabled/disabled for channel2.
// <i> Default: DISABLED
#define RTE_LPTIMER_CHANNEL2_PWM            (0)

// <o> CHANNEL2 FREE RUN feature
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines whether free run feature is not enabled/disabled for channel2.
// <i> Default: DISABLED
#define RTE_LPTIMER_CHANNEL2_FREE_RUN_MODE  (0)

// <o> CHANNEL2 clock source
//    <0=> CLK_32768Hz
//    <1=> CLK_128KHz
//    <2=> EXT_CLK
//    <3=> CLK_CASCADE
// <i> Defines clock source for channel2.
// <i> Default: CLK_32768Hz
#define RTE_LPTIMER_CHANNEL2_CLK_SRC        (0)
// </e> LPTIMER CHANNEL 2

// <e> LPTIMER CHANNEL 3
// <i> Configuration settings for Driver_LPTIMER channel 3 in component ::Drivers:LPTIMER
#define RTE_LPTIMER_CHANNEL3   1

// <o> CHANNEL3 IRQ priority <0-255>
// <i> Defines Interrupt priority for LPTIMER channel 3.
// <i> Default: 0
#define RTE_LPTIMER_CHANNEL3_IRQ_PRIORITY   (0)

// <o> CHANNEL3 PWM feature
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines whether PWM feature is not enabled/disabled for channel3.
// <i> Default: DISABLED
#define RTE_LPTIMER_CHANNEL3_PWM            (0)

// <o> CHANNEL3 FREE RUN feature
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines whether free run feature is not enabled/disabled for channel3.
// <i> Default: DISABLED
#define RTE_LPTIMER_CHANNEL3_FREE_RUN_MODE  (0)

// <o> CHANNEL3 clock source
//    <0=> CLK_32768Hz
//    <1=> CLK_128KHz
//    <2=> EXT_CLK
//    <3=> CLK_CASCADE
// <i> Defines clock source for channel3.
// <i> Default: CLK_32768Hz
#define RTE_LPTIMER_CHANNEL3_CLK_SRC        (0)
// </e> LPTIMER CHANNEL 3
#endif /* RTE_LPTIMER */
// </e> LPTIMER (Low power timer) [Driver_LPTIMER]
// </h> LPTIMER (Low power timer)


// <h> RTC (Real time clock)
// <e> RTC0 (Real time clock) [Driver_RTC0]
// <i> Configuration settings for Driver_RTC0 in component ::Drivers:RTC
#define RTE_RTC0                1

#if RTE_RTC0

// <o> RTC0 PRESCALER VALUE
// <i> Defines prescaler value for RTC0.
// <i> Default: 32768
#define RTE_RTC0_DEFAULT_PRESCALER_VALUE    32768

// <o> RTC0 IRQ priority <0-255>
// <i> Defines Interrupt priority for RTC0.
// <i> Default: 0
#define RTE_RTC0_IRQ_PRI                    0
#endif
// </e> RTC0 (Real time clock) [Driver_RTC0]
// </h> RTC (Real time clock)


// <h> WDT (Watchdog Timer)
// <e> WDT0 (Watchdog Timer) [Driver_WDT0]
// <i> Configuration settings for Driver_WDT0 in component ::Drivers:WDT
#define RTE_WDT0                1
// </e> WDT0 (Watchdog Timer) [Driver_WDT0]
// </h> WDT (Watchdog Timer)


// <h> UTIMER (Universal Timer)
// <e> UTIMER (Universal Timer) [Driver_UTIMER]
// <i> Configuration settings for Driver_UTIMER in component ::Drivers:UTIMER
#define RTE_UTIMER   1

#if RTE_UTIMER

// <e> UTIMER CHANNEL 0
// <i> Configuration settings for UTIMER channel 0 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL0   1

// <o> CHANNEL0 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 0.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL0_FIXED_BUFFER            0

// <o> CHANNEL0 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 0.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL0_BUF_TROUGH_N_CREST      4

// <o> CHANNEL0 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 0.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL0_DRIVER_A                0

// <o> CHANNEL0 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 0.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL0_DRIVER_B                0

// <o> CHANNEL0 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 0.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL0_CMP_START_STATE         0

// <o> CHANNEL0 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 0.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL0_CMP_STOP_STATE          0

// <o> CHANNEL0 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 0.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL0_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL0 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 0.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL0_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL0 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 0.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL0_EVENT_AT_CREST          1

// <o> CHANNEL0 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 0.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL0_EVENT_AT_TROUGH         0

// <o> CHANNEL0 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 0.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL0_BUFFERING_TYPE          1

// <o> CHANNEL0 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 0.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL0_BUFFER_OPERATION        1

// <o> CHANNEL0 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 0 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL0_BUFFERING_TYPE_A        2

// <o> CHANNEL0 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 0 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL0_BUFFERING_TYPE_B        0

// <o> CHANNEL0 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 0 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL0_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL0 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 0 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL0_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL0 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 0 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL0_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL0 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 0 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL0_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL0 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 0 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL0_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL0 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 0 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL0_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL0 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 0 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL0_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL0 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 0 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL0_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL0 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 0.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL0_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL0 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 0.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL0_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 0


// <e> UTIMER CHANNEL 1
// <i> Configuration settings for UTIMER channel 1 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL1   1

// <o> CHANNEL1 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 1.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL1_FIXED_BUFFER            0

// <o> CHANNEL1 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 1.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL1_BUF_TROUGH_N_CREST      4

// <o> CHANNEL1 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 1.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL1_DRIVER_A                0

// <o> CHANNEL1 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 1.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL1_DRIVER_B                0

// <o> CHANNEL1 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 1.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL1_CMP_START_STATE         0

// <o> CHANNEL1 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 1.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL1_CMP_STOP_STATE          0

// <o> CHANNEL1 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 1.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL1_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL1 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 1.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL1_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL1 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 1.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL1_EVENT_AT_CREST          1

// <o> CHANNEL1 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 1.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL1_EVENT_AT_TROUGH         0

// <o> CHANNEL1 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 1.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL1_BUFFERING_TYPE          1

// <o> CHANNEL1 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 1.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL1_BUFFER_OPERATION        1

// <o> CHANNEL1 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 1 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL1_BUFFERING_TYPE_A        2

// <o> CHANNEL1 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 1 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL1_BUFFERING_TYPE_B        0

// <o> CHANNEL1 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 1 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL1_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL1 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 1 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL1_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL1 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 1 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL1_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL1 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 1 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL1_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL1 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 1 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL1_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL1 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 1 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL1_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL1 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 1 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL1_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL1 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 1 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL1_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL1 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 1.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL1_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL1 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 1.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL1_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 1


// <e> UTIMER CHANNEL 2
// <i> Configuration settings for UTIMER channel 2 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL2   1

// <o> CHANNEL2 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 2.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL2_FIXED_BUFFER            0

// <o> CHANNEL2 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 2.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL2_BUF_TROUGH_N_CREST      4

// <o> CHANNEL2 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 2.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL2_DRIVER_A                0

// <o> CHANNEL2 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 2.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL2_DRIVER_B                0

// <o> CHANNEL2 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 2.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL2_CMP_START_STATE         0

// <o> CHANNEL2 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 2.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL2_CMP_STOP_STATE          0

// <o> CHANNEL2 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 2.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL2_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL2 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 2.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL2_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL2 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 2.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL2_EVENT_AT_CREST          1

// <o> CHANNEL2 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 2.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL2_EVENT_AT_TROUGH         0

// <o> CHANNEL2 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 2.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL2_BUFFERING_TYPE          1

// <o> CHANNEL2 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 2.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL2_BUFFER_OPERATION        1

// <o> CHANNEL2 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 2 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL2_BUFFERING_TYPE_A        2

// <o> CHANNEL2 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 2 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL2_BUFFERING_TYPE_B        0

// <o> CHANNEL2 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 2 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL2_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL2 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 2 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL2_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL2 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 2 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL2_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL2 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 2 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL2_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL2 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel2 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL2_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL2 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 2 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL2_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL2 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 2 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL2_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL2 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 2 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL2_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL2 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 2.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL2_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL2 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 2.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL2_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 2


// <e> UTIMER CHANNEL 3
// <i> Configuration settings for UTIMER channel 3 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL3   1

// <o> CHANNEL3 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 3.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL3_FIXED_BUFFER            0

// <o> CHANNEL3 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 3.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL3_BUF_TROUGH_N_CREST      4

// <o> CHANNEL3 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 3.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL3_DRIVER_A                0

// <o> CHANNEL3 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 3.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL3_DRIVER_B                0

// <o> CHANNEL3 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 3.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL3_CMP_START_STATE         0

// <o> CHANNEL3 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 3.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL3_CMP_STOP_STATE          0

// <o> CHANNEL3 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 3.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL3_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL3 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 3.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL3_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL3 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 3.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL3_EVENT_AT_CREST          1

// <o> CHANNEL3 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 3.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL3_EVENT_AT_TROUGH         0

// <o> CHANNEL3 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 3.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL3_BUFFERING_TYPE          1

// <o> CHANNEL3 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 3.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL3_BUFFER_OPERATION        1

// <o> CHANNEL3 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 3 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL3_BUFFERING_TYPE_A        2

// <o> CHANNEL3 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 3 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL3_BUFFERING_TYPE_B        0

// <o> CHANNEL3 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 3 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL3_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL3 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 3 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL3_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL3 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 3 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL3_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL3 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 3 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL3_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL3 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 3 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL3_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL3 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 3 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL3_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL3 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 3 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL3_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL3 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 3 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL3_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL3 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 3.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL3_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL3 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 3.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL3_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 3


// <e> UTIMER CHANNEL 4
// <i> Configuration settings for UTIMER channel 4 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL4   1

// <o> CHANNEL4 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 4.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL4_FIXED_BUFFER            0

// <o> CHANNEL4 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 4.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL4_BUF_TROUGH_N_CREST      4

// <o> CHANNEL4 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 4.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL4_DRIVER_A                1

// <o> CHANNEL4 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 4.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL4_DRIVER_B                0

// <o> CHANNEL4 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 4.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL4_CMP_START_STATE         0

// <o> CHANNEL4 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 4.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL4_CMP_STOP_STATE          0

// <o> CHANNEL4 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 4.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL4_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL4 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 4.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL4_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL4 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 4.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL4_EVENT_AT_CREST          1

// <o> CHANNEL4 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 4.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL4_EVENT_AT_TROUGH         0

// <o> CHANNEL4 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 4.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL4_BUFFERING_TYPE          1

// <o> CHANNEL4 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 4.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL4_BUFFER_OPERATION        1

// <o> CHANNEL4 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 4 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL4_BUFFERING_TYPE_A        2

// <o> CHANNEL4 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 4 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL4_BUFFERING_TYPE_B        0

// <o> CHANNEL4 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 4 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL4_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL4 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 4 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL4_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL4 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 4 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL4_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL4 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 4 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL4_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL4 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 4 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL4_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL4 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 4 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL4_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL4 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 4 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL4_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL4 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 4 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL4_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL4 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 4.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL4_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL4 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 4.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL4_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 4


// <e> UTIMER CHANNEL 5
// <i> Configuration settings for UTIMER channel 5 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL5   1


// <o> CHANNEL5 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 5.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL5_FIXED_BUFFER            0

// <o> CHANNEL5 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 5.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL5_BUF_TROUGH_N_CREST      4

// <o> CHANNEL5 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 5.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL5_DRIVER_A                1

// <o> CHANNEL5 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 5.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL5_DRIVER_B                0

// <o> CHANNEL5 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 5.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL5_CMP_START_STATE         1

// <o> CHANNEL5 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 5.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL5_CMP_STOP_STATE          0

// <o> CHANNEL5 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 5.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL5_DRV_OP_AT_MATCH_COUNT   3

// <o> CHANNEL5 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 5.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL5_DRV_OP_AT_CYCLE_END     3

// <o> CHANNEL5 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 5.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL5_EVENT_AT_CREST          1

// <o> CHANNEL5 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 5.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL5_EVENT_AT_TROUGH         0

// <o> CHANNEL5 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 5.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL5_BUFFERING_TYPE          1

// <o> CHANNEL5 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 5.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL5_BUFFER_OPERATION        1

// <o> CHANNEL5 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 5 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL5_BUFFERING_TYPE_A        2

// <o> CHANNEL5 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 5 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL5_BUFFERING_TYPE_B        0

// <o> CHANNEL5 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 5 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL5_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL5 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 5 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL5_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL5 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 5 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL5_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL5 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 5 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL5_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL5 CAPTURE E IRQ priority <0-255 >
// <i> Defines Interrupt priority for channel 5 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL5_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL5 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 5 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL5_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL5 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 5 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL5_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL5 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 5 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL5_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL5 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 5.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL5_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL5 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 5.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL5_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 5


// <e> UTIMER CHANNEL 6
// <i> Configuration settings for UTIMER channel 6 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL6   1

// <o> CHANNEL6 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 6.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL6_FIXED_BUFFER            0

// <o> CHANNEL6 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 6.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL6_BUF_TROUGH_N_CREST      4

// <o> CHANNEL6 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 6.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL6_DRIVER_A                1

// <o> CHANNEL6 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 6.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL6_DRIVER_B                1

// <o> CHANNEL6 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 6.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL6_CMP_START_STATE         0

// <o> CHANNEL6 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 6.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL6_CMP_STOP_STATE          0

// <o> CHANNEL6 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 6.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL6_DRV_OP_AT_MATCH_COUNT   3

// <o> CHANNEL6 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 6.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL6_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL6 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 6.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL6_EVENT_AT_CREST          1

// <o> CHANNEL6 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 6.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL6_EVENT_AT_TROUGH         0

// <o> CHANNEL6 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 6.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE          1

// <o> CHANNEL6 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 6.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL6_BUFFER_OPERATION        1

// <o> CHANNEL6 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 6 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE_A        0

// <o> CHANNEL6 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 6 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL6_BUFFERING_TYPE_B        0

// <o> CHANNEL6 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 6 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL6_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL6 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 6 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL6_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL6 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 6 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL6_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL6 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 6 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL6_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL6 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 6 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL6_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL6 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 6 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL6_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL6 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 6 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL6_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL6 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 6 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL6_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL6 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 6.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL6_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL6 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 6.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL6_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 6


// <e> UTIMER CHANNEL 7
// <i> Configuration settings for UTIMER channel 7 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL7   1

// <o> CHANNEL7 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 7.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL7_FIXED_BUFFER            0

// <o> CHANNEL7 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 7.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL7_BUF_TROUGH_N_CREST      4

// <o> CHANNEL7 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 7.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL7_DRIVER_A                0

// <o> CHANNEL7 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 7.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL7_DRIVER_B                0

// <o> CHANNEL7 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 7.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL7_CMP_START_STATE         0

// <o> CHANNEL7 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 7.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL7_CMP_STOP_STATE          0

// <o> CHANNEL7 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 7.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL7_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL7 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 7.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL7_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL7 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 7.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL7_EVENT_AT_CREST          1

// <o> CHANNEL7 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 7.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL7_EVENT_AT_TROUGH         0

// <o> CHANNEL7 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 7.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL7_BUFFERING_TYPE          1

// <o> CHANNEL7 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 7.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL7_BUFFER_OPERATION        1

// <o> CHANNEL7 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 7 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL7_BUFFERING_TYPE_A        2

// <o> CHANNEL7 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 7 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL7_BUFFERING_TYPE_B        0

// <o> CHANNEL7 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 7 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL7_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL7 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 7 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL7_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL7 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 7 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL7_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL7 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 7 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL7_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL7 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 7 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL7_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL7 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 7 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL7_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL7 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 7 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL7_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL7 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 7 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL7_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL7 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 7.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL7_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL7 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 7.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL7_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 7


// <e> UTIMER CHANNEL 8
// <i> Configuration settings for UTIMER channel 8 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL8   1

// <o> CHANNEL8 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 8.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL8_FIXED_BUFFER            0

// <o> CHANNEL8 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 8.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL8_BUF_TROUGH_N_CREST      4

// <o> CHANNEL8 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 8.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL8_DRIVER_A                0

// <o> CHANNEL8 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 8.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL8_DRIVER_B                0

// <o> CHANNEL8 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 8.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL8_CMP_START_STATE         0

// <o> CHANNEL8 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 8.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL8_CMP_STOP_STATE          0

// <o> CHANNEL8 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 8.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL8_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL8 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 8.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL8_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL8 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 8.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL8_EVENT_AT_CREST          1

// <o> CHANNEL8 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 8.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL8_EVENT_AT_TROUGH         0

// <o> CHANNEL8 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 8.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL8_BUFFERING_TYPE          1

// <o> CHANNEL8 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 8.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL8_BUFFER_OPERATION        1

// <o> CHANNEL8 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 8 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL8_BUFFERING_TYPE_A        2

// <o> CHANNEL8 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 8 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL8_BUFFERING_TYPE_B        0

// <o> CHANNEL8 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 8 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL8_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL8 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 8 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL8_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL8 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 8 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL8_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL8 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 8 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL8_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL8 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 8 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL8_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL8 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 8 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL8_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL8 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 8 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL8_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL8 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 8 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL8_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL8 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 8.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL8_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL8 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 8.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL8_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 8


// <e> UTIMER CHANNEL 9
// <i> Configuration settings for UTIMER channel 9 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL9   1

// <o> CHANNEL9 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 9.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL9_FIXED_BUFFER            0

// <o> CHANNEL9 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 9.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL9_BUF_TROUGH_N_CREST      4

// <o> CHANNEL9 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 9.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL9_DRIVER_A                0

// <o> CHANNEL9 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 9.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL9_DRIVER_B                0

// <o> CHANNEL9 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 9.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL9_CMP_START_STATE         0

// <o> CHANNEL9 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 9.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL9_CMP_STOP_STATE          0

// <o> CHANNEL9 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 9.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL9_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL9 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 9.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL9_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL9 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 9.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL9_EVENT_AT_CREST          1

// <o> CHANNEL9 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 9.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL9_EVENT_AT_TROUGH         0

// <o> CHANNEL9 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 9.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL9_BUFFERING_TYPE          1

// <o> CHANNEL9 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 9.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL9_BUFFER_OPERATION        1

// <o> CHANNEL9 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 9 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL9_BUFFERING_TYPE_A        2

// <o> CHANNEL9 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 9 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL9_BUFFERING_TYPE_B        0

// <o> CHANNEL9 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 9 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL9_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL9 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 9 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL9_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL9 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 9 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL9_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL9 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 9 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL9_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL9 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 9 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL9_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL9 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 9 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL9_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL9 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 9 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL9_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL9 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 9 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL9_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL9 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 9.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL9_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL9 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 9.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL9_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 9


// <e> UTIMER CHANNEL 10
// <i> Configuration settings for UTIMER channel 10 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL10   1

// <o> CHANNEL10 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 10.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL10_FIXED_BUFFER            0

// <o> CHANNEL10 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 10.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL10_BUF_TROUGH_N_CREST      4

// <o> CHANNEL10 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 10.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL10_DRIVER_A                0

// <o> CHANNEL10 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 10.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL10_DRIVER_B                0

// <o> CHANNEL10 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 10.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL10_CMP_START_STATE         0

// <o> CHANNEL10 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 10.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL10_CMP_STOP_STATE          0

// <o> CHANNEL10 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 10.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL10_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL10 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 10.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL10_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL10 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 10.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL10_EVENT_AT_CREST          1

// <o> CHANNEL10 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 10.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL10_EVENT_AT_TROUGH         0

// <o> CHANNEL10 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 10.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL10_BUFFERING_TYPE          1

// <o> CHANNEL10 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 10.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL10_BUFFER_OPERATION        1

// <o> CHANNEL10 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 10 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL10_BUFFERING_TYPE_A        2

// <o> CHANNEL10 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 10 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL10_BUFFERING_TYPE_B        0

// <o> CHANNEL10 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 10 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL10_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL10 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 10 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL10_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL10 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 10 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL10_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL10 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 10 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL10_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL10 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 10 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL10_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL10 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 10 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL10_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL10 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 10 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL10_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL10 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 10 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL10_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL10 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 10.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL10_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL10 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 10.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL10_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 10


// <e> UTIMER CHANNEL 11
// <i> Configuration settings for UTIMER channel 11 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL11   1

// <o> CHANNEL11 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 11.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL11_FIXED_BUFFER            0

// <o> CHANNEL11 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 11.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL11_BUF_TROUGH_N_CREST      4

// <o> CHANNEL11 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 11.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL11_DRIVER_A                0

// <o> CHANNEL11 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 11.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL11_DRIVER_B                0

// <o> CHANNEL11 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 11.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL11_CMP_START_STATE         0

// <o> CHANNEL11 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 11.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL11_CMP_STOP_STATE          0

// <o> CHANNEL11 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 11.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL11_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL11 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 11.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL11_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL11 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 11.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL11_EVENT_AT_CREST          1

// <o> CHANNEL11 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 11.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL11_EVENT_AT_TROUGH         0

// <o> CHANNEL11 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 11.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL11_BUFFERING_TYPE          1

// <o> CHANNEL11 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 11.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL11_BUFFER_OPERATION        1

// <o> CHANNEL11 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 11 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL11_BUFFERING_TYPE_A        2

// <o> CHANNEL11 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 11 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL11_BUFFERING_TYPE_B        0

// <o> CHANNEL11 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 11 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL11_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL11 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 11 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL11_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL11 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 11 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL11_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL11 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 11 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL11_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL11 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 11 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL11_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL11 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 11 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL11_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL11 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 11 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL11_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL11 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 11 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL11_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL11 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 11.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL11_DUTY_CYCLE_ENABLE       0
// </e> UTIMER CHANNEL 11


// <e> UTIMER CHANNEL 12
// <i> Configuration settings for UTIMER channel 12 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL12   1

// <o> CHANNEL11 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 11.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL11_DUTY_CYCLE_VALUE        0

// <o> CHANNEL12 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 12.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL12_FIXED_BUFFER            0

// <o> CHANNEL12 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 12.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL12_BUF_TROUGH_N_CREST      4

// <o> CHANNEL12 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 12.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL12_DRIVER_A                1

// <o> CHANNEL12 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 12.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL12_DRIVER_B                0

// <o> CHANNEL12 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 12.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL12_CMP_START_STATE         0

// <o> CHANNEL12 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 12.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL12_CMP_STOP_STATE          0

// <o> CHANNEL12 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 12.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL12_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL12 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 12.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL12_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL12 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 12.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL12_EVENT_AT_CREST          1

// <o> CHANNEL12 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 12.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL12_EVENT_AT_TROUGH         0

// <o> CHANNEL12 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 12.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL12_BUFFERING_TYPE          0

// <o> CHANNEL12 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 12.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL12_BUFFER_OPERATION        0

// <o> CHANNEL12 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 12 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL12_BUFFERING_TYPE_A        0

// <o> CHANNEL12 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 12 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL12_BUFFERING_TYPE_B        0

// <o> CHANNEL12 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 12 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL12_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL12 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 12 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL12_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL12 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 12 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL12_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL12 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 12 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL12_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL12 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 12 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL12_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL12 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 12 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL12_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL12 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 12 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL12_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL12 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 12 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL12_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL12 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 12.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL12_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL12 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 12.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL12_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 12


// <e> UTIMER CHANNEL 13
// <i> Configuration settings for UTIMER channel 13 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL13   1

// <o> CHANNEL13 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 13.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL13_FIXED_BUFFER            0

// <o> CHANNEL13 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 13.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL13_BUF_TROUGH_N_CREST      4

// <o> CHANNEL13 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 13.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL13_DRIVER_A                1

// <o> CHANNEL13 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 13.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL13_DRIVER_B                0

// <o> CHANNEL13 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 13.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL13_CMP_START_STATE         0

// <o> CHANNEL13 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 13.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL13_CMP_STOP_STATE          0

// <o> CHANNEL13 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 13.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL13_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL13 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 13.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL13_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL13 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 13.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL13_EVENT_AT_CREST          1

// <o> CHANNEL13 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 13.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL13_EVENT_AT_TROUGH         0

// <o> CHANNEL13 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 13.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL13_BUFFERING_TYPE          0

// <o> CHANNEL13 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 13.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL13_BUFFER_OPERATION        0

// <o> CHANNEL13 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 13 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL13_BUFFERING_TYPE_A        0

// <o> CHANNEL13 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 13 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL13_BUFFERING_TYPE_B        0

// <o> CHANNEL13 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 13 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL13_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL13 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 13 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL13_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL13 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 13 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL13_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL13 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 13 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL13_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL13 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 13 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL13_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL13 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 13 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL13_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL13 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 13 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL13_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL13 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 13 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL13_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL13 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 13.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL13_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL13 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 13.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL13_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 13


// <e> UTIMER CHANNEL 14
// <i> Configuration settings for UTIMER channel 14 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL14   1

// <o> CHANNEL14 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 14.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL14_FIXED_BUFFER            0

// <o> CHANNEL14 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 14.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL14_BUF_TROUGH_N_CREST      4

// <o> CHANNEL14 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 14.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL14_DRIVER_A                1

// <o> CHANNEL14 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 14.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL14_DRIVER_B                0

// <o> CHANNEL14 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 14.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL14_CMP_START_STATE         0

// <o> CHANNEL14 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 14.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL14_CMP_STOP_STATE          0

// <o> CHANNEL14 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 14.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL14_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL14 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 14.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL14_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL14 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 14.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL14_EVENT_AT_CREST          1

// <o> CHANNEL14 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 14.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL14_EVENT_AT_TROUGH         0

// <o> CHANNEL14 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 14.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL14_BUFFERING_TYPE          0

// <o> CHANNEL14 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 14.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL14_BUFFER_OPERATION        0

// <o> CHANNEL14 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 14 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL14_BUFFERING_TYPE_A        0

// <o> CHANNEL14 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 14 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL14_BUFFERING_TYPE_B        0

// <o> CHANNEL14 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 14 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL14_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL14 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 14 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL14_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL14 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 14 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL14_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL14 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 14 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL14_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL14 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 14 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL14_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL14 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 14 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL14_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL14 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 14 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL14_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL14 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 14 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL14_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL14 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 14.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL14_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL14 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 14.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL14_DUTY_CYCLE_VALUE        0
// </e> UTIMER CHANNEL 14


// <e> UTIMER CHANNEL 15
// <i> Configuration settings for UTIMER channel 15 in component ::Drivers:UTIMER
#define RTE_UTIMER_CHANNEL15   1

// <o> CHANNEL15 FIXED BUFFER
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Fixed Buffer features for UT channel 15.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL15_FIXED_BUFFER            0

// <o> CHANNEL15 BUFFER EVENT
//    <4=> BUF_AT_TROUGH
//    <5=> BUF_AT_TROUGH_N_CREST
//    <6=> TRIANGLE_FIXED_BUF
// <i> Defines Fixed Buffer features for UT channel 15.
// <i> Default: BUF_AT_TROUGH
#define RTE_UTIMER_CHANNEL15_BUF_TROUGH_N_CREST      4

// <o> CHANNEL15 DRIVER A
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver A state for UT channel 15.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL15_DRIVER_A                1

// <o> CHANNEL15 DRIVER B
//    <0=> DISABLED
//    <1=> ENABLED
// <i> Defines Driver B state for UT channel 15.
// <i> Default: DISABLED
#define RTE_UTIMER_CHANNEL15_DRIVER_B                0

// <o> CHANNEL15 DRIVER START STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver start state for UT channel 15.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL15_CMP_START_STATE         0

// <o> CHANNEL15 DRIVER STOP STATE
//    <0=> LOW
//    <1=> HIGH
// <i> Defines Driver stop state for UT channel 15.
// <i> Default: LOW
#define RTE_UTIMER_CHANNEL15_CMP_STOP_STATE          0

// <o> CHANNEL15 DRIVER AT COMP MATCH
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at compare match for UT channel 15.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL15_DRV_OP_AT_MATCH_COUNT   0

// <o> CHANNEL15 DRIVER AT CYCLE END
//    <0=> NO CHANGE
//    <1=> LOW
//    <2=> HIGH
//    <3=> TOGGLE
// <i> Defines Driver state at cycle end for UT channel 15.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL15_DRV_OP_AT_CYCLE_END     0

// <o> CHANNEL15 BUFFER TRANSFER AT CREST
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at crest for UT channel 15.
// <i> Default: NO CHANGE
#define RTE_UTIMER_CHANNEL15_EVENT_AT_CREST          1

// <o> CHANNEL15 BUFFER TRANSFER AT TROUGH
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer transfer at trough for UT channel 15.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL15_EVENT_AT_TROUGH         0

// <o> CHANNEL15 BUFFERING TYPE
//    <0=> SINGLE BUFFER
//    <1=> DOUBLE BUFFER
// <i> Defines buffer type for UT channel 15.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL15_BUFFERING_TYPE          0

// <o> CHANNEL15 BUFFER OPERATION
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines buffer operation for UT channel 15.
// <i> Default: ENABLE
#define RTE_UTIMER_CHANNEL15_BUFFER_OPERATION        0

// <o> CHANNEL15 CAPTURE BUFFER OPERATION FOR DRIVER A
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 15 capture mode.
// <i> Default: DOUBLE BUFFER
#define RTE_UTIMER_CHANNEL15_BUFFERING_TYPE_A        0

// <o> CHANNEL15 CAPTURE BUFFER OPERATION FOR DRIVER B
//    <0=> NO BUFFER
//    <1=> SINGLE BUFFER
//    <2=> DOUBLE BUFFER
// <i> Defines buffer operation for UT channel 15 capture mode.
// <i> Default: NO BUFFER
#define RTE_UTIMER_CHANNEL15_BUFFERING_TYPE_B        0

// <o> CHANNEL15 CAPTURE A IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 15 capture A.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL15_CAPTURE_A_IRQ_PRIORITY  0

// <o> CHANNEL15 CAPTURE B IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 15 capture B.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL15_CAPTURE_B_IRQ_PRIORITY  0

// <o> CHANNEL15 CAPTURE C IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 15 capture C.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL15_CAPTURE_C_IRQ_PRIORITY  0

// <o> CHANNEL15 CAPTURE D IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 15 capture D.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL15_CAPTURE_D_IRQ_PRIORITY  0

// <o> CHANNEL15 CAPTURE E IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 15 capture E.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL15_CAPTURE_E_IRQ_PRIORITY  0

// <o> CHANNEL15 CAPTURE F IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 15 capture F.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL15_CAPTURE_F_IRQ_PRIORITY  0

// <o> CHANNEL15 OVERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 15 overflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL15_OVER_FLOW_IRQ_PRIORITY  0

// <o> CHANNEL15 UNDERFLOW IRQ priority <0-255>
// <i> Defines Interrupt priority for channel 15 underflow interrupt.
// <i> Default: 0
#define RTE_UTIMER_CHANNEL15_UNDER_FLOW_IRQ_PRIORITY 0

// <o> CHANNEL15 DUTY CYCLE
//    <0=> DISABLE
//    <1=> ENABLE
// <i> Defines duty cycle for UT channel 15.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL15_DUTY_CYCLE_ENABLE       0

// <o> CHANNEL15 DUTY CYCLE VALUE
//    <0=> COMP MATCH
//    <1=> COMP MATCH
//    <2=> 0 PERCENT
//    <3=> 100 PERCENT
// <i> Defines duty cycle value for UT channel 15.
// <i> Default: DISABLE
#define RTE_UTIMER_CHANNEL15_DUTY_CYCLE_VALUE        0
// </e>  UTIMER CHANNEL 15
#endif /*RTE_UTIMER*/
// </e> UTIMER (Universal timer) [Driver_UTIMER]
// </h> UTIMER (Universal Timer)


// <e> Analog configuration [vbat analog register2 and comparator register2]
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

// <h> ADC (Analog to Digital Converter)
// <e> ADC0 (Analog to Digital Converter 0) [Driver_ADC0]
// <i> Configuration settings for Driver_ADC0 in component ::Drivers:ADC
#define RTE_ADC0     1
#if RTE_ADC0
#define RTE_ADC0_IRQ_PRIORITY        0

// <o> Input channel number <0-8>
// <i> Defines select input channel number.
// <i> Default: 0
#define RTE_ADC0_INPUT_NUM               (0)

// <o> Clock Divider <2-64>
// <i> Defines Clock divider applied to main clock
// <i> Default: 2
#define RTE_ADC0_CLOCK_DIV               (2)

// <o> Sample Width <2-32>
// <i> Defines Width in divided clocks of the sample output signal
// <i> Default: 16
#define RTE_ADC0_SAMPLE_WIDTH            (16)

// <o> Number of average sample <0-256>
// <i> Defines Number of sample to be taken and it must be power of 2
// <i> Default: 256
#define RTE_ADC0_AVG_SAMPLE_NUM          (256)

// <o> Number of bits to shift <0-8>
// <i> Defines How much bit to shift before storing in sample register.
// <i> Default: 8
#define RTE_ADC0_SHIFT_N_BIT             (8)

// <o> Left or Right shift
//     <0=> LEFT SHIFT
//     <1=> RIGHT SHIFT
// <i> Defines shift averaged value before loading in sample register.
// <i> Default: RIGHT SHIFT
#define RTE_ADC0_SHIFT_LEFT_OR_RIGHT     (1)

// <o> Test Signal enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines Enable test signal to go to the test MUX: if "1" otherwise off
// <i> Default: DISABLE
#define RTE_ADC0_TEST_EN                  0

// <o> Differential mode enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines Enable differential Mode: 1 ==> in1/2, in3/4, in5/6 will be differential  other wise all single ended
// <i> Default: DISABLE
#define RTE_ADC0_DIFFERENTIAL_EN          0

// <o> Comparator mode enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines set 1 to ENABLE the comparator otherwise comparator "DISABLE"
// <i> Default: ENABLE
#define RTE_ADC0_COMPARATOR_EN            1

// <o> Comparator bias enable
//     <0=> 0.5MS/s
//     <1=> 1MS/s
//     <2=> 2.5MS/s
//     <3=> 5MS/s
// <i> Defines: "11":5MS/s; "10""2.5MS/s; "01":1MS/s;"00":0.5MS/s
// <i> Default: ENABLE
#define RTE_ADC0_COMPARATOR_BIAS          2

// <o> Resistive divider enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines "0": the resistive divider is off, common mode is not well defined,    "1": the resistive divider is on, common mode is VDD/2
// <i> Default: ENABLE
#define RTE_ADC0_VCM_RDIV_EN              1

#endif
// </e> ADC0 (Analog to Digital Converter 0) [Driver_ADC0]

// <e> ADC1 (Analog to Digital Converter 1) [Driver_ADC1]
// <i> Configuration settings for Driver_ADC1 in component ::Drivers:ADC
#define RTE_ADC1   1
#if RTE_ADC1

// <o> Input channel number <0-8>
// <i> Defines select input channel number.
// <i> Default: 0
#define RTE_ADC1_INPUT_NUM               (0)

// <o> Clock Divider <2-64>
// <i> Defines Clock divider applied to main clock
// <i> Default: 2
#define RTE_ADC1_CLOCK_DIV               (2)

// <o> Sample Width <2-32>
// <i> Defines Width in divided clocks of the sample output signal
// <i> Default: 16
#define RTE_ADC1_SAMPLE_WIDTH            (16)

// <o> Number of average sample <0-256>
// <i> Defines Number of sample to be taken and it must be power of 2
// <i> Default: 256
#define RTE_ADC1_AVG_SAMPLE_NUM          (256)

// <o> Number of bits to shift <0-8>
// <i> Defines How much bit to shift before storing in sample register.
// <i> Default: 8
#define RTE_ADC1_SHIFT_N_BIT             (8)

// <o> Left or Right shift
//     <0=> LEFT SHIFT
//     <1=> RIGHT SHIFT
// <i> Defines shift averaged value before loading in sample register.
// <i> Default: RIGHT SHIFT
#define RTE_ADC1_SHIFT_LEFT_OR_RIGHT     (1)

// <o> Test Signal enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines Enable test signal to go to the test MUX: if "1" otherwise off
// <i> Default: DISABLE
#define RTE_ADC1_TEST_EN                  0

// <o> Differential mode enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines Enable differential Mode: 1 ==> in1/2, in3/4, in5/6 will be differential  other wise all single ended
// <i> Default: DISABLE
#define RTE_ADC1_DIFFERENTIAL_EN          0

// <o> Comparator mode enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines set 1 to ENABLE the comparator otherwise comparator "DISABLE"
// <i> Default: ENABLE
#define RTE_ADC1_COMPARATOR_EN            1

// <o> Comparator bias enable
//     <0=> 0.5MS/s
//     <1=> 1MS/s
//     <2=> 2.5MS/s
//     <3=> 5MS/s
// <i> Defines: "11":5MS/s; "10""2.5MS/s; "01":1MS/s;"00":0.5MS/s
// <i> Default: ENABLE
#define RTE_ADC1_COMPARATOR_BIAS          2

// <o> Resistive divider enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines "0": the resistive divider is off, common mode is not well defined,    "1": the resistive divider is on, common mode is VDD/2
// <i> Default: ENABLE
#define RTE_ADC1_VCM_RDIV_EN              1

#endif
// </e> ADC1 (Analog to Digital Converter 1) [Driver_ADC1]

// <e> ADC2 (Analog to Digital Converter 2) [Driver_ADC2]
// <i> Configuration settings for Driver_ADC2 in component ::Drivers:ADC
#define RTE_ADC2   1
#if RTE_ADC2

// <o> Input channel number <0-8>
// <i> Defines select input channel number.
// <i> Default: 0
#define RTE_ADC2_INPUT_NUM               (0)

// <o> Clock Divider <2-64>
// <i> Defines Clock divider applied to main clock
// <i> Default: 2
#define RTE_ADC2_CLOCK_DIV               (2)

// <o> Sample Width <2-32>
// <i> defines Width in divided clocks of the sample output signal
// <i> default: 16
#define RTE_ADC2_SAMPLE_WIDTH            (16)

// <o> Number of average sample <0-256>
// <i> Defines Number of sample to be taken and it must be power of 2
// <i> Default: 256
#define RTE_ADC2_AVG_SAMPLE_NUM          (256)

// <o> Number of bits to shift <0-8>
// <i> Defines How much bit to shift before storing in sample register.
// <i> Default: 8
#define RTE_ADC2_SHIFT_N_BIT             (8)

// <o> Left or Right shift
//     <0=> LEFT SHIFT
//     <1=> RIGHT SHIFT
// <i> Defines shift averaged value before loading in sample register.
// <i> Default: RIGHT SHIFT
#define RTE_ADC2_SHIFT_LEFT_OR_RIGHT     (1)

// <o> Test Signal enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines Enable test signal to go to the test MUX: if "1" otherwise off
// <i> Default: DISABLE
#define RTE_ADC2_TEST_EN                  0

// <o> Differential mode enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines Enable differential Mode: 1 ==> in1/2, in3/4, in5/6 will be differential  other wise all single ended
// <i> Default: DISABLE
#define RTE_ADC2_DIFFERENTIAL_EN          0

// <o> Comparator mode enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines set 1 to ENABLE the comparator otherwise comparator "DISABLE"
// <i> Default: ENABLE
#define RTE_ADC2_COMPARATOR_EN            1

// <o> Comparator bias enable
//     <0=> 0.5MS/s
//     <1=> 1MS/s
//     <2=> 2.5MS/s
//     <3=> 5MS/s
// <i> Defines: "11":5MS/s; "10""2.5MS/s; "01":1MS/s;"00":0.5MS/s
// <i> Default: ENABLE
#define RTE_ADC2_COMPARATOR_BIAS          2

// <o> Resistive divider enable
//     <0=> DISABLE
//     <1=> ENABLE
// <i> Defines "0": the resistive divider is off, common mode is not well defined,    "1": the resistive divider is on, common mode is VDD/2
// <i> Default: ENABLE
#define RTE_ADC2_VCM_RDIV_EN              1

#endif
// </e> ADC2 (Analog to Digital Converter 2) [Driver_ADC2]
// </h> ADC (Analog to Digital Converter)

// <e> ADC common bits for each instance
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
#define RTE_HWSEM1                                     1
#if RTE_HWSEM1
#define RTE_HWSEM1_IRQPRIORITY                         0
#endif

// </e> HWSEM1 (Hardware Semaphore) [Driver_HWSEM1]


// <e> HWSEM2 (Hardware Semaphore) [Driver_HWSEM2]
// <i> Configuration settings for Driver_HWSEM2 in component ::Drivers:HWSEM
#define RTE_HWSEM2                                     1
#if RTE_HWSEM2
#define RTE_HWSEM2_IRQPRIORITY                         0
#endif

// </e> HWSEM2 (Hardware Semaphore) [Driver_HWSEM2]


// <e> HWSEM3 (Hardware Semaphore) [Driver_HWSEM3]
// <i> Configuration settings for Driver_HWSEM3 in component ::Drivers:HWSEM
#define RTE_HWSEM3                                     1
#if RTE_HWSEM3
#define RTE_HWSEM3_IRQPRIORITY                         0
#endif

// </e> HWSEM3 (Hardware Semaphore) [Driver_HWSEM3]


// <e> HWSEM4 (Hardware Semaphore) [Driver_HWSEM4]
// <i> Configuration settings for Driver_HWSEM4 in component ::Drivers:HWSEM
#define RTE_HWSEM4                                     1
#if RTE_HWSEM4
#define RTE_HWSEM4_IRQPRIORITY                         0
#endif

// </e> HWSEM4 (Hardware Semaphore) [Driver_HWSEM4]


// <e> HWSEM5 (Hardware Semaphore) [Driver_HWSEM5]
// <i> Configuration settings for Driver_HWSEM5 in component ::Drivers:HWSEM
#define RTE_HWSEM5                                     1
#if RTE_HWSEM5
#define RTE_HWSEM5_IRQPRIORITY                         0
#endif

// </e> HWSEM5 (Hardware Semaphore) [Driver_HWSEM5]


// <e> HWSEM6 (Hardware Semaphore) [Driver_HWSEM6]
// <i> Configuration settings for Driver_HWSEM6 in component ::Drivers:HWSEM
#define RTE_HWSEM6                                     1
#if RTE_HWSEM6
#define RTE_HWSEM6_IRQPRIORITY                         0
#endif

// </e> HWSEM6 (Hardware Semaphore) [Driver_HWSEM6]


// <e> HWSEM7 (Hardware Semaphore) [Driver_HWSEM7]
// <i> Configuration settings for Driver_HWSEM7 in component ::Drivers:HWSEM
#define RTE_HWSEM7                                     1
#if RTE_HWSEM7
#define RTE_HWSEM7_IRQPRIORITY                         0
#endif

// </e> HWSEM7 (Hardware Semaphore) [Driver_HWSEM7]


// <e> HWSEM8 (Hardware Semaphore) [Driver_HWSEM8]
// <i> Configuration settings for Driver_HWSEM8 in component ::Drivers:HWSEM
#define RTE_HWSEM8                                     1
#if RTE_HWSEM8
#define RTE_HWSEM8_IRQPRIORITY                         0
#endif

// </e> HWSEM8 (Hardware Semaphore) [Driver_HWSEM8]


// <e> HWSEM9 (Hardware Semaphore) [Driver_HWSEM9]
// <i> Configuration settings for Driver_HWSEM9 in component ::Drivers:HWSEM
#define RTE_HWSEM9                                     1
#if RTE_HWSEM9
#define RTE_HWSEM9_IRQPRIORITY                         0
#endif

// </e> HWSEM9 (Hardware Semaphore) [Driver_HWSEM9]


// <e> HWSEM10 (Hardware Semaphore) [Driver_HWSEM10]
// <i> Configuration settings for Driver_HWSEM10 in component ::Drivers:HWSEM
#define RTE_HWSEM10                                     1
#if RTE_HWSEM10
#define RTE_HWSEM10_IRQPRIORITY                         0
#endif

// </e> HWSEM10 (Hardware Semaphore) [Driver_HWSEM10]


// <e> HWSEM11 (Hardware Semaphore) [Driver_HWSEM11]
// <i> Configuration settings for Driver_HWSEM11 in component ::Drivers:HWSEM
#define RTE_HWSEM11                                     1
#if RTE_HWSEM11
#define RTE_HWSEM11_IRQPRIORITY                         0
#endif

// </e> HWSEM11 (Hardware Semaphore) [Driver_HWSEM11]


// <e> HWSEM12 (Hardware Semaphore) [Driver_HWSEM12]
// <i> Configuration settings for Driver_HWSEM12 in component ::Drivers:HWSEM
#define RTE_HWSEM12                                     1
#if RTE_HWSEM12
#define RTE_HWSEM12_IRQPRIORITY                         0
#endif

// </e> HWSEM12 (Hardware Semaphore) [Driver_HWSEM12]


// <e> HWSEM13 (Hardware Semaphore) [Driver_HWSEM13]
// <i> Configuration settings for Driver_HWSEM13 in component ::Drivers:HWSEM
#define RTE_HWSEM13                                     1
#if RTE_HWSEM13
#define RTE_HWSEM13_IRQPRIORITY                         0
#endif

// </e> HWSEM13 (Hardware Semaphore) [Driver_HWSEM13]


// <e> HWSEM14 (Hardware Semaphore) [Driver_HWSEM14]
// <i> Configuration settings for Driver_HWSEM14 in component ::Drivers:HWSEM
#define RTE_HWSEM14                                     1
#if RTE_HWSEM14
#define RTE_HWSEM14_IRQPRIORITY                         0
#endif

// </e> HWSEM14 (Hardware Semaphore) [Driver_HWSEM14]


// <e> HWSEM15 (Hardware Semaphore) [Driver_HWSEM15]
// <i> Configuration settings for Driver_HWSEM15 in component ::Drivers:HWSEM
#define RTE_HWSEM15                                     1
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

// <e> I2C (Inter Integrated Circuit) [Driver_I2C0]
// <i> Configuration settings for Driver_I2C0 in component ::Drivers:I2C
#define RTE_I2C0      1
#if RTE_I2C0
#define RTE_I2C0_IRQ_PRIORITY        0
#endif
// </e> I2C (Inter Integrated Circuit) [Driver_I2C0]

// <e> I2C (Inter Integrated Circuit) [Driver_I2C1]
// <i> Configuration settings for Driver_I2C1 in component ::Drivers:I2C
#define RTE_I2C1      1
#if RTE_I2C1
#define RTE_I2C1_IRQ_PRIORITY        0
#endif
// </e> I2C (Inter Integrated Circuit) [Driver_I2C1]

// <e> I2C (Inter Integrated Circuit) [Driver_I2C2]
// <i> Configuration settings for Driver_I2C2 in component ::Drivers:I2C
#define RTE_I2C2      1
#if RTE_I2C2
#define RTE_I2C2_IRQ_PRIORITY        0
#endif
// </e> I2C (Inter Integrated Circuit) [Driver_I2C2]

// <e> I2C (Inter Integrated Circuit) [Driver_I2C3]
// <i> Configuration settings for Driver_I2C3 in component ::Drivers:I2C
#define RTE_I2C3      1
#if RTE_I2C3
#define RTE_I2C3_IRQ_PRIORITY        0
#endif
// </e> I2C (Inter Integrated Circuit) [Driver_I2C3]

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

#if RTE_RTSS_M55_HP
// <e> DMA1 (Direct Memory Access Controller) [Driver_DMA1]
// <i> Configuration settings for Driver_DMA1 in component ::Drivers:DMA
#define RTE_DMA1      1
#if RTE_DMA1
//   DMA APB Interface to be used <0-1>
//   0 - Secure APB interface, 1 - Non-Secure
//   Default: 0
#define RTE_DMA1_APB_INTERFACE 0
#define RTE_DMA1_ABORT_IRQ_PRI 0
#endif
// </e> DMA1 (Direct Memory Access Controller) [Driver_DMA1]
#endif

#if RTE_RTSS_M55_HE
// <e> DMA2 (Direct Memory Access Controller) [Driver_DMA2]
// <i> Configuration settings for Driver_DMA2 in component ::Drivers:DMA
#define RTE_DMA2      1
#if RTE_DMA2
//   DMA APB Interface to be used <0-1>
//   0 - Secure APB interface, 1 - Non-Secure
//   Default: 0
#define RTE_DMA2_APB_INTERFACE 0
#define RTE_DMA2_ABORT_IRQ_PRI 0
#endif
// </e> DMA2 (Direct Memory Access Controller) [Driver_DMA2]
#endif //todo

// <h> GPIO (general purpose input and output)
// <e> GPIO0
// <i> Configuration settings for Driver_GPIO0 in component ::Drivers:GPIO
#define RTE_GPIO0   1

#if RTE_GPIO0

// <o> GPIO0 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO0.
// <i> Default: 0
#define RTE_GPIO0_DB_CLK_DIV            1

// <o> GPIO0_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO0_PIN0.
// <i> Default: 0
#define RTE_GPIO0_PIN0_IRQ_PRIORITY     0

// <o> GPIO0_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO0_PIN0.
// <i> Default: 0
#define RTE_GPIO0_PIN0_IRQ_PRIORITY     0

// <o> GPIO0_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO0_PIN1.
// <i> Default: 0
#define RTE_GPIO0_PIN1_IRQ_PRIORITY     0

// <o> GPIO0_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO0_PIN2.
// <i> Default: 0
#define RTE_GPIO0_PIN2_IRQ_PRIORITY     0

// <o> GPIO0_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO0_PIN3.
// <i> Default: 0
#define RTE_GPIO0_PIN3_IRQ_PRIORITY     0

// <o> GPIO0_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO0_PIN4.
// <i> Default: 0
#define RTE_GPIO0_PIN4_IRQ_PRIORITY     0

// <o> GPIO0_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO0_PIN5.
// <i> Default: 0
#define RTE_GPIO0_PIN5_IRQ_PRIORITY     0

// <o> GPIO0_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO0_PIN6.
// <i> Default: 0
#define RTE_GPIO0_PIN6_IRQ_PRIORITY     0

// <o> GPIO0_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO0_PIN7.
// <i> Default: 0
#define RTE_GPIO0_PIN7_IRQ_PRIORITY     0
#endif
// </e> GPIO0

// <e> GPIO1
// <i> Configuration settings for Driver_GPIO1 in component ::Drivers:GPIO
#define RTE_GPIO1   1

#if RTE_GPIO1

// <o> GPIO1 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO1.
// <i> Default: 0
#define RTE_GPIO1_DB_CLK_DIV            1

// <o> GPIO1_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO1_PIN0.
// <i> Default: 0
#define RTE_GPIO1_PIN0_IRQ_PRIORITY     0

// <o> GPIO1_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO1_PIN1.
// <i> Default: 0
#define RTE_GPIO1_PIN1_IRQ_PRIORITY     0

// <o> GPIO1_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO1_PIN2.
// <i> Default: 0
#define RTE_GPIO1_PIN2_IRQ_PRIORITY     0

// <o> GPIO1_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO1_PIN3.
// <i> Default: 0
#define RTE_GPIO1_PIN3_IRQ_PRIORITY     0

// <o> GPIO1_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO1_PIN4.
// <i> Default: 0
#define RTE_GPIO1_PIN4_IRQ_PRIORITY     0

// <o> GPIO1_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO1_PIN5.
// <i> Default: 0
#define RTE_GPIO1_PIN5_IRQ_PRIORITY     0

// <o> GPIO1_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO1_PIN6.
// <i> Default: 0
#define RTE_GPIO1_PIN6_IRQ_PRIORITY     0

// <o> GPIO1_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO1_PIN7.
// <i> Default: 0
#define RTE_GPIO1_PIN7_IRQ_PRIORITY     0
#endif
// </e> GPIO1

// <e> GPIO2
// <i> Configuration settings for Driver_GPIO2 in component ::Drivers:GPIO
#define RTE_GPIO2   1

#if RTE_GPIO2

// <o> GPIO2 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO2.
// <i> Default: 0
#define RTE_GPIO2_DB_CLK_DIV            1

// <o> GPIO2_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO2_PIN0.
// <i> Default: 0
#define RTE_GPIO2_PIN0_IRQ_PRIORITY     0

// <o> GPIO2_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO2_PIN1.
// <i> Default: 0
#define RTE_GPIO2_PIN1_IRQ_PRIORITY     0

// <o> GPIO2_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO2_PIN2.
// <i> Default: 0
#define RTE_GPIO2_PIN2_IRQ_PRIORITY     0

// <o> GPIO2_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO1_PIN3.
// <i> Default: 0
#define RTE_GPIO2_PIN3_IRQ_PRIORITY     0

// <o> GPIO2_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO2_PIN4.
// <i> Default: 0
#define RTE_GPIO2_PIN4_IRQ_PRIORITY     0

// <o> GPIO2_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO2_PIN5.
// <i> Default: 0
#define RTE_GPIO2_PIN5_IRQ_PRIORITY     0

// <o> GPIO2_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO2_PIN6.
// <i> Default: 0
#define RTE_GPIO2_PIN6_IRQ_PRIORITY     0

// <o> GPIO2_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO2_PIN7.
// <i> Default: 0
#define RTE_GPIO2_PIN7_IRQ_PRIORITY     0
#endif
// </e> GPIO2

// <e> GPIO3
// <i> Configuration settings for Driver_GPIO3 in component ::Drivers:GPIO
#define RTE_GPIO3   1

#if RTE_GPIO3

// <o> GPIO3 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO3.
// <i> Default: 0
#define RTE_GPIO3_DB_CLK_DIV            1

// <o> GPIO3_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO3_PIN0.
// <i> Default: 0
#define RTE_GPIO3_PIN0_IRQ_PRIORITY     0

// <o> GPIO3_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO3_PIN1.
// <i> Default: 0
#define RTE_GPIO3_PIN1_IRQ_PRIORITY     0

// <o> GPIO3_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO3_PIN2.
// <i> Default: 0
#define RTE_GPIO3_PIN2_IRQ_PRIORITY     0

// <o> GPIO3_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO3_PIN3.
// <i> Default: 0
#define RTE_GPIO3_PIN3_IRQ_PRIORITY     0

// <o> GPIO3_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO3_PIN4.
// <i> Default: 0
#define RTE_GPIO3_PIN4_IRQ_PRIORITY     0

// <o> GPIO3_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO3_PIN5.
// <i> Default: 0
#define RTE_GPIO3_PIN5_IRQ_PRIORITY     0

// <o> GPIO3_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO3_PIN6.
// <i> Default: 0
#define RTE_GPIO3_PIN6_IRQ_PRIORITY     0

// <o> GPIO3_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO3_PIN7.
// <i> Default: 0
#define RTE_GPIO3_PIN7_IRQ_PRIORITY     0
#endif
// </e> GPIO3

// <e> GPIO4
// <i> Configuration settings for Driver_GPIO4 in component ::Drivers:GPIO
#define RTE_GPIO4   1

#if RTE_GPIO4

// <o> GPIO4 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO4.
// <i> Default: 0
#define RTE_GPIO4_DB_CLK_DIV            1

// <o> GPIO4_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO4_PIN0.
// <i> Default: 0
#define RTE_GPIO4_PIN0_IRQ_PRIORITY     0

// <o> GPIO4_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO4_PIN1.
// <i> Default: 0
#define RTE_GPIO4_PIN1_IRQ_PRIORITY     0

// <o> GPIO4_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO4_PIN2.
// <i> Default: 0
#define RTE_GPIO4_PIN2_IRQ_PRIORITY     0

// <o> GPIO4_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO4_PIN3.
// <i> Default: 0
#define RTE_GPIO4_PIN3_IRQ_PRIORITY     0

// <o> GPIO4_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO4_PIN4.
// <i> Default: 0
#define RTE_GPIO4_PIN4_IRQ_PRIORITY     0

// <o> GPIO4_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO4_PIN5.
// <i> Default: 0
#define RTE_GPIO4_PIN5_IRQ_PRIORITY     0

// <o> GPIO4_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO4_PIN6.
// <i> Default: 0
#define RTE_GPIO4_PIN6_IRQ_PRIORITY     0

// <o> GPIO4_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO4_PIN7.
// <i> Default: 0
#define RTE_GPIO4_PIN7_IRQ_PRIORITY     0
#endif
// </e> GPIO4

// <e> GPIO5
// <i> Configuration settings for Driver_GPIO5 in component ::Drivers:GPIO
#define RTE_GPIO5   1

#if RTE_GPIO5

// <o> GPIO5 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO5.
// <i> Default: 0
#define RTE_GPIO5_DB_CLK_DIV            1

// <o> GPIO5_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO5_PIN0.
// <i> Default: 0
#define RTE_GPIO5_PIN0_IRQ_PRIORITY     0

// <o> GPIO5_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO5_PIN1.
// <i> Default: 0
#define RTE_GPIO5_PIN1_IRQ_PRIORITY     0

// <o> GPIO5_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO5_PIN2.
// <i> Default: 0
#define RTE_GPIO5_PIN2_IRQ_PRIORITY     0

// <o> GPIO5_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO5_PIN3.
// <i> Default: 0
#define RTE_GPIO5_PIN3_IRQ_PRIORITY     0

// <o> GPIO5_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO5_PIN4.
// <i> Default: 0
#define RTE_GPIO5_PIN4_IRQ_PRIORITY     0

// <o> GPIO5_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO5_PIN5.
// <i> Default: 0
#define RTE_GPIO5_PIN5_IRQ_PRIORITY     0

// <o> GPIO5_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO5_PIN6.
// <i> Default: 0
#define RTE_GPIO5_PIN6_IRQ_PRIORITY     0

// <o> GPIO5_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO5_PIN7.
// <i> Default: 0
#define RTE_GPIO5_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO5 */
// </e> GPIO5

// <e> GPIO6
// <i> Configuration settings for Driver_GPIO6 in component ::Drivers:GPIO
#define RTE_GPIO6   1

#if RTE_GPIO6

// <o> GPIO6 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO6.
// <i> Default: 0
#define RTE_GPIO6_DB_CLK_DIV            1

// <o> GPIO6_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO6_PIN0.
// <i> Default: 0
#define RTE_GPIO6_PIN0_IRQ_PRIORITY     0

// <o> GPIO6_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO6_PIN1.
// <i> Default: 0
#define RTE_GPIO6_PIN1_IRQ_PRIORITY     0

// <o> GPIO6_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO6_PIN2.
// <i> Default: 0
#define RTE_GPIO6_PIN2_IRQ_PRIORITY     0

// <o> GPIO6_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO6_PIN3.
// <i> Default: 0
#define RTE_GPIO6_PIN3_IRQ_PRIORITY     0

// <o> GPIO6_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO6_PIN4.
// <i> Default: 0
#define RTE_GPIO6_PIN4_IRQ_PRIORITY     0

// <o> GPIO6_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO6_PIN5.
// <i> Default: 0
#define RTE_GPIO6_PIN5_IRQ_PRIORITY     0

// <o> GPIO6_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO6_PIN6.
// <i> Default: 0
#define RTE_GPIO6_PIN6_IRQ_PRIORITY     0

// <o> GPIO6_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO6_PIN7.
// <i> Default: 0
#define RTE_GPIO6_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO6 */
// </e> GPIO6

// <e> GPIO7
// <i> Configuration settings for Driver_GPIO7 in component ::Drivers:GPIO
#define RTE_GPIO7   1

#if RTE_GPIO7

// <o> GPIO7 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO7.
// <i> Default: 0
#define RTE_GPIO7_DB_CLK_DIV            1

// <o> GPIO7_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO7_PIN0.
// <i> Default: 0
#define RTE_GPIO7_PIN0_IRQ_PRIORITY     0

// <o> GPIO7_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO7_PIN1.
// <i> Default: 0
#define RTE_GPIO7_PIN1_IRQ_PRIORITY     0

// <o> GPIO7_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO7_PIN2.
// <i> Default: 0
#define RTE_GPIO7_PIN2_IRQ_PRIORITY     0

// <o> GPIO7_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO7_PIN3.
// <i> Default: 0
#define RTE_GPIO7_PIN3_IRQ_PRIORITY     0

// <o> GPIO7_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO7_PIN4.
// <i> Default: 0
#define RTE_GPIO7_PIN4_IRQ_PRIORITY     0

// <o> GPIO7_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO7_PIN5.
// <i> Default: 0
#define RTE_GPIO7_PIN5_IRQ_PRIORITY     0

// <o> GPIO7_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO7_PIN6.
// <i> Default: 0
#define RTE_GPIO7_PIN6_IRQ_PRIORITY     0

// <o> GPIO7_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO7_PIN7.
// <i> Default: 0
#define RTE_GPIO7_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO7 */
// </e> GPIO7

// <e> GPIO8
// <i> Configuration settings for Driver_GPIO8 in component ::Drivers:GPIO
#define RTE_GPIO8   1

#if RTE_GPIO8

// <o> GPIO8 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO8.
// <i> Default: 0
#define RTE_GPIO8_DB_CLK_DIV            1

// <o> GPIO8_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO8_PIN0.
// <i> Default: 0
#define RTE_GPIO8_PIN0_IRQ_PRIORITY     0

// <o> GPIO8_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO8_PIN1.
// <i> Default: 0
#define RTE_GPIO8_PIN1_IRQ_PRIORITY     0

// <o> GPIO8_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO8_PIN2.
// <i> Default: 0
#define RTE_GPIO8_PIN2_IRQ_PRIORITY     0

// <o> GPIO8_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO8_PIN3.
// <i> Default: 0
#define RTE_GPIO8_PIN3_IRQ_PRIORITY     0

// <o> GPIO8_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO8_PIN4.
// <i> Default: 0
#define RTE_GPIO8_PIN4_IRQ_PRIORITY     0

// <o> GPIO8_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO8_PIN5.
// <i> Default: 0
#define RTE_GPIO8_PIN5_IRQ_PRIORITY     0

// <o> GPIO8_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO8_PIN6.
// <i> Default: 0
#define RTE_GPIO8_PIN6_IRQ_PRIORITY     0

// <o> GPIO8_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO8_PIN7.
// <i> Default: 0
#define RTE_GPIO8_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO8 */
// </e> GPIO8

// <e> GPIO9
// <i> Configuration settings for Driver_GPIO9 in component ::Drivers:GPIO
#define RTE_GPIO9   1

#if RTE_GPIO9

// <o> GPIO9 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO9.
// <i> Default: 0
#define RTE_GPIO9_DB_CLK_DIV            1

// <o> GPIO9_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO9_PIN0.
// <i> Default: 0
#define RTE_GPIO9_PIN0_IRQ_PRIORITY     0

// <o> GPIO9_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO9_PIN1.
// <i> Default: 0
#define RTE_GPIO9_PIN1_IRQ_PRIORITY     0

// <o> GPIO9_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO9_PIN2.
// <i> Default: 0
#define RTE_GPIO9_PIN2_IRQ_PRIORITY     0

// <o> GPIO9_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO9_PIN3.
// <i> Default: 0
#define RTE_GPIO9_PIN3_IRQ_PRIORITY     0

// <o> GPIO9_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO9_PIN4.
// <i> Default: 0
#define RTE_GPIO9_PIN4_IRQ_PRIORITY     0

// <o> GPIO9_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO9_PIN5.
// <i> Default: 0
#define RTE_GPIO9_PIN5_IRQ_PRIORITY     0

// <o> GPIO9_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO9_PIN6.
// <i> Default: 0
#define RTE_GPIO9_PIN6_IRQ_PRIORITY     0

// <o> GPIO9_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO9_PIN7.
// <i> Default: 0
#define RTE_GPIO9_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO9 */
// </e> GPIO9

// <e> GPIO10
// <i> Configuration settings for Driver_GPIO10 in component ::Drivers:GPIO
#define RTE_GPIO10   1

#if RTE_GPIO10

// <o> GPIO10 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO10.
// <i> Default: 0
#define RTE_GPIO10_DB_CLK_DIV            1

// <o> GPIO10_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO10_PIN0.
// <i> Default: 0
#define RTE_GPIO10_PIN0_IRQ_PRIORITY     0

// <o> GPIO10_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO10_PIN1.
// <i> Default: 0
#define RTE_GPIO10_PIN1_IRQ_PRIORITY     0

// <o> GPIO10_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO10_PIN2.
// <i> Default: 0
#define RTE_GPIO10_PIN2_IRQ_PRIORITY     0

// <o> GPIO10_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO10_PIN3.
// <i> Default: 0
#define RTE_GPIO10_PIN3_IRQ_PRIORITY     0

// <o> GPIO10_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO10_PIN4.
// <i> Default: 0
#define RTE_GPIO10_PIN4_IRQ_PRIORITY     0

// <o> GPIO10_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO10_PIN5.
// <i> Default: 0
#define RTE_GPIO10_PIN5_IRQ_PRIORITY     0

// <o> GPIO10_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO10_PIN6.
// <i> Default: 0
#define RTE_GPIO10_PIN6_IRQ_PRIORITY     0

// <o> GPIO10_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO10_PIN7.
// <i> Default: 0
#define RTE_GPIO10_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO10 */
// </e> GPIO10

// <e> GPIO11
// <i> Configuration settings for Driver_GPIO11 in component ::Drivers:GPIO
#define RTE_GPIO11   1

#if RTE_GPIO11

// <o> GPIO11 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO11.
// <i> Default: 0
#define RTE_GPIO11_DB_CLK_DIV            1

// <o> GPIO11_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO11_PIN0.
// <i> Default: 0
#define RTE_GPIO11_PIN0_IRQ_PRIORITY     0

// <o> GPIO11_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO11_PIN1.
// <i> Default: 0
#define RTE_GPIO11_PIN1_IRQ_PRIORITY     0

// <o> GPIO11_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO11_PIN2.
// <i> Default: 0
#define RTE_GPIO11_PIN2_IRQ_PRIORITY     0

// <o> GPIO11_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO11_PIN3.
// <i> Default: 0
#define RTE_GPIO11_PIN3_IRQ_PRIORITY     0

// <o> GPIO11_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO11_PIN4.
// <i> Default: 0
#define RTE_GPIO11_PIN4_IRQ_PRIORITY     0

// <o> GPIO11_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO11_PIN5.
// <i> Default: 0
#define RTE_GPIO11_PIN5_IRQ_PRIORITY     0

// <o> GPIO11_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO11_PIN6.
// <i> Default: 0
#define RTE_GPIO11_PIN6_IRQ_PRIORITY     0

// <o> GPIO11_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO11_PIN7.
// <i> Default: 0
#define RTE_GPIO11_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO11 */
// </e> GPIO11

// <e> GPIO12
// <i> Configuration settings for Driver_GPIO12 in component ::Drivers:GPIO
#define RTE_GPIO12   1

#if RTE_GPIO12

// <o> GPIO12 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO12.
// <i> Default: 0
#define RTE_GPIO12_DB_CLK_DIV            1

// <o> GPIO12_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO12_PIN0.
// <i> Default: 0
#define RTE_GPIO12_PIN0_IRQ_PRIORITY     0

// <o> GPIO12_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO12_PIN1.
// <i> Default: 0
#define RTE_GPIO12_PIN1_IRQ_PRIORITY     0

// <o> GPIO12_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO12_PIN2.
// <i> Default: 0
#define RTE_GPIO12_PIN2_IRQ_PRIORITY     0

// <o> GPIO12_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO12_PIN3.
// <i> Default: 0
#define RTE_GPIO12_PIN3_IRQ_PRIORITY     0

// <o> GPIO12_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO12_PIN4.
// <i> Default: 0
#define RTE_GPIO12_PIN4_IRQ_PRIORITY     0

// <o> GPIO12_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO12_PIN5.
// <i> Default: 0
#define RTE_GPIO12_PIN5_IRQ_PRIORITY     0

// <o> GPIO12_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO12_PIN6.
// <i> Default: 0
#define RTE_GPIO12_PIN6_IRQ_PRIORITY     0

// <o> GPIO12_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO12_PIN7.
// <i> Default: 0
#define RTE_GPIO12_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO12 */
// </e> GPIO12

// <e> GPIO13
// <i> Configuration settings for Driver_GPIO13 in component ::Drivers:GPIO
#define RTE_GPIO13   1

#if RTE_GPIO13

// <o> GPIO13 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO13.
// <i> Default: 0
#define RTE_GPIO13_DB_CLK_DIV            1

// <o> GPIO13_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO13_PIN0.
// <i> Default: 0
#define RTE_GPIO13_PIN0_IRQ_PRIORITY     0

// <o> GPIO13_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO13_PIN1.
// <i> Default: 0
#define RTE_GPIO13_PIN1_IRQ_PRIORITY     0

// <o> GPIO13_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO13_PIN2.
// <i> Default: 0
#define RTE_GPIO13_PIN2_IRQ_PRIORITY     0

// <o> GPIO13_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO13_PIN3.
// <i> Default: 0
#define RTE_GPIO13_PIN3_IRQ_PRIORITY     0

// <o> GPIO13_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO13_PIN4.
// <i> Default: 0
#define RTE_GPIO13_PIN4_IRQ_PRIORITY     0

// <o> GPIO13_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO13_PIN5.
// <i> Default: 0
#define RTE_GPIO13_PIN5_IRQ_PRIORITY     0

// <o> GPIO13_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO13_PIN6.
// <i> Default: 0
#define RTE_GPIO13_PIN6_IRQ_PRIORITY     0

// <o> GPIO13_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO13_PIN7.
// <i> Default: 0
#define RTE_GPIO13_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO13 */
// </e> GPIO13

// <e> GPIO14
// <i> Configuration settings for Driver_GPIO14 in component ::Drivers:GPIO
#define RTE_GPIO14   1

#if RTE_GPIO14

// <o> GPIO14 Debounce Clock Divisor <0-1023>
// <i> Defines debounce clock divisor for GPIO14.
// <i> Default: 0
#define RTE_GPIO14_DB_CLK_DIV            1

// <o> GPIO14_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO14_PIN0.
// <i> Default: 0
#define RTE_GPIO14_PIN0_IRQ_PRIORITY     0

// <o> GPIO14_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO14_PIN1.
// <i> Default: 0
#define RTE_GPIO14_PIN1_IRQ_PRIORITY     0

// <o> GPIO14_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO14_PIN2.
// <i> Default: 0
#define RTE_GPIO14_PIN2_IRQ_PRIORITY     0

// <o> GPIO14_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO14_PIN3.
// <i> Default: 0
#define RTE_GPIO14_PIN3_IRQ_PRIORITY     0

// <o> GPIO14_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO14_PIN4.
// <i> Default: 0
#define RTE_GPIO14_PIN4_IRQ_PRIORITY     0

// <o> GPIO14_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO14_PIN5.
// <i> Default: 0
#define RTE_GPIO14_PIN5_IRQ_PRIORITY     0

// <o> GPIO14_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO14_PIN6.
// <i> Default: 0
#define RTE_GPIO14_PIN6_IRQ_PRIORITY     0

// <o> GPIO14_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO14_PIN7.
// <i> Default: 0
#define RTE_GPIO14_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO14 */
// </e> GPIO14

// <e> GPIO15
// <i> Configuration settings for Driver_GPIO15 in component ::Drivers:GPIO
#define RTE_GPIO15   1

#if RTE_GPIO15

// <o> GPIO15_PIN0 IRQ Priority
// <i> Defines Interrupt priority for GPIO15_PIN0.
// <i> Default: 0
#define RTE_GPIO15_PIN0_IRQ_PRIORITY     0

// <o> GPIO15_PIN1 IRQ Priority
// <i> Defines Interrupt priority for GPIO15_PIN1.
// <i> Default: 0
#define RTE_GPIO15_PIN1_IRQ_PRIORITY     0

// <o> GPIO15_PIN2 IRQ Priority
// <i> Defines Interrupt priority for GPIO15_PIN2.
// <i> Default: 0
#define RTE_GPIO15_PIN2_IRQ_PRIORITY     0

// <o> GPIO15_PIN3 IRQ Priority
// <i> Defines Interrupt priority for GPIO15_PIN3.
// <i> Default: 0
#define RTE_GPIO15_PIN3_IRQ_PRIORITY     0

// <o> GPIO15_PIN4 IRQ Priority
// <i> Defines Interrupt priority for GPIO15_PIN4.
// <i> Default: 0
#define RTE_GPIO15_PIN4_IRQ_PRIORITY     0

// <o> GPIO15_PIN5 IRQ Priority
// <i> Defines Interrupt priority for GPIO15_PIN5.
// <i> Default: 0
#define RTE_GPIO15_PIN5_IRQ_PRIORITY     0

// <o> GPIO15_PIN6 IRQ Priority
// <i> Defines Interrupt priority for GPIO15_PIN6.
// <i> Default: 0
#define RTE_GPIO15_PIN6_IRQ_PRIORITY     0

// <o> GPIO15_PIN7 IRQ Priority
// <i> Defines Interrupt priority for GPIO15_PIN7.
// <i> Default: 0
#define RTE_GPIO15_PIN7_IRQ_PRIORITY     0
#endif /* RTE_GPIO15 */
// </e> GPIO15

// </h> GPIO (general purpose input and output)

// <h> PDM(Pulse Density Modulation)
// <e> PDM (Pulse density modulation) [Driver_PDM]
// <i> Configuration settings for Driver_PDM in component ::Drivers:PDM
#define RTE_PDM      1
#if RTE_PDM
// <o> PDM IRQ priority <0-255>
// <i> Defines Interrupt priority for PDM.
// <i> Default: 0
#define RTE_PDM_IRQ_PRIORITY   0
#endif
// </e> PDM (Pulse density modulation) [Driver_PDM]

// </h> PDM (Pulse density modulation)

#endif  /* __RTE_DEVICE_H */
