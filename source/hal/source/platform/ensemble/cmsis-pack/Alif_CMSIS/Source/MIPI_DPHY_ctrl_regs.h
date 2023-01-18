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
 * @file     MIPI_DPHY_ctrl_regs.h
 * @author   Prasanna Ravi
 * @email    prasanna.ravi@alifsemi.com
 * @version  V1.0.0
 * @date     6-Jan-2022
 * @brief    Device Specific Header file for MIPI DPHY Driver.
 ******************************************************************************/

#ifndef INCLUDE_MIPI_DPHY_CTRL_REGS_H_
#define INCLUDE_MIPI_DPHY_CTRL_REGS_H_

#include "RTE_Components.h"
#include "system_utils.h"
#include CMSIS_device_header


typedef struct {
	__IOM uint32_t dphy_pll_ctrl0;
	__IOM uint32_t dphy_pll_ctrl1;
	__IOM uint32_t dphy_pll_ctrl2;
	__IM  uint32_t reserved0;
	__OM  uint32_t dphy_pll_stat0;
	__OM  uint32_t dphy_pll_stat1;
	__IM  uint32_t reserved1[2];
	__IOM uint32_t tx_dphy_ctrl0;
	__IOM uint32_t tx_dphy_ctrl1;
	__IOM uint32_t rx_dphy_ctrl0;
	__IOM uint32_t rx_dphy_ctrl1;
}MIPI_DPHY_ctrl_cfg_Type;


#define MIPI_DPHY_CTRL_CFG_BASE             ((MIPI_DPHY_ctrl_cfg_Type *)(CFGSLV1_BASE + 0x10))

/*Register definition*/

/*dphy_pll_ctrl0 register bits parameters*/
#define PLL_SOC_FORCE_LOCK_Pos              0U
#define PLL_SOC_FORCE_LOCK_Msk              (1U << PLL_SOC_FORCE_LOCK_Pos)
#define PLL_SOC_SHADOW_CONTROL_Pos          4U
#define PLL_SOC_SHADOW_CONTROL_Msk          (1U << PLL_SOC_SHADOW_CONTROL_Pos)
#define PLL_SOC_UPDATEPLL_Pos               8U
#define PLL_SOC_UPDATEPLL_Msk               (1U << PLL_SOC_UPDATEPLL_Pos)
#define PLL_SOC_SHADOW_CLEAR_Pos            12U
#define PLL_SOC_SHADOW_CLEAR_Msk            (1U << PLL_SOC_SHADOW_CLEAR_Pos)
#define PLL_SOC_GMP_CNTRL_Pos               16U
#define PLL_SOC_GMP_CNTRL_Msk               MASK(17,16)
#define PLL_SOC_CLKSEL_Pos                  20U
#define PLL_SOC_CLKSEL_Msk                  MASK(21,20)
#define PLL_SOC_GP_CLK_EN_Pos               24U
#define PLL_SOC_GP_CLK_EN_Msk               (1U << PLL_SOC_GP_CLK_EN_Pos)

/*dphy_pll_ctrl1 register bits parameters*/
#define PLL_SOC_M_Pos                       0U
#define PLL_SOC_M_Msk                       MASK(9,0)
#define PLL_SOC_N_Pos                       12U
#define PLL_SOC_N_Msk                       MASK(15,12)

/*dphy_pll_ctrl2 register bits parameters*/
#define PLL_SOC_CPBIAS_CNTRL_Pos            0U
#define PLL_SOC_CPBIAS_CNTRL_Msk            MASK(6,0)
#define PLL_SOC_INT_CNTRL_Pos               8U
#define PLL_SOC_INT_CNTRL_Msk               MASK(13,8)
#define PLL_SOC_PROP_CNTRL_Pos              16U
#define PLL_SOC_PROP_CNTRL_Msk              MASK(21,16)
#define PLL_SOC_VCO_CNTRL_Pos               24U
#define PLL_SOC_VCO_CNTRL_Msk               MASK(29,24)

