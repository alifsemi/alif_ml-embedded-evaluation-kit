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
 * @file     MIPI_DSI_dev.h
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     18-April-2022
 * @brief    Device Specific Header file for MIPI DSI Driver.
 ******************************************************************************/

#ifndef MIPI_DSI_DEV_H_
#define MIPI_DSI_DEV_H_

#include "RTE_Components.h"
#include CMSIS_device_header
#include "Driver_MIPI_DSI.h"

/**
  \ brief DSI register description.
  */
typedef struct {
	__IM  uint32_t VERSION;
	__IOM uint32_t PWR_UP;
	__IOM uint32_t CLKMGR_CFG;
	__IOM uint32_t DPI_VCID;
	__IOM uint32_t DPI_COLOR_CODING;
	__IOM uint32_t DPI_CFG_POL;
	__IOM uint32_t DPI_LP_CMD_TIM;
	__IOM uint32_t DBI_VCID;
	__IOM uint32_t DBI_CFG;
	__IOM uint32_t DBI_PARTITIONING_EN;
	__IOM uint32_t DBI_CMDSIZE;
	__IOM uint32_t PCKHDL_CFG;
	__IOM uint32_t GEN_VCID;
	__IOM uint32_t MODE_CFG;
	__IOM uint32_t VID_MODE_CFG;
	__IOM uint32_t VID_PKT_SIZE;
	__IOM uint32_t VID_NUM_CHUNKS;
	__IOM uint32_t VID_NULL_SIZE;
	__IOM uint32_t VID_HSA_TIME;
	__IOM uint32_t VID_HBP_TIME;
	__IOM uint32_t VID_HLINE_TIME;
	__IOM uint32_t VID_VSA_LINES;
	__IOM uint32_t VID_VBP_LINES;
	__IOM uint32_t VID_VFP_LINES;
	__IOM uint32_t VID_VACTIVE_LINES;
	__IOM uint32_t EDPI_CMD_SIZE;
	__IOM uint32_t CMD_MODE_CFG;
	__IOM uint32_t GEN_HDR;
	__IOM uint32_t GEN_PLD_DATA;
	__IM  uint32_t CMD_PKT_STATUS;
	__IOM uint32_t TO_CNT_CFG;
	__IOM uint32_t HS_RD_TO_CNT;
	__IOM uint32_t LP_RD_TO_CNT;
	__IOM uint32_t HS_WR_TO_CNT;
	__IOM uint32_t LP_WR_TO_CNT;
	__IOM uint32_t BTA_TO_CNT;
	__IOM uint32_t SDF_3D;
	__IOM uint32_t LPCLK_CTRL;
	__IOM uint32_t PHY_TMR_LPCLK_CFG;
	__IOM uint32_t PHY_TMR_CFG;
	__IOM uint32_t PHY_RSTZ;
	__IOM uint32_t PHY_IF_CFG;
	__IOM uint32_t PHY_ULPS_CTRL;
	__IOM uint32_t PHY_TX_TRIGGERS;
	__IM  uint32_t PHY_STATUS;
	__IOM uint32_t PHY_TST_CTRL0;
	__IOM uint32_t PHY_TST_CTRL1;
	__IM  uint32_t INT_ST0;
	__IM  uint32_t INT_ST1;
	__IOM uint32_t INT_MSK0;
	__IOM uint32_t INT_MSK1;
	__IOM uint32_t PHY_CAL;
	__IM  uint32_t reserved0[2];
	__IOM uint32_t INT_FORCE0;
	__IOM uint32_t INT_FORCE1;
	__IOM uint32_t AUTO_ULPS_MODE;
	__IOM uint32_t AUTO_ULPS_ENTRY_DELAY;
	__IOM uint32_t AUTO_ULPS_WAKEUP_TIME;
	__IM  uint32_t reserved1;
	__IOM uint32_t DSC_PARAMETER;
	__IOM uint32_t PHY_TMR_RD_CFG;
	__IOM uint32_t AUTO_ULPS_MIN_TIME;
	__IOM uint32_t PHY_MODE;
	__IOM uint32_t VID_SHADOW_CTRL;
	__IM  uint32_t reserved2[2];
	__IM  uint32_t DPI_VCID_ACT;
	__IOM uint32_t DPI_COLOR_CODING_ACT;
	__IM  uint32_t reserved3;
	__IOM uint32_t DPI_LP_CMD_TIM_ACT;
	__IOM uint32_t EDPI_TE_HW_CFG;
	__IM  uint32_t reserved4[6];
	__IM  uint32_t VID_MODE_CFG_ACT;
	__IM  uint32_t VID_PKT_SIZE_ACT;
	__IM  uint32_t VID_NUM_CHUNKS_ACT;
	__IM  uint32_t VID_NULL_SIZE_ACT;
	__IM  uint32_t VID_HSA_TIME_ACT;
	__IM  uint32_t VID_HBP_TIME_ACT;
	__IM  uint32_t VID_HLINE_TIME_ACT;
	__IM  uint32_t VID_VSA_LINES_ACT;
	__IM  uint32_t VID_VBP_LINES_ACT;
	__IM  uint32_t VID_VFP_LINES_ACT;
	__IM  uint32_t VID_VACTIVE_LINES_ACT;
	__IM  uint32_t reserved5;
	__IM  uint32_t VID_PKT_STATUS;
	__IM  uint32_t reserved6[9];
	__IM  uint32_t SDF_3D_ACT;
	__IM  uint32_t reserved7[73];
}MIPI_DSI_Type;

