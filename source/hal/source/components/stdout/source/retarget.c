/*
 * SPDX-FileCopyrightText: Copyright 2022, 2025 Arm Limited and/or its
 * affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#if defined(USE_SEMIHOSTING)
#error "This file should not be used if semihosting is disabled."
#endif /* defined(USE_SEMIHOSTING) */

#include "uart_stdout.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100)
/* Arm compiler re-targeting */

#include <rt_misc.h>
#include <rt_sys.h>

/* Standard IO device handles. */
#define STDIN  0x8001
#define STDOUT 0x8002
#define STDERR 0x8003

#define RETARGET(fun) _sys##fun

#if __ARMCLIB_VERSION >= 6190004
#define TMPNAM_FUNCTION RETARGET(_tmpnam2)
#else
#define TMPNAM_FUNCTION RETARGET(_tmpnam)
#endif

#elif defined(__clang__)

/* Clang/LLVM toolchain support */
#define RETARGET(fun) fun

#elif defined (__GNUC__)
/* GNU compiler re-targeting */

/*
 * This type is used by the _ I/O functions to denote an open
 * file.
 */
typedef int FILEHANDLE;

/*
 * Open a file. May return -1 if the file failed to open.
 */
extern FILEHANDLE _open(const char* /*name*/, int /*openmode*/);

/* Standard IO device handles. */
#define STDIN  0x00
#define STDOUT 0x01
#define STDERR 0x02

#define RETARGET(fun) fun

#define TMPNAM_FUNCTION RETARGET(_tmpnam)

#endif /* __GNUC__ */

/*
 ********************************************************************
 *             Common functions for all toolchains                  *
 ********************************************************************
 */
static void UartEndSimulation(int code)
{
    UartPutc((char)0x4);  // End of simulation
    UartPutc((char)code); // Exit code
}

void __attribute__((noreturn)) RETARGET(_exit)(int return_code)
{
    UartEndSimulation(return_code);
    while (1) {}
}

void __attribute__((constructor(101))) _retarget_init(void)
{
    UartStdOutInit();
}

/*
 ********************************************************************
 *        Functions for Arm Compiler and GNU toolchains             *
 ********************************************************************
 */
#if (defined(__GNUC__) && !defined(__clang__)) || defined(__ARMCC_VERSION)
/* Standard IO device name defines. */
const char __stdin_name[] __attribute__((aligned(4)))  = "STDIN";
const char __stdout_name[] __attribute__((aligned(4))) = "STDOUT";
const char __stderr_name[] __attribute__((aligned(4))) = "STDERR";

void _ttywrch(int ch)
{
    (void)fputc(ch, stdout);
}

FILEHANDLE RETARGET(_open)(const char* name, int openmode)
{
    (void)(openmode);

    if (strcmp(name, __stdin_name) == 0) {
        return (STDIN);
    }

    if (strcmp(name, __stdout_name) == 0) {
        return (STDOUT);
    }

    if (strcmp(name, __stderr_name) == 0) {
        return (STDERR);
    }

    return -1;
}

int RETARGET(_write)(FILEHANDLE fh, const unsigned char* buf, unsigned int len, int mode)
{
    (void)(mode);

    switch (fh) {
    case STDOUT:
    case STDERR: {
        int c;

        while (len-- > 0) {
            c = fputc(*buf++, stdout);
            if (c == EOF) {
                return EOF;
            }
        }

        return 0;
    }
    default:
        return EOF;
    }
}

int RETARGET(_read)(FILEHANDLE fh, unsigned char* buf, unsigned int len, int mode)
{
    (void)(mode);

    switch (fh) {
    case STDIN: {
        int c;

        while (len-- > 0) {
            c = fgetc(stdin);
            if (c == EOF) {
                return EOF;
            }

            *buf++ = (unsigned char)c;
        }

        return 0;
    }
    default:
        return EOF;
    }
}

int RETARGET(_istty)(FILEHANDLE fh)
{
    switch (fh) {
    case STDIN:
    case STDOUT:
    case STDERR:
        return 1;
    default:
        return 0;
    }
}

