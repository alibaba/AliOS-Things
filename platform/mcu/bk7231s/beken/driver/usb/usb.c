#include "include.h"
#include "typedef.h"
#include "arm_arch.h"

#include "usb_pub.h"
#include "usb.h"

#include "drv_model_pub.h"
#include "icu_pub.h"
#include "sys_ctrl_pub.h"
#include "intc_pub.h"

#if CFG_USB
#include "usb_msd.h"
#include "target_util_pub.h"

#include "board.h"
#include "ps.h"
#include "brd_cnf.h"
#include "uart_pub.h"

#define USB_BACKGROUND_STACK_SIZE     (2 * 1024)
beken_thread_t ubg_thread_handle = NULL;
beken_semaphore_t ubg_semaphore = NULL;
uint32_t usb_connected_flag = 0;

#if CFG_SUPPORT_MSD
#include "usb_msd.h"
#elif CFG_SUPPORT_HID
#include "usb_hid.h"
#elif CFG_SUPPORT_UVC
#include "usb_uvc.h"
#else
#endif

static DD_OPERATIONS usb_op =
{
    usb_open,
    usb_close,
    usb_read,
    usb_write,
    usb_ctrl
};


FUNCPTR usb_connected_cb = 0;

/*******************************************************************/
void usb_init(void)
{
    intc_service_register(IRQ_USB, PRI_IRQ_USB, usb_isr);
    ddev_register_dev(USB_DEV_NAME, &usb_op);

    USB_PRT("usb_init\r\n");
}

void usb_exit(void)
{
    ddev_unregister_dev(USB_DEV_NAME);

    USB_PRT("usb_exit\r\n");
}

void usb_event_post(void)
{
    if(ubg_semaphore)
    {
        rtos_set_semaphore(&ubg_semaphore);
    }
    else
    {
        USB_PRT("usb_event_post null_function_ptr\r\n");
    }
}

void usb_thread_background(void *arg)
{
    uint32_t ret;
    OSStatus result;

    while(1)
    {
        result = rtos_get_semaphore(&ubg_semaphore, BEKEN_WAIT_FOREVER);
        if(kNoErr == result)
        {
            ret = MUSB_NoneRunBackground();
            switch(ret)
            {
            case BSR_CONNECTED_EVENT:
                usb_connected_flag = 1;
                if(usb_connected_cb)
                {
                    (*usb_connected_cb)();
                }
                USB_PRT("BSR_CONNECTED_EVENT\r\n");
                break;

            case BSR_CONNECT_EVENT:
                USB_PRT("BSR_CONNECT_EVENT\r\n");
                break;

            case BSR_DISCONNECT_EVENT:
                usb_connected_flag = 0;
                USB_PRT("BSR_DISCONNECT_EVENT\r\n");
                break;

            case BSR_ERROR_EVENT:
                USB_PRT("BSR_ERROR_EVENT\r\n");
                break;

            default:
                USB_NPRT("BSR_OTHER:0x%x\r\n", ret);
                break;
            }
        }
    }
}

UINT32 usb_sw_open(void)
{
    OSStatus ret = USB_SUCCESS;

    if(NULL == ubg_thread_handle)
    {
        /* usb backgroud thread create*/
        ret = rtos_create_thread(&ubg_thread_handle,
                                 THD_UBG_PRIORITY,
                                 "ubg_thread",
                                 (beken_thread_function_t)usb_thread_background,
                                 (unsigned short)USB_BACKGROUND_STACK_SIZE,
                                 (beken_thread_arg_t)0);
        if (kNoErr != ret)
        {
            USB_PRT("create background thread failed\r\n");
            goto sw_open_exit;
        }
    }

    if(NULL == ubg_semaphore)
    {
        ret = rtos_init_semaphore(&ubg_semaphore, 10);
        if (kNoErr != ret)
        {
            USB_PRT("create background sema failed\r\n");
            goto sw_open_exit;
        }
    }

sw_open_exit:
    return ret;
}

