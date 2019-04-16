/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */
#include "lwip/opt.h"
#include "lwip/pbuf.h"
#include "lwip/debug.h"
#include "lwip/def.h"

#ifndef u32
#define u32 uint32_t
#endif
#ifndef u16
#define u16 uint16_t
#endif
#ifndef u8
#define u8  uint8_t
#endif

#define IPHDR_LEN 20
#define TCPS_MAX  11

/* IP packet type */
#define IPPROTO_ICMP    0x01
#define IPPROTO_IGMP    0x02
#define IPPROTO_TCP     0x06
#define IPPROTO_UDP     0x11

/* port type */
#define PORT_DNS        0x35
#define PORT_DHCP_SRV   0x43
#define PORT_DHCP_CLT   0x44

/* TCP packet state flag */
#define TCPF_SYN        0x02
#define TCPF_RST        0x04
#define TCPF_ACK        0x10
#define TCPF_FINACK     0x11
#define TCPF_SYNACK     0x12
#define TCPF_RSTACK     0x14
#define TCPF_PSHACK     0x18
#define TCPF_FINPSHACK  0x19

/* ICMP packet type */
#define ICMP_TYPE_ECHO_REPLY    0x00
#define ICMP_TYPE_ECHO_REQST    0x08

/* print buffer length */
#define PKT_INFO_BUFF_SIZE      80

/* net layer type enum */
typedef enum
{
    NL_NONE     = 0,
    NL_IP4      = 1,
    NL_IP6      = 2,
    NL_MAX
} DBG_NET_E;

/* transport layer type enum */
typedef enum
{
    TL_NONE     = 0,
    TL_ICMP     = 1,
    TL_IGMP     = 2,
    TL_TCP      = 3,
    TL_UDP      = 4,
    TL_MAX
} DBG_TRANS_E;

/* sub-transport layer type enum */
typedef enum
{
    STL_NONE            = 0,

    /* TCP subtype */
    STL_TCP_SYN         = 1,
    STL_TCP_SYNACK      = 2,
    STL_TCP_ACK         = 3,
    STL_TCP_PSHACK      = 4,
    STL_TCP_FINPSHACK   = 5,
    STL_TCP_FINACK      = 6,
    STL_TCP_RSTACK      = 7,
    STL_TCP_RST         = 8,

    /* UDP subtype */
    STL_UDP_DHCPREQ     = 9,
    STL_UDP_DHCPREP     = 10,
    STL_UDP_DNS         = 11,
    /* ICMP subtype */
    STL_ICMP_PINGREQ    = 12,
    STL_ICMP_PINGREP    = 13,
    STL_MAX
} DBG_SUBTRANS_E;

/* IP4 packet info struct */
typedef struct
{
    u32_t   ip_src;
    u32_t   ip_dst;
    u16_t   len;
    u16_t   id;
    u16_t   offset;
    u8_t    hlen;
    u8_t    type;
    u8_t    ttl;
    u8_t    padding[3];
} DBG_IP4_T;

typedef struct
{
//tbd
} DBG_IP6_T;

/* TCP packet info struct */
typedef struct
{
    u16_t   port_src;
    u16_t   port_dst;
    u32_t   seq;
    u32_t   ack;
    u16_t   wnd;
    u16_t   dlen;
} DBG_TCP_T;

/* UDP packet info struct */
typedef struct
{
    u16_t   port_src;
    u16_t   port_dst;
    u16_t   len;
    u8_t    padding[2];
} DBG_UDP_T;

/* ICMP packet info struct */
typedef struct
{
    u8_t    type;
    u8_t    code;
    u16_t   id;
    u16_t   seq;
    u8_t    padding[2];
} DBG_ICMP_T;

/* IGMP packet info struct */

typedef struct

{
    u32_t   group_addr;
    u8_t    type;
    u8_t    padding[3];
} DBG_IGMP_T;

/* packet info struct */
typedef struct pkt_info
{
    DBG_NET_E      net;
    DBG_TRANS_E    trans;
    DBG_SUBTRANS_E subtrans;
    u32_t       pkt_id;
    union{
        DBG_IP4_T   ip4;
        DBG_IP6_T   ip6;
    }nl;
    union{
        DBG_TCP_T  tcp;
        DBG_UDP_T  udp;
        DBG_ICMP_T icmp;
        DBG_IGMP_T igmp;
    }tl;
} DBG_PKT_INFO_T;

