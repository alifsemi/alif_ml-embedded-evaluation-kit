/**
 * @file services_lib_ids.h
 *
 * @brief Private header file for services library
 *
 * @par
 * @ingroup host_services
 *
 * Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 */
#ifndef __SERVICES_LIB_IDS_H__
#define __SERVICES_LIB_IDS_H__

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

/**
 * @enum SERVICE_ID_t Service Identifiers (SID)
 */
enum SERVICE_ID_t {
  /**
   * Maintenance Services
   */
  SERVICE_MAINTENANCE_START                     = 0,                         /**< 0 (0x0) */
  SERVICE_MAINTENANCE_HEARTBEAT_ID              = SERVICE_MAINTENANCE_START, /**< 0 (0x0) */
  SERVICE_MAINTENANCE_RTC_ID,                                                /**< 1 (0x1) */
  SERVICE_MAINTENANCE_END                       = 99,                        /**< 99 (0x63) */

  /**
   * Application Services
   */
  SERVICE_APPLICATION_START                     = 100,                       /**< 100 (0x64) */
  SERVICE_APPLICATION_CLOCK_MANAGEMENT_ID       = SERVICE_APPLICATION_START, /**< 100 (0x64) */
  SERVICE_APPLICATION_PINMUX_ID,                                             /**< 101 (0x65) */
  SERVICE_APPLICATION_PAD_CONTROL_ID,                                        /**< 102 (0x66) */
  SERVICE_APPLICATION_FIRMWARE_VERSION_ID,                                   /**< 103 (0x67) */
  SERVICE_APPLICATION_UART_WRITE_ID,                                         /**< 104 (0x68) */
  SERVICE_APPLICATION_OSPI_WRITE_KEY_ID,                                     /**< 105 (0x69) */
  SERVICE_APPLICATION_DMPU_ID,                                               /**< 106 (0x6A) */
  SERVICE_APPLICATION_END                       = 199,                       /**< 199 (0xC7) */

  /**
   * System Management Services
   */
  SERVICE_SYSTEM_MGMT_START                     = 200,                       /**< 200 (0xC8) */
  SERVICE_SYSTEM_MGMT_GET_TOC_VERSION           = SERVICE_SYSTEM_MGMT_START, /**< 200 (0xC8) */
  SERVICE_SYSTEM_MGMT_GET_TOC_NUMBER,                                        /**< 201 (0xC9) */
  SERVICE_SYSTEM_MGMT_GET_TOC_FLAGS,                                         /**< 202 (0xCA) */
  SERVICE_SYSTEM_MGMT_GET_TOC_VIA_CPU_ID,                                    /**< 203 (0xCB) */
  SERVICE_SYSTEM_MGMT_GET_TOC_VIA_CPU_NAME,                                  /**< 204 (0xCC) */
  SERVICE_SYSTEM_MGMT_GET_TOC_INFO,                                          /**< 205 (0xCD) */
  SERVICE_SYSTEM_MGMT_GET_OTP_INFO,                                          /**< 206 (0xCE) */
  SERVICE_SYSTEM_MGMT_GET_DEVICE_PART_NUMBER,                                /**< 207 (0xCF) */
  SERVICE_SYSTEM_MGMT_GET_DEVICE_REVISION_DATA,                              /**< 208 (0xD0) */
  SERVICE_SYSTEM_MGMT_SET_CAPABILITIES_DEBUG,                                /**< 209 (0xD1) */
  SERVICE_SYSTEM_MGMT_READ_OTP,                                              /**< 210 (0xD2) */
  SERVICE_SYSTEM_MGMT_WRITE_OTP,                                             /**< 211 (0xD3) */
  SERVICE_SYSTEM_MGMT_GET_ECC_PUBLIC_KEY,                                    /**< 212 (0xD4) */
  SERVICE_SYSTEM_MGMT_END                       = 299,                       /**< 299 (0x12B) */

