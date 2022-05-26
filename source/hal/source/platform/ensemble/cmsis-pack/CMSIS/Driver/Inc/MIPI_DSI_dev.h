/* Copyright (c) 2022 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.
 *
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 ---------------------------------------------------------------------------*/

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

typedef struct {
	__IM  uint32_t reserved[40];
	__IOM uint32_t PHY_RSTZ;
	__IOM uint32_t PHY_IF_CFG;
	__IOM uint32_t PHY_ULPS_CTRL;
	__IOM uint32_t PHY_TX_TRIGGERS;
	__IM  uint32_t PHY_STATUS;
	__IOM uint32_t PHY_TST_CTRL0;
	__IOM uint32_t PHY_TST_CTRL1;
	__IM  uint32_t reserved1[127];
}MIPI_DSI_Type;

#define MIPI_DSI                            ((MIPI_DSI_Type*)MIPI_DSI_BASE)

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

/*PHY_TEST_CTRL0 register bits parameters*/
#define DSI_PHY_TESTCLR_Pos                 0U
#define DSI_PHY_TESTCLR_Msk                 (1U << DSI_PHY_TESTCLR_Pos)
#define DSI_PHY_TESTCLK_Pos                 1U
#define DSI_PHY_TESTCLK_Msk                 (1U << DSI_PHY_TESTCLK_Pos)

/*PHY_TEST_CTRL1 register bits parameters*/
#define DSI_PHY_TESTDIN_Pos                 0U
#define DSI_PHY_TESTDIN_Msk                 MASK(7,0)
#define DSI_PHY_TESTDOUT_Pos                8U
#define DSI_PHY_TESTDOUT_Msk                MASK(15,8)
#define DSI_PHY_TESTEN_Pos                  16U
#define DSI_PHY_TESTEN_Msk                  (1U << DSI_PHY_TESTEN_Pos)

/*MIPI DSI helper macros*/
#define MIPI_DSI_SET_BIT(REG, BIT)          ((REG) |= (BIT))
#define MIPI_DSI_CLEAR_BIT(REG, BIT)        ((REG) &= ~(BIT))
#define MIPI_DSI_READ_BIT(REG, BIT)         ((REG) & (BIT))
#define MIPI_DSI_CLEAR_REG(REG)             ((REG) = (0x0))
#define MIPI_DSI_WRITE_REG(REG, VAL)        ((REG) = (VAL))
#define MIPI_DSI_READ_REG(REG)              ((REG))

/**
  \brief MIPI DSI Driver flags
  */
typedef enum {
	DSI_FLAG_DRV_INIT_DONE   = (1U << 0),
	DSI_FLAG_CLKSRC_ENABLED  = (1U << 1),
}DSI_FLAG_Type;

/**
  \brief MIPI DSI driver info
  */
typedef struct {
	MIPI_DSI_Type   *reg_base;
	uint8_t         n_lanes;
	uint8_t         flags;
}MIPI_DSI_DRV_INFO;

#endif /* MIPI_DSI_DEV_H_ */
