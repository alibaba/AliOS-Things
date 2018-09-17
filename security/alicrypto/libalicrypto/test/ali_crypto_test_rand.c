/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 */

#include "./inc/ali_crypto_test.h"
#include <sys/time.h>

#define OSA_strlen(_str) strlen(_str)

static int myrand(void *rng_state, unsigned char *output, size_t len)
{
    int result;
    uint8_t *seed;
    uint32_t tmp;
    struct timeval tv;

    if (rng_state != NULL) {
        ali_seed(rng_state, OSA_strlen(rng_state));
    }else{
        gettimeofday(&tv, NULL);
        tmp =tv.tv_sec*1000 + tv.tv_usec/1000;
        seed = (uint8_t *)&tmp;
        ali_seed(seed, sizeof(tmp));
    }
    result = ali_rand_gen(output, len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "gen rand fail(%08x)\n", result);
    }
    return 0;
}

int ali_crypto_rand_test(void)
{
    uint32_t i        = 0;
    uint8_t  seed[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    size_t   seed_len = 16;
    uint8_t  tmp_buf[32];
    uint8_t  rand_buf[32];
    size_t   rand_len = 32;
    ali_crypto_result result;

   /* test myrand for not set seed before */
    void *rng_state = NULL;
    unsigned char *output;
    output = CRYPT_MALLOC(rand_len);

    result = myrand(rng_state, output, rand_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "myrand fail(%08x)\n", result);
    }
    printf("output of myrand for not set seed before:\n");
    for(i=0; i < rand_len; i++){
        printf("%d\t",output[i]);
    }
    printf("\n");

   /* test myrand after set seed  */
    result = ali_seed(seed, seed_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "ali_seed fail(%08x)\n", result);
    }
    result = myrand(seed, output, rand_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "myrand fail(%08x)\n", result);
    }
    printf("output of myrand after set seed :\n");
    for(i=0; i< rand_len; i++){
        printf("%d\t",output[i]);
    }
	printf("\n");

    result = ali_seed(seed, seed_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "ali_seed fail(%08x)\n", result);
    }

    /* for gcov coverage */
    result = ali_rand_gen(NULL, rand_len);
    if (result == ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "gen rand fail(%08x)\n", result);
    }

    result = ali_rand_gen(rand_buf, rand_len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "gen rand fail(%08x)\n", result);
    }

    while (i++ < 10) {
        CRYPT_MEMCPY(tmp_buf, rand_buf, rand_len);

        result = ali_rand_gen(rand_buf, rand_len);
        if (result != ALI_CRYPTO_SUCCESS) {
            PRINT_RET(-1, "gen rand fail(%08x)\n", result);
        }

        if (!CRYPT_MEMCMP(tmp_buf, rand_buf, rand_len)) {
            ali_crypto_print_data("tmp_buf", tmp_buf, rand_len);
            ali_crypto_print_data("rand_buf", rand_buf, rand_len);
            PRINT_RET(-1, "RAND test fail!\n");
        }
    }

    CRYPT_INF("RAND test success!\n");

    return 0;
}
