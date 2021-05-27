/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/
#ifndef __RTW_SAE_CRYPTO_MBEDTLS_H_
#define __RTW_SAE_CRYPTO_MBEDTLS_H_

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/ecp.h"
#include "mbedtls/sha256.h"

/************************************************************************************************

                  Structure definition

*************************************************************************************************/
#define sae_ecc_crypto mbedtls_ecp_group
#define sae_crypto_bignum mbedtls_mpi
#define sae_ecc_point mbedtls_ecp_point


/************************************************************************************************

                  Function Declaration

************************************************************************************************/
//Initialization Function
int rtw_platform_set_calloc_free( void * (*calloc_func)( size_t, size_t ), void (*free_func)( void * ) );
//ECC Alogorithm Function
int rtw_crypto_ecc_init(sae_ecc_crypto *ecc,unsigned char group_id);
void rtw_crypto_ecc_free(sae_ecc_crypto *ecc);
int rtw_crypto_ecc_get_param_a(sae_ecc_crypto *ecc, sae_crypto_bignum *a);
int rtw_crypto_ecc_get_param_b(sae_ecc_crypto *ecc, sae_crypto_bignum *b);
int rtw_crypto_ecc_get_param_order(sae_ecc_crypto *ecc, sae_crypto_bignum *n);
int rtw_crypto_ecc_get_param_prime(sae_ecc_crypto *ecc, sae_crypto_bignum *prime);
int rtw_crypto_ecc_point_write_bignum(sae_ecc_crypto *ecc,sae_crypto_bignum *x,sae_crypto_bignum *y,sae_ecc_point *p);
int rtw_crypto_ecc_point_read_bignum(sae_ecc_crypto *ecc,sae_crypto_bignum *x,sae_crypto_bignum *y,sae_ecc_point *p);
void rtw_crypto_ecc_point_init(sae_ecc_point *point);
void rtw_crypto_ecc_point_free(sae_ecc_point *point);
int rtw_crypto_ecc_point_mul_bignum(sae_ecc_crypto *ecc,sae_ecc_point *R,sae_crypto_bignum *m,sae_ecc_point *P);
int rtw_crypto_ecc_point_add_point(sae_ecc_crypto *ecc,sae_ecc_point *X,sae_ecc_point *A,sae_ecc_point *B);
int rtw_crypto_ecc_point_is_at_infinity(sae_ecc_point *point);
int rtw_crypto_ecc_point_is_on_curve(sae_ecc_crypto *ecc,sae_ecc_point *P);
int rtw_crypto_ecc_point_cmp(sae_ecc_point *P1,sae_ecc_point *P2);


//Big number operation and computatio Function
#define SAE_BIGNUM_CHK(f) do { if( ( ret = f ) != 0 ) goto cleanup; } while( 0 )
void rtw_crypto_bignum_init(sae_crypto_bignum *X);
void rtw_crypto_bignum_free(sae_crypto_bignum *X);
int rtw_crypto_bignum_copy(sae_crypto_bignum *X,sae_crypto_bignum *Y);
size_t rtw_crypto_bignum_bitlen(sae_crypto_bignum *X);
size_t rtw_crypto_bignum_get_bit(sae_crypto_bignum *X,size_t pos);
size_t rtw_crypto_bignum_size(sae_crypto_bignum *X);
int rtw_crypto_bignum_read_binary(sae_crypto_bignum *X, const unsigned char *buf, size_t buf_len);
int rtw_crypto_bignum_write_binary(sae_crypto_bignum *X,unsigned char *buf, size_t buf_len);
int rtw_crypto_bignum_shift_r(sae_crypto_bignum *X,size_t count);
int rtw_crypto_bignum_cmp_bignum(sae_crypto_bignum *X,sae_crypto_bignum *Y);
int rtw_crypto_bignum_cmp_int(sae_crypto_bignum *X,int z);
int rtw_crypto_bignum_add_bignum(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B);
int rtw_crypto_bignum_add_int(sae_crypto_bignum *X,sae_crypto_bignum *A,int b);
int rtw_crypto_bignum_sub_bignum(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B);
int rtw_crypto_bignum_sub_int(sae_crypto_bignum *X,sae_crypto_bignum *A,int b);
int rtw_crypto_bignum_div_bignum(sae_crypto_bignum *Q,sae_crypto_bignum *R,sae_crypto_bignum *A,sae_crypto_bignum *B);
int rtw_crypto_bignum_mod_bignum(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *N);
int rtw_crypto_bignum_add_mod(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B,sae_crypto_bignum *N);
int rtw_crypto_bignum_mul_mod(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B,sae_crypto_bignum *N);
int rtw_crypto_bignum_exp_mod(sae_crypto_bignum *X,sae_crypto_bignum *A,sae_crypto_bignum *B,sae_crypto_bignum *N);
int rtw_crypto_bignum_assign(sae_crypto_bignum *X, sae_crypto_bignum *Y,unsigned char inv);
int rtw_crypto_bignum_inverse(sae_crypto_bignum *X, sae_crypto_bignum *A,sae_crypto_bignum *N);
#endif
