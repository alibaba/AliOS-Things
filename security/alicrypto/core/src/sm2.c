/**
* Copyright (C) 2018  Alibaba Group Holding Limited.
**/

#include "config.h"
#include "osa.h"

#if defined(ALI_ALGO_SM2_C)
#include "sm2.h"
#include "ecp.h"

#if defined(ALI_ALGO_SM2_ENCRYPT_C) || defined(ALI_ALGO_SM2_DH_C)
#include "sm3.h"

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
do {                                                    \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
} while( 0 )
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
do {                                                    \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
} while( 0 )
#endif
 
/*
 * Key derive function
 */
void KDF( unsigned char *K, size_t klen, const unsigned char *Z, size_t zlen )
{
    uint32_t ct = 1;
    uint8_t i, s;
	uint16_t v = 32;    //since SM3 is used, v is 32 bytes
    size_t m,n;
    ali_algo_sm3_context ctx0,ctx1;
    unsigned char tmp[4], output[32];
   
    ali_algo_sm3_init( &ctx0 );
    ali_algo_sm3_init( &ctx1 );
    ali_algo_sm3_starts( &ctx0 );
    ali_algo_sm3_update( &ctx0, Z, zlen);
    m = klen % v;
    if ( m == 0)
    {
        n = klen / v;   
    } else {
        n = (klen-m) / v + 1 ;
    }
    for ( i = 0; i < n-1; i++ )
    {   
        ali_algo_sm3_clone( &(ctx1), &(ctx0) );
        PUT_UINT32_BE( ct, tmp, 0 );
        ali_algo_sm3_update( &(ctx1), tmp, 4 );
        ali_algo_sm3_finish( &(ctx1), output );
		for( s=0; s < 32; s++)
		{
			K[i*32+s] = output[s];
		}
        ct++;
    }
    ali_algo_sm3_clone( &(ctx1), &(ctx0) );
    PUT_UINT32_BE( ct, tmp, 0 );
    ali_algo_sm3_update( &(ctx1), tmp, 4 );
    ali_algo_sm3_finish( &(ctx1), output );
    if ( m == 0 ) {
		for( s=0; s < 32; s++)
		{
			K[(n-1)*32+s] = output[s];
		}
	} else {
        i = klen - v * (n-1);
		for (s=0; s<i; s++)
		{
			K[(n-1)*32+s] = output[s];
		}
		
    }
    ali_algo_sm3_free( &ctx0 );
    ali_algo_sm3_free( &ctx1 );
}
#endif  /* ALI_ALGO_SM2_ENCRYPT_C  || ALI_ALGO_SM2_DH_C */

#if defined(ALI_ALGO_SM2_SIGN_C)
/*
 * Derive a suitable integer for group grp from a buffer of length len
 * SEC1 4.1.3 step 5 aka SEC1 4.1.4 step 3
 */
static int derive_mpi( const ali_algo_ecp_group *grp, ali_algo_mpi *x,
                       const unsigned char *buf, size_t blen )
{
    int ret;
    size_t n_size = ( grp->nbits + 7 ) / 8;
    size_t use_size = blen > n_size ? n_size : blen;

    ALI_ALGO_MPI_CHK( ali_algo_mpi_read_binary( x, buf, use_size ) );
    if( use_size * 8 > grp->nbits )
        ALI_ALGO_MPI_CHK( ali_algo_mpi_shift_r( x, use_size * 8 - grp->nbits ) );

    /* While at it, reduce modulo N */
    if( ali_algo_mpi_cmp_mpi( x, &grp->N ) >= 0 )
        ALI_ALGO_MPI_CHK( ali_algo_mpi_sub_mpi( x, x, &grp->N ) );

cleanup:
    return( ret );
}

/*
 * Compute SM2_SIGN signature of a hashed message 
 * Obviously, compared to the SM2 standard, we skip step A1 and A2 (hash message)
 */
