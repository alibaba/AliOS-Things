/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "amp_adapter_mbedtls.h"

/* SHA256 cal */
void amp_sha256_free(amp_sha256_context *ctx)
{
    mbedtls_sha256_free((mbedtls_sha256_context *)ctx);
}

void amp_sha256_init(amp_sha256_context *ctx)
{
    mbedtls_sha256_init((mbedtls_sha256_context *)ctx);
}

void amp_sha256_starts(amp_sha256_context *ctx, int is224)
{
    mbedtls_sha256_starts((mbedtls_sha256_context *)ctx, is224);
}

void amp_sha256_update(amp_sha256_context *ctx, const unsigned char *input, unsigned int ilen)
{
    mbedtls_sha256_update((mbedtls_sha256_context *)ctx, input, ilen);
}

void amp_sha256_finish(amp_sha256_context *ctx, unsigned char output[32])
{
    mbedtls_sha256_finish((mbedtls_sha256_context *)ctx, output);
}

/*MD5*/
void amp_md5_free(amp_md5_context *ctx)
{
    mbedtls_md5_free((mbedtls_md5_context *)ctx);
}

void amp_md5_init(amp_md5_context *ctx)
{
    mbedtls_md5_init((mbedtls_md5_context *)ctx);
}

void amp_md5_starts(amp_md5_context *ctx)
{
    mbedtls_md5_starts((mbedtls_md5_context *)ctx);
}

void amp_md5_update(amp_md5_context *ctx, const unsigned char *input, unsigned int ilen)
{
    mbedtls_md5_update((mbedtls_md5_context *)ctx, input, ilen);
}

void amp_md5_finish(amp_md5_context *ctx, unsigned char output[16])
{
    mbedtls_md5_finish((mbedtls_md5_context *)ctx, output);
}

/* base64 encode & decode */
int amp_base64_decode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen)
{
    return mbedtls_base64_decode(dst, dlen, olen, src, slen);
}

int amp_base64_encode(unsigned char *dst, size_t dlen, size_t *olen, const unsigned char *src, size_t slen)
{
    return mbedtls_base64_encode(dst, dlen, olen, src, slen);
}

// get pubkey function
int amp_get_pubkey(unsigned char *public_key, char *pub_N, size_t buflen_N, char *pub_E, size_t buflen_E)
{
    int ret = 0;
    size_t n;
    mbedtls_pk_context pk;
    mbedtls_mpi N, E;

    mbedtls_pk_init(&pk);
    mbedtls_mpi_init(&N);
    mbedtls_mpi_init(&E); 

    // parse public key
    ret = mbedtls_pk_parse_public_key( &pk, public_key, (strlen(public_key)+1));

    if(ret != 0) {
        ret = AMP_ENCRYPT_GET_PUBKEY_FAIL;
        amp_encrypt_printf("amp get public key error, ret =%d\n\n", ret);
        goto EXIT;
    }
    
    mbedtls_rsa_context *rsa = mbedtls_pk_rsa(pk);

    if((ret = mbedtls_rsa_export(rsa, &N, NULL, NULL, NULL, &E)) != 0 )
    {
        amp_encrypt_printf(" failed\n  ! could not export RSA parameters\n\n");
        goto EXIT;
    }

    // get pub N key
    ret = mbedtls_mpi_write_string(&N, 16, pub_N, buflen_N-2, &n);
    if(ret != 0) {
        ret = AMP_ENCRYPT_GET_PUBKEY_FAIL;
        amp_encrypt_printf("amp write public key n error, ret =%d\n\n", ret);
        goto EXIT;
    }
    // get pub E key
    ret = mbedtls_mpi_write_string(&E, 16, pub_E, buflen_E-2, &n);
    if(ret != 0) {
        ret = AMP_ENCRYPT_GET_PUBKEY_FAIL;
        amp_encrypt_printf("amp write public key e error, ret =%d\n\n", ret);
        goto EXIT;
    }

    return ret;

EXIT:
    amp_encrypt_printf("amp get public key error, ret =%d\n\n", ret);
    return ret;
}

