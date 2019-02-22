/**
 * Copyright (C) 2018  Alibaba Group Holding Limited.
 */
#include "sal_crypto.h"
#include "ali_crypto.h"

#ifdef ALI_CRYPTO_SM2
#define SM2_KEY_SIZE      (256)

static int myrand( void *rng_state, unsigned char *output, size_t len )
{
    int result;

    result = ali_rand_gen(output, len);
    if (result != ALI_CRYPTO_SUCCESS) {
        PRINT_RET(-1, "gen rand fail(%08x)\n", result);
    }
    return 0;
}


typedef struct _ali_ecc_keypair_t {
    size_t curve;                  /* elliptic curve  */
    unsigned char x[(SM2_KEY_SIZE>>3)]; /* x coordinate of public key */
    size_t x_size;       
    unsigned char y[(SM2_KEY_SIZE>>3)]; /* y coordinate of public key */
    size_t y_size;
    unsigned char d[(SM2_KEY_SIZE>>3)];       /* secret key */
    size_t d_size;
} ali_ecc_keypair_t;

typedef struct _ali_ecc_pubkey_t {
    size_t curve;                  /* elliptic curve  */
    unsigned char x[(SM2_KEY_SIZE>>3)]; /* x coordinate of public key */
    size_t x_size;       
    unsigned char y[(SM2_KEY_SIZE>>3)]; /* y coordinate of public key */
    size_t y_size;
} ali_ecc_pubkey_t;

ali_crypto_result sal_sm2_get_keypair_size(size_t keybits, size_t *size)
{
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                "get_keypair_size: invalid args!\n" );
    }

    if (keybits != SM2_KEY_SIZE ) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                "get_keypair_size: invalid keybits(%u)\n", keybits);
    }

    *size = sizeof(ali_ecc_keypair_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_sm2_get_pubkey_size(size_t keybits, size_t *size)
{
    if (size == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "get_pubkey_size: invalid args!\n");
    }

    if ( keybits != SM2_KEY_SIZE ) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG,
                "get_pubkey_size: invalid keybits(%u)\n", keybits);
    }

    *size = sizeof(ali_ecc_pubkey_t);

    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_sm2_init_keypair(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      const uint8_t *d, size_t d_size,
                      size_t curve, ecc_keypair_t *keypair)
{
    ali_ecc_keypair_t *sm2_key;
    if (keypair == NULL || 
        x == NULL || x_size == 0 ||
        y == NULL || y_size == 0 ||
        d == NULL || d_size == 0 ||
        curve == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Init_keypair: invalid args!\n");
    }

    sm2_key = (ali_ecc_keypair_t *)keypair;
    osa_memset(sm2_key, 0, sizeof(ali_ecc_keypair_t));
    sm2_key->x_size = x_size;
    osa_memcpy(sm2_key->x, x, x_size);
    sm2_key->y_size = y_size;
    osa_memcpy(sm2_key->y, y, y_size);
    sm2_key->d_size = d_size;
    osa_memcpy(sm2_key->d, d, d_size);
    sm2_key->curve = curve;
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_sm2_init_pubkey(
                      const uint8_t *x, size_t x_size,
                      const uint8_t *y, size_t y_size,
                      size_t curve, ecc_pubkey_t *pubkey)
{
    ali_ecc_pubkey_t *sm2_key;
    if (pubkey == NULL || 
        x == NULL || x_size == 0 ||
        y == NULL || y_size == 0 ||
        curve == 0) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Init_keypair: invalid args!\n");
    }

    sm2_key = (ali_ecc_pubkey_t *)pubkey;
    osa_memset(sm2_key, 0, sizeof(ali_ecc_pubkey_t));
    sm2_key->x_size = x_size;
    osa_memcpy(sm2_key->x, x, x_size);
    sm2_key->y_size = y_size;
    osa_memcpy(sm2_key->y, y, y_size);
    sm2_key->curve = curve;
    return ALI_CRYPTO_SUCCESS;
}

