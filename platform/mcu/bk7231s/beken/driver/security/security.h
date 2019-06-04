#ifndef _SEC_H_
#define _SEC_H_

#include "security_reg.h"
#include "sha.h"
#include "aes.h"

extern int is_secrity_sha_busy(void);
extern int is_secrity_rsa_busy(void);
extern int is_secrity_aes_busy(void);
int security_aes_init(SECURITY_AES_DRV_DESC *p_security_aes_drv_desc);
int security_sha_init(SECURITY_SHA_DRV_DESC *p_security_sha_drv_desc);
void get_security_aes_data(unsigned long *pul_data);
void get_security_sha_data(hal_sha_context *ctx, unsigned long *pul_data);

#endif

