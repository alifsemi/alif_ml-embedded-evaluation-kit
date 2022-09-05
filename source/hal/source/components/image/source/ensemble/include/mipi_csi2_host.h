/*
 * mipi_csi_host.h
 *
 *  Created on: 16-Feb-2022
 *      Author: prasanna
 */

#ifndef MIPI_CSI2_MIPI_CSI2_HOST_H_
#define MIPI_CSI2_MIPI_CSI2_HOST_H_

#include "base_def.h"

//CSI-2 host controller
#define  CSI2_HOST_VERSION                     0x000
#define  CSI2_HOST_N_LANES                     0x004
#define  CSI2_HOST_CSI2_RESETN                 0x008
#define  CSI2_HOST_INT_ST_MAIN                 0x00c
#define  CSI2_HOST_VC_EXTENSION                0x0C8

//Phy related registers
#define  CSI2_HOST_PHY_SHUTDOWNZ               0x040
#define  CSI2_HOST_DPHY_RSTZ                   0x044
#define  CSI2_HOST_PHY_RX                      0x048
#define  CSI2_HOST_PHY_STOPSTATE               0x04c
#define  CSI2_HOST_PHY_TEST_CTRL0              0x050
#define  CSI2_HOST_PHY_TEST_CTRL1              0x054

//IPI interface related registers
#define  CSI2_HOST_IPI_MODE                    0x080
#define  CSI2_HOST_IPI_VCID                    0x084
#define  CSI2_HOST_IPI_DATA_TYPE               0x088
#define  CSI2_HOST_IPI_HSA_TIME                0x090
#define  CSI2_HOST_IPI_HBP_TIME                0x094
#define  CSI2_HOST_IPI_HSD_TIME                0x098
#define  CSI2_HOST_IPI_HLINE_TIME              0x09c
#define  CSI2_HOST_IPI_SOFTRSTN                0x0a0
#define  CSI2_HOST_IPI_ADV_FEATURES            0x0ac
#define  CSI2_HOST_IPI_VSA_LINES               0x0b0
#define  CSI2_HOST_IPI_VBP_LINES               0x0b4
#define  CSI2_HOST_IPI_VFP_LINES               0x0b8
#define  CSI2_HOST_IPI_VACTIVE_LINES           0x0bc
#define  CSI2_HOST_IPI_MEM_FLUSH               0x08c

#define  CSI2_HOST_INT_ST_BNDRY_FRAME_FATAL      0x280
#define  CSI2_HOST_INT_MSK_BNDRY_FRAME_FATAL     0x284
#define  CSI2_HOST_INT_FORCE_BNDRY_FRAME_FATAL   0x288

#define  CSI2_HOST_INT_ST_SEQ_FRAME_FATAL        0x290
#define  CSI2_HOST_INT_MSK_SEQ_FRAME_FATAL       0x294
#define  CSI2_HOST_INT_FORCE_SEQ_FRAME_FATAL     0x298

#define  CSI2_HOST_INT_ST_CRC_FRAME_FATAL        0x2A0
#define  CSI2_HOST_INT_MSK_CRC_FRAME_FATAL       0x2A4
#define  CSI2_HOST_INT_FORCE_CRC_FRAME_FATAL     0x2A8

#define  CSI2_HOST_INT_ST_PLD_CRC_FATAL          0x2B0
#define  CSI2_HOST_INT_MSK_PLD_CRC_FATAL         0x2B4
#define  CSI2_HOST_INT_FORCE_PLD_CRC_FATAL       0x2B8

#define  CSI2_HOST_INT_ST_DATA_ID                0x2C0
#define  CSI2_HOST_INT_MSK_DATA_ID               0x2C4
#define  CSI2_HOST_INT_FORCE_DATA_ID             0x2C8

#define  CSI2_HOST_INT_ST_ECC_CORRECT            0x2D0
#define  CSI2_HOST_INT_MSK_ECC_CORRECT           0x2D4
#define  CSI2_HOST_INT_FORCE_ECC_CORRECT         0x2D8

#define  CSI2_HOST_PHY_CAL                       0x0CC

//Interruption related registers
#define  CSI2_HOST_INT_ST_PHY_FATAL            0x0e0
#define  CSI2_HOST_INT_MSK_PHY_FATAL           0x0e4
#define  CSI2_HOST_INT_FORCE_PHY_FATAL         0x0e8

#define  CSI2_HOST_INT_ST_PKT_FATAL            0x0f0
#define  CSI2_HOST_INT_MSK_PKT_FATAL           0x0f4
#define  CSI2_HOST_INT_FORCE_PKT_FATAL         0x0f8

#define  CSI2_HOST_INT_ST_PHY                  0x110
#define  CSI2_HOST_INT_MSK_PHY                 0x114
#define  CSI2_HOST_INT_FORCE_PHY               0x118

#define  CSI2_HOST_INT_ST_IPI_FATAL            0x140
#define  CSI2_HOST_INT_MSK_IPI_FATAL           0x144
#define  CSI2_HOST_INT_FORCE_IPI_FATAL         0x148

