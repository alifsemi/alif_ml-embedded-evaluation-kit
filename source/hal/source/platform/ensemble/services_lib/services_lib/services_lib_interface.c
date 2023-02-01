/**
 * @file  services_lib_interface.c
 *
 * @brief Public interface for Services library
 * @note  Unique for each platform
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
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

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

#define SERVICES_PRINT_ENABLE       0

#define MAXIMUM_TIMEOUT             0x10000

#if DEVICE_REVISION == REV_B0
#define HE_DTCM_GLOBAL_ADDRESS      0x58800000ul
#else
#define HE_DTCM_GLOBAL_ADDRESS      0x60800000ul
#endif
#define HP_DTCM_GLOBAL_ADDRESS      0x50800000ul
#define M55_DTCM_LOCAL_OFFSET       0x20000000ul

#if   CPU == M55_HE || defined(M55_HE)
#define DTCM_GLOBAL_ADDRESS         HE_DTCM_GLOBAL_ADDRESS
#elif CPU == M55_HP || defined(M55_HP)
#define DTCM_GLOBAL_ADDRESS         HP_DTCM_GLOBAL_ADDRESS
#else
#error Target CPU is not defined
#endif

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
 * @brief Public interface for SERVICES delay function
 * @param wait_time_ms
 * @return
 * @note  User must supply this implementation for their platform. This is a
 *        bare metal use case
 */
int SERVICES_wait_ms(uint32_t wait_time_ms)
{
  /*
   * To be filled in by the user
   */
  for (int i = 0; i < wait_time_ms; i++)
  {
     /* Do nothing */
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
  char buffer[256];

  /*
   * @todo Handle long strings bigger than buffer size
   */
  va_start(args,fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);

  printf("%s", buffer);
#else
  (void)fmt;
#endif // #if SERVICES_PRINT_ENABLE != 0

  return 0;
}

debug_print_function_t drv_debug_print_fn;

/**
 * @fn    SERVICES_Setup(MHU_send_message_t send_message)
 * @brief Public interface to initialize the SERVICES library
 */
void SERVICES_Setup(MHU_send_message_t send_message)
{
  /**
   *  @brief  Service library initialization parameters
   */
  services_lib_t  services_init_params =
  {
    .global_offset        = DTCM_GLOBAL_ADDRESS - M55_DTCM_LOCAL_OFFSET,
    .fn_send_mhu_message  = send_message,
    .fn_wait_ms           = &SERVICES_wait_ms,
    .wait_timeout         = 400000,
    .fn_print_msg         = &SERVICES_print,
  };
  drv_debug_print_fn = &SERVICES_print;

  SERVICES_initialize(&services_init_params);
}