ali_crypto_result sal_sm2_gen_keypair(
                      size_t curve, ecc_keypair_t *keypair)
{
    ali_ecc_keypair_t *sm2_key;
    ali_algo_mpi d;
    ali_algo_ecp_point Q;
    ali_algo_ecp_group grp;
    int ret;
    uint8_t buf[(SM2_KEY_SIZE>>3)]; 
    size_t len;
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;
    unsigned char seed[32];

    if (curve == 0 || keypair == NULL) {
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "Init_keypair: invalid args!\n");
    }
    ali_algo_mpi_init( &d );
    ali_algo_ecp_group_init( &grp );
    ali_algo_ecp_group_load( &grp, curve );
    ali_algo_ecp_point_init( &Q );
    len = (SM2_KEY_SIZE>>3);
    ret = ali_seed(seed, 32); 
    if ( 0 != ret) {
        GO_RET(ret , "SM2 gen_keypair: generate random seed failed\n");
    }
    ret = ali_rand_gen(buf,len);
    if ( 0 != ret) {
        GO_RET(ret , "SM2 gen_keypair: generate secrete key failed\n");
    }
    ret = ali_algo_mpi_read_binary( &d, buf, len);
    if ( 0 != ret) {
        GO_RET(ret , "SM2 gen_keypair: read secrete key failed\n");
    }
	ret = ali_algo_mpi_mod_mpi(&d, &d, &(grp.N));
    if ( 0 != ret) {
        GO_RET(ret , "SM2 gen_keypair: d mod N failed\n");
    }
    ret = ali_algo_ecp_mul(&grp, &Q, &d, &(grp.G), NULL, NULL);
    if ( 0 != ret) {
        GO_RET(ret , "SM2 gen_keypair: point multiplication failed\n");
    }
    sm2_key = (ali_ecc_keypair_t *)keypair;
    osa_memset(sm2_key, 0, sizeof(ali_ecc_keypair_t));
    ret = ali_algo_mpi_write_binary( &d, sm2_key->d, len);
    if ( 0 != ret) {
        GO_RET(ret , "SM2 gen_keypair: write secrete key failed\n");
    }  //  sm2_key->d = d;   
    ret = ali_algo_mpi_write_binary( &Q.X, sm2_key->x, len);
    if ( 0 != ret) {
        GO_RET(ret , "SM2 gen_keypair: write public key failed\n");
    }  //  sm2_key->x = Q.X;
    ret = ali_algo_mpi_write_binary( &Q.Y, sm2_key->y, len);
    if ( 0 != ret) {
        GO_RET(ret , "SM2 gen_keypair: write public key failed\n");
    }  //  sm2_key->y = Q.Y;    
    sm2_key->d_size = len;
    sm2_key->x_size = len;
    sm2_key->y_size = len;

_OUT:
    ali_algo_ecp_point_free( &Q );
    ali_algo_ecp_group_free( &grp );
    ali_algo_mpi_free( &d );
    return result;
};

#ifdef ALI_ALGO_SM2_SIGN_C
ali_crypto_result sal_sm2_sign(const ecc_keypair_t *priv_key,
                      const uint8_t *src, size_t src_size,
                      uint8_t *signature, size_t *sig_size)
{
    int ret = 0;
    ali_algo_mpi *r, *s, *sk = NULL;
    ali_algo_ecp_group grp;
    ali_ecc_keypair_t *sm2_keypair;
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;
    uint32_t seed;

    if (priv_key == NULL ||
        src == NULL || src_size == 0 ||
        signature == NULL || (*sig_size) == 0){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sm2_sign: invalid input args!\n");
    }
    sm2_keypair = (ali_ecc_keypair_t *)priv_key;
    if (( r = osa_calloc(src_size, sizeof(ali_algo_mpi))) == NULL ){
        PRINT_RET(ALI_CRYPTO_OUTOFMEM, "SM2 sign: calloc (%u) fail\n", src_size);
    };
    if (( s = osa_calloc(src_size, sizeof(ali_algo_mpi))) == NULL ){
        GO_RET(ALI_CRYPTO_OUTOFMEM, "SM2 sign: calloc (%u) fail\n", src_size);
    };
    if (( sk = osa_calloc(src_size, sizeof(ali_algo_mpi))) == NULL ){
        GO_RET(ALI_CRYPTO_OUTOFMEM, "SM2 sign: calloc (%u) fail\n", src_size);
    };
    ali_algo_mpi_init( r );
    ali_algo_mpi_init( s );
    ali_algo_mpi_init( sk );
    ali_algo_ecp_group_init( &grp );

    ret = ali_algo_ecp_group_load(&grp, sm2_keypair->curve);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "SM2 sign: load group failed!\n");
    }
    ret = ali_algo_mpi_read_binary(sk, sm2_keypair->d, sm2_keypair->d_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "SM2 sign: input secret key is invalid!\n");
    }

    seed = (uint32_t)ls_osa_get_time_ms();
    ret = ali_seed((uint8_t *)&seed, 4);
    if ( 0 != ret) {
        GO_RET(ret, "SM2 sign: set seed failed!\n");
    }
    ret = ali_algo_sm2_sign( &grp, r, s, sk, (const unsigned char *)src, src_size, myrand, (uint32_t *)&seed );
    if ( 0 != ret) {
        GO_RET(ret, "SM2 sign: failed!\n");
    }
    osa_memset(signature, 0, *sig_size);
    ret = ali_algo_mpi_write_binary( r, signature, src_size);
    if ( 0 != ret) {
        GO_RET(ret, "SM2 sign: write signature failed!\n");
    }
    ret = ali_algo_mpi_write_binary( s, signature+src_size, src_size);
    if ( 0 != ret) {
        GO_RET(ret, "SM2 sign: write signature failed!\n");
    }
    *sig_size = src_size + src_size;

