#include "include.h"
#include "arm_arch.h"

#if CFG_USE_CAMERA_INTF
//#include "process.h"
#include "llc.h"

#include "video_transfer.h"

#include "jpeg_encoder_pub.h"
#include "i2c_pub.h"

#include "camera_intf.h"
#include "camera_intf_pub.h"

#include "drv_model_pub.h"
#include "general_dma_pub.h"
#include "mem_pub.h"
#include "bk_timer_pub.h"

#define CAMERA_INTF_DEBUG
#include "uart_pub.h"
#ifdef CAMERA_INTF_DEBUG
#define CAMERA_INTF_PRT             os_printf
#define CAMERA_INTF_WPRT            warning_prf
#define CAMERA_INTF_FATAL           fatal_prf
#else
#define CAMERA_INTF_PRT             null_prf
#define CAMERA_INTF_WPRT            null_prf
#define CAMERA_INTF_FATAL           null_prf
#endif

#define EJPEG_DMA_CHNAL             GDMA_CHANNEL_5
#define EJPEG_DELAY_HTIMER_CHNAL    3
#define EJPEG_DELAY_HTIMER_VAL      (2)  // 2ms
#define USE_JTAG_FOR_DEBUG          0

DJPEG_DESC_ST ejpeg_cfg;
DD_HANDLE i2c_hdl = DD_HANDLE_UNVALID, ejpeg_hdl = DD_HANDLE_UNVALID;
I2C_OP_ST i2c_operater;

void camera_intf_delay_timer_hdl(UINT8 param)
{
    #if CFG_GENERAL_DMA
    GDMA_CFG_ST en_cfg;
    UINT16 already_len = ejpeg_cfg.rx_read_len;
    UINT32 channel = ejpeg_cfg.dma_channel;
    GLOBAL_INT_DECLARATION();

    REG_WRITE((0x00802800+(16*4)), 0x02);

    int left_len = sddev_control(GDMA_DEV_NAME, CMD_GDMA_GET_LEFT_LEN, (void*)channel);
    int rec_len = ejpeg_cfg.node_len - left_len;
    UINT32 frame_len = 0;
    frame_len = ddev_control(ejpeg_hdl, EJPEG_CMD_GET_FRAME_LEN, NULL);

    if((ejpeg_cfg.node_full_handler != NULL) && (rec_len > 0)) {
        ejpeg_cfg.node_full_handler(ejpeg_cfg.rxbuf + already_len, rec_len, 1, frame_len);
    }

    already_len += rec_len;
    if(already_len >= ejpeg_cfg.rxbuf_len) {
        already_len -= ejpeg_cfg.rxbuf_len;
    }

    GLOBAL_INT_DISABLE();
    ejpeg_cfg.rx_read_len = already_len;
    GLOBAL_INT_RESTORE();

    // turn off dma, so dma can start from first configure. for easy handler
    en_cfg.channel = ejpeg_cfg.dma_channel;
    en_cfg.param = 0;   
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);

    ejpeg_cfg.rx_read_len = 0;
    en_cfg.param = 1;   
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);
    #endif
    
    if((ejpeg_cfg.data_end_handler)) {
        ejpeg_cfg.data_end_handler();
    }

    channel = EJPEG_DELAY_HTIMER_CHNAL;
    sddev_control(TIMER_DEV_NAME, CMD_TIMER_UNIT_DISABLE, &channel);

    REG_WRITE((0x00802800+(16*4)), 0x00);
}

static void camera_intf_start_delay_timer(void)
{
	timer_param_t param;

    REG_WRITE((0x00802800+(16*4)), 0x02);
    
	param.channel = EJPEG_DELAY_HTIMER_CHNAL;
	param.div = 1;
	param.period = EJPEG_DELAY_HTIMER_VAL;
	param.t_Int_Handler= camera_intf_delay_timer_hdl;
        
	sddev_control(TIMER_DEV_NAME, CMD_TIMER_INIT_PARAM, &param);
    
    REG_WRITE((0x00802800+(16*4)), 0x00);    
}

static void camera_intf_ejpeg_rx_handler(UINT32 dma)
{   
    UINT16 already_len = ejpeg_cfg.rx_read_len;
    UINT16 copy_len = ejpeg_cfg.node_len;
    GLOBAL_INT_DECLARATION();

    REG_WRITE((0x00802800+(17*4)), 0x02);

    if(ejpeg_cfg.node_full_handler != NULL) {
        ejpeg_cfg.node_full_handler(ejpeg_cfg.rxbuf + already_len, copy_len, 0, 0);
    }

    already_len += copy_len;
    
    if(already_len >= ejpeg_cfg.rxbuf_len)
        already_len = 0;

    GLOBAL_INT_DISABLE();
    ejpeg_cfg.rx_read_len = already_len;
    GLOBAL_INT_RESTORE();

    REG_WRITE((0x00802800+(17*4)), 0x00);
}

static void camera_intf_ejpeg_end_handler(void)
{      
    camera_intf_start_delay_timer();     
}

