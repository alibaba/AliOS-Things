#include "test_ip4_route.h"

#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "lwip/ip4.h"
#include "lwip/udp.h"
#include "lwip/inet_chksum.h"
#include "lwip/stats.h"
#include "lwip/prot/ip.h"
#include "lwip/prot/ip4.h"
#include "lwip/lwip_napt.h"
#include "lwip/priv/tcp_priv.h"

#include "lwip/tcpip.h"

#if !LWIP_IPV4 || !IP_REASSEMBLY || !MIB2_STATS || !IPFRAG_STATS
#error "This tests needs LWIP_IPV4, IP_REASSEMBLY; MIB2- and IPFRAG-statistics enabled"
#endif

#if !LWIP_NETIF_EXT_STATUS_CALLBACK
#error "This tests needs LWIP_NETIF_EXT_STATUS_CALLBACK enabled"
#endif

static struct netif sta;
static struct netif ap;

static int ap_cnt = 0;
static int sta_cnt = 0;
static u32_t last_src_addr = 0;
static u32_t last_dst_addr = 0;
static u16_t last_src_port = 0;

#if IP_NAPT
static int random_mock = -1;
/* Mock the esp-random to return 0 for easier result checking */
uint32_t esp_random(void) 
{
  if (random_mock == -1) {
    /* fall back to default "port/unix" rand (if other tests are launched with IP_NAPT ON) */
    return rand();
  }
  return random_mock; 
}
#endif

/* Setups/teardown functions */
static void
ip4route_setup(void)
{
  lwip_check_ensure_no_alloc(SKIP_POOL(MEMP_SYS_TIMEOUT));
}

static void
ip4route_teardown(void)
{
  lwip_check_ensure_no_alloc(SKIP_POOL(MEMP_SYS_TIMEOUT));
}

/* test helper functions */
static void debug_print_packet(struct pbuf *q)
{
    struct ip_hdr *iphdr = (struct ip_hdr *)q->payload;
    u16_t iphdr_hlen;

    iphdr_hlen = IPH_HL_BYTES(iphdr);
    fail_unless(0 == inet_chksum((struct ip_hdr *)q->payload, iphdr_hlen));

    ip4_debug_print(q);

    last_src_addr = iphdr->src.addr;
    last_dst_addr = iphdr->dest.addr;

    if (IPH_PROTO(iphdr) == IP_PROTO_UDP) {
      struct udp_hdr *udphdr = (struct udp_hdr *)(( u8_t *)iphdr + iphdr_hlen);
      udp_debug_print(udphdr);
      LWIP_DEBUGF(UDP_DEBUG, ("udp ("));
      ip_addr_debug_print_val(UDP_DEBUG, *ip_current_dest_addr());
      LWIP_DEBUGF(UDP_DEBUG, (", %"U16_F") <-- (", lwip_ntohs(udphdr->dest)));
      ip_addr_debug_print_val(UDP_DEBUG, *ip_current_src_addr());
      LWIP_DEBUGF(UDP_DEBUG, (", %"U16_F")\n", lwip_ntohs(udphdr->src)));
     last_src_port = udphdr->src;
      
   } else if (IPH_PROTO(iphdr) == IP_PROTO_TCP) {
       struct tcp_hdr* tcphdr;
      pbuf_header(q, -(s16_t)sizeof(struct ip_hdr));
      tcphdr = (struct tcp_hdr*)q->payload;
      tcp_debug_print(tcphdr);
      last_src_port = tcphdr->src;


   }

}

static err_t
ap_output(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
    ap_cnt++;
    debug_print_packet(q);
    LWIP_UNUSED_ARG(netif);
    LWIP_UNUSED_ARG(q);
    LWIP_UNUSED_ARG(ipaddr);

    return ERR_OK;
}

static err_t
sta_output(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
    sta_cnt++;
    debug_print_packet(q);
    LWIP_UNUSED_ARG(netif);
    LWIP_UNUSED_ARG(q);
    LWIP_UNUSED_ARG(ipaddr);

    return ERR_OK;
}

static err_t
sta_tx_func(struct netif *netif, struct pbuf *p)
{
  LWIP_UNUSED_ARG(netif);
  LWIP_UNUSED_ARG(p);
  return ERR_OK;
}

static err_t
ap_tx_func(struct netif *netif, struct pbuf *p)
{
  LWIP_UNUSED_ARG(netif);
  LWIP_UNUSED_ARG(p);
  return ERR_OK;
}

