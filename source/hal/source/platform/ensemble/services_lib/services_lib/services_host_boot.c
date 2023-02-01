/**
 * @file services_host_boot.c
 *
 * @brief Boot services service source file
 * @ingroup host-services
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
 * @brief Services Boot process TOC entry
 * @param services_handle
 * @param image_id
 * @return
 */
uint32_t SERVICES_boot_process_toc_entry(uint32_t services_handle, 
                                         const uint8_t * image_id,
                                         uint32_t * error_code)
{
  process_toc_entry_svc_t svc;
  memset(&svc, 0x0, sizeof(process_toc_entry_svc_t));

  strncpy((char *)svc.send_entry_id, (const char *)image_id, IMAGE_NAME_LENGTH);

  uint32_t ret = SERVICES_send_request(services_handle,
                               SERVICE_BOOT_PROCESS_TOC_ENTRY, &svc, NULL);
  *error_code = svc.resp_error_code;
  return ret;
}

/**
 * @brief Services Boot a CPU core
 * @param services_handle
 * @param cpu_id
 * @param address
 * @return
 */
uint32_t SERVICES_boot_cpu(uint32_t services_handle, 
                           uint32_t cpu_id,
                           uint32_t address,
                           uint32_t * error_code)
{
  boot_cpu_svc_t svc;
  memset(&svc, 0x0, sizeof(boot_cpu_svc_t));

  svc.send_cpu_id = cpu_id;
  svc.send_address = address;

  uint32_t ret = SERVICES_send_request(services_handle, SERVICE_BOOT_CPU, &svc, NULL);
  *error_code = svc.resp_error_code;
  return ret;
}

/**
 * @brief Services Release a CPU core
 * @param services_handle
 * @param cpu_id
 * @return
 */
uint32_t SERVICES_boot_release_cpu(uint32_t services_handle,
                                   uint32_t cpu_id,
                                   uint32_t * error_code)
{
  control_cpu_svc_t svc;
  memset(&svc, 0x0, sizeof(control_cpu_svc_t));

  svc.send_cpu_id = cpu_id;

  uint32_t ret = SERVICES_send_request(services_handle,
                               SERVICE_BOOT_RELEASE_CPU, &svc, NULL);
  *error_code = svc.resp_error_code;
  return ret;
}

/**
 * @brief Services Reset a CPU core
 * @param services_handle
 * @param cpu_id
 * @return
 */
uint32_t SERVICES_boot_reset_cpu(uint32_t services_handle,
                                 uint32_t cpu_id,
                                 uint32_t * error_code)
{
  control_cpu_svc_t svc;
  memset(&svc, 0x0, sizeof(control_cpu_svc_t));

  svc.send_cpu_id = cpu_id;

  uint32_t ret = SERVICES_send_request(services_handle,
                               SERVICE_BOOT_RESET_CPU, &svc, NULL);
  *error_code = svc.resp_error_code;
  return ret;
}

/**
 * @brief Services Reset the SoC
 * @param services_handle
 * @return
 */
uint32_t SERVICES_boot_reset_soc(uint32_t services_handle)
{
  service_header_t svc;
  memset(&svc, 0x0, sizeof(service_header_t));
  
  return SERVICES_send_request(services_handle, 
                               SERVICE_BOOT_RESET_SOC, &svc, NULL);
}