#define MIPI_DSI                            ((MIPI_DSI_Type*)MIPI_DSI_BASE)

/*PWR_UP register bits parameters*/
#define DSI_SHUTDOWNZ_Pos                   0U
#define DSI_SHUTDOWNZ_Msk                   (1U << DSI_SHUTDOWNZ_Pos)

/*CLKMGR_CFG register bits parameters*/
#define DSI_TX_ESC_CLK_DIVISION_Pos         0U
#define DSI_TX_ESC_CLK_DIVISION_Msk         MASK(7,0)
#define DSI_TO_CLK_DIVISION_Pos             8U
#define DSI_TO_CLK_DIVISION_Msk             MASK(15,8)

/*DPI_VCID register bits parameters*/
#define DSI_DPI_VCID_Pos                    0U
#define DSI_DPI_VCID_Msk                    MASK(1,0)

/*DPI_COLOR_CODING register bits parameters*/
#define DSI_DPI_COLOR_CODING_Pos            0U
#define DSI_DPI_COLOR_CODING_Msk            MASK(3,0)

/*DPI_CFG_POL register bits parameters*/
#define DSI_DATAEN_ACTIVE_LOW_Pos           0U
#define DSI_DATAEN_ACTIVE_LOW_Msk           (1U << DSI_DATAEN_ACTIVE_LOW_Pos)
#define DSI_VSYNC_ACTIVE_LOW_Pos            1U
#define DSI_VSYNC_ACTIVE_LOW_Msk            (1U << DSI_VSYNC_ACTIVE_LOW_Pos)
#define DSI_HSYNC_ACTIVE_LOW_Pos            2U
#define DSI_HSYNC_ACTIVE_LOW_Msk            (1U << DSI_HSYNC_ACTIVE_LOW_Pos)
#define DSI_SHUTD_ACTIVE_LOW_Pos            3U
#define DSI_SHUTD_ACTIVE_LOW_Msk            (1U << DSI_SHUTD_ACTIVE_LOW_Pos)
#define DSI_COLORM_ACTIVE_LOW_Pos           4U
#define DSI_COLORM_ACTIVE_LOW_Msk           (1U << DSI_COLORM_ACTIVE_LOW_Pos)

/*MODE_CFG register bits parameters*/
#define DSI_CMD_VIDEO_MODE_Pos              0U
#define DSI_CMD_VIDEO_MODE_Msk              (1U << DSI_CMD_VIDEO_MODE_Pos)

