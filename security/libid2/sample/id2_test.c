/**
 * Copyright (C) 2017 The YunOS Project. All rights reserved.
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sm_id2.h"

#if CONFIG_AOS_SUPPORT
#include "aos/kernel.h"
#endif

#if ID2_MULT_SUPPORT
#include <pthread.h>
#endif

//only support aes key later
#define ID2_RSA_KEY 0
#define ID2_AES_KEY 1

#define ID_LEN                 (24)
#define SIGN_LEN               (128)

#define THREAD_NUM  0x01
#define TEST_COUNT  0x01
#define SYM_TEST_DATA_LEN   48

#if ID2_MULT_SUPPORT
static uint8_t thread_id = 0;
#endif

uint8_t test_data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00};
uint32_t test_data_len = 16;
uint8_t test_data1[SIGN_LEN] = {0x00};
uint32_t test_data1_len = SIGN_LEN;

static uint8_t aes_test_data[SYM_TEST_DATA_LEN] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
};

uint32_t short_test_count = 5;
uint32_t short_test_len[] = {13, 17, 35, 16, 32};

void id2_test_dump_data(char *name, uint8_t *data, uint32_t len)
{
    uint32_t i;
    printf("name is %s, len is %d\n", name, len);

    for (i = 0; i < (len - len % 8); i += 8) {
        printf("%s:  %02x %02x %02x %02x %02x %02x %02x %02x\n",
                          name, data[i+0], data[i+1], data[i+2], data[i+3],
                          data[i+4], data[i+5], data[i+6], data[i+7]);
    }

    printf("%s:: ", name);
    while(i < len) {
        printf("%02x, ", data[i++]);
    }
    printf("\n");

    return;
}

int test_get_id()
{
    int ret;
    uint8_t id_value[ID_LEN + 1];
    uint32_t id2_len = ID_LEN;

    ret = tee_get_ID2(id_value, &id2_len);
    if (ret < 0) {
        printf("fail to get ID!\n");
        return ret;
    }
    id_value[ID_LEN] = '\0';
    printf("id2_len is %d, ID2 ID: %s\n", id2_len, id_value);

    return ret;
}

#if ID2_AES_KEY
int test_cipher(uint8_t cipher_type, uint8_t block_mode, uint8_t padding_type)
{
    uint32_t test_len = 0;
    int ret = 0;
    uint8_t ID = KEY_ID_33;
    cipher_param_t cipher_param;
    uint8_t in[128];
    uint32_t in_len = 0;
    uint8_t *enc = NULL;
    uint32_t enc_len = 0;
    uint8_t *dec = NULL;
    uint32_t dec_len = 0;
    uint8_t *iv = NULL;
    uint8_t iv_len = 0;
    uint8_t test_iv[16] = {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    };

    if (padding_type == SM_PKCS5) {
        test_len = SYM_TEST_DATA_LEN - 3;
    } else {
        test_len = SYM_TEST_DATA_LEN;
    }

    if (block_mode == SM_ECB) {
        iv = NULL;
    } else if (block_mode == SM_CBC || block_mode == SM_CTR) {
        iv = test_iv;
        iv_len = 16;
    }

    in_len = test_len;

    cipher_param.cipher_type = cipher_type;
    cipher_param.block_mode = block_mode;
    cipher_param.padding_type = padding_type;
    cipher_param.is_enc = SM_ENCRYPT;

    memcpy(in, aes_test_data, test_len);

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                     in, in_len, enc, &enc_len);
    if (!ret) {
        printf("fail to get cipher enc len\n");
        ret = -1;
        return ret;
    }

    enc = malloc(enc_len);
    if (!enc) {
        printf("malloc enc buffer failed\n");
        return -1;
    }

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                       in, in_len, enc, &enc_len);
    if (ret) {
        printf("fail to cipher enc\n");
        ret = -1;
        goto clean1;
    }

#if ID2_TEST_DEBUG
    id2_test_dump_data("enc is:", enc, enc_len);
#endif

    //test decrypt
    cipher_param.is_enc = SM_DECRYPT;
    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                        enc, enc_len, dec, &dec_len);
    if (!ret) {
        printf("fail to get cipher dec len %d\n", ret);
        ret = -1;
        goto clean1;
    }

    dec = malloc(dec_len);
    if (!dec) {
        printf("malloc enc buffer failed\n");
        ret = -1;
        goto clean1;
    }

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                          enc, enc_len, dec, &dec_len);
    if (ret) {
        printf("fail to cipher dec\n");
        ret = -1;
        goto clean2;
    }

#if ID2_TEST_DEBUG
    id2_test_dump_data("dec is:", dec, dec_len);
#endif

    if (memcmp(in, dec, in_len)) {
        printf("decrypt result is not correct\n");
        ret = -1;
    } else {
        printf("sym cipher test success\n");
    }

clean2:
    if (dec) {
        free(dec);
        dec = NULL;
    }

clean1:
    if (enc) {
        free(enc);
        enc = NULL;
    }

    return ret;
}

int test_cipher_short_buffer(uint8_t cipher_type,
               uint8_t block_mode, uint32_t index)
{
    uint32_t test_len = short_test_len[index];
    int ret = 0;
    uint8_t ID = KEY_ID_33;
    cipher_param_t cipher_param;
    uint8_t in[128];
    uint32_t in_len = test_len;
    uint8_t *enc = NULL;
    uint32_t enc_len = 0;
    uint8_t *dec = NULL;
    uint32_t dec_len = 0;
    uint8_t tmp_buf[SYM_TEST_DATA_LEN];
    uint8_t *iv = NULL;
    uint32_t iv_len = 0;
    uint8_t test_iv[16] = {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    };


    cipher_param.cipher_type = cipher_type;
    cipher_param.block_mode = block_mode;
    cipher_param.padding_type = SM_PKCS5;
    cipher_param.is_enc = SM_ENCRYPT;

    if (block_mode == SM_ECB) {
        iv = NULL;
    } else if (block_mode == SM_CBC || block_mode == SM_CTR) {
        iv = test_iv;
        iv_len = 16;
    }

    memcpy(in, aes_test_data, test_len);

    enc_len = in_len;
    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                        in, in_len, tmp_buf, &enc_len);
    if (!ret) {
        printf("fail to get cipher enc len\n");
        ret = -1;
        return ret;
    }
    printf("ID2_SHORT_BUFFER: get enc len is %d\n", enc_len);
    enc = malloc(enc_len);
    if (!enc) {
        printf("malloc enc buffer failed\n");
        return -1;
    }

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                           in, in_len, enc, &enc_len);
    if (ret) {
        printf("fail to cipher enc\n");
        ret = -1;
        goto clean1;
    }

#if ID2_TEST_DEBUG
    id2_test_dump_data("enc is:", enc, enc_len);
#endif

    //test decrypt
    cipher_param.is_enc = SM_DECRYPT;
    dec_len = in_len - 1;
    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                 enc, enc_len, tmp_buf, &dec_len);
    if (!ret) {
        printf("fail to get cipher dec len %d\n", ret);
        ret = -1;
        goto clean1;
    }
    printf("ID2_SHORT_BUFFER: dec len is %d\n", dec_len);

    dec = malloc(dec_len);
    if (!dec) {
        printf("malloc enc buffer failed\n");
        ret = -1;
        goto clean1;
    }

    ret = tee_sym_cipher(ID, &cipher_param, iv, iv_len,
                           enc, enc_len, dec, &dec_len);
    if (ret) {
        printf("fail to cipher dec\n");
        ret = -1;
        goto clean2;
    }

#if ID2_TEST_DEBUG
    id2_test_dump_data("dec is:", dec, dec_len);
#endif

    if (memcmp(in, dec, in_len)) {
        printf("decrypt result is not correct\n");
        ret = -1;
    } else {
        printf("sym cipher test success\n");
    }

clean2:
    if (dec) {
        free(dec);
        dec = NULL;
    }

clean1:
    if (enc) {
        free(enc);
        enc = NULL;
    }

    return ret;
}

int test_sym_cipher()
{
    int ret = 0;

    ret = test_get_id();
    if (ret) {
        printf("get id failed\n");
        return -1;
    }

    ret = test_cipher(SM_AES, SM_ECB, SM_PKCS5);
    if (ret) {
        printf("test aes ecb pkcs5 failed\n");
        return ret;
    }

    ret = test_cipher(SM_AES, SM_CBC, SM_PKCS5);
    if (ret) {
        printf("test aes ecb pkcs5 failed\n");
        return ret;
    }

    ret = test_cipher(SM_AES, SM_ECB, SM_No_PADDING);
    if (ret) {
        printf("test aes ecb no padding failed\n");
        return ret;
    }

    ret = test_cipher(SM_AES, SM_CBC, SM_No_PADDING);
    if (ret) {
        printf("test aes ecb no padding failed\n");
        return ret;
    }

    ret = test_cipher(SM_AES, SM_CTR, SM_No_PADDING);
    if (ret) {
        printf("test aes ecb no padding failed\n");
        return ret;
    }

    return ret;
}

int test_short_sym_cipher()
{
    int ret = 0;
    int i = 0;

    ret = test_get_id();
    if (ret) {
        printf("get id failed\n");
        return -1;
    }

    for (i = 0; i < short_test_count; i++) {
        ret = test_cipher_short_buffer(SM_AES, SM_ECB, i);
        if (ret) {
            printf("test aes ecb pkcs5 failed\n");
            return ret;
        }

        ret = test_cipher_short_buffer(SM_AES, SM_CBC, i);
        if (ret) {
            printf("test aes ecb pkcs5 failed\n");
            return ret;
        }
    }

    return ret;
}

int test_whole_sym()
{
    int ret = 0;

    ret = test_sym_cipher();
    if (ret) {
        printf("test sym cipher success\n");
        return -1;
    }
    printf("\n<<<<<<< test sym cipher success>>>>>>>>\n");

    ret = test_short_sym_cipher();
    if (ret) {
        printf("test short sym cipher success\n");
        return -1;
    }
    printf("\n<<<<<<< test short sym cipher success>>>>>>>>\n");

    return 0;
}

int test_sym_perf()
{
    int ret = 0;
    uint32_t data_len = 100;
    uint8_t data[128];
    uint32_t test_num = 10000;
    uint8_t ID = KEY_ID_33;
    uint8_t enc[256];
    uint32_t enc_len = 256;
    uint8_t dec[256];
    uint32_t dec_len = 256;
    uint32_t i = 0;
    cipher_param_t cipher_param;
#if (CONFIG_GENERIC_LINUX)
    time_t start_time, end_time;
#else
    long long yos_time1, yos_time2;
#endif
    double total_time, av_time;

    memset(data, 0x33, data_len);

#if (CONFIG_GENERIC_LINUX)
    start_time = time(NULL);
#else
    yos_time1 = aos_now_ms();
#endif

    cipher_param.cipher_type = SM_AES;
    cipher_param.block_mode = SM_ECB;
    cipher_param.padding_type = SM_PKCS5;
    cipher_param.is_enc = SM_ENCRYPT;

    for(i = 0; i < test_num; i++) {
        ret = tee_sym_cipher(ID, &cipher_param, NULL, 0,
                     data, data_len, enc, &enc_len);
        if (ret < 0) {
            printf("fail to sign data!\n");
            return ret;
        }
    }

#if CONFIG_GENERIC_LINUX
    end_time = time(NULL);
    total_time = difftime(end_time, start_time);
#else
    yos_time2 = aos_now_ms();
    total_time = ((yos_time2 - yos_time1) & 0x00000000ffffffff);
#endif

    av_time = total_time / test_num;
    printf("cipher enc total time: %f, av_time: %f\n", total_time, av_time);

#if CONFIG_GENERIC_LINUX
    start_time = time(NULL);
#else
    yos_time1 = aos_now_ms();
#endif
    cipher_param.is_enc = SM_DECRYPT;
    for (i = 0; i < test_num; i++) {
        ret = tee_sym_cipher(ID, &cipher_param, NULL, 0,
                     enc, enc_len, dec, &dec_len);
        if (ret < 0) {
            printf("fail to verify data!\n");
            return ret;
        }
    }

#if CONFIG_GENERIC_LINUX
    end_time = time(NULL);
    total_time = difftime(end_time, start_time);
#else
    yos_time2 = aos_now_ms();
    total_time = ((yos_time2 - yos_time1) & 0x00000000ffffffff);
#endif
    av_time = total_time / test_num;
    printf("cipher dec total time: %f, av_time: %f\n", total_time, av_time);

    return 0;
}
#endif

#if ID2_RSA_KEY
int test_sig_ver(uint8_t type, uint8_t *data, uint32_t data_len)
{
    int ret;
    uint8_t ID = KEY_ID_0;
    uint8_t digest[SIGN_LEN];
    uint32_t digest_len = SIGN_LEN;

    ret = tee_RSA_sign(ID, data, data_len, digest, &digest_len, type);
    if (ret < 0) {
        printf("fail to sign data!\n");
        return ret;
    }

    ret = tee_RSA_verify(ID, data, data_len, digest, digest_len, type);
    if (ret < 0) {
        printf("fail to verify data!\n");
        return ret;
    }

    printf("test %d sig and ver success\n", type);

    return 0;
}

int test_enc_dec(uint8_t padding, uint8_t *data, uint32_t data_len)
{
    int ret;
    uint8_t ID = KEY_ID_0;
    uint8_t *enc_data = NULL;
    uint32_t enc_len = 0;
    uint8_t *dec_data = NULL;
    uint32_t dec_len = 0;

    if ((padding != SM_No_PADDING) &&
        (padding != SM_PKCS1_PADDING)) {
        printf("wriong padding type %d\n", padding);
        return -1;
    }

    ret = tee_RSA_public_encrypt(ID,
                  data, data_len, enc_data, &enc_len, padding);
    if (ret == 0) {
        printf("fail to get enc len\n");
        ret = -1;
        return ret;
    }

    enc_data = malloc(enc_len);
    if (!enc_data) {
        printf("malloc failed\n");
        return -1;
    }

    ret = tee_RSA_public_encrypt(ID,
                  data, data_len, enc_data, &enc_len, padding);
    if (ret < 0 || enc_len != SIGN_LEN) {
        printf("fail to encrypt data data len is %d,enc len is %d\n", data_len, enc_len);
        ret = -1;
        goto clean;
    }

    printf("rsa encrypted data len: %d\n", enc_len);

    ret = tee_RSA_private_decrypt(ID,
                  enc_data, enc_len, dec_data, &dec_len, padding);
    if (ret == 0) {
        printf("fail to get decrypt len!\n");
        ret = -1;
        goto clean;
    }

    printf("rsa decrypted data len: %d\n", dec_len);
    dec_data = malloc(dec_len);
    if (!dec_data) {
        free(dec_data);
        dec_data = NULL;
    }
    ret = tee_RSA_private_decrypt(ID,
                  enc_data, enc_len, dec_data, &dec_len, padding);
    if (ret < 0) {
        printf("fail to decrypt len!\n");
        ret = -1;
        goto clean;
    }

    if (memcmp(data, dec_data, data_len)) {
        printf("bad decrypt result\n");
#if ID2_TEST_DEBUG
        id2_test_dump_data("data is:", data, data_len);
        id2_test_dump_data("dec_data is:", dec_data, data_len);
#endif
        ret = -1;
        goto clean;
    } else {
        printf("test enc dec success\n");
    }

clean:
    if (enc_data) {
        free(enc_data);
        enc_data = NULL;
    }
    if (dec_data) {
        free(dec_data);
        dec_data = NULL;
    }

    return ret;
}

int test_sig_ver_perf(uint8_t type)
{
    int ret = 0;
    uint32_t data_len = 128;
    uint8_t data[128];
    uint32_t test_num = 100;
    uint8_t ID = KEY_ID_0;
    uint8_t digest[SIGN_LEN];
    uint32_t digest_len = SIGN_LEN;
    uint32_t i = 0;
#if CONFIG_GENERIC_LINUX
    time_t start_time, end_time;
    double total_time, av_time;
#else
    long long yos_time1, yos_time2;
#endif

    memset(data, 0x33, data_len);

#if CONFIG_GENERIC_LINUX
    start_time = time(NULL);
#else
    yos_time1 = aos_now_ms();
#endif

    for(i = 0; i < test_num; i++) {
        ret = tee_RSA_sign(ID, data, data_len, digest, &digest_len, type);
        if (ret < 0) {
            printf("fail to sign data!\n");
            return ret;
        }
    }

#if CONFIG_GENERIC_LINUX
    end_time = time(NULL);
    total_time = difftime(end_time, start_time);
#else
    yos_time2 = aos_now_ms();
    total_time = ((yos_time2 - yos_time1) & 0x00000000ffffffff);
#endif

    av_time = total_time / test_num;
    printf("%d sign total time is %f, av_time is %f\n", type, total_time, av_time);

#if CONFIG_GENERIC_LINUX
    start_time = time(NULL);
#else
    yos_time1 = aos_now_ms();
#endif
    for (i = 0; i < test_num; i++) {
        ret = tee_RSA_verify(ID, data, data_len, digest, digest_len, type);
        if (ret < 0) {
            printf("fail to verify data!\n");
            return ret;
        }
    }

#if CONFIG_GENERIC_LINUX
    end_time = time(NULL);
    total_time = difftime(end_time, start_time);
#else
    yos_time2 = aos_now_ms();
    total_time = ((yos_time2 - yos_time1) & 0x00000000ffffffff);
#endif
    av_time = total_time / test_num;
    printf("%d verify total time is %f, av_time is %f\n", type, total_time, av_time);

    return 0;
}

int test_whole_rsa()
{
    int ret = 0;

    ret = test_get_id();
    if (ret) {
        printf("get id failed\n");
        return -1;
    }

    ret = test_sig_ver(SM_MD5, test_data, test_data_len);
    if (ret) {
        printf("test MD5 sign verify failed\n");
        return -1;
    }

    printf("test md5 sign and verify success\n");
    ret = test_sig_ver(SM_SHA1, test_data, test_data_len);
    if (ret) {
        printf("test SHA1 sign verify failed\n");
        return -1;
    }
    printf("test sha1 sign and verify success\n");

    memset(test_data1, 0x33, test_data1_len);

#if 0
    //not support yet
    ret = test_enc_dec(SM_No_PADDING, test_data1, test_data1_len);
    if (ret) {
        printf("test no padding enc and dec failed\n");
        return -1;
    }
#endif

    ret = test_enc_dec(SM_PKCS1_PADDING, test_data, test_data_len);
    if (ret) {
        printf("test pkcs1 enc and dec failed\n");
        return -1;
    }

    return 0;
}

int id2_rsa_perf_test()
{
    int ret = 0;

    ret = test_sig_ver_perf(SM_MD5);
    if (ret) {
        printf("test md5 performance failed\n");
        return ret;
    }
    ret = test_sig_ver_perf(SM_SHA1);
    if (ret) {
        printf("test sha1 performance failed\n");
        return ret;
    }

    return ret;
}
#endif

#if ID2_MULT_SUPPORT
void * thread_content(void* argv)
{
    int i = 0;
    int ret = 0;
    uint32_t id = 0;

    printf("*** the %dth thread start ***\n", thread_id);
    thread_id++;
    id = thread_id;

    for (i = 0; i < TEST_COUNT; i++) {
#if ID2_RSA_KEY
        ret = test_whole_rsa();
#elif ID2_AES_KEY
        ret = test_whole_sym();
#endif
        if (ret) {
            printf("%dth thread failed\n", id);
            break;
        }
    }
    printf("*** the %dth thread success ***\n", id);

    return NULL;
}

int id2_fun_test()
{
    int i = 0;
    pthread_t thread[THREAD_NUM];

    for(i = 0; i < THREAD_NUM; i++) {
        pthread_create(&thread[i], NULL, thread_content, NULL);
    }

    for(i = 0; i < THREAD_NUM; i++) {
        pthread_join(thread[i],NULL);
    }

    return 0;
}

inline void printf_help()
{
    printf("Usage:    prov_test [option]\n");
    printf("-h:       display helpful information.\n");
    printf("-f:       test function\n");
    printf("-p:       test performance\n");

    return;
}

int id2_main(int argc, char *argv[])
{
    int ret = 0;

    if ((argc != 2) || (0 == strcmp(argv[1], "-h"))) {
        printf_help();
        return -1;
    }
    if (!strcmp(argv[1], "-f")) {
        id2_fun_test();
    } else if (!strcmp(argv[1], "-p")) {
#if ID2_RSA_KEY
        ret = id2_rsa_perf_test();
#elif ID2_AES_KEY
        ret = test_sym_perf();
#endif
        if (ret) {
            printf("id2 perf test failed\n");
            return -1;
        }
    } else {
        printf_help();
    }

    return 0;
}
#endif

