/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#ifndef __IMPL_RAND_H__
#define __IMPL_RAND_H__
#include "ali_crypto_types.h"

/* HMAC */
typedef struct _rand_ops_t {
    ali_crypto_result (*rand_gen)(uint8_t *buf, size_t len);
    
    ali_crypto_result (*seed)(uint8_t *seed, size_t seed_len);
} rand_ops_t;

typedef struct _rand_impl_t {
	rand_ops_t        ops;
} rand_impl_t;

#define ALI_CRYPTO_RAND_IMPL_START(name)    static rand_impl_t rand_impl_vect_##name = {     

#define ALI_CRYPTO_RAND_IMPL(                   \
            _$$rand_gen,                        \
            _$$seed)                            \
    {                                           \
        _$$rand_gen,                            \
        _$$seed,                                \
    }

#define ALI_CRYPTO_RAND_IMPL_END()     };

//start
#define ALI_CRYPTO_RAND_GET_IMPL(name)                      \
rand_impl_t *ali_crypto_rand_get_impl()                     \
{                                                           \
    return &rand_impl_vect_##name;                          \
}                                                           \
//end





#endif //__IMPL_RAND_H__