/*VID_MODE_CFG register bits parameters*/
#define DSI_VID_MODE_TYPE_Pos               0U
#define DSI_VID_MODE_TYPE_Msk               MASK(1,0)
#define DSI_LP_CMD_EN_Pos                   8U
#define DSI_LP_CMD_EN_Msk                   MASK(13,8)

/*VID_PKT_SIZE register bits parameters*/
#define DSI_VID_PKT_SIZE_Pos                0U
#define DSI_VID_PKT_SIZE_Msk                MASK(13,0)

/*VID_NUM_CHUNKS register bits parameters*/
#define DSI_VID_NUM_CHUNKS_Pos              0U
#define DSI_VID_NUM_CHUNKS_Msk              MASK(12,0)

/*VID_NULL_SIZE register bits parameters*/
#define DSI_VID_NULL_SIZE_Pos               0U
#define DSI_VID_NULL_SIZE_Msk               MASK(12,0)

/*VID_HSA_TIME register bits parameters*/
#define DSI_VID_HSA_TIME_Pos                0U
#define DSI_VID_HSA_TIME_Msk                MASK(11,0)

/*VID_HBP_TIME register bits parameters*/
#define DSI_VID_HBP_TIME_Pos                0U
#define DSI_VID_HBP_TIME_Msk                MASK(11,0)

/*VID_HLINE_TIME register bits parameters*/
#define DSI_VID_HLINE_TIME_Pos              0U
#define DSI_VID_HLINE_TIME_Msk              MASK(14,0)

/*VID_VSA_LINES register bits parameters*/
#define DSI_VSA_LINES_Pos                   0U
#define DSI_VSA_LINES_Msk                   MASK(9,0)

/*VID_VBP_LINES register bits parameters*/
#define DSI_VBP_LINES_Pos                   0U
#define DSI_VBP_LINES_Msk                   MASK(9,0)

/*VID_VFP_LINES register bits parameters*/
#define DSI_VFP_LINES_Pos                   0U
#define DSI_VFP_LINES_Msk                   MASK(9,0)

/*VID_VACTIVE_LINES register bits parameters*/
#define DSI_V_ACTIVE_LINES_Pos              0U
#define DSI_V_ACTIVE_LINES_Msk              MASK(13,0)

/*CMD_MODE_CFG register bits parameters*/
#define DSI_GEN_SW_0P_TX_Pos                8U
#define DSI_GEN_SW_0P_TX_Msk                (1U << DSI_GEN_SW_0P_TX_Pos)
#define DSI_GEN_SW_1P_TX_Pos                9U
#define DSI_GEN_SW_1P_TX_Msk                (1U << DSI_GEN_SW_1P_TX_Pos)
#define DSI_GEN_SW_2P_TX_Pos                10U
#define DSI_GEN_SW_2P_TX_Msk                (1U << DSI_GEN_SW_2P_TX_Pos)
#define DSI_GEN_SR_0P_TX_Pos                11U
#define DSI_GEN_SR_0P_TX_Msk                (1U << DSI_GEN_SR_0P_TX_Pos)
#define DSI_GEN_SR_1P_TX_Pos                12U
#define DSI_GEN_SR_1P_TX_Msk                (1U << DSI_GEN_SR_1P_TX_Pos)
#define DSI_GEN_SR_2P_TX_Pos                13U
#define DSI_GEN_SR_2P_TX_Msk                (1U << DSI_GEN_SR_2P_TX_Pos)
#define DSI_GEN_LW_TX_Pos                   14U
#define DSI_GEN_LW_TX_Msk                   (1U << DSI_GEN_LW_TX_Pos)
#define DSI_DCS_SW_0P_TX_Pos                16U
#define DSI_DCS_SW_0P_TX_Msk                (1U << DSI_DCS_SW_0P_TX_Pos)
#define DSI_DCS_SW_1P_TX_Pos                17U
#define DSI_DCS_SW_1P_TX_Msk                (1U << DSI_DCS_SW_1P_TX_Pos)
#define DSI_DCS_SR_0P_TX_Pos                18U
#define DSI_DCS_SR_0P_TX_Msk                (1U << DSI_DCS_SR_0P_TX_Pos)
#define DSI_DCS_LW_TX_Pos                   19U
#define DSI_DCS_LW_TX_Msk                   (1U << DSI_DCS_LW_TX_Pos)
#define DSI_MAX_RD_PKT_SIZE_Pos             24U
#define DSI_MAX_RD_PKT_SIZE_Msk             (1U << DSI_MAX_RD_PKT_SIZE_Pos)

