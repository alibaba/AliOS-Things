#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "iot_import.h"
#include "aos/kernel.h"

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format,
                  va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

void HAL_Printf(_IN_ const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    fflush(stdout);
}

void HAL_Srandom(uint32_t seed)
{
    srand(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    return (region > 0) ? (rand() % region) : 0;
}

void *HAL_Malloc(_IN_ uint32_t size)
{
    return aos_malloc(size);
}

void *HAL_Realloc(_IN_ void *ptr, _IN_ uint32_t size)
{
    return aos_realloc(ptr, size);
}

void HAL_Free(_IN_ void *ptr)
{
    aos_free(ptr);
}

uint64_t HAL_UptimeMs(void)
{
    return aos_now_ms();
}

void HAL_SleepMs(_IN_ uint32_t ms)
{
    aos_msleep(ms);
}

