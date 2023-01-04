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
 * @file     system_utils.h
 * @author   Sudhir Sreedharan
 * @email    sudhir@alifsemi.com
 * @brief    System Header File for Utility functions
 * @version  V1.0.0
 * @date     13. May 2021
 * @bug      None
 * @Note     None
 ******************************************************************************/
#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// common helper macros

/* for Unused Arguments. */
#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)                     ((void)arg)
#endif

#ifndef ENABLE
#define ENABLE                              (0x1)
#endif

#ifndef DISABLE
#define DISABLE                             (0x0)
#endif

#ifndef MASK
#define MASK(h,l)                           (((~(0U)) << (l)) & (~(0U) >> (32 - 1 - (h))))
#endif

#define BIT(nr)                             (1UL << (nr))
#define SET_BIT(REG, BIT_Msk)               ((REG) |= (BIT_Msk))
#define CLEAR_BIT(REG, BIT_Msk)             ((REG) &= ~(BIT_Msk))
#define READ_BIT(REG, BIT_Msk)              ((REG) & (BIT_Msk))
#define CLEAR_REG(REG)                      ((REG) = (0x0))
#define WRITE_REG(REG, VAL)                 ((REG) = (VAL))
#define READ_REG(REG)                       ((REG))

// Function documentation

/**
  \fn          void* LocalToGlobal (void* in_addr)
  \brief       Return the corresponding global address
  \param[in]   in_addr  address to convert
  \return      uint32_t global address
*/
uint32_t LocalToGlobal(const volatile void *in_addr);

/**
  \fn          void* GlobalToLocal (void* in_addr)
  \brief       Return the corresponding local address
  \param[in]   in_addr  address to convert
  \return      void* local address
*/
void *GlobalToLocal(uint32_t in_addr);

/**
  \fn          void PMU_delay_loop_us(unsigned int delay_us)
  \brief       Using PMU cycle counter for delay. User need to
               take care of disabling the preemption before
               calling this PMU_delay_loop_us function. Maximum
               delay supported (2^32/(SystemCoreClock/1000000))
               micro seconds.
  \param[in]   delay_us delay in micro seconds.
*/
void PMU_delay_loop_us(unsigned int delay_us);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEM_UTILS_H */
