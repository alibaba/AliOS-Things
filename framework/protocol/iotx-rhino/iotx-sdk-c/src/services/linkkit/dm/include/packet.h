#ifndef __LINKKIT_PACKET_H__
#define __LINKKIT_PACKET_H__

typedef struct {
    char *buf;
    char *end;
    char *ptr;

    int got_data;
    int is_request;
} linkkit_pkt_t;

int packet_init(linkkit_pkt_t *pkt, char *buf, int buf_len);
int packet_printf(linkkit_pkt_t *pkt, char *fmt, ...);
int packet_strncat(linkkit_pkt_t *pkt, char *str, int n);

#endif /* __LINKKIT_PACKET_H__ */
