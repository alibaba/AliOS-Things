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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// HTTPD
#include "os_port.h"
#include "axhttp.h"
#include "auth_check.h"

struct userinfo_t userinfo[USER_NUM_MAX];

static void send_authenticate(struct connstruct *cn, char* realm );

void user_config(int lan_updated, int wan_updated)
{
	int i;
	char username[50], passwd[50];

    HTTPD_DEBUGF((httpd, "[HTTPD] user_config() - lan_updated = %d, wan_updated = %d \n", lan_updated, wan_updated));

	for (i=0; i < USER_NUM_MAX; i++)
	{
		username[0] = '\0';
		passwd[0] = '\0';

    #if 0
		CFG_get(CFG_SYS_USERS+i, username);
		CFG_get(CFG_SYS_ADMPASS+i, passwd);
	#endif

        // TODO: NVRAM
        memset(username, 0, 50);
        memset(passwd, 0, 50);
        memcpy(username, "admin", 5);
        memcpy(passwd, "admin", 5);

        HTTPD_DEBUGF((httpd, "[HTTPD] user_config() - NVRAM username= %s, passwd = %s\n", username, passwd));

		if (strcmp(username, userinfo[i].username) || strcmp(passwd, userinfo[i].passwd) ||
			(lan_updated && userinfo[i].listenif ==0)||(wan_updated && userinfo[i].listenif == 1))
		{
			memset(&(userinfo[i]), 0, sizeof(struct userinfo_t));
			strcpy(userinfo[i].username, username);
			strcpy(userinfo[i].passwd, passwd);
		}
	}
}

int auth_check(struct connstruct *cn)
{
    char* authpass;
    int i;
    char authinfo[MAXREQUESTLENGTH];

    HTTPD_DEBUGF((httpd, "[HTTPD] auth_check() - cn = %p, cn->authorization = %s\n", cn, cn->authorization));

    /* Split into user and password. */
    memcpy(authinfo, cn->authorization, MAXREQUESTLENGTH);
    authpass = strchr(authinfo, ':' );

    if ( authpass == (char*) 0 )
	/* No colon?  Bogus auth info. */
		goto send_auth;
    *authpass++ = '\0';

    /* Is this the right user? */
	for(i=0; i < USER_NUM_MAX; i++)
	{
        HTTPD_DEBUGF((
            httpd, "[HTTPD] auth_check() - (authinfo = %s, authpass = %s), (username = %s, passwd = %s)\n",
            authinfo, authpass, userinfo[i].username, userinfo[i].passwd));
        if((strcmp( authinfo, userinfo[i].username) == 0 ) &&
			( strcmp( authpass, userinfo[i].passwd) == 0 ))
			break;
	}

	/* no match username and password */
	if(i == USER_NUM_MAX)
	{
        HTTPD_DEBUGF((httpd, "[HTTPD] auth_check() - no match username and password\n"));
		goto send_auth;
	}

	return 0;

send_auth:
    send_authenticate(cn, cn->server_name);
	return HTTP_UNAUTHORIZED;
}

static void send_authenticate(struct connstruct *cn, char *realm)
{
    char buf[1024];

    HTTPD_DEBUGF((httpd, "[HTTPD] send_authenticate() - cn = %p, realm = %s\n", cn, realm));

    snprintf(buf, sizeof(buf), HTTP_VERSION" 401 Unauthorized\n"
         "WWW-Authenticate: Basic\n"
                 "realm=\"%s\"\n", realm);

    // no matter special_write result, also need to removeconnection
    special_write(cn->networkdesc, buf, strlen(buf));
    removeconnection(cn);
}

