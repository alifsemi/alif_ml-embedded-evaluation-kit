/*
 * Private header file for NetXDuo PHY driver
 *
 * Author   : Silesh C V <silesh@alifsemi.com>
 *
 * Copyright (C) 2021 ALIF SEMICONDUCTOR
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/** \file nx_eth_phy_ctrl.h
    \brief Private header file for NetXDuo PHY driver.
*/
#ifndef NX_ETH_PHY_CTRL_H
#define NX_ETH_PHY_CTRL_H

#include "nx_eth_phy.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
  \struct PHY_CTRL
  \brief  Private data structure used by the PHY driver to represent a PHY.
*/
typedef struct PHY_CTRL_STRUCT {
    uint32_t device_id; /**< PHY Device ID */
    phy_read_t read;    /**< Read function pointer to access PHY registers */
    phy_write_t write;  /**< Write function pointer to access PHY registers */
    uint8_t id;         /**< The PHY address */
    uint8_t an_status;  /**< Auto negotiation status */
    uint8_t flags;      /**< flags, used to keep track of the PHY state */
} PHY_CTRL;

/* phy flags */
#define PHY_INITIALIZED			0x1 /**< The PHY has been initialized */
#define AN_ENABLED              0x1 /**< User has enabled auto negotiation */

#ifdef  __cplusplus
}
#endif
#endif /* NX_ETH_PHY_CTRL_H */
