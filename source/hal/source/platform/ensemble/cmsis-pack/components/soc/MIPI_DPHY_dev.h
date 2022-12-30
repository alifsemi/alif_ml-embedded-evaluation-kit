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
 * @file     MIPI_DPHY_dev.h
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     24-Feb-2022
 * @brief    Device Specific Header file for MIPI DPHY Driver.
 ******************************************************************************/

#ifndef MIPI_DPHY_DEV_H_
#define MIPI_DPHY_DEV_H_


/*CFG_CLK frequency*/
#define DPHY_FCFG_CLOCK_MHZ                           25
/*Input Reference Clock Frequency*/
#define DPHY_FCLKIN_MHZ                               38.4
/*Charge-pump Programmability*/
#define DPHY_CPBIAS_CNTRL                             0x00
#define DPHY_GMP_CNTRL                                0x01
#define DPHY_INT_CNTRL                                0x04
#define DPHY_PROP_CNTRL                               0x10
#define DPHY_CB_VREF_MPLL_REG_REL_RW                  0x02
/*Oscillation target for slew rate calibration*/
#define DPHY_LESS_THEN_1GBPS_SR_OSC_FREQ_TARGET       657
#define DPHY_MORE_THEN_1GBPS_SR_OSC_FREQ_TARGET       920
/*PLL input division factor*/
#define DPHY_DEFAULT_PLL_INPUT_DIV_FACTOR_N           3
/*Minimum DPHY clock frequency*/
#define DPHY_MINIMUM_FREQUENCY                        40000000

/*DPHY TX register index*/
#define dphy4txtester_DIG_RD_TX_SYS_0                 (0x01EU)
#define dphy4txtester_DIG_RD_TX_SYS_1                 (0x01FU)
#define dphy4txtester_DIG_RDWR_TX_CB_0                (0x1AAU)
#define dphy4txtester_DIG_RDWR_TX_CB_1                (0x1ABU)
#define dphy4txtester_DIG_RDWR_TX_CB_2                (0x1ACU)
#define dphy4txtester_DIG_RDWR_TX_CB_3                (0x1ADU)
#define dphy4txtester_DIG_RDWR_TX_SLEW_5              (0x270U)
#define dphy4txtester_DIG_RDWR_TX_SLEW_6              (0x271U)
#define dphy4txtester_DIG_RDWR_TX_SLEW_7              (0x272U)
#define dphy4txtester_DIG_RDWR_TX_CLK_TERMLOWCAP      (0x402U)
#define dphy4txtester_DIG_RDWR_TX_LANE1_SLEWRATE_0    (0x70BU)
#define dphy4txtester_DIG_RDWR_TX_LANE2_SLEWRATE_0    (0x90BU)
#define dphy4txtester_DIG_RDWR_TX_LANE3_SLEWRATE_0    (0xB0BU)

/*TX PLL register index*/
#define dphy4txtester_DIG_RDWR_TX_PLL_1               (0x15EU)
#define dphy4txtester_DIG_RDWR_TX_PLL_5               (0x162U)
#define dphy4txtester_DIG_RDWR_TX_PLL_13              (0x16AU)
#define dphy4txtester_DIG_RDWR_TX_PLL_17              (0x16EU)
#define dphy4txtester_DIG_RDWR_TX_PLL_27              (0x178U)
#define dphy4txtester_DIG_RDWR_TX_PLL_28              (0x179U)
#define dphy4txtester_DIG_RDWR_TX_PLL_29              (0x17AU)
#define dphy4txtester_DIG_RDWR_TX_PLL_30              (0x17BU)
#define dphy4txtester_DIG_RD_TX_PLL_0                 (0x191U)

/*DPHY RX register index*/
#define dphy4rxtester_DIG_RD_RX_SYS_1                 (0x01FU)
#define dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_2    (0x0E2U)
#define dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_3    (0x0E3U)
#define dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_4    (0x0E4U)
#define dphy4rxtester_DIG_RDWR_RX_RX_STARTUP_OVR_17   (0x0F1U)
#define dphy4rxtester_DIG_RDWR_RX_CLKLANE_LANE_6      (0x307U)
#define dphy4rxtester_DIG_RD_RX_CLKLANE_OFFSET_CAL_3  (0x39CU)
#define dphy2rxtester_DIG_RD_RX_CLKLANE_OFFSET_CAL_0  (0x39DU)
#define dphy2rxtester_DIG_RD_RX_LANE0_OFFSET_CAL_0    (0x58DU)
#define dphy2rxtester_DIG_RD_RX_LANE0_OFFSET_CAL_2    (0x5A1U)
#define dphy2rxtester_DIG_RD_RX_LANE0_DDL_0           (0x5E0U)
#define dphy2rxtester_DIG_RD_RX_LANE0_DDL_5           (0x5E5U)
#define dphy2rxtester_DIG_RD_RX_LANE1_OFFSET_CAL_0    (0x79FU)
#define dphy2rxtester_DIG_RD_RX_LANE1_OFFSET_CAL_2    (0x7A1U)
#define dphy4rxtester_DIG_RD_RX_LANE1_DDL_0           (0x7E0U)
#define dphy2rxtester_DIG_RD_RX_LANE1_DDL_5           (0x7E5U)

