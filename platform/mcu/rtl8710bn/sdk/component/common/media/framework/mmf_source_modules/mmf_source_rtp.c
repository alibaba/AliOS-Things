#include "mmf_source.h"

#include "wifi_conf.h"
#include "lwip/api.h" //netconn use
#include <lwip/sockets.h>
#include "rtsp/rtp_api.h"

#define MAX_SELECT_SOCKET       8
#define DEFAULT_RTP_PORT        16384

#if CONFIG_EXAMPLE_MP3_STREAM_RTP    
#define G711_BLK_SIZE   (634)
#define AUDIO_BUF_SIZE  (646) //160 BYTE DATA + 12 BYTE RTP HEADER
#else
#define G711_BLK_SIZE   (160)
#define AUDIO_BUF_SIZE  (172) //160 BYTE DATA + 12 BYTE RTP HEADER
#endif

#define AUDIO_BUF_DEPTH   (4)

typedef struct rtp_exbuf_type
{
    u8 buf[AUDIO_BUF_SIZE];
    int len;
}rtp_exbuf_t;

typedef struct rtp_client_type
{
    TaskHandle_t task_handle;
    xQueueHandle cache_queue;
    xQueueHandle done_queue;
    struct connect_context connect_ctx;
    _sema rtp_sema;
    u8 rtp_shutdown;
}rtp_client_t;

//static u8* priv_buf[AUDIO_BUF_DEPTH];

static rtp_exbuf_t rtp_buf;
static int q_depth = 0;

static long listen_time_s = 0;
static long listen_time_us = 20000; //20ms

int rtp_client_start(void *param)
{
    rtp_client_t *rtp_ctx = (rtp_client_t *)param;
    u32 start_time, current_time;
    int ret = 0;
    int len, offset;
    rtp_hdr_t rtp_header;
    struct sockaddr_in rtp_addr;                                      
    socklen_t addrlen = sizeof(struct sockaddr_in); 
    fd_set read_fds;
    struct timeval listen_timeout;
    int mode = 0;
    int opt = 1;
    u16 last_seq = 0;
    start_time = rtw_get_current_time();
    wext_get_mode(WLAN0_NAME, &mode);
    printf("\n\rwlan mode:%d", mode);
   
    switch(mode)
    {
        case(IW_MODE_MASTER)://AP mode
            while(wifi_is_ready_to_transceive(RTW_AP_INTERFACE) < 0)
            {
                    vTaskDelay(1000);
                    current_time = rtw_get_current_time();
                    if((current_time - start_time) > 60000)
                    {
                        printf("\n\rwifi Tx/Rx not ready...");
                        return ret;
                    }
            }
            break;
        case(IW_MODE_INFRA)://STA mode
   
        case(IW_MODE_AUTO)://when ameba doesn't join bss
            while(wifi_is_ready_to_transceive(RTW_STA_INTERFACE) < 0)
            {
                    vTaskDelay(1000);
                    current_time = rtw_get_current_time();
                    if((current_time - start_time) > 60000)
                    {
                        printf("\n\rwifi Tx/Rx not ready...");
                        return ret;
                    }
            }
            break; 
        default:
                printf("\n\rillegal wlan state!rtp client cannot start");
                return ret;
    }
    
    wext_get_mode(WLAN0_NAME, &mode);
    printf("\n\rwlan mode:%d", mode);
    vTaskDelay(1000);   

    if((rtp_ctx->connect_ctx.socket_id = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\n\rcreate rtp client socket failed!");
        return 0;//???
    }
    
    if((setsockopt(rtp_ctx->connect_ctx.socket_id, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt))) < 0){
            printf("\r\n Error on setting socket option");
            goto exit;
    }    
    memset(&rtp_addr, 0, addrlen);
    rtp_addr.sin_family = AF_INET;
    rtp_addr.sin_addr.s_addr = INADDR_ANY;
    rtp_addr.sin_port = htons(rtp_ctx->connect_ctx.server_port);                
    if (bind(rtp_ctx->connect_ctx.socket_id,(struct sockaddr *)&rtp_addr, addrlen)<0) {
        printf("bind failed\r\n");
        goto exit;
    }   
    rtp_ctx->rtp_shutdown = 0;
    while(!rtp_ctx->rtp_shutdown)
    {
        FD_ZERO(&read_fds);
        listen_timeout.tv_sec = listen_time_s;
        listen_timeout.tv_usec = listen_time_us;
        FD_SET(rtp_ctx->connect_ctx.socket_id, &read_fds);
        if(select(MAX_SELECT_SOCKET, &read_fds, NULL, NULL, &listen_timeout))
        {
            //q_depth = uxQueueSpacesAvailable(rtp_ctx->done_queue);
            //if(q_depth > 0)
            if(xQueueReceive(rtp_ctx->done_queue, (void *)&rtp_buf, 0xFFFFFFFF) != pdTRUE){
                    continue;
                    //insert silence here 
            }          
            //printf("\n\ro");         
            memset(rtp_buf.buf, 0, AUDIO_BUF_SIZE);
            len = recvfrom(rtp_ctx->connect_ctx.socket_id, rtp_buf.buf, AUDIO_BUF_SIZE, 0, (struct sockaddr *)&rtp_addr, &addrlen); 
            rtp_buf.len = len;
            xQueueSend(rtp_ctx->cache_queue, (void *)&rtp_buf, 0xFFFFFFFF);
        }else{          
            //printf("\n\r.");
            //insert silence here
        }
    }
exit:
    close(rtp_ctx->connect_ctx.socket_id);

    return ret;
}

