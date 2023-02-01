/**
 * @file  services_host_error.c
 *
 * @brief Services library Error handling
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
 *  @ingroup host_services
 */


/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "services_lib_api.h"
#include "services_lib_public.h"
#include "services_lib_protocol.h"

#define MAX_ERROR_STRING_LENGTH     38

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
 * @fn    char *SERVICES_error_to_string(uint32_t error_code)
 * @brief Error code to string conversion
 * @param error_code
 * @return
 */
char *SERVICES_error_to_string(uint32_t error_code)
{
  static char err_string[MAX_ERROR_STRING_LENGTH];
  char *p_str = NULL;

  memset(err_string, ' ',sizeof(err_string));

  switch (error_code)
   {
       case SERVICES_REQ_SUCCESS:
         p_str = "SERVICES_REQ_SUCCESS        "; break;
       case SERVICES_REQ_NOT_ACKNOWLEDGE:
         p_str = "SERVICES_REQ_NOT_ACKNOWLEDGE"; break;
       case SERVICES_REQ_ACKNOWLEDGE:
         p_str = "SERVICES_REQ_ACKNOWLEDGE    "; break;
       case SERVICES_REQ_TIMEOUT:
         p_str = "SERVICES_REQ_TIMEOUT        "; break;
       case SERVICES_RESP_UNKNOWN_COMMAND:
         p_str = "SERVICES_RESP_UNKNOWN_COMMAND"; break;
       case SERVICES_REQ_BAD_PACKET_SIZE:
         p_str = "SERVICES_REQ_BAD_PACKET_SIZE"; break;
       case SERVICES_REQ_CANNOT_EXECUTE_SERVICE:
         p_str = "SERVICES_REQ_CANNOT_EXECUTE_SERVICE"; break;
       case SERVICES_REQ_BAD_PAYLOAD:
         p_str = "SERVICES_REQ_BAD_PAYLOAD"; break;
       case SERVICES_REQ_BAD_PAYLOAD_LENGTH:
         p_str = "SERVICES_REQ_BAD_PAYLOAD_LENGTH"; break;
       case SERVICES_REQ_PAYLOAD_OK:
         p_str = "SERVICES_REQ_PAYLOAD_OK"; break;
       case SERVICES_REQ_PIN_LOCKED:
         p_str = "SERVICES_REQ_PIN_LOCKED"; break;
       default:
         p_str = ">>  Error UNKNOWN  <<"; break;
  }
  strncpy(err_string, p_str, sizeof(err_string));

  return (char *)&err_string[0];
}