_OUT:
    ali_algo_ecp_group_free( &grp );
    ali_algo_mpi_free( sk );
    ali_algo_mpi_free( s );
    ali_algo_mpi_free( r );
    if (sk != NULL)  osa_free(sk);
    if (s  != NULL)  osa_free(s);
    if (r  != NULL)  osa_free(r);
    return result;
}

ali_crypto_result sal_sm2_verify(const ecc_pubkey_t *pub_key,
                      const uint8_t *src, size_t src_size,
                      const uint8_t *signature, size_t sig_size,
                      bool *p_result)
{
    int ret = 0;
    unsigned char one = 1;
    ali_algo_mpi *r,*s; 
    ali_algo_ecp_point Q;
    ali_algo_ecp_group grp;
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;

    if (pub_key == NULL ||
        src == NULL || src_size == 0 ||
        signature == NULL || sig_size == 0 ||
        p_result == NULL){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sm2_verify: invalid input args!\n");
    }

    if (( r = osa_calloc(src_size, sizeof(ali_algo_mpi))) == NULL ){
        *p_result = 0;
        PRINT_RET(ALI_CRYPTO_OUTOFMEM, "SM2 verify: calloc (%u) fail\n", src_size);
    };
    if (( s = osa_calloc(src_size, sizeof(ali_algo_mpi))) == NULL ){
        *p_result = 0;
        GO_RET(ALI_CRYPTO_OUTOFMEM, "SM2 verify: calloc (%u) fail\n", src_size);
    };

    ali_algo_mpi_init( r );
    ali_algo_mpi_init( s );
    ali_algo_ecp_point_init( &Q );
    ali_algo_ecp_group_init( &grp );

    ret = ali_algo_mpi_read_binary( r, signature, src_size);
    if ( ret != 0) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "SM2 verify: input signature is invalid!\n");
    }
    ret = ali_algo_mpi_read_binary( s, signature+src_size, src_size);
    if ( ret != 0) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "SM2 verify: input signature is invalid!\n");
    }

    ret = ali_algo_ecp_group_load( &grp, ((ali_ecc_pubkey_t *)pub_key)->curve);
    if ( ret != 0 ){
        GO_RET(ALI_CRYPTO_INVALID_ARG, "SM2 verify: load group failed!\n");
    }
    ali_algo_mpi_read_binary( &(Q.X), ((ali_ecc_pubkey_t *)pub_key)->x, ((ali_ecc_pubkey_t *)pub_key)->x_size );
    ali_algo_mpi_read_binary( &(Q.Y), ((ali_ecc_pubkey_t *)pub_key)->y, ((ali_ecc_pubkey_t *)pub_key)->y_size );
    ali_algo_mpi_read_binary( &(Q.Z), &one, 1 );

    ret = ali_algo_sm2_verify( &grp, (const unsigned char *)src, src_size, &Q, r, s);
    if ( ret != 0 ) {
        GO_RET(ALI_CRYPTO_INVALID_AUTHENTICATION, "SM2 verify: failed!\n");
    }

_OUT:
    if (result == 0 ) {
        *p_result = 1;
    } else {
        *p_result = 0;
    }
    ali_algo_ecp_group_free( &grp );
    ali_algo_ecp_point_free( &Q );
    ali_algo_mpi_free( s );
    ali_algo_mpi_free( r );
    if (s != NULL) osa_free(s);
    if (r != NULL) osa_free(r);
    return result;
};

