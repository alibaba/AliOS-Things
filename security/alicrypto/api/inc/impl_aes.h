/**
 * Copyright (C) 2017  Alibaba Group Holding Limited.
 **/
#ifndef __IMPL_AES_H__
#define __IMPL_AES_H__
#include "ali_crypto_types.h"

/* AES */
typedef struct _aes_ops_t {
    ali_crypto_result (*aes_get_ctx_size)(aes_type_t type, size_t *size);
    
    ali_crypto_result (*aes_init)(aes_type_t      type, 
                        bool          is_enc, 
                        const uint8_t *key1, 
                        const uint8_t *key2, 
                        size_t        keybytes,  
                        const uint8_t *iv, 
                        void          *context);
    
    ali_crypto_result (*aes_process)(const uint8_t    *src, 
                        uint8_t         *dst, 
                        size_t          size, 
                        void            *context);
    
    ali_crypto_result (*aes_finish)(const uint8_t   *src, 
                        size_t          src_size, 
                        uint8_t         *dst, 
                        size_t          *dst_size, 
                        sym_padding_t   padding,
                        void            *context);
    
    ali_crypto_result (*aes_reset)(void *context);
} aes_ops_t;

typedef struct _aes_impl_t {
	aes_type_t       type;
	aes_ops_t        ops;
} aes_impl_t;

#define ALI_CRYPTO_AES_IMPL_START(name)    static aes_impl_t aes_impl_vect_##name[] = {     

#define ALI_CRYPTO_AES_IMPL(type,                   \
            _$$aes$$_$$get$$_$$ctx$$_$$size$$,      \
            _$$aes$$_$$init$$,                      \
            _$$aes$$_$$process$$,                   \
            _$$aes$$_$$finish$$,                    \
            _$$aes$$_$$reset$$)                     \
    {                                               \
        type,                                       \
        {_$$aes$$_$$get$$_$$ctx$$_$$size$$,         \
        _$$aes$$_$$init$$,                          \
        _$$aes$$_$$process$$,                       \
        _$$aes$$_$$finish$$,                        \
        _$$aes$$_$$reset$$}                         \
    },


#define ALI_CRYPTO_AES_IMPL_END()     };

#define AES_IMPL_COUNT(name)      (sizeof(aes_impl_vect_##name)/sizeof(aes_impl_t))

#define AES_IMPL_VECT(name)     aes_impl_vect_##name
//start
#define ALI_CRYPTO_AES_GET_IMPL(name)                   \
aes_impl_t *ali_crypto_aes_get_impl(aes_type_t type)    \
{                                                       \
    uint32_t count = 0;                                 \
    uint32_t i = 0;                                     \
    uint32_t find = 0;                                  \
    aes_impl_t *p_impl = NULL;                      \
    count =                                             \
       sizeof(aes_impl_vect_##name)/sizeof(aes_impl_t); \
    for(i = 0; i < count; i++) {                        \
        p_impl = &aes_impl_vect_##name[i];          \
        if(p_impl->type == type) {                  \
            find = 1;                                   \
            break;                                      \
        }                                               \
    }                                                   \
    if(0 == find) {                                     \
        p_impl = NULL;                              \
    }                                                   \
    return p_impl;                                  \
}                                                       \
//end



#endif //__IMPL_AES_H__


