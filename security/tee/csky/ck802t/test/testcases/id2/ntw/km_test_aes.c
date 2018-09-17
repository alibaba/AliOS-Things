#include "km.h"
#include "km_test_comm.h"
#include "km_test_dbg.h"
//#include <sys/time.h>
//#include <string.h>
#include <stdlib.h>
#include "tee_dbg.h"
#include "tee_sym.h"

#define KM_TEST_DEBUG 1

#define AES_BLOCK_LEN 16

#define ENC_DEC_TEST_LEN 48

// UPDATE_TEST_LEN + AES_BLOCK_LEN
static uint8_t aes_test_data[] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61,
    0x62, 0x63, 0x64, 0x65, 0x66, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
    0x36, 0x37, 0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x30,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x61, 0x62,
    0x63, 0x64, 0x65, 0x66, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
};

static uint8_t usr_iv[16] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                              0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66 };


#if KM_TEST_DEBUG
void km_test_dump_data(char *name, uint8_t *data, uint32_t len)
{
    uint32_t i;
    printk("name is %s, len is %d\n", name, len);

    for (i = 0; i < (len - len % 8); i += 8) {
        printk("%s:  %02x %02x %02x %02x %02x %02x %02x %02x\n", name,
               data[i + 0], data[i + 1], data[i + 2], data[i + 3], data[i + 4],
               data[i + 5], data[i + 6], data[i + 7]);
    }

    printk("%s:", name);
    while (i < len) {
        printk("%02x; ", data[i++]);
    }
    printk("\n");

    return;
}
#endif


uint32_t test_import(char *name, uint32_t name_len, km_key_type type)
{
    return 0;
}
uint32_t test_generate(char *name, uint32_t name_len, km_key_type type,
                       uint32_t key_size)
{
    return 0;
}

uint32_t test_delete(char *name, uint32_t name_len)
{
    return 0;
}

uint32_t test_cipher_enc_dec(char *name, uint32_t name_len,
                             km_block_mode_type type,
                             km_padding_type    padding_type)
{
    size_t       test_len = ENC_DEC_TEST_LEN;
    km_sym_param sym_param;
    uint8_t      src[ENC_DEC_TEST_LEN];
    uint8_t      enc_array[ENC_DEC_TEST_LEN + AES_BLOCK_LEN] = { 0 };
    uint8_t      dec_array[ENC_DEC_TEST_LEN + AES_BLOCK_LEN] = { 0 };
    size_t       src_len                                     = test_len;
    size_t       enc_len = ENC_DEC_TEST_LEN + AES_BLOCK_LEN;
    size_t       dec_len = ENC_DEC_TEST_LEN + AES_BLOCK_LEN;
    uint8_t *    enc     = enc_array;
    uint8_t *    dec     = dec_array;
    uint8_t *    iv      = NULL;
    uint8_t      iv_len  = 0;
    uint32_t     ret     = 0;

    sym_param.key_type                  = KM_AES;
    sym_param.cipher_param.purpose_type = KM_PURPOSE_ENCRYPT;
    sym_param.cipher_param.block_mode   = type;
    sym_param.cipher_param.padding_type = padding_type;

    memcpy(src, aes_test_data, test_len);

    if (type == KM_ECB) {
        iv = NULL;
    } else if (type == KM_CBC || type == KM_CTR) {
        iv     = usr_iv;
        iv_len = 16;
    }

    ret = km_cipher(name, name_len, &sym_param, iv, iv_len, src, src_len, enc,
                    &enc_len);
    if (ret) {
        KM_TEST_ERR("km enc failed 0x%x\n", ret);
        return KM_ERR_GENERIC;
    }

#if KM_TEST_DEBUG
    km_test_dump_data("cipher enc dest :", enc, enc_len);
#endif

    sym_param.cipher_param.purpose_type = KM_PURPOSE_DECRYPT;
    ret = km_cipher(name, name_len, &sym_param, iv, iv_len, enc, enc_len, dec,
                    &dec_len);

#if KM_TEST_DEBUG
    km_test_dump_data("cipher dec dest :", dec, dec_len);
#endif

    if (ret || dec_len != test_len || memcmp(dec, src, test_len)) {
        KM_TEST_ERR("dec result is not correct dec len %d\n", dec_len);
        return KM_ERR_GENERIC;
    }

    return ret;
}

