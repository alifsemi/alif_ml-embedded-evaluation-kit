/*
 * mipi_dsi_host.h
 *
 *  Created on: 15-Feb-2022
 *      Author: prasanna
 */

#ifndef MIPI_DSI_MIPI_DSI_HOST_H_
#define MIPI_DSI_MIPI_DSI_HOST_H_

#include "base_def.h"

//define VERSION ADDRESS REGISTER
#define     A_VERSION_CFG                       0x00
//define CLOCK MANAGER ADDRESS REGISTERS
#define     A_PWR_UP                            0x04
#define     A_CLKMGR_CFG                        0x08
//define DPI ADDRESS REGISTERS
#define     A_DPI_VCID                          0x0C
#define     A_DPI_COLOR_CODING                  0x10
#define     A_DPI_CFG_POL                       0x14
#define     A_DPI_LP_CMD_TIM                    0x18
#define     A_DPI_DUAL_PIXEL_MODE               0x14
//define DBI ADDRESS REGISTERS
#define     A_DBI_VCID                          0x1C
#define     A_DBI_CFG                           0x20
#define     A_DBI_PARTITIONING_EN               0x24
#define     A_DBI_CMDSIZE                       0x28
//define PACKET HANDLER ADDRESS REGISTERS
#define     A_PCKHDL_CFG                        0x2C
#define     A_GEN_VCID                          0x30
#define     A_MODE_CFG                          0x34
//define VIDEO MODE ADDRESS REGISTERS
#define     A_VID_MODE_CFG                      0x38
#define     A_VID_PKT_SIZE                      0x3C
#define     A_VID_NUM_CHUNKS                    0x40
#define     A_VID_NULL_SIZE                     0x44
#define     A_VID_HSA_TIME                      0x48
#define     A_VID_HBP_TIME                      0x4C
#define     A_VID_HLINE_TIME                    0x50
#define     A_VID_VSA_LINE                      0x54
#define     A_VID_VBP_LINES                     0x58
#define     A_VID_VFP_LINES                     0x5C
#define     A_VID_VACTIVE_LINES                 0x60
#define     A_EDPI_CMD_SIZE                     0x64
#define     A_CMD_MODE_CFG                      0x68
//define SFR2GENERIC ADDRESS REGISTERS
#define     A_GEN_HDR                           0x6C
#define     A_GEN_PLD_DATA                      0x70
#define     A_CMD_PKT_STATUS                    0x74
#define     A_TO_CNT_CFG                        0x78
#define     A_HS_RD_TO_CNT                      0x7C
#define     A_LP_RD_TO_CNT                      0x80
#define     A_HS_WR_TO_CNT                      0x84
#define     A_LP_WR_TO_CNT                      0x88
#define     A_BTA_TO_CNT                        0x8C
#define     A_SDF_3D                            0x90
#define     A_LPCLK_CTRL                        0x94
//define PHYIFCTRLDIG ADDRESS REGISTERS
#define     A_PHY_TMR_LPCLK_CFG                 0x98
#define     A_PHY_TMR_CFG                       0x9C
#define     A_PHY_TMR_RD_CFG                    0xF4
#define     A_PHYRSTZ                           0xA0
#define     A_PHY_IF_CFG                        0xA4
#define     A_PHY_ULPS_CTRL                     0xA8
#define     A_PHY_TX_TRIGGERS                   0xAC
#define     A_PHY_STATUS                        0xB0
#define     A_PHY_TST_CTRL0                     0xB4
#define     A_PHY_TST_CTRL1                     0xB8
//define ERROR CONTROL ADDRESS REGISTERS
#define     A_INT_ST0                           0xBC
#define     A_INT_ST1                           0xC0
#define     A_INT_MASK0_CFG                     0xC4
#define     A_INT_MASK1_CFG                     0xC8
#define     A_PHY_CAL                           0xCC
#define     A_VID_SHADOW_CTRL                   0x100

#define     A_DPI_VCID_ACT                      0x10C
#define     A_DPI_COLOR_CODING_ACT              0x110
#define     A_DPI_LP_CMD_TIM_ACT                0x118
#define     A_EDPI_TE_HW_CFG                    0x11C
#define     A_VID_MODE_CFG_ACT                  0x138
#define     A_VID_PKT_SIZE_ACT                  0x13C
#define     A_VID_NUM_CHUNKS_ACT                0x140
#define     A_VID_NULL_SIZE_ACT                 0x144
#define     A_VID_HSA_TIME_ACT                  0x148
#define     A_VID_HBP_TIME_ACT                  0x14C
#define     A_VID_HLINE_TIME_ACT                0x150
#define     A_VID_VSA_LINES_ACT                 0x154
#define     A_VID_VBP_LINES_ACT                 0x158
#define     A_VID_VFP_LINES_ACT                 0x15C
#define     A_VID_VACTIVE_LINES_ACT             0x160
#define     A_VID_PKT_STATUS                    0x168
#define     A_SDF_3D_CFG_ACT                    0x190

#define     A_INT_FORCE0                        0xD8
#define     A_INT_FORCE1                        0xDC

#define     A_AUTO_ULPS_MODE                    0xE0
#define     A_AUTO_ULPS_ENTRY_DELAY             0xE4
#define     A_AUTO_ULPS_WAKEUP_TIME             0xE8
#define     A_EDPI_ADV_FEATURES                 0xEC
#define     A_AUTO_ULPS_MIN_TIME                0xF8

#define     A_DSC_PARAMETER                     0xF0

int tx_phyconfig();

//Display
void DCSWN_S(uint8_t);
void DCSW1_S(uint8_t,uint8_t);
void DCSRN_S (uint8_t);
void SMRPS_S (uint8_t);
void DCSW_L (uint8_t,uint8_t, uint8_t, uint8_t, uint8_t);

void dsi_command_mode_initialization(void);
void dsi_video_mode_initialization(void);

#endif /* MIPI_DSI_MIPI_DSI_HOST_H_ */
