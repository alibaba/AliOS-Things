/*
 * MediaTek Inc. (C) 2020. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
#ifndef __MT_GCPU_H__
#define __MT_GCPU_H__

typedef enum
{
    GCPU_RET_OK = 0,
    E_GCPU_PARAM_WRONG = 1,
    E_GCPU_OPERATE_FORBID = 2,
    E_GCPU_NO_MEM = 3,
    E_GCPU_INVALID_ARG = 4,
    E_GCPU_MEM_MAP_FAIL = 5,
    E_GCPU_HW_FAIL = 6,
} GCPU_RET_T;

/* Note:
   1. The driver does not work while enabling cache,
   It directly accesses DRAM without considering cache.

   2. The driver assumes PA == VA.

   3. pre-device key--> load from efuse HUK
   GCPU_AESPK_D/EPAK -> aes_pk_ecb
   GCPU_AESPK_D/ECBC -> aes_pk_cbc
*/

/*
* @This function using pre-devic key to do enc/dec operations
* @key length = 128 bits
* @param
* @return Zero on success
*/
int32_t gcpu_aes_ecb_crypt(
uint8_t *src,         /* src address */
uint8_t *dst,		  /* dst address */
uint32_t dataSize,	  /* data length in unit of byte */
int32_t encrypt);	  /* 0: decrypt; 1: encrypt */

int32_t gcpu_aes_cbc_crypt(
uint8_t *src,         /* src address */
uint8_t *dst,		  /* dst address */
uint32_t dataSize,	  /* data length in unit of byte */
uint8_t *iv, 	      /* initial vector */
int32_t encrypt);     /* 0: decrypt; 1: encrypt */

/* gcpu_init() should be called first */
extern void gcpu_init();
extern void gcpu_cli_register(void);
#endif
