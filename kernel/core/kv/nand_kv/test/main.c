#include <stdio.h>
#include <string.h>
#include <kv_api.h>
#include <aos/hal/flash.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <kv_types.h>

#define KV_SET_MAX 1000000
#define KV_TOTAL_MAX (KV_BLOCK_SIZE / 100 * 90)

struct kv_set_record_s {
    struct kv_set_record_s *next;
    char name[KV_MAX_KEY_LEN + 1];
    uint8_t value[KV_MAX_VAL_LEN + 1];
    uint32_t len;
} *record, *record_end;

extern void traverse_test();
int main(int argc, char *argv[])
{
    int i, j, ret, total = 0, diff_siz, clean_cnt = 0;
    char name[KV_MAX_KEY_LEN + 1], value[KV_MAX_VAL_LEN + 1], value_read[KV_MAX_VAL_LEN + 1], c;
    int32_t nlen, vlen;
    bool is_update = false;
    struct kv_set_record_s *r;
    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);

    if (hal_flash_init() != 0) {
        printf("Failed to initialize flash device.\n");
        return -1;
    }

    if (kv_init() != 0) {
        printf("Failed to initialize kv.\n");
        hal_flash_destroy();
        return -1;
    }

    for (i = 0; i < KV_SET_MAX; i++) {
        if (total > KV_TOTAL_MAX) {
            printf("KV is almost full now (%d), let's clean kv for further test.\n", total);
            for (r = record; r;) {
                vlen = sizeof(value_read);
                memset(value_read, 0, vlen);
                if (kv_item_get(r->name, value_read, &vlen)) {
                    printf("Before deleting, we fail to read %s\n", r->name);
                } else {
                    printf("Before deleting, we succeed to read %s, value: %s\n", r->name, value_read);
                }
                if (kv_item_delete(r->name) != 0) {
                    printf("Failed to delete item %s, value length: %d\n", r->name, r->len);
                    printf("For now total size in kv is %d.\n", total);
                    vlen = sizeof(value_read);
                    memset(value_read, 0, vlen);
                    if (kv_item_get(r->name, value_read, &vlen)) {
                        printf("For now, we fail to read %s\n", r->name);
                    } else {
                        printf("For now, we succeed to read %s\n", r->name);
                        if (strcmp(value_read, r->value) == 0) {
                            printf("The read value equals to the origin value.\n");
                        } else {
                            printf("The read value NOT equals to the origin value.\n"\
                                   "origin: %s\nread  : %s\n", r->value, value_read);
                        }
                    }
                    assert(0);
                } else {
                    printf("Item %s deleted, value length: %d, value: %s\n", r->name, r->len, r->value);
                    total -= r->len + strlen(r->name) + 1;
                    record = record->next;
                    free(r);
                    r = record;
                }
            }
            printf("After clean, kv size is %d now.\n", total);
            clean_cnt++;
        }

        printf("*********************start%d*******************\n", i);

        memset(name, 0, sizeof(name));
        memset(value, 0, sizeof(value));

        /* random name and value */
        nlen = rand() % (sizeof(name) - 8);
        if (nlen < 1) {
            nlen = 1;
        }
        vlen = rand() % (sizeof(value) - 8);
        if (vlen < 1) {
            vlen = 1;
        }
        for (j = 0; j < nlen; j++) {
            /* pick up a printable char */
            c = rand() % 126;
            if (c < 32) {
                c += 32;
            }
            name[j] = c;
        }
        name[j] = '\0';
        for (j = 0; j < vlen; j++) {
            /* pick up a printable char */
            c = rand() % 126;
            if (c < 32) {
                c += 32;
            }
            value[j] = c;
        }
        value[j] = '\0';

        printf("=====>setting\n%s\nto\n%s\n<=====\n", name, value);
        if (ret = kv_item_set(name, value, strlen(value) + 1)) {
            printf("=====>Failed to set\n%s\nret: %d\n<=====\n", name, ret);
            continue;
        }

        diff_siz = strlen(name) + 1 + strlen(value) + 1;
        /* decide if it's an update */
        is_update = false;
        for (r = record; r; r = r->next) {
            if (strcmp(r->name, name) == 0) {
                is_update = true;
                diff_siz = strlen(value) - strlen(r->value);
                memset(r->value, 0, sizeof(r->value));
                strncpy(r->value, value, sizeof(r->value));
                r->len = strlen(value) + 1;
                break;
            }
        }

        if (!is_update) {
            r = malloc(sizeof(struct kv_set_record_s));
            if (r) {
                strncpy(r->name, name, strlen(name) + 1);
                memcpy(r->value, value, strlen(value) + 1);
                r->len = strlen(value) + 1;
                r->next = NULL;
                if (!record) {
                    record = r;
                    record_end = r;
                } else {
                    record_end->next = r;
                    record_end = r;
                }
            } else {
                printf("Failed to malloc.\n");
                exit(1);
            }
        }

        printf("=====>reading\n%s\nback ...\n<=====\n", name);
        vlen = sizeof(value_read);
        memset(value_read, 0, sizeof(value_read));
        if (ret = kv_item_get(name, value_read, &vlen)) {
            printf("=====>Failed to read\n%s\nret: %d\n<=====\n", name, ret);
        } else {
            printf("=====>read value for\n%s\nis\n%s\n<=====\n", name, value_read);
            if (memcmp(value, value_read, strlen(value))) {
                printf("*** Error: the read back value is not the same as original value ***.\n");
                return -1;
            }
        }

        total += diff_siz;
        printf("Totally %d bytes data stored in kv now.\n", total);
        printf("*********************end%d*******************\n\n", i);
    }

    /* do a round read back check before end. */
    struct kv_set_record_s *p = record, *q;
    printf("\n\nWrite-Read test finished. Now let's do last round readback check.\n");
    printf("Totally %d bytes data stored in kv now, cleaned for %d times.\n", total, clean_cnt);
    while (p) {
        //printf("=====>reading\n%s\nback ...\n<=====\n", p->name);
        vlen = sizeof(value_read);
        memset(value_read, 0, sizeof(value_read));
        if (ret = kv_item_get(p->name, value_read, &vlen)) {
            printf("=====>Failed to read\n%s\nret: %d\n<=====\n", p->name, ret);
        } else {
            //printf("=====>read value for\n%s\nis\n%s\n<=====\n", p->name, value_read);
            if (memcmp(p->value, value_read, p->len)) {
                printf("*** Error: the read back value is not the same as original value ***.\n");
                printf("*** item name: %s ***\n", p->name);
                printf("*** item set value:      %s ***\n", p->value);
                printf("*** item readback value: %s ***\n", value_read);
                return -1;
            }
        }
        q = p;
        p = p->next;
        free(q);
    }
    printf("\nGood! Last round readback check succeed.\n\n");

    traverse_test();

    hal_flash_destroy();
    return 0;
}