/*GEN_HDR register bits parameters*/
#define DSI_GEN_DT_Pos                      0U
#define DSI_GEN_DT_Msk                      MASK(5,0)
#define DSI_GEN_VC_Pos                      6U
#define DSI_GEN_VC_Msk                      MASK(7,6)
#define DSI_GEN_WC_LSBYTE_Pos               8U
#define DSI_GEN_WC_LSBYTE_Msk               MASK(15,8)
#define DSI_GEN_WC_MSBYTE_Pos               16U
#define DSI_GEN_WC_MSBYTE_Msk               MASK(23,16)

/*GEN_PLD_DATA register bits parameters*/
#define DSI_GEN_PLD_B1_Pos                  0U
#define DSI_GEN_PLD_B1_Msk                  MASK(7,0)
#define DSI_GEN_PLD_B2_Pos                  8U
#define DSI_GEN_PLD_B2_Msk                  MASK(15,8)
#define DSI_GEN_PLD_B3_Pos                  16U
#define DSI_GEN_PLD_B3_Msk                  MASK(23,16)
#define DSI_GEN_PLD_B4_Pos                  24U
#define DSI_GEN_PLD_B4_Msk                  MASK(31,24)

/*LPCLK_CTRL register bits parameters*/
#define DSI_PHY_TXREQUESTCLKHS_Pos          0U
#define DSI_PHY_TXREQUESTCLKHS_Msk          (1U << DSI_PHY_TXREQUESTCLKHS_Pos)
#define DSI_AUTO_CLKLANE_CTRL_Pos           1U
#define DSI_AUTO_CLKLANE_CTRL_Msk           (1U << DSI_AUTO_CLKLANE_CTRL_Pos)

/*PHY_TMR_LPCLK_CFG register bits parameters*/
#define DSI_PHY_CLKLP2HS_TIME_Pos           0U
#define DSI_PHY_CLKLP2HS_TIME_Msk           MASK(9,0)
#define DSI_PHY_CLKHS2LP_TIME_Pos           16U
#define DSI_PHY_CLKHS2LP_TIME_Msk           MASK(25,16)

/*PHY_TMR_CFG register bits parameters*/
#define DSI_PHY_LP2HS_TIME_Pos              0U
#define DSI_PHY_LP2HS_TIME_Msk              MASK(9,0)
#define DSI_PHY_HS2LP_TIME_Pos              16U
#define DSI_PHY_HS2LP_TIME_Msk              MASK(25,16)

