/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _AWSS_SECURITY_H_
#define _AWSS_SECURITY_H_

#include <stdint.h>

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int softap_decrypt_password(const char *cipher, const uint8_t *random, char *passwd);

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
#endif /* #ifndef _AWSS_SECURITY_H_ */
