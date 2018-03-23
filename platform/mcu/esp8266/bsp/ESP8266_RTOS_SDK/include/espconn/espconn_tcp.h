/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __ESPCONN_TCP_H__
#define __ESPCONN_TCP_H__

#ifndef ESPCONN_TCP_DEBUG
#define ESPCONN_TCP_DEBUG LWIP_DBG_OFF
#endif

#include "espconn/espconn.h"

#ifndef ESPCONN_TCP_TIMER
#define ESPCONN_TCP_TIMER 40
#endif

#define  espconn_keepalive_enable(pcb)   ((pcb)->so_options |= SOF_KEEPALIVE)
#define  espconn_keepalive_disable(pcb)   ((pcb)->so_options &= ~SOF_KEEPALIVE)

/******************************************************************************
 * FunctionName : espconn_kill_oldest_pcb
 * Description  : A oldest incoming connection has been killed.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

extern void espconn_kill_oldest_pcb(void);

/******************************************************************************
 * FunctionName : espconn_tcp_disconnect
 * Description  : A new incoming connection has been disconnected.
 * Parameters   : espconn -- the espconn used to disconnect with host
 * Returns      : none
*******************************************************************************/

extern void espconn_tcp_disconnect(espconn_msg* pdiscon, u8 type);

/******************************************************************************
 * FunctionName : espconn_tcp_client
 * Description  : Initialize the client: set up a connect PCB and bind it to
 *                the defined port
 * Parameters   : espconn -- the espconn used to build client
 * Returns      : none
*******************************************************************************/

extern sint8 espconn_tcp_client(struct espconn* espconn);

/******************************************************************************
 * FunctionName : espconn_tcp_server
 * Description  : Initialize the server: set up a listening PCB and bind it to
 *                the defined port
 * Parameters   : espconn -- the espconn used to build server
 * Returns      : none
*******************************************************************************/

extern sint8 espconn_tcp_server(struct espconn* espconn);

#endif /* __CLIENT_TCP_H__ */