#define  CSI2_HOST_SCRAMBLING                      0x300
#define  CSI2_HOST_SCRAMBLING_SEED1                0x304
#define  CSI2_HOST_SCRAMBLING_SEED1_RSTZ           0x1008
#define  CSI2_HOST_SCRAMBLING_SEED2                0x308
#define  CSI2_HOST_SCRAMBLING_SEED2_RSTZ           0x1188

#define  CSI2_HOST_INT_ST_FAP_BNDRY_FRAME_FATAL    0x420
#define  CSI2_HOST_INT_MSK_FAP_BNDRY_FRAME_FATAL   0x424
#define  CSI2_HOST_INT_FORCE_FAP_BNDRY_FRAME_FATAL 0x428

#define  CSI2_HOST_INT_ST_FAP_SEQ_FRAME_FATAL      0x430
#define  CSI2_HOST_INT_MSK_FAP_SEQ_FRAME_FATAL     0x434
#define  CSI2_HOST_INT_FORCE_FAP_SEQ_FRAME_FATAL   0x438

#define  CSI2_HOST_INT_ST_FAP_CRC_FRAME_FATAL      0x440
#define  CSI2_HOST_INT_MSK_FAP_CRC_FRAME_FATAL     0x444
#define  CSI2_HOST_INT_FORCE_FAP_CRC_FRAME_FATAL   0x448

#define  CSI2_HOST_INT_ST_FAP_PLD_CRC_FATAL        0x450
#define  CSI2_HOST_INT_MSK_FAP_PLD_CRC_FATAL       0x454
#define  CSI2_HOST_INT_FORCE_FAP_PLD_CRC_FATAL     0x458

#define  CSI2_HOST_INT_ST_FAP_DATA_ID              0x460
#define  CSI2_HOST_INT_MSK_FAP_DATA_ID             0x464
#define  CSI2_HOST_INT_FORCE_FAP_DATA_ID           0x468

#define  CSI2_HOST_INT_ST_FAP_ECC_CORRECT          0x470
#define  CSI2_HOST_INT_MSK_FAP_ECC_CORRECT         0x474
#define  CSI2_HOST_INT_FORCE_FAP_ECC_CORRECT       0x478

/* IPI Data Types */
enum video_t {
   CSI_2_YUV420_8 = 0x18,
   CSI_2_YUV420_10 = 0x19,
   CSI_2_YUV420_8_LEG = 0x1A,
   CSI_2_YUV420_8_SHIFT = 0x1C,
   CSI_2_YUV420_10_SHIFT = 0x1D,
   CSI_2_YUV422_8 = 0x1E,
   CSI_2_YUV422_10 = 0x1F,
   CSI_2_RGB444 = 0x20,
   CSI_2_RGB555 = 0x21,
   CSI_2_RGB565 = 0x22,
   CSI_2_RGB666 = 0x23,
   CSI_2_RGB888 = 0x24,
   CSI_2_RAW6 = 0x28,
   CSI_2_RAW7 = 0x29,
   CSI_2_RAW8 = 0x2A,
   CSI_2_RAW10 = 0x2B,
   CSI_2_RAW12 = 0x2C,
   CSI_2_RAW14 = 0x2D,
   CSI_2_RAW16 = 0x2E,
   CSI_2_RAW20 = 0x2F,
   USER_DEFINED_1 = 0x30,
   USER_DEFINED_2 = 0x31,
   USER_DEFINED_3 = 0x32,
   USER_DEFINED_4 = 0x33,
   USER_DEFINED_5 = 0x34,
   USER_DEFINED_6 = 0x35,
   USER_DEFINED_7 = 0x36,
   USER_DEFINED_8 = 0x37,
};


//#define iMX219
//#define ov5647
//#define ov5647_1081_raw8
//#define arx3A0_560
//#define arx3A0_560_c
#define arx3A0_405c

#ifdef ov5647
  #define format_640x480_raw8
  #define FREQ_150M
#elif defined iMX219
  #define format_640x480_raw10
#elif defined ov5647_1080_raw10
  #define format_1920x1080_raw10
  #define FREQ_250M
#elif defined ov5647_1081_raw8 //snps
  #define format_1920x1081_raw8
  #define FREQ_371M
#elif defined arx3A0_560 //on semi
  #define format_560x560_raw10
  #define FREQ_400M
#elif defined arx3A0_560_c //on semi controller mode
  #define format_560x560_raw10
  #define FREQ_400M
  #define controller_mode
#elif defined arx3A0_560_125Mhz //on semi match to dsi frequency
  #define format_560x560_raw10
  #define RX_FREQ_400M
  #define TX_FREQ_125M
#elif defined arx3A0_405c //on semi match to dsi frequency
  #define rx_format_560x560_raw10
  #define RX_FREQ_400M
#endif



int rx_phyconfig();

void csi2_IPI_initialization(void);
void csi2_IPI_enable(void);
void csi2_status(void);
#endif /* MIPI_CSI2_MIPI_CSI2_HOST_H_ */
