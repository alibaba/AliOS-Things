/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __COAP_INTERNAL_H__
#define __COAP_INTERNAL_H__
#include "CoAPNetwork.h"
#include "CoAPExport.h"
#include "lite-list.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct
{
    void                    *list_mutex;
    struct list_head         list;
    unsigned char            count;
    unsigned char            maxcount;
}CoAPList;


typedef struct
{
    unsigned short           message_id;
    NetworkContext           *p_network;
    CoAPEventNotifier        notifier;
    unsigned char            *sendbuf;
    unsigned char            *recvbuf;
    CoAPList                 sendlist;
    CoAPList                 obsserver;
    CoAPList                 obsclient;
    CoAPList                 resource;
    unsigned int             waittime;
    void                     *appdata;
    void                     *mutex;
}CoAPIntContext;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
