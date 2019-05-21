#include "include.h"
#include "arm_arch.h"

#if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_pub.h"
#include "error.h"
#include "co_list.h"

#include "video_transfer.h"

#include "drv_model_pub.h"
#include "mem_pub.h"

#include "spidma_intf_pub.h"
#include "camera_intf_pub.h"

#include "app_lwip_tcp.h"
#include "app_lwip_udp.h"

#if CFG_GENERAL_DMA
#include "general_dma_pub.h"
#endif

#define TVIDEO_DEBUG                1
#include "uart_pub.h"
#if TVIDEO_DEBUG
#define TVIDEO_PRT                  os_printf
#define TVIDEO_WPRT                 warning_prf
#define TVIDEO_FATAL                fatal_prf
#else
#define TVIDEO_PRT                  null_prf
#define TVIDEO_WPRT                 null_prf
#define TVIDEO_FATAL                null_prf
#endif

#define TVIDEO_DROP_DATA_NONODE     0
#define TVIDEO_USE_HDR              1

#define TVIDEO_RXNODE_SIZE_UDP      1472//32//1472
#define TVIDEO_RXNODE_SIZE_TCP      1460//20//1460
#ifndef TVIDEO_RXNODE_SIZE
#define TVIDEO_RXNODE_SIZE          TVIDEO_RXNODE_SIZE_UDP
#endif

#define TVIDEO_DROP_DATA_FLAG       0x01

#if TVIDEO_DROP_DATA_NONODE
#define TVIDEO_POOL_LEN             (TVIDEO_RXNODE_SIZE * 38)  // 54KB
#else
#define TVIDEO_POOL_LEN             (TVIDEO_RXNODE_SIZE * 10)  // 7KB
#endif

#define TVIDEO_RXBUF_LEN            (TVIDEO_RXNODE_SIZE_UDP * 4)

UINT8 tvideo_rxbuf[TVIDEO_RXBUF_LEN];

TVIDEO_DESC_ST tvideo_st;

typedef struct tvideo_hdr_st
{
    UINT8 id;
    UINT8 is_eof;
    UINT8 pkt_cnt; 
    UINT8 size;
}TV_HDR_ST, *TV_HDR_PTR;

typedef struct tvideo_elem_st
{
    struct co_list_hdr hdr;
    void *buf_start;
    UINT32 buf_len;
} TVIDEO_ELEM_ST, *TVIDEO_ELEM_PTR;

typedef struct tvideo_pool_st
{
    UINT8  pool[TVIDEO_POOL_LEN];
    TVIDEO_ELEM_ST elem[TVIDEO_POOL_LEN / TVIDEO_RXNODE_SIZE];
    struct co_list free;
    struct co_list ready; 
    #if TVIDEO_DROP_DATA_NONODE
    struct co_list receiving;
    UINT32 drop_pkt_flag;    
    #endif
    int (*send_func)(UINT8 *data, UINT32 len);
    #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
    TV_HDR_ST tv_hdr;
    #endif
} TVIDEO_POOL_ST, *TVIDEO_POOL_PTR;

TVIDEO_POOL_ST tvideo_pool;

enum
{
	TV_INT_POLL          = 0, 
	TV_EXIT,
};

typedef struct tvideo_message 
{
	UINT32 data;
}TV_MSG_T;

#define TV_QITEM_COUNT      (30)
xTaskHandle  tvideo_thread_hdl = NULL;
beken_queue_t tvideo_msg_que = NULL;

void tvideo_intfer_send_msg(UINT32 new_msg)
{
	OSStatus ret;
	TV_MSG_T msg;

    if(tvideo_msg_que) 
    {
    	msg.data = new_msg;
    	
    	ret = rtos_push_to_queue(&tvideo_msg_que, &msg, BEKEN_NO_WAIT);
    	if(kNoErr != ret)
    	{
    		TVIDEO_FATAL("tvideo_intfer_send_msg failed\r\n");
    	}
    }
}

