/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * File: lightduer_aes.h
 * Auth: Leliang Zhang(zhangleliang@baidu.com)
 * Desc: Provide the aes encryption(only CBC mode support now).
 */

#ifndef BAIDU_DUER_LIGHTDUER_AES_H
#define BAIDU_DUER_LIGHTDUER_AES_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// aes context, real type depend on the implementation
typedef void *duer_aes_context;

// get the initialized aes context
duer_aes_context duer_aes_context_init(void);

/*
 *set the key used in the encryption and decryption
 *@param ctx, got from duer_aes_context_init
 *@param key, the key will be used,
 *@param keybits, the length of @key in bits, should be one of 128, 192, 256
 *@return, DUER_OK on success, or other error code
 */
int duer_aes_setkey(duer_aes_context ctx,
                    const unsigned char *key,
                    unsigned int keybits);

/*
 *set the iv used in the encryption and decryption
 *@param ctx, got from duer_aes_context_init
 *@param iv, the iv will be used, it's 16 bytes length.
 *@return, DUER_OK on success, or other error code
 */
int duer_aes_setiv(duer_aes_context ctx, unsigned char iv[16]);

/*
 *encrypt the input info, use the key/iv from @duer_aes_setkey and @duer_aes_setiv
 *@param ctx, got from duer_aes_context_init
 *@param length, the length of the input, should be multiple of 16
 *@param input, the content will be encrypted
 *#param output, the encrypted content
 *@return, DUER_OK on success, or other error code
 */
int duer_aes_cbc_encrypt(duer_aes_context ctx,
                         size_t length,
                         const unsigned char* input,
                         unsigned char* output);

/*
 *decrypt the input info, use the key/iv from @duer_aes_setkey and @duer_aes_setiv
 *@param ctx, got from duer_aes_context_init
 *@param length, the length of the input, should be multiple of 16
 *@param input, the content will be decrypted
 *#param output, the decrypted content
 *@return, DUER_OK on success, or other error code
 */
int duer_aes_cbc_decrypt(duer_aes_context ctx,
                         size_t length,
                         const unsigned char* input,
                         unsigned char* output);

/*
 *destroy the context
 *@param ctx, got from duer_aes_context_init
 *@return, DUER_OK on success, or other error code
 */
int duer_aes_context_destroy(duer_aes_context ctx);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_AES_H*/
