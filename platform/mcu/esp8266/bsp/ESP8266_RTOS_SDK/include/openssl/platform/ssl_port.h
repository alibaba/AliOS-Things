// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SSL_PORT_H_
#define _SSL_PORT_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "c_types.h"
#include "esp_system.h"
#include "string.h"

#ifdef MEMLEAK_DEBUG

extern void *pvPortMalloc( size_t xWantedSize, const char * file, unsigned line, bool use_iram);
extern void *pvPortZalloc( size_t xWantedSize, const char * file, unsigned line);
extern void vPortFree(void *pv, const char * file, unsigned line);

#define ssl_mem_malloc(s)    \
    ({  \
        static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;    \
        pvPortMalloc(s, mem_debug_file, __LINE__, false);  \
    })

#define ssl_mem_zalloc(s)    \
    ({  \
        static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;    \
        pvPortZalloc(s, mem_debug_file, __LINE__);  \
    })

#define ssl_mem_free(s) \
do{\
    static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;    \
    vPortFree(s, mem_debug_file, __LINE__);\
}while(0)


#else

extern void *pvPortMalloc( size_t xWantedSize );
extern void *pvPortZalloc( size_t xWantedSize );
extern void vPortFree(void *pv);

#define ssl_mem_zalloc(s) pvPortZalloc(s)
#define ssl_mem_malloc(s) pvPortMalloc(s)
#define ssl_mem_free(p) vPortFree(p)

#endif

#define ssl_memcpy memcpy
#define ssl_strlen strlen

#define ssl_speed_up_enter() system_update_cpu_freq(SYS_CPU_160MHZ)
#define ssl_speed_up_exit()  system_update_cpu_freq(SYS_CPU_80MHZ)

#ifndef os_printf
#define os_printf(fmt, ...) do {    \
        static const char flash_str[] ICACHE_RODATA_ATTR STORE_ATTR = fmt;  \
        printf(flash_str, ##__VA_ARGS__);   \
    } while(0)
#endif

#define SSL_DEBUG_LOG os_printf

#define LOCAL_ATRR ICACHE_RODATA_ATTR STORE_ATTR

#endif

