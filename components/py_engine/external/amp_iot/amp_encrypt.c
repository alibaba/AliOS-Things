/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "amp_encrypt.h"

// #define AMP_ENCRYPT_DEBUG_MESSAGE

// haas hash code cal
int amp_hash_init(amp_hash_ctx_t *ctx, unsigned char type)
{
    int ret = 0;
    if (ctx == NULL) {
        amp_encrypt_printf("Invalid hash param");
        return AMP_ENCRYPT_INVALID_PARAMETER;
    }

    ctx->hash_method = type;
    switch (type) {
        case AMP_HASH_SHA256:
            amp_sha256_init(&ctx->sha256_ctx);
            amp_sha256_starts(&ctx->sha256_ctx, 0);
            break;
        case AMP_HASH_MD5:
            amp_md5_init(&ctx->md5_ctx);
            amp_md5_starts(&ctx->md5_ctx);
            break;
        default:
            ret = AMP_ENCRYPT_INVALID_PARAMETER;
            break;
    }

    return ret;
}

int amp_hash_update(amp_hash_ctx_t *ctx, const unsigned char *buf, unsigned int len)
{
    int ret = 0;
    if (ctx == NULL) {
        amp_encrypt_printf("Invalid hash param");
        return AMP_ENCRYPT_INVALID_PARAMETER;
    }

    switch (ctx->hash_method) {
        case AMP_HASH_SHA256:
            amp_sha256_update(&ctx->sha256_ctx, buf, len);
            break;
        case AMP_HASH_MD5:
            amp_md5_update(&ctx->md5_ctx, buf, len);
            break;
        default:
            ret = AMP_ENCRYPT_INVALID_PARAMETER;
            break;
    }

    return ret;
}

int amp_hash_final(amp_hash_ctx_t *ctx, unsigned char *dgst)
{
    int ret = 0;
    if (ctx == NULL) {
        amp_encrypt_printf("Invalid hash param");
        return AMP_ENCRYPT_INVALID_PARAMETER;
    }
    switch (ctx->hash_method) {
        case AMP_HASH_SHA256:
            amp_sha256_finish(&ctx->sha256_ctx, dgst);
            amp_sha256_free(&ctx->sha256_ctx);
            break;
        case AMP_HASH_MD5:
            amp_md5_finish(&ctx->md5_ctx, dgst);
            amp_md5_free(&ctx->md5_ctx);
            break;
        default:
            ret = AMP_ENCRYPT_INVALID_PARAMETER;
            break;
    }

    return ret;
}

// amp verify rsa hash value
// input:
//      src:        input string :      example:    static char input_text[] = "1234567890";
//      src_len :   input len :         example:    strlen(input_text)
//      dig:        output buf:         example:    dig = aos_malloc(dig_len);
//      type:       input type:         example:    AMP_HASH_SHA256
// result:
//      when cal finish, the input string "1234567890" will change to hash256 buf
//      dig :  c775e7b757ede630cd0aa1113bd102661ab38829ca52a6422ab782862f268646
int amp_verify_rsa_hash(unsigned char *src, int src_len, unsigned char *dig, unsigned char type)
{
    int ret             = 0;
    amp_hash_ctx_t ctx = {0};

    if (src == NULL || dig == NULL) {
        ret = AMP_ENCRYPT_VERIFY_SHA2_FAIL;
        goto EXIT;
    }

    ret = amp_hash_init(&ctx, type);
    if (ret != 0) {
        ret = AMP_ENCRYPT_VERIFY_SHA2_FAIL;
        goto EXIT;
    }

    ret = amp_hash_update(&ctx, (const unsigned char *)src, src_len);
    if (ret != 0) {
        ret = AMP_ENCRYPT_VERIFY_SHA2_FAIL;
        goto EXIT;
    }

    ret = amp_hash_final(&ctx, dig);
    if (ret != 0) {
        ret = AMP_ENCRYPT_VERIFY_SHA2_FAIL;
        goto EXIT;
    }

    return ret;

EXIT:
    amp_encrypt_printf("amp_verify_rsa_hash error ! ret=%d\n", ret);
    return ret;
}


