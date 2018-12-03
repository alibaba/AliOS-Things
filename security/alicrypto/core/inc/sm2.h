/*
 * Copyright (C) 2018  Alibaba Group Holding Limited.
 */

#ifndef ALI_ALGO_SM2_H
#define ALI_ALGO_SM2_H

#include "config.h"
#include "ecp.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ALI_ALGO_SM2_ENC_C) || defined(ALI_ALGO_SM2_DH_C)
/**
 * \brief Compute key derive function
 * \param K            shared key to be returned
 * \param klen         the bit length of K
 * \param Z            the input data 
 * \param zlen         the byte length of Z
 * 
 */
void KDF( unsigned char *K, size_t klen, const unsigned char *Z, size_t zlen );
#endif /*ALI_ALGO_SM2_ENC_C || ALI_ALGO_SM2_DH_C */

#if defined(ALI_ALGO_SM2_SIGN_C)
int ali_algo_sm2_sign( ali_algo_ecp_group *grp, ali_algo_mpi *r, ali_algo_mpi *s,
                const ali_algo_mpi *d, const unsigned char *buf, size_t blen,
                int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

int ali_algo_sm2_verify( ali_algo_ecp_group *grp,
                  const unsigned char *buf, size_t blen,
                  const ali_algo_ecp_point *P, const ali_algo_mpi *r, const ali_algo_mpi *s);

#endif /* ALI_ALGO_SM2_SIGN_C */

#if defined(ALI_ALGO_SM2_ENCRYPT_C)
int ali_algo_sm2_encrypt( ali_algo_ecp_group *grp,
                         unsigned char *C, size_t *clen,
                         const ali_algo_ecp_point P,
                         const unsigned char *M, size_t mlen,
                         int (*f_rng)(void *, unsigned char *, size_t), void *p_rng );

int ali_algo_sm2_decrypt( ali_algo_ecp_group *grp,
                         unsigned char *M, size_t *mlen,
                         const unsigned char *C, size_t clen,
                         const ali_algo_mpi *d );

#endif /* ALI_ALGO_SM2_ENCRYPT_C */

#if defined(ALI_ALGO_SM2_DH_C)

int ali_algo_sm2dh_compute_shared( ali_algo_ecp_group *grp,
            ali_algo_mpi *K, const size_t secret_size,
            const ali_algo_mpi *ZA, const ali_algo_mpi *ZB,
            const ali_algo_mpi *dA, const ali_algo_mpi *rA,
            const ali_algo_ecp_point *RA,
            const ali_algo_ecp_point *RB,
            const ali_algo_ecp_point *PB );

#endif /* ALI_ALGO_SM2_DH_C */

#ifdef __cplusplus
}
#endif

#endif /* sm2.h */
