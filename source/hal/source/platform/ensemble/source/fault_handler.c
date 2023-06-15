/* Copyright (C) 2023 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "RTE_Device.h"
#include "RTE_Components.h"

#include "fault_handler.h"

#define MMFSR (((volatile uint8_t *) &SCB->CFSR)[0])
#define BFSR (((volatile uint8_t *) &SCB->CFSR)[1])
#define UFSR (((volatile uint16_t *) &SCB->CFSR)[1])

enum FaultType {
    FT_HardFault = 0,
    FT_MemManage = 1,
    FT_BusFault = 2,
    FT_UsageFault = 3,
    FT_SecureFault = 4,
    FT_DebugMonitor = 5
};

static const char *const FaultNames[] = {
    [FT_HardFault]  = "HardFault",
    [FT_MemManage]  = "MemManage",
    [FT_BusFault]   = "BusFault",
    [FT_UsageFault] = "UsageFault",
    [FT_SecureFault] = "SecureFault",
    [FT_DebugMonitor] = "DebugMonitor"
};

static char flag_names[] = "NZCVQIIT00B0GGGG";

static enum FaultType fault_type;
static uint32_t exc_return;
static uint32_t regs[17];
static void FaultDump(void);

static bool fault_dump_enabled;

/* Compressed encoding of bits - bit number, then name, ending with space (32) */
/* Octal bit number encoding, because C - it's either that or hex */
static const char mmfsr_bits[] = {
    "\007MMARVALID"
    "\005MLSPERR"
    "\004MSTKERR"
    "\003MUNSTKERR"
    "\001DACCVIOL"
    "\000IACCVIOL"
    " "
};

static const char bfsr_bits[] = {
    "\007BFARVALID"
    "\005LSPERR"
    "\004STKERR"
    "\003UNSTKERR"
    "\002IMPRECISERR"
    "\001PRECISERR"
    "\000IBUSERR"
    " "
};

static const char ufsr_bits[] = {
    "\011DIVBYZERO"
    "\010UNALIGNED"
    "\004STKOF"
    "\003NOCP"
    "\002INVPC"
    "\001INVSTATE"
    "\000UNDEFINSTR"
    " "
};

static const char hfsr_bits[] = {
    "\037DEBUGEVT"
    "\036FORCED"
    "\001VECTBL"
    " "
};

static const char sfsr_bits[] = {
    "\007LSERR"
    "\006SFARVALID"
    "\005LSPERR"
    "\004INVTRAN"
    "\003AUVIOL"
    "\002INVER"
    "\001INVIS"
    "\000INVEP"
    " "
};

static const char dfsr_bits[] = {
    "\005PMU"
    "\004EXTERNAL"
    "\003VCATCH"
    "\002DWTTRAP"
    "\001BKPT"
    "\00HALTED"
    " "
};

#if __CORTEX_M == 55
static const char afsr_bits[] = {
    "\036FPOISON"
    "\035FTGU"
    "\034FECC"
    "\030FMAXI"
    "\033FMAXITYPE=DECERR"
    "\026FDTCM"
    "\025FITCM"
    "\023PPOISON"
    "\022PTGU"
    "\021PECC"
    "\017PIPPB"
    "\016PEPPB"
    "\015PMAXI"
    "\020PMAXITYPE=DECERR"
    "\014PPAHB"
    "\013PDTCM"
    "\012PITCM"
    "\011IPOISON"
    "\007IECC"
    "\004IEPPB"
    "\003IMAXI"
    "\006IMAXITYPE=DECERR"
    "\002IPAHB"
    "\001IDTCM"
    "\000IITCM"
    " "
};
#endif

static void print_fsrbits(uint32_t val, const char *bitnames)
{
     bool first = true;

     while (val != 0 && *bitnames != ' ') {
         int bit = *bitnames++;
         const char *next_bitname = bitnames;
         while (*next_bitname > ' ') {
             next_bitname++;
         }
         if (val & (1U << bit)) {
             val &=~ (1U << bit);
             printf("%s%.*s", first ? " (" : ", ", next_bitname - bitnames, bitnames);
             first = false;
         }
         bitnames = next_bitname;
     } ;

     puts(first ? "" : ")");
}

static void print_memmanage(void)
{
     uint32_t mmfsr = MMFSR;
     if (mmfsr == 0) {
         return;
     }
     printf("MMFSR = %02X", mmfsr);
     print_fsrbits(mmfsr, mmfsr_bits);
     if (mmfsr & 0x80) {
        printf("MMFAR = %08X\n", SCB->MMFAR);
     }
     MMFSR = mmfsr;
}

static void print_busfault(void)
{
     uint32_t bfsr = BFSR;
     if (bfsr == 0) {
         return;
     }
     printf("BFSR  = %02X", bfsr);
     print_fsrbits(bfsr, bfsr_bits);
     if (bfsr & 0x80) {
        printf("BFAR  = %08X\n", SCB->BFAR);
     }
#if __CORTEX_M == 55
     uint32_t afsr = SCB->AFSR;
     printf("AFSR  = %08X", afsr);

     print_fsrbits(afsr, afsr_bits);
     SCB->AFSR = afsr;
#endif
     BFSR = bfsr;
}