static void camera_intf_config_ejpeg(void* data)
{   
    os_memset(&ejpeg_cfg, 0, sizeof(DJPEG_DESC_ST));
    os_memcpy(&ejpeg_cfg, data, sizeof(TVIDEO_DESC_ST));

    ejpeg_cfg.x_pixel = X_PIXEL_640;
    ejpeg_cfg.y_pixel = Y_PIXEL_480;

    ejpeg_cfg.start_frame_handler = NULL;
    ejpeg_cfg.end_frame_handler = camera_intf_ejpeg_end_handler;

#if CFG_GENERAL_DMA
    ejpeg_cfg.dma_rx_handler = camera_intf_ejpeg_rx_handler;
    ejpeg_cfg.dma_channel = GDMA_CHANNEL_4;
#endif
}

static void camera_intf_sccb_write(UINT8 addr, UINT8 data)
{
	char buff = (char)data;
    
    i2c_operater.op_addr = addr;
    ddev_write(i2c_hdl, &buff, 1, (UINT32)&i2c_operater);
}

void camera_intf_sccb_read(UINT8 addr, UINT8 *data)
{   
    i2c_operater.op_addr = addr;
    ddev_read(i2c_hdl, (char*)data, 1, (UINT32)&i2c_operater);
}

static void camera_intf_config_senser(void)
{
    UINT32 i, size;
    UINT8 addr, data;
    
#if (USE_CAMERA == PAS6329_DEV)

    i2c_operater.salve_id = PAS6329_DEV_ID;

    size = sizeof(pas6329_page0)/2;
    PAS6329_SET_PAGE0;
    
    for(i=0; i<size; i++) {
        addr = pas6329_page0[i][0];
        data = pas6329_page0[i][1];
        camera_intf_sccb_write(addr, data);
    }

    size = sizeof(pas6329_page1)/2;
    PAS6329_SET_PAGE1;
    for(i=0; i<size; i++) {
        addr = pas6329_page1[i][0];
        data = pas6329_page1[i][1];
        camera_intf_sccb_write(addr, data);
    }

    size = sizeof(pas6329_page2)/2;
    PAS6329_SET_PAGE2;
    for(i=0; i<size; i++) {
        addr = pas6329_page2[i][0];
        data = pas6329_page2[i][1];
        camera_intf_sccb_write(addr, data);
    }

    PAS6329_SET_PAGE0;
    CAMERA_INTF_WPRT("PAS6329 init finish\r\n");
    
#elif (USE_CAMERA == OV_7670_DEV)

    i2c_operater.salve_id = OV_7670_DEV_ID;

    size = sizeof(ov_7670_init_talbe)/2;
    
    for(i=0; i<size; i++) {
        addr = ov_7670_init_talbe[i][0];
        data = ov_7670_init_talbe[i][1];
        camera_intf_sccb_write(addr, data);
    }
    CAMERA_INTF_WPRT("OV_7670 init finish\r\n");
    
#elif (USE_CAMERA == PAS6375_DEV)

    i2c_operater.salve_id = PAS6375_DEV_ID;

    size = sizeof(pas6375_init_talbe)/2;
    
    for(i=0; i<size; i++) {
        addr = pas6375_init_talbe[i][0];
        data = pas6375_init_talbe[i][1];
        camera_intf_sccb_write(addr, data);
    }
    CAMERA_INTF_WPRT("PAS6375 init finish\r\n");

#elif (USE_CAMERA == GC0328C_DEV) 
    i2c_operater.salve_id = GC0328C_DEV_ID;

    size = sizeof(gc0328c_init_talbe)/2;
    
    for(i=0; i<size; i++) {
        addr = gc0328c_init_talbe[i][0];
        data = gc0328c_init_talbe[i][1];
        camera_intf_sccb_write(addr, data);
    }
    CAMERA_INTF_WPRT("GC0328C init finish\r\n");
#endif
    
}

/*---------------------------------------------------------------------------*/
void camera_intfer_init(void* data)
{
    UINT32 status, oflag;

    camera_intf_config_ejpeg(data);

    ejpeg_hdl = ddev_open(EJPEG_DEV_NAME, &status, (UINT32)&ejpeg_cfg);
    
    oflag = I2C_DEF_DIV;
    #if USE_JTAG_FOR_DEBUG
    i2c_hdl = ddev_open(I2C2_DEV_NAME, &status, oflag);
    #else
    i2c_hdl = ddev_open(I2C1_DEV_NAME, &status, oflag);
    #endif
        
    camera_intf_config_senser();

    CAMERA_INTF_FATAL("camera_intfer_init,%p-%p\r\n", ejpeg_hdl, i2c_hdl);
}

void camera_intfer_deinit(void)
{
    CAMERA_INTF_FATAL("camera_intfer_deinit,%p-%p\r\n", ejpeg_hdl, i2c_hdl);
    
    ddev_close(ejpeg_hdl);
    ddev_close(i2c_hdl);

    ejpeg_hdl = i2c_hdl = DD_HANDLE_UNVALID;

    os_memset(&ejpeg_cfg, 0, sizeof(DJPEG_DESC_ST));
}
/*---------------------------------------------------------------------------*/

#endif // CFG_USE_CAMERA_INTF
