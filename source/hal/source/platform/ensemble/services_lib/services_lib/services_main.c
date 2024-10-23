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
 * @file  main.c
 * @brief Source file for main function
 * @par
 */

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include <stdio.h>
#include <inttypes.h>
#include <stdio.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "mhu.h"
#include "services_lib_interface.h"
#include "services_main.h"

#define MAXIMUM_TIMEOUT        0x01000000

uint32_t services_handle = 0xff;
uint32_t he_comms_handle = 0xff;
uint32_t hp_comms_handle = 0xff;
uint32_t a32_0_comms_handle = 0xff;
uint32_t a32_1_comms_handle = 0xff;

static bool initialized = false;

#ifdef A32
#define MHU1_OFFSET            0x00020000
#define NUM_MHU                3
#define SE_MHU0_SEND_BASE      0x1B800000
#define SE_MHU0_RECV_BASE      0x1B810000
#define SE_MHU1_SEND_BASE      0x1B820000
#define SE_MHU1_RECV_BASE      0x1B830000
#define HE_MHU0_SEND_BASE      0x1B040000
#define HE_MHU0_RECV_BASE      0x1B050000
#define HE_MHU1_SEND_BASE      0x1B060000
#define HE_MHU1_RECV_BASE      0x1B070000
#define HP_MHU0_SEND_BASE      0x1B000000
#define HP_MHU0_RECV_BASE      0x1B010000
#define HP_MHU1_SEND_BASE      0x1B020000
#define HP_MHU1_RECV_BASE      0x1B030000

#define MHU_CPU_SE_MHU         0
#define MHU_CPU_HE_MHU         1
#define MHU_CPU_HP_MHU         2

uint32_t sender_base_address_list_0[NUM_MHU] =
{
  SE_MHU0_SEND_BASE,
  HE_MHU0_SEND_BASE,
  HP_MHU0_SEND_BASE,
};

uint32_t sender_base_address_list_1[NUM_MHU] =
{
  SE_MHU1_SEND_BASE,
  HE_MHU1_SEND_BASE,
  HP_MHU1_SEND_BASE,
};

uint32_t receiver_base_address_list_0[NUM_MHU] =
{
  SE_MHU0_RECV_BASE,
  HE_MHU0_RECV_BASE,
  HP_MHU0_RECV_BASE,
};

uint32_t receiver_base_address_list_1[NUM_MHU] =
{
  SE_MHU1_RECV_BASE,
  HE_MHU1_RECV_BASE,
  HP_MHU1_RECV_BASE,
};

#else
#define NUM_MHU                5
#define SE_MHU0_SEND_BASE      MHU_SESS_S_TX_BASE
#define SE_MHU0_RECV_BASE      MHU_SESS_S_RX_BASE
#define M55_MHU0_SEND_BASE     MHU_RTSS_S_TX_BASE
#define M55_MHU0_RECV_BASE     MHU_RTSS_S_RX_BASE
#define A32_MHU0_SEND_BASE     MHU_APSS_S_TX_BASE
#define A32_MHU0_RECV_BASE     MHU_APSS_S_RX_BASE
#define A32_MHU1_SEND_BASE     MHU_APSS_NS_TX_BASE
#define A32_MHU1_RECV_BASE     MHU_APSS_NS_RX_BASE


#define MHU0_SE_SENDER_IRQn    MHU_SESS_S_TX_IRQ_IRQn
#define MHU0_SE_RECVER_IRQn    MHU_SESS_S_RX_IRQ_IRQn

#define MHU0_HE_SENDER_IRQn    MHU_RTSS_S_TX_IRQ_IRQn
#define MHU0_HE_RECVER_IRQn    MHU_RTSS_S_RX_IRQ_IRQn

#define MHU0_HP_SENDER_IRQn    MHU_RTSS_S_TX_IRQ_IRQn
#define MHU0_HP_RECVER_IRQn    MHU_RTSS_S_RX_IRQ_IRQn

