#include "include.h"
#include "arm_arch.h"

#if CFG_USE_CAMERA_INTF
#include "jpeg_encoder_pub.h"
#include "jpeg_encoder.h"

#include "intc_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"

#include "drv_model_pub.h"
#include "mem_pub.h"
#include "general_dma_pub.h"

#define JPEG_BITRATE_MAX_SIZE           (40 * 1024)
#define JPEG_BITRATE_MIN_SIZE           (20 * 1024)

const UINT32 jpeg_quant_table[JPEG_QUANT_TAB_LEN] = {
    0x07060608, 0x07080506, 0x09090707, 0x140c0a08,
    0x0b0b0c0d, 0x1312190c, 0x1a1d140f, 0x1a1d1e1f,
    0x24201c1c, 0x2220272e, 0x1c1c232c, 0x2c293728,
    0x34343130, 0x39271f34, 0x3c32383d, 0x3234332e,
    0x0c090909, 0x0d180c0b, 0x2132180d, 0x3232211c,
    0x32323232, 0x32323232, 0x32323232, 0x32323232,
    0x32323232, 0x32323232, 0x32323232, 0x32323232,
    0x32323232, 0x32323232, 0x32323232, 0x32323232
};

static DJPEG_DESC_PTR p_ejpeg = NULL;
static DD_OPERATIONS ejpeg_op =
{
    ejpeg_open,
    ejpeg_close,
    NULL,
    NULL,
    ejpeg_ctrl
};

static void ejpeg_set_start_frame_int(UINT32 enable)
{
    UINT32 reg_addr = JPEG_REG0;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= START_FRM_INT;
    else
        reg_val &= ~START_FRM_INT;
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_set_end_frame_int(UINT32 enable)
{
    UINT32 reg_addr = JPEG_REG0;
    UINT32 reg_val = REG_READ(reg_addr);
    
    if(enable)
        reg_val |= END_FRM_INT;
    else
        reg_val &= ~END_FRM_INT;
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_set_mclk_div(UINT32 div)
{
    UINT32 reg_addr = JPEG_REG0;
    UINT32 reg_val = REG_READ(reg_addr);
    
    reg_val = (reg_val & ~(DIV_MASK << DIV_POSI)) 
        | ((div & DIV_MASK) << DIV_POSI);
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_set_video_byte_reverse(UINT32 reverse)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);

    if(reverse)
        reg_val |= VIDEO_BYTE_REV;
    else
        reg_val &= ~VIDEO_BYTE_REV;
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_select_yuv_format(UINT32 format)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);
    
    reg_val = (reg_val & ~(YUV_FMT_SEL_MASK<< YUV_FMT_SEL_POSI)) 
        | ((format & YUV_FMT_SEL_MASK) << YUV_FMT_SEL_POSI);
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_set_encoder_enable(UINT32 enable)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);

    if(enable)
        reg_val |= JPEG_ENC_EN;
    else
        reg_val &= ~JPEG_ENC_EN;
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_set_x_pixel(UINT32 x_piexl)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);
    
    reg_val = (reg_val & ~(X_PIXEL_MASK<< X_PIXEL_POSI)) 
        | ((x_piexl & X_PIXEL_MASK) << X_PIXEL_POSI);
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_enable_enc_size(UINT32 enable)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);
    
    if(enable)
        reg_val |= JPEG_ENC_SIZE;
    else
        reg_val &= ~JPEG_ENC_SIZE;
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_enable_bitrate_ctrl(UINT32 enable)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);
    
    if(enable)
        reg_val |= BIT_RATE_CTRL;
    else
        reg_val &= ~BIT_RATE_CTRL;
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_set_bitrate_step(UINT32 step)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);
    
    reg_val = (reg_val & ~(BIT_RATE_STEP_MASK<< BIT_RATE_STEP_POSI)) 
        | ((step & BIT_RATE_STEP_MASK) << BIT_RATE_STEP_POSI);
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_set_hsync_reverse(UINT32 reverse)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);
    
    if(reverse)
        reg_val |= HSYNC_REV;
    else
        reg_val &= ~HSYNC_REV;
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_set_vsync_reverse(UINT32 reverse)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);
    
    if(reverse)
        reg_val |= VSYNC_REV;
    else
        reg_val &= ~VSYNC_REV;
    REG_WRITE(reg_addr, reg_val);
}

static void ejpeg_set_y_pixel(UINT32 y_piexl)
{
    UINT32 reg_addr = JPEG_REG1;
    UINT32 reg_val = REG_READ(reg_addr);
    
    reg_val = (reg_val & ~(Y_PIXEL_MASK<< Y_PIXEL_POSI)) 
        | ((y_piexl & Y_PIXEL_MASK) << Y_PIXEL_POSI);
    REG_WRITE(reg_addr, reg_val);
}

