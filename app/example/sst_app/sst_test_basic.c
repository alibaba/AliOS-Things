#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sst.h"
#include "sst_test.h"

#if CONFIG_AOS_SUPPORT
#define TEST_BASIC_KEY "./data/.sst/kc_test_basic_key"
#define TEST_LONG_NAME "./data/.sst/5kjdaoasdfjasoidfjoasidfjasidofiasodfisaodifaosdijfosajfdaoisfdoasifdaosdfiasofdiasdfoasidfaisjfdoaifdosadifaosdifsaodfiaosdfaisfoasfdjfisaodjfasidfjadfasdsdddddddddsfajsdofiaosdfjasdfsadfioasifdasodfoasdifosaijfsaofdioafdiasodfosafdsaoifdfasfaefawefalsadifjosajfaisdfoijsabaoibjoaadfasfdasdfjasiodjfosaidfjaosidfjaosifdaosdifosaidfaosifdoasifdaosidfaosdifaosdfiasdofiasdfoasifdoasifdsaodifosadifsoifdosifdosdsdsadfiosadfiasofisafibjaobijaoidjaodijfaoidj.sst"
#else
#define TEST_BASIC_KEY "kc_test_basic_key"
#define TEST_LONG_NAME "5kjdaoasdfjasoidfjoasidfjasidofiasodfisaodifaosdijfosajfdaoisfdoasifdaosdfiasofdiasdfoasidfaisjfdoaifdosadifaosdifsaodfiaosdfaisfoasfdjfisaodjfasidfjadfasdsdddddddddsfajsdofiaosdfjasdfsadfioasifdasodfoasdifosaijfsaofdioafdiasodfosafdsaoifdfasfaefawefalsadifjosajfaisdfoijsabaoibjoaadfasfdasdfjasiodjfosaidfjaosidfjaosifdaosdifosaidfaosifdoasifdaosidfaosdifaosdfiasdofiasdfoasifdoasifdsaodifosadifsoifdosifdosdsdsadfiosadfiasofisafibjaobijaoidjaodijfaoidj.sst"
#endif /* CONFIG_AOS_SUPPORT */

#define TEST_BASIC_SECRET "kc_test_basic_secret"
#define TEST_BASIC_SECRET_LEN 20
#define TEST_BASIC_SECRET_SHORT_LEN 19

static int test_sec_sst_store();
static int test_sec_sst_get();
static int test_sec_sst_delete();

typedef struct _store_test_params_t {
    char *name;
    uint8_t *secret;
    uint32_t secret_len;
    uint32_t key_type;
    uint32_t exp_ret;
} store_test_params_t;

typedef struct _get_test_params_t {
    char *name;
    uint8_t secret_null;
    uint32_t secret_len;
    uint32_t exp_ret;
} get_test_params_t;

typedef struct _delete_test_params_t {
    char *name;
    uint32_t exp_ret;
} delete_test_params_t;

store_test_params_t store_params[] = {
    {NULL, NULL, 0, 0, SST_ERROR_BAD_PARAMETERS},
    {NULL, (uint8_t *)TEST_BASIC_SECRET, TEST_BASIC_SECRET_LEN, 0, SST_ERROR_BAD_PARAMETERS},
    {TEST_BASIC_KEY, NULL, 0, 0, SST_ERROR_BAD_PARAMETERS},
    {TEST_BASIC_KEY, (uint8_t *)TEST_BASIC_SECRET, TEST_BASIC_SECRET_LEN, 0, SST_SUCCESS},
    {TEST_LONG_NAME, (uint8_t *)TEST_BASIC_SECRET, TEST_BASIC_SECRET_LEN, 0, SST_SUCCESS},
};

get_test_params_t get_params[] = {
    {NULL, 0, 0, SST_ERROR_BAD_PARAMETERS},
    {TEST_BASIC_KEY, 1, 1, SST_ERROR_BAD_PARAMETERS}, //secret = NULL
    {TEST_BASIC_KEY, 1, 0, SST_ERROR_SHORT_BUFFER},
    {TEST_BASIC_KEY, 0, TEST_BASIC_SECRET_SHORT_LEN, SST_ERROR_SHORT_BUFFER},
    {TEST_BASIC_KEY, 0, TEST_BASIC_SECRET_LEN, SST_SUCCESS},
    {TEST_BASIC_KEY, 0, 1024, SST_SUCCESS},
    {TEST_BASIC_SECRET, 0, TEST_BASIC_SECRET_LEN, SST_ERROR_ITEM_NOT_FOUND},
    {TEST_LONG_NAME, 0, TEST_BASIC_SECRET_LEN, SST_ERROR_ITEM_NOT_FOUND},
};

