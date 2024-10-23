/**
 * @file services_host_clocks.c
 *
 * @brief Clocks services service source file
 * @ingroup host-services
 * @par
 *
 * Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "services_lib_api.h"
#include "services_lib_protocol.h"
#include "services_lib_ids.h"

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

#define BIT0        0x01
#define BIT1        0x02
#define BIT2        0x04
#define BIT3        0x08
#define BIT4        0x10
#define BIT5        0x20
#define BIT6        0x40
#define BIT7        0x80
#define BIT8        0x0100
#define BIT9        0x0200
#define BIT10       0x0400
#define BIT11       0x0800
#define BIT12       0x1000
#define BIT13       0x2000
#define BIT14       0x4000
#define BIT15       0x8000
#define BIT16       0x00010000UL
#define BIT17       0x00020000UL
#define BIT18       0x00040000UL
#define BIT19       0x00080000UL
#define BIT20       0x00100000UL
#define BIT21       0x00200000UL
#define BIT22       0x00400000UL
#define BIT23       0x00800000UL
#define BIT24       0x01000000UL
#define BIT25       0x02000000UL
#define BIT26       0x04000000UL
#define BIT27       0x08000000UL
#define BIT28       0x10000000UL
#define BIT29       0x20000000UL
#define BIT30       0x40000000UL
#define BIT31       0x80000000UL

#define FREQ_38_4_MHz  38400000
#define FREQ_76_8_MHz  76800000
#define FREQ_100_MHz   100000000
#define FREQ_400_MHz   400000000
/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

/*******************************************************************************
 *  G L O B A L   V A R I A B L E S
 ******************************************************************************/

static uint32_t s_se_scaled_frequency[SCALED_FREQ_NONE] =
    {76800000, 38400000, 19200000, 9600000, 4800000, 2400000, 1200000, 600000,  // 1/X
     76800000, 38400000, 19200000, 4800000, 1200000, 600000, 300000, 75000,     // 1/Y
     38400000, 19200000, 9600000, 4800000, 2400000, 1200000, 600000, 300000,    // 1/Z low
     38400000, 19200000, 9600000, 2400000, 600000, 300000, 150000, 37500};      // 1/Z high

/*******************************************************************************
 *  C O D E
 ******************************************************************************/