int RETARGET(_close)(FILEHANDLE fh)
{
    if (RETARGET(_istty(fh))) {
        return 0;
    }

    return -1;
}

int RETARGET(_seek)(FILEHANDLE fh, long pos)
{
    (void)(fh);
    (void)(pos);

    return -1;
}

int RETARGET(_ensure)(FILEHANDLE fh)
{
    (void)(fh);

    return -1;
}

long RETARGET(_flen)(FILEHANDLE fh)
{
    if (RETARGET(_istty)(fh)) {
        return 0;
    }

    return -1;
}

int TMPNAM_FUNCTION(char* name, int sig, unsigned int maxlen)
{
    (void)(name);
    (void)(sig);
    (void)(maxlen);

    return 1;
}

char* RETARGET(_command_string)(char* cmd, int len)
{
    (void)(len);

    return cmd;
}


int system(const char* cmd)
{
    (void)(cmd);

    return 0;
}

time_t time(time_t* timer)
{
    time_t current;

    current = 0; // To Do !! No RTC implemented

    if (timer != NULL) {
        *timer = current;
    }

    return current;
}

void _clock_init(void) {}

clock_t clock(void)
{
    return (clock_t)-1;
}

int remove(const char* arg)
{
    (void)(arg);

    return 0;
}

int rename(const char* oldn, const char* newn)
{
    (void)(oldn);
    (void)(newn);

    return 0;
}

int fputc(int ch, FILE* f)
{
    (void)(f);

    return UartPutc(ch);
}

int fgetc(FILE* f)
{
    (void)(f);

    return UartPutc(UartGetc());
}

#endif /* defined(__GNUC__) || defined(__ARMCC_VERSION) */

#ifndef ferror

/* arm-none-eabi-gcc with newlib uses a define for ferror */
int ferror(FILE* f)
{
    (void)(f);

    return EOF;
}

#endif /* #ifndef ferror */

/*
 ********************************************************************
 *        Functions for GNU toolchains                              *
 ********************************************************************
 */
/* If using GNU compiler */
#if defined(__GNUC__)

/* If Arm GNU compiler version > 11.3.0 */
#if __GNUC__ > 11 || \
    (__GNUC__ == 11 && (__GNUC_MINOR__ > 3 || (__GNUC_MINOR__ == 3 && __GNUC_PATCHLEVEL__ > 0)))
struct stat;
int _fstat_r(struct _reent* r, int fdes, struct stat* s)
{
    (void)(r);
    (void)(fdes);
    (void)(s);
    return -1;
}

int _getpid_r(struct _reent* r)
{
    (void)(r);
    return -1;
}

int _isatty_r(struct _reent* r, int desc)
{
    (void)(r);
    (void)(desc);
    return -1;
}

int _kill_r(struct _reent* r, int pid, int signal)
{
    (void)(r);
    (void)(pid);
    (void)(signal);
    return -1;
}

_off_t _lseek_r(struct _reent* r, int fdes, _off_t offset, int w)
{
    (void)(r);
    (void)(fdes);
    (void)(offset);
    (void)(w);
    return -1;
}

#endif /* GNU  toolchain version > 11.3.0 */
#endif /* If using GNU toolchain */

/*
 ********************************************************************
 *        Functions for Clang toolchain                             *
 ********************************************************************
 */
#if defined (__clang__) && !defined(__ARMCC_VERSION)

/** Write a character over UART */
static int llvm_picolib_putc(char c, FILE *file)
{
    (void) file;
    UartPutc(c);
    return c;
}

/* Read a character over UART */
static int llvm_picolib_getc(FILE *file)
{
    unsigned char c = 0;
    (void) file;
    c = UartGetc();
    return c;
}

/* Initialise the stream for picolib */
static FILE __stdio = FDEV_SETUP_STREAM(
                       llvm_picolib_putc,
                       llvm_picolib_getc,
                       NULL,
                       _FDEV_SETUP_RW);

/* Wire the stdout and stderr streams to the same file */
FILE *const stdin = &__stdio;
__strong_reference(stdin, stdout);
__strong_reference(stdin, stderr);

#endif /* defined(__clang__) */