#endif /* ALI_ALGO_SM2_SIGN_C */

#ifdef ALI_ALGO_SM2_ENCRYPT_C
ali_crypto_result sal_sm2_encrypt(const ecc_pubkey_t *pub_key,
                      const uint8_t *plaintext, size_t p_size,
                      uint8_t *ciphertext, size_t *c_size)
{
    int ret = 0;
    unsigned char one = 1;
    ali_algo_ecp_group grp;
    ali_algo_ecp_point P;
    ali_ecc_pubkey_t *sm2_pubkey;
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;
    uint32_t seed;

    if (pub_key == NULL || plaintext  == NULL || p_size == 0 ||
              (ciphertext == NULL && (*c_size) == 0)){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2_encrypt: invsald input args!\n");
    }

    sm2_pubkey = (ali_ecc_pubkey_t *)pub_key;
    ali_algo_ecp_point_init( &P );
    ali_algo_ecp_group_init( &grp );
    ret = ali_algo_ecp_group_load( &grp, sm2_pubkey->curve);
    if ( ret != 0 ){
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2_encrypt: load group failed!\n");
    }

    ali_algo_mpi_read_binary( &(P.X), sm2_pubkey->x, sm2_pubkey->x_size );
    ali_algo_mpi_read_binary( &(P.Y), sm2_pubkey->y, sm2_pubkey->y_size );
    ali_algo_mpi_read_binary( &(P.Z), &one, 1 );

    seed = (uint32_t)ls_osa_get_time_ms();
    ret = ali_seed((uint8_t *)&seed, 4);
    if ( 0 != ret) {
        GO_RET(ret, "SM2 sign: set seed failed!\n");
    }
    ret = ali_algo_sm2_encrypt( &grp, ciphertext, c_size, P, plaintext,
                        p_size, myrand, (uint32_t *)&seed );
    if ( ret != 0 ) {
        GO_RET(ret, "sal_sm2_encrypt: failed!\n");
    }

_OUT:
    ali_algo_ecp_group_free( &grp );
    ali_algo_ecp_point_free( &P );
    return( result );
}

ali_crypto_result sal_sm2_decrypt(const ecc_keypair_t *priv_key,
                      const uint8_t *ciphertext, size_t *c_size,
                      uint8_t *plaintext, size_t *p_size)
{
    int ret = 0;
    ali_algo_ecp_group grp;
    ali_algo_mpi sk;
    ali_ecc_keypair_t *sm2_keypair;
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;

    if (priv_key == NULL || ciphertext == NULL || (*c_size) == 0 ||
                (plaintext  == NULL && (*p_size) == 0)){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2_decrypt: invsald input args!\n");
    }

    ali_algo_mpi_init( &sk );
    ali_algo_ecp_group_init( &grp );

    sm2_keypair = (ali_ecc_keypair_t *)priv_key;
    ret = ali_algo_ecp_group_load(&grp, sm2_keypair->curve);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2_decrypt: load group failed!\n");
    }
    ret = ali_algo_mpi_read_binary(&sk, sm2_keypair->d, sm2_keypair->d_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2_decrypt: input secret key is invsald!\n");
    }

    ret =  ali_algo_sm2_decrypt( &grp, plaintext, p_size, ciphertext, *c_size, &sk );
    if ( 0 != ret ) {
        GO_RET(ret, "sal_sm2_decrypt: failed!\n");
    }

_OUT:
    ali_algo_ecp_group_free( &grp );
    ali_algo_mpi_free( &sk );
    return( result );
}

#endif /* ALI_ALGO_SM2_ENCRYPT_C */


#ifdef ALI_ALGO_SM2_DH_C

