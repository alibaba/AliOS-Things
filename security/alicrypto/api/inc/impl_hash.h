/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#ifndef __IMPL_HASH_H__
#define __IMPL_HASH_H__
#include "ali_crypto_types.h"

/* HASH */
typedef struct _hash_ops_t {
    ali_crypto_result (*hash_get_ctx_size)(hash_type_t type, size_t *size);
    
    ali_crypto_result (*hash_init)(hash_type_t type, void *context);
    
    ali_crypto_result (*hash_update)(const uint8_t *src, size_t size, void *context);
    
    ali_crypto_result (*hash_final)(uint8_t *dgst, void *context);
    
    ali_crypto_result (*hash_reset)(void *context);
} hash_ops_t;

typedef struct _hash_impl_t {
	hash_type_t       type;
	hash_ops_t        ops;
} hash_impl_t;

#define ALI_CRYPTO_HASH_IMPL_START(name)    static hash_impl_t hash_impl_vect_##name[] = {     

#define ALI_CRYPTO_HASH_IMPL(type,                   \
            _$$hash$$_$$get$$_$$ctx$$_$$size$$,      \
            _$$hash$$_$$init$$,                      \
            _$$hash$$_$$update$$,                   \
            _$$hash$$_$$final$$,                    \
            _$$hash$$_$$reset$$)                     \
    {                                               \
        type,                                       \
        {_$$hash$$_$$get$$_$$ctx$$_$$size$$,         \
        _$$hash$$_$$init$$,                          \
        _$$hash$$_$$update$$,                       \
        _$$hash$$_$$final$$,                        \
        _$$hash$$_$$reset$$}                         \
    },


#define ALI_CRYPTO_HASH_IMPL_END()     };

//start
#define ALI_CRYPTO_HASH_GET_IMPL(name)                      \
hash_impl_t *ali_crypto_hash_get_impl(hash_type_t type)     \
{                                                           \
    uint32_t count = 0;                                     \
    uint32_t i = 0;                                         \
    uint32_t find = 0;                                      \
    hash_impl_t *p_impl = NULL;                             \
    count =                                                 \
       sizeof(hash_impl_vect_##name)/sizeof(hash_impl_t);   \
    for(i = 0; i < count; i++) {                            \
        p_impl = &hash_impl_vect_##name[i];                 \
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





#endif //__IMPL_HASH_H__


