#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kv_api.h>
#include <kv_types.h>
#include <kv_util.h>

extern int32_t kv_flash_read(uint32_t blk, uint32_t page, uint32_t offset, void *outbuf, uint32_t buflen);
static int __item_print_cb(item_info_t *item, const char *key)
{
    printf("%s = %s\r\n", item->name, (char *)(item->value));
    return KV_LOOP_CONTINUE;
}

extern item_info_t *kv_item_traverse(item_func func, const char *key);

void traverse_test()
{
    item_info_t *item;
    printf("\n===Priting all kv items===\n");
    item = kv_item_traverse(__item_print_cb, NULL);
    if (item) {
        aos_free(item);
    }
}
