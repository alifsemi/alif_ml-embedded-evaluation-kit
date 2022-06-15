/**
 * @file services_lib_api.h
 *
 * @brief Services library public API header file
 * @defgroup host_services Host Services
 * @par
 *
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
 */

#ifndef __SERVICES_LIB_API_H__
#define __SERVICES_LIB_API_H__

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/

#include "mhu.h"

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

#define SERVICES_INIT_SUCCESS                      0x0
#define SERVICES_INIT_FAILED                       0x1

#define SERVICES_REQ_TIMEOUT_MS                    0x20

// Error codes
#define SERVICES_REQ_SUCCESS                       0x00
#define SERVICES_REQ_NOT_ACKNOWLEDGE               0xFF
#define SERVICES_REQ_ACKNOWLEDGE                   0xFE
#define SERVICES_REQ_TIMEOUT                       0xFD
#define SERVICES_RESP_UNKNOWN_COMMAND              0xFC
#define SERVICES_REQ_BAD_PACKET_SIZE               0xFB
#define SERVICES_REQ_CANNOT_EXECUTE_SERVICE        0xFA
#define SERVICES_REQ_BAD_PAYLOAD                   0xF9
#define SERVICES_REQ_BAD_PAYLOAD_LENGTH            0xF8
#define SERVICES_REQ_PAYLOAD_OK                    0xF7
#define SERVICES_REQ_PIN_LOCKED                    0xF5

#define SERVICES_DEVICE_PART_NUMBER_LENGTH         16

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

typedef int32_t (*wait_ms_t)(uint32_t wait_time_ms);
typedef int (*print_msg_t)(const char * fmt, ...);

// Initialization to be done by host
/**
 * @struct services_lib_t
 */
typedef struct
{
  uint32_t             global_offset;
  MHU_send_message_t   fn_send_mhu_message;
  wait_ms_t            fn_wait_ms;
  uint32_t             wait_timeout;
  print_msg_t          fn_print_msg;
} services_lib_t;

/**
 *  @enum SERVICES_cpuid_t
 */
typedef enum
{
  FUSION_A32_0   = 0,
  FUSION_A32_1   = 1,
  FUSION_M55_HP  = 2,
  FUSION_M55_HE  = 3
} SERVICES_cpuid_t;

/**
 *  @enum SERVICES_toc_info_t
 */
typedef struct
{
  uint8_t   image_identifier[8];
  uint32_t  version;
  uint32_t  flags;
} SERVICES_toc_info_t;

/*******************************************************************************
 *  G L O B A L   D E F I N E S
 ******************************************************************************/