uint32_t test_cipher_short(char *name, uint32_t name_len,
                           km_block_mode_type type,
                           km_padding_type    padding_type)
{
    size_t       test_len = ENC_DEC_TEST_LEN;
    km_sym_param sym_param;
    uint8_t      src[ENC_DEC_TEST_LEN];
    uint8_t *    enc     = NULL;
    uint8_t *    dec     = NULL;
    size_t       src_len = test_len;
    size_t       enc_len = 0;
    size_t       dec_len = 0;
    uint8_t *    iv      = NULL;
    uint8_t      iv_len  = 0;
    uint32_t     ret     = 0;

    sym_param.key_type                  = KM_AES;
    sym_param.cipher_param.purpose_type = KM_PURPOSE_ENCRYPT;
    sym_param.cipher_param.block_mode   = type;
    sym_param.cipher_param.padding_type = padding_type;

    memcpy(src, aes_test_data, test_len);

    if (type == KM_ECB) {
        iv = NULL;
    } else if (type == KM_CBC || type == KM_CTR) {
        iv     = usr_iv;
        iv_len = 16;
    }

    ret = km_cipher(name, name_len, &sym_param, iv, iv_len, src, src_len, enc,
                    &enc_len);
    if (ret != KM_ERR_SHORT_BUFFER) {
        KM_TEST_ERR("km get aes encrypt length failed 0x%x\n", ret);
        return ret;
    }
    enc = malloc(enc_len);
    if (!enc) {
        KM_TEST_ERR("malloc enc failed\n");
        return KM_ERR_OUT_OF_MEMORY;
    }
    ret = km_cipher(name, name_len, &sym_param, iv, iv_len, src, src_len, enc,
                    &enc_len);
    if (ret) {
        KM_TEST_ERR("km get aes encrypt length failed 0x%x\n", ret);
        goto clean;
    }

#if KM_TEST_DEBUG
    km_test_dump_data("cipher enc dest :", enc, enc_len);
#endif

    sym_param.cipher_param.purpose_type = KM_PURPOSE_DECRYPT;
    ret = km_cipher(name, name_len, &sym_param, iv, iv_len, enc, enc_len, dec,
                    &dec_len);
    if (ret != KM_ERR_SHORT_BUFFER) {
        KM_TEST_ERR("km get aes decrypt length failed 0x%x\n", ret);
        goto clean;
    }
    dec = malloc(dec_len);
    if (!dec) {
        KM_TEST_ERR("malloc dec buffer failed\n");
        ret = KM_ERR_OUT_OF_MEMORY;
        goto clean;
    }
    ret = km_cipher(name, name_len, &sym_param, iv, iv_len, enc, enc_len, dec,
                    &dec_len);
    if (ret) {
        KM_TEST_ERR("km get aes decrypt length failed 0x%x\n", ret);
        goto clean1;
    }

#if KM_TEST_DEBUG
    km_test_dump_data("cipher dec dest :", dec, dec_len);
#endif

    if (dec_len != test_len || memcmp(dec, src, test_len)) {
        KM_TEST_ERR("dec result is not correct\n");
        ret = KM_ERR_GENERIC;
    }

clean1:
    if (dec) {
        free(dec);
        dec = NULL;
    }
clean:
    if (enc) {
        free(enc);
        enc = NULL;
    }

    return ret;
}