/*dphy_pll_stat0 register bits parameters*/
#define PLL_SOC_LOCK_STATUS_Pos             0U
#define PLL_SOC_LOCK_STATUS_Msk             (1U << PLL_SOC_LOCK_STATUS_Pos)
#define PLL_SOC_SHADOW_CONTROL_STATUS_Pos   4U
#define PLL_SOC_SHADOW_CONTROL_STATUS_Msk   (1U << PLL_SOC_SHADOW_CONTROL_STATUS_Pos)
#define PLL_SOC_GMP_CNTRL_STATUS_Pos        8U
#define PLL_SOC_GMP_CNTRL_STATUS_Msk        MASK(9,8)
#define PLL_SOC_M_STATUS_Pos                16U
#define PLL_SOC_M_STATUS_Msk                MASK(25,16)
#define PLL_SOC_N_STATUS_Pos                28U
#define PLL_SOC_N_STATUS_Msk                MASK(31,28)

/*dphy_pll_stat1 register bits parameters*/
#define PLL_SOC_CPBIAS_CNTRL_STATUS_Pos     0U
#define PLL_SOC_CPBIAS_CNTRL_STATUS_Msk     MASK(6,0)
#define PLL_SOC_INT_CNTRL_STATUS_Pos        8U
#define PLL_SOC_INT_CNTRL_STATUS_Msk        MASK(13,8)
#define PLL_SOC_PROP_CNTRL_STATUS_Pos       16U
#define PLL_SOC_PROP_CNTRL_STATUS_Msk       MASK(21,16)
#define PLL_SOC_VCO_CNTRL_STATUS_Pos        24U
#define PLL_SOC_VCO_CNTRL_STATUS_Msk        MASK(29,24)

/*dphy_ctrl0 register bits parameters*/
#define BIST_ON_Pos                         0U
#define BIST_ON_Msk                         (1U << BIST_ON_Pos)
#define BIST_DONE_Pos                       1U
#define BIST_DONE_Msk                       (1U << BIST_DONE_Pos)
#define BIST_OK_Pos                         2U
#define BIST_OK_Msk                         (1U << BIST_OK_Pos)
#define CONT_EN_Pos                         3U
#define CONT_EN_Msk                         (1U << CONT_EN_Pos)
#define TESTPORT_SEL_Pos                    4U
#define TESTPORT_SEL_Msk                    (1U << TESTPORT_SEL_Pos)
#define TXRXZ_Pos                           8U
#define TXRXZ_Msk                           (1U << TXRXZ_Pos)
#define BASEDIR_Pos                         12U
#define BASEDIR_Msk                         MASK(13,12)
#define HSFREQRANGE_Pos                     16U
#define HSFREQRANGE_Msk                     MASK(22,16)
#define CFGCLKFREQRANGE_Pos                 24U
#define CFGCLKFREQRANGE_Msk                 MASK(31,24)

/*dphy_ctrl1 register bits parameters*/
#define FORCERXMODE_Pos                     0U
#define FORCERXMODE_Msk                     MASK(1,0)
#define FORCETXSTOPMODE_Pos                 2U
#define FORCETXSTOPMODE_Msk                 MASK(3,2)
#define TURNDISABLE_Pos                     4U
#define TURNDISABLE_Msk                     MASK(5,4)
#define TURNREQUEST_Pos                     6U
#define TURNREQUEST_Msk                     MASK(7,6)


/*mipi_dphy_ctrl_cfg helper macros*/
#define MIPI_DPHY_CTRL_CFG_SET_BIT(REG, BIT)      SET_BIT(REG, BIT)
#define MIPI_DPHY_CTRL_CFG_CLEAR_BIT(REG, BIT)    CLEAR_BIT(REG, BIT)
#define MIPI_DPHY_CTRL_CFG_READ_BIT(REG, BIT)     READ_BIT(REG, BIT)
#define MIPI_DPHY_CTRL_CFG_CLEAR_REG(REG)         CLEAR_REG(REG)
#define MIPI_DPHY_CTRL_CFG_WRITE_REG(REG, VAL)    WRITE_REG(REG, VAL)
#define MIPI_DPHY_CTRL_CFG_READ_REG(REG)          READ_REG(REG)

#endif /* INCLUDE_MIPI_DPHY_CTRL_REGS_H_ */
