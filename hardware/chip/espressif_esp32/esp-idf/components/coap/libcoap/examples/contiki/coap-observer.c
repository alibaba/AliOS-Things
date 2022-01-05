/* coap-server.c -- Example CoAP server using Contiki and libcoap
 *
 * Copyright (C) 2011,2015,2018-2019 Olaf Bergmann <bergmann@tzi.org> and others
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "coap_config.h"
#include "net/uip-debug.h"

#include <string.h>

#include "coap.h"

static coap_context_t *coap_context;

/* Where the resource to subscribe is hosted */
static coap_address_t dst;

/* The resource to observe */
static char resource[] = "/s/light";

/* when did the last notify arrive? (0 == never) */
static coap_tick_t last_seen = 0;

PROCESS(coap_server_process, "CoAP server process");
AUTOSTART_PROCESSES(&coap_server_process);
/*---------------------------------------------------------------------------*/
void
init_coap() {
  coap_address_t listen_addr;

  coap_address_init(&listen_addr);
  listen_addr.port = UIP_HTONS(COAP_DEFAULT_PORT);

#ifdef WITH_CONTIKI
  /* initialize uIP address for SLAAC */
  uip_ip6addr(&listen_addr.addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&listen_addr.addr, &uip_lladdr);
  uip_ds6_addr_add(&listen_addr.addr, 0, ADDR_AUTOCONF);

  uip_debug_lladdr_print(&uip_lladdr);
  printf("\r\n");
  uip_debug_ipaddr_print(&listen_addr.addr);
  printf("\r\n");
#endif /* WITH_CONTIKI */

#ifdef WITH_CONTIKI
  printf("tentative address: [%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]:%d\r\n",
         listen_addr.addr.u8[0], listen_addr.addr.u8[1],
         listen_addr.addr.u8[2], listen_addr.addr.u8[3],
         listen_addr.addr.u8[4], listen_addr.addr.u8[5],
         listen_addr.addr.u8[6], listen_addr.addr.u8[7],
         listen_addr.addr.u8[8], listen_addr.addr.u8[9],
         listen_addr.addr.u8[10], listen_addr.addr.u8[11],
         listen_addr.addr.u8[12], listen_addr.addr.u8[13],
         listen_addr.addr.u8[14], listen_addr.addr.u8[15] ,
         uip_ntohs(listen_addr.port));
#endif

  coap_context = coap_new_context(&listen_addr);

  coap_set_log_level(LOG_DEBUG);

  if (!coap_context)
    coap_log(LOG_CRIT, "cannot create CoAP context\r\n");
}

void
message_handler(struct coap_context_t  *ctx,
                const coap_address_t *remote,
                coap_pdu_t *sent,
                coap_pdu_t *received,
                const coap_tid_t id) {
  /* send ACK if received message is confirmable (i.e. a separate response) */
  coap_send_ack(ctx, remote, received);

  coap_log(LOG_DEBUG, "** process incoming %d.%02d response:\n",
        (received->hdr->code >> 5), received->hdr->code & 0x1F);
  coap_show_pdu(LOG_WARNING, received);

  coap_ticks(&last_seen);
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coap_server_process, ev, data)
{
  coap_pdu_t *request;
  coap_uri_t uri;
  PROCESS_BEGIN();

  init_coap();

  if (!coap_context) {
    coap_log(LOG_EMERG, "cannot create context\n");
    PROCESS_EXIT();
  }

  coap_register_response_handler(coap_context, message_handler);

  /* setup subscription request */

  coap_address_init(&dst);
  dst.port = uip_htons(COAP_DEFAULT_PORT);
  uip_ip6addr(&dst.addr, 0xaaaa, 0, 0, 0, 0x206, 0x98ff, 0xfe00, 0x232);
  /* uip_ip6addr(&dst.addr, 0xfe80, 0, 0, 0, 0x206, 0x98ff, 0xfe00, 0x232); */

  request = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_GET,
                          coap_new_message_id(coap_context),
                          COAP_DEFAULT_MTU);

  coap_split_uri((unsigned char *)resource, strlen(resource), &uri);

  if (uri.port != COAP_DEFAULT_PORT) {
    unsigned char portbuf[2];
    coap_add_option(request, COAP_OPTION_URI_PORT,
                    coap_encode_var_safe(portbuf, sizeof(portbuf),
                                         uri.port),
                    portbuf);
  }

  if (uri.path.length) {
#define BUFSIZE 20
    unsigned char _buf[BUFSIZE];
    unsigned char *buf = _buf;
    size_t buflen;
    int res;

    buflen = BUFSIZE;
#undef BUFSIZE
    res = coap_split_path(uri.path.s, uri.path.length, buf, &buflen);

    while (res--) {
      coap_add_option(request, COAP_OPTION_URI_PATH,
                      coap_opt_length(buf), coap_opt_value(buf));

      buf += coap_opt_size(buf);
    }
  }

  coap_add_option(request, COAP_OPTION_OBSERVE, 0, NULL);
  {
    unsigned char buf[2];
    prng(buf, 2);
    coap_add_option(request, COAP_OPTION_TOKEN, 2, buf);
  }

  if (COAP_INVALID_TID == coap_send_confirmed(coap_context, &dst, request))
    coap_delete_pdu(request);

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      coap_read(coap_context);        /* read received data */
      coap_dispatch(coap_context); /* and dispatch PDUs from receivequeue */
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