  /**
   * Power Services
   */
  SERVICE_POWER_START                           = 300,                       /**< 300 (0x12C) */
  SERVICE_POWER_STOP_MODE_REQ_ID                = SERVICE_POWER_START,       /**< 300 (0x12C) */
  SERVICE_POWER_EWIC_CONFIG_REQ_ID,                                          /**< 301 (0x12D) */
  SERVICE_POWER_VBAT_WAKEUP_CONFIG_REQ_ID,                                   /**< 302 (0x12E) */
  SERVICE_POWER_MEM_RETENTION_CONFIG_REQ_ID,                                 /**< 303 (0x12F) */
  SERVICE_POWER_M55_HE_VTOR_SAVE_REQ_ID,                                     /**< 304 (0x130) */
  SERVICE_POWER_M55_HP_VTOR_SAVE_REQ_ID,                                     /**< 305 (0x131) */
  SERVICE_POWER_GLOBAL_STANDBY_REQ_ID,                                       /**< 306 (0x132) */
  SERVICE_POWER_MEMORY_POWER_REQ_ID,                                         /**< 307 (0x133) */
  SERVICE_POWER_DCDC_VOLTAGE_REQ_ID,                                         /**< 308 (0x134) */
  SERVICE_POWER_LDO_VOLTAGE_REQ_ID,                                          /**< 309 (0x135) */
  SERVICE_POWER_GET_RUN_REQ_ID,                                              /**< 310 (0x136) */
  SERVICE_POWER_SET_RUN_REQ_ID,                                              /**< 311 (0x137) */
  SERVICE_POWER_GET_OFF_REQ_ID,                                              /**< 312 (0x138) */
  SERVICE_POWER_SET_OFF_REQ_ID,                                              /**< 313 (0x139) */
  SERVICE_POWER_SETTING_CONFIG_REQ_ID,                                       /**< 314 (0x13A) */
  SERVICE_POWER_SETTING_GET_REQ_ID,                                          /**< 315 (0x13B) */
  SERVICE_POWER_SE_SLEEP_REQ_ID,                                             /**< 316 (0x13C) */
  SERVICE_POWER_STOP_MODE_REQ_RAW_ID,                                        /**< 317 (0x13D) */
  SERVICE_POWER_EWIC_CONFIG_REQ_RAW_ID,                                      /**< 318 (0x13E) */
  SERVICE_POWER_VBAT_WAKEUP_CONFIG_REQ_RAW_ID,                               /**< 319 (0x13F) */
  SERVICE_POWER_MEM_RETENTION_CONFIG_REQ_RAW_ID,                             /**< 320 (0x140) */
  SERVICE_POWER_M55_HE_VTOR_SAVE_REQ_RAW_ID,                                 /**< 321 (0x141) */
  SERVICE_POWER_END                             = 399,                       /**< 399 (0x18F) */

  /**
   * Cryptocell / Security Services
   */
  SERVICE_CRYPTOCELL_START                      = 400,                       /**< 400 (0x190) */
  SERVICE_CRYPTOCELL_GET_RND                    = SERVICE_CRYPTOCELL_START,  /**< 400 (0x190) */
  SERVICE_CRYPTOCELL_GET_LCS,                                                /**< 401 (0x191) */
  SERVICE_CRYPTOCELL_MBEDTLS_AES_INIT,                                       /**< 402 (0x192) */
  SERVICE_CRYPTOCELL_MBEDTLS_AES_SET_KEY,                                    /**< 403 (0x193) */
  SERVICE_CRYPTOCELL_MBEDTLS_AES_CRYPT,                                      /**< 404 (0x194) */
  SERVICE_CRYPTOCELL_MBEDTLS_CCM_GCM_SET_KEY,                                /**< 405 (0x195) */
  SERVICE_CRYPTOCELL_MBEDTLS_CCM_GCM_CRYPT,                                  /**< 406 (0x196) */
  SERVICE_CRYPTOCELL_MBEDTLS_CHACHA20_CRYPT,                                 /**< 407 (0x197) */
  SERVICE_CRYPTOCELL_MBEDTLS_CHACHAPOLY_CRYPT,                               /**< 408 (0x198) */
  SERVICE_CRYPTOCELL_MBEDTLS_POLY1305_CRYPT,                                 /**< 409 (0x199) */
  SERVICE_CRYPTOCELL_MBEDTLS_SHA_STARTS,                                     /**< 410 (0x19A) */
  SERVICE_CRYPTOCELL_MBEDTLS_SHA_PROCESS,                                    /**< 411 (0x19B) */
  SERVICE_CRYPTOCELL_MBEDTLS_SHA_UPDATE,                                     /**< 412 (0x19C) */
  SERVICE_CRYPTOCELL_MBEDTLS_SHA_FINISH,                                     /**< 413 (0x19D) */
  SERVICE_CRYPTOCELL_MBEDTLS_TRNG_HARDWARE_POLL,                             /**< 414 (0x19E) */
  SERVICE_CRYPTOCELL_MBEDTLS_CMAC_INIT_SETKEY,                               /**< 415 (0x19F) */
  SERVICE_CRYPTOCELL_MBEDTLS_CMAC_UPDATE,                                    /**< 416 (0x1A0) */
  SERVICE_CRYPTOCELL_MBEDTLS_CMAC_FINISH,                                    /**< 417 (0x1A1) */
  SERVICE_CRYPTOCELL_MBEDTLS_CMAC_RESET,                                     /**< 418 (0x1A2) */
  SERVICE_CRYPTOCELL_MBEDTLS_AES,                                            /**< 419 (0x1A3) */
  SERVICE_CRYPTOCELL_MBEDTLS_SHA,                                            /**< 420 (0x1A4) */
  SERVICE_CRYPTOCELL_MBEDTLS_CMAC,                                           /**< 421 (0x1A5) */
  SERVICE_CRYPTOCELL_MBEDTLS_CCM_GCM,                                        /**< 422 (0x1A6) */
  SERVICE_CRYPTOCELL_END                        = 499,                       /**< 499 (0x1F3) */