uint32_t km_cipher_test(char *name, uint32_t name_len)
{
    uint32_t ret = 0;
    /*
    KM_TEST_INF("start test import/generate\n");
//    ret = test_import(NAME, NAME_LEN, KM_AES);
    ret = test_generate(name, name_len, KM_AES, 128);
    if (ret && ret != KM_ERR_ACCESS_CONFLICT) {
        KM_TEST_ERR("test import/generate failed 0x%x\n", ret);
        return ret;
    }*/

    ret = test_cipher_enc_dec(name, name_len, KM_ECB, KM_PKCS5);
    if (ret) {
        KM_TEST_ERR("test ecb pkcs5 failed 0x%x\n", ret);
        goto clean;
    }

    /*
    ret = test_cipher_enc_dec(name, name_len, KM_CBC, KM_PKCS5);
    if (ret) {
        KM_TEST_ERR("test ecb pkcs5 failed 0x%x\n", ret);
        goto clean;
    }*/

    ret = test_cipher_enc_dec(name, name_len, KM_ECB, KM_NO_PADDING);
    if (ret) {
        KM_TEST_ERR("test cbc no padding failed 0x%x\n", ret);
        goto clean;
    }

    /*
    ret = test_cipher_enc_dec(name, name_len, KM_CBC, KM_NO_PADDING);
    if (ret) {
        KM_TEST_ERR("test cbc no padding failed 0x%x\n", ret);
        goto clean;
    }

    ret = test_cipher_enc_dec(name, name_len, KM_CTR, KM_NO_PADDING);
    if (ret) {
        KM_TEST_ERR("test ctr no padding failed 0x%x\n", ret);
        goto clean;
    }*/

clean:
    /*
    if (test_delete(name, name_len)) {
        KM_TEST_ERR("test delete failed 0x%x\n", ret);
    }*/

    return ret;
}

uint32_t km_cipher_short_test(char *name, uint32_t name_len)
{
    uint32_t ret = 0;

    /*
    ret = test_import(name, name_len, KM_AES);
    if (ret && ret != KM_ERR_ACCESS_CONFLICT) {
        KM_TEST_ERR("test import failed 0x%x\n", ret);
        return ret;
    }*/

    ret = test_cipher_short(name, name_len, KM_ECB, KM_PKCS5);
    if (ret) {
        KM_TEST_ERR("test cipher short ecb pkcs5 failed 0x%x\n", ret);
        goto clean;
    }
    /*
    ret = test_cipher_short(name, name_len, KM_CBC, KM_PKCS5);
    if (ret) {
        KM_TEST_ERR("test cipher short cbc pkcs5 failed 0x%x\n", ret);
        goto clean;
    }*/
    KM_TEST_INF("****test cipher short pkcs5 success****\n");

    ret = test_cipher_short(name, name_len, KM_ECB, KM_NO_PADDING);
    if (ret) {
        KM_TEST_ERR("test cipher short ecb pkcs5 failed 0x%x\n", ret);
        goto clean;
    }
    /*
    ret = test_cipher_short(name, name_len, KM_CBC, KM_NO_PADDING);
    if (ret) {
        KM_TEST_ERR("test cipher short cbc pkcs5 failed 0x%x\n", ret);
        goto clean;
    }
    ret = test_cipher_short(name, name_len, KM_CTR, KM_NO_PADDING);
    if (ret) {
        KM_TEST_ERR("test cipher short ctr pkcs5 failed 0x%x\n", ret);
        goto clean;
    }*/
    KM_TEST_INF("****test cipher short no padding success****\n");

clean:
    /*
        if (test_delete(name, name_len)) {
            KM_TEST_ERR("test delete failed 0x%x\n", ret);
        }
    */
    return ret;
}

// uint32_t km_cipher_whole_test(char *name, uint32_t name_len)
uint32_t km_cipher_whole_test()
{
    uint32_t ret      = 0;
    char *   name     = ID2_KEY_NAME;
    uint32_t name_len = ID2_KEY_NAME_LEN;

    ret = km_cipher_test(name, name_len);
    if (ret) {
        KM_TEST_ERR("km_sym_test failed\n");
        return ret;
    }
    KM_TEST_INF("\n<<<<<< km cipher test success ! >>>>>>>>\n");

    ret = km_cipher_short_test(name, name_len);
    if (ret) {
        KM_TEST_ERR("km_sym_short_test failed\n");
        return ret;
    }
    KM_TEST_INF("\n<<<<<< ks sym short test success ! >>>>>>>>\n");

    return ret;
}

