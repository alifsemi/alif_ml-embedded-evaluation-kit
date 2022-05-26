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

/**
  \file nx_eqos_network_driver.h
  \brief Public header file for NetXDuo network driver for Designware QOS MAC.
 */

#ifndef NX_EQOS_NETWORK_DRIVER_H
#define NX_EQOS_NETWORK_DRIVER_H

#include "tx_api.h"

#ifdef  __cplusplus
extern "C"
{
#endif

/** \brief 10mbps Link speed, return value for NX_LINK_GET_SPEED direct driver command */
#define NX_DRIVER_LINK_SPEED_10         (1 << 0)
/** \brief 100mbps Link speed, return value for NX_LINK_GET_SPEED direct driver command */
#define NX_DRIVER_LINK_SPEED_100        (1 << 1)

/** \brief Half duplex link duplex type, return value for NX_LINK_GET_DUPLEX_TYPE direct driver command */
#define NX_DRIVER_LINK_DUPLEX_HALF      (1 << 0)
/** \brief Full duplex link duplex type, return value for NX_LINK_GET_DUPLEX_TYPE direct driver command */
#define NX_DRIVER_LINK_DUPLEX_FULL      (1 << 1)

/**
  \fn VOID nx_eth_driver(NX_IP_DRIVER *driver_req_ptr)
  \brief The main entry point into the driver.
  \param[in] driver_req_ptr Pointer to the driver request structure.
 */
VOID nx_eth_driver(NX_IP_DRIVER *driver_req_ptr);

#ifdef  __cplusplus
}
#endif

#endif /* NX_EQOS_NETWORK_DRIVER_H */
