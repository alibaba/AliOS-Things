/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "aos/kv.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

/*
kv_init has been called in aos_components_init, but can called repeatedly.
#ifdef AOS_COMP_KV
    kv_init();
#endif
*/

static void kv_comp_example(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    char *key = "test_kv_key";
    char *set_value = "test_kv_value";
    int  set_len = strlen(set_value);
    char get_value[32] = {0};
    int  get_len = 32;
    int  ret = -1;

    ret = aos_kv_set(key, set_value, set_len, 1);
    if(ret){
        printf("kv comp test fail! aos_kv_set ret:%d\r\n", ret);
        return;
    }

    ret = aos_kv_get(key, get_value, &get_len);
    if(ret){
        printf("kv comp test fail! aos_kv_get ret:%d\r\n", ret);
        return;
    }

    if(get_len != set_len){
        printf("kv comp test fail! get_len != set_len, get_len:%d\r\n", get_len);
        return;
    }

    if(strcmp(get_value, set_value)){
        printf("kv comp test fail! get value not right! get_value:%s\r\n", get_value);
        return;
    }

    printf("kv comp test success!\r\n");
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(kv_comp_example, kv_example, kv component base example)
#endif
