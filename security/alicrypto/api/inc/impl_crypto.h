/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#ifndef __IMPL_CRYPTO_H__
#define __IMPL_CRYPTO_H__
#include "impl_aes.h"
#include "impl_sm4.h"
#include "impl_hash.h"
#include "impl_hmac.h"
#include "impl_rsa.h"
#include "impl_sm2.h"
#include "impl_rand.h"


aes_impl_t *ali_crypto_aes_get_impl(aes_type_t type);

sm4_impl_t *ali_crypto_sm4_get_impl(sm4_type_t type);

hash_impl_t *ali_crypto_hash_get_impl(hash_type_t type);

hmac_impl_t *ali_crypto_hmac_get_impl(hash_type_t type);

rsa_impl_t *ali_crypto_rsa_get_impl();

sm2_impl_t *ali_crypto_sm2_get_impl();

rand_impl_t *ali_crypto_rand_get_impl();

#endif //__IMPL_CRYPTO_H__


