/**
 * Copyright (C) 2018  Alibaba Group Holding Limited.
 **/
#ifndef __IMPL_SM4_H__
#define __IMPL_SM4_H__
#include "ali_crypto_types.h"

/* SM4 */
typedef struct _sm4_ops_t {
    ali_crypto_result (*sm4_get_ctx_size)(sm4_type_t type, size_t *size);
    
    ali_crypto_result (*sm4_init)(sm4_type_t      type, 
                        bool          is_enc, 
                        const uint8_t *key1, 
                        const uint8_t *key2, 
                        size_t        keybytes,  
                        const uint8_t *iv, 
                        void          *context);
    
    ali_crypto_result (*sm4_process)(const uint8_t    *src, 
                        uint8_t         *dst, 
                        size_t          size, 
                        void            *context);
    
    ali_crypto_result (*sm4_finish)(const uint8_t   *src, 
                        size_t          src_size, 
                        uint8_t         *dst, 
                        size_t          *dst_size, 
                        sym_padding_t   padding,
                        void            *context);
    
    ali_crypto_result (*sm4_reset)(void *context);
} sm4_ops_t;

typedef struct _sm4_impl_t {
	sm4_type_t       type;
	sm4_ops_t        ops;
} sm4_impl_t;

#define ALI_CRYPTO_SM4_IMPL_START(name)    static sm4_impl_t sm4_impl_vect_##name[] = {     

#define ALI_CRYPTO_SM4_IMPL(type,                   \
            _$$sm4$$_$$get$$_$$ctx$$_$$size$$,      \
            _$$sm4$$_$$init$$,                      \
            _$$sm4$$_$$process$$,                   \
            _$$sm4$$_$$finish$$,                    \
            _$$sm4$$_$$reset$$)                     \
    {                                               \
        type,                                       \
        {_$$sm4$$_$$get$$_$$ctx$$_$$size$$,         \
        _$$sm4$$_$$init$$,                          \
        _$$sm4$$_$$process$$,                       \
        _$$sm4$$_$$finish$$,                        \
        _$$sm4$$_$$reset$$}                         \
    },


#define ALI_CRYPTO_SM4_IMPL_END()     };

#define SM4_IMPL_COUNT(name)      (sizeof(sm4_impl_vect_##name)/sizeof(sm4_impl_t))

#define SM4_IMPL_VECT(name)     sm4_impl_vect_##name
//start
#define ALI_CRYPTO_SM4_GET_IMPL(name)                   \
sm4_impl_t *ali_crypto_sm4_get_impl(sm4_type_t type)    \
{                                                       \
    uint32_t count = 0;                                 \
    uint32_t i = 0;                                     \
    uint32_t find = 0;                                  \
    sm4_impl_t *p_impl = NULL;                      \
    count =                                             \
       sizeof(sm4_impl_vect_##name)/sizeof(sm4_impl_t); \
    for(i = 0; i < count; i++) {                        \
        p_impl = &sm4_impl_vect_##name[i];          \
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



#endif //__IMPL_SM4_H__


