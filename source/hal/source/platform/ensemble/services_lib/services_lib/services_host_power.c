/**
 * @file services_host_power.c
 *
 * @brief Cryptocell services service source file
 * @ingroup host_services
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
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
 * @brief     Function to request stop mode
 * @param     services_handle
 * @return
 */
uint32_t SERVICES_power_stop_mode_request(uint32_t services_handle)
{
  stop_mode_request_svc_t svc;

  memset(&svc, 0x0, sizeof(stop_mode_request_svc_t));

  svc.enable = 1;
  svc.ewic_wakeup_source = 1 << 6; // bit6 for RTC_A

  return SERVICES_send_request(services_handle, 
                               SERVICE_POWER_STOP_MODE_REQ_ID, &svc, NULL);
}

