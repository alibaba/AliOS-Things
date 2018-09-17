#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "qcli_api.h"

#define _IN_
#define _OU_

#define LOG_BUFFER_SIZE 256

extern QCLI_Group_Handle_t qcli_iotkit_handle;     /* Handle for IOT Command Group. */

int HAL_Vsnprintf(_IN_ char *str, _IN_ const int len, _IN_ const char *format, va_list ap)
{
    return vsnprintf(str, len, format, ap);
}

void HAL_Printf(_IN_ const char *fmt, ...)
{
    static char str_tmp[LOG_BUFFER_SIZE];
    va_list list;

    memset(str_tmp, 0, LOG_BUFFER_SIZE);

    va_start(list, fmt);
    HAL_Vsnprintf(str_tmp, LOG_BUFFER_SIZE - 1, fmt, list);
    va_end(list);
    QCLI_Printf(qcli_iotkit_handle, "%s", str_tmp);
}

int HAL_Snprintf(_IN_ char *str, const int len, const char *fmt, ...)
{
    va_list args;
    int     rc;

    va_start(args, fmt);
    rc = vsnprintf(str, len, fmt, args);
    va_end(args);

    return rc;
}

char *HAL_GetModuleID(char mid_str[])
{
    strcpy(mid_str, "xyz");
    return "xyz";
}


void HAL_Srandom(uint32_t seed)
{
    srand(seed);
}

uint32_t HAL_Random(uint32_t region)
{
    return (region > 0) ? (rand() % region) : 0;
}

int HAL_Atoi(const char *str)
{
    int res = 0, i;
    for (i = 0 ; str[i] <= '9' && str[i] >= '0'; ++i) {
        res = res * 10 + str[i] - '0';
    }
    return res;
}

