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

#ifdef __cplusplus
extern "C" {
#endif

// common helper macros

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
  \return      void* global address
*/
void* LocalToGlobal(void *in_addr);

/**
  \fn          void* GlobalToLocal (void* in_addr)
  \brief       Return the corresponding local address
  \param[in]   in_addr  address to convert
  \return      void* local address
*/
void* GlobalToLocal(void *in_addr);

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
