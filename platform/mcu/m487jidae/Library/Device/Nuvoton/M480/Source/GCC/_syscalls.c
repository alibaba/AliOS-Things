/**************************************************************************//**
 * @file     _syscalls.c
 * @version  V1.00
 * @brief    system call for semihost.
 *
 *
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

// ----------------------------------------------------------------------------

/* Support files for GNU libc.  Files in the system namespace go here.
 Files in the C namespace (ie those that do not start with an
 underscore) go in .c.  */

#include <_ansi.h>
#include <stdint.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>

#include "semihosting.h"

/* Struct used to keep track of the file position, just so we
 can implement fseek(fh,x,SEEK_CUR).  */
struct fdent
{
    int handle;
    int pos;
};


#define MAX_OPEN_FILES 20

static struct fdent openfiles[MAX_OPEN_FILES];

static int monitor_stdin;
static int monitor_stdout;
static int monitor_stderr;

void
initialise_monitor_handles (void)
{
    int i;

    /* Open the standard file descriptors by opening the special
     * teletype device, ":tt", read-only to obtain a descriptor for
     * standard input and write-only to obtain a descriptor for standard
     * output. Finally, open ":tt" in append mode to obtain a descriptor
     * for standard error. Since this is a write mode, most kernels will
     * probably return the same value as for standard output, but the
     * kernel can differentiate the two using the mode flag and return a
     * different descriptor for standard error.
     */

    int volatile block[3];

    block[0] = (int) ":tt";
    block[2] = 3; /* length of filename */
    block[1] = 0; /* mode "r" */
    monitor_stdin = call_host (SEMIHOSTING_SYS_OPEN, (void*) block);

    block[0] = (int) ":tt";
    block[2] = 3; /* length of filename */
    block[1] = 4; /* mode "w" */
    monitor_stdout = call_host (SEMIHOSTING_SYS_OPEN, (void*) block);

    block[0] = (int) ":tt";
    block[2] = 3; /* length of filename */
    block[1] = 8; /* mode "a" */
    monitor_stderr = call_host (SEMIHOSTING_SYS_OPEN, (void*) block);

    /* If we failed to open stderr, redirect to stdout. */
    if (monitor_stderr == -1)
    {
        monitor_stderr = monitor_stdout;
    }

    for (i = 0; i < MAX_OPEN_FILES; i++)
    {
        openfiles[i].handle = -1;
    }

    openfiles[0].handle = monitor_stdin;
    openfiles[0].pos = 0;
    openfiles[1].handle = monitor_stdout;
    openfiles[1].pos = 0;
    openfiles[2].handle = monitor_stderr;
    openfiles[2].pos = 0;
}