void ejpeg_set_target_high_byte(UINT32 high)
{
    REG_WRITE(JPEG_REG2, high & TARGET_BYTE_H_MASK);
}

UINT32 ejpeg_get_target_high_byte(void)
{
    return REG_READ(JPEG_REG2) & TARGET_BYTE_H_MASK;
}

void ejpeg_set_target_low_byte(UINT32 low)
{
    REG_WRITE(JPEG_REG3, low & TARGET_BYTE_L_MASK);
}

static UINT32 ejpeg_get_target_low_byte(void)
{
    return REG_READ(JPEG_REG3) & TARGET_BYTE_L_MASK;
}

static UINT32 ejpeg_get_frame_len(void)
{
    return REG_READ(JPEG_REG7) & BYTE_CNT_PFRM_MASK;
}

static void ejpeg_gpio_config(void)
{
    UINT32 param;
    param = GFUNC_MODE_DCMI;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
}

static void ejpeg_init_quant_table(void)
{
    UINT32 i;
    UINT32 reg_addr;
  
    for(i=0; i<JPEG_QUANT_TAB_LEN; i++) {
        reg_addr = JPEG_REG20_QUANT_TAB + i*4;
        REG_WRITE(reg_addr, (UINT32)jpeg_quant_table[i]);
    }
}

static void ejpeg_power_up(void)
{
    UINT32 param;
    param = PWD_JEPG_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);
}

static void ejpeg_power_down(void)
{
    UINT32 param;
    param = PWD_JEPG_CLK_BIT;
	sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);
}