/* op_flag: USB_HOST_MODE or USB_DEVICE_MODE*/
UINT32 usb_open (UINT32 op_flag)
{
    UINT8 reg;
    UINT32 param, ret;
    UINT32 usb_mode = op_flag;

    USB_PRT("usb_open\r\n");

#if ((SOC_BK7231U == CFG_SOC_NAME) || (SOC_BK7221U == CFG_SOC_NAME))
    USB_PRT("gpio_usb_second_function\r\n");
    gpio_usb_second_function();
#endif

    /*step0.0: power up usb subsystem*/
    param = 0;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_POWERUP, &param);

    /*step 1.0: reset usb module*/
    param = 0;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_SUBSYS_RESET, &param);

    /*step1.1: open clock*/
    param = BLK_BIT_DPLL_480M | BLK_BIT_USB;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_BLK_ENABLE, &param);

    param = MCLK_SELECT_DPLL;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_SELECT, &param);

    param = USB_DPLL_DIVISION;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_MCLK_DIVISION, &param);

    /*step2: config clock power down for peripheral unit*/
    param = PWD_USB_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);

    VREG_USB_TEST_MODE = 0x01;

#ifdef GUWENFU_SETTING
    VREG_USB_INTRRX1E = 0x0;
    VREG_USB_INTRTX1E = 0x0;
    VREG_USB_INTRUSBE = 0x0;
    REG_AHB2_USB_VTH &= ~(1 << 7);
#endif // 

#ifdef MUSB_FORCE_FULLSPEED
    VREG_USB_POWER = 0x01;
#else
    VREG_USB_POWER |= 0x21;
#endif

    VREG_USB_FADDR = 0;
    VREG_USB_DEVCTL = 0x01;

#ifdef GUWENFU_SETTING
    if (usb_mode == USB_HOST_MODE)
    {
        USB_PRT("usb host\r\n");
        REG_AHB2_USB_OTG_CFG = 0x50;        // host
        REG_AHB2_USB_DEV_CFG = 0x00;
    }
    else
    {
        USB_PRT("usb device\r\n");
        VREG_USB_INTRRX1E = 0x07;
        VREG_USB_INTRTX1E = 0x07;
        VREG_USB_INTRUSBE = 0x3F;

        REG_AHB2_USB_OTG_CFG = 0x08;        // dp pull up
        REG_AHB2_USB_DEV_CFG = 0xF4;
        REG_AHB2_USB_OTG_CFG |= 0x01;       // device
    }

    reg = REG_AHB2_USB_INT;
    delay(100);
    REG_AHB2_USB_INT = reg;
    delay(100);

    /*dp and dn driver current selection*/
    REG_AHB2_USB_GEN = (0x7 << 4) | (0x7 << 0);
#endif
#if CFG_USB
    if (usb_sw_init())
    {
        os_printf("usb_sw_init ERROR\r\n");
    }

    ret = usb_sw_open();
    ASSERT(USB_SUCCESS == ret);

    os_printf("usb_sw_init OK\r\n");
#endif

    /*step2: interrupt setting about usb*/
    intc_enable(IRQ_USB);

    param = GINTR_IRQ_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_GLOBAL_INT_ENABLE, &param);

    return USB_SUCCESS;
}

UINT32 usb_close (void)
{
    UINT32 param;

    USB_PRT("usb_close\r\n");

    param = IRQ_USB_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);

    param = PWD_USB_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_DOWN, &param);

#if CFG_USB
    usb_sw_uninit();
#endif

    return USB_SUCCESS;
}

UINT32 usb_read (char *user_buf, UINT32 count, UINT32 op_flag)
{
    USB_PRT("usb_read\r\n");
    return USB_SUCCESS;
}

UINT32 usb_write (char *user_buf, UINT32 count, UINT32 op_flag)
{
    USB_PRT("usb_write\r\n");
    return USB_SUCCESS;
}