int ali_algo_sm2_sign( ali_algo_ecp_group *grp, ali_algo_mpi *r,
        ali_algo_mpi *s, const ali_algo_mpi *d,
        const unsigned char *buf, size_t blen,
        int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    int ret, key_tries, sign_tries;
    ali_algo_ecp_point Q;
    ali_algo_mpi k, e;

    if( grp->N.p == NULL )
        return( ALI_ALGO_ERR_ECP_BAD_INPUT_DATA );

    ali_algo_ecp_point_init( &Q );
    ali_algo_mpi_init( &k );
    ali_algo_mpi_init( &e );

    sign_tries = 0;
    do
    {
        /*
         * derive MPI from hashed message
         */
        ALI_ALGO_MPI_CHK( derive_mpi( grp, &e, buf, blen ) );
        /*
         * Steps A3-A5: generate a suitable ephemeral keypair k and Q=[k]G
         * and set r = (e+x1) mod n
         */
        key_tries = 0;
        do
        {
            ALI_ALGO_MPI_CHK( ali_algo_ecp_gen_keypair( grp, &k, &Q, f_rng, p_rng ) );
            ALI_ALGO_MPI_CHK( ali_algo_mpi_add_mpi( r, &e, &(Q.X)) );
            /* reduce modulo N */
            if( ali_algo_mpi_cmp_mpi( r, &grp->N ) >= 0 )
                ALI_ALGO_MPI_CHK( ali_algo_mpi_sub_mpi( r, r, &grp->N ) );

            if( key_tries++ > 10 )
            {
                ret = ALI_ALGO_ERR_ECP_RANDOM_FAILED;
                goto cleanup;
            }
            ALI_ALGO_MPI_CHK( ali_algo_mpi_add_mpi( &e, r, &k ) );
        }
        while( ali_algo_mpi_cmp_int( r, 0 ) == 0 || ali_algo_mpi_cmp_mpi( &e, &grp->N ) == 0 );
        /*
         * Step A6: compute s = (1 + d)^(-1) *(k - r * d)  mod n
         */

        ALI_ALGO_MPI_CHK( ali_algo_mpi_mul_mpi( s, r, d ) );
        ALI_ALGO_MPI_CHK( ali_algo_mpi_sub_mpi( s, &k, s ) );
        ALI_ALGO_MPI_CHK( ali_algo_mpi_add_int( &e, d, 1 ) );
        ALI_ALGO_MPI_CHK( ali_algo_mpi_inv_mod( &e, &e, &grp->N ) );
        ALI_ALGO_MPI_CHK( ali_algo_mpi_mul_mpi( s, s, &e ) );
        ALI_ALGO_MPI_CHK( ali_algo_mpi_mod_mpi( s, s, &grp->N ) );

        if( sign_tries++ > 10 )
        {
            ret = ALI_ALGO_ERR_ECP_RANDOM_FAILED;
            goto cleanup;
        }
    }
    while( ali_algo_mpi_cmp_int( s, 0 ) == 0 );

cleanup:
    ali_algo_mpi_free( &e );
    ali_algo_mpi_free( &k ); 
    ali_algo_ecp_point_free( &Q );

    return( ret );
}

/*
 * Verify SM2_SIGN signature of hashed message 
 * Obviously, compared to the SM2 standard, we skip step B3-B4 (hash message)
 */
