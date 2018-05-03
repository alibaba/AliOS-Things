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

#ifndef	__CGI_API_H__
#define	__CGI_API_H__
//==============================================================================
//                                INCLUDE FILES
//==============================================================================

//==============================================================================
//                               MACROS PORTED FROM APUI
//==============================================================================

typedef unsigned long long		UINT64;
typedef int					INT32;
typedef long long 				INT64;

typedef unsigned char			UCHAR;
typedef unsigned short			USHORT;
typedef unsigned int			UINT;
typedef unsigned long			ULONG;

typedef unsigned char *		PUINT8;
typedef unsigned short *		PUINT16;
typedef unsigned int *			PUINT32;
typedef unsigned long long *	PUINT64;
typedef int *					PINT32;
typedef long long * 			PINT64;
typedef char 					STRING;
typedef signed char			CHAR;

typedef signed short			SHORT;
typedef signed int				INT;
typedef signed long			LONG;
typedef signed long long		LONGLONG;

typedef unsigned long long		ULONGLONG;

typedef unsigned char			BOOLEAN;
typedef void					VOID;

typedef char *				PSTRING;
typedef VOID *				PVOID;
typedef CHAR *				PCHAR;
typedef UCHAR * 				PUCHAR;
typedef USHORT *				PUSHORT;
typedef LONG *				PLONG;
typedef ULONG *				PULONG;
typedef UINT *				PUINT;

//==============================================================================
//                               Definitions PORTED FROM APUI
//==============================================================================


//==============================================================================
//                                    MACROS
//==============================================================================
#define	CGI_MAP(name,id)	CGI_var_map(cn,#name,id)
#define	CGI_CFG_ID(id)		WEB_printf(cn,"%u",id)
#define	CGI_CFG_GET(id)		CGI_get_cfg(cn,id)
#define	CGI_QUERY(var)		CGI_put_var(cn,var)
#define	CGI_DO_CMD()		WEB_printf(cn, "%d", CGI_do_cmd(cn))

#define CGI_AP_SSID()       CGI_get_ap_ssid(cn)
#define CGI_AP_PASSWD()     CGI_get_ap_passwd(cn)

#define CGI_MT7687AP_SSID()       CGI_get_mt7687ap_ssid(cn)
#define CGI_MT7687AP_PASSWD()     CGI_get_mt7687ap_passwd(cn)

//==============================================================================
//                               TYPE DEFINITIONS
//==============================================================================

//==============================================================================
//                               LOCAL VARIABLES
//==============================================================================

//==============================================================================
//                          LOCAL FUNCTION PROTOTYPES
//==============================================================================

//==============================================================================
//                              EXTERNAL FUNCTIONS
//==============================================================================
extern void CGI_var_map(struct connstruct *cn, char *name, int id);
extern void CGI_var_map_array(struct connstruct *cn, char *name, int id, int num);
extern void CGI_get_cfg(struct connstruct *cn, int tag);
extern char * CGI_put_var(struct connstruct *cn, char *var);
extern int CGI_do_cmd(struct connstruct *cn);

extern void CGI_get_ap_ssid(struct connstruct *cn);
extern void CGI_get_ap_passwd(struct connstruct *cn);

extern void CGI_get_mt7687ap_ssid(struct connstruct *cn);
extern void CGI_get_mt7687ap_passwd(struct connstruct *cn);

//==============================================================================
//                              EXTERNAL FUNCTIONS FOR WLAN
//==============================================================================

#endif //__CGI_API_H__

