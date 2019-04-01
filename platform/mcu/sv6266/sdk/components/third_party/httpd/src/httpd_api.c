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

#include <stdio.h>
#include <string.h>

//#include "task_def.h"

// HTTPD
#include "os_port.h"
#include "axhttp.h"
#include "httpd.h"
#include "httpd_iprot.h"

static OsBufQ httpd_proc_mbox = NULL;
static OsTaskHandle httpd_proc_task_handle = NULL;
static HTTPD_STATUS httpd_status = HTTPD_STATUS_UNINIT;
static OsBufQ httpd_fb_mbox = NULL;

void httpd_start_handle(void)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] httpd_start_handle()\n"));

    httpd_set_status(HTTPD_STATUS_RUN);
    httpd_fb_status(HTTPD_STATUS_RUN, (HTTPD_STOP_REASON)0);
}

void httpd_stop_handle(HTTPD_STOP_REASON reason)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] httpd_stop_handle() - reason = %d\n", reason));

    httpd_set_status(HTTPD_STATUS_STOP);
    sigint_cleanup(1);
    httpd_fb_status(HTTPD_STATUS_STOP, reason);
}

int httpd_fb_status(HTTPD_STATUS status, HTTPD_STOP_REASON reason)
{
    httpd_fb fb;

    HTTPD_DEBUGF((httpd, "[HTTPD] httpd_fb_status() - httpd_fb_mbox = %p\n", httpd_fb_mbox));

    fb.status = status;
    fb.reason = reason;

    if(httpd_fb_mbox)
    {
        if(OS_BufQPush(httpd_fb_mbox, (void *)&fb) != OS_SUCCESS)
        {
            HTTPD_DEBUGF((httpd, "[HTTPD] httpd_fb_status() - Time out: can't add a job to queue\n"));
            return -1;
        }
    }

    return 0;
}

void httpd_proc_thread(void *arg)
{
    httpd_para *parameter = NULL;

    HTTPD_DEBUGF((httpd, "[HTTPD] httpd_proc_thread()\n"));

    parameter = (httpd_para *)ax_malloc(sizeof(httpd_para));

    /* Loop, processing httpd cmds. */
    for (;;)
    {
        /* Wait until there is something to do. */
        if (OS_BufQPop(httpd_proc_mbox, (void *)parameter, portMAX_DELAY) == OS_SUCCESS)
        {
            /* process this net job */
            if((parameter->fb_queue != NULL) || (parameter->str_ip_addr != NULL) || (parameter->str_port != NULL))
            {
                HTTPD_DEBUGF((httpd, "[HTTPD] httpd_proc_thread() - receive the job, fb_queue = %p\n", parameter->fb_queue));
                httpd_fb_mbox = parameter->fb_queue;
                httpd_main(parameter);
            }
            else
            {
                HTTPD_DEBUGF((httpd, "[HTTPD] httpd_proc_thread() - receive the job, default value\n"));
                httpd_fb_mbox = NULL;
                httpd_main(NULL);
            }
        }
    }
}

void httpd_set_status(HTTPD_STATUS status)
{
    HTTPD_DEBUGF((httpd, "[HTTPD] httpd_set_status() - status = %d\n", status));
    httpd_status = status;
}

HTTPD_RESULT httpd_init(void)
{
    portBASE_TYPE xReturn = pdFALSE;

    HTTPD_DEBUGF((httpd, "[HTTPD] httpd_init()\n"));

    /* Only create the job queue if it has not already been created, in
    case this function is called more than once. */
    if (httpd_proc_mbox == NULL)
    {
        //httpd_proc_mbox = xQueueCreate(1, sizeof(httpd_para));
        OS_BufQCreate(&httpd_proc_mbox, 1, sizeof(httpd_para));
    	if(httpd_proc_mbox == NULL)
        {
            HTTPD_DEBUGF((httpd, "[HTTPD] httpd_init(), xQueueCreate fail\n"));
            return HTTPD_RESULT_UNINIT;
        }
        //vQueueAddToRegistry(httpd_proc_mbox, "HttpdJob");
    }

    if (httpd_proc_task_handle == NULL)
    {
        xReturn = OS_TaskCreate(
            httpd_proc_thread,
            "httpd_proc",
            2500/sizeof(portSTACK_TYPE),
            NULL,
            OS_TASK_PRIO2,
            &httpd_proc_task_handle);

        if(httpd_proc_task_handle == NULL)
        {
            HTTPD_DEBUGF((httpd, "[HTTPD] httpd_init(), OS_TaskCreate fail, return vaule = %ld\n", xReturn));
            return HTTPD_RESULT_UNINIT;
        }

    httpd_set_status(HTTPD_STATUS_STOP);
    }

    return HTTPD_RESULT_SUCCESS;
}

HTTPD_RESULT httpd_stop(void)
{
    HTTPD_STATUS status = httpd_get_status();

    HTTPD_DEBUGF((httpd, "[HTTPD] httpd_stop() - currently status = %d\n", status));

    switch(status)
    {
        case HTTPD_STATUS_RUN:
            httpd_set_status(HTTPD_STATUS_STOPPING);
            return HTTPD_RESULT_WAITING;   // Need to wait feedback or polling status
        case HTTPD_STATUS_STOPPING:
            return HTTPD_RESULT_WAITING;   // Need to wait feedback or polling status
        case HTTPD_STATUS_STOP:
            return HTTPD_RESULT_SUCCESS;   // Has been stop
        case HTTPD_STATUS_UNINIT:
            return HTTPD_RESULT_UNINIT;    // Do not init
    }

    return HTTPD_RESULT_SUCCESS;
}

HTTPD_RESULT httpd_start(httpd_para *parameter)
{
    HTTPD_STATUS status = httpd_get_status();
    httpd_para para;

    HTTPD_DEBUGF((httpd, "[HTTPD] httpd_start() - currently status = %d\n", status));

    switch(status)
    {
        case HTTPD_STATUS_RUN:
            return HTTPD_RESULT_SUCCESS;  // Has been run
        case HTTPD_STATUS_STOPPING:
            return HTTPD_RESULT_WAITING;  // Need to wait feedback or polling status
        case HTTPD_STATUS_UNINIT:
            return HTTPD_RESULT_UNINIT;   // Do not init
        case HTTPD_STATUS_STOP:
            break;
    }

    if(parameter)
    {
        memcpy(&para, parameter, sizeof(httpd_para));
    }
    else
    {
        memset(&para, 0, sizeof(httpd_para));
    }

    if(OS_BufQPush(httpd_proc_mbox, (void *)&para) != OS_SUCCESS)
    {
        HTTPD_DEBUGF((httpd, "[HTTPD] httpd_start() - Time out: can't add a job to queue\n"));
        return HTTPD_RESULT_QUEUE_FULL;
    }

    return HTTPD_RESULT_WAITING; // need to wait feedback or polling status
}

HTTPD_STATUS httpd_get_status(void)
{
    return httpd_status;
}

#if 0
HTTPD_RESULT httpd_deinit(void)
{
    printf("[HTTPD] httpd_deinit()\n");
    HTTPD_DEBUGF((httpd, "[HTTPD] httpd_deinit()\n"));
    httpd_set_status(HTTPD_STATUS_STOPPING);

    if (httpd_proc_mbox != NULL)
    {
        vQueueDelete(httpd_proc_mbox);
        httpd_proc_mbox = NULL;
    }

    if(httpd_proc_task_handle != NULL)
    {
        OS_TaskDelete(httpd_proc_task_handle);
        httpd_proc_task_handle = NULL;
    }

    return HTTPD_RESULT_SUCCESS;
}
#endif


