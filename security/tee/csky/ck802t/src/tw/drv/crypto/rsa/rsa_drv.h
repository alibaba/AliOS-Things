/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 **/

#ifndef _RSA_DRV_H_
#define _RSA_DRV_H_

#include "tee_types.h"
#include "hal_memmap.h"

#define _MAX_RSA_LP_CNT    (100000)

typedef struct _rsa_reg_t {
    uint32_t RSA_MWID;
    uint32_t RSA_DWID;
    uint32_t RSA_BWID;
    uint32_t RSA_CTRL;
    uint32_t RSA_RST;
    uint32_t RSA_LP_CNT;
    uint32_t RSA_Q0;
    uint32_t RSA_Q1;
    uint32_t RSA_ISR;
    uint32_t RSA_IMR;
    uint32_t Rev1[54];
    uint32_t RSA_RFM;
    uint32_t Rev2[63];
    uint32_t RSA_RFD;
    uint32_t Rev3[63];
    uint32_t RSA_RFC;
    uint32_t Rev4[63];
    uint32_t RSA_RFB;
    uint32_t Rev5[63];
    uint32_t RSA_RFR;
} rsa_reg_t, *prsa_reg_t;

#endif /* _RSA_DRV_H_ */
