/*
 * NetXDuo network driver for Designware QOS MAC
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

/*! \file nx_eqos_network_driver.h
    \brief Private headerfile for nx_eqos_network_driver.c.
*/

#ifndef NX_EQOS_NETWORK_DRIVER_PRIVATE_H
#define NX_EQOS_NETWORK_DRIVER_PRIVATE_H

#include "eqos_hw.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/**
 * enum DRIVER_STATE.
 * Different states that the  driver can be in.
 */
typedef enum {
    DRIVER_ATTACHED = 0x1,     /**< Driver has been attached to an IP instance. */
    DRIVER_INITIALIZED = 0x2,  /**< Driver has been initialized. */
    DRIVER_ENABLED = 0x4,      /**< Driver has been enabled. */
} DRIVER_STATE;

/**
  \struct MAC_ADDRESS
  \brief  Represents the 48bit MAC address.
*/
typedef struct MAC_ADDRESS_STRUCT
{
    ULONG nx_mac_address_msw;
    ULONG nx_mac_address_lsw;
} MAC_ADDRESS;

/**
  \struct NX_EQOS_DRIVER
  \brief  Represents the driver instance.
*/
typedef struct NX_EQOS_DRIVER_STRUCT
{
    UINT          nx_driver_state;          /**< The driver state */

    ULONG         nx_driver_total_rx_packets; /**< Total number of packets received */

    ULONG         nx_driver_total_tx_packets; /**< Total number of packets transmitted */

    ULONG         nx_driver_tx_packets_dropped; /**< Number of tx packets dropped because of unavailability of DMA descs */

    ULONG         nx_driver_allocation_errors; /**< Total packet allocation errors */

    NX_INTERFACE *nx_driver_interface_ptr;  /**< Pointer to the NetX interface structure */

    NX_IP        *nx_driver_ip_ptr;         /**< Pointer to the NetX IP instance attached to this driver */

    EQOS_DEV     *nx_driver_dev;            /**< Pointer to \ref EQOS_DEV owned by this driver */

    MAC_ADDRESS   nx_driver_mac_address;    /**< MAC Address */

    TX_TIMER      nx_driver_phy_poll_timer; /**< ThreadX timer for polling the link status */

    TX_THREAD     nx_driver_phy_poll_thread; /**< ThreadX thread for polling the link status */

    TX_EVENT_FLAGS_GROUP   nx_driver_phy_events; /**< ThreadX event flags group polling the link status */

    UCHAR         nx_driver_link_speed;   /**< Current link speed */

    UCHAR         nx_driver_link_duplex;  /**< Current link duplex information */

} NX_EQOS_DRIVER;

#ifdef  __cplusplus
}
#endif

#endif /* NX_EQOS_NETWORK_DRIVER_PRIVATE_H */