delete_test_params_t del_params[] = {
    {NULL, SST_ERROR_BAD_PARAMETERS},
    {TEST_BASIC_SECRET, SST_ERROR_ITEM_NOT_FOUND},
    {TEST_LONG_NAME, SST_SUCCESS},
};

static int test_sec_sst_store()
{
    char *name;
    uint8_t *secret;
    uint32_t secret_len = 0;
    uint32_t key_type;
    uint32_t ret;
    int size_store_case = (int)(sizeof(store_params) / sizeof(store_params[0]));
    store_test_params_t store_param;
    int i = 0;

    printf("test_sec_sst_store.\n");
    printf("store_case size %d\n", size_store_case);

    for (i = 0; i < size_store_case; i++) {
        store_param = store_params[i];
        printf("\ncase %d: ", i);

        name = store_param.name;
        secret = store_param.secret;
        secret_len = store_param.secret_len;
        key_type = store_param.key_type;
        ret = sst_add_item(name, secret, secret_len, key_type, 1);
        if (ret != store_param.exp_ret) {
            printf("add item return wrong\n");
            printf("test_sec_sst_store[%d] failed\n", i);
            return -1;
        }
        printf("test_sec_sst_store[%d] success\n", i);
    }
    printf("test_sec_sst_store total %d case success\n\n", size_store_case);
    return 0;
}

static int test_sec_sst_get()
{
    char *name = NULL;
    uint8_t *secret = NULL;
    uint32_t secret_len = 0;
    uint32_t key_type = 0;
    int i = 0;
    int size_get_case = (int)(sizeof(get_params) / sizeof(get_params[0]));
    get_test_params_t get_param;

    printf("test_sec_sst_get, ");
    printf("size_get_case : %d\n", size_get_case);

    for (i = 0; i < size_get_case; i++) {
        printf("\ncase %d: ", i);
        get_param = get_params[i];

        name = get_param.name;
        if (get_param.secret_null) {
            printf("secret_null is false\n");
            secret = NULL;
        } else {
            secret = sst_test_malloc(1024);
            sst_test_memset(secret, 0, 1024);
        }
        secret_len = get_param.secret_len;
        if (sst_get_item(name, secret, &secret_len, &key_type) != get_param.exp_ret) {
            printf("test_sec_sst_get[%d] failed\n", i);
            goto clean;
        }

        if (get_param.exp_ret == SST_ERROR_SHORT_BUFFER) {
            if (secret_len != TEST_BASIC_SECRET_LEN) {
                printf("test_sec_sst_get[%d] failed wrong len secret len %d\n", i, secret_len);
                goto clean;
            }
        }

        if (get_param.exp_ret == SST_SUCCESS) {
            if (secret_len != TEST_BASIC_SECRET_LEN) {
                printf("test_sec_sst_get[%d] failed wrong len %d\n", i, secret_len);
                goto clean;
            }

            if (sst_test_memcmp(secret, TEST_BASIC_SECRET, secret_len)) {
                printf("test_sec_sst_get[%d] failed wrong secret %s\n", i, secret);
                goto clean;
            } else {
                printf("secret is %s\n", secret);
            }
        }

        if (secret) {
            sst_test_free(secret);
            secret = NULL;
        }

        printf("test_sec_sst_get[%d] success\n", i);
    }
    printf("test_sec_sst_get success. \n\n");
    return 0;

clean:
    if (secret) {
        sst_test_free(secret);
        secret = NULL;
    }
    return -1;
}

int test_sec_sst_delete()
{
    char *name = NULL;
    int i = 0;
    int size_del_case = (int)(sizeof(del_params) / sizeof(del_params[0]));
    delete_test_params_t del_param;

    for (i = 0; i < size_del_case; i++) {
        del_param = del_params[i];
        name = del_param.name;
        if (sst_delete_item(name) != del_param.exp_ret) {
            printf("test_delete_item[%d] failed\n", i);
            return -1;
        }
    }

    return 0;
}

int sec_sst_basic_test()
{
    int ret = 0;

    if (sst_init()) {
        printf("sst init failed\n");
        return -1;
    }

    ret = test_sec_sst_store();
    if (ret) {
        printf("test sec sst store failed\n");
        return -1;
    }
    printf("<<<<<test store success>>>>\n");

    ret = test_sec_sst_delete();
    if (ret) {
        printf("test sec sst delete failed\n");
        return -1;
    }
    printf("<<<<<< test delete success >>>>>>\n");

    ret = test_sec_sst_get();
    if (ret) {
        printf("test sec sst store failed\n");
        return -1;
    }
    printf("<<<<<< test get success >>>>>>\n");
    printf("<<<<<< test all basic tetscase success >>>>>>\n");

    return 0;
}