#define MHU0_A32_SENDER_IRQn   MHU_APSS_S_TX_IRQ_IRQn
#define MHU0_A32_RECVER_IRQn   MHU_APSS_S_RX_IRQ_IRQn
#define MHU1_A32_SENDER_IRQn   MHU_APSS_NS_TX_IRQ_IRQn
#define MHU1_A32_RECVER_IRQn   MHU_APSS_NS_RX_IRQ_IRQn


#define MHU_CPU_SE_MHU         0
#define MHU_CPU_HE_MHU         1
#define MHU_CPU_HP_MHU         2
#define MHU_CPU_A32_0_MHU      3
#define MHU_CPU_A32_1_MHU      4

uint32_t sender_base_address_list[NUM_MHU] =
{
  SE_MHU0_SEND_BASE,
  M55_MHU0_SEND_BASE,
  M55_MHU0_SEND_BASE,
  A32_MHU0_SEND_BASE,
  A32_MHU1_SEND_BASE,
};

uint32_t receiver_base_address_list[NUM_MHU] =
{
  SE_MHU0_RECV_BASE,
  M55_MHU0_RECV_BASE,
  M55_MHU0_RECV_BASE,
  A32_MHU0_RECV_BASE,
  A32_MHU1_RECV_BASE,
};

#endif

static mhu_driver_in_t s_mhu_driver_in;
static mhu_driver_out_t s_mhu_driver_out;

static mhu_receive_callback_t mhu_receive_callback = 0;

void MHU_SESS_S_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_CPU_SE_MHU);
}

void MHU_SESS_S_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_CPU_SE_MHU);
}

void MHU_RTSS_S_TX_IRQHandler()
{
#ifdef M55_HE
  s_mhu_driver_out.sender_irq_handler(MHU_CPU_HP_MHU);
#else
  s_mhu_driver_out.sender_irq_handler(MHU_CPU_HE_MHU);
#endif
}

void MHU_RTSS_S_RX_IRQHandler()
{
#ifdef M55_HE
  s_mhu_driver_out.receiver_irq_handler(MHU_CPU_HP_MHU);
#else
  s_mhu_driver_out.receiver_irq_handler(MHU_CPU_HE_MHU);
#endif
}

void MHU_HP_S_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_CPU_HP_MHU);
}

void MHU_HP_S_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_CPU_HP_MHU);
}

#ifdef A32
void MHU_SESS_NS_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_CPU_SE_MHU);
}

void MHU_SESS_NS_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_CPU_SE_MHU);
}

void MHU_RTSS_NS_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_CPU_HE_MHU);
}

void MHU_RTSS_NS_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_CPU_HE_MHU);
}

void MHU_HP_NS_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_CPU_HP_MHU);
}

void MHU_HP_NS_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_CPU_HP_MHU);
}

#else // A32
void MHU_APSS_S_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_CPU_A32_0_MHU);
}

void MHU_APSS_S_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_CPU_A32_0_MHU);
}


void MHU_APSS_NS_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_CPU_A32_1_MHU);
}

void MHU_APSS_NS_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_CPU_A32_1_MHU);
}
#endif

static void setup_irq(int irq_num)
{
  NVIC_DisableIRQ(irq_num);
  NVIC_ClearPendingIRQ(irq_num);
  NVIC_SetPriority(irq_num, 2);
  NVIC_EnableIRQ(irq_num);
}

static void handle_my_msg(uint32_t service_data)
{
    if (mhu_receive_callback) {
        mhu_receive_callback((void*)service_data);
    }
}

void rx_msg_callback(uint32_t receiver_id,
                     uint32_t channel_number,
                     uint32_t service_data)
{
    switch (receiver_id)
    {
        case MHU_CPU_SE_MHU:
            SERVICES_rx_msg_callback(receiver_id, channel_number, service_data);
            break;

        case MHU_CPU_HE_MHU:
        case MHU_CPU_HP_MHU:
#ifndef A32
        case MHU_CPU_A32_0_MHU:
        case MHU_CPU_A32_1_MHU:
#endif
            handle_my_msg(service_data);
            break;

        default:
            printf("rx_msg_callback: Invalid channel_number = %ld\n", channel_number);
    }
}