// amp verify rsa sign value
// input:
//      dig_src:        input string :      example:    static char input_text[] = "1234567890";
//      sig_src :       input sig :         example:    static char sign_text[] = "ZbUUmlmmENOqFQm3EiWPl6wwTyllt/90eVgYwsheKvizdJFenulePQTiDBeOFSAWlcMQArT+5wK1e1e3eyhP4bBPFMgRGp7hN4rSdM3orj/obBr+pcB7DyNmeci/4wQgMSrIzk4kI11HvHacSC1WajZKBE/107raobfgrE0+Gxg=";
//      pub_key:        input buf:         example:    static char public_key[] = "-----BEGIN PUBLIC KEY-----\n"
// result:
//      when cal OK will return 0
int amp_verify_rsa_sign(unsigned char *dig_src, unsigned char *sig_src, unsigned char *pub_key)
{
    int                 ret     = 0;
    int                 dig_len = 0;
    unsigned char       *dig    = NULL;
    unsigned int        pubkey_n_size = 0;
    unsigned int        pubkey_e_size = 0;

    //======== step 1 : get public key n & e param from pub_key =======//

    // pub-key n
    char pubkey_n[ AMP_MPI_RW_BUFFER_SIZE ];
    amp_encrypt_memset( pubkey_n, 0, sizeof( pubkey_n ) );
    // pub-key e
    char pubkey_e[ AMP_MPI_RW_BUFFER_SIZE ];
    amp_encrypt_memset( pubkey_e, 0, sizeof( pubkey_e ) );
    // get pub-key value
    ret = amp_get_pubkey(pub_key, pubkey_n, sizeof(pubkey_n), pubkey_e, sizeof(pubkey_e));
    if(ret != 0) {
        ret = AMP_ENCRYPT_GET_PUBKEY_FAIL;
        amp_encrypt_printf("RSA verify err, amp get public key error, ret:%d\n", ret);
        goto EXIT;
    }
    // cal pubkey n & e size
    pubkey_n_size = strlen(pubkey_n);
    pubkey_e_size = strlen(pubkey_e);

#ifdef AMP_ENCRYPT_DEBUG_MESSAGE
    // check pubkey value 
    char *p = "N:  ";
    char *p1 = "E:  ";
    amp_encrypt_printf( "%s%s\n", p, pubkey_n );
    amp_encrypt_printf( "pubkey_n size=%d\n", pubkey_n_size );
    amp_encrypt_printf( "%s%s\n", p1, pubkey_e );
    amp_encrypt_printf( "pubkey_n size=%d\n", pubkey_e_size );
#endif

    //======== step 2 : change input string to hash256 buf =======//

    // sha256 change will make 32B lens buf
    dig_len = 32;
    dig = amp_encrypt_malloc(dig_len);

    if (NULL == dig) {
        ret = AMP_ENCRYPT_VERIFY_RSA_FAIL;
        amp_encrypt_printf("RSA verify err, dig len buf malloc error ret:%d\n", ret);
        goto EXIT;
    }
    // input_text hash change
    ret = amp_verify_rsa_hash(dig_src, strlen(dig_src), dig, AMP_HASH_SHA256);

    if (ret != 0) {
        ret = AMP_ENCRYPT_VERIFY_RSA_FAIL;
        amp_encrypt_printf("RSA verify err, input hash cal error ret:%d\n", ret);
        goto EXIT;
    }

#ifdef AMP_ENCRYPT_DEBUG_MESSAGE
    // Add log message for check dig buf
    amp_encrypt_printf("dig_len= %d\n", dig_len);
    for(int ii=0; ii<dig_len; ii++)
    {
        amp_encrypt_printf("%02x", dig[ii]);
    }
    amp_encrypt_printf("\n");
#endif

    //======== step 3 : change sig string to base64 decode =======//
    
    // init sig buf to save change result
    char sig[256] = {0};
    size_t sig_len;

    amp_base64_decode(sig, sizeof(sig), &sig_len, sig_src, strlen(sig_src));

#ifdef AMP_ENCRYPT_DEBUG_MESSAGE
    // Add log message for check sig buf
    amp_encrypt_printf("sig_len= %d\n", sig_len);
    for(int ii=0; ii<sig_len; ii++)
    {
        amp_encrypt_printf("%02x", sig[ii]);
    }
    amp_encrypt_printf("\n");
#endif

    //======== step 4 : sig verify =======//

    ret = amp_rsa_pubkey_verify(pubkey_n, pubkey_e, pubkey_n_size, pubkey_e_size, dig, dig_len, sig, sig_len);

EXIT:
    if (NULL != dig) {
        aos_free(dig);
        dig = NULL;
    }
    if (ret != 0)
        amp_encrypt_printf("RSA verify err:%d\n", ret);
    return ret;
}


int amp_encrypt_rsa(unsigned char *dig_src, unsigned char *pub_key)
{
    int ret = 0;

    // init sig buffer & sig len
    char sig[1024] = {0};
    size_t sig_len = sizeof(sig);
    size_t sig_olen;

    ret = amp_rsa_pubkey_encrypt(dig_src, sig, sig_len, &sig_olen, pub_key);
    if(ret != 0) {
        ret = AMP_ENCRYPT_PUBKEY_ENCRYPT_FAIL;
        return ret;
    }

// #ifdef AMP_ENCRYPT_DEBUG_MESSAGE
    amp_encrypt_printf("sig=%s\n", sig);
    amp_encrypt_printf("sig_len=%d\n", sig_olen);
// #endif

    return ret;
}