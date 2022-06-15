/**
 * @file services_host_maintenance.c
 *
 * @brief Maintenance Host Services source file
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
 *
 * @ingroup host_services
 */

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "services_lib_api.h"
#include "services_lib_public.h"
#include "services_lib_protocol.h"

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

/*******************************************************************************
 *  G L O B A L   V A R I A B L E S
 ******************************************************************************/

/*******************************************************************************
 *  C O D E
 ******************************************************************************/

/**
 * @brief   Hearbeat async service call
 * @return  Errorcode
 * @note    No payload required.
 */
uint32_t SERVICES_heartbeat_async(uint32_t services_handle, 
                                  SERVICES_sender_callback callback)
{
  service_header_t svc;
  memset(&svc, 0x0, sizeof(service_header_t));
  return SERVICES_send_request(services_handle, 
                               SERVICE_MAINTENANCE_HEARTBEAT_ID, &svc, callback);
}

/**
 * @brief   Hearbeat service call
 * @return  Errorcode
 * @note    No payload required.
 */
uint32_t SERVICES_heartbeat(uint32_t services_handle)
{
  return SERVICES_heartbeat_async(services_handle, NULL);
}