static void tvideo_pool_init(void* data)
{
    UINT32 i = 0;
    TVIDEO_SND_TYPE snd_type = (TVIDEO_SND_TYPE)((int)data);
    os_memset(&tvideo_pool.pool[0], 0, sizeof(UINT8)*TVIDEO_POOL_LEN);
    co_list_init(&tvideo_pool.free);
    co_list_init(&tvideo_pool.ready);
    #if TVIDEO_DROP_DATA_NONODE
    co_list_init(&tvideo_pool.receiving);    
    tvideo_pool.drop_pkt_flag = 0;
    #endif
    
    for(i = 0; i < (TVIDEO_POOL_LEN / TVIDEO_RXNODE_SIZE); i++)
    {
        tvideo_pool.elem[i].buf_start =
            (void *)&tvideo_pool.pool[i * TVIDEO_RXNODE_SIZE];
        tvideo_pool.elem[i].buf_len = 0;

        co_list_push_back(&tvideo_pool.free,
                          (struct co_list_hdr *)&tvideo_pool.elem[i].hdr);
    }

    TVIDEO_PRT("video transfer send type:%d\r\n", snd_type);

    if(snd_type == TVIDEO_SND_UDP)
        tvideo_pool.send_func = app_lwip_udp_send_packet;
    else if(snd_type == TVIDEO_SND_TCP)
        tvideo_pool.send_func = app_lwip_tcp_send_packet; 
    else
        tvideo_pool.send_func = NULL; 

    #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
    tvideo_pool.tv_hdr.id = 0;
    tvideo_pool.tv_hdr.is_eof = 0;
    tvideo_pool.tv_hdr.pkt_cnt = 0;
    tvideo_pool.tv_hdr.size = 0;
    #endif
}

#if CFG_GENERAL_DMA
static void tvideo_config_general_dma(void)
{
    GDMACFG_TPYES_ST cfg;

    cfg.dstdat_width = 32;
    cfg.srcdat_width = 32;
    cfg.dstptr_incr = 1;
    cfg.srcptr_incr = 1;
    cfg.src_start_addr = NULL;
    cfg.dst_start_addr = NULL;

    cfg.channel = GDMA_CHANNEL_1;
    cfg.prio = 0;
    cfg.u.type1.src_loop_start_addr = &tvideo_rxbuf[0];
    cfg.u.type1.src_loop_end_addr = &tvideo_rxbuf[tvideo_st.rxbuf_len];

    sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE1, &cfg);
}
#endif

void *tvideo_memcpy(void *out, const void *in, UINT32 n)
{
    #if CFG_GENERAL_DMA
    GDMA_DO_ST do_st;
    do_st.channel = GDMA_CHANNEL_1;
    do_st.src_addr = (void*)in;
    do_st.length = n;
    do_st.dst_addr = out;
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_ENABLE, &do_st);
    #else
    os_memcpy(out, in, n);
    #endif
    return out;
} 

