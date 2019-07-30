#ifndef _WRAPPERS_TLS_H_
#define _WRAPPERS_TLS_H_

#include "linkkit/infra/infra_types.h"
#include "linkkit/infra/infra_defs.h"

int HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms);

int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms);

int32_t HAL_SSL_Destroy(uintptr_t handle);

uintptr_t HAL_SSL_Establish(const char *host,
                            uint16_t port,
                            const char *ca_crt,
                            uint32_t ca_crt_len);
#endif