#if CFG_SUPPORT_UVC
UINT32 usb_uvc_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret;

    ret = USB_SUCCESS;

    switch(cmd)
    {
    case UCMD_UVC_SET_PARAM:
    {
        UINT32 resolution_id;
        UINT32 fps;

        if(0 == param)
        {
            return USB_FAILURE;
        }

        fps = UVC_DEMUX_FPS(*((UINT32 *)param));
        resolution_id = UVC_DEMUX_ID(*((UINT32 *)param));
        MGC_UvcSetParameter(resolution_id, fps);
    }
    break;

    case UCMD_UVC_ENABLE_H264:
        MGC_UvcEnableH264();
        break;

    case UCMD_UVC_ENABLE_MJPEG:
        MGC_UvcEnableMjpeg();
        break;

    case UCMD_UVC_REGISTER_RX_VSTREAM_BUF_PTR:
        MGC_UvcRegisterRxVstreamBufPtr(param);
        break;

    case UCMD_UVC_REGISTER_RX_VSTREAM_BUF_LEN:
        MGC_UvcRegisterRxVstreamBufLen((*(uint32_t *)param));
        break;

    case UCMD_UVC_RECEIVE_VSTREAM:
        ret = MGC_UvcReceiveVideoStream();
        break;

    case UCMD_UVC_REGISTER_CONFIG_NOTIFY_CB:
        MGC_UvcRegisterConfiguredCallback((FUNCPTR)param);
        break;

    case UCMD_UVC_REGISTER_RX_VSTREAM_CB:
        MGC_UvcRegisterVSRxedCallback((FUNCPTR)param);
        break;

    case UCMD_UVC_REGISTER_LINK:
        MGC_UvcRegisterLink( *(uint32_t *)param );
        break;

    case UCMD_UVC_START_STREAM:
        ret = MGC_UvcStartStream();
        break;

    case UCMD_UVC_STOP_STREAM:
        ret = MGC_UvcStopStream();
        break;

    case UCMD_UVC_GET_CONNECT_STATUS:
        ret = MGC_UvcGetConnectStatus();
        break;

    default:
        break;
    }

    return ret;
}
#endif // CFG_SUPPORT_UVC

UINT32 usb_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret;

    ret = USB_SUCCESS;

    switch(cmd)
    {
    case UCMD_RESET:
        break;

    case UCMD_USB_CONNECTED_REGISTER_CB:
        usb_connected_cb = param;
        if(usb_connected_cb)
        {
            (*usb_connected_cb)();
        }
        break;

#if CFG_SUPPORT_MSD
    case UCMD_MSC_REGISTER_FIDDLE_CB:
        MGC_RegisterCBTransferComplete((FUNCPTR)param);
        break;
#endif // CFG_SUPPORT_MSD

    default:
        break;
    }

#if CFG_SUPPORT_UVC
    ret = usb_uvc_ctrl(cmd, param);
#endif // CFG_SUPPORT_UVC

    return ret;
}

void usb_isr(void)
{
    MGC_AfsUdsIsr();
}

#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
UINT32 usb_plug_inout_open(UINT32 op_flag);
UINT32 usb_plug_inout_close(void);

USB_PLUG_INOUT_ST usb_plug;
static DD_OPERATIONS usb_plug_op =
{
    usb_plug_inout_open,
    usb_plug_inout_close,
    NULL,
    NULL,
    NULL
};

static void usb_plug_inout_icu_int_open(void)
{
    UINT32 param;
    param = (FIQ_USB_PLUG_INOUT_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

static void usb_plug_inout_icu_int_close(void)
{
    UINT32 param;
    param = (FIQ_USB_PLUG_INOUT_BIT);
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);
}

UINT32 usb_plug_inout_open(UINT32 op_flag)
{
    UINT32 param;
    USB_PLUG_INOUT_ST *cfg;

    cfg = (USB_PLUG_INOUT_ST*)op_flag;
    usb_plug.handler = cfg->handler;
    usb_plug.usr_data = cfg->usr_data;

    param = 0;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_POWERUP, &param);
    
    usb_plug_inout_icu_int_open();
    
    param = 1;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_EN_USB_PLUG_IN_INT, &param);

    param = 1;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_EN_USB_PLUG_OUT_INT, &param);

    if(usb_is_plug_in())
        param = USB_PLUG_IN_EVENT;
    else
        param = USB_PLUG_OUT_EVENT;

    if(usb_plug.handler)
        usb_plug.handler(usb_plug.usr_data, param);

    return USB_PLUG_SUCCESS;
}

UINT32 usb_plug_inout_close(void)
{
    UINT32 param;
    param = 0;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_EN_USB_PLUG_IN_INT, &param);

    param = 0;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_EN_USB_PLUG_OUT_INT, &param);

    usb_plug_inout_icu_int_close();

    param = 0;
    sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_USB_POWERDOWN, &param);

    usb_plug.handler = NULL;
    usb_plug.usr_data = NULL;

	return 0;
}

void usb_plug_inout_init(void) 
{
    usb_plug.handler = NULL;
    usb_plug.usr_data = NULL;
    intc_service_register(FIQ_USB_PLUG_INOUT, PRI_FIQ_USB_PLUG_INOUT, usb_plug_inout_isr);
    ddev_register_dev(USB_PLUG_DEV_NAME, &usb_plug_op);
}

void usb_plug_inout_exit(void)
{
    ddev_unregister_dev(USB_PLUG_DEV_NAME);
}

#endif
// EOF
