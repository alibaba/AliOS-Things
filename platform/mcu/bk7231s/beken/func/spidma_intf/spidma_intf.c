#include "include.h"
#include "arm_arch.h"

#if CFG_USE_SPIDMA
#include "video_transfer.h"

#include "spidma_pub.h"
#include "spidma_intf.h"
#include "spidma_intf_pub.h"

#include "drv_model_pub.h"
#include "mem_pub.h"
#if CFG_GENERAL_DMA
#include "general_dma_pub.h"
#endif

#define SPIDMA_INTF_DEBUG           1
#include "uart_pub.h"
#if SPIDMA_INTF_DEBUG
#define SPIDMA_INTF_PRT             os_printf
#define SPIDMA_INTF_WPRT            warning_prf
#define SPIDMA_INTF_FATAL           fatal_prf
#else
#define SPIDMA_INTF_PRT             null_prf
#define SPIDMA_INTF_WPRT            null_prf
#define SPIDMA_INTF_FATAL           null_prf
#endif

#define RX_TIMEOUT_30US             30
#define RX_TIMEOUT_500US            500
#define FPGA_MAIN_CLK               120
#define SPIDMA_RXDATA_TIMEOUT       (FPGA_MAIN_CLK * RX_TIMEOUT_500US)

#ifndef SPIDMA_RXDATA_TIMEOUT
#define SPIDMA_RXDATA_TIMEOUT       SPIDMA_DEF_RXDATA_TIMEOUT_VAL
#endif

#define SPIDMA_CHNAL                GDMA_CHANNEL_5 

DD_HANDLE spidma_hdl = DD_HANDLE_UNVALID;
SPIDMA_DESC_ST spidma_intf;

#if CFG_GENERAL_DMA
static void spidma_intf_node_rx_handler(UINT32 dma)
{   
    UINT16 already_len = spidma_intf.rx_read_len;
    UINT16 copy_len = spidma_intf.node_len;
    GLOBAL_INT_DECLARATION();

    if(spidma_intf.node_full_handler != NULL) {
        spidma_intf.node_full_handler(spidma_intf.rxbuf + already_len, copy_len, 0, 0);
    }

    already_len += copy_len;
    
    if(already_len >= spidma_intf.rxbuf_len)
        already_len = 0;

    GLOBAL_INT_DISABLE();
    spidma_intf.rx_read_len = already_len;
    GLOBAL_INT_RESTORE();
}
#endif

static void spidma_intf_frame_end_handler(void)
{   
    #if CFG_GENERAL_DMA
    GDMA_CFG_ST en_cfg;
    UINT16 already_len = spidma_intf.rx_read_len;
    UINT32 channel = SPIDMA_CHNAL;
    int left_len = sddev_control(GDMA_DEV_NAME, CMD_GDMA_GET_LEFT_LEN, (void*)channel);
    int rec_len = spidma_intf.node_len - left_len;
    
    if((spidma_intf.node_full_handler != NULL) && (rec_len > 0)) {
        spidma_intf.node_full_handler(spidma_intf.rxbuf + already_len, rec_len, 0, 0);
    }

    already_len += rec_len;
    if(already_len >= spidma_intf.rxbuf_len) {
        already_len -= spidma_intf.rxbuf_len;
    }

    spidma_intf.rx_read_len = already_len;

    // turn off dma, so dma can start from first configure. for easy handler
    en_cfg.channel = SPIDMA_CHNAL;
    en_cfg.param = 0;   
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);

    spidma_intf.rx_read_len = 0;
    en_cfg.param = 1;   
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg); 
    #endif

    if((spidma_intf.data_end_handler)) {
        spidma_intf.data_end_handler();
    }
}

static void spidma_intfer_config_desc(void* data)
{
    os_memset(&spidma_intf, 0, sizeof(SPIDMA_DESC_ST));
    os_memcpy(&spidma_intf, data, sizeof(TVIDEO_DESC_ST));

    spidma_intf.txbuf = NULL;
    spidma_intf.txbuf_len = 0;

    spidma_intf.tx_handler = NULL;

    spidma_intf.mode = 0;
    spidma_intf.timeout_val = SPIDMA_RXDATA_TIMEOUT;
    spidma_intf.end_frame_handler = spidma_intf_frame_end_handler;

 #if CFG_GENERAL_DMA
    spidma_intf.dma_rx_handler = spidma_intf_node_rx_handler;
    spidma_intf.dma_channel = SPIDMA_CHNAL;
#endif   
}

/*---------------------------------------------------------------------------*/
void spidma_intfer_init(void* data)
{
    UINT32 status;
    spidma_intfer_config_desc(data);
        
    spidma_hdl = ddev_open(SPIDMA_DEV_NAME, &status, (UINT32)&spidma_intf);
    SPIDMA_INTF_FATAL("spidma_intfer_init,%p\r\n", spidma_hdl);
}

void spidma_intfer_deinit(void)
{
    SPIDMA_INTF_FATAL("spidma_intfer_deinit,%p\r\n", spidma_hdl);
    ddev_close(spidma_hdl);
    os_memset(&spidma_intf, 0, sizeof(SPIDMA_DESC_ST));

    spidma_hdl = DD_HANDLE_UNVALID;
}
/*---------------------------------------------------------------------------*/

#endif  // CFG_USE_SPIDMA

