/**
 * @file mhu_driver.c
 * @brief MHU driver
 * @ingroup SERVICES
 * @par
 * COPYRIGHT NOTICE: (c) 2022 Alif Group. All rights reserved.
 */

#include "mhu_driver.h"

void MHU_driver_initialize(mhu_driver_in_t * data_in, mhu_driver_out_t * data_out)
{
  MHU_sender_initialize(data_in->sender_base_address_list, 
                        data_in->mhu_count, 
                        data_in->send_msg_acked_callback);
  MHU_receiver_initialize(data_in->receiver_base_address_list, 
                          data_in->mhu_count, 
                          data_in->rx_msg_callback);

  data_out->send_message = MHU_send_message;
  data_out->sender_irq_handler = MHU_send_message_irq_handler;
  data_out->receiver_irq_handler = MHU_receive_message_irq_handler;
}
