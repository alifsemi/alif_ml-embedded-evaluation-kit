/**
 * @file services_lib_public.h
 *
 * @brief Public header file for services library
 * @ingroup host_services
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
 */

#ifndef __SERVICES_LIB_PUBLIC_H__
#define __SERVICES_LIB_PUBLIC_H__

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/


/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/
/**
 * Version   JIRA         Description
 * 0.0.7    SE-1165       mbed TLS accelerators
 * 0.0.6    SE-1173       Added enable / disable debug status
 * 0.0.5    SE-1144       Service API error code added
 * 0.0.4    SE-700        Service BOOT reset added
 * 0.0.3    SE-827        RPC Parameter changes
 * 0.0.2    SE-708        First refactoring
 * 0.0.1                  First implementation
 */
#define SE_SERVICES_VERSION_STRING "0.0.7"

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

/**
 * @enum SERVICE_ID_t Service Identifiers (SID)
 */
typedef enum
{
  /**
   * Maintenance Services
   */
  SERVICE_MAINTENANCE_START  = 0,                                     /**< SERVICE_MAINTENANCE_START        */
  SERVICE_MAINTENANCE_HEARTBEAT_ID = SERVICE_MAINTENANCE_START,       /**< SERVICE_MAINTENANCE_HEARTBEAT_ID */
  SERVICE_MAINTENANCE_RTC_ID,                                         /**< SERVICE_MAINTENANCE_RTC_ID       */
  SERVICE_MAINTENANCE_END    = 99,                                    /**< SERVICE_MAINTENANCE_END          */

  /**
   * Application Services
   */
  SERVICE_APPLICATION_START  = 100,                                   /**< SERVICE_APPLICATION_START */
  SERVICE_APPLICATION_CLOCK_MANAGEMENT_ID = SERVICE_APPLICATION_START,/**< SERVICE_APPLICATION_CLOCK_MANAGEMENT_ID */
  SERVICE_APPLICATION_PINMUX_ID,                                      /**< SERVICE_APPLICATION_PINMUX_ID */
  SERVICE_APPLICATION_PAD_CONTROL_ID,                                 /**< SERVICE_APPLICATION_PAD_CONTROL */
  SERVICE_APPLICATION_FIRMWARE_VERSION_ID,                            /**< SERVICE_APPLICATION_FIRMWARE_VERSION_ID */
  SERVICE_APPLICATION_UART_WRITE_ID,                                  /**< SERVICE_APPLICATION_UART_WRITE_ID */
  SERVICE_APPLICATION_END    = 199,                                   /**< SERVICE_APPLICATION_END */

  /**
   * System Management Services
   */
  SERVICE_SYSTEM_MGMT_START  = 200,                                   /**< SERVICE_SYSTEM_MGMT_START */
  SERVICE_SYSTEM_MGMT_GET_TOC_VERSION = SERVICE_SYSTEM_MGMT_START,
  SERVICE_SYSTEM_MGMT_GET_TOC_NUMBER,
  SERVICE_SYSTEM_MGMT_GET_TOC_FLAGS,
  SERVICE_SYSTEM_MGMT_GET_TOC_VIA_CPU_ID,
  SERVICE_SYSTEM_MGMT_GET_TOC_VIA_CPU_NAME,
  SERVICE_SYSTEM_MGMT_GET_DEVICE_PART_NUMBER,                         /**< SERVICE_SYSTEM_MGMT_GET_DEVICE_PART_NUMBER */
  SERVICE_SYSTEM_MGMT_SET_CAPABILITIES_DEBUG,                         /**< SERVICE_SYSTEM_MGMT_SET_CAPABILITIES_DEBUG */
  SERVICE_SYSTEM_MGMT_END    = 299,                                   /**< SERVICE_SYSTEM_MGMT_END */

  /**
   * Power Services
   */
  SERVICE_POWER_START        = 300,                                   /**< SERVICE_POWER_START */
  SERVICE_POWER_STOP_MODE_REQ_ID,                                     /**< SERVICE_POWER_STOP_MODE_REQ_ID */
  SERVICE_POWER_END          = 399,                                   /**< SERVICE_POWER_END */

  /**
   * Cryptocell / Security Services
   */
  SERVICE_CRYPTOCELL_START   = 400,                                   /**< SERVICE_CRYPTOCELL_START */
  SERVICE_CRYPTOCELL_GET_RND =  SERVICE_CRYPTOCELL_START,             /**< SERVICE_CRYPTOCELL_GET_RND */
  SERVICE_CRYPTOCELL_GET_LCS,                                         /**< SERVICE_CRYPTOCELL_GET_LCS */
  SERVICE_CRYPTOCELL_MBEDTLS_AES_INIT,                                /**< SERVICE_CRYPTOCELL_MBEDTLS_AES_INIT */
  SERVICE_CRYPTOCELL_MBEDTLS_AES_SET_KEY,                             /**< SERVICE_CRYPTOCELL_MBEDTLS_AES_SET_KEY */
  SERVICE_CRYPTOCELL_MBEDTLS_AES_CRYPT,                               /**< SERVICE_CRYPTOCELL_MBEDTLS_AES_CRYPT_ECB */
  SERVICE_CRYPTOCELL_MBEDTLS_CCM_SET_KEY,                             /**< SERVICE_CRYPTOCELL_MBEDTLS_CCM_SET_KEY */
  SERVICE_CRYPTOCELL_MBEDTLS_CCM_CRYPT,                               /**< SERVICE_CRYPTOCELL_MBEDTLS_CCM_CRYPT */
  SERVICE_CRYPTOCELL_MBEDTLS_CHACHA20_CRYPT,                          /**< SERVICE_CRYPTOCELL_MBEDTLS_CHACHA20_CRYPT */
  SERVICE_CRYPTOCELL_MBEDTLS_CHACHAPOLY_CRYPT,                        /**< SERVICE_CRYPTOCELL_MBEDTLS_CHACHAPOLY_CRYPT */
  SERVICE_CRYPTOCELL_MBEDTLS_POLY1305_CRYPT,                          /**< SERVICE_CRYPTOCELL_MBEDTLS_POLY1305_CRYPT */
  SERVICE_CRYPTOCELL_MBEDTLS_SHA_STARTS,                              /**< SERVICE_CRYPTOCELL_MBEDTLS_SHA_STARTS */
  SERVICE_CRYPTOCELL_MBEDTLS_SHA_PROCESS,                             /**< SERVICE_CRYPTOCELL_MBEDTLS_SHA_PROCESS */
  SERVICE_CRYPTOCELL_MBEDTLS_SHA_UPDATE,                              /**< SERVICE_CRYPTOCELL_MBEDTLS_SHA_UPDATE */
  SERVICE_CRYPTOCELL_MBEDTLS_SHA_FINISH,                              /**< SERVICE_CRYPTOCELL_MBEDTLS_SHA_FINISH */
  SERVICE_CRYPTOCELL_MBEDTLS_TRNG_HARDWARE_POLL,                      /**< SERVICE_CRYPTOCELL_MBEDTLS_TRNG_HARDWARE_POLL */
  SERVICE_CRYPTOCELL_END     = 499,                                   /**< SERVICE_CRYPTOCELL_END */

  /**
   * Boot Services
   */
  SERVICE_BOOT_START         = 500,                                   /**< SERVICE_BOOT_START */
  SERVICE_BOOT_PROCESS_TOC_ENTRY = SERVICE_BOOT_START,                /**< SERVICE_BOOT_PROCESS_TOC_ENTRY */
  SERVICE_BOOT_CPU,                                                   /**< SERVICE_BOOT_CPU */
  SERVICE_BOOT_RELEASE_CPU,                                           /**< SERVICE_BOOT_RESEASE_CPU */
  SERVICE_BOOT_RESET_CPU,                                             /**< SERVICE_BOOT_RESET_CPU */
  SERVICE_BOOT_RESET_SOC,                                             /**< SERVICE_BOOT_RESET_SOC */
  SERVICE_BOOT_END           = 599,                                   /**< SERVICE_BOOT_END */

  /**
   * Update Services
   */
  SERVICE_UPDATE_START       = 600,                                   /**< SERVICE_UPDATE_START */
  SERVICE_UPDATE_END         = 699                                    /**< SERVICE_UPDATE_END */
} SERVICE_ID_t;


/**
 * @struct  SERVICE_toc_info_t
 * @brief   Table of Contents data used by services.
 */
typedef struct
{
  uint8_t   toc_name[8]; /* @todo Change hard coded number */
  uint32_t  toc_version;
  uint32_t  toc_flags;
} SERVICE_toc_info_t;


//-------------------------------------------------------------------
// MbedTLS services
//
#define MBEDTLS_OP_DECRYPT      0
#define MBEDTLS_OP_ENCRYPT      1

#define MBEDTLS_AES_CRYPT_ECB   0
#define MBEDTLS_AES_CRYPT_CBC   1
#define MBEDTLS_AES_CRYPT_CTR   2
#define MBEDTLS_AES_CRYPT_OFB   3

#define MBEDTLS_HASH_SHA1       0
#define MBEDTLS_HASH_SHA224     1
#define MBEDTLS_HASH_SHA256     2


/*******************************************************************************
 *  G L O B A L   D E F I N E S
 ******************************************************************************/


/*******************************************************************************
 *  F U N C T I O N   P R O T O T Y P E S
 ******************************************************************************/

#endif /* __SERVICES_LIB_PUBLIC_H__ */
