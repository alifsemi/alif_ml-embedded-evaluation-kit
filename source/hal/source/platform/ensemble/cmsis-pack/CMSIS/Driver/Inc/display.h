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
 * @file     display.h
 * @author   Girish BN
 * @email    girish.bn@alifsemi.com
 * @version  V1.0.0
 * @date     30-Sep-2021
 * @brief    display driver header.
 * @bug      None.
 * @Note     None
 ******************************************************************************/

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdint.h>

// Use to change between displays
//#define E50RA_MW550_N
#define E43RB_FW405_C
//#define E43GB_MW405_C

#if defined(E50RA_MW550_N)
#define RATE_464M
#define VACT 854
#define VSA 4
#define VBP 30
#define VFP 20
#define HACT 480
#define HSA 4
#define HBP 30
#define HFP 18
#elif defined(E43RB_FW405_C)
#define RATE_415M
#define VACT 854
#define VSA 2
#define VBP 10
#define VFP 10
#define HACT 480
#define HSA 4
#define HBP 5
#define HFP 5
#elif defined(E43GB_MW405_C)
#define RATE_415M
#define VACT 854
#define VSA 2
#define VBP 10
#define VFP 10
#define HACT 480
#define HSA 4
#define HBP 5
#define HFP 5
#else
#error
#endif

#if defined(RATE_415M)
#define hsfreqrange 0x5
#define pll_soc_m_7_0 0x7
#define pll_soc_m_9_8 0x2
#define pll_soc_n 0x2
#define below_450Mbps 1
#define vco_cntrl 0x18
#elif defined(RATE_464M)
#define hsfreqrange 0x16
#define pll_soc_m_7_0 0x44
#define pll_soc_m_9_8 0x2
#define pll_soc_n 0x2
#define below_450Mbps 1
#define vco_cntrl 0x18
#else
#error
#endif

#endif /* __DISPLAY_H__ */