static ali_crypto_result H256(ali_algo_ecp_group *grp, const uint8_t *ID, size_t ID_size, const ali_algo_mpi *x_A, const ali_algo_mpi *y_A, ali_algo_mpi *Z)
{
    void *hash_ctx = NULL;
    size_t hash_ctx_size ;
    ali_crypto_result result;
    int ret = 0;
    unsigned char tmp[(SM2_KEY_SIZE)>>3];
    uint8_t hash[32], len;

    len = (SM2_KEY_SIZE)>>3;
    result = ali_hash_get_ctx_size(SM3, &hash_ctx_size);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        PRINT_RET(result, "get ctx size fail(%08x)\n", result);
    }
    hash_ctx = osa_malloc(hash_ctx_size);
    if (hash_ctx == NULL)
    {
        GO_RET(result, "malloc(%d) fail\n", (int)hash_ctx_size);
    }
    result = ali_hash_init(SM3, hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }
    tmp[0] = (unsigned char) ((ID_size & 0xFF00) >> 8);
    tmp[1] = (unsigned char) (ID_size & 0x00FF);

    result = ali_hash_update(tmp, 2, hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }

    result = ali_hash_update(ID, (ID_size >> 3), hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }

    ret = ali_algo_mpi_write_binary(&grp->A, tmp, len);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2dh_derive_secret: input elliptic curve parameter a is invalid!\n");
    }

    result = ali_hash_update(tmp, len, hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }

    ret = ali_algo_mpi_write_binary(&grp->B, tmp, len);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2dh_derive_secret: input elliptic curve parameter b is invalid!\n");
    }

    result = ali_hash_update(tmp, len, hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }

    ret = ali_algo_mpi_write_binary(&grp->G.X, tmp, len);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2dh_derive_secret: input x coordiante of point G is invalid!\n");
    }

    result = ali_hash_update(tmp, len, hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }

    ret = ali_algo_mpi_write_binary(&grp->G.Y, tmp, len);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2dh_derive_secret: input x coordiante of point G is invalid!\n");
    }

    result = ali_hash_update(tmp, len, hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }

    ret = ali_algo_mpi_write_binary(x_A, tmp, len);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2dh_derive_secret: input x coordiante of public key PA is invalid!\n");
    }

    result = ali_hash_update(tmp, len, hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }

    ret = ali_algo_mpi_write_binary(y_A, tmp, len);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2dh_derive_secret: input y coordiante of public key PA is invalid!\n");
    }

    result = ali_hash_update(tmp, len, hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }
    result = ali_hash_final(hash, hash_ctx);
    if (result != ALI_CRYPTO_SUCCESS)
    {
        GO_RET(result, "compute H256 fail(%08x)", result);
    }

    ret = ali_algo_mpi_read_binary(Z, hash, len);
    if ( 0 != ret) {
        GO_RET(-1, "sal_sm2dh_derive_secret: failed!\n");
    }
_OUT:
    osa_free(hash_ctx);
    return (result);
}

