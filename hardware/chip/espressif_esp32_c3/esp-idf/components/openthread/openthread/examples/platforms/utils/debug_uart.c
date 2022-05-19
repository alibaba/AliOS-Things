/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */
#include <openthread-core-config.h>
#include <stdarg.h>
#include <stdio.h>
#include <openthread/config.h>

#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/debug_uart.h>
#include <openthread/platform/toolchain.h>

/*
 * Implementation note:
 *   These are all "weak" so that a platform may if it chooses  override the instance.
 */

OT_TOOL_WEAK
void otPlatDebugUart_printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    otPlatDebugUart_vprintf(fmt, ap);
    va_end(ap);
}

OT_TOOL_WEAK
void otPlatDebugUart_vprintf(const char *fmt, va_list ap)
{
    char buf[128];
    /* by standard ...
     * vsnprintf() always null terminates
     */
    vsnprintf(buf, sizeof(buf), fmt, ap);
    /* however ... some platforms have bugs */
    buf[sizeof(buf) - 1] = 0;
    otPlatDebugUart_puts_no_nl(buf);
}

OT_TOOL_WEAK
void otPlatDebugUart_write_bytes(const uint8_t *pBytes, int nBytes)
{
    while (nBytes > 0)
    {
        otPlatDebugUart_putchar((int)(*pBytes));
        pBytes++;
        nBytes--;
    }
}

OT_TOOL_WEAK
void otPlatDebugUart_puts_no_nl(const char *s)
{
    while (*s)
    {
        otPlatDebugUart_putchar(*s);
        s++;
    }
}

OT_TOOL_WEAK
void otPlatDebugUart_puts(const char *s)
{
    otPlatDebugUart_puts_no_nl(s);
    otPlatDebugUart_putchar('\n');
}

OT_TOOL_WEAK
void otPlatDebugUart_putchar(int c)
{
    /* map lf to crlf as needed */
    if (c == '\n')
    {
        otPlatDebugUart_putchar_raw('\r');
    }

    otPlatDebugUart_putchar_raw(c);
}

/* provide WEAK stubs for platforms that do not implement all functions */
OT_TOOL_WEAK
void otPlatDebugUart_putchar_raw(int c)
{
    OT_UNUSED_VARIABLE(c);
}

OT_TOOL_WEAK
int otPlatDebugUart_kbhit(void)
{
    return 0; /* nothing */
}

OT_TOOL_WEAK
int otPlatDebugUart_getc(void)
{
    return -1; /* nothing */
}

OT_TOOL_WEAK
otError otPlatDebugUart_logfile(const char *filename)
{
    OT_UNUSED_VARIABLE(filename);

    return OT_ERROR_FAILED;
}

#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_DEBUG_UART)
/* this should not be a WEAK function */
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);

    va_list  ap;
    uint32_t now;

    now = otPlatAlarmMilliGetNow();
    otPlatDebugUart_printf("%3d.%03d | ", (int)(now / 1000), (int)(now % 1000));
    va_start(ap, aFormat);
    otPlatDebugUart_vprintf(aFormat, ap);
    va_end(ap);

    otPlatDebugUart_putchar('\n');
}
#endif
