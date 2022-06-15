/*
 * services_main.h
 *
 *  Created on: May 13, 2022
 *      Author: jammu
 */

#ifndef SERVICES_MAIN_H_
#define SERVICES_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif
typedef void (*mhu_m55_receive_callback_t)(void* data);

typedef struct
{
  uint16_t id;
  char     msg[64];
} m55_data_payload_t;

int services_init (mhu_m55_receive_callback_t cb);
int send_message_to_HE(void* data);

#ifdef __cplusplus
}
#endif
#endif /* SERVICES_MAIN_H_ */
