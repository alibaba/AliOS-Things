/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#ifndef __IMPL_HMAC_H__
#define __IMPL_HMAC_H__
#include "ali_crypto_types.h"

/* HMAC */
typedef struct _hmac_ops_t {
    ali_crypto_result (*hmac_get_ctx_size)(hash_type_t type, size_t *size);
    
    ali_crypto_result (*hmac_init)(hash_type_t type, const uint8_t *key,
                                    size_t keybytes, void *context);
    
    ali_crypto_result (*hmac_update)(const uint8_t *src, size_t size, void *context);
    
    ali_crypto_result (*hmac_final)(uint8_t *dgst, void *context);
    
    ali_crypto_result (*hmac_reset)(void *context);
} hmac_ops_t;

typedef struct _hmac_impl_t {
	hash_type_t       type;
	hmac_ops_t        ops;
} hmac_impl_t;

#define ALI_CRYPTO_HMAC_IMPL_START(name)    static hmac_impl_t hmac_impl_vect_##name[] = {     

#define ALI_CRYPTO_HMAC_IMPL(type,                   \
            _$$hmac$$_$$get$$_$$ctx$$_$$size$$,      \
            _$$hmac$$_$$init$$,                      \
            _$$hmac$$_$$update$$,                   \
            _$$hmac$$_$$final$$,                    \
            _$$hmac$$_$$reset$$)                     \
    {                                               \
        type,                                       \
        {_$$hmac$$_$$get$$_$$ctx$$_$$size$$,         \
        _$$hmac$$_$$init$$,                          \
        _$$hmac$$_$$update$$,                       \
        _$$hmac$$_$$final$$,                        \
        _$$hmac$$_$$reset$$}                         \
    },


#define ALI_CRYPTO_HMAC_IMPL_END()     };

//start
#define ALI_CRYPTO_HMAC_GET_IMPL(name)                      \
hmac_impl_t *ali_crypto_hmac_get_impl(hash_type_t type)     \
{                                                           \
    uint32_t count = 0;                                     \
    uint32_t i = 0;                                         \
    uint32_t find = 0;                                      \
    hmac_impl_t *p_impl = NULL;                             \
    count =                                                 \
       sizeof(hmac_impl_vect_##name)/sizeof(hmac_impl_t);   \
    for(i = 0; i < count; i++) {                            \
        p_impl = &hmac_impl_vect_##name[i];                 \
        if(p_impl->type == type) {                          \
            find = 1;                                       \
            break;                                          \
        }                                                   \
    }                                                       \
    if(0 == find) {                                         \
        p_impl = NULL;                                      \
    }                                                       \
    return p_impl;                                          \
}                                                           \
//end





#endif //__IMPL_HMAC_H__