/*INT_ST0 / INT_MSK0 / INT_FORCE0 register bits parameters*/
#define DSI_ACK_WITH_ERR_0_Pos              0U
#define DSI_ACK_WITH_ERR_0_Msk              (1U << DSI_ACK_WITH_ERR_0_Pos)
#define DSI_ACK_WITH_ERR_1_Pos              1U
#define DSI_ACK_WITH_ERR_1_Msk              (1U << DSI_ACK_WITH_ERR_1_Pos)
#define DSI_ACK_WITH_ERR_2_Pos              2U
#define DSI_ACK_WITH_ERR_2_Msk              (1U << DSI_ACK_WITH_ERR_2_Pos)
#define DSI_ACK_WITH_ERR_3_Pos              3U
#define DSI_ACK_WITH_ERR_3_Msk              (1U << DSI_ACK_WITH_ERR_3_Pos)
#define DSI_ACK_WITH_ERR_4_Pos              4U
#define DSI_ACK_WITH_ERR_4_Msk              (1U << DSI_ACK_WITH_ERR_4_Pos)
#define DSI_ACK_WITH_ERR_5_Pos              5U
#define DSI_ACK_WITH_ERR_5_Msk              (1U << DSI_ACK_WITH_ERR_5_Pos)
#define DSI_ACK_WITH_ERR_6_Pos              6U
#define DSI_ACK_WITH_ERR_6_Msk              (1U << DSI_ACK_WITH_ERR_6_Pos)
#define DSI_ACK_WITH_ERR_7_Pos              7U
#define DSI_ACK_WITH_ERR_7_Msk              (1U << DSI_ACK_WITH_ERR_7_Pos)
#define DSI_ACK_WITH_ERR_8_Pos              8U
#define DSI_ACK_WITH_ERR_8_Msk              (1U << DSI_ACK_WITH_ERR_8_Pos)
#define DSI_ACK_WITH_ERR_9_Pos              9U
#define DSI_ACK_WITH_ERR_9_Msk              (1U << DSI_ACK_WITH_ERR_9_Pos)
#define DSI_ACK_WITH_ERR_10_Pos             10U
#define DSI_ACK_WITH_ERR_10_Msk             (1U << DSI_ACK_WITH_ERR_10_Pos)
#define DSI_ACK_WITH_ERR_11_Pos             11U
#define DSI_ACK_WITH_ERR_11_Msk             (1U << DSI_ACK_WITH_ERR_11_Pos)
#define DSI_ACK_WITH_ERR_12_Pos             12U
#define DSI_ACK_WITH_ERR_12_Msk             (1U << DSI_ACK_WITH_ERR_12_Pos)
#define DSI_ACK_WITH_ERR_13_Pos             13U
#define DSI_ACK_WITH_ERR_13_Msk             (1U << DSI_ACK_WITH_ERR_13_Pos)
#define DSI_ACK_WITH_ERR_14_Pos             14U
#define DSI_ACK_WITH_ERR_14_Msk             (1U << DSI_ACK_WITH_ERR_14_Pos)
#define DSI_ACK_WITH_ERR_15_Pos             15U
#define DSI_ACK_WITH_ERR_15_Msk             (1U << DSI_ACK_WITH_ERR_15_Pos)
#define DSI_DPHY_ERRORS_0_Pos               16U
#define DSI_DPHY_ERRORS_0_Msk               (1U << DSI_DPHY_ERRORS_0_Pos)
#define DSI_DPHY_ERRORS_1_Pos               17U
#define DSI_DPHY_ERRORS_1_Msk               (1U << DSI_DPHY_ERRORS_1_Pos)
#define DSI_DPHY_ERRORS_2_Pos               18U
#define DSI_DPHY_ERRORS_2_Msk               (1U << DSI_DPHY_ERRORS_2_Pos)
#define DSI_DPHY_ERRORS_3_Pos               19U
#define DSI_DPHY_ERRORS_3_Msk               (1U << DSI_DPHY_ERRORS_3_Pos)
#define DSI_DPHY_ERRORS_4_Pos               20U
#define DSI_DPHY_ERRORS_4_Msk               (1U << DSI_DPHY_ERRORS_4_Pos)

/*INT_ST1 / INT_MSK1 / INT_FORCE1 register bits parameters*/
#define DSI_TO_HS_TX_Pos                    0U
#define DSI_TO_HS_TX_Msk                    (1U << DSI_TO_HS_TX_Pos)
#define DSI_TO_LP_RX_Pos                    1U
#define DSI_TO_LP_RX_Msk                    (1U << DSI_TO_LP_RX_Pos)
#define DSI_ECC_SINGLE_ERR_Pos              2U
#define DSI_ECC_SINGLE_ERR_Msk              (1U << DSI_ECC_SINGLE_ERR_Pos)
#define DSI_ECC_MILTI_ERR_Pos               3U
#define DSI_ECC_MILTI_ERR_Msk               (1U << DSI_ECC_MILTI_ERR_Pos)
#define DSI_CRC_ERR_Pos                     4U
#define DSI_CRC_ERR_Msk                     (1U << DSI_CRC_ERR_Pos)
#define DSI_PKT_SIZE_ERR_Pos                5U
#define DSI_PKT_SIZE_ERR_Msk                (1U << DSI_PKT_SIZE_ERR_Pos)
#define DSI_EOPT_ERR_Pos                    6U
#define DSI_EOPT_ERR_Msk                    (1U << DSI_EOPT_ERR_Pos)
#define DSI_DPI_PLD_WR_ERR_Pos              7U
#define DSI_DPI_PLD_WR_ERR_Msk              (1U << DSI_DPI_PLD_WR_ERR_Pos)
#define DSI_GEN_CMD_WR_ERR_Pos              8U
#define DSI_GEN_CMD_WR_ERR_Msk              (1U << DSI_GEN_CMD_WR_ERR_Pos)
#define DSI_GEN_PLD_WR_ERR_Pos              9U
#define DSI_GEN_PLD_WR_ERR_Msk              (1U << DSI_GEN_PLD_WR_ERR_Pos)
#define DSI_GEN_PLD_SEND_ERR_Pos            10U
#define DSI_GEN_PLD_SEND_ERR_Msk            (1U << DSI_GEN_PLD_SEND_ERR_Pos)
#define DSI_GEN_PLD_RD_ERR_Pos              11U
#define DSI_GEN_PLD_RD_ERR_Msk              (1U << DSI_GEN_PLD_RD_ERR_Pos)
#define DSI_GEN_PLD_RECEV_ERR_Pos           12U
#define DSI_GEN_PLD_RECEV_ERR_Msk           (1U << DSI_GEN_PLD_RECEV_ERR_Pos)
#define DSI_DPI_BUFF_PLD_UNDER_Pos          19U
#define DSI_DPI_BUFF_PLD_UNDER_Msk          (1U << DSI_DPI_BUFF_PLD_UNDER_Pos)
#define DSI_TEAR_REQUEST_ERR_Pos            20U
#define DSI_TEAR_REQUEST_ERR_Msk            (1U << DSI_TEAR_REQUEST_ERR_Pos)

/*PHY_RSTZ register bits parameters*/
#define DSI_PHY_SHUTDOWNZ_Pos               0U
#define DSI_PHY_SHUTDOWNZ_Msk               (1U << DSI_PHY_SHUTDOWNZ_Pos)
#define DSI_PHY_RSTZ_Pos                    1U
#define DSI_PHY_RSTZ_Msk                    (1U << DSI_PHY_RSTZ_Pos)
#define DSI_PHY_ENABLECLK_Pos               2U
#define DSI_PHY_ENABLECLK_Msk               (1U << DSI_PHY_ENABLECLK_Pos)
#define DSI_PHY_FORCEPLL_Pos                3U
#define DSI_PHY_FORCEPLL_Msk                (1U << DSI_PHY_FORCEPLL_Pos)

/*PHY_IF_CFG register bits parameters*/
#define DSI_PHY_N_LANE_Pos                  0U
#define DSI_PHY_N_LANE_Msk                  MASK(1,0)
#define DSI_PHY_STOP_WAIT_TIME_Pos          8U
#define DSI_PHY_STOP_WAIT_TIME_Msk          MASK(15,8)