static void print_usagefault(void)
{
     uint32_t ufsr = UFSR;
     if (ufsr == 0) {
         return;
     }
     printf("UFSR  = %04X", ufsr);
     print_fsrbits(ufsr, ufsr_bits);
     UFSR = ufsr;
}

static void print_hardfault(void)
{
     uint32_t hfsr = SCB->HFSR;
     if (hfsr == 0) {
         return;
     }
     printf("HFSR  = %08X", hfsr);
     print_fsrbits(hfsr, hfsr_bits);
     SCB->HFSR = hfsr;
}

static void print_securefault(void)
{
     uint32_t sfsr = SCB->SFSR;
     if (sfsr == 0) {
         return;
     }
     printf("SFSR  = %08X", sfsr);
     print_fsrbits(sfsr, sfsr_bits);
     SCB->SFSR = sfsr;
}

static void print_debugfault(void)
{
     uint32_t dfsr = SCB->DFSR;
     if (dfsr == 0) {
         return;
     }
     printf("DFSR  = %08X", dfsr);
     print_fsrbits(dfsr, dfsr_bits);
     SCB->DFSR = dfsr;
}

static void print_faults(void)
{
    print_usagefault();
    print_memmanage();
    print_busfault();
    print_securefault();
    print_debugfault();
    print_hardfault();
}

/* External activation function serves to ensure this file is pulled in, and the weak default handlers are overridden */
void fault_dump_enable(bool enable)
{
    fault_dump_enabled = enable;

    /* Show any pending faults (shouldn't be any) and clear registers */
    print_faults();

    IPSR_Type ipsr;
    ipsr.w = __get_IPSR();
    if (ipsr.b.ISR != 0) {
        printf("!!! In exception %d !!!\n", ipsr.b.ISR);
    }
}

__attribute__((naked))
void HardFault_Handler(void)
{
    __asm("MOVS R0, #0; B CommonAsmFaultHandler");
}

__attribute__((naked))
void MemManage_Handler(void)
{
    __asm("MOVS R0, #1; B CommonAsmFaultHandler");
}

__attribute__((naked))
void BusFault_Handler(void)
{
    __asm("MOVS R0, #2; B CommonAsmFaultHandler");
}

__attribute__((naked))
void UsageFault_Handler(void)
{
    __asm("MOVS R0, #3; B CommonAsmFaultHandler");
}

__attribute__((naked))
void SecureFault_Handler(void)
{
    __asm("MOVS R0, #4; B CommonAsmFaultHandler");
}

__attribute__((naked))
void DebugMonitor_Handler(void)
{
    __asm("MOVS R0, #5; B CommonAsmFaultHandler");
}

__attribute__((used))
__attribute__((naked))
static void CommonAsmFaultHandler(int type)
{
    __asm("LDR   R1, =regs+4*4\n\t"
          "STM   R1, {R4-R11}\n\t"
          "TST   LR, #1<<2\n\t"
          "ITE   EQ\n\t"
          "MOVEQ R1, SP\n\t"
          "MRSNE R1, PSP\n\t"
          "MOV   R2, LR\n\t"
          "B     CommonFaultHandler");
}

__attribute__((used))
static void CommonFaultHandler(enum FaultType type, uint32_t * restrict sp, uint32_t lr)
{
    if (!fault_dump_enabled) {
        while (1) {
            __WFE();
        }
    }

    exc_return = lr;
    fault_type = type;
    uint32_t retpsr = sp[7];

    // Assembler fills in R4-R11, we do the rest
    regs[0] = sp[0];
    regs[1] = sp[1];
    regs[2] = sp[2];
    regs[3] = sp[3];
    regs[12] = sp[4];
    // For SP, we need to see if we pushed an integer-only or FP context, and also check for doubleword alignment adjustment
    regs[13] = (uint32_t) sp + (exc_return & 0x10 ? 0x20 : 0x68) + (retpsr & 0x200 ? 4 : 0);
    regs[14] = sp[5];
    regs[15] = sp[6];
    regs[16] = sp[7];

    // Modify the fault frame so we "return" to the FaultDump routine, which hopefully will execute okay
    sp[6] = (uint32_t) FaultDump;
}

static void FaultDump(void)
{
    printf("\n==== %s exception ====\n\n", FaultNames[fault_type]);

    print_faults();

    printf("\nEXC_RETURN = %08X\n\n"
           "Register dump (stored at &%08X) is:\n",  exc_return, (uint32_t) regs);
    for (int i = 0; i < 13; i++) {
        printf("R%-3d= %08X%c", i, regs[i], i % 4 < 3 ? ' ' : '\n');
    }
    printf("SP  = %08X LR  = %08X PC  = %08X\n", regs[13], regs[14], regs[15]);
    printf("Mode %-8sflags set: ", exc_return & 8 ? "Thread" : "Handler");
    for (size_t i = 0, bit = 1u<<31; i < 24; i++) {
        if (i < sizeof flag_names) {
            if (flag_names[i] != '0') {
                putchar(regs[16] & bit ? flag_names[i] : flag_names[i] + 'a' - 'A');
            }
            bit >>= 1;
        } else {
            putchar(' ');
        }
    }
    printf("PSR = %08X\n", regs[16]);
    if (!(exc_return & 8)) {
        printf("Exception %d\n", regs[16] & 0x1FF);
    }

    for (;;) {
        __WFE();
    }
}



