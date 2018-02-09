
#ifndef _MJPEG_H
#define _MJPEG_H
  
#include "dlist.h"      //list management
#include "basic_types.h"
#include "osdep_service.h"
//#include "osdep_api.h"

#define MJPEG_DEBUG 0

#if MJPEG_DEBUG
#define MJPEG_PRINTF(fmt, args...)    printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define MJPEG_ERROR(fmt, args...)     printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#else
#define MJPEG_PRINTF(fmt, args...)    
#define MJPEG_ERROR(fmt, args...)     
#endif

struct jpeghdr {
        unsigned int tspec:8;   /* type-specific field */
        unsigned int off:24;    /* fragment byte offset */
        u8 type;            /* id of jpeg decoder params */
        u8 q;               /* quantization factor (or table id) */
        u8 width;           /* frame width in 8 pixel blocks */
        u8 height;          /* frame height in 8 pixel blocks */
};

struct jpeghdr_rst {
        u16 dri;                /*restart interval*/
        unsigned int f:1;       /*restart first bit flag*/
        unsigned int l:1;       /*restart last bit flag*/
        unsigned int count:14;  /*restart count*/
};


struct jpeghdr_qtable {
        u8  mbz;
        u8  precision;
        u16 length;
};

#define RTP_JPEG_RESTART        0x40
#define USE_EXPLICIT_DQT        128
#define USE_IMPLICIT_DQT        0

struct rtp_jpeg_obj
{
        struct jpeghdr jpghdr;
        struct jpeghdr_rst rsthdr;
        struct jpeghdr_qtable qtable;
        u8     lqt[64*2];          /* Luma Quantizer table               */
        u8     cqt[64*2];          /* Croma Quantizer table              */
        int hdr_len;
        int frame_offset;
};

/*for debug purpose*/
void dumpJpegHeader(struct jpeghdr *jpghdr);
void dumpRstDeader(struct jpeghdr_rst *rsthdr);

#endif /*_MJPEG_H*/