#include <inttypes.h>
#include <stdio.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "mhu.h"
#include "services_lib_interface.h"
#include "services_main.h"

#include "uart_tracelib.h"

#if   CPU == M55_HE || defined(M55_HE)
//    #include "M55_HE.h"
#elif CPU == M55_HP || defined(M55_HP)
//    #include "M55_HP.h"
#else
    #error Target CPU is not defined
#endif



uint32_t services_handle = 0xff;
uint32_t m55_comms_handle = 0xff;

#define SE_MHU0_RECV_BASE      0x40040000
#define SE_MHU0_SEND_BASE      0x40050000

#define SE_MHU1_RECV_BASE      0x40060000
#define SE_MHU1_SEND_BASE      0x40070000


// On M55 HE, these are communication HE -> HP
// On M55 HP, these are communication HP -> HE
#define MHU0_RECV_M55_BASE     0x40080000
#define MHU0_SEND_M55_BASE     0x40090000

#define MHU1_RECV_M55_BASE     0x400A0000
#define MHU1_SEND_M55_BASE     0x400B0000

#define NUM_MHU                4

#define MHU_M55_SE_MHU0        0
#define MHU_M55_SE_MHU1        1

#define MHU_M55_M55_MHU0       2
#define MHU_M55_M55_MHU1       3


#define SEES_MHU0_COMB_IRQn    37
#define ESSE_MHU0_COMB_IRQn    38
#define SEES_MHU1_COMB_IRQn    39
#define ESSE_MHU1_COMB_IRQn    40

#define M55RECV_MHU0_COMB_IRQn 41
#define M55SEND_MHU0_COMB_IRQn 42
#define M55RECV_MHU1_COMB_IRQn 43
#define M55SEND_MHU1_COMB_IRQn 44

uint32_t sender_base_address_list[NUM_MHU] =
{
  SE_MHU0_SEND_BASE,
  SE_MHU1_SEND_BASE,
  MHU0_SEND_M55_BASE,
  MHU1_SEND_M55_BASE
};

uint32_t receiver_base_address_list[NUM_MHU] =
{
  SE_MHU0_RECV_BASE,
  SE_MHU1_RECV_BASE,
  MHU0_RECV_M55_BASE,
  MHU1_RECV_M55_BASE
};

static mhu_driver_in_t s_mhu_driver_in;
static mhu_driver_out_t s_mhu_driver_out;

static mhu_m55_receive_callback_t M55_receive_callback = 0;

void MHU_SESS_S_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_M55_SE_MHU0);
}

void MHU_SESS_S_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_M55_SE_MHU0);
}

void MHU_SESS_NS_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_M55_SE_MHU1);
}

void MHU_SESS_NS_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_M55_SE_MHU1);
}

void MHU_RTSS_S_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_M55_M55_MHU0);
}

void MHU_RTSS_S_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_M55_M55_MHU0);
}

void MHU_RTSS_NS_TX_IRQHandler()
{
  s_mhu_driver_out.sender_irq_handler(MHU_M55_M55_MHU1);
}

void MHU_RTSS_NS_RX_IRQHandler()
{
  s_mhu_driver_out.receiver_irq_handler(MHU_M55_M55_MHU1);
}

static void setup_irq(int irq_num/*, uint32_t irq_handler*/)
{
  NVIC_DisableIRQ(irq_num);
  NVIC_ClearPendingIRQ(irq_num);
  //NVIC_SetVector(irq_num, irq_handler);
  NVIC_SetPriority(irq_num, 2);
  NVIC_EnableIRQ(irq_num);
}

void handle_my_msg(uint32_t receiver_id,
                uint32_t channel_number,
                uint32_t service_data)
{
    if(M55_receive_callback)
    {
        M55_receive_callback((void*)service_data);
    }
}

void rx_msg_callback(uint32_t receiver_id,
                     uint32_t channel_number,
                     uint32_t service_data)
{
    switch (receiver_id)
    {
        case MHU_M55_SE_MHU0:
        case MHU_M55_SE_MHU1:
            SERVICES_rx_msg_callback(receiver_id, channel_number, service_data);
            break;

        case MHU_M55_M55_MHU0:
        case MHU_M55_M55_MHU1:
            handle_my_msg(receiver_id, channel_number, service_data);
            break;

        default:
            printf("rx_msg_callback: Invalid channel_number = %d\n", channel_number);
    }
}

static void mhu_initialize(void)
{
  __disable_irq();
  s_mhu_driver_in.sender_base_address_list = sender_base_address_list;
  s_mhu_driver_in.receiver_base_address_list = receiver_base_address_list;
  s_mhu_driver_in.mhu_count = NUM_MHU;
  s_mhu_driver_in.send_msg_acked_callback = SERVICES_send_msg_acked_callback;
  s_mhu_driver_in.rx_msg_callback = rx_msg_callback;
//  s_mhu_driver_in.debug_print = SERVICES_print;

  MHU_driver_initialize(&s_mhu_driver_in, &s_mhu_driver_out);

  setup_irq(ESSE_MHU0_COMB_IRQn/*, (uint32_t)&mhu0_se_sender_irq_handler*/);
  setup_irq(SEES_MHU0_COMB_IRQn/*, (uint32_t)&mhu0_se_receiver_irq_handler*/);

  setup_irq(ESSE_MHU1_COMB_IRQn/*, (uint32_t)&mhu1_se_sender_irq_handler*/);
  setup_irq(SEES_MHU1_COMB_IRQn/*, (uint32_t)&mhu1_se_receiver_irq_handler*/);

  setup_irq(M55SEND_MHU0_COMB_IRQn/*, (uint32_t)&mhu0_m55_sender_irq_handler*/);
  setup_irq(M55RECV_MHU0_COMB_IRQn/*, (uint32_t)&mhu0_m55_receiver_irq_handler*/);

  setup_irq(M55SEND_MHU1_COMB_IRQn/*, (uint32_t)&mhu1_m55_sender_irq_handler*/);
  setup_irq(M55RECV_MHU1_COMB_IRQn/*, (uint32_t)&mhu1_m55_receiver_irq_handler*/);

  __enable_irq();
}

void disable_se_debug()
{
    uint32_t service_error_code;

    /* Disable tracing output for services */
    uint32_t err = SERVICES_system_set_services_debug(services_handle,
                                                      false,
                                                      &service_error_code);
    printf("SE debug disabled: err = %d, service_error = %d\n", err, service_error_code);
}

int send_message_to_HE(void* data)
{
    return SERVICES_send_msg(m55_comms_handle, data);
}


int services_init (mhu_m55_receive_callback_t cb)
{
    /**
     * Initialise the MHU and SERVICES Library
     */
    mhu_initialize();
    SERVICES_Setup(s_mhu_driver_out.send_message);
    services_handle = SERVICES_register_channel(MHU_M55_SE_MHU0, 0);
    m55_comms_handle = SERVICES_register_channel(MHU_M55_M55_MHU0, 0);
    M55_receive_callback = cb;
    return 0;
}
