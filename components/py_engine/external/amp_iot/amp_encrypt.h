/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "amp_adapter_mbedtls.h"

// RSA sig verify step (Haas board)
// step 1: get n & e value from public_key
//      example:
//      static char public_key[] = "-----BEGIN PUBLIC KEY-----\n"
//                                  "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCLGcVZcBZmsusYf4cjkjSgCkiV\n"
//                                  "2d2bfFyeMbBvFI58enXX3ZX9my7VjzeAVfv7u4mIbnCcTeYGYW/v8cvxty2jcDVC\n"
//                                  "SLvuOMuPimMBjjU+EhayFOzyzfQamXc0fHtvXIHMgN5RQg2Gjjs684LaG8SaFNQx\n"
//                                  "ZjubD00zykFtxkjD0wIDAQAB\n"
//                                  "-----END PUBLIC KEY-----\n";
//      get value n & e from public_key:
//          char pubkey_n[]="8B19C559701666B2EB187F87239234A00A4895D9DD9B7C5C9E31B06F148E7C7A75D7DD95FD9B2ED58F378055FBFBBB89886E709C4DE606616FEFF1CBF1B72DA370354248BBEE38CB8F8A63018E353E1216B214ECF2CDF41A9977347C7B6F5C81CC80DE51420D868E3B3AF382DA1BC49A14D431663B9B0F4D33CA416DC648C3D3";
//          char pubkey_e[]="010001";
// 
// step 2: set value n & e in mbedtls content
//      example for mbedtls
//      mbedtls_mpi_read_string(&ctx.N, 16, pubkey_n);
//      mbedtls_mpi_read_string(&ctx.E, 16, pubkey_e);
// 
// step 3: change input string do hash sha256 change
//      example:
//       static char input_text[] = "1234567890";
//       do sha256 change:
//       input_hash :  c775e7b757ede630cd0aa1113bd102661ab38829ca52a6422ab782862f268646
// 
// step 4: signature_file base64 decode
//      example
//      static char signature_output[] = "kux+S6rap8Rb92UDebUzJntmhIbRUHi34rD6Nn55wty88mYGj0SBzf8zwyc6R7mH4LivuBPOrLIjTnxOueMkjPUzBwGJ7McryNVlTmBQAl2zqO0ezSGpWGRoNWaNl2Q4zgReLgAE6OP5KJw1QT6z2YPTPf1ValV9KJpdeN0WRTs=";
//      do base64 decode:
//      92 EC 7E 4B AA DA A7 C4 5B F7 65 03 79 B5 33 26
//      7B 66 84 86 D1 50 78 B7 E2 B0 FA 36 7E 79 C2 DC
//      BC F2 66 06 8F 44 81 CD FF 33 C3 27 3A 47 B9 87
//      E0 B8 AF B8 13 CE AC B2 23 4E 7C 4E B9 E3 24 8C
//      F5 33 07 01 89 EC C7 2B C8 D5 65 4E 60 50 02 5D
//      B3 A8 ED 1E CD 21 A9 58 64 68 35 66 8D 97 64 38
//      CE 04 5E 2E 00 04 E8 E3 F9 28 9C 35 41 3E B3 D9
//      83 D3 3D FD 55 6A 55 7D 28 9A 5D 78 DD 16 45 3B
// 
// step 5: RSA sign verify
//      example use mbedtls
//      mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, 0, input_hash_sha256, signature_output_base64_decode)
// 



// hash cal core
int amp_hash_init(amp_hash_ctx_t *ctx, unsigned char type);
int amp_hash_update(amp_hash_ctx_t *ctx, const unsigned char *buf, unsigned int len);
int amp_hash_final(amp_hash_ctx_t *ctx, unsigned char *dgst);
int amp_verify_rsa_hash(unsigned char *src, int src_len, unsigned char *dig, unsigned char type);
int amp_verify_rsa_sign(unsigned char *dig_src, unsigned char *sig_src, unsigned char *pub_key);
int amp_encrypt_rsa(unsigned char *dig_src, unsigned char *pub_key);

