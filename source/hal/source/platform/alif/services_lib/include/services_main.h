/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#ifndef SERVICES_MAIN_H_
#define SERVICES_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif
typedef void (*mhu_receive_callback_t)(void* data);

typedef struct
{
  uint16_t id;
  char     msg[64];
} m55_data_payload_t;

int services_init (mhu_receive_callback_t cb);

#ifdef __cplusplus
}
#endif
#endif /* SERVICES_MAIN_H_ */
