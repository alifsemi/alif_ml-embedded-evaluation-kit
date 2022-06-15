/**
 * @file services_host_cryptocell.c
 *
 * @brief Cryptocell services service source file
 * @ingroup host_services
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
 */


/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "services_lib_api.h"
#include "services_lib_public.h"
#include "services_lib_protocol.h"

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/


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
 * @brief Services Crypto get TRNG
 * @param services_handle
 * @param rnd_len
 * @param rnd_value
 * @return
 */
uint32_t SERVICES_cryptocell_get_rnd(uint32_t services_handle, 
                                     uint16_t rnd_len,
                                     void * rnd_value,
                                     int32_t * error_code)
{
  get_rnd_svc_t svc;
  memset(&svc, 0x0, sizeof(get_rnd_svc_t));
  svc.send_rnd_length = rnd_len;
  
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_GET_RND, &svc, NULL);
  memcpy(rnd_value, svc.resp_rnd, rnd_len);
  *error_code = svc.resp_error_code;
  return err;
}

/**
 * @brief Services Crypto get LCS State
 * @param services_handle
 * @param lcs_state
 * @return
 */
uint32_t SERVICES_cryptocell_get_lcs(uint32_t services_handle, 
                                     uint32_t * lcs_state,
                                     int32_t * error_code)
{
  get_lcs_svc_t svc;
  memset(&svc, 0x0, sizeof(get_lcs_svc_t));
  
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_GET_LCS, &svc, NULL);
  *lcs_state = svc.resp_lcs;
  *error_code = svc.resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_aes_init(uint32_t services_handle, 
                                              uint32_t * error_code, 
                                              uint32_t ctx)
{
  mbedtls_crypto_init_svc_t svc;
  memset(&svc, 0x0, sizeof(mbedtls_crypto_init_svc_t));

  svc.send_context_addr = SERVICES_local_to_global_addr(ctx);
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_AES_INIT, 
                                       &svc, NULL);
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_aes_set_key(uint32_t services_handle, 
                                                 uint32_t * error_code, 
                                                 uint32_t ctx,
                                                 uint32_t key,
                                                 uint32_t keybits,
                                                 uint32_t dir)
{
  mbedtls_aes_set_key_svc_t svc;
  memset(&svc, 0x0, sizeof(mbedtls_aes_set_key_svc_t));

  svc.send_context_addr = SERVICES_local_to_global_addr(ctx);
  svc.send_key_addr = SERVICES_local_to_global_addr(key);
  svc.send_key_bits = keybits;
  svc.send_direction = dir;
  
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_AES_SET_KEY, 
                                       &svc, NULL);
  *error_code = svc.resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_aes_crypt(uint32_t services_handle, 
                                               uint32_t * error_code,
                                               uint32_t ctx,
                                               uint32_t crypt_type, 
                                               uint32_t mode, 
                                               uint32_t length, 
                                               uint32_t iv, 
                                               uint32_t input, 
                                               uint32_t output)
{
  mbedtls_aes_crypt_svc_t svc;
  memset(&svc, 0x0, sizeof(mbedtls_aes_crypt_svc_t));

  svc.send_context_addr = SERVICES_local_to_global_addr(ctx);
  svc.send_crypt_type = crypt_type;
  svc.send_mode = mode;
  svc.send_length = length;
  svc.send_iv_addr = SERVICES_local_to_global_addr(iv);
  svc.send_input_addr = SERVICES_local_to_global_addr(input);
  svc.send_output_addr = SERVICES_local_to_global_addr(output);
  
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_AES_CRYPT, 
                                       &svc, NULL);
  *error_code = svc.resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_sha_starts(uint32_t services_handle, 
                                                uint32_t * error_code, 
                                                uint32_t ctx, 
                                                uint32_t sha_type)
{
  mbedtls_sha_svc_t svc;
  memset(&svc, 0x0, sizeof(mbedtls_sha_svc_t));
  
  svc.send_context_addr = SERVICES_local_to_global_addr(ctx);
  svc.send_sha_type = sha_type;

  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_SHA_STARTS, 
                                       &svc, NULL);
  *error_code = svc.resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_sha_process(uint32_t services_handle, 
                                                 uint32_t * error_code, 
                                                 uint32_t ctx, 
                                                 uint32_t sha_type, 
                                                 uint32_t data)
{
  mbedtls_sha_svc_t svc;
  memset(&svc, 0x0, sizeof(mbedtls_sha_svc_t));
  
  svc.send_context_addr = SERVICES_local_to_global_addr(ctx);
  svc.send_sha_type = sha_type;
  svc.send_data_addr = SERVICES_local_to_global_addr(data);

  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_SHA_PROCESS, 
                                       &svc, NULL);
  *error_code = svc.resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_sha_update(uint32_t services_handle, 
                                                uint32_t * error_code, 
                                                uint32_t ctx, 
                                                uint32_t sha_type, 
                                                uint32_t data, 
                                                uint32_t data_length)
{
  mbedtls_sha_svc_t svc;
  memset(&svc, 0x0, sizeof(mbedtls_sha_svc_t));
  
  svc.send_context_addr = SERVICES_local_to_global_addr(ctx);
  svc.send_sha_type = sha_type;
  svc.send_data_addr = SERVICES_local_to_global_addr(data);
  svc.send_data_length = data_length;

  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_SHA_UPDATE, 
                                       &svc, NULL);
  *error_code = svc.resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_sha_finish(uint32_t services_handle, 
                                                uint32_t * error_code, 
                                                uint32_t ctx, 
                                                uint32_t sha_type, 
                                                uint32_t data)
{
  mbedtls_sha_svc_t svc;
  memset(&svc, 0x0, sizeof(mbedtls_sha_svc_t));
  
  svc.send_context_addr = SERVICES_local_to_global_addr(ctx);
  svc.send_sha_type = sha_type;
  svc.send_data_addr = SERVICES_local_to_global_addr(data);

  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_SHA_FINISH, 
                                       &svc, NULL);
  *error_code = svc.resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_hardware_poll(uint32_t services_handle, 
                                                   uint32_t * error_code, 
                                                   uint32_t data, 
                                                   uint32_t output, 
                                                   uint32_t len, 
                                                   uint32_t olen)
{
  mbedtls_trng_hardware_poll_svc_t svc;
  memset(&svc, 0x0, sizeof(mbedtls_trng_hardware_poll_svc_t));
  
  svc.send_data_addr = SERVICES_local_to_global_addr(data);
  svc.send_output_addr = SERVICES_local_to_global_addr(output);
  svc.send_olen_addr = SERVICES_local_to_global_addr(olen);
  
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_TRNG_HARDWARE_POLL, 
                                       &svc, NULL);
  *error_code = svc.resp_error_code;
  return err;
}

