/* Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
   *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */
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
    else if((addr < (ITCM_BASE+ ITCM_SIZE)))
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