/**
 * @fn   uint32_t SERVICES_clocks_select_osc_source(uint32_t services_handle,
 *                                                  oscillator_source_t source,
 *                                                  oscillator_target_t target,
 *                                                  uint32_t * error_code)
 * @brief Select RC or XTAL as Oscillator clock source
 * @param services_handle
 * @param source            RC or XTAL
 * @param target            SYSCLK (HF), PERIPHCLK (HF), S32K (LF)
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_select_osc_source(uint32_t services_handle,
                                           oscillator_source_t source,
                                           oscillator_target_t target,
                                           uint32_t * error_code)
{
  clk_select_clock_source_svc_t * p_svc =
      (clk_select_clock_source_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(clk_select_clock_source_svc_t));

  p_svc->send_clock_source = source;
  p_svc->send_clock_target = target;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_CLOCK_SELECT_OSC_SOURCE, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn   uint32_t SERVICES_clocks_select_pll_source(uint32_t services_handle,
 *                                                  oscillator_source_t source,
 *                                                  oscillator_target_t target,
 *                                                  uint32_t * error_code)
 * @brief Select Oscillator or PLL clock source for various target clocks
 * @param services_handle
 * @param source            Oscillator or PLL
 * @param target            SYSREFCLK, SYSCLK, ExtSus0, ExtSys1
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_select_pll_source(uint32_t services_handle,
                                           pll_source_t source,
                                           pll_target_t target,
                                           uint32_t * error_code)
{
  clk_select_clock_source_svc_t * p_svc =
      (clk_select_clock_source_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(clk_select_clock_source_svc_t));

  p_svc->send_clock_source = source;
  p_svc->send_clock_target = target;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_CLOCK_SELECT_PLL_SOURCE, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn   uint32_t SERVICES_clocks_enable_clock(uint32_t services_handle,
 *                                             clock_enable_t clock,
 *                                             bool enable,
 *                                             uint32_t * error_code)
 * @brief Select Oscialltor or PLL clock source for various target clocks
 * @param services_handle
 * @param clock             Clock to enable or disable
 * @param enable            Enable/Disable flag
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_enable_clock(uint32_t services_handle,
                                      clock_enable_t clock,
                                      bool enable,
                                      uint32_t * error_code)
{
  clk_set_enable_svc_t * p_svc =
      (clk_set_enable_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(clk_set_enable_svc_t));

  p_svc->send_clock_type = clock;
  p_svc->send_enable = enable;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_CLOCK_SET_ENABLE, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_clocks_set_ES0_frequency(uint32_t services_handle,
 *                                                 clock_frequency_t frequency,
 *                                                 uint32_t * error_code)
 * @brief Set the clock frequency for External System 0 (M55-HP)
 * @param services_handle
 * @param frequency         Clock frequency
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_set_ES0_frequency(uint32_t services_handle,
                                           clock_frequency_t frequency,
                                           uint32_t * error_code)
{
  clk_m55_set_frequency_svc_t * p_svc =
      (clk_m55_set_frequency_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(clk_m55_set_frequency_svc_t));

  p_svc->send_frequency = frequency;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_CLOCK_ES0_SET_FREQ, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_clocks_set_ES1_frequency(uint32_t services_handle,
 *                                                 clock_frequency_t frequency,
 *                                                 uint32_t * error_code)
 * @brief Set the clock frequency for External System 1 (M55-HE)
 * @param services_handle
 * @param frequency         Clock frequency
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_set_ES1_frequency(uint32_t services_handle,
                                           clock_frequency_t frequency,
                                           uint32_t * error_code)
{
  clk_m55_set_frequency_svc_t * p_svc =
      (clk_m55_set_frequency_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(clk_m55_set_frequency_svc_t));

  p_svc->send_frequency = frequency;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_CLOCK_ES1_SET_FREQ, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_clocks_select_a32_source(uint32_t services_handle,
 *                                                 a32_source_t source,
 *                                                 uint32_t * error_code)
 * @brief Select a source clock for the A32 cores
 * @param services_handle
 * @param source            Clock source
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_select_a32_source(uint32_t services_handle,
                                           a32_source_t source,
                                           uint32_t * error_code)
{
  clk_select_sys_clk_source_svc_t * p_svc =
      (clk_select_sys_clk_source_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(clk_select_sys_clk_source_svc_t));

  p_svc->send_source = source;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_CLOCK_SELECT_A32_SOURCE, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_clocks_select_aclk_source(uint32_t services_handle,
 *                                                  aclk_source_t source,
 *                                                  uint32_t * error_code)
 * @brief Select a source clock for system buses (AXI, AHB, APB)
 * @param services_handle
 * @param source            Clock source
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_select_aclk_source(uint32_t services_handle,
                                            aclk_source_t source,
                                            uint32_t * error_code)
{
  clk_select_sys_clk_source_svc_t * p_svc =
      (clk_select_sys_clk_source_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(clk_select_sys_clk_source_svc_t));

  p_svc->send_source = source;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_CLOCK_SELECT_ACLK_SOURCE, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_clocks_set_divider(uint32_t services_handle,
 *                                           clock_divider_t divider,
 *                                           uint32_t value,
 *                                           uint32_t * error_code)
 * @brief Set the value of a divider
 * @param services_handle
 * @param divider           Which divider
 * @param value             Divider value
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_set_divider(uint32_t services_handle,
                                     clock_divider_t divider,
                                     uint32_t value,
                                     uint32_t * error_code)
{
  clk_set_clk_divider_svc_t * p_svc =
      (clk_set_clk_divider_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(clk_set_clk_divider_svc_t));

  p_svc->send_divider = divider;
  p_svc->send_value = value;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_CLOCK_SET_DIVIDER, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_pll_initialize(uint32_t services_handle,
 *                                       uint32_t * error_code)
 * @brief High-level PLL initialize - startup HF XTAL and PLL, and switch to PLL
 * @param services_handle
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_pll_initialize(uint32_t services_handle,
                                 uint32_t * error_code)
{
  generic_svc_t * p_svc =
      (generic_svc_t *)SERVICES_prepare_packet_buffer(sizeof(generic_svc_t));

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_PLL_INITIALIZE, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_pll_deinit(uint32_t services_handle,
 *                                   uint32_t * error_code)
 * @brief High-level PLL deinit - switch to RC clocks, stop PLL and HF XTAL
 * @param services_handle
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_pll_deinit(uint32_t services_handle, uint32_t * error_code)
{
  generic_svc_t * p_svc =
      (generic_svc_t *)SERVICES_prepare_packet_buffer(sizeof(generic_svc_t));

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_PLL_DEINIT, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_pll_xtal_start(uint32_t services_handle,
 *                                       bool faststart,
 *                                       bool boost,
 *                                       uint32_t delay_count,
 *                                       uint32_t * error_code)
 * @brief Start the HF XTAL
 * @param services_handle
 * @param faststart         Enable faststart mode
 * @param boost             Enable boost mode
 * @param delay_count       Wait time
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_pll_xtal_start(uint32_t services_handle,
                                 bool faststart,
                                 bool boost,
                                 uint32_t delay_count,
                                 uint32_t * error_code)
{
  pll_xtal_start_svc_t * p_svc = (pll_xtal_start_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(pll_xtal_start_svc_t));

  p_svc->send_faststart = faststart;
  p_svc->send_boost = boost;
  p_svc->send_delay_count = delay_count;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_PLL_XTAL_START, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_pll_xtal_stop(uint32_t services_handle,
 *                                      uint32_t * error_code)
 * @brief Stop the HF XTAL
 * @param services_handle
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_pll_xtal_stop(uint32_t services_handle,
                                uint32_t * error_code)
{
  generic_svc_t * p_svc =
      (generic_svc_t *)SERVICES_prepare_packet_buffer(sizeof(generic_svc_t));

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_PLL_XTAL_STOP, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_pll_xtal_is_started(uint32_t services_handle,
 *                                            bool * is_started,
 *                                            uint32_t * error_code)
 * @brief Check if the HF XTAL is started
 * @param services_handle
 * @param is_started
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_pll_xtal_is_started(uint32_t services_handle,
                                      bool * is_started,
                                      uint32_t * error_code)
{
  generic_svc_t * p_svc =
      (generic_svc_t *)SERVICES_prepare_packet_buffer(sizeof(generic_svc_t));

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_PLL_XTAL_IS_STARTED, DEFAULT_TIMEOUT);

  *is_started = p_svc->resp_error_code != 0x0;
  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_pll_clkpll_start(uint32_t services_handle,
 *                                         bool faststart,
 *                                         uint32_t delay_count,
 *                                         uint32_t * error_code)
 * @brief Start the PLL
 * @param services_handle
 * @param faststart         Enable faststart mode
 * @param delay_count       Wait time
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_pll_clkpll_start(uint32_t services_handle,
                                   bool faststart,
                                   uint32_t delay_count,
                                   uint32_t * error_code)
{
  pll_clkpll_start_svc_t * p_svc = (pll_clkpll_start_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(pll_clkpll_start_svc_t));

  p_svc->send_faststart = faststart;
  p_svc->send_delay_count = delay_count;

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_PLL_CLKPLL_START, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_pll_clkpll_stop(uint32_t services_handle,
 *                                        uint32_t * error_code)
 * @brief Stop the PLL
 * @param services_handle
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_pll_clkpll_stop(uint32_t services_handle,
                                  uint32_t * error_code)
{
  generic_svc_t * p_svc =
      (generic_svc_t *)SERVICES_prepare_packet_buffer(sizeof(generic_svc_t));

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_PLL_CLKPLL_STOP, DEFAULT_TIMEOUT);

  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_pll_clkpll_is_locked(uint32_t services_handle,
 *                                             bool * is_locked,
 *                                             uint32_t * error_code)
 * @brief Check if the PLL is started and locked
 * @param services_handle
 * @param is_locked         Which divider
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_pll_clkpll_is_locked(uint32_t services_handle,
                                       bool * is_locked,
                                       uint32_t * error_code)
{
  generic_svc_t * p_svc =
      (generic_svc_t *)SERVICES_prepare_packet_buffer(sizeof(generic_svc_t));

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_PLL_CLKPLL_IS_LOCKED, DEFAULT_TIMEOUT);

  *is_locked = p_svc->resp_error_code != 0x0;
  *error_code = p_svc->resp_error_code;
  return ret;
}

/**
 * @fn  uint32_t SERVICES_clocks_get_clocks(uint32_t services_handle,
 *                                          clk_get_clocks_svc_t ** pp_svc,
 *                                          uint32_t * error_code)
 * @brief Get the values of the clocks registers
 * @param services_handle
 * @param pp_svc            Service struct definition
 * @param scaled_clk_freq   Scaled clock frequency
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_get_clocks(uint32_t services_handle,
                                    clk_get_clocks_svc_t ** pp_svc,
                                    scaled_clk_freq_t * scaled_clk_freq,
                                    uint32_t * error_code)
{
  *pp_svc = (clk_get_clocks_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(clk_get_clocks_svc_t));

  uint32_t ret = SERVICES_send_request(services_handle,
      SERVICE_CLOCK_GET_CLOCKS, DEFAULT_TIMEOUT);

  *error_code = (*pp_svc)->resp_error_code;
  if (ret != SERVICES_REQ_SUCCESS)
  {
    return ret;
  }

  // Get the scaled clock frequency
  power_setting_svc_t * p_svc =
      (power_setting_svc_t *)
      SERVICES_prepare_packet_buffer(sizeof(power_setting_svc_t));

  p_svc->send_setting_type = POWER_SETTING_SCALED_CLK_FREQ;
  SERVICES_send_request(services_handle,
      SERVICE_POWER_SETTING_GET_REQ_ID, DEFAULT_TIMEOUT);
  *scaled_clk_freq = p_svc->value;

  return SERVICES_REQ_SUCCESS;
}

/**
 * @fn  uint32_t SERVICES_clocks_get_apb_frequency(uint32_t services_handle,
 *                                                 uint32_t * frequency,
 *                                                 uint32_t * error_code)
 * @brief Get the APB clock frequency
 * @param services_handle
 * @param frequency         calculated APB frequency in Hz
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_get_apb_frequency(uint32_t services_handle,
                                           uint32_t * frequency,
                                           uint32_t * error_code)
{
  clk_get_clocks_svc_t * p_clocks;
  scaled_clk_freq_t scaled_freq;
  uint32_t ret =
      SERVICES_clocks_get_clocks(services_handle, &p_clocks, &scaled_freq, error_code);

  if (ret != SERVICES_REQ_SUCCESS)
  {
    return ret;
  }

  *frequency = 0;

  uint32_t osc_freq = (p_clocks->cgu_osc_ctrl & BIT0) > 0 ?
      FREQ_38_4_MHz : FREQ_76_8_MHz;
  if (scaled_freq < SCALED_FREQ_NONE)
  {
    osc_freq = s_se_scaled_frequency[scaled_freq];
  }

  uint32_t calc_freq = 0;
  a32_source_t aclk = p_clocks->aclk_ctrl & (BIT1 | BIT0);
  if (A32_SYSPLL == aclk)
  {
    bool syspll_clk_is_pll = (p_clocks->cgu_pll_sel & BIT4) > 0;
    calc_freq = syspll_clk_is_pll ? FREQ_400_MHz : osc_freq;
    uint32_t syspll_clk_divider =
        p_clocks->hostcpuclk_div1 & (BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
    syspll_clk_divider += 1;
    calc_freq /= syspll_clk_divider;
  }
  else if (A32_REFCLK == aclk)
  {
    uint32_t refclk_freq = (p_clocks->cgu_pll_sel & BIT0) > 0 ?
        FREQ_100_MHz : osc_freq;

    calc_freq = refclk_freq;
  }

  uint32_t apb_divider = p_clocks->systop_clk_div & (BIT1 | BIT0);
  if (0x0 == apb_divider)
  {
    apb_divider = 1;
  }
  else if (0x1 == apb_divider)
  {
    apb_divider = 2;
  }
  else  // 0x2, 0x3
  {
    apb_divider = 4;
  }
  calc_freq /= apb_divider;

  *frequency = calc_freq;
  return SERVICES_REQ_SUCCESS;
}

/**
 * @fn  uint32_t SERVICES_clocks_get_refclk_frequency(uint32_t services_handle,
 *                                                    uint32_t * frequency,
 *                                                    uint32_t * error_code)
 * @brief Get the REFCLK frequency
 * @param services_handle
 * @param frequency         calculated REFCLK frequency in Hz
 * @param error_code        Service error code
 * @return                  Transport layer error code
 */
uint32_t SERVICES_clocks_get_refclk_frequency(uint32_t services_handle,
                                              uint32_t * frequency,
                                              uint32_t * error_code)
{
  clk_get_clocks_svc_t * p_clocks;
  scaled_clk_freq_t scaled_freq;
  uint32_t ret =
      SERVICES_clocks_get_clocks(services_handle, &p_clocks, &scaled_freq, error_code);

  if (ret != SERVICES_REQ_SUCCESS)
  {
    return ret;
  }

  uint32_t osc_freq = (p_clocks->cgu_osc_ctrl & BIT0) > 0 ?
      FREQ_38_4_MHz : FREQ_76_8_MHz;
  if (scaled_freq < SCALED_FREQ_NONE)
  {
    osc_freq = s_se_scaled_frequency[scaled_freq];
  }

  uint32_t refclk_freq = (p_clocks->cgu_pll_sel & BIT0) > 0 ?
      FREQ_100_MHz : osc_freq;

  *frequency = refclk_freq;
  return SERVICES_REQ_SUCCESS;
}
