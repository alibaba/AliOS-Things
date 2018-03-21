#ifndef _AAC_H
#define _AAC_H
  
#include "dlist.h"      //list management
#include "basic_types.h"
#include "osdep_service.h"
//#include "osdep_api.h"

#define AAC_DEBUG 1

#if AAC_DEBUG
#define AAC_PRINTF(fmt, args...)    printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#define AAC_ERROR(fmt, args...)     printf("\n\r%s: " fmt, __FUNCTION__, ## args)
#else
#define AAC_PRINTF(fmt, args...)    
#define AAC_ERROR(fmt, args...)   
#endif

struct rtp_au_hdr
{
        u32 au_size; //contain AU size in octets(header length included)
#if 0
        u16 au_index;
        u16 au_index_delta;
        u16 cts_flag;
        u16 cts_delta;
        u16 dts_flag;
        u16 dts_delta;
        u16 rap_flag;
        u16 stream_state;
#endif
        u32 au_header_len; //record AU header size in octets        
};

struct rtp_aac_obj 
{
        u16 au_headers_len; //contain AU header size in octet (we will translate it into bit-wise count before sending)
        u8 au_header_num; //contain AU header number
//        struct rtp_au_hdr *au_hdr;
};

#endif /*_AAC_H*/