/*PHY_STATUS register bits parameters*/
#define DSI_PHY_LOCK_Pos                    0U
#define DSI_PHY_LOCK_Msk                    (1U << DSI_PHY_LOCK_Pos)
#define DSI_PHY_STOPSTATECLKLANE_Pos        2U
#define DSI_PHY_STOPSTATECLKLANE_Msk        (1U << DSI_PHY_STOPSTATECLKLANE_Pos)
#define DSI_PHY_STOPSTATELANE_0_Pos         4U
#define DSI_PHY_STOPSTATELANE_0_Msk         (1U << DSI_PHY_STOPSTATELANE_0_Pos)
#define DSI_PHY_STOPSTATELANE_1_Pos         7U
#define DSI_PHY_STOPSTATELANE_1_Msk         (1U << DSI_PHY_STOPSTATELANE_1_Pos)

/*MIPI DSI helper macros*/
#define MIPI_DSI_SET_BIT(REG, BIT)          ((REG) |= (BIT))
#define MIPI_DSI_CLEAR_BIT(REG, BIT)        ((REG) &= ~(BIT))
#define MIPI_DSI_READ_BIT(REG, BIT)         ((REG) & (BIT))
#define MIPI_DSI_CLEAR_REG(REG)             ((REG) = (0x0))
#define MIPI_DSI_WRITE_REG(REG, VAL)        ((REG) = (VAL))
#define MIPI_DSI_READ_REG(REG)              ((REG))

/*Interrupt event*/
#define DSI_MASK_ACK_ERROR_EVENT            (DSI_ACK_WITH_ERR_0_Msk  | \
                                             DSI_ACK_WITH_ERR_1_Msk  | \
                                             DSI_ACK_WITH_ERR_2_Msk  | \
                                             DSI_ACK_WITH_ERR_3_Msk  | \
                                             DSI_ACK_WITH_ERR_4_Msk  | \
                                             DSI_ACK_WITH_ERR_5_Msk  | \
                                             DSI_ACK_WITH_ERR_6_Msk  | \
                                             DSI_ACK_WITH_ERR_7_Msk  | \
                                             DSI_ACK_WITH_ERR_8_Msk  | \
                                             DSI_ACK_WITH_ERR_9_Msk  | \
                                             DSI_ACK_WITH_ERR_10_Msk | \
                                             DSI_ACK_WITH_ERR_11_Msk | \
                                             DSI_ACK_WITH_ERR_12_Msk | \
                                             DSI_ACK_WITH_ERR_13_Msk | \
                                             DSI_ACK_WITH_ERR_14_Msk | \
                                             DSI_ACK_WITH_ERR_15_Msk)

#define DSI_MASK_PHY_ERROR_EVENT            (DSI_DPHY_ERRORS_0_Msk   | \
                                             DSI_DPHY_ERRORS_1_Msk   | \
                                             DSI_DPHY_ERRORS_2_Msk   | \
                                             DSI_DPHY_ERRORS_3_Msk   | \
                                             DSI_DPHY_ERRORS_4_Msk)

#define DSI_MASK_PKT_ERROR_EVENT            (DSI_TO_HS_TX_Msk        | \
                                             DSI_TO_LP_RX_Msk        | \
                                             DSI_ECC_SINGLE_ERR_Msk  | \
                                             DSI_ECC_MILTI_ERR_Msk   | \
                                             DSI_CRC_ERR_Msk         | \
                                             DSI_PKT_SIZE_ERR_Msk    | \
                                             DSI_EOPT_ERR_Msk)

#define DSI_MASK_DPI_ERROR_EVENT            (DSI_DPI_PLD_WR_ERR_Msk    | \
                                             DSI_GEN_CMD_WR_ERR_Msk    | \
                                             DSI_GEN_PLD_WR_ERR_Msk    | \
                                             DSI_GEN_PLD_SEND_ERR_Msk  | \
                                             DSI_GEN_PLD_RD_ERR_Msk    | \
                                             DSI_GEN_PLD_RECEV_ERR_Msk | \
                                             DSI_TEAR_REQUEST_ERR_Msk)

/*DCS Data types and Command*/
#define DSI_DCS_SHORT_WRITE_NODATA_TYPE      0x05
#define DSI_DCS_SHORT_WRITE_DATA_TYPE        0x15
#define DSI_DCS_LONG_WRITE_DATA_TYPE         0x39
#define DSI_DCS_LONG_WRITE_DATA_LEN          0x06


