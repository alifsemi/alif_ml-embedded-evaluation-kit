/**
 * @file services_host_padcontrol.c
 *
 * @brief Pad control service source file
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

/**
 * @ingroup services
 */

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

uint32_t SERVICES_uart_write(uint32_t services_handle, const uint8_t *uart_data)
{
  uart_write_svc_t svc;

  memset(&svc, 0x0, sizeof(uart_write_svc_t));
  memcpy(svc.string_contents, uart_data, sizeof(svc.string_contents));
  svc.string_length = 0; /* @todo get the length? */

  return SERVICES_send_request(services_handle,
                               SERVICE_APPLICATION_UART_WRITE_ID,
                               &svc, NULL);
}