/*DPHY power and clock configuration parameters*/
#define MIPI_DPHY_PLL_LDO_EN_Pos                      10U
#define MIPI_DPHY_PLL_LDO_EN_Msk                      (1U << MIPI_DPHY_PLL_LDO_EN_Pos)

#define CLK_XTAL_SEL_Pos                              1U
#define CLK_XTAL_SEL_Msk                              (1U << CLK_XTAL_SEL_Pos)

#define XO_DIGITAL_CLOCK_Pos                          5U
#define XO_DIGITAL_CLOCK_Msk                          (1U << XO_DIGITAL_CLOCK_Pos)

/*PHY_TEST_CTRL0 register bits parameters*/
#define PHY_TESTCLR_Pos                               0U
#define PHY_TESTCLR_Msk                               (1U << PHY_TESTCLR_Pos)
#define PHY_TESTCLK_Pos                               1U
#define PHY_TESTCLK_Msk                               (1U << PHY_TESTCLK_Pos)

/*PHY_TEST_CTRL1 register bits parameters*/
#define PHY_TESTDIN_Pos                               0U
#define PHY_TESTDIN_Msk                               MASK(7,0)
#define PHY_TESTDOUT_Pos                              8U
#define PHY_TESTDOUT_Msk                              MASK(15,8)
#define PHY_TESTEN_Pos                                16U
#define PHY_TESTEN_Msk                                (1U << PHY_TESTEN_Pos)

/*MIPI DPHY helper macros*/
#define MIPI_DPHY_SET_BIT(REG, BIT_Msk)               SET_BIT(REG, BIT_Msk)
#define MIPI_DPHY_CLEAR_BIT(REG, BIT_Msk)             CLEAR_BIT(REG, BIT_Msk)
#define MIPI_DPHY_READ_BIT(REG, BIT_Msk)              READ_BIT(REG, BIT_Msk)
#define MIPI_DPHY_CLEAR_REG(REG)                      CLEAR_REG(REG)
#define MIPI_DPHY_WRITE_REG(REG, VAL)                 WRITE_REG(REG, VAL)
#define MIPI_DPHY_READ_REG(REG)                       READ_REG(REG)

/**
\brief MIPI PHY physical lanes stop state status
*/
typedef enum {
	DPHY_STOPSTATE_LANE0 = (1U << 0),
	DPHY_STOPSTATE_LANE1 = (1U << 1),
	DPHY_STOPSTATE_CLOCK = (1U << 2),
}DPHY_STOPSTATE_Type;

/**
\brief MIPI PHY PLL lock status
*/
typedef enum {
	DPHY_NO_PLL_LOCK,
	DPHY_PLL_LOCK,
}DPHY_PLL_Status;

/**
\brief DPHY initialization status
*/
typedef enum {
	DPHY_UNINITIALIZED,
	DPHY_INITIALIZED,
}DPHY_INIT_Status;

/**
\brief MIPI PHY clock lane supports LP mode or not
*/
typedef enum {
	CONTINUOUS_CLOCK_MODE,
	NON_CONTINUOUS_CLOCK_MODE,
}DPHY_CLK_Mode;

/**
\brief MIPI PHY mode(CIS2/DSI)
*/
typedef enum {
	DPHY_DSI,
	DPHY_CSI2,
}DPHY_Mode;

/**
\brief hsfreqrange and osc_freq_target range
*/
typedef struct {
	uint32_t bitrate_in_mbps;
	uint8_t  hsfreqrange;
	uint32_t osc_freq_target;
}ARM_DPHY_FREQ_RANGE;

/**
\brief PLL vco_cntrl range
*/
typedef struct {
	float frequency_mhz;
	int8_t   vco_ctrl;
}ARM_DPHY_PLL_VCO_CTRL;

/**
\brief PLL Output division factor range
*/
typedef struct {
	float frequency_mhz;
	int8_t   p;
}ARM_DPHY_PLL_OUTPUT_DIVISION_FACTOR;

#endif /* DPHY_MIPI_DPHY_DEV_H_ */