/* net layer type, it should be matched with DBG_NET_E */
static char s_net_type[NL_MAX][8] =
{
    "NONE",
    "IP4",
    "IP6",
};

/* transport layer type, it should be matched with DBG_TRANS_E */
static char s_trans_type[TL_MAX][8] =
{
    "NONE",
    "ICMP",
    "IGMP",
    "TCP",
    "UDP"
};

/* sub-transport layer type, it should be matched with DBG_SUBTRANS_E */
static char s_subtrans_type[STL_MAX][16] =
{
    "NONE",
    /* TCP */
    "TCP_SYN",
    "TCP_SYNACK",
    "TCP_ACK",
    "TCP_PSHACK",
    "TCP_FINPSHACK",
    "TCP_FINACK",
    "TCP_RSTACK",
    "TCP_RST",
    /* UDP */
    "DHCP_BOOTREQ",
    "DHCP_BOOTREP",
    "DNS",
    /* ICMP */
    "ICMP_PINGREQ",
    "ICMP_PINGREP"
};

/* TCP state */
static char s_tcp_state[TCPS_MAX][24] =
{
    "TCPS_CLOSED",          // 0 - closed */
    "TCPS_LISTEN",          // 1 - listening for connection */
    "TCPS_SYN_SENT",        // 2 - active, have sent syn */
    "TCPS_SYN_RECEIVED",    // 3 - have send and received syn */
    // states < "TCPS_ESTABLISHED are those where connections not established
    "TCPS_ESTABLISHED",     // 4 - established */
    "TCPS_CLOSE_WAIT",      // 5 - rcvd fin, waiting for close */
    // states > "TCPS_CLOSE_WAIT are those where user has closed
    "TCPS_FIN_WAIT_1",      // 6 - have closed, sent fin */
    "TCPS_CLOSING",         // 7 - closed xchd FIN; await FIN ACK */
    "TCPS_LAST_ACK",        // 8 - had fin and close; await FIN ACK */
    // states > "TCPS_CLOSE_WAIT && < "TCPS_FIN_WAIT_2 await ACK of FIN */
    "TCPS_FIN_WAIT_2",      // 9 - have closed, fin is acked
    "TCPS_TIME_WAIT"        // 10 - in 2*msl quiet wait after close */
};

