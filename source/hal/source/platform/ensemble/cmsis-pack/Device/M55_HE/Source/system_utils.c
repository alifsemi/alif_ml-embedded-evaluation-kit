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
/******************************************************************************
 * @file     system_utils.c
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @brief    System  Utility functions
 * @version  V1.0.0
 * @date     13. May 2021
 * @bug      None
 * @Note     None
 ******************************************************************************/

#if defined (M55_HE)
  #include "M55_HE.h"
#else
  #error device not specified!
#endif

/**
  \fn          void* LocalToGlobal(void* in_addr)
  \brief       Return the corresponding global address
  \param[in]   in_addr  address to convert
  \return      void* global address
*/
void* LocalToGlobal(void *in_addr)
{
    /* Only for local TCM address, we need to map it to global address space, rest
     * for all other memories like SRAM0/1, MRAM, OctalSPI etc we can pass the address
     * as-is as those are accessed using global address
     */
    uint32_t addr = (uint32_t)in_addr;

    if((addr >= DTCM_BASE) && (addr < (DTCM_BASE + DTCM_SIZE)))
        return (void*)(addr - DTCM_BASE + DTCM_GLOBAL_BASE);
    else if((addr >= ITCM_BASE) && (addr < (ITCM_BASE+ ITCM_SIZE)))
        return (void*)(addr - ITCM_BASE + ITCM_GLOBAL_BASE);
    else
        return ((void*)addr);
}

/**
  \fn          void* GlobalToLocal(void* in_addr)
  \brief       Return the corresponding local address
  \param[in]   in_addr  address to convert
  \return      void* local address
*/
void* GlobalToLocal(void *in_addr)
{
    /* Only for local TCM address, we need to map it to local address space, rest
     * for all other memories like SRAM0/1, MRAM, OctalSPI etc we can pass the address
     * as-is as it is global.
     */
    uint32_t addr = (uint32_t)in_addr;

    if((addr >= DTCM_GLOBAL_BASE) && (addr < (DTCM_GLOBAL_BASE + DTCM_SIZE)))
        return (void*)(addr - DTCM_GLOBAL_BASE + DTCM_BASE);
    else if((addr >= ITCM_GLOBAL_BASE) && (addr < (ITCM_GLOBAL_BASE+ ITCM_SIZE)))
        return (void*)(addr - ITCM_GLOBAL_BASE + ITCM_BASE);
    else
        return ((void*)addr);
}

/**
  \fn          void PMU_delay_loop_us(unsigned int delay_us)
  \brief       Using PMU cycle counter for delay. User need to
               take care of disabling the preemption before
	       calling this PMU_delay_loop_us function. Maximum
               delay supported (2^32/(SystemCoreClock/1000000))
               micro seconds.
  \param[in]   delay_us delay in micro seconds.
*/
void PMU_delay_loop_us(unsigned int delay_us)
{
    if (delay_us == 0)
            return;
    uint32_t timestamp = ARM_PMU_Get_CCNTR();
    unsigned int delay_in_cycles = delay_us * (GetSystemCoreClock()/1000000);
    unsigned int diff = 0, curt_count = 0;

    while (diff < delay_in_cycles)
    {
        curt_count = ARM_PMU_Get_CCNTR();
        if(curt_count > timestamp)
            diff = curt_count - timestamp;
        else
            diff = (((0xFFFFFFFF) - timestamp) + curt_count);
    }
}