static void tvideo_rx_handler(void *curptr, UINT32 newlen, UINT32 is_eof, UINT32 frame_len)
{
    TVIDEO_ELEM_PTR elem = NULL;
    do {
        if(!newlen)
            break;
        
        #if TVIDEO_DROP_DATA_NONODE
        // drop pkt has happened, so drop it, until spidma timeout handler.
        if(tvideo_pool.drop_pkt_flag & TVIDEO_DROP_DATA_FLAG)
            break;
        #endif

        elem = (TVIDEO_ELEM_PTR)co_list_pick(&tvideo_pool.free);
        if(elem) {               
            if(newlen > tvideo_st.node_len)
                newlen = tvideo_st.node_len;

            #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
            TV_HDR_PTR elem_tvhdr = (TV_HDR_PTR)elem->buf_start;

            if(is_eof) {
                tvideo_pool.tv_hdr.is_eof = 1;
                tvideo_pool.tv_hdr.pkt_cnt = frame_len / tvideo_st.node_len;
                if(frame_len % tvideo_st.node_len) {
                    tvideo_pool.tv_hdr.pkt_cnt += 1;
                }
            }

            elem_tvhdr->id = tvideo_pool.tv_hdr.id;
            elem_tvhdr->is_eof = tvideo_pool.tv_hdr.is_eof;
            elem_tvhdr->pkt_cnt = tvideo_pool.tv_hdr.pkt_cnt;
            elem_tvhdr->size = tvideo_pool.tv_hdr.size;
            
            tvideo_memcpy(elem_tvhdr + 1, curptr, newlen);
            if(tvideo_st.node_len > newlen){
                //UINT32 left = tvideo_st.node_len - newlen;
                //os_memset((elem_tvhdr + 1 + newlen), 0, left);
            }
            //elem->buf_len = tvideo_st.node_len + sizeof(TV_HDR_ST);
            elem->buf_len = newlen + sizeof(TV_HDR_ST);

            #else // (TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
            tvideo_memcpy(elem->buf_start, curptr, newlen);
            if(tvideo_st.node_len > newlen){
                //UINT32 left = tvideo_st.node_len - newlen;
                //os_memset(((UINT8*)elem->buf_start + newlen), 0, left);
            }
            //elem->buf_len = tvideo_st.node_len;
            elem->buf_len = newlen;
            #endif // (TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
            
            co_list_pop_front(&tvideo_pool.free);
            #if TVIDEO_DROP_DATA_NONODE
            co_list_push_back(&tvideo_pool.receiving, (struct co_list_hdr *)&elem->hdr);
            #else
            co_list_push_back(&tvideo_pool.ready, (struct co_list_hdr *)&elem->hdr);            
            #endif
        } 
        else {
            #if TVIDEO_DROP_DATA_NONODE
            // not node for receive pkt, drop aready received, and also drop
            // the new come.
            UINT32 cnt_rdy = co_list_cnt(&tvideo_pool.receiving);     

            tvideo_pool.drop_pkt_flag |= TVIDEO_DROP_DATA_FLAG;
            if(cnt_rdy)
                co_list_concat(&tvideo_pool.free, &tvideo_pool.receiving);
            #endif
            //os_printf("lost\r\n");
        }
    } while(0);

    tvideo_intfer_send_msg(TV_INT_POLL);
}

static void tvideo_end_frame_handler(void)
{
    #if TVIDEO_DROP_DATA_NONODE
    // reset drop flag, new pkt can receive
    tvideo_pool.drop_pkt_flag &= (~TVIDEO_DROP_DATA_FLAG);
    if(!co_list_is_empty(&tvideo_pool.receiving)) {        
        co_list_concat(&tvideo_pool.ready, &tvideo_pool.receiving);  
    }
    #endif

    #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
    tvideo_pool.tv_hdr.id++;
    tvideo_pool.tv_hdr.is_eof = 0;
    #endif
    
    tvideo_intfer_send_msg(TV_INT_POLL);
}

static void tvideo_config_desc(void* data)
{
    TVIDEO_SND_TYPE snd_type = (TVIDEO_SND_TYPE)((int)data);
    UINT32 node_len = TVIDEO_RXNODE_SIZE_TCP;

    if(snd_type == TVIDEO_SND_UDP) {
        #if(TVIDEO_USE_HDR && CFG_USE_CAMERA_INTF)
        node_len = TVIDEO_RXNODE_SIZE_UDP - sizeof(TV_HDR_ST);    
        #else
        node_len = TVIDEO_RXNODE_SIZE_UDP;
        #endif
    }
    else if(snd_type == TVIDEO_SND_TCP)
        node_len = TVIDEO_RXNODE_SIZE_TCP; 
    else {
        TVIDEO_WPRT("Err snd tpye in spidma\r\n"); 
    }
    
    tvideo_st.rxbuf = &tvideo_rxbuf[0];
    tvideo_st.rxbuf_len = node_len * 4;
    tvideo_st.node_len = node_len;
    tvideo_st.rx_read_len = 0;

    tvideo_st.node_full_handler = tvideo_rx_handler;
    tvideo_st.data_end_handler = tvideo_end_frame_handler;
}

