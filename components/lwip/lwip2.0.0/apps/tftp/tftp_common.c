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

#include "lwip/opt.h"
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

static void tftp_get_done(int error, int len)
{
    if (error == 0) {
        cli_printf("tftp received len:%d done.\r\n", len);
    } else {
        cli_printf("tftp received failed.\r\n");
    }
    char _buf[PATH_MAX] = {0};
    cli_printf("(%s:%s)#", cli_task_get_console_name(), aos_getcwd(_buf, sizeof(_buf)));
}

extern tftp_context_t client_ctx;
static int tftp_cmd(int argc, char **argv)
{
    if (argc < 3) {
        goto tftp_print_usage;
    }

    if (strncmp(argv[1], "server", 6) == 0) {
        if (strncmp(argv[2], "start", 5) == 0) {
            err_t err = tftp_server_start();
            cli_printf("tftp start server %s\r\n", err == ERR_OK ? "done" : "failed");
            return 0;
        } else if (strncmp(argv[2], "stop", 4) == 0) {
            tftp_server_stop();
            cli_printf("tftp stop server done\r\n");
            return 0;
        }
        goto tftp_print_usage;
    } else if (strncmp(argv[1], "get", 3) == 0) {
        ip_addr_t dst_addr;
        uint16_t port;

        ipaddr_aton(argc == 6 ? argv[2] : "10.0.0.2", &dst_addr);
        port = (uint16_t)atoi(argv[3]);
        tftp_client_set_server_port(port);
        tftp_client_get(&dst_addr, argv[argc - 2], argv[argc - 1], &client_ctx, tftp_get_done);
        return 0;
    }

tftp_print_usage:
    cli_printf("usage:\r\n"
               "  tftp server <start|stop>\r\n"
               "  tftp get $server_ip server_src_path device_dest_path\r\n"
               "eg:\r\n"
               "  1. get file from server:\r\n"
               "     tftp get 192.168.0.100 test.txt /tmp/test.txt\r\n");
    return 0;
}


#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

#ifdef AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(tftp_cmd, tftp, TFTP command)
#endif

