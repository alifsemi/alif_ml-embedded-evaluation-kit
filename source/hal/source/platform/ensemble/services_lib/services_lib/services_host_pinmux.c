/**
 * @file services_host_pinmux.c
 *
 * @brief Pinmux service source file
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
 * @brief Pinmux service API
 * @param services_handle
 * @param port_number
 * @param pin_number
 * @param config_data
 * @return
 */
uint32_t SERVICES_pinmux(uint32_t services_handle, 
                         uint8_t port_number, 
                         uint8_t pin_number, 
                         uint8_t config_data,
                         uint32_t * error_code)
{
  pinmux_svc_t svc;
  memset(&svc, 0x0, sizeof(pinmux_svc_t));

  svc.send_port_num = port_number;
  svc.send_pin_num = pin_number;
  svc.send_config_data = config_data;

  uint32_t ret = SERVICES_send_request(services_handle,
                               SERVICE_APPLICATION_PINMUX_ID, &svc, NULL);
  *error_code = svc.resp_error_code;
  return ret;
}
