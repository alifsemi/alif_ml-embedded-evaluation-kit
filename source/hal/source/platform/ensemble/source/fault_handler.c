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

enum FaultType {
    FT_HardFault = 0,
    FT_MemManage = 1,
    FT_BusFault = 2,
    FT_UsageFault = 3
};

static const char *const FaultNames[] = {
    [FT_HardFault]  = "HardFault",
    [FT_MemManage]  = "MemManage",
    [FT_BusFault]   = "BusFault",
    [FT_UsageFault] = "UsageFault"
};

static char flag_names[] = "NZCVQIIT00B0GGGG";

static enum FaultType fault_type;
static uint32_t exc_return;
static uint32_t regs[17];
static void FaultDump(void);

static bool fault_dump_enabled;

/* External activation function serves to ensure this file is pulled in, and the weak default handlers are overridden */
void fault_dump_enable(bool enable)
{
    fault_dump_enabled = enable;
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
    printf("\n%s exception\n", FaultNames[fault_type]);
    printf("EXC_RETURN = %08X\n\n", exc_return);

    printf("Register dump (stored at &%08X) is:\n", (uint32_t) regs);
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



