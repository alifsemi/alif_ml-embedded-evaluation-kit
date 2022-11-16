/**
 * @file services_host_handler.c
 * @brief Services handler file
 * COPYRIGHT NOTICE: (c) 2022 Alif Group. All rights reserved.
 * @ingroup host_services
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "services_lib_api.h"
#include "services_lib_public.h"
#include "services_lib_protocol.h"
#include "RTE_Components.h"

#define SEND_MSG_ACK_TIMEOUT    100000ul

static services_lib_t s_services_host = {0};

static uint32_t s_pkt_buffer_address = 0x0;
static volatile bool s_service_req_ack_received = false;
static volatile bool s_new_msg_received = false;
static SERVICES_sender_callback s_callback;

/**
 * Function to initialize the services library
 * @param init_params Initialization parameters
 */
void SERVICES_initialize(services_lib_t * init_params)
{
  s_services_host.global_offset = init_params->global_offset;
  s_services_host.fn_send_mhu_message = init_params->fn_send_mhu_message;
  s_services_host.fn_wait_ms = init_params->fn_wait_ms;
  s_services_host.wait_timeout = init_params->wait_timeout,
  s_services_host.fn_print_msg = init_params->fn_print_msg;
}

uint32_t SERVICES_local_to_global_addr(uint32_t local_addr)
{
  // uint32_t global_addr = local_addr + s_services_host.global_offset;
  // return global_addr;
    return LocalToGlobal((void *) local_addr);
}

uint32_t SERVICES_global_to_local_addr(uint32_t global_addr)
{
  // uint32_t local_addr = global_addr - s_services_host.global_offset;
  // return local_addr;
    return (uint32_t) GlobalToLocal(global_addr);
}

/**
 * @fn      uint32_t SERVICES_register_channel(uint32_t mhu_id,
 *                                             uint32_t channel_number)
 * @brief   Register a MHU communication channel with the Services library
 * @param   mhu_id
 * @param   channel_number
 * @return  Handle to be used in subsequent service calls
 */
uint32_t SERVICES_register_channel(uint32_t mhu_id,
                                   uint32_t channel_number)
{
  return mhu_id * MHU_NUMBER_OF_CHANNELS_MAX + channel_number;
}

/**
 *
 * @param services_handle
 * @return
 */
static uint32_t services_get_mhu_id(uint32_t services_handle)
{
  return services_handle / MHU_NUMBER_OF_CHANNELS_MAX;
}

/**
 *
 * @param services_handle
 * @return
 */
static uint32_t services_get_channel_number(uint32_t services_handle)
{
  return services_handle % MHU_NUMBER_OF_CHANNELS_MAX;
}

/**
 * @fn    const char *SERVICES_version(void)
 * @brief SERVICES version
 * @return version string
 */
const char *SERVICES_version(void)
{
  return SE_SERVICES_VERSION_STRING;
}

/**
 * @brief Callback function for sent msg ACK
 * @fn    void SERVICES_send_msg_acked_callback(uint32_t sender_id,
 *                                              uint32_t channel_number)
 */
void SERVICES_send_msg_acked_callback(uint32_t sender_id,
                                      uint32_t channel_number)
{
  s_service_req_ack_received = true;
}

/**
 * @fn    void SERVICES_rx_msg_callback(uint32_t receiver_id,
 *                                      uint32_t channel_number,
 *                                      uint32_t service_data)
 * @brief Callback function for response message reception
 * @param receiver_id
 * @param channel_number
 * @param service_data
 */
void SERVICES_rx_msg_callback(uint32_t receiver_id,
                              uint32_t channel_number,
                              uint32_t service_data)
{
  uint32_t local_address = SERVICES_global_to_local_addr(service_data);
  // Validate response by checking the message
  if (s_pkt_buffer_address == local_address)
  {
    s_services_host.fn_print_msg("[SERVICESLIB] rx_msg=0x%x \n", service_data);
    s_new_msg_received = true;

    if (s_callback)
    {
      s_callback(receiver_id, service_data);
      s_callback = NULL;
    }
  }
  else
  {
    // @todo: handle invalid message
    s_services_host.fn_print_msg("[SERVICESLIB] Invalid msg=0x%x\n",
                                  service_data);
  }
}

uint32_t SERVICES_send_msg(uint32_t services_handle,
                           void* service_data)
{
    // Send a message to the SE
    uint32_t global_address = SERVICES_local_to_global_addr((uint32_t)service_data);
    s_services_host.fn_send_mhu_message(services_get_mhu_id(services_handle),
                                        services_get_channel_number(services_handle),
                                        global_address);

    // Wait for ACK from SE
    uint32_t timeout = SEND_MSG_ACK_TIMEOUT;
    while (!s_service_req_ack_received)
    {
      timeout--;
      if (0 == timeout) // ACK not received
      {
        s_services_host.fn_print_msg("[ERROR][SERVICESLIB] SERVICES_REQ_NOT_ACKNOWLEDGE \n");
        return SERVICES_REQ_NOT_ACKNOWLEDGE;
      }
    }
    return SERVICES_REQ_SUCCESS;
}

/**
 *
 * @param services_handle
 * @param service_id
 * @param service_data
 * @param callback
 * @return
 */
uint32_t SERVICES_send_request(uint32_t services_handle,
                               uint16_t service_id,
                               void *service_data,
                               SERVICES_sender_callback callback)
{
  s_services_host.fn_print_msg("[SERVICESLIB] Send service request 0x%x\n",
                               service_id);

  s_pkt_buffer_address = (uint32_t)service_data;
  s_new_msg_received = false;
  s_service_req_ack_received = false;
  s_callback = callback;

  // Initialize the service request common header
  service_header_t * p_header = (service_header_t *)s_pkt_buffer_address;
  p_header->send_service_id = service_id;
  p_header->send_flags = 0;

  // Send a message to the SE
  uint32_t ret = SERVICES_send_msg(services_handle, service_data);
  if (ret != SERVICES_REQ_SUCCESS)
      return ret;

  if (callback) // asynchronous invokation, don't wait for the service response
  {
    return 0;
  }

  // Wait for response from SE
  uint32_t timeout = s_services_host.wait_timeout;
  while (!s_new_msg_received)
  {
    timeout--;
    if (0 == timeout) // No response from SE
    {
      return SERVICES_REQ_TIMEOUT;
    }

    if (0 != s_services_host.fn_wait_ms(SERVICES_REQ_TIMEOUT_MS))
    {
      break;
    }
  }

  return p_header->resp_error_code;
}