void rtp_client_init(void *param)
{
    rtp_client_t *rtp_ctx = (rtp_client_t *)param;
    rtw_init_sema(&rtp_ctx->rtp_sema, 0);
    while(1)
    {
        if(rtw_down_timeout_sema(&rtp_ctx->rtp_sema,100))
        {
            if(rtp_client_start(rtp_ctx) < 0)
                goto exit;
        }        
    }
exit:
    rtp_ctx->rtp_shutdown = 0;
    rtw_free_sema(&rtp_ctx->rtp_sema);
    vTaskDelete(NULL);  
}

void* rtp_mod_open(void)
{
    rtp_client_t *rtp_ctx = malloc(sizeof(rtp_client_t));
    if(rtp_ctx == NULL)
        return NULL;
    memset(rtp_ctx, 0, sizeof(rtp_client_t));
    /* set default port */
    rtp_ctx->connect_ctx.socket_id = -1;
    rtp_ctx->connect_ctx.server_port = DEFAULT_RTP_PORT;
/* create a rtp client to receive audio data */
    if(xTaskCreate(rtp_client_init, ((const char*)"rtp_client_init"), 512, rtp_ctx, 2, &rtp_ctx->task_handle) != pdPASS) {
        //printf("\r\n geo_rtp_client_init: Create Task Error\n");
        free(rtp_ctx);
        return NULL;
    }    
    return (void*)rtp_ctx;
}

void rtp_mod_close(void* ctx)
{
    rtp_client_t *rtp_ctx = (rtp_client_t *)ctx;
    if(!rtp_ctx->task_handle && xTaskGetCurrentTaskHandle()!=rtp_ctx->task_handle)
        vTaskDelete(rtp_ctx->task_handle);
    free(rtp_ctx);
}


