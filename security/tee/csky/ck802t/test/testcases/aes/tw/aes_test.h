/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _AES_TEST_H_
#define _AES_TEST_H_

#include "tee_srv.h"
#include "tee_types.h"
#include "tee_dbg.h"

#include "tee_tos.h"
#include "tee_crypto.h"

#define TEST_AES_CTX_BUF_SZ (120)

#define DATA_SIZE (128)

extern uint8_t Key128[];
extern uint8_t Key192[];
extern uint8_t Key256[];
extern uint8_t IV[];
extern uint8_t SrcData[];
extern uint8_t DstData_Key128_Opssl_ECB_Out[];
extern uint8_t DstData_Key192_Opssl_ECB_Out[];
extern uint8_t DstData_Key256_Opssl_ECB_Out[];
extern uint8_t DstData_Key128_Opssl_CBC_Out[];

extern void dump_code(char *name, unsigned char *data, unsigned int size);

extern TEE_Result AES_API_Test(void);
extern int32_t    aes_ecb_cbc_test_v1();
extern int32_t    aes_ecb_opssl_test_v2();
extern int32_t    aes_cbc_opssl_test_v3();
extern int32_t    aes_multi_ctx_test_v4();

#endif /* _AES_TEST_H_ */
