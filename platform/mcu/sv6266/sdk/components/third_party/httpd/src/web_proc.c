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

//==============================================================================
//                                INCLUDE FILES
//==============================================================================
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// HTTPD
#include "os_port.h"
#include "axhttp.h"

//==============================================================================
//                                    MACROS
//==============================================================================

//==============================================================================
//                               TYPE DEFINITIONS
//==============================================================================

//==============================================================================
//                               LOCAL VARIABLES
//==============================================================================
char *err_string="?";

static void unescape(char *si);

//==============================================================================
//                          LOCAL FUNCTION PROTOTYPES
//==============================================================================

//==============================================================================
//                              EXTERNAL FUNCTIONS
//==============================================================================

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
static void unescape(char *si)
{
	unsigned int c;
	char *s=si;

	if (s == NULL)return ;
	while ((s = strpbrk(s, "%+")) != NULL) {
		/* Parse %xx */
		if (*s == '%') {
			sscanf(s + 1, "%02x", &c);
			*s++ = (char) c;
			strncpy(s, s + 2, strlen(s) + 1);
		}
		/* Space is special */
		else if (*s == '+')
			*s++ = ' ';
	}
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
void query_init(struct connstruct *cn)
{
	char *q, *query, *val;
	//int len;
	int idx=0;
	query_var *var;

    HTTPD_DEBUGF((httpd, "[HTTPD] query_init() - cn = %p, cn->query = %s\n", cn, cn->query));

	cn->query_tbl = (query_var *)ax_malloc(sizeof(query_var)*NUM_QUERYVAR);
	if (cn->query_tbl == NULL )
	{
        HTTPD_DEBUGF((httpd, "[HTTPD] query_init() - query_tbl = NULL\n"));
		return ;
	}
	memset(cn->query_tbl, 0, sizeof(query_var)*NUM_QUERYVAR);

	var = cn->query_tbl;

	query = cn->query;
	// len = cn->query_len;

	if ( (!query) || (*query == '\0') ) {
		// len = 0;
		return;
	}

	/* Parse into individual assignments */
	//while (strsep(&query, "&;"));
	q = query;
	var->name = q;
	var++;
	//unescape(q); /* Unescape each assignment */
	while ((q = strchr(q, '&')) != NULL){
		*q = '\0';
		var->name = ++q;
		var++;
	}

	var = cn->query_tbl;
	while( var->name && (idx<NUM_QUERYVAR) )
	{
		if ((val = strchr(var->name, '=')) != NULL)
		{
			*val = '\0';
			var->value = val+1;
		}
		else
			var->value = err_string;

		unescape(var->name);
		unescape(var->value);

        HTTPD_DEBUGF((
            httpd, "[HTTPD] query_init() - var->name = %s, var->value = %s, idx = %d\n",
            var->name, var->value, idx));

		var++;
		idx++;
	}
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
char *query_getvar(struct connstruct *cn, char *name)
{
	query_var *var;
	int idx=0;

    HTTPD_DEBUGF((httpd, "[HTTPD] query_getvar() - cn = %p, name = %s\n", cn, name));

	if(!(cn->query_tbl))
		query_init(cn);

	var = cn->query_tbl;

	while( var->name && (idx<NUM_QUERYVAR) ) {
        HTTPD_DEBUGF((
            httpd, "[HTTPD] query_getvar() - var->name = %s, var->value = %s, idx = %d\n",
            var->name, var->value, idx));
		//org, if( strncmp(var->name, name, strlen(name))== 0 )
        if( strcmp(var->name, name)== 0 )
			return var->value;
		var++;
		idx++;
	}

	return NULL;
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
int WEB_write_blk(struct connstruct *cn, char* startp, int offset, int len)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] WEB_write_blk() - soc_id = %d, buff_len = %d\n", cn->networkdesc, len));

    return special_write(cn->networkdesc, startp + offset, len);
}

//------------------------------------------------------------------------------
// FUNCTION
//
//
// DESCRIPTION
//
//
// PARAMETERS
//
//
// RETURN
//
//
//------------------------------------------------------------------------------
int WEB_printf(struct connstruct *cn, const char *format, ...)
{
	//int rc;      // return code
    va_list ap;  // for variable args
	char buf[1024];


	buf[0] = 0;
	va_start(ap, format); // init specifying last non-var arg
	vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap); // end var args

	return WEB_write_blk(cn, buf, 0, strlen(buf));
}

