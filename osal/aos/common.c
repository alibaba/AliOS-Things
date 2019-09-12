/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdlib.h>

#include "aos/kv.h"
#include "aos/kernel.h"
#include "k_config.h"

#define SEED_MAGIC 0x123

extern void hal_reboot(void);

void aos_reboot(void)
{
    hal_reboot();
}

int aos_get_hz(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}

const char *aos_version_get(void)
{
    return SYSINFO_KERNEL_VERSION;
}

void aos_srand(unsigned int seed)
{
#if !defined (ENABLE_RNG) && defined (AOS_COMP_KV)
    int           ret        = 0;
    int           seed_len   = 0;
    unsigned int  seed_val   = 0;
    static char  *g_seed_key = "seed_key";

    seed_len = sizeof(seed_val);
    ret = aos_kv_get(g_seed_key, &seed_val, &seed_len);
    if (ret) {
        seed_val = SEED_MAGIC;
    }
    seed_val += seed;
    srand(seed_val);

    seed_val = rand();
    aos_kv_set(g_seed_key, &seed_val, sizeof(seed_val), 1);
#else
    srand(seed);
#endif
}

int aos_rand(void)
{
    return rand();
}

