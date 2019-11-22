/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "exception_handler.h"

/******************************************************************************/
/*            Memory Regions Definition                                       */
/******************************************************************************/
#if defined(__GNUC__)

extern unsigned int Image$$TEXT$$Base[];
extern unsigned int Image$$TEXT$$Limit[];
extern unsigned int Image$$CACHED_SYSRAM_TEXT$$Base[];
extern unsigned int Image$$CACHED_SYSRAM_TEXT$$Limit[];
extern unsigned int Image$$CACHED_SYSRAM_DATA$$RW$$Base[];
extern unsigned int Image$$CACHED_SYSRAM_DATA$$ZI$$Limit[];
extern unsigned int Image$$NONCACHED_SYSRAM_DATA$$Base[];
extern unsigned int Image$$NONCACHED_SYSRAM_ZI$$Limit[];
extern unsigned int Image$$TCM$$RO$$Base[];
extern unsigned int Image$$TCM$$ZI$$Limit[];
extern unsigned int Image$$STACK$$ZI$$Base[];
extern unsigned int Image$$STACK$$ZI$$Limit[];

memory_region_type memory_regions[] =
{
    {"text", Image$$TEXT$$Base, Image$$TEXT$$Limit, 0},
    {"sysram_text", Image$$CACHED_SYSRAM_TEXT$$Base, Image$$CACHED_SYSRAM_TEXT$$Limit, 1},
    {"cached_sysram_data", Image$$CACHED_SYSRAM_DATA$$RW$$Base, Image$$CACHED_SYSRAM_DATA$$ZI$$Limit, 1},
    {"noncached_sysram_data", Image$$NONCACHED_SYSRAM_DATA$$Base, Image$$NONCACHED_SYSRAM_ZI$$Limit, 1},
    {"tcm", Image$$TCM$$RO$$Base, Image$$TCM$$ZI$$Limit, 1},
    {"stack", Image$$STACK$$ZI$$Base, Image$$STACK$$ZI$$Limit, 1},
    {"scs",  (unsigned int *)SCS_BASE, (unsigned int *)(SCS_BASE + 0x1000), 1},
    {0}
};

#ifdef MTK_SWLA_ENABLE
extern unsigned int Image$$SWLA$$Base[];
extern unsigned int Image$$SWLA$$Limit[];

void SLA_get_region(uint32_t *pxBase, uint32_t *pxLen)
{
    *pxBase = (((uint32_t)Image$$NONCACHED_SYSRAM_ZI$$Limit | VRAM_BASE) + 0x20) & ~(0x20 - 1);  /* align up to 32Byte */
    *pxLen = (uint32_t)(((uint32_t)Image$$SWLA$$Limit - *pxBase) ) & ~(0x20 - 1);  /* swla buffer area is free ram + swla  reserve area */
}
#endif /* MTK_SWLA_ENABLE */
#endif /* __GNUC__ */

#if defined (__CC_ARM)

extern unsigned int Image$$TEXT$$Base[];
extern unsigned int Image$$TEXT$$Limit[];
extern unsigned int Image$$CACHED_SYSRAM_TEXT$$Base[];
extern unsigned int Image$$CACHED_SYSRAM_TEXT$$Limit[];
extern unsigned int Image$$CACHED_SYSRAM_DATA$$RW$$Base[];
extern unsigned int Image$$CACHED_SYSRAM_DATA$$ZI$$Limit[];
extern unsigned int Image$$NONCACHED_SYSRAM_DATA$$Base[];
extern unsigned int Image$$NONCACHED_SYSRAM_ZI$$Limit[];
extern unsigned int Image$$TCM$$RO$$Base[];
extern unsigned int Image$$TCM$$ZI$$Limit[];
extern unsigned int Image$$STACK$$ZI$$Base[];
extern unsigned int Image$$STACK$$ZI$$Limit[];

const memory_region_type memory_regions[] =
{
    {"text", Image$$TEXT$$Base, Image$$TEXT$$Limit, 0},
    {"sysram_text", Image$$CACHED_SYSRAM_TEXT$$Base, Image$$CACHED_SYSRAM_TEXT$$Limit, 1},
    {"cached_sysram_data", Image$$CACHED_SYSRAM_DATA$$RW$$Base, Image$$CACHED_SYSRAM_DATA$$ZI$$Limit, 1},
    {"noncached_sysram_data", Image$$NONCACHED_SYSRAM_DATA$$Base, Image$$NONCACHED_SYSRAM_ZI$$Limit, 1},
    {"tcm", Image$$TCM$$RO$$Base, Image$$TCM$$ZI$$Limit, 1},
    {"stack", Image$$STACK$$ZI$$Base, Image$$STACK$$ZI$$Limit, 1},
    {"scs",  (unsigned int *)SCS_BASE, (unsigned int *)(SCS_BASE + 0x1000), 1},
    {0}
};

#endif /* __CC_ARM */

#if defined(__ICCARM__)

extern unsigned int SYSRAM_BLOCK$$Base[];
extern unsigned int SYSRAM_BLOCK$$Limit[];
extern unsigned int VSYSRAM_BLOCK$$Base[];
extern unsigned int VSYSRAM_BLOCK$$Limit[];
extern unsigned int TCM_BLOCK$$Base[];
extern unsigned int TCM_BLOCK$$Limit[];
extern unsigned int CSTACK$$Base[];
extern unsigned int CSTACK$$Limit[];

const memory_region_type memory_regions[] =
{
    {"sysram", SYSRAM_BLOCK$$Base, SYSRAM_BLOCK$$Limit, 1},
    {"vsysram", VSYSRAM_BLOCK$$Base, VSYSRAM_BLOCK$$Limit, 1},
    {"tcm", TCM_BLOCK$$Base, CSTACK$$Limit, 1},
    {"stack", CSTACK$$Base, CSTACK$$Limit, 0},
    {"scs", (unsigned int*)SCS_BASE, (unsigned int*)(SCS_BASE + 0x1000), 1},
    {0}
};


#endif /* __ICCARM__ */
void heap_test_()
{
    printf("%x,%x,%x,%x,%x\r\n",
           (unsigned int)Image$$CACHED_SYSRAM_TEXT$$Base,
           (unsigned int)Image$$CACHED_SYSRAM_DATA$$RW$$Base,
           (unsigned int)Image$$NONCACHED_SYSRAM_DATA$$Base,
           (unsigned int)Image$$TCM$$RO$$Base,
           (unsigned int)Image$$TCM$$ZI$$Limit);
    printf("%x,%x,%x,%x,%x\r\n",
           (unsigned int)Image$$CACHED_SYSRAM_TEXT$$Limit,
           (unsigned int)Image$$CACHED_SYSRAM_DATA$$ZI$$Limit,
           (unsigned int)Image$$NONCACHED_SYSRAM_ZI$$Limit,
           (unsigned int)Image$$TCM$$ZI$$Limit,
           (unsigned int)Image$$STACK$$ZI$$Limit);
    printf("stack:%x,%x\r\n",
           (unsigned int)Image$$STACK$$ZI$$Base,
           (unsigned int)Image$$STACK$$ZI$$Limit);
}


