/**
 * @file services_lib_protocol.h
 *
 * @brief Private header file for services library
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
 * @ingroup services
 */
#ifndef __SERVICES_LIB_PROTOCOL_H__
#define __SERVICES_LIB_PROTOCOL_H__

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

// Transport layer header structure, common for all services
/**
 * @struct service_header_t
 */
typedef struct
{
  uint16_t send_service_id; // Requested Service ID
  uint16_t send_flags;      // Request flags
  uint16_t resp_error_code; // Transport layer error code
  uint16_t none_padding;
} service_header_t;

#define IMAGE_NAME_LENGTH    8

/*******************************************************************************
 *  Service format definitions
 ******************************************************************************/

//////////////////////////////////////////////////////////////////
// Crypto Services
//

//----------------------------------------------------------------
// Get Random Data
//

// Same as MBEDTLS_CTR_DRBG_MAX_REQUEST in cryptocell-rt
#define MAX_RND_LENGTH 1024
typedef struct
{
  service_header_t header;
  uint32_t  send_rnd_length;
  uint8_t   resp_rnd[MAX_RND_LENGTH];
  int       resp_error_code;
} get_rnd_svc_t;

//----------------------------------------------------------------
// Get LCS
//
typedef struct
{
  service_header_t header;
  uint32_t  resp_lcs;
  uint32_t  resp_error_code;
} get_lcs_svc_t;

//----------------------------------------------------------------
// MBEDTLS CRYPTO INIT
typedef struct
{
  service_header_t header;
  uint32_t send_context_addr;
} mbedtls_crypto_init_svc_t;

//----------------------------------------------------------------
// MBEDTLS AES SET KEY
typedef struct
{
  service_header_t header;
  uint32_t send_context_addr;
  uint32_t send_key_addr;
  uint32_t send_key_bits;
  uint32_t send_direction;
  uint32_t resp_error_code;
} mbedtls_aes_set_key_svc_t;

//----------------------------------------------------------------
// MBEDTLS AES CRYPT
typedef struct
{
  service_header_t header;
  uint32_t send_context_addr;
  uint32_t send_crypt_type; // ECB, CBC, CTR_OFB
  uint32_t send_mode;       // Encrypt, Decrypt
  uint32_t send_length;
  uint32_t send_iv_addr;
  uint32_t send_input_addr;
  uint32_t send_output_addr;
  uint32_t resp_error_code;
} mbedtls_aes_crypt_svc_t;

//----------------------------------------------------------------
// MBEDTLS SHA
typedef struct
{
  service_header_t header;
  uint32_t send_context_addr;
  uint32_t send_sha_type;  // SHA1, SHA224, SHA256
  uint32_t send_data_addr;
  uint32_t send_data_length;
  uint32_t resp_error_code;
} mbedtls_sha_svc_t;

//----------------------------------------------------------------
// MBEDTLS CCM SET KEY
typedef struct
{
  service_header_t header;
  uint32_t send_context_addr;
  uint32_t send_cipher;
  uint32_t send_key_addr;
  uint32_t send_key_bits;
  uint32_t resp_error_code;
} mbedtls_ccm_set_key_svc_t;

//----------------------------------------------------------------
// MBEDTLS CHACHA20 CRYPT
typedef struct
{
  service_header_t header;
  uint32_t send_key_addr;
  uint32_t send_nonce_addr;
  uint32_t send_counter;
  uint32_t send_data_len;
  uint32_t send_input_addr;
  uint32_t send_output_addr;
  uint32_t resp_error_code;
} mbedtls_chacha20_crypt_svc_t;

//----------------------------------------------------------------
// MBEDTLS CHACHAPOLY CRYPT
typedef struct
{
  service_header_t header;
  uint32_t send_context_addr;
  uint32_t send_mode;
  uint32_t send_length;
  uint32_t send_nonce_addr;
  uint32_t send_aad_addr;
  uint32_t send_aad_len;
  uint32_t send_tag_addr;
  uint32_t send_input_addr;
  uint32_t send_output_addr;
  uint32_t resp_error_code;
} mbedtls_chachapoly_crypt_svc_t;

//----------------------------------------------------------------
// MBEDTLS POLY1305 CRYPT
typedef struct
{
  service_header_t header;
  uint32_t send_key_addr;
  uint32_t send_input_addr;
  uint32_t send_ilen;
  uint32_t send_mac_addr;
  uint32_t resp_error_code;
} mbedtls_poly1305_crypt_svc_t;

