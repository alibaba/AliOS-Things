#ifndef __APP_HAL_H__
#define __APP_HAL_H__

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#if BES_HAL_DEBUG
#define ENTER_FUNCTION() printf("%s enter ->\n", __FUNCTION__)
#define LEAVE_FUNCTION() printf("%s <- leave\n", __FUNCTION__)
#define FOOTPRINT() printf("%s:%d\n", __FUNCTION__, __LINE__)
#define TRACEME(str, ...)  printf("%s:%d "str, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define ENTER_FUNCTION()
#define LEAVE_FUNCTION()
#define FOOTPRINT()
#define TRACEME(str, ...)
#endif
#define FAIL_FUNCTION() printf("%s:%d fail!\n", __FUNCTION__, __LINE__)

#endif // __APP_HAL_H__