#if IP_NAPT
static struct pbuf*
test_create_tcp_packet(u32_t src_ip4, u32_t dst_ip4,
                   u16_t src_port, u16_t dst_port,
                   u32_t seqno, u32_t ackno, u8_t headerflags, u16_t wnd)
{
  struct pbuf *p, *q;
  ip_addr_t src_ip;
  ip_addr_t dst_ip;
  struct ip_hdr* iphdr;
  struct tcp_hdr* tcphdr;
  u16_t pbuf_len = (u16_t)(sizeof(struct ip_hdr) + sizeof(struct tcp_hdr));

  src_ip.u_addr.ip4.addr = src_ip4;
  dst_ip.u_addr.ip4.addr = dst_ip4;

  p = pbuf_alloc(PBUF_RAW, pbuf_len, PBUF_POOL);
  EXPECT_RETNULL(p != NULL);
  /* first pbuf must be big enough to hold the headers */
  EXPECT_RETNULL(p->len >= (sizeof(struct ip_hdr) + sizeof(struct tcp_hdr)));

  for(q = p; q != NULL; q = q->next) {
    memset(q->payload, 0, q->len);
  }

  iphdr = (struct ip_hdr*)p->payload;
  /* fill IP header */
  iphdr->dest.addr = ip_2_ip4(&dst_ip)->addr;
  iphdr->src.addr = ip_2_ip4(&src_ip)->addr;
  IPH_VHL_SET(iphdr, 4, IP_HLEN / 4);
  IPH_TOS_SET(iphdr, 0);
  IPH_LEN_SET(iphdr, htons(p->tot_len));
  iphdr->_ttl = 32;
  iphdr->_proto = IP_PROTO_TCP;
  IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN));

  /* let p point to TCP header */
  pbuf_header(p, -(s16_t)sizeof(struct ip_hdr));

  tcphdr = (struct tcp_hdr*)p->payload;
  tcphdr->src   = htons(src_port);
  tcphdr->dest  = htons(dst_port);
  tcphdr->seqno = htonl(seqno);
  tcphdr->ackno = htonl(ackno);
  TCPH_HDRLEN_SET(tcphdr, sizeof(struct tcp_hdr)/4);
  TCPH_FLAGS_SET(tcphdr, headerflags);
  tcphdr->wnd   = htons(wnd);

  /* calculate checksum */
  tcphdr->chksum = ip_chksum_pseudo(p,
          IP_PROTO_TCP, p->tot_len, &src_ip, &dst_ip);

  pbuf_header(p, sizeof(struct ip_hdr));

  return p;
}
#endif

