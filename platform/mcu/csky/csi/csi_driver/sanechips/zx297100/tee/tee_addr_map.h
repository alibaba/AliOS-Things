/* ****************************************************************************
 *                                                                          *
 * C-Sky Microsystems Confidential                                          *
 * -------------------------------                                          *
 * This file and all its contents are properties of C-Sky Microsystems. The *
 * information contained herein is confidential and proprietary and is not  *
 * to be disclosed outside of C-Sky Microsystems except under a             *
 * Non-Disclosured Agreement (NDA).                                         *
 *                                                                          *
 ****************************************************************************/

#ifndef TEE_ADDR_MAP_H
#define TEE_ADDR_MAP_H

#include "partitions.h"


#ifndef PART_ADDR_TEE
#error "PART_ADDR_TEE undefined!"
#endif

#ifndef PART_SIZE_TEE
#error "PART_SIZE_TEE undefined!"
#endif

#define OTP_BANK_SIZE               2048
#define OTP_BASE_ADDR               0x0f000000

#define TW_RO_ADDR      PART_ADDR_TEE
#define TW_RO_SIZE      PART_SIZE_TEE
#define TW_RW_ADDR      0x30000000
#define TW_RW_SIZE      0x00001000

#define NTW_RO_ADDR     0x0ffae000
#define NTW_RO_SIZE     0x00010000
#define NTW_RW_ADDR     (TW_RW_ADDR + 0x00002000)
#define NTW_RW_SIZE     0x00008000

#define SRAM_BASE_ADDR  0x30000000

#define NTW_ENTRY_ADDR  NTW_RO_ADDR

#define PLATFORM_SECURE_BASE  (0x21420000)
#define PLATFORM_AES_ADDRBASE        (0x21421000)
#define PLATFORM_SHA_BADDR             (0x21422000)
#define PLATFORM_RSA_ADDRBASE        (0x21423000)
#define PLATFORM_RAND_ADDRBASE     (0x21424000)
#define AP_CRPM_BASE_ADDR                 (0x22005000) 
#define TOP_CRPM_BASE_ADDR                (0x21306000)

#endif
