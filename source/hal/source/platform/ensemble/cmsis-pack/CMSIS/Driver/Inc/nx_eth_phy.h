/*
 * NetXDuo Ethernet PHY driver
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

/** \file nx_eth_phy.h
    \brief Public headerfile for the PHY driver.
*/

#ifndef NX_ETH_PHY_H
#define NX_ETH_PHY_H

#include  "nx_api.h"
#include <stdint.h>

#ifdef  __cplusplus
extern "C"
{
#endif

/* Bit masks and shifts for PHY speed/duplex settings */
#define ETH_PHY_SPEED_SHIFT             (0U)                        /**< Bit shifts for Link speed */
#define ETH_PHY_SPEED_MASK              (3U << ETH_PHY_SPEED_SHIFT) /**< Bit mask for Link speed */
#define ETH_PHY_DUPLEX_SHIFT            (2U)                        /**< Bit shifts for Link duplex */
#define ETH_PHY_DUPLEX_MASK             (3U << ETH_PHY_DUPLEX_SHIFT)/**< Bit mask for Link duplex */
#define ETH_PHY_AN_SHIFT                (4U)                        /**< Bit shift for Autonegotiation */
#define ETH_PHY_AN_MASK                 (1U << ETH_PHY_AN_SHIFT)    /**< Bit mask for Autonegotiation */

#define ETH_PHY_SPEED_10                (1U << ETH_PHY_SPEED_SHIFT) /**< 10mbps */
#define ETH_PHY_SPEED_100               (2U << ETH_PHY_SPEED_SHIFT) /**< 100mbps */
#define ETH_PHY_SPEED_1000              (3U << ETH_PHY_SPEED_SHIFT) /**< 1gbps */

#define ETH_PHY_DUPLEX_HALF             (1U << ETH_PHY_DUPLEX_SHIFT) /**< Half duplex */
#define ETH_PHY_DUPLEX_FULL             (2U << ETH_PHY_DUPLEX_SHIFT) /**< Full duplex */

#define ETH_PHY_AUTONEGOTIATE           (1U << ETH_PHY_AN_SHIFT)     /**< Auto negotiation */

/* Link status */
#define ETH_PHY_LINK_DOWN               (0U)                        /**< Link down */
#define ETH_PHY_LINK_UP                 (1U)                        /**< Link up */

/* Standard PHY Registers */
#define PHY_REG_CONTROL                 (0U)
#define PHY_REG_STATUS                  (1U)
#define PHY_REG_PHYID1                  (2U)
#define PHY_REG_PHYID2                  (3U)
#define PHY_REG_AN_ADVERTISEMENT        (4U)
#define PHY_REG_AN_LINK_PARTNER         (5U)
#define PHY_REG_AN_EXPANSION            (6U)

#define DEVICE_ID_KSZ8081RNB            (0x221560)

/* KSZ8081RNB specific Registers */
#define PHY_CONTROL_1                   (0x1EU)
#define PHY_CONTROL_2                   (0x1FU)

/* Basic Control Register fields */
#define PHY_CONTROL_RESET               (1U << 15)
#define PHY_CONTROL_LOOPBACK            (1U << 14)
#define PHY_CONTROL_SPEED_SELECT        (1U << 13)
#define PHY_CONTROL_AN_ENABLE           (1U << 12)
#define PHY_CONTROL_POWER_DOWN          (1U << 11)
#define PHY_CONTROL_ISOLATE             (1U << 10)
#define PHY_CONTROL_RESTART_AN          (1U << 9)
#define PHY_CONTROL_DUPLEX_MODE         (1U << 8)
#define PHY_CONTROL_COL_TEST            (1U << 7)

/* Basic Status Register fields */
#define PHY_STATUS_100BASE_T4           (1U << 15)
#define PHY_STATUS_100BASE_FD           (1U << 14)
#define PHY_STATUS_100BASE_HD           (1U << 13)
#define PHY_STATUS_10BASE_FD            (1U << 12)
#define PHY_STATUS_10BASE_HD            (1U << 11)
#define PHY_STATUS_NO_PREAMBLE          (1U << 6)
#define PHY_STATUS_AN_COMPLETE          (1U << 5)
#define PHY_STATUS_REMOTE_FAULT         (1U << 4)
#define PHY_STATUS_AN_ABILITY           (1U << 3)
#define PHY_STATUS_LINK_STATUS          (1U << 2)
#define PHY_STATUS_JABBER_DETECT        (1U << 1)
#define PHY_STATUS_EX_CAPABILITY        (1U << 0)

/* PHY Control 1 Register fields */
#define PHY_CONTROL1_LINK_UP            (1U << 8)
#define PHY_CONTROL1_SPEED_100          (1U << 1)
#define PHY_CONTROL1_DUPLEX_FULL        (1U << 2)

/* function pointer types for phy read and write */
typedef UINT (*phy_read_t) (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data);
typedef UINT (*phy_write_t) (uint8_t phy_addr, uint8_t reg_addr, uint16_t data);

/* PHY driver interface */
/** \fn UINT nx_phy_init(uint8_t phy_id, phy_read_t read_fn, phy_write_t write_fn)
    \brief Initialize the PHY found at \a phy_id using MDIO read/write functions \a read_fn and \a write_fn.
    \param phy_id The phy address.
    \param read_fn The read function pointer for MDIO reads.
    \param write_fn The write function pointer for MDIO writes.
    \return The status of operation (NX_SUCCESS, NX_INVALID_PARAMETERS or NX_NOT_SUCCESSFUL)
*/
UINT nx_phy_init(uint8_t phy_id, phy_read_t read_fn, phy_write_t write_fn);

/** \fn UINT nx_phy_setmode(UINT mode)
    \brief Set the link mode of a previously initialized PHY to \a mode.
    \param mode A bitmask of Speed/Duplex/Autonegotiation settings.
    \return NX_INVALID_INTERFACE if PHY has not been initialized or status of the operation(NX_SUCCESS/NX_NOT_SUCCESSFUL).
*/
UINT nx_phy_setmode(UINT mode);

/** \fn UINT nx_phy_get_link_status(VOID)
    \brief Get the current link status.
    \return ETH_PHY_LINK_UP or ETH_PHY_LINK_DOWN.
    \warning Returns ETH_PHY_LINK_DOWN for a PHY interface that is not initialized/configured.
*/
UINT nx_phy_get_link_status(VOID);

/** \fn UINT nx_phy_get_link_info(VOID)
    \brief Get the current link speed/duplex information.
    \return Bitmask of current speed/duplex information.
    \warning Should be called only if the link has been initialized/configured AND is up.
*/
UINT nx_phy_get_link_info(VOID);

#ifdef  __cplusplus
}
#endif
#endif /* NX_ETH_PHY_H */
