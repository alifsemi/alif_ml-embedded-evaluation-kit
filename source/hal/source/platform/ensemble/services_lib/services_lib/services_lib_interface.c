/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
/**
 * @file  services_lib_interface.c
 * @brief Public interface for Services library
 * @note  Unique for each platform
 * @par
 */

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "services_lib_interface.h"
#include "services_lib_protocol.h"
#include "mhu.h"

#include "RTE_Components.h"
#include CMSIS_device_header
/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

#define SERVICES_PRINT_BUFFER_SIZE (256)

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

/*******************************************************************************
 *  G L O B A L   V A R I A B L E S
 ******************************************************************************/
static uint8_t
  s_packet_buffer[SERVICES_MAX_PACKET_BUFFER_SIZE] __attribute__ ((aligned (4)));

debug_print_function_t drv_debug_print_fn;

/*******************************************************************************
 *  C O D E
 ******************************************************************************/

/**
 * @brief Public interface for SERVICES delay function
 * @param wait_time_ms
 * @return
 * @note  User must supply this implementation for their platform. This is a
 *        bare metal use case
 */
int32_t SERVICES_wait_ms(uint32_t wait_time_ms)
{
  /*
   * To be filled in by the user
   */
  for (volatile uint32_t i = 0; i < wait_time_ms; i++)
  {
    sys_busy_loop_us(1000);
  }

  return 0;
}

/**
 * @brief Public interface for SERVICES stub printing function
 * @param fmt
 * @note  Add you favourite printing method here
 */
int SERVICES_print(const char * fmt, ...)
{
#if SERVICES_PRINT_ENABLE != 0
  va_list args;
  char buffer[SERVICES_PRINT_BUFFER_SIZE];

  /*
   * @todo Handle long strings bigger than buffer size
   */
  va_start(args,fmt);
  vsnprintf(buffer, SERVICES_PRINT_BUFFER_SIZE, fmt, args);
  va_end(args);

  printf("%s", buffer);
#else
  (void)fmt;
#endif // #if SERVICES_PRINT_ENABLE != 0

  return 0;
}

/**
 * @fn    SERVICES_Setup(MHU_send_message_t send_message)
 * @brief Public interface to initialize the SERVICES library
 *
 * @param send_message
 * @param timeout
 */
void SERVICES_Setup(MHU_send_message_t send_message, uint32_t timeout)
{
  services_lib_t  services_init_params =
  {
    .packet_buffer_address = (uint32_t)s_packet_buffer,
    .fn_send_mhu_message   = send_message,
    .fn_wait_ms            = &SERVICES_wait_ms,
    .wait_timeout          = timeout,
    .fn_print_msg          = &SERVICES_print,
  };
  drv_debug_print_fn = &SERVICES_print;

  SERVICES_initialize(&services_init_params);
}
