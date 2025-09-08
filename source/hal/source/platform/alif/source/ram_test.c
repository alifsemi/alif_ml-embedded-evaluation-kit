/* Copyright (C) 2024 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <RTE_Device.h>
#include <RTE_Components.h>
#include CMSIS_device_header

#include "log_macros.h"

#include "ram_test.h"

// Address bit test - primarily to check the address shim for parts that need it
void ram_address_test(volatile uint8_t *ram, uint32_t size)
{
    ram[0] = 'K';
    __DSB();
    ram[size - 1] = 'B';
    __DSB();
    for (unsigned bit = 0; (UINT32_C(1) << bit) < size; bit++) {
        ram[UINT32_C(1) << bit] = bit;
        __DSB();
    }
    unsigned errors = 0, good = 0;
    if (ram[0] != 'K')
    {
        printf("RAM start failure\n");
        errors++;
    }
    if (ram[size - 1] != 'B')
    {
        printf("RAM end failure\n");
        errors++;
    }
    for (unsigned bit = 0; (UINT32_C(1) << bit) < size; bit++) {
        if (ram[UINT32_C(1) << bit] != bit) {
            printf("RAM address bit %u failure\n", bit);
            errors++;
        } else {
            good++;
        }
    }
    if (errors == 0) {
        printf("%u address bits all good\n", good);
    }
}

void ram_linear_test(uint8_t *ram)
{
#if 1
#define BIG_TEST_SIZE_M 1
    srand(1);
    printf("Writing %dM\n", BIG_TEST_SIZE_M);
    for (int i = 0; i < BIG_TEST_SIZE_M*1024*1024; i++) {
        ram[i] = rand();
    }
    printf("Reading %dM\n", BIG_TEST_SIZE_M);
    srand(1);
    int err_cnt = 0;
    for (int i = 0; i < BIG_TEST_SIZE_M*1024*1024; i++) {
        uint8_t expected = rand();
        if (ram[i] != expected) {
            ++err_cnt;
            if (err_cnt < 20) {
                printf_err("Mismatch at %p: Got %02X expected %02X\n", (void*) &ram[i], ram[i], expected);
            } else if (err_cnt == 20) {
                printf_err("...\n");
            }
        }
    }
    printf("Finished linear test (%d matches, %d mismatches)\n", BIG_TEST_SIZE_M*1024*1024 - err_cnt, err_cnt);
#endif
}

void ram_random_test(uint8_t *test_ram)
{
#define TEST_SIZE 0x0020000
#define TEST_ITERS (1024*1024)
    static uint8_t check_ram[TEST_SIZE] __attribute__((section(".bss.large_ram"))) __attribute__((aligned(16)));

    printf("Init test (%d iterations in %d bytes)\n", TEST_ITERS, TEST_SIZE);
    memset((uint8_t *)test_ram, 0xAA, TEST_SIZE);
    memset(check_ram, 0xAA, TEST_SIZE);
    printf("Start\n");
    for (int i = 0; i < TEST_ITERS; i++) {
        size_t pos = rand() & (TEST_SIZE-1);
        uint32_t val = rand();

        switch (rand() & 7) {
        case 0:
            check_ram[pos] = test_ram[pos] = val;
            break;
        case 1:
            if (pos + 2 > TEST_SIZE) break;
            __UNALIGNED_UINT16_WRITE(check_ram+pos, val);
            __UNALIGNED_UINT16_WRITE(test_ram+pos, val);
            break;
        case 2:
            if (pos + 4 > TEST_SIZE) break;
            __UNALIGNED_UINT32_WRITE(check_ram+pos, val);
            __UNALIGNED_UINT32_WRITE(test_ram+pos, val);
            break;
        case 3:
            if (pos + 11 > TEST_SIZE) break;
            memset(check_ram+pos, val, 11);
            memset((uint8_t *)test_ram+pos, val, 11);
            break;
        case 4:
            if (pos + 3 > TEST_SIZE) break;
            check_ram[pos] = check_ram[pos + 2] = val;
            test_ram[pos] = test_ram[pos + 2] = val;
            break;
        case 5:
            if (pos + 6 > TEST_SIZE) break;
            check_ram[pos] = check_ram[pos + 5] = val;
            test_ram[pos] = val; test_ram[pos + 5] = val;
            break;
        case 6:
#if 1
            if (pos + 7 > TEST_SIZE) break;
            check_ram[pos] = test_ram[pos] = val;
            check_ram[pos + 3] = test_ram[pos + 3] = val;
            check_ram[pos + 6] = test_ram[pos + 6] = val;
#else
            pos = (pos &~ 0x1F);
#if 1
            if (pos + 0x1C > TEST_SIZE) break;
            *(uint32_t *)(test_ram+pos + 0xC) = val;
            *(uint16_t *)(test_ram+pos + 0x10) = val;
            *(uint8_t *)(test_ram+pos + 0x12) = val;
            *(uint32_t *)(test_ram+pos + 0x14) = val;
            *(uint32_t *)(test_ram+pos + 0x18) = val;
            *(uint32_t *)(check_ram+pos + 0xC) = val;
            *(uint16_t *)(check_ram+pos + 0x10) = val;
            *(uint8_t *)(check_ram+pos + 0x12) = val;
            *(uint32_t *)(check_ram+pos + 0x14) = val;
            *(uint32_t *)(check_ram+pos + 0x18) = val;
#else
            if (pos + 0xB > TEST_SIZE) break;
            *(uint32_t *)(check_ram+pos + 0x4) = val;
            *(uint16_t *)(check_ram+pos + 0x8) = val;
            *(uint8_t *)(check_ram+pos + 0xA) = val;
            *(uint32_t *)(test_ram+pos + 0x4) = val;
            *(uint16_t *)(test_ram+pos + 0x8) = val;
            *(uint8_t *)(test_ram+pos + 0xA) = val;
#endif
#endif
            break;
        case 7:
            if (!(rand() & 0xF) && pos + 1024 <= TEST_SIZE) {
                memset(check_ram+pos, val, 1024);
                memset((uint8_t *)test_ram+pos, val, 1024);
            } else {
                if (pos + 17 > TEST_SIZE) break;
                check_ram[pos] = test_ram[pos] = test_ram[pos + 16];
            }
            break;
        }
    }

   // test_ram[2000] = 3;

    int err_cnt = 0;
    for (int i = 0; i < TEST_SIZE; i++) {
        if (test_ram[i] != check_ram[i]) {
            ++err_cnt;
            if (err_cnt < 20) {
                printf_err("Mismatch at %p: Got %02X expected %02X\n", (void*) &test_ram[i], test_ram[i], check_ram[i]);
            } else if (err_cnt == 20) {
                printf_err("...\n");
            }
        }
    }
    printf("Test complete (%d matches, %d mismatches)\n", TEST_SIZE - err_cnt, err_cnt);
}