int amp_rsa_pubkey_verify(const unsigned char *pubkey_n,
                          const unsigned char *pubkey_e,
                          unsigned int pubkey_n_size,
                          unsigned int pubkey_e_size,
                          const unsigned char *dig,
                          unsigned int dig_size,
                          const unsigned char *sig,
                          unsigned int sig_size)
{
    int                   ret = 0;
    mbedtls_rsa_context   ctx;

    // step 1: init mbedtls rsa context
    mbedtls_rsa_init(&ctx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_SHA256);

    // step 2: check input value
    if (pubkey_n == NULL || pubkey_n == NULL || dig == NULL || sig == NULL) {
        ret = AMP_ENCRYPT_INVALID_PARAMETER;
        goto EXIT;
    }

    if (pubkey_n_size == 0 || pubkey_e_size == 0 || sig_size == 0) {
        ret = AMP_ENCRYPT_INVALID_PARAMETER;
        goto EXIT;
    }

    // step 3: input publinc key n & e to mbedtls rsa context
    ret = mbedtls_mpi_read_string(&ctx.N, 16, pubkey_n);
    if (0 != ret) {
        goto EXIT;
    }

    ret = mbedtls_mpi_read_string(&ctx.E, 16, pubkey_e);
    if (0 != ret) {
        goto EXIT;
    }
    
    // step 4: cal ctx len
    ctx.len = ( mbedtls_mpi_bitlen( &ctx.N ) + 7 ) >> 3;

    // step 5: check public key
    ret = mbedtls_rsa_check_pubkey(&ctx);
    if (0 != ret) {
        goto EXIT;
    }

    // step 6: verify signal value
    ret = mbedtls_rsa_pkcs1_verify(&ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, (unsigned int)0, (const unsigned char *)dig, (const unsigned char *)sig);
    if (0 != ret) {
        goto EXIT;
    }

    amp_encrypt_printf("RSA verify OK!!\n");

EXIT:
    if (ret != 0) {
        amp_encrypt_printf("rsa verify ret: 0x%x\n", ret);
    }

    mbedtls_rsa_free(&ctx);

    return ret;
}


// amp verify rsa sign value
// input:
//      dig_src:        input string :      example:    static char input_text[] = "1234567890";
//      sig_src :       output str :        example:
//      sig_len :       output len :
//      pub_key:        input buf:         example:    static char public_key[] = "-----BEGIN PUBLIC KEY-----\n"
// result:
//      when cal OK will return 0
int amp_rsa_pubkey_encrypt(unsigned char *dig_src, unsigned char *sig_des, size_t sig_len, size_t *sig_olen, unsigned char *pub_key)
{
    FILE *f;
    int ret = 1;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    size_t i, olen = 0;
    mbedtls_pk_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    unsigned char input[1024];
    unsigned char buf[512];
    const char *pers = "mbedtls_pk_encrypt";

    // step 1: init mbedtls ctr drbg & entropy & pk value
    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );
    mbedtls_pk_init( &pk );

    amp_encrypt_printf( "Seeding the random number generator...\n" );

    // step 2: seed drbg value 
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func,
                                       &entropy, (const unsigned char *) pers,
                                       strlen( pers ) ) ) != 0 )
    {
        amp_encrypt_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned -0x%04x\n", -ret );
        goto exit;
    }

    // step 3: set public key value to entropy context
    // amp_encrypt_printf( "check pubkey = %s\n", pub_key);
    ret = mbedtls_pk_parse_public_key(&pk, pub_key, (strlen(pub_key)+1));


    // step 4: calculate the rsa encryption of the hash
    amp_encrypt_printf( "Generating the encrypted value\n" );
    // amp_encrypt_printf( "check input = %s\n", dig_src);
    if( ( ret = mbedtls_pk_encrypt( &pk, dig_src, strlen(dig_src),
                            buf, &olen, sizeof(buf),
                            mbedtls_ctr_drbg_random, &ctr_drbg ) ) != 0 )
    {
        amp_encrypt_printf( " failed\n  ! mbedtls_pk_encrypt returned -0x%04x\n", -ret );
        goto exit;
    }

    // step 5: do base64 change for calulate signature info
    // amp_encrypt_printf("==zzzzzz encrypt data ==\n\n");
    // for( i = 0; i < olen; i++ )
    // {
    //     amp_encrypt_printf("%02X", buf[i]);
    // }
    // amp_encrypt_printf("\n");

    // save string
    mbedtls_base64_encode(sig_des, sig_len, sig_olen, buf, 128);

    amp_encrypt_printf( "\n  . Pubkey encrypt Done)\n\n");

    exit_code = MBEDTLS_EXIT_SUCCESS;

exit:

    mbedtls_pk_free( &pk );
    mbedtls_entropy_free( &entropy );
    mbedtls_ctr_drbg_free( &ctr_drbg );

    return( exit_code );
}