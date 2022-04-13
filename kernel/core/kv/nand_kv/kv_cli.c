/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_COMP_CLI
void kv_register_cli_command(void) {}
#else

#include <stdint.h>
#include "kv_types.h"
#include <kv_api.h>
#include "aos/cli.h"

extern item_info_t *kv_item_traverse(item_func func, const char *key);
extern int32_t kv_flash_read(uint32_t blk, uint32_t page, uint32_t offset, void *outbuf, uint32_t buflen);

static int __item_print_cb(item_info_t *item, const char *key)
{
    char *p_key = NULL;
    char *p_val = NULL;

    p_key = (char *)aos_malloc(item->nlen + 1);
    if (!p_key) {
        return KV_ERR_MALLOC_FAILED;
    }

    memset(p_key, 0, item->nlen);
    memcpy(p_key, item->name, item->nlen);

    p_val = (char *)aos_malloc(item->vlen + 1);
    if (!p_val) {
        aos_free(p_key);
        return KV_ERR_MALLOC_FAILED;
    }

    memset(p_val, 0, item->vlen);
    memcpy(p_val, item->value, item->vlen);

    aos_cli_printf("%s = %s\r\n", p_key, p_val);
    aos_free(p_key);
    aos_free(p_val);

    return KV_LOOP_CONTINUE;
}

static void handle_kv_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int num = 0;
    int res = KV_OK;
    int len = KV_MAX_VAL_LEN;

    char *buffer = NULL;

    const char *rtype = argc > 1 ? argv[1] : "";

    if (strcmp(rtype, "set") == KV_OK) {
        if (argc != 4) {
            return;
        }

        res = kv_item_set(argv[2], argv[3], strlen(argv[3]));
        if (res != KV_OK) {
            aos_cli_printf("cli set kv failed %d.\r\n", res);
        }
    } else if (strcmp(rtype, "get") == KV_OK) {
        if (argc != 3) {
            return;
        }

        buffer = (char *)aos_malloc(KV_MAX_VAL_LEN);
        if (!buffer) {
            aos_cli_printf("there is no space\r\n");
            return;
        }

        memset(buffer, 0, KV_MAX_VAL_LEN);
        res = kv_item_get(argv[2], buffer, &len);
        if (res != 0) {
            aos_cli_printf("cli: no paired kv\r\n");
        } else {
            aos_cli_printf("value is %s\r\n", buffer);
        }

        if (buffer) {
            aos_free(buffer);
        }
    } else if (strcmp(rtype, "del") == KV_OK) {
        if (argc != 3) {
            return;
        }

        res = kv_item_delete(argv[2]);
        if (res != KV_OK) {
            aos_cli_printf("cli kv del failed %d\r\n", res);
        }
    } else if (strcmp(rtype, "list") == KV_OK) {
        kv_item_traverse(__item_print_cb, NULL);
    } else if (strcmp(rtype, "seti") == KV_OK) {
        if (argc != 4) {
            return;
        }

        num = atoi(argv[3]);
        res = kv_item_set(argv[2], (void *)(&num), sizeof(int));
        if (res != KV_OK) {
            aos_cli_printf("cli set integer kv failed %d.\r\n", res);
        }
    } else if (strcmp(rtype, "geti") == KV_OK) {
        num = 0;
        len = sizeof(int);

        if (argc != 3) {
            return;
        }

        res = kv_item_get(argv[2], &num, &len);
        if (res != 0) {
            aos_cli_printf("cli: no paired kv\r\n");
        } else {
            aos_cli_printf("value is %d\r\n", num);
        }
    }

    return;
}

static struct cli_command kv_cmd = {
    "kv", "kv [set key value | get key | del key | seti key int_val | geti key | list]", handle_kv_cmd
};

void kv_register_cli_command(void)
{
    cli_register_command(&kv_cmd);
}

#endif

