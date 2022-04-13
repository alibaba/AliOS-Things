/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef MT_PRINTF_H
#define MT_PRINTF_H

//#include "FreeRTOSConfig.h"
#include "freertos_to_aos.h"

#define DEBUGLEVEL LOG_DEBUG

/* DEBUG LEVEL definition*/
#define LOG_DEBUG  3
#define LOG_INFO   2
#define LOG_WARN   1
#define LOG_ERROR  0
#define PRINTF(x...) PRINTF_D(x)

#define DEBUGLEVEL 2

#define PRINTF_D(x...)            \
({                                \
    if (LOG_DEBUG <= DEBUGLEVEL)  \
        printf(x);                \
})

#define PRINTF_I(x...)            \
({                                \
    if (LOG_INFO <= DEBUGLEVEL)   \
        printf(x);                \
})

#define PRINTF_W(x...)            \
({                                \
    if (LOG_WARN <= DEBUGLEVEL)   \
        printf(x);                \
})


#define PRINTF_E(x...)            \
({                                \
    if (LOG_ERROR <= DEBUGLEVEL)  \
        printf(x);                \
})

#define PRINTF_ONCE(x...)                                   \
({                                                              \
        static char __print_once;                 \
                                                                \
        if (!__print_once) {                                    \
                __print_once = 1;                            \
                printf(x);                     \
        }                                                       \
})
#endif /*  MT_PRINTF_H */