static void ejpeg_enable_interrupt(void)
{
    UINT32 param;
    param = (FIQ_JPEG_DECODER_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

static void ejpeg_disable_interrupt(void)
{
    UINT32 param;
    param = (FIQ_JPEG_DECODER_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

#if CFG_GENERAL_DMA
static void ejpeg_config_rxdma(void)
{
    GDMACFG_TPYES_ST cfg;
    GDMA_CFG_ST en_cfg;
    os_memset(&cfg, 0, sizeof(GDMACFG_TPYES_ST));
    
    cfg.dstdat_width = 32;
    cfg.srcdat_width = 32;
    cfg.dstptr_incr = 1;
    cfg.srcptr_incr = 0;
    cfg.src_start_addr = (void*)JPEG_REG5;
    cfg.dst_start_addr = &(p_ejpeg->rxbuf[0]);

    cfg.channel = p_ejpeg->dma_channel;
    cfg.prio = 0;
    cfg.u.type5.dst_loop_start_addr = &(p_ejpeg->rxbuf[0]);
    cfg.u.type5.dst_loop_end_addr = &(p_ejpeg->rxbuf[p_ejpeg->rxbuf_len]);

    cfg.fin_handler = p_ejpeg->dma_rx_handler;
    
    cfg.src_module = GDMA_X_SRC_JPEG_WR_REQ;
    cfg.dst_module = GDMA_X_DST_DTCM_WR_REQ;

    sddev_control(GDMA_DEV_NAME, CMD_GDMA_CFG_TYPE5, &cfg);

    en_cfg.channel = p_ejpeg->dma_channel;
    en_cfg.param = p_ejpeg->node_len; // dma translen
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_TRANS_LENGTH, &en_cfg);

    en_cfg.channel = p_ejpeg->dma_channel;
    en_cfg.param = 1;
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);
}

static void ejpeg_eixt_rxdma(void)
{
    GDMA_CFG_ST en_cfg;

    en_cfg.channel = p_ejpeg->dma_channel;
    en_cfg.param = 1;
    sddev_control(GDMA_DEV_NAME, CMD_GDMA_SET_DMA_ENABLE, &en_cfg);
}
#endif

static void ejpeg_isr(void)
{
    UINT32 status = REG_READ(JPEG_REG6);

    if(status & START_FRM_INT_STATUS) {
        if((p_ejpeg) &&(p_ejpeg->start_frame_handler)) {
            p_ejpeg->start_frame_handler();
        }
    }

    if(status & END_FRM_INT_STATUS) {
        if((p_ejpeg) &&(p_ejpeg->end_frame_handler)) {
            p_ejpeg->end_frame_handler();
        }
    }

    REG_WRITE(JPEG_REG6, status);
}

static void ejpeg_software_init(void)
{
    ddev_register_dev(EJPEG_DEV_NAME, &ejpeg_op);
}

static void ejpeg_hardware_init(void)
{
    UINT32 reg;
    /* register interrupt */
    intc_service_register(FIQ_JPEG_ENCODER, PRI_FIQ_JPEG_DECODER, ejpeg_isr);

    REG_WRITE(JPEG_REG0, 0);

    /* reset int status */
    reg = REG_READ(JPEG_REG6);
    REG_WRITE(JPEG_REG6, reg);

    /* config gpio */
}

void ejpeg_init(void)
{
    ejpeg_software_init();
    ejpeg_hardware_init();
}

void ejpeg_exit(void)
{
    REG_WRITE(JPEG_REG0, 0);
    REG_WRITE(JPEG_REG6, REG_READ(JPEG_REG6));

    ddev_unregister_dev(EJPEG_DEV_NAME);
}

static UINT32 ejpeg_open(UINT32 op_flag)
{
   // UINT32 reg;
    if(!op_flag) {
        EJPEG_PRT("ejpeg_open is NULL\r\n");
        return EJPEG_FAILURE;
    }

    p_ejpeg = (DJPEG_DESC_PTR)op_flag;

    ejpeg_set_encoder_enable(0);
    ejpeg_init_quant_table();

    ejpeg_set_x_pixel(p_ejpeg->x_pixel);
    ejpeg_set_y_pixel(p_ejpeg->y_pixel);    

    if(p_ejpeg->start_frame_handler)
        ejpeg_set_start_frame_int(1);
    if(p_ejpeg->end_frame_handler)
        ejpeg_set_end_frame_int(1);

    // this 4 byte size attched to the end of JPEG, use to check crc 
    ejpeg_enable_enc_size(1);
    ejpeg_set_video_byte_reverse(1);

    ejpeg_set_target_high_byte(JPEG_BITRATE_MAX_SIZE);
    ejpeg_set_target_low_byte(JPEG_BITRATE_MIN_SIZE);
    ejpeg_set_bitrate_step(7);
    ejpeg_enable_bitrate_ctrl(1);
    
    ejpeg_enable_interrupt();
    ejpeg_power_up();
    ejpeg_gpio_config();

    #if CFG_GENERAL_DMA
    ejpeg_config_rxdma();
    #endif

    ejpeg_set_encoder_enable(1);

    return EJPEG_SUCCESS;
}

static UINT32 ejpeg_close(void)
{
    ejpeg_set_encoder_enable(0);
    
    #if CFG_GENERAL_DMA
    ejpeg_eixt_rxdma();
    #endif

    ejpeg_disable_interrupt();
    ejpeg_power_down();

    return EJPEG_SUCCESS;
}

static UINT32 ejpeg_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = EJPEG_SUCCESS;

    switch(cmd)
    {
    case EJPEG_CMD_SET_START_FRAME_INT:
        ejpeg_set_start_frame_int(*((UINT32 *)param));
        break;
    case EJPEG_CMD_SET_END_FRAME_INT:
        ejpeg_set_end_frame_int(*((UINT32 *)param));
        break;
    case EJPEG_CMD_SET_MCLK_DIV:
        ejpeg_set_mclk_div(*((UINT32 *)param));
        break;
    case EJPEG_CMD_SET_VIDEO_BYTE_REVERSE:
        ejpeg_set_video_byte_reverse(*((UINT32 *)param));
        break;
    case EJPEG_CMD_SELECT_YUV_FORMAT:
        ejpeg_select_yuv_format(*((UINT32 *)param));
        break;
    case EJPEG_CMD_SET_ENCODER_ENABLE:
        ejpeg_set_encoder_enable(*((UINT32 *)param));
        break;
    case EJPEG_CMD_SET_X_PIXEL:
        ejpeg_set_x_pixel(*((UINT32 *)param));
        break;
    case EJPEG_CMD_ENABLE_ENC_SIZE:
        ejpeg_enable_enc_size(*((UINT32 *)param));
        break;
    case EJPEG_CMD_ENABLE_BITRATE_CTRL:
        ejpeg_enable_bitrate_ctrl(*((UINT32 *)param));
        break;
    case EJPEG_CMD_SET_BITRATE_STEP:
        ejpeg_set_bitrate_step(*((UINT32 *)param));
        break;        
    case EJPEG_CMD_SET_HSYNC_REVERSE:
        ejpeg_set_hsync_reverse(*((UINT32 *)param));
        break;
    case EJPEG_CMD_SET_VSYNC_REVERSE:
        ejpeg_set_vsync_reverse(*((UINT32 *)param));
        break;
    case EJPEG_CMD_SET_Y_PIXEL:
        ejpeg_set_y_pixel(*((UINT32 *)param));
        break; 
    case EJPEG_CMD_GET_TARGET_HIGH_BYTE:
        ret = ejpeg_get_target_high_byte();
        break;        
    case EJPEG_CMD_GET_TARTGE_LOW_BYTE:
        ret = ejpeg_get_target_low_byte();
        break;
    case EJPEG_CMD_GET_FRAME_LEN:
        ret = ejpeg_get_frame_len();
        break;      
        
    default:
        break;
    }

    return ret;
}

#endif
