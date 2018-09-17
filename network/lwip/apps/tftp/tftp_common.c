/****************************************************************//**
 *
 * @file tftp_common.c
 *
 * @author   Logan Gunthorpe <logang@deltatee.com>
 *           Dirk Ziegelmeier <dziegel@gmx.de>
 *
 * @brief    Trivial File Transfer Protocol (RFC 1350)
 *
 * Copyright (c) Deltatee Enterprises Ltd. 2013
 * All rights reserved.
 *
 ********************************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification,are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Logan Gunthorpe <logang@deltatee.com>
 *         Dirk Ziegelmeier <dziegel@gmx.de>
 *
 */

/**
 * @defgroup tftp TFTP server
 * @ingroup apps
 *
 * This is simple TFTP server for the lwIP raw API.
 */


#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"
#include "lwip/apps/tftp.h"

#include <string.h>

void
tftp_send_error(struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port,
                tftp_error_t code, const char *str)
{
  int str_length = strlen(str);
  struct pbuf* p;
  u16_t* payload;

  p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(TFTP_HEADER_LENGTH + str_length + 1), PBUF_RAM);
  if(p == NULL) {
    return;
  }

  payload = (u16_t*) p->payload;
  payload[0] = PP_HTONS(TFTP_ERROR);
  payload[1] = lwip_htons(code);
  MEMCPY(&payload[2], str, str_length + 1);

  udp_sendto(pcb, p, addr, port);
  pbuf_free(p);
}

void
tftp_send_ack(struct udp_pcb *pcb, const ip_addr_t *addr, u16_t port, u16_t blknum)
{
  struct pbuf* p;
  u16_t* payload;

  p = pbuf_alloc(PBUF_TRANSPORT, TFTP_HEADER_LENGTH, PBUF_RAM);
  if(p == NULL) {
    return;
  }
  payload = (u16_t*) p->payload;

  payload[0] = PP_HTONS(TFTP_ACK);
  payload[1] = lwip_htons(blknum);
  udp_sendto(pcb, p, addr, port);
  pbuf_free(p);
}
