/**
 * @file services_lib_interface.h
 *
 * @brief Services library interface header file
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
 */

#ifndef __SERVICES_LIB_INTERFACE_H__
#define __SERVICES_LIB_INTERFACE_H__

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "services_lib_api.h"

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

//#define M55_HE             0x1001
//#define M55_HP             0x1002
#define REV_A              0x1
#define REV_B0             0x2
#define REV_A1             0x3

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/


/*******************************************************************************
 *  G L O B A L   D E F I N E S
 ******************************************************************************/


/*******************************************************************************
 *  F U N C T I O N   P R O T O T Y P E S
 ******************************************************************************/

void SERVICES_Setup(MHU_send_message_t send_message);
int SERVICES_print(const char * fmt, ...);
int SERVICES_wait_ms(uint32_t wait_time_ms);

#endif /* __SERVICES_LIB_INTERFACE_H__ */