int rtp_mod_set_param(void* ctx, int cmd, int arg)
{
    int ret = 0;
    rtp_client_t *rtp_ctx = (rtp_client_t *)ctx;
    
    switch(cmd)
    {
      case(CMD_SET_PRIV_BUF):
          if(arg==1)//initially set queue
          {
              rtp_ctx->cache_queue = xQueueCreate(AUDIO_BUF_DEPTH, sizeof(rtp_exbuf_t));
              rtp_ctx->done_queue = xQueueCreate(AUDIO_BUF_DEPTH, sizeof(rtp_exbuf_t));          
          }else if(arg==2)//reset queue
          {
              xQueueReset(rtp_ctx->cache_queue);
              xQueueReset(rtp_ctx->done_queue);              
          }else if(arg==0)//delete queue
          {
              vQueueDelete(rtp_ctx->cache_queue);
              vQueueDelete(rtp_ctx->done_queue);
              break;
          }else{
              ret = -EINVAL;
              break;
          }
          for(int i = 0; i < AUDIO_BUF_DEPTH; i++)
          {
              xQueueSend(rtp_ctx->done_queue, (void*)&rtp_buf, 0xFFFFFFFF);
          }
          break;
      case(CMD_SET_STREAMMING):
          if(arg == ON){	// stream on
                  rtw_up_sema(&rtp_ctx->rtp_sema);
          }else{			// stream off
                  rtp_ctx->rtp_shutdown = 0;
          }	
          break;          
      default:
          ret = -EINVAL;
          break;
    }
    return ret;
}

//give audio data here

rtp_exbuf_t tmp_exbuf[3];
static int tmp_order = 0;
static u16 last_seq = 0;
int rtp_mod_handle(void* ctx, void* b)
{
    int ret = 0;
    int q_depth = 0;
    rtp_client_t *rtp_ctx = (rtp_client_t *)ctx;
    exch_buf_t *exbuf = (exch_buf_t*)b;   
    rtp_hdr_t rtp_header;
    int offset;
    if(exbuf->state==STAT_USED){
            xQueueSend(rtp_ctx->done_queue, (void *)&tmp_exbuf[exbuf->index], 0xFFFFFFFF);
            exbuf->state = STAT_INIT;
    }    
    //decide if audio cache is enough for send
    q_depth = uxQueueSpacesAvailable(rtp_ctx->cache_queue);
    //printf("\n\rQ-%d", q_depth);
    if(q_depth >= AUDIO_BUF_DEPTH/2)
        return -EAGAIN;
    if(xQueueReceive(rtp_ctx->cache_queue, (void *)&tmp_exbuf[tmp_order], 0xFFFFFFFF) != pdTRUE)
        return -EAGAIN;
    offset = rtp_parse_header(tmp_exbuf[tmp_order].buf, &rtp_header, 1);
    //printf("\n\r%d-%dB", offset, tmp_exbuf[tmp_order].len);
/* for data loss debugging */
#if 1
    if(last_seq == 0)
        last_seq = ntohs(rtp_header.seq);
    else
    {
        if((ntohs(rtp_header.seq) - last_seq)>1)
            printf("\n\r(%d-%d)", last_seq, ntohs(rtp_header.seq));
        last_seq = ntohs(rtp_header.seq); 
    }
#endif    
    
#if CONFIG_EXAMPLE_MP3_STREAM_RTP    
    if((rtp_header.pt == RTP_PT_PCMU) || (rtp_header.pt == RTP_PT_DYN_BASE) || (rtp_header.pt == RTP_PT_MPA))
#else
    if(rtp_header.pt == RTP_PT_PCMU)
#endif
    {
        if(exbuf->state!=STAT_READY){
            exbuf->index = tmp_order;
            exbuf->data = tmp_exbuf[tmp_order].buf + offset;
            exbuf->len = tmp_exbuf[tmp_order].len - offset;
            //exbuf->timestamp = ?;
            exbuf->state = STAT_READY;
            tmp_order++;
            if(tmp_order == 3)
                tmp_order = 0;
        }
    }
    return ret;
}

msrc_module_t rtp_src_module =
{
	.create = rtp_mod_open,
	.destroy = rtp_mod_close,
	.set_param = rtp_mod_set_param,
	.handle = rtp_mod_handle,
};