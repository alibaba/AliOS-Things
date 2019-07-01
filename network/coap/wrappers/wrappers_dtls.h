/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef _WRAPPERS_DTLS_H_
#define _WRAPPERS_DTLS_H_

#include "infra_types.h"
#include "infra_defs.h"

DTLSContext *HAL_DTLSSession_create(coap_dtls_options_t *p_options);

unsigned int HAL_DTLSSession_write(DTLSContext *context,
                                   const unsigned char *p_data,
                                   unsigned int *p_datalen);

unsigned int HAL_DTLSSession_read(DTLSContext *context,
                                  unsigned char   *p_data,
                                  unsigned int    *p_datalen,
                                  unsigned int     timeout);

unsigned int HAL_DTLSSession_free(DTLSContext *context);
#endif