static struct pbuf *
test_udp_create_test_packet(u16_t length, u16_t dst_port, u16_t src_port, u32_t dst_addr, u32_t src_addr)
{
  err_t err;
  u8_t ret;
  struct udp_hdr *uh;
  struct ip_hdr *ih;
  struct pbuf *p;
  const u8_t test_data[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

  p = pbuf_alloc(PBUF_TRANSPORT, length, PBUF_POOL);
  fail_unless(p != NULL);
  if (p == NULL) {
    return NULL;
  }
  fail_unless(p->next == NULL);
  err = pbuf_take(p, test_data, length);
  fail_unless(err == ERR_OK);

  /* add UDP header */
  ret = pbuf_add_header(p, sizeof(struct udp_hdr));
  fail_unless(!ret);
  uh = (struct udp_hdr *)p->payload;
  uh->chksum = 0;
  uh->dest = lwip_htons(dst_port);
  uh->src = lwip_htons(src_port);
  uh->len = lwip_htons(p->tot_len);
  /* add IPv4 header */
  ret = pbuf_add_header(p, sizeof(struct ip_hdr));
  fail_unless(!ret);
  ih = (struct ip_hdr *)p->payload;
  memset(ih, 0, sizeof(*ih));
  ih->dest.addr = dst_addr;
  ih->src.addr = src_addr;
  ih->_len = lwip_htons(p->tot_len);
  ih->_ttl = 32;
  ih->_proto = IP_PROTO_UDP;
  IPH_VHL_SET(ih, 4, sizeof(struct ip_hdr) / 4);
  IPH_CHKSUM_SET(ih, inet_chksum(ih, sizeof(struct ip_hdr)));
  return p;
}


static void
send_to_netif(struct netif *input_netif, struct pbuf *p)
{
  err_t err;

  if (p != NULL) {
    err = ip4_input(p, input_netif);
    fail_unless(err == ERR_OK);
  }
}

static err_t
testif_init_sta(struct netif *netif)
{
    netif->name[0] = 's';
    netif->name[1] = 't';
    netif->output = sta_output;
    netif->linkoutput = sta_tx_func;
    netif->mtu = 1500;
    netif->hwaddr_len = 6;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6 | NETIF_FLAG_LINK_UP;

    netif->hwaddr[0] = 0x02;
    netif->hwaddr[1] = 0x03;
    netif->hwaddr[2] = 0x04;
    netif->hwaddr[3] = 0x05;
    netif->hwaddr[4] = 0x06;
    netif->hwaddr[5] = 0x08;

    return ERR_OK;
}

static err_t
testif_init_ap(struct netif *netif)
{
  netif->name[0] = 'a';
  netif->name[1] = 'p';
  netif->output = ap_output;
  netif->linkoutput = ap_tx_func;
  netif->mtu = 1500;
  netif->hwaddr_len = 6;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6 | NETIF_FLAG_LINK_UP;

  netif->hwaddr[0] = 0x02;
  netif->hwaddr[1] = 0x03;
  netif->hwaddr[2] = 0x04;
  netif->hwaddr[3] = 0x05;
  netif->hwaddr[4] = 0x06;
  netif->hwaddr[5] = 0x07;

  return ERR_OK;
}


/* Test functions */
START_TEST(test_ip4_route_netif_forward)
{
#define UDP_PORT 1234
  ip4_addr_t addr, src_addr;
  ip4_addr_t netmask;
  ip4_addr_t gw;
  struct pbuf *p;
  LWIP_UNUSED_ARG(_i);

  /* setup station */
  IP4_ADDR(&addr, 1, 2, 4, 4);
  IP4_ADDR(&netmask, 255, 255, 255, 0);
  IP4_ADDR(&gw, 1, 2, 4, 10);
  netif_add(&sta, &addr, &netmask, &gw, &sta, testif_init_sta, ethernet_input);
  netif_set_up(&sta);

  /* setup access point */
  IP4_ADDR(&addr, 1, 2, 3, 5);
  IP4_ADDR(&netmask, 255, 255, 255, 0);
  IP4_ADDR(&gw, 1, 2, 3, 4);
  netif_add(&ap, &addr, &netmask, &gw, &ap, testif_init_ap, ethernet_input);
  netif_set_up(&ap);

  /* create packet and send it to the AP */
  IP4_ADDR(&addr, 1, 2, 4, 100);
  IP4_ADDR(&src_addr, 1, 2, 3, 5);
  p = test_udp_create_test_packet(16, UDP_PORT, UDP_PORT, addr.addr, src_addr.addr);
  send_to_netif(&ap, p);

  /* cleanup */
  netif_set_down(&ap);
  netif_remove(&ap);
  netif_set_down(&sta);
  netif_remove(&sta);

  fail_unless(ap_cnt == 0);
#if IP_FORWARD
  fail_unless(sta_cnt == 1);
  fail_unless(last_src_port == lwip_ntohs(UDP_PORT));
  fail_unless(last_src_addr == src_addr.addr);
#else
  /* if IP FORWARD disabled, no packet to be found on the other interface */
  fail_unless(sta_cnt == 0);
#endif
#undef UDP_PORT
}
END_TEST

#if IP_NAPT
START_TEST(test_ip4_route_netif_napt_udp)
{
#define UDP_PORT 1234
  ip4_addr_t addr, src_addr, sta_addr;
  ip4_addr_t netmask;
  ip4_addr_t gw;
  struct pbuf *p;
  LWIP_UNUSED_ARG(_i);

  /* setup station */
  IP4_ADDR(&sta_addr, 1, 2, 4, 4);
  IP4_ADDR(&netmask, 255, 255, 255, 0);
  IP4_ADDR(&gw, 1, 2, 4, 10);
  netif_add(&sta, &sta_addr, &netmask, &gw, &sta, testif_init_sta, ethernet_input);
  netif_set_up(&sta);

  /* setup access point */
  IP4_ADDR(&addr, 10, 0, 0, 1);
  IP4_ADDR(&netmask, 255, 255, 0, 0);
  IP4_ADDR(&gw, 10, 0, 0, 1);
  netif_add(&ap, &addr, &netmask, &gw, &ap, testif_init_ap, ethernet_input);
  netif_set_up(&ap);
  ip_napt_enable(addr.addr, 1);

  /* create packet and send it to the AP */
  IP4_ADDR(&addr, 1, 2, 4, 100);
  IP4_ADDR(&src_addr, 10, 0, 0, 2);
  p = test_udp_create_test_packet(16, UDP_PORT, UDP_PORT, addr.addr, src_addr.addr);
 
  random_mock = 1;
  send_to_netif(&ap, p);

  /* expect to see a random port and translated source address to be station address */
  fail_unless(last_src_port == lwip_ntohs(IP_NAPT_PORT_RANGE_START+random_mock));
  fail_unless(last_src_addr == sta_addr.addr);
  fail_unless(ap_cnt == 0);
  fail_unless(sta_cnt == 1);

  p = test_udp_create_test_packet(16, IP_NAPT_PORT_RANGE_START+random_mock, UDP_PORT, sta_addr.addr, addr.addr);
  send_to_netif(&sta, p);
  fail_unless(ap_cnt == 1);
  fail_unless(sta_cnt == 1);
  /* expect to see a random port and translated source address to be station address */
  fail_unless(last_src_port == lwip_ntohs(UDP_PORT));
  fail_unless(last_dst_addr == src_addr.addr);

  /* cleanup */
  netif_set_down(&ap);
  netif_remove(&ap);
  netif_set_down(&sta);
  netif_remove(&sta);

  IP4_ADDR(&addr, 10, 0, 0, 1);
  ip_napt_enable(addr.addr, 0);

#undef UDP_PORT
}
END_TEST

START_TEST(test_ip4_route_netif_napt_tcp)
{
#define TCP_PORT 2222
  ip4_addr_t addr, src_addr, sta_addr;
  ip4_addr_t netmask;
  ip4_addr_t gw;
  struct pbuf *p;
  LWIP_UNUSED_ARG(_i);

  /* setup station */
  IP4_ADDR(&sta_addr, 1, 2, 4, 4);
  IP4_ADDR(&netmask, 255, 255, 255, 0);
  IP4_ADDR(&gw, 1, 2, 4, 10);
  netif_add(&sta, &sta_addr, &netmask, &gw, &sta, testif_init_sta, ethernet_input);
  netif_set_up(&sta);

  /* setup access point */
  IP4_ADDR(&addr, 10, 0, 0, 1);
  IP4_ADDR(&netmask, 255, 255, 0, 0);
  IP4_ADDR(&gw, 10, 0, 0, 1);
  netif_add(&ap, &addr, &netmask, &gw, &ap, testif_init_ap, ethernet_input);
  netif_set_up(&ap);
  ip_napt_enable_no(ap.num, 1);

  /* create packet and send it to the AP */
  IP4_ADDR(&addr, 1, 2, 4, 100);
  IP4_ADDR(&src_addr, 10, 0, 0, 2);
  p = test_create_tcp_packet(src_addr.addr, addr.addr, TCP_PORT, TCP_PORT, 0, 0, TCP_SYN, 0);
 
  random_mock = 2;
  send_to_netif(&ap, p);

  /* expect to see a random port and translated source address to be station address */
  fail_unless(last_src_port == lwip_ntohs(IP_NAPT_PORT_RANGE_START+random_mock));
  fail_unless(last_src_addr == sta_addr.addr);
  fail_unless(ap_cnt == 0);
  fail_unless(sta_cnt == 1);

  p = test_create_tcp_packet(addr.addr, sta_addr.addr, TCP_PORT, IP_NAPT_PORT_RANGE_START+random_mock, 0, 0, TCP_SYN|TCP_ACK, 0);
  send_to_netif(&sta, p);
  fail_unless(ap_cnt == 1);
  fail_unless(sta_cnt == 1);
  /* expect to see a random port and translated source address to be station address */
  fail_unless(last_src_port == lwip_ntohs(TCP_PORT));
  fail_unless(last_dst_addr == src_addr.addr);

  /* cleanup */
  ip_napt_enable_no(ap.num, 0);
  netif_set_down(&ap);
  netif_remove(&ap);
  netif_set_down(&sta);
  netif_remove(&sta);

#undef TCP_PORT
}
END_TEST

START_TEST(test_ip4_route_netif_max_napt)
{
#define TCP_PORT 2222
  ip4_addr_t addr, src_addr, sta_addr;
  ip4_addr_t netmask;
  ip4_addr_t gw;
  struct pbuf *p;
  LWIP_UNUSED_ARG(_i);

  /* setup station */
  IP4_ADDR(&sta_addr, 1, 2, 4, 4);
  IP4_ADDR(&netmask, 255, 255, 255, 0);
  IP4_ADDR(&gw, 1, 2, 4, 10);
  netif_add(&sta, &sta_addr, &netmask, &gw, &sta, testif_init_sta, ethernet_input);
  netif_set_up(&sta);

  /* setup access point */
  IP4_ADDR(&addr, 10, 0, 0, 1);
  IP4_ADDR(&netmask, 255, 255, 0, 0);
  IP4_ADDR(&gw, 10, 0, 0, 1);
  netif_add(&ap, &addr, &netmask, &gw, &ap, testif_init_ap, ethernet_input);
  netif_set_up(&ap);
  ip_napt_enable_no(ap.num, 1);

  /* create packet and send it to the AP */
  IP4_ADDR(&addr, 1, 2, 4, 100);
  IP4_ADDR(&src_addr, 10, 0, 0, 2);
  for (int i=0; i<IP_NAPT_MAX*2; ++i) {
    random_mock = i;
    p = test_create_tcp_packet(src_addr.addr, addr.addr, TCP_PORT + i, TCP_PORT + i, 0, 0, TCP_SYN, 0);
    send_to_netif(&ap, p);

    if (i<IP_NAPT_MAX) {
      /* if translated pairs of addr/port still below IP_NAT_MAX -> expect to see an outgoing packet */
      fail_unless(last_src_port == lwip_ntohs(IP_NAPT_PORT_RANGE_START+random_mock));
      fail_unless(last_src_addr == sta_addr.addr);
      fail_unless(sta_cnt == 1+i);
      p = test_create_tcp_packet(addr.addr, sta_addr.addr, TCP_PORT+i, IP_NAPT_PORT_RANGE_START+random_mock, 0, 0, TCP_SYN | TCP_ACK, 0);
      send_to_netif(&sta, p);

    } else {
      /* if more than NAPT_MAX, no more packet to be forwarded */
      fail_unless(sta_cnt == IP_NAPT_MAX);
    }
  }

  /* moves time forward to test releasing: */
  lwip_sys_now += IP_NAPT_TIMEOUT_MS_TCP_DISCON + 1;
  p = test_create_tcp_packet(addr.addr, sta_addr.addr, TCP_PORT, IP_NAPT_PORT_RANGE_START+0, 0, 0, TCP_PSH, 0);
  send_to_netif(&sta, p);
  p = test_create_tcp_packet(src_addr.addr, addr.addr, TCP_PORT + IP_NAPT_MAX*2, TCP_PORT + IP_NAPT_MAX*2, 0, 0, TCP_PSH | TCP_ACK, 0);
  sta_cnt = 0;
  send_to_netif(&ap, p);
  /* should not be released yet, since all the TCP connections are active */
  fail_unless(sta_cnt == 0);  /* expect no packet forwarded */

  /* FIN the first connection so it could be released */
  p = test_create_tcp_packet(addr.addr, sta_addr.addr, TCP_PORT+0, IP_NAPT_PORT_RANGE_START+0, 0, 0, TCP_FIN, 0);
  send_to_netif(&sta, p);
  p = test_create_tcp_packet(src_addr.addr, addr.addr, TCP_PORT+0, TCP_PORT + 0, 0, 0, TCP_FIN | TCP_ACK, 0);
  send_to_netif(&ap, p);
  p = test_create_tcp_packet(addr.addr, sta_addr.addr, TCP_PORT+0, IP_NAPT_PORT_RANGE_START+0, 0, 0, TCP_ACK, 0);
  send_to_netif(&sta, p);
  /* moves time forward to test releasing: */
  lwip_sys_now += IP_NAPT_TIMEOUT_MS_TCP_DISCON + 1;

  /* now sending a new packet with max port, that should be forwarded */  
  p = test_create_tcp_packet(src_addr.addr, addr.addr, TCP_PORT + IP_NAPT_MAX*2, TCP_PORT + IP_NAPT_MAX*2, 0, 0, TCP_SYN, 0);
  random_mock = 0;
  sta_cnt = 0;
  send_to_netif(&ap, p);

  fail_unless(last_src_port == lwip_ntohs(IP_NAPT_PORT_RANGE_START+0));
  fail_unless(last_src_addr == sta_addr.addr);
  fail_unless(sta_cnt == 1); /* expect this packet gets forwarded */

  /* cleanup */
  netif_set_down(&ap);
  ip_napt_enable_no(ap.num, 0);
  netif_remove(&ap);
  netif_set_down(&sta);
  netif_remove(&sta);

#undef TCP_PORT
}
END_TEST
#endif

/** Create the suite including all tests for this module */
Suite *
ip4route_suite(void)
{
  testfunc tests[] = {
    TESTFUNC(test_ip4_route_netif_forward),
#if IP_NAPT
    TESTFUNC(test_ip4_route_netif_napt_udp),
    TESTFUNC(test_ip4_route_netif_napt_tcp),
    TESTFUNC(test_ip4_route_netif_max_napt),
    
#endif
  };
  return create_suite("IP4_ROUTE", tests, sizeof(tests)/sizeof(testfunc), ip4route_setup, ip4route_teardown);
}
