/* Copyright (c) 2021 ALIF SEMICONDUCTOR

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ALIF SEMICONDUCTOR nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

/**************************************************************************//**
 * @file     delay.c
 * @author   Tanay Rami
 * @email    tanay@alifsemi.com
 * @version  V1.0.0
 * @date     05-Oct-2021
 * @brief    Common Delay functions.
 * @bug      None.
 * @Note     None.
 ******************************************************************************/

#if RTOS_THREADX
#include "tx_api.h"

/* mark as RTOS Available. */
#define RTOS_AVAILABLE     1
#endif


/* System Includes */
#include "RTE_Components.h"
#include CMSIS_device_header

/* Project Includes */
#include "delay.h"
#include "timer_ensemble.h"

/**
  \fn           void sleep_or_wait_msec(uint32_t msec)
  \brief        Delay for millisecond
                 Provide delay in terms of sleep or wait for millisecond
                 depending on RTOS availability.
  \param[in]    msec  : desired delay in milliseconds
  \return       none
*/
void sleep_or_wait_msec(uint32_t msec)
{

#if RTOS_AVAILABLE

#if RTOS_THREADX
#define MILLI_SECOND    ((ULONG) 1000)
  ULONG timer_ticks_msec = 0;

  timer_ticks_msec = ( (msec * TX_TIMER_TICKS_PER_SECOND) / MILLI_SECOND );

  if(timer_ticks_msec == 0)
    timer_ticks_msec = 1;

  /* Sleep for given millisecond. */
  tx_thread_sleep(timer_ticks_msec);

#endif /* RTOS_THREADX */

#else /* NO RTOS_AVAILABLE */

  uint32_t initial_tick_count = Get_SysTick_Count();
  while (Get_SysTick_Count() - initial_tick_count < msec) {
    __WFE();
  }
#endif /* RTOS_AVAILABLE */
}

void sleep_or_wait_usec(uint32_t usec)
{
	extern uint32_t SystemCoreClock;

	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	ARM_PMU_Enable();
	ARM_PMU_CNTR_Enable(1U << 31);
	uint32_t start = ARM_PMU_Get_CCNTR();

	uint32_t sleeptime = usec * (SystemCoreClock / 1000000);
	while ((ARM_PMU_Get_CCNTR() - start) < sleeptime);
}

/************************ (C) COPYRIGHT ALIF SEMICONDUCTOR *****END OF FILE****/
