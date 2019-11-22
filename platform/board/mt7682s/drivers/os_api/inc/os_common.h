#ifndef _OS_COMMON_H
#define _OS_COMMON_H

#include "k_api.h"
#include "aos/kernel.h"
#include "os_config.h"
#include "os_assert.h"
#include "typedef.h"
#define os_true 1
#define os_false 0

#define OS_WAIT_FOREVER 0xfffffffful
typedef u32_t os_tick_type_t;
#endif