void lwip_pkt_print(char* note_ptr, struct pbuf *pbuf, struct netif* netif)
{
    int  len = NULL;
    char* ptr = NULL;
    DBG_PKT_INFO_T dbg_pkt_info = {0};

    if(pbuf == NULL){
	return ;
    }

    ptr = pbuf->payload;
    len = pbuf->len;

    if( len <= IPHDR_LEN || NULL == ptr ){
        return ;
    }

    /* check if IP v4 header */
    if( 0x40 == (ptr[0] & 0xF0) )
    {
        DBG_IP4_T* ip4_ptr = &(dbg_pkt_info.nl.ip4);
        u8 ip4_hlen = 0;
        ip4_ptr->hlen = 4*(ptr[0] & 0x0F);
        ip4_hlen = ip4_ptr->hlen;

        /* check IP header length */
        if( len <= ip4_hlen ){
#if PKTPRINT_DEBUG
            LWIP_DEBUGF(PKTPRINT_DEBUG, ("LwIP: PacketInfoIp() - invalid len %d < iplen %d\n", len, ip4_hlen));
#endif
            return ;
        }

        /* get IP packet information */
        dbg_pkt_info.net = NL_IP4;
        ip4_ptr->len = (u16)(256*ptr[2] + ptr[3]);
        ip4_ptr->id = (u16)(256*ptr[4] + ptr[5]);
        ip4_ptr->offset = 2064*(0x1F & ptr[6]) + 8*ptr[7];   /* 2064 = 256*8 */
        ip4_ptr->type = ptr[9];
        ip4_ptr->ip_src = ntohl(*((u32*)(ptr + 12)));
        ip4_ptr->ip_dst = ntohl(*((u32*)(ptr + 16)));

        /* classify IP sub-type */
        switch( ip4_ptr->type ){
        // ICMP
        case IPPROTO_ICMP:
            dbg_pkt_info.trans = TL_ICMP;
            dbg_pkt_info.subtrans = STL_NONE;
            if( 0 == ip4_ptr->offset ){
                DBG_ICMP_T* icmp_ptr = &(dbg_pkt_info.tl.icmp);
                icmp_ptr->type = ptr[ip4_hlen];
                icmp_ptr->code = ptr[ip4_hlen + 1];
                switch( icmp_ptr->type ){
                case ICMP_TYPE_ECHO_REQST:
                    dbg_pkt_info.subtrans = STL_ICMP_PINGREQ;
                    icmp_ptr->id  = (u16)(256*ptr[ip4_hlen + 4] + ptr[ip4_hlen + 5]);
                    icmp_ptr->seq = (u16)(256*ptr[ip4_hlen + 6] + ptr[ip4_hlen + 7]);
                    break;
                case ICMP_TYPE_ECHO_REPLY:
                    dbg_pkt_info.subtrans = STL_ICMP_PINGREP;
                    icmp_ptr->id  = (u16)(256*ptr[ip4_hlen + 4] + ptr[ip4_hlen + 5]);
                    icmp_ptr->seq = (u16)(256*ptr[ip4_hlen + 6] + ptr[ip4_hlen + 7]);
                    break;
                default:
                    icmp_ptr->id  = 0;
                    icmp_ptr->seq = 0;
                    break;
                }
            }
            break;
        // IGMP
        case IPPROTO_IGMP:
            dbg_pkt_info.trans = TL_IGMP;
            dbg_pkt_info.subtrans = STL_NONE;
            if( 0 == ip4_ptr->offset ){
                DBG_IGMP_T* igmp_ptr = &(dbg_pkt_info.tl.igmp);
                igmp_ptr->type = ptr[ip4_hlen];
                igmp_ptr->group_addr = ntohl(*((u32*)(ptr + ip4_hlen + 4)));
            }
            break;
        // TCP
        case IPPROTO_TCP:
            dbg_pkt_info.trans = TL_TCP;
            dbg_pkt_info.subtrans = STL_NONE;
            if( 0 == ip4_ptr->offset ){
                DBG_TCP_T* tcp_ptr = &(dbg_pkt_info.tl.tcp);
                tcp_ptr->port_src = (u16)(256*ptr[ip4_hlen]   + ptr[ip4_hlen+1]);
                tcp_ptr->port_dst = (u16)(256*ptr[ip4_hlen+2] + ptr[ip4_hlen+3]);
                tcp_ptr->seq = ntohl(*((u32*)(ptr+ip4_hlen+4)));
                tcp_ptr->ack = ntohl(*((u32*)(ptr+ip4_hlen+8)));
                tcp_ptr->dlen = (u16)(ip4_ptr->len - ip4_ptr->hlen - (ptr[ip4_hlen+12]>>2));
                tcp_ptr->wnd = (u16)(256*ptr[ip4_hlen+14] + ptr[ip4_hlen+15]);
                switch( ptr[ip4_hlen+13] ){
                case TCPF_SYN:          dbg_pkt_info.subtrans = STL_TCP_SYN;          break;
                case TCPF_RST:          dbg_pkt_info.subtrans = STL_TCP_RST;          break;
                case TCPF_ACK:          dbg_pkt_info.subtrans = STL_TCP_ACK;          break;
                case TCPF_FINACK:       dbg_pkt_info.subtrans = STL_TCP_FINACK;       break;
                case TCPF_SYNACK:       dbg_pkt_info.subtrans = STL_TCP_SYNACK;       break;
                case TCPF_RSTACK:       dbg_pkt_info.subtrans = STL_TCP_RSTACK;       break;
                case TCPF_PSHACK:       dbg_pkt_info.subtrans = STL_TCP_PSHACK;       break;
                case TCPF_FINPSHACK:    dbg_pkt_info.subtrans = STL_TCP_FINPSHACK;    break;
                default:
                    break;
                }
            }
            break;
        // UDP
        case IPPROTO_UDP:
            dbg_pkt_info.trans = TL_UDP;
            dbg_pkt_info.subtrans = STL_NONE;
            if( 0 == ip4_ptr->offset ){
                DBG_UDP_T* udp_ptr = &(dbg_pkt_info.tl.udp);
                udp_ptr->port_src = (u16)(256*ptr[ip4_hlen]   + ptr[ip4_hlen+1]);
                udp_ptr->port_dst = (u16)(256*ptr[ip4_hlen+2] + ptr[ip4_hlen+3]);
                udp_ptr->len = (u16)(256*ptr[ip4_hlen+4] + ptr[ip4_hlen+5]);
                /* switch for source port */
                switch(udp_ptr->port_src){
                case PORT_DHCP_CLT: dbg_pkt_info.subtrans = STL_UDP_DHCPREQ;  break;
                case PORT_DHCP_SRV: dbg_pkt_info.subtrans = STL_UDP_DHCPREP;  break;
                case PORT_DNS:      dbg_pkt_info.subtrans = STL_UDP_DNS;      break;
                default:
                    break;
                }
                /* check type for dest port */
                if( PORT_DNS == udp_ptr->port_dst ){
                    dbg_pkt_info.subtrans = STL_UDP_DNS;
                }
            }
            break;
        default:
            dbg_pkt_info.trans = TL_NONE;
            dbg_pkt_info.subtrans = STL_NONE;
            break;

        }
    }

    if(dbg_pkt_info.net == NL_IP4)
    {
        DBG_IP4_T *ip4_ptr = &(dbg_pkt_info.nl.ip4);
        char * type_ptr = NULL;
        char info_str[PKT_INFO_BUFF_SIZE] = {0};

        /* set type string */
        if( STL_NONE != dbg_pkt_info.subtrans )
        {
           type_ptr = s_subtrans_type[dbg_pkt_info.subtrans];
        }
        else if( TL_NONE != dbg_pkt_info.trans )
        {
           type_ptr = s_trans_type[dbg_pkt_info.trans];
        }
        else
        {
           type_ptr = s_net_type[dbg_pkt_info.net];
        }

        switch( dbg_pkt_info.net ) {
           case NL_IP4:
              if( 0 == ip4_ptr->offset )
              {
                 /* set packet info string by transportation type*/
                 switch( dbg_pkt_info.trans )
                 {
                    case TL_ICMP:
                    {
                        DBG_ICMP_T* icmp_ptr = &(dbg_pkt_info.tl.icmp);
                        snprintf(info_str, (PKT_INFO_BUFF_SIZE-1),
                          "Type(%d), Code(%d), Id(%x), Seq(%x)",
                           icmp_ptr->type, icmp_ptr->code, icmp_ptr->id, icmp_ptr->seq);
                    }
                    break;
                    case TL_IGMP:
                    {
                        DBG_IGMP_T* igmp_ptr = &(dbg_pkt_info.tl.igmp);
                        snprintf(info_str, (PKT_INFO_BUFF_SIZE-1),
                          "Type(%d), GroupAddr(0x%08x)",
                          igmp_ptr->type, igmp_ptr->group_addr);
                    }
                    break;
                    case TL_TCP:
                    {
                       DBG_TCP_T* tcp_ptr = &(dbg_pkt_info.tl.tcp);
                       snprintf(info_str, (PKT_INFO_BUFF_SIZE-1),
                        "S:%x, A:%x, W:%d, l:%d, %d -> %d",
                        tcp_ptr->seq, tcp_ptr->ack, tcp_ptr->wnd,
                        tcp_ptr->dlen, tcp_ptr->port_src, tcp_ptr->port_dst);
                    }
                    break;
                    case TL_UDP:
                    {
                        DBG_UDP_T* udp_ptr = &(dbg_pkt_info.tl.udp);
                        snprintf(info_str, (PKT_INFO_BUFF_SIZE-1),
                          "%d -> %d", udp_ptr->port_src, udp_ptr->port_dst);
                    }
                    break;
                    default:
                       //error
                    break;
                }
#if PKTPRINT_DEBUG
                LWIP_DEBUGF(PKTPRINT_DEBUG, ("[ LwIP ] %s, pkt:%x, netif(%x), IPID(%x), %s, %s(%d)\n",
                   note_ptr, pbuf, netif, ip4_ptr->id, info_str,
                   type_ptr, ip4_ptr->len));
#endif
            }
            else
            {
#if PKTPRINT_DEBUG
               LWIP_DEBUGF(PKTPRINT_DEBUG, ("[ LwIP ] %s, pkt:%x, netif(%x), IPID(%x), FRAG_OFFSET(%d), %s(%d)\n",
                   note_ptr, pbuf, netif, ip4_ptr->id, ip4_ptr->offset,
                   type_ptr, ip4_ptr->len));
#endif
            }
            break;
            case NL_NONE:
            default:
#if PKTPRINT_DEBUG
               LWIP_DEBUGF(PKTPRINT_DEBUG, ("[ LwIP ] %s, pkt:%x, netif(%x), UNKNOWN TYPE(%d)\n",
                  note_ptr, pbuf, netif, pbuf->len));
#endif
           break;

      }
   }
}