  /**
   * Boot Services
   */
  SERVICE_BOOT_START                            = 500,                       /**< 500 (0x1F4) */
  SERVICE_BOOT_PROCESS_TOC_ENTRY                = SERVICE_BOOT_START,        /**< 500 (0x1F4) */
  SERVICE_BOOT_CPU,                                                          /**< 501 (0x1F5) */
  SERVICE_BOOT_RELEASE_CPU,                                                  /**< 502 (0x1F6) */
  SERVICE_BOOT_RESET_CPU,                                                    /**< 503 (0x1F7) */
  SERVICE_BOOT_RESET_SOC,                                                    /**< 504 (0x1F8) */
  SERVICE_BOOT_SET_VTOR,                                                     /**< 505 (0x1F9) */
  SERVICE_BOOT_SET_ARGS,                                                     /**< 506 (0x1FA) */
  SERVICE_BOOT_END                              = 599,                       /**< 599 (0x257) */

  /**
   * Update Services
   */
  SERVICE_UPDATE_START                          = 600,                       /**< 600 (0x258) */
  SERVICE_UPDATE_STOC                           = SERVICE_UPDATE_START,      /**< 600 (0x258) */
  SERVICE_UPDATE_END                            = 699,                       /**< 699 (0x2BB) */

  /**
   * Clocks Services
   */
  SERVICE_CLOCK_START                           = 700,                       /**< 700 (0x2BC) */
  SERVICE_CLOCK_SELECT_OSC_SOURCE               = SERVICE_CLOCK_START,       /**< 700 (0x2BC) */
  SERVICE_CLOCK_SELECT_PLL_SOURCE,                                           /**< 701 (0x2BD) */
  SERVICE_CLOCK_SET_ENABLE,                                                  /**< 702 (0x2BE) */
  SERVICE_CLOCK_ES0_SET_FREQ,                                                /**< 703 (0x2BF) */
  SERVICE_CLOCK_ES1_SET_FREQ,                                                /**< 704 (0x2C0) */
  SERVICE_CLOCK_SELECT_A32_SOURCE,                                           /**< 705 (0x2C1) */
  SERVICE_CLOCK_SELECT_ACLK_SOURCE,                                          /**< 706 (0x2C2) */
  SERVICE_CLOCK_SET_DIVIDER,                                                 /**< 707 (0x2C3) */
  SERVICE_PLL_INITIALIZE,                                                    /**< 708 (0x2C4) */
  SERVICE_PLL_DEINIT,                                                        /**< 709 (0x2C5) */
  SERVICE_PLL_XTAL_START,                                                    /**< 710 (0x2C6) */
  SERVICE_PLL_XTAL_STOP,                                                     /**< 711 (0x2C7) */
  SERVICE_PLL_XTAL_IS_STARTED,                                               /**< 712 (0x2C8) */
  SERVICE_PLL_CLKPLL_START,                                                  /**< 713 (0x2C9) */
  SERVICE_PLL_CLKPLL_STOP,                                                   /**< 714 (0x2CA) */
  SERVICE_PLL_CLKPLL_IS_LOCKED,                                              /**< 715 (0x2CB) */
  SERVICE_CLOCK_GET_CLOCKS,                                                  /**< 716 (0x2CC) */
  SERVICE_CLOCK_SETTING_GET_REQ_ID,                                          /**< 717 (0x2CD) */
  SERVICE_CLOCK_END                             = 799,                       /**< 799 (0x31F) */

  /**
   * ExtSys0 Services
   */
  SERVICE_EXTSYS0_START                         = 800,                       /**< 800 (0x320) */
  SERVICE_EXTSYS0_BOOT_SET_ARGS                 = SERVICE_EXTSYS0_START,     /**< 800 (0x320) */
  SERVICE_EXTSYS0_EXTSYS1_WAKEUP,                                            /**< 801 (0x321) */
  SERVICE_EXTSYS0_SHUTDOWN,                                                  /**< 802 (0x322) */
  SERVICE_EXTSYS0_END                           = 899                        /**< 899 (0x383) */
};

#endif /* __SERVICES_LIB_IDS_H__ */
