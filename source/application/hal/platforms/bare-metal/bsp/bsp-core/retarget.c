/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
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
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)

#include "uart_stdout.h"
#include "bsp_core_log.h"

#if defined (MPS3_PLATFORM)
#include "smm_mps3.h"
#endif  /* MPS3_PLATFORM */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <rt_misc.h>
#include <rt_sys.h>


/* Standard IO device handles. */
#define STDIN   0x8001
#define STDOUT  0x8002
#define STDERR  0x8003

/* Standard IO device name defines. */
const char __stdin_name[]   = "STDIN";
const char __stdout_name[]  = "STDOUT";
const char __stderr_name[]  = "STDERR";

int fputc(int ch, FILE *f)
{
    UNUSED(f);
    return (UartPutc(ch));
}

int fgetc(FILE *f)
{
    UNUSED(f);
    return (UartPutc(UartGetc()));
}

int ferror(FILE *f)
{
    UNUSED(f);
    /* Your implementation of ferror */
    return EOF;
}

void _ttywrch(int ch)
{
    UartPutc(ch);
}

FILEHANDLE _sys_open(const char *name, int openmode)
{
    UNUSED(openmode);

    /* Register standard Input Output devices. */
    if (strcmp(name, "STDIN") == 0)
    {
        return (STDIN);
    }
    if (strcmp(name, "STDOUT") == 0)
    {
        return (STDOUT);
    }
    if (strcmp(name, "STDERR") == 0)
    {
        return (STDERR);
    }
    return (-1);
}

int _sys_close(FILEHANDLE fh)
{
    if (fh > 0x8000)
    {
        return (0);
    }
    return (-1);
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned int len, int mode)
{
    UNUSED(mode);
    if (fh == STDOUT || fh == STDERR )
    {
        /* Standard Output device. */
        for (; len; len--)
        {
            UartPutc(*buf++);
        }
        return (0);
    }

    if (fh > 0x8000)
    {
        return (-1);
    }
    return (-1);
}

int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned int len, int mode)
{
    UNUSED(mode);
    if (fh == STDIN)
    {
        /* Standard Input device. */
        for (; len; len--)
        {
            *buf++ = UartGetc();
        }
        return (0);
    }

    if (fh > 0x8000)
    {
        return (-1);
    }
    return (-1);
}

int _sys_istty(FILEHANDLE fh)
{
    if (fh > 0x8000)
    {
        return (1);
    }
    return (0);
}

int _sys_seek(FILEHANDLE fh, long pos)
{
    UNUSED(pos);
    if (fh > 0x8000)
    {
        return (-1);
    }
    return (-1);
}

int _sys_ensure(FILEHANDLE fh)
{
    if (fh > 0x8000)
    {
        return (-1);
    }
    return (-1);
}

long _sys_flen(FILEHANDLE fh)
{
    if (fh > 0x8000)
    {
        return (0);
    }
    return (-1);
}

int _sys_tmpnam(char *name, int sig, unsigned maxlen)
{
    UNUSED(name);
    UNUSED(sig);
    UNUSED(maxlen);
    return (1);
}

char *_sys_command_string(char *cmd, int len)
{
    UNUSED(len);
    return (cmd);
}

void _sys_exit(int return_code)
{
    UartEndSimulation(return_code);
}

int system(const char *cmd)
{
    UNUSED(cmd);
    return (0);
}

time_t time(time_t *timer)
{
    time_t current;

#if defined (MPS3_PLATFORM)
    current = MPS3_FPGAIO->COUNTER;
#else   /* MPS3_PLATFORM */
    current  = 0;   /* No RTC implementation available. */
#endif  /* MPS3_PLATFORM */

    if (timer != NULL) {
        *timer = current;
    }

    return (current);
}

#else   /* #if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */

/******************************************************************************/
/* Retarget functions for GNU Tools for ARM Embedded Processors               */
/******************************************************************************/
#include <stdio.h>
#include <sys/stat.h>

extern unsigned char UartPutc(unsigned char my_ch);

__attribute__((used)) int _write(int fd, char *ptr, int len)
{
    size_t i;
    for (i = 0; i < len; i++)
    {
        UartPutc(ptr[i]); /* call character output function. */
    }
    return len;
}

#endif /* #if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) */