int ali_algo_sm2_verify( ali_algo_ecp_group *grp,
                  const unsigned char *buf, size_t blen,
                  const ali_algo_ecp_point *P, const ali_algo_mpi *r, const ali_algo_mpi *s)
{
    int ret = 0;
    ali_algo_mpi e, t, R;
    ali_algo_ecp_point Q;

    ali_algo_mpi_init( &e );
    ali_algo_ecp_point_init( &Q );
    ali_algo_mpi_init( &R );
    ali_algo_mpi_init( &t );

    if( grp->N.p == NULL )
        return( ALI_ALGO_ERR_ECP_BAD_INPUT_DATA );
    /*
     * Step B1-B2: make sure r and s are in range [1,n-1]
     */
    if( ali_algo_mpi_cmp_int( r, 1 ) < 0 || ali_algo_mpi_cmp_mpi( r, &grp->N ) >= 0 ||
        ali_algo_mpi_cmp_int( s, 1 ) < 0 || ali_algo_mpi_cmp_mpi( s, &grp->N ) >= 0 )
    {
        ret = ALI_ALGO_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    /*
     * Additional precaution: make sure P is valid
     */
    ALI_ALGO_MPI_CHK( ali_algo_ecp_check_pubkey( grp, P ) );

    /*
     * derive MPI from hashed message
     */
    ALI_ALGO_MPI_CHK( derive_mpi( grp, &e, buf, blen ) );

    /*
     * Step B5: t = (r + s) mod n
     */
    ALI_ALGO_MPI_CHK( ali_algo_mpi_add_mpi( &t, r, s ) );
    ALI_ALGO_MPI_CHK( ali_algo_mpi_mod_mpi( &t, &t, &grp->N ) );
        if( ali_algo_mpi_cmp_int( &t, 0 ) == 0 )
    {
        ret = ALI_ALGO_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }
    /*
     * Step B6: Q = s*G + t*P
     *
     */
    ALI_ALGO_MPI_CHK( ali_algo_ecp_muladd( grp, &Q, s, &grp->G, &t, P ) );
    if( ali_algo_ecp_is_zero( &Q ) )
    {
        ret = ALI_ALGO_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }
        /*
         * Step B7: R = ( e + x ) mod n and check if R is equal to r
         */
    ALI_ALGO_MPI_CHK( ali_algo_mpi_add_mpi( &R, &e, &Q.X ) );
    ALI_ALGO_MPI_CHK( ali_algo_mpi_mod_mpi( &R, &R, &grp->N ) );
    if( ali_algo_mpi_cmp_mpi( &R, r ) != 0 )
    {
        ret = ALI_ALGO_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

cleanup:
    ali_algo_mpi_free( &t );
    ali_algo_mpi_free( &R );
    ali_algo_ecp_point_free( &Q );
    ali_algo_mpi_free( &e );
    return( ret );
}

#endif /* ALI_ALGO_SM2_SIGN_C */

#if defined(ALI_ALGO_SM2_ENCRYPT_C)
/*
 * SM2 encryption
 */

int ali_algo_sm2_encrypt( ali_algo_ecp_group *grp,
                         unsigned char *C, size_t *clen,
                         const ali_algo_ecp_point P,
                         const unsigned char *M, size_t mlen,
                         int (*f_rng)(void *, unsigned char *, size_t), void *p_rng )
{
    ali_algo_ecp_point S, C1;
    ali_algo_mpi k;
    unsigned char *tmp, *Ctmp, *t ;
    size_t C1len, len;
    uint8_t flag = 0;
    ali_algo_sm3_context ctx;
    uint32_t i, ret = 0;

    if( grp->N.p == NULL )
        return( ALI_ALGO_ERR_ECP_BAD_INPUT_DATA );
    ali_algo_ecp_point_init( &S );
    ali_algo_ecp_point_init( &C1 );
    ali_algo_mpi_init( &k );
    tmp = osa_calloc(64, sizeof(unsigned char));
    t = osa_calloc(mlen, sizeof(unsigned char));
    ali_algo_sm3_init( &ctx );

    len = 32; //since the length of X,Y coordinates of SM2 points is 32 bytes
    do{
            /*
         * Steps A1-A2: generate a random number k and compute C1=[k]G
         */
        ALI_ALGO_MPI_CHK( ali_algo_ecp_gen_keypair( grp, &k, &C1, f_rng, p_rng ));

                /* skip S=[h]P, since h=1 for given parameters in the SM2 standard  */
        /* compute (x2,y2) = [k]P     */
        ALI_ALGO_MPI_CHK( ali_algo_ecp_mul( grp, &S, &k, &P, NULL, NULL ) );
        ALI_ALGO_MPI_CHK( ali_algo_mpi_write_binary( &(S.X), tmp, len ));
        ALI_ALGO_MPI_CHK( ali_algo_mpi_write_binary( &(S.Y), tmp+len, len ));
        KDF(t, mlen, tmp, len+len);

        for( i=0; i < mlen; i++ )
        {
            if(t[i] != 0)
            {
                flag = 1;
                break;
            }
        }
    }while(!flag);
    /* C = C1  */
    ALI_ALGO_MPI_CHK( ali_algo_ecp_point_write_binary( grp, &C1, ALI_ALGO_ECP_PF_UNCOMPRESSED, &C1len, C, *(clen) ));

    /* C2 = M ^ t and C = C1 || C2  */
    Ctmp = C + C1len;
    for( i=0; i < mlen; i++ )
    {
         *(Ctmp + i) = (M[i]) ^ (t[i]);
    }
    Ctmp += mlen;
    /* C3 = Hash(x2||M||y2) */
    ali_algo_sm3_starts( &ctx );
    ali_algo_sm3_update( &ctx, tmp, len);
    ali_algo_sm3_update( &ctx, M, mlen );
    ali_algo_sm3_update( &ctx, tmp+len, len);
    /* C = C1||C2||C3 */
    ali_algo_sm3_finish( &ctx, Ctmp );
    *clen = C1len + mlen + 32;   //since the output length of sm3 is 32 bytes
cleanup:
    ali_algo_sm3_free( &ctx );
    osa_free( t );
    osa_free( tmp );
    ali_algo_mpi_free( &k );
    ali_algo_ecp_point_free( &C1 );
    ali_algo_ecp_point_free( &S );
    return ( ret );
}

/* SM2 decryption */
int ali_algo_sm2_decrypt( ali_algo_ecp_group *grp,
                         unsigned char *M, size_t *mlen,
                         const unsigned char *C, size_t clen,
                         const ali_algo_mpi *d )
{
    size_t C1len, len = 32;
    ali_algo_ecp_point S, C1;
    uint32_t i, ret = 0;
    unsigned char tmp[64], *t;
    const unsigned char *Ctmp;
    ali_algo_sm3_context ctx;

    ali_algo_ecp_point_init( &C1 );
    ali_algo_ecp_point_init( &S );
    t = osa_calloc((*mlen), sizeof(unsigned char));
    ali_algo_sm3_init( &ctx );
    C1len = 65;  // only support uncompressed point
    ALI_ALGO_MPI_CHK(ali_algo_ecp_point_read_binary(grp, &C1, C, C1len));
    ALI_ALGO_MPI_CHK( ali_algo_ecp_check_pubkey( grp, &C1 ) );
    /* skip S=[h]C1, since h=1 for given parameters in the SM2 standard  */
    /* compute (x2,y2) = [d]C1     */

    ALI_ALGO_MPI_CHK( ali_algo_ecp_mul( grp, &S, d, &C1, NULL, NULL ) );

    ALI_ALGO_MPI_CHK( ali_algo_mpi_write_binary( &(S.X), tmp, len ));
    ALI_ALGO_MPI_CHK( ali_algo_mpi_write_binary( &(S.Y), tmp+len, len ));
    /* t = KDF(x2||y2, klen)  */
    *mlen = clen - 97;  // clen - len(C1) - len(C3)

    KDF(t, *mlen, tmp, len+len);
    for( i=0; i < (*mlen); i++ )
    {
        if(t[i] != 0) break;
    }
    if (i == (*mlen))
    {
        ret = ALI_ALGO_ERR_ECP_DECRYPT_FAILED;
        goto cleanup;
    }
    Ctmp = C + C1len;
    for( i = 0; i < (*mlen); i++ )
    {
        M[i] = (Ctmp[i]) ^ (t[i]);
    }
    /* u = Hash(x2||M||y2) */
    ali_algo_sm3_starts( &ctx );
    ali_algo_sm3_update( &ctx, tmp, len);
    ali_algo_sm3_update( &ctx, M, (*mlen) );
    ali_algo_sm3_update( &ctx, tmp+len, len);
    ali_algo_sm3_finish( &ctx, tmp );

    Ctmp = Ctmp + (*mlen);
    for( i = 0; i < 32; i++ ) // since the output of SM3 is 32 bytes
    {
        if(tmp[i] != Ctmp[i]) break;
    }
    if( i != 32 )
    {
        ret = ALI_ALGO_ERR_ECP_DECRYPT_FAILED;
        goto cleanup;
    }

cleanup:
    ali_algo_sm3_free( &ctx );
    osa_free( t );
    ali_algo_ecp_point_free( &S );
    ali_algo_ecp_point_free( &C1 );
    return ( ret );
}

#endif /* ALI_ALGO_SM2_ENCRYPT_C */


#if defined(ALI_ALGO_SM2_DH_C)
/*
 * Compute shared secret
 */
int ali_algo_sm2dh_compute_shared( ali_algo_ecp_group *grp, ali_algo_mpi *K, const size_t secret_size, const ali_algo_mpi *ZA, const ali_algo_mpi *ZB, const ali_algo_mpi *dA, const ali_algo_mpi *rA, const ali_algo_ecp_point *RA, const ali_algo_ecp_point *RB, const ali_algo_ecp_point *PB )
{
    int ret;
    uint8_t w, i;
    size_t tmp, len;
    ali_algo_mpi bX, tA;
    ali_algo_ecp_point U;
    unsigned char *buf = NULL, *K0 = NULL;

    ali_algo_mpi_init( &bX );
    ali_algo_mpi_init( &tA );
    ali_algo_ecp_point_init( &U );
    /* compute w=ceil(N/2)-1  */
    tmp = ali_algo_mpi_bitlen(&(grp->N));

    if ((tmp % 2) == 0)
    { w = tmp/2 - 1; }
    else
    { w = (tmp + 1)/2 - 1; }
    //  \bar{x_1}=2^w+(x1 & (2^w-1))
    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset( &bX, 1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_l( &bX, w));
    for(i=0; i< bX.n; i++){
            bX.p[i] = RA->X.p[i];
    }
    ALI_ALGO_MPI_CHK(ali_algo_mpi_set_bit( &bX, w, 1));

    //  t_A=(d_A+\bar{x_1}\cdot r_A) mod n
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&tA, &bX, rA));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_add_mpi(&tA, &tA, dA));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&tA, &tA, &(grp->N)));

    //  \bar{x_2}=2^w+(x2 & (2^w-1))
    ALI_ALGO_MPI_CHK(ali_algo_mpi_lset( &bX, 1));
    ALI_ALGO_MPI_CHK(ali_algo_mpi_shift_l( &bX, w));
    for(i=0; i< bX.n; i++){
            bX.p[i] = RB->X.p[i];
    }
    ALI_ALGO_MPI_CHK(ali_algo_mpi_set_bit( &bX, w, 1));

   // U=[h\cdot tA](PB+[\bar{x2}]RB)
   ALI_ALGO_MPI_CHK(ali_algo_mpi_mul_mpi(&bX, &tA, &bX));
   ALI_ALGO_MPI_CHK(ali_algo_mpi_mod_mpi(&bX, &bX, &(grp->N)));
   ALI_ALGO_MPI_CHK(ali_algo_ecp_muladd(grp, &U, &tA, PB, &bX, RB));
   if( ali_algo_ecp_is_zero( &U ))
   {
       ret = ALI_ALGO_ERR_ECP_DH_FAILED;
       goto cleanup;
   }

   tmp = ali_algo_mpi_size(&U.X);
   len = tmp + ali_algo_mpi_size(&U.Y);
   len += ali_algo_mpi_size(ZA) + ali_algo_mpi_size(ZB);
   buf = osa_calloc(len, sizeof(unsigned char));
   ALI_ALGO_MPI_CHK(ali_algo_mpi_write_binary( &U.X, buf, tmp ));
   len = tmp;
   tmp = ali_algo_mpi_size(&U.Y);
   ALI_ALGO_MPI_CHK(ali_algo_mpi_write_binary( &U.Y, buf+len, tmp ));
   len += tmp;
   tmp = ali_algo_mpi_size(ZA);
   ALI_ALGO_MPI_CHK(ali_algo_mpi_write_binary( ZA, buf+len, tmp ));
   len += tmp;
   tmp = ali_algo_mpi_size(ZB);
   ALI_ALGO_MPI_CHK(ali_algo_mpi_write_binary( ZB, buf+len, tmp ));

   len += tmp;
   K0  = osa_calloc(secret_size, sizeof(unsigned char));
   KDF(K0, secret_size, buf, len);
   ALI_ALGO_MPI_CHK(ali_algo_mpi_read_binary( K, K0, secret_size ));

cleanup:
   osa_free( K0 );
   osa_free( buf );
   ali_algo_ecp_point_free( &U );
   ali_algo_mpi_free( &tA );
   ali_algo_mpi_free( &bX );
   return(ret);
}

#endif /* ALI_ALGO_SM2_DH_C  */

#endif /* ALI_ALGO_SM2_C */