ali_crypto_result sal_sm2dh_derive_secret(const uint8_t flag_server, const uint8_t *ID, size_t ID_size, const uint8_t *peer_ID, size_t peer_ID_size, const ecc_keypair_t *priv_key, const ecc_keypair_t *tmp_priv_key, const ecc_pubkey_t *peer_pubkey, const ecc_pubkey_t *tmp_peer_pubkey, uint8_t *shared_secret, const size_t secret_size)
{
    ali_ecc_keypair_t *keypair, *tmp_keypair;
    ali_ecc_pubkey_t *pubkey, *tmp_pubkey;
    ali_algo_ecp_point PAB, RA, RB;
    ali_algo_mpi ZA, ZB, dA, rA, K, x, y;
    ali_crypto_result result = ALI_CRYPTO_SUCCESS;
    int ret = 0;
    ali_algo_ecp_group grp;

    if ((flag_server != 1 && flag_server != 0) ||
           ID == NULL || ID_size == 0 ||
           peer_ID == NULL || peer_ID_size == 0 ||
           priv_key == NULL || tmp_priv_key == NULL ||
           peer_pubkey == NULL || tmp_peer_pubkey == NULL ||
           shared_secret == NULL || secret_size == 0){
        PRINT_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: invalid input args!\n");
    }
    ali_algo_ecp_point_init( &PAB );
    ali_algo_ecp_point_init( &RA );
    ali_algo_ecp_point_init( &RB );
    ali_algo_mpi_init( &ZA );
    ali_algo_mpi_init( &ZB );
    ali_algo_mpi_init( &dA );
    ali_algo_mpi_init( &rA );
    ali_algo_mpi_init( &K );
    ali_algo_mpi_init( &x );
    ali_algo_mpi_init( &y );
    ali_algo_ecp_group_init( &grp );

    keypair = (ali_ecc_keypair_t *)priv_key;
    pubkey = (ali_ecc_pubkey_t *)peer_pubkey;
    tmp_keypair = (ali_ecc_keypair_t *)tmp_priv_key;
    tmp_pubkey = (ali_ecc_pubkey_t *)tmp_peer_pubkey;
    ret = ali_algo_ecp_group_load( &grp, keypair->curve );
    if( ret != 0 ) {
        GO_RET(-1, " failed! ali_algo_ecp_group_load returned %d\n", ret );
    }

    //compute ZA for client or ZB for server
    ret = ali_algo_mpi_read_binary(&x, keypair->x, keypair->x_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2dh_derive_secret: input x coordiante of private key is invalid!\n");
    }
    ret = ali_algo_mpi_read_binary(&y, keypair->y, keypair->y_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "sal_sm2dh_derive_secret: input y coordiante of private key is invalid!\n");
    }
    if (flag_server){
        H256(&grp, ID, ID_size, &x, &y, &ZA);
    }else{
        H256(&grp, ID, ID_size, &x, &y, &ZB);
    }

    //compute ZB for client or ZA for server
    ret = ali_algo_mpi_read_binary(&x, pubkey->x, pubkey->x_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input x coordiante of peer pubkey is invalid!\n");
    }
    ret = ali_algo_mpi_read_binary(&y, pubkey->y, pubkey->y_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input y coordiante of peer pubkey is invalid!\n");
    }
    if (flag_server){
        H256(&grp, peer_ID, peer_ID_size, &x, &y, &ZB);
    }else{
        H256(&grp, peer_ID, peer_ID_size, &x, &y, &ZA);
    }
    // write dA
    ret = ali_algo_mpi_read_binary(&dA, keypair->d, keypair->d_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input priv key is invalid!\n");
    }
        // write rA
    ret = ali_algo_mpi_read_binary(&rA, tmp_keypair->d, tmp_keypair->d_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input tmp priv key is invalid!\n");
    }
        // write RA
    ret = ali_algo_mpi_read_binary(&(RA.X), tmp_keypair->x, tmp_keypair->x_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input tmp priv key is invalid!\n");
    }
    ret = ali_algo_mpi_read_binary(&(RA.Y), tmp_keypair->y, tmp_keypair->y_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input tmp priv key is invalid!\n");
    }
    ret = ali_algo_mpi_lset( &(RA.Z), 1 );
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: init Z coordinate of point RA failed!\n");
    }
        // write RB
    ret = ali_algo_mpi_read_binary(&(RB.X), tmp_pubkey->x, tmp_pubkey->x_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input tmp pub key is invalid!\n");
    }
    ret = ali_algo_mpi_read_binary(&(RB.Y), tmp_pubkey->y, tmp_pubkey->y_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input tmp pub key is invalid!\n");
    }

    ret = ali_algo_mpi_lset( &(RB.Z), 1 );
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: init Z coordinate of point RB failed!\n");
    }
        //write PAB
    ret = ali_algo_mpi_read_binary(&(PAB.X), pubkey->x, pubkey->x_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input pub key is invalid!\n");
    }
    ret = ali_algo_mpi_read_binary(&(PAB.Y), pubkey->y, pubkey->y_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input pub key is invalid!\n");
    }
    ret = ali_algo_mpi_lset( &(PAB.Z), 1 );
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: init Z coordinate of point PA or PB failed!\n");
    }
        // compute share secret
    ret = ali_algo_sm2dh_compute_shared( &grp, &K, secret_size, &ZA, &ZB, &dA, &rA, &RA, &RB, &PAB );

    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: compute ali_algo_sm2dh_compute_shared failed!\n");
    }
    ret = ali_algo_mpi_write_binary(&K, shared_secret, secret_size);
    if ( 0 != ret) {
        GO_RET(ALI_CRYPTO_INVALID_ARG, "ali_sm2dh_derive_secret: input pub key is invalid!\n");
    }

_OUT:
    ali_algo_ecp_group_free( &grp );
    ali_algo_mpi_free( &y );
    ali_algo_mpi_free( &x );
    ali_algo_mpi_free( &K );
    ali_algo_mpi_free( &rA );
    ali_algo_mpi_free( &dA );
    ali_algo_mpi_free( &ZB );
    ali_algo_mpi_free( &ZA );
    ali_algo_ecp_point_free( &RB );
    ali_algo_ecp_point_free( &RA );
    ali_algo_ecp_point_free( &PAB );
    return ( result );
}

#endif /* ALI_ALGO_SM2_DH_C */
#endif