static void mhu_initialize(void)
{
  __disable_irq();
  s_mhu_driver_in.mhu_count = NUM_MHU;
  s_mhu_driver_in.send_msg_acked_callback = SERVICES_send_msg_acked_callback;
  s_mhu_driver_in.rx_msg_callback = rx_msg_callback;

#ifdef A32
  if (smp_processor_id() == 0) {
      s_mhu_driver_in.sender_base_address_list = sender_base_address_list_0;
      s_mhu_driver_in.receiver_base_address_list = receiver_base_address_list_0;

      setup_irq(MHU0_SE_SENDER_IRQn);
      setup_irq(MHU0_SE_RECVER_IRQn);
      setup_irq(MHU0_HE_SENDER_IRQn);
      setup_irq(MHU0_HE_RECVER_IRQn);
      setup_irq(MHU0_HP_SENDER_IRQn);
      setup_irq(MHU0_HP_RECVER_IRQn);
  } else {
      s_mhu_driver_in.sender_base_address_list = sender_base_address_list_1;
      s_mhu_driver_in.receiver_base_address_list = receiver_base_address_list_1;

      setup_irq(MHU1_SE_SENDER_IRQn);
      setup_irq(MHU1_SE_RECVER_IRQn);
      setup_irq(MHU1_HE_SENDER_IRQn);
      setup_irq(MHU1_HE_RECVER_IRQn);
      setup_irq(MHU1_HP_SENDER_IRQn);
      setup_irq(MHU1_HP_RECVER_IRQn);
  }
#else
  s_mhu_driver_in.sender_base_address_list = sender_base_address_list;
  s_mhu_driver_in.receiver_base_address_list = receiver_base_address_list;

  setup_irq(MHU0_SE_SENDER_IRQn);
  setup_irq(MHU0_SE_RECVER_IRQn);

#ifndef M55_HE
  setup_irq(MHU0_HE_SENDER_IRQn);
  setup_irq(MHU0_HE_RECVER_IRQn);
#endif

#ifndef M55_HP
  setup_irq(MHU0_HP_SENDER_IRQn);
  setup_irq(MHU0_HP_RECVER_IRQn);
#endif

  setup_irq(MHU0_A32_SENDER_IRQn);
  setup_irq(MHU0_A32_RECVER_IRQn);
  setup_irq(MHU1_A32_SENDER_IRQn);
  setup_irq(MHU1_A32_RECVER_IRQn);
#endif

  MHU_driver_initialize(&s_mhu_driver_in, &s_mhu_driver_out);

  __enable_irq();
}

void disable_se_debug()
{
    uint32_t service_error_code;

    /* Disable tracing output for services */
    uint32_t err = SERVICES_system_set_services_debug(services_handle,
                                                      false,
                                                      &service_error_code);
    //printf("SE debug disabled: err = %ld, service_error = %ld\n", err, service_error_code);
    (void)err;
}

int services_init (mhu_receive_callback_t cb)
{
    mhu_receive_callback = cb;

    if (initialized) {
      return 0;
    }

    /**
     * Initialise the MHU and SERVICES Library
     */
    mhu_initialize();
    SERVICES_Setup(s_mhu_driver_out.send_message, MAXIMUM_TIMEOUT);
    SERVICES_wait_ms(2);
    services_handle = SERVICES_register_channel(MHU_CPU_SE_MHU, 0);
#ifndef M55_HE
    he_comms_handle = SERVICES_register_channel(MHU_CPU_HE_MHU, 0);
#endif
#ifndef M55_HP
    hp_comms_handle = SERVICES_register_channel(MHU_CPU_HP_MHU, 0);
#endif
#ifndef A32
    a32_0_comms_handle = SERVICES_register_channel(MHU_CPU_A32_0_MHU, 0);
    a32_1_comms_handle = SERVICES_register_channel(MHU_CPU_A32_1_MHU, 0);
#endif

    initialized = true;

    return 0;
}
