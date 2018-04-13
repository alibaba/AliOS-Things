#ifndef DM_IMPORT_H
#define DM_IMPORT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DM_MODULE_NAME "dm"

#ifdef ESP8266
#include "esp_common.h"

#define dm_printf      os_printf
#define dm_sprintf     os_sprintf
#define dm_snprintf    os_snprintf
#define __DM_READ_ONLY__ ICACHE_RODATA_ATTR STORE_ATTR
#else
#define dm_printf      printf
#define dm_sprintf     sprintf
#define dm_snprintf    snprintf
#define __DM_READ_ONLY__
#endif

void* dm_lite_malloc(size_t size);
void* dm_lite_calloc(size_t nmemb, size_t size);
void  dm_lite_free_func(void* ptr);
void  dm_lite_free(void* ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DM_IMPORT_H */