/*
uint32_t km_cipher_perf_test(uint32_t test_count)
{
    uint32_t i = 0;
    uint32_t ret = 0;
    double total_time, av_time;
    struct timeval start_tv, end_tv;

    uint8_t aes_src[ENC_DEC_TEST_LEN];
    uint8_t aes_enc[ENC_DEC_TEST_LEN + AES_BLOCK_LEN] = {0};
    uint8_t aes_dec[ENC_DEC_TEST_LEN + AES_BLOCK_LEN] = {0};
    size_t aes_src_len = ENC_DEC_TEST_LEN;
    size_t aes_enc_len = ENC_DEC_TEST_LEN + AES_BLOCK_LEN;
    size_t aes_dec_len = ENC_DEC_TEST_LEN + AES_BLOCK_LEN;
    km_sym_param sym_param;

    KM_TEST_INF("******************aes perf test start*******************\n");

    ret = test_generate(AES_GEN_NAME, AES_GEN_NAME_LEN, KM_AES, 128);
    if (ret && ret != KM_ERR_ACCESS_CONFLICT) {
        KM_TEST_ERR("test import/generate failed 0x%x\n", ret);
        return ret;
    }

    memcpy(aes_src, aes_test_data, aes_src_len);

    gettimeofday(&start_tv, NULL);
    sym_param.key_type = KM_AES;
    sym_param.cipher_param.purpose_type = KM_PURPOSE_ENCRYPT;
    sym_param.cipher_param.block_mode = KM_ECB;
    sym_param.cipher_param.padding_type = KM_PKCS5;

    for (i = 0; i < test_count; i++) {
        ret = km_cipher(AES_GEN_NAME, AES_GEN_NAME_LEN, &sym_param, NULL, 0,
               aes_src, aes_src_len, aes_enc, &aes_enc_len);
        if (ret) {
            KM_TEST_ERR("test enc dec failed 0x%x\n", ret);
            test_delete(AES_GEN_NAME, AES_GEN_NAME_LEN);
            return ret;
        }
    }

    gettimeofday(&end_tv, NULL);
    total_time = (end_tv.tv_usec - start_tv.tv_usec)/1000 +
                 (end_tv.tv_sec - start_tv.tv_sec) * 1000;

    av_time = total_time / test_count;
    KM_TEST_INF("cipher enc total time: %fms, av_time: %fms\n", total_time,
av_time);

    gettimeofday(&start_tv, NULL);
    sym_param.cipher_param.purpose_type = KM_PURPOSE_DECRYPT;
    for (i = 0; i < test_count; i++) {
        ret = km_cipher(AES_GEN_NAME, AES_GEN_NAME_LEN,
                &sym_param, NULL, 0, aes_enc, aes_enc_len,
                aes_dec, &aes_dec_len);
        if (ret) {
            KM_TEST_ERR("test enc dec failed 0x%x\n", ret);
            //test_delete(AES_GEN_NAME, AES_GEN_NAME_LEN);
            return ret;
        }
    }

    gettimeofday(&end_tv, NULL);
    if (aes_dec_len != aes_src_len &&
        memcmp(aes_src, aes_dec, aes_src_len)) {
        KM_TEST_INF("perf test aes decrypt wrong result\n");
        return KM_ERR_GENERIC;
    }
    total_time = (end_tv.tv_usec - start_tv.tv_usec)/1000 +
                 (end_tv.tv_sec - start_tv.tv_sec) * 1000;

    av_time = total_time / test_count;
    KM_TEST_INF("cipher dec total time: %fms, av_time: %fms\n", total_time,
av_time);


    ret = test_delete(AES_GEN_NAME, AES_GEN_NAME_LEN);
    if (ret) {
        KM_TEST_ERR("EEEEE test_delete failed\n");
        return ret;
    }

    return ret;
}*/
