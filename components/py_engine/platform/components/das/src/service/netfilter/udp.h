#ifndef __DAS_UDP_H__
#define __DAS_UDP_H__

struct udphdr
{
    struct
    {
        u_int16_t uh_sport;		/* source port */
        u_int16_t uh_dport;		/* destination port */
        u_int16_t uh_ulen;		/* udp length */
        u_int16_t uh_sum;		/* udp checksum */
    };
};

#endif /* __DAS_UDP_H__ */