/**
  \brief MIPI DSI Driver flags
  */
typedef enum {
	DSI_FLAG_DRV_INIT_DONE   = (1U << 0),
	DSI_FLAG_POWER_ENABLED   = (1U << 1),
	DSI_FLAG_HOST_CONFIGURED = (1U << 2),
	DSI_FLAG_DPI_CONFIGURED  = (1U << 3),
	DSI_FLAG_CMD_MODE_DONE   = (1U << 4),
	DSI_FLAG_VID_MODE_DONE   = (1U << 5),
}DSI_FLAG_Type;

/**
  \brief MIPI DSI Number of data lanes
  */
typedef enum {
	MIPI_DSI_ONE_LANE,
	MIPI_DSI_TWO_LANE,
}DSI_N_LANES_Type;

/**
  \brief MIPI DSI DPI polarity
  */
typedef enum {
	MIPI_DSI_ACTIVE_HIGH,
	MIPI_DSI_ACTIVE_LOW,
}DSI_POL_Type;


/**
  \brief MIPI DSI virtual channel ID
  */
typedef enum {
	MIPI_DSI_VC_ID_0,
	MIPI_DSI_VC_ID_1,
	MIPI_DSI_VC_ID_2,
	MIPI_DSI_VC_ID_3,
}DSI_VC_ID_Type;

/**
  \brief MIPI DSI Video mode
  */
typedef enum {
	MIPI_DSI_NON_BURST_SYNC_PULSES,
	MIPI_DSI_NON_BURST_SYNC_EVENTS,
	MIPI_DSI_BURST_MODE,
}DSI_VID_MODE_Type;

/**
  \brief MIPI DSI IPI frame info
  */
typedef struct {
	uint32_t hsa_time;
	uint32_t hbp_time;
	uint32_t hfp_time;
	uint32_t hactive_time;
	uint32_t vsa_line;
	uint32_t vbp_line;
	uint32_t vfp_line;
	uint32_t vactive_line;
}MIPI_DSI_FRAME_INFO;

/**
  \brief MIPI DSI DPI signal polarity.
  */
typedef struct {
	uint8_t dataen_active_low;
	uint8_t vsync_active_low;
	uint8_t hsync_active_low;
	uint8_t shutd_active_low;
	uint8_t colorm_active_low;
}MIPI_DSI_DPI_POL;

/**
  \brief MIPI DSI DPI Info
  */
typedef struct {
	uint8_t             color_code;
	DSI_VID_MODE_Type   vid_mode;
	uint32_t            vid_pkt_size;
	uint32_t            vid_num_chunks;
	uint32_t            vid_null_size;
	MIPI_DSI_DPI_POL    *dpi_pol;
	MIPI_DSI_FRAME_INFO *frame_info;
}MIPI_DSI_DPI_CFG;

/**
  \brief MIPI DSI DPHY TIME CFG
  */
typedef struct {
	uint32_t clklp2hs_time;
	uint32_t clkhs2lp_time;
	uint32_t lp2hs_time;
	uint32_t hs2lp_time;
}MIPI_DSI_DPHY_TMR_CFG;

/**
  \brief MIPI DSI driver info
  */
typedef struct {
	MIPI_DSI_Type                   *reg_base;
	ARM_MIPI_DSI_SignalEvent_t      cb_event;
	DSI_N_LANES_Type                n_lanes;
	DSI_VC_ID_Type                  vc_id;
	uint32_t                        tx_ecs_clk_div;
	MIPI_DSI_DPHY_TMR_CFG           *dphy_tmr;
	MIPI_DSI_DPI_CFG                *dpi_cfg;
	IRQn_Type                       irq;
	uint32_t                        irq_priority;
	uint32_t                        int_st0;
	uint32_t                        int_st1;
	uint8_t                         flags;
}MIPI_DSI_DRV_INFO;

#endif /* MIPI_DSI_DEV_H_ */