//----------------------------------------------------------------
// MBEDTLS TRNG HARDWARE POLL
typedef struct
{
  service_header_t header;
  uint32_t send_data_addr;
  uint32_t send_output_addr;
  uint32_t send_len;
  uint32_t send_olen_addr;
  uint32_t resp_error_code;
} mbedtls_trng_hardware_poll_svc_t;

//////////////////////////////////////////////////////////////////
// Boot Services
//

//----------------------------------------------------------------
// Process TOC Entry
//
typedef struct
{
  service_header_t header;
  uint8_t   send_entry_id[IMAGE_NAME_LENGTH];
  uint32_t  resp_error_code;
} process_toc_entry_svc_t;

//----------------------------------------------------------------
// Boot CPU
//
typedef struct
{
  service_header_t header;
  uint32_t  send_cpu_id;
  uint32_t  send_address;
  uint32_t  resp_error_code;
} boot_cpu_svc_t;

//----------------------------------------------------------------
// Control CPU
//
typedef struct
{
  service_header_t header;
  uint32_t  send_cpu_id;
  uint32_t  resp_error_code;
} control_cpu_svc_t;

//////////////////////////////////////////////////////////////////
// Application Services
//

//----------------------------------------------------------------
// Pimux
//
typedef struct
{
  service_header_t header;
  uint8_t  send_port_num;
  uint8_t  send_pin_num;
  uint8_t  send_config_data;
  uint8_t  resp_error_code;
} pinmux_svc_t;

//----------------------------------------------------------------
// Pad Control
//
typedef struct
{
  service_header_t header;
  uint8_t  send_port_num;
  uint8_t  send_pin_num;
  uint8_t  send_config_data;
  uint8_t  resp_error_code;
} pad_control_svc_t;

/* UART Write */
typedef struct
{
  service_header_t header;
  uint32_t  string_length;
  uint8_t   string_contents[256];
  uint32_t  resp_error_code;
} uart_write_svc_t;

/**
 * System Management Services
 */

/**
 * @struct get_toc_version_svc_t
 * Get TOC Version
 */
typedef struct
{
  service_header_t header;
  uint32_t  resp_version;
  uint32_t  resp_error_code;
} get_toc_version_svc_t;

/**
 * @struct get_toc_number_svc_t
 * Get TOC Number
 */
typedef struct
{
  service_header_t header;
  uint32_t  resp_number_of_toc;
  uint32_t  resp_error_code;
} get_toc_number_svc_t;

/**
 * @struct get_toc_via_name_svc_t
 * Get TOC via name
 */
typedef struct
{
  service_header_t header;
  uint8_t   send_cpu_name[IMAGE_NAME_LENGTH];
  uint32_t  resp_error_code;
} get_toc_via_name_svc_t;

/**
 * @struct get_toc_via_cpu_id_svc_t
 * Get TOC via CPU ID
 */
typedef struct
{
  service_header_t header;
  uint32_t  send_cpu_id;
  uint8_t   resp_image_identifier[IMAGE_NAME_LENGTH];
  uint32_t  resp_image_version;
  uint32_t  resp_image_flags;
  uint32_t  resp_error_code;
} get_toc_via_cpu_id_svc_t;

/**
 * @struct get_device_part_svc_t
 * Get Device Part
 */
typedef struct
{
  service_header_t header;
  uint32_t  resp_device_string;
  uint32_t  resp_error_code;
} get_device_part_svc_t;

/**
 * @struct set_services_capabilities_t
 * Set capabilities
 * @note for now it is limited to debug toggle
 */
typedef struct
{
  service_header_t header;
  bool      services_debug;
  uint32_t  resp_error_code;
} set_services_capabilities_t;

//////////////////////////////////////////////////////////////////
// Power Services
//

//----------------------------------------------------------------
// Stop Mode Request
//
typedef struct
{
  service_header_t header;
  uint8_t enable;
  uint8_t override;
  uint8_t reserved[2];
  uint32_t ewic_wakeup_source;
} stop_mode_request_svc_t;


/*******************************************************************************
 *  G L O B A L   D E F I N E S
 ******************************************************************************/

/*******************************************************************************
 *  F U N C T I O N   P R O T O T Y P E S
 ******************************************************************************/

#endif /* __SERVICES_LIB_PROTOCOL_H__ */
