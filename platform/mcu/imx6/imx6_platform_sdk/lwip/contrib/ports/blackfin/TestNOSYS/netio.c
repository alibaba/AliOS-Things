#include "netio.h"

#include "lwip/opt.h"
#include "lwip/tcp.h"

/* See http://www.nwlab.net/art/netio/netio.html to get the netio tool */

#if LWIP_TCP
static err_t netio_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{

  LWIP_UNUSED_ARG(arg);

  if (err == ERR_OK && p != NULL) {
    tcp_recved(pcb, p->tot_len);
    pbuf_free(p);
  } else {
    pbuf_free(p);
  }

  if (err == ERR_OK && p == NULL) {
    tcp_arg(pcb, NULL);
    tcp_sent(pcb, NULL);
    tcp_recv(pcb, NULL);
    tcp_close(pcb);
  }

  return ERR_OK;
}

static err_t netio_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  tcp_arg(pcb, NULL);
  tcp_sent(pcb, NULL);
  tcp_recv(pcb, netio_recv);
  return ERR_OK;
}

void netio_init(void)
{
  struct tcp_pcb *pcb;

  pcb = tcp_new();
  tcp_bind(pcb, IP_ADDR_ANY, 18767);
  pcb = tcp_listen(pcb);
  tcp_accept(pcb, netio_accept);
}
#endif /* LWIP_TCP */