/*******************************************************************************
 *  F U N C T I O N   P R O T O T Y P E S
 ******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void SERVICES_initialize(services_lib_t * init_params);
uint32_t SERVICES_register_channel(uint32_t mhu_id,
                                   uint32_t channel_number);

typedef void (*SERVICES_sender_callback) (uint32_t sender_id, uint32_t data);
uint32_t SERVICES_send_msg(uint32_t services_handle,
                           void* service_data);
uint32_t SERVICES_send_request(uint32_t services_handle,
                               uint16_t service_id,
                               void * service_data,
                               SERVICES_sender_callback callback);
void SERVICES_send_msg_acked_callback(uint32_t sender_id,
                                      uint32_t channel_number);
void SERVICES_rx_msg_callback(uint32_t receiver_id,
                              uint32_t channel_number,
                              uint32_t data);

uint32_t SERVICES_local_to_global_addr(uint32_t local_addr);
uint32_t SERVICES_global_to_local_addr(uint32_t global_addr);

const char *SERVICES_version(void);
char *SERVICES_error_to_string(uint32_t error_code);

uint32_t SERVICES_heartbeat(uint32_t services_handle);
uint32_t SERVICES_heartbeat_async(uint32_t services_handle,
                                  SERVICES_sender_callback callback);
uint32_t SERVICES_uart_write(uint32_t services_handle, const uint8_t *uart_data);
uint32_t SERVICES_pinmux(uint32_t services_handle,
                         uint8_t port_number,
                         uint8_t pin_number,
                         uint8_t config_data,
                         uint32_t * error_code);
uint32_t SERVICES_padcontrol(uint32_t services_handle,
                             uint8_t port_number,
                             uint8_t pin_number,
                             uint8_t configuration_value,
                             uint32_t * error_code);

uint32_t SERVICES_cryptocell_get_rnd(uint32_t services_handle,
                                     uint16_t rnd_len,
                                     void *rnd_value,
                                     int32_t * error_code);

uint32_t SERVICES_cryptocell_get_lcs(uint32_t services_handle,
                                     uint32_t *lcs_state,
                                     int32_t * error_code);

uint32_t SERVICES_cryptocell_mbedtls_aes_init(uint32_t services_handle,
                                              uint32_t * error_code,
                                              uint32_t ctx);
uint32_t SERVICES_cryptocell_mbedtls_aes_set_key(uint32_t services_handle,
                                                 uint32_t * error_code,
                                                 uint32_t ctx,
                                                 uint32_t key,
                                                 uint32_t keybits,
                                                 uint32_t dir);
uint32_t SERVICES_cryptocell_mbedtls_aes_crypt(uint32_t services_handle,
                                               uint32_t * error_code,
                                               uint32_t ctx,
                                               uint32_t crypt_type,
                                               uint32_t mode,
                                               uint32_t length,
                                               uint32_t iv,
                                               uint32_t input,
                                               uint32_t output);

uint32_t SERVICES_cryptocell_mbedtls_sha_starts(uint32_t services_handle,
                                                uint32_t * error_code,
                                                uint32_t ctx,
                                                uint32_t sha_type);
uint32_t SERVICES_cryptocell_mbedtls_sha_process(uint32_t services_handle,
                                                uint32_t * error_code,
                                                uint32_t ctx,
                                                uint32_t sha_type,
                                                uint32_t data);
uint32_t SERVICES_cryptocell_mbedtls_sha_update(uint32_t services_handle,
                                                uint32_t * error_code,
                                                uint32_t ctx,
                                                uint32_t sha_type,
                                                uint32_t data,
                                                uint32_t data_length);
uint32_t SERVICES_cryptocell_mbedtls_sha_finish(uint32_t services_handle,
                                                uint32_t * error_code,
                                                uint32_t ctx,
                                                uint32_t sha_type,
                                                uint32_t data);

uint32_t SERVICES_system_get_toc_version(uint32_t services_handle,
                                         uint32_t * toc_version,
                                         uint32_t * error_code);
uint32_t SERVICES_system_get_toc_number(uint32_t services_handle,
                                        uint32_t *toc_number,
                                        uint32_t * error_code);
uint32_t SERVICES_system_get_toc_via_name(uint32_t services_handle,
                                          const uint8_t *cpu_name,
                                          uint32_t * error_code);
uint32_t SERVICES_system_get_toc_via_cpuid(uint32_t services_handle,
                                           SERVICES_cpuid_t cpuid,
                                           SERVICES_toc_info_t *toc_info,
                                           uint32_t * error_code);
uint32_t SERVICES_system_get_device_part_number(uint32_t services_handle,
                                                uint32_t *device_part_number,
                                                uint32_t * error_code);
uint32_t SERVICES_system_set_services_debug (uint32_t services_handle,
                                             bool debug_enable,
                                             uint32_t *error_code);
uint32_t SERVICES_boot_process_toc_entry(uint32_t services_handle,
                                         const uint8_t * image_id,
                                         uint32_t * error_code);
uint32_t SERVICES_boot_cpu(uint32_t services_handle,
                           uint32_t cpu_id,
                           uint32_t address,
                           uint32_t * error_code);
uint32_t SERVICES_boot_release_cpu(uint32_t services_handle,
                                   uint32_t cpu_id,
                                   uint32_t * error_code);
uint32_t SERVICES_boot_reset_cpu(uint32_t services_handle,
                                 uint32_t cpu_id,
                                 uint32_t * error_code);
uint32_t SERVICES_boot_reset_soc(uint32_t services_handle);

uint32_t SERVICES_power_stop_mode_request(uint32_t services_handle);

#ifdef __cplusplus
}
#endif

#endif /* __SERVICES_LIB_API_H__ */
