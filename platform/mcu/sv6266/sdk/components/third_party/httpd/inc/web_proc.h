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

#ifndef __WEB_PROC_H__
#define __WEB_PROC_H__
//==============================================================================
//                                INCLUDE FILES
//==============================================================================
#include <stdio.h>
#include <time.h>

#define	WEBP_COMPRESS		0x0001
#define	WEBP_NORMAL			0x0000
#define NO_AUTH				0x0002
#define CACHE				0x0004
#define	WEBP_LOCKED			0x0008

#define NUM_QUERYVAR	    255
#define MAXREQUESTLENGTH    256

/* record environmental variables */

typedef struct
{
	char* name;
	char* value;
}query_var;

struct connstruct
{
    struct connstruct *next;
    int state;
    int reqtype;
    int networkdesc;
    int filedesc; // MT7687 do not support FS, this paramter is not used
    char actualfile[MAXREQUESTLENGTH];
    char filereq[MAXREQUESTLENGTH];
    char dirname[MAXREQUESTLENGTH];
    char server_name[MAXREQUESTLENGTH];
    int numbytes;
    unsigned char is_v1_0;
    unsigned char close_when_done;
    time_t if_modified_since;
    char authorization[MAXREQUESTLENGTH];
    int post_read;
    int post_state;
    char *post_data;
    char *query;
    int query_len;
    int content_length;
	query_var *query_tbl;
    unsigned char is_cgi;
};

typedef struct
{
	char *path;						/* Web page URL path */
	int (*fileFunc)(struct connstruct *, char *);		/* web page function call */
	unsigned long size;			/* storage size */
	short flag;						/* flag */
	void *param;					/* parameter to fileFunc() */
	char *(*cgiFunc)(struct connstruct *);		/* cgi function call */
	unsigned long offset;		/* the start offset of file*/
}webpage_entry;

typedef struct
{
	int code;
	char *filename;
}err_page;

void query_init(struct connstruct *req);
char *query_getvar(struct connstruct *req, char *name);

#define WEB_getvar(req, var, default)	( query_getvar(req, var) ? : default )
#define WEB_query(req, var) query_getvar(req, var)

int WEB_write_blk(struct connstruct *req, char* startp, int offset, int len);
int WEB_printf(struct connstruct *req, const char *format, ...);

#endif /* __WEB_PROC_H__ */
