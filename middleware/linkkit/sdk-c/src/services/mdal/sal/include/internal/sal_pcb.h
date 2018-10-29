/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _SAL_PCB_H_
#define _SAL_PCB_H_


#ifdef __cplusplus
extern "C" {
#endif

#if SAL_NETIF_HWADDRHINT
#define IP_PCB_ADDRHINT ;u8_t addr_hint
#else
#define IP_PCB_ADDRHINT
#endif

/** Main packet buffer struct */
struct pbuf {
    /** next pbuf in singly linked pbuf chain */
    struct pbuf *next;

    /** pointer to the actual data in the buffer */
    void *payload;

    /**
     * total length of this buffer and all next buffers in chain
     * belonging to the same packet.
     *
     * For non-queue packet chains this is the invariant:
     * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
     */
    u16_t tot_len;

    /** length of this buffer */
    u16_t len;

    /** pbuf_type as u8_t instead of enum to save space */
    u8_t /*pbuf_type*/ type;

    /** misc flags */
    u8_t flags;

#if SAL_XR_EXT_MBUF_SUPPORT
    /** new flags for mbuf */
    u8_t mb_flags;

    /** decrease its size to u8_t */
    u8_t ref;
#else /* SAL_XR_EXT_MBUF_SUPPORT */
    /**
     * the reference count always equals the number of pointers
     * that refer to this pbuf. This can be pointers from an application,
     * the stack itself, or pbuf->next pointers from a chain.
     */
    u16_t ref;
#endif
};
/** This is the common part of all PCB types. It needs to be at the
   beginning of a PCB type definition. It is located here so that
   changes to this common part are made in one location instead of
   having to change all PCB structs. */
#define IP_PCB \
  /* ip addresses in network byte order */ \
  ip_addr_t local_ip; \
  ip_addr_t remote_ip; \
   /* Socket options */  \
  u8_t so_options;      \
   /* Type Of Service */ \
  u8_t tos;              \
  /* Time To Live */     \
  u8_t ttl               \
  /* link layer address resolution hint */ \
  IP_PCB_ADDRHINT

struct ip_pcb {
    /* Common members of all PCB types */
    IP_PCB;
};

struct tcp_pcb;
typedef void (*tcp_recv_fn)(void *arg, struct tcp_pcb *pcb, struct pbuf *p,
                            const ip_addr_t *addr, u16_t port);
/**
 * members common to struct tcp_pcb and struct tcp_listen_pcb
 */
#define TCP_PCB_COMMON(type) \
  type *next; /* for the linked list */ \
  void *callback_arg; \
  u8_t prio; \
  /* ports are in host byte order */ \
  u16_t local_port

/** the TCP protocol control block */
struct tcp_pcb {
    /** common PCB members */
    IP_PCB;
    /** protocol specific PCB members */
    TCP_PCB_COMMON(struct tcp_pcb);

    /* ports are in host byte order */
    u16_t remote_port;
    /** receive callback function */
    tcp_recv_fn recv;
    /* user-supplied argument for the recv callback */
    void *recv_arg;
};

#ifdef __cplusplus
}
#endif

#endif /* __AOS_EVENTFD_H__ */

