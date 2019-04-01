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
//#include <stdint.h>
#include <stdio.h>
#include <string.h>

//MT7687
//#include <hal_rtc.h>

// HTTPD
#include "os_port.h"
#include "axhttp.h"
#include "cgi.h"

// FreeRTOS
//#include "hal_sys.h"

//==============================================================================
//                                    MACROS
//==============================================================================

//==============================================================================
//                               TYPE DEFINITIONS
//==============================================================================

//==============================================================================
//                               Porting from APSoC
//==============================================================================


//==============================================================================
//                               LOCAL VARIABLES
//==============================================================================

//==============================================================================
//                          LOCAL FUNCTION PROTOTYPES
//==============================================================================
static int apConfig(struct cgi_para *para);
static int test2(struct cgi_para *para);

const struct cgi cgi_cmds[]=
{
	{"/apConfig.cgi", &apConfig},
	{"/Test2.cgi", &test2},
};

//==============================================================================
//                              EXTERNAL FUNCTIONS
//==============================================================================
CGI_FUNC cgi_filename_match(char *path_info)
{
    int i;

    HTTPD_DEBUGF((httpd, "[HTTPD] cgi_filename_match() - path_info = %s\n", path_info));

    for (i=0; i< (sizeof(cgi_cmds)/sizeof(struct cgi)) ; i++)
    {
        if (!strcmp(cgi_cmds[i].name, path_info))
        {
            return cgi_cmds[i].func;
        }
    }

    return NULL;
}

static int apConfig(struct cgi_para *para)
{
    char buf[MAXREQUESTLENGTH];

    HTTPD_DEBUGF((httpd, "[HTTPD] apConfig() - sd = %d, cmd = %s, cmd_len = %d\n", para->sd, para->cmd, para->cmd_len));

    snprintf(buf, sizeof(buf),
        "<html><body><h1>apConfig.cgi http response</h1><table><tr><td>cmd = %s</td></tr><tr><td>cmd_len = %d</td></tr></table></body></html>\n",
        para->cmd, para->cmd_len);

    para->func(para->sd, buf, strlen(buf));

    return 0;
}

static int test2(struct cgi_para *para)
{
    char buf[MAXREQUESTLENGTH];
    struct tm gt;
    //hal_rtc_time_t r_time;
    char currentTime[32] = "\0";
    const char * format = "%a, %d %b %Y %H:%M:%S GMT";

    HTTPD_DEBUGF((httpd, "[HTTPD] test2() - sd = %d, cmd = %s, cmd_len = %d\n", para->sd, para->cmd, para->cmd_len));

    memset(&gt, 0, sizeof(struct tm));
#if 0 // need to modify
    if(HAL_RTC_STATUS_OK == hal_rtc_get_time(&r_time))
    {
        gt.tm_year = r_time.rtc_year; // ?
        gt.tm_mon = r_time.rtc_mon; // ?
        gt.tm_mday = r_time.rtc_day;
        gt.tm_wday = r_time.rtc_week;
        gt.tm_hour = r_time.rtc_hour;
        gt.tm_min = r_time.rtc_min;
        gt.tm_sec = r_time.rtc_sec;
        strftime(currentTime, sizeof(currentTime), format, &gt);
    }
#endif
    snprintf(buf, sizeof(buf),
        "<html><head></head><body>"
        "<h1>Welcome to Test2.cgi</h1>"
        "<h1>Current system time: %s</h1>"
        "<form id=\"Test\" action=\"Test.html\" method=\"get\"><input type=\"submit\" value=\"Back\" /></form>"
        "</body></html>\n",currentTime
        );

    para->func(para->sd, buf, strlen(buf));

    return 0;
}


