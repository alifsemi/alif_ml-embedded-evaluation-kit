/**
 * @file services_host_system.c
 *
 * @brief System Management services source file
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2021 Alif Group. All rights reserved.
 *
 * @ingroup host_services
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
 * @fn      uint32_t SERVICES_system_get_toc_version(uint32_t * toc_version)
 * @brief   get the TOC
 * @param   services_handle
 * @param   toc_version
 * @return
 */
uint32_t SERVICES_system_get_toc_version(uint32_t services_handle, 
                                         uint32_t * toc_version,
                                         uint32_t * error_code)
{
  get_toc_version_svc_t svc;
  memset(&svc, 0x0, sizeof(get_toc_version_svc_t));

  uint32_t ret = SERVICES_send_request(services_handle, 
                                       SERVICE_SYSTEM_MGMT_GET_TOC_VERSION, 
                                       &svc, NULL);

  *toc_version = svc.resp_version;
  *error_code = svc.resp_error_code;
  return ret;
}

/**
 * @fn      uint32_t SERVICES_system_get_toc_number(uint32_t toc_version)
 * @brief   get the TOC number
 * @param   services_handle
 * @param   toc_number
 * @return
 */
uint32_t SERVICES_system_get_toc_number(uint32_t services_handle, 
                                        uint32_t * toc_number,
                                        uint32_t * error_code)
{
  get_toc_number_svc_t svc;
  memset(&svc, 0x0, sizeof(get_toc_number_svc_t));
	
  uint32_t ret = SERVICES_send_request(services_handle, 
                                       SERVICE_SYSTEM_MGMT_GET_TOC_NUMBER, 
                                       &svc, NULL);

  *toc_number = svc.resp_number_of_toc;
  *error_code = svc.resp_error_code;
  return ret;
}

/**
 * @brief   get toc info via name
 * @param   services_handle
 * @param   cpu_name
 * @return
 */
uint32_t SERVICES_system_get_toc_via_name(uint32_t services_handle, 
                                          const uint8_t * cpu_name,
                                          uint32_t * error_code)
{
  get_toc_via_name_svc_t svc;
  memset(&svc, 0x0, sizeof(get_toc_via_name_svc_t));
  memcpy(svc.send_cpu_name, cpu_name, IMAGE_NAME_LENGTH);

  uint32_t ret = SERVICES_send_request(services_handle,
                               SERVICE_SYSTEM_MGMT_GET_TOC_VIA_CPU_NAME, 
                               &svc, NULL);
  *error_code = svc.resp_error_code;
  return ret;
}

/**
 * @brief     get toc info via cpuid
 * @param     services_handle
 * @param[in] cpuid
 * @param     toc_info
 * @return
 */
uint32_t SERVICES_system_get_toc_via_cpuid(uint32_t services_handle, 
                                           SERVICES_cpuid_t cpuid, 
                                           SERVICES_toc_info_t * toc_info,
                                           uint32_t * error_code)
{
  get_toc_via_cpu_id_svc_t svc;
  memset(&svc, 0x0, sizeof(get_toc_via_cpu_id_svc_t));

  svc.send_cpu_id = cpuid; 
  
  uint32_t ret = SERVICES_send_request(services_handle, 
                                       SERVICE_SYSTEM_MGMT_GET_TOC_VIA_CPU_ID, 
                                       &svc, NULL);

  memcpy(toc_info->image_identifier, svc.resp_image_identifier, IMAGE_NAME_LENGTH);
  toc_info->version = svc.resp_image_version;
  toc_info->flags = svc.resp_image_flags;
  *error_code = svc.resp_error_code;

  return ret; 
}

/**
 * @fn    uint32_t SERVICES_system_get_device_part_number(
 *                                uint32_t services_handle,
                                  uint32_t *device_part_number)
 * @param[in]   services_handle
 * @param[out]  device_part_number
 * @return
 */
uint32_t SERVICES_system_get_device_part_number(uint32_t services_handle, 
                                                uint32_t * device_part_number,
                                                uint32_t * error_code)
{
  get_device_part_svc_t svc;
  memset(&svc, 0x0, sizeof(get_device_part_svc_t));
  
  uint32_t ret = SERVICES_send_request(services_handle, 
                                       SERVICE_SYSTEM_MGMT_GET_DEVICE_PART_NUMBER, 
                                       &svc, NULL);

  *device_part_number = svc.resp_device_string;
  *error_code = svc.resp_error_code;

  return ret;
}

/**
 * @fn    uint32_t SERVICES_system_set_services_debug (uint32_t services_handle,
 *                                                     uint32_t * error_code)
 * @brief Set debug capability will
 * @param services_handle
 * @param error_code
 * @return
 */
uint32_t SERVICES_system_set_services_debug (uint32_t services_handle,
                                             bool debug_enable,
                                             uint32_t *error_code)
{
  set_services_capabilities_t svc = { 0 };

  svc.services_debug = debug_enable;

  uint32_t ret = SERVICES_send_request(services_handle,
                                       SERVICE_SYSTEM_MGMT_SET_CAPABILITIES_DEBUG,
                                       &svc, NULL);

  *error_code = svc.resp_error_code;

  return ret;
}