static void tvideo_poll_handler(void)
{
    UINT32 send_len;
    TVIDEO_ELEM_PTR elem = NULL;

    do{
        elem = (TVIDEO_ELEM_PTR)co_list_pick(&tvideo_pool.ready);        
        if(elem) {
            if(tvideo_pool.send_func) {
                //REG_WRITE((0x00802800+(18*4)), 0x02);
                send_len = tvideo_pool.send_func(elem->buf_start, elem->buf_len);
                //REG_WRITE((0x00802800+(18*4)), 0x00);
                if(send_len != elem->buf_len) {
                    break;
                }
            }
            
            co_list_pop_front(&tvideo_pool.ready);
            co_list_push_back(&tvideo_pool.free, (struct co_list_hdr *)&elem->hdr);
        }
    }
    while(elem);
}

/*---------------------------------------------------------------------------*/
static void video_transfer_main( beken_thread_arg_t data )
{
    OSStatus err;
    TVIDEO_PRT("video_transfer_main entry\r\n");
    
    tvideo_pool_init(data);
    tvideo_config_desc(data);
#if CFG_GENERAL_DMA
    tvideo_config_general_dma();
#endif

    {
        #if CFG_USE_SPIDMA
        spidma_intfer_init(&tvideo_st);
        #elif CFG_USE_CAMERA_INTF
        camera_intfer_init(&tvideo_st);
        #endif
    }

    while(1)
    {
        TV_MSG_T msg;
        err = rtos_pop_from_queue(&tvideo_msg_que, &msg, BEKEN_WAIT_FOREVER);
        if(kNoErr == err)
        {
        	switch(msg.data) 
            {
                case TV_INT_POLL:
                    tvideo_poll_handler();
                    break;
                    
                case TV_EXIT:
                    goto tvideo_exit;
                    break;
                    
                default:
                    break;
            }
        }
    }
    
tvideo_exit:
    TVIDEO_PRT("video_transfer_main exit\r\n");

    #if CFG_USE_SPIDMA
    spidma_intfer_deinit();
    #elif CFG_USE_CAMERA_INTF
    camera_intfer_deinit();
    #endif

    rtos_deinit_queue(&tvideo_msg_que);
    tvideo_msg_que = NULL;

    tvideo_thread_hdl = NULL;
    rtos_delete_thread(NULL);
}

UINT32 video_transfer_init(UINT32 tcp_udp_mode)
{
    int ret;

    TVIDEO_PRT("video_transfer_init %d\r\n", tcp_udp_mode);
    if((!tvideo_thread_hdl) && (!tvideo_msg_que))
    {

    	ret = rtos_init_queue(&tvideo_msg_que, 
    							"tvideo_queue",
    							sizeof(TV_MSG_T),
    							TV_QITEM_COUNT);
    	if (kNoErr != ret) 
    	{
    		TVIDEO_FATAL("spidma_intfer ceate queue failed\r\n");
            return kGeneralErr;
    	}
        
        ret = rtos_create_thread(&tvideo_thread_hdl,
                                      4,
                                      "video_intf",
                                      (beken_thread_function_t)video_transfer_main,
                                      1024,
                                      (beken_thread_arg_t)tcp_udp_mode);
        if (ret != kNoErr)
        {
            rtos_deinit_queue(&tvideo_msg_que);
            tvideo_msg_que = NULL;
            tvideo_thread_hdl = NULL;
            TVIDEO_FATAL("Error: Failed to create spidma_intfer: %d\r\n", ret);
            return kGeneralErr;
        }
    }

    return kNoErr;
}

UINT32 video_transfer_deinit(void)
{
    TVIDEO_PRT("video_transfer_deinit\r\n");
    
    tvideo_intfer_send_msg(TV_EXIT);
}
#endif  // (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)

