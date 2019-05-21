#include "include.h"
#include "uart_pub.h"
#include "fusb_pub.h"
#include "fusb.h"
#include "usb_pub.h"
#include "drv_model_pub.h"
#include "sys_rtos.h"
#include "rtos_pub.h"
#include "diskio.h"
#include "usb_pub.h"

#if CFG_USB
volatile int g_usb_flag = 0;
xTaskHandle  usb_test_thread_handle;

extern void test_mount(DISK_NUMBER number);
extern void scan_file_system(DISK_NUMBER number);
extern void test_fatfs(DISK_NUMBER number);

static void usb_test_thread_main( void *arg )
{
    FUSB_PRT("usb_test_thread_main\r\n");
    while (1)
    {
        if (MUSB_GetConnect_Flag())
        {
            FUSB_PRT("usb_test_thread_main: Udisk Connected\r\n");
            if (g_usb_flag == 0)
            {
                g_usb_flag = 1;
                FUSB_PRT("test_mount\r\n");
                test_mount(DISK_NUMBER_UDISK);
            }
            else if (g_usb_flag == 1)
            {
                g_usb_flag = 2;
                FUSB_PRT("scan_file_system\r\n");
                scan_file_system(DISK_NUMBER_UDISK);
            }
            else if (g_usb_flag == 2)
            {
                g_usb_flag = 3;
                FUSB_PRT("test_fatfs\r\n");
                test_fatfs(DISK_NUMBER_UDISK);
            }
        }
        else
        {
            FUSB_PRT("usb_test_thread_main: Udisk Disconnected\r\n");
        }
    }
}

UINT32 fusb_init(void)
{
    UINT32 ret;
    UINT32 status;
    UINT32 op_flag;
    DD_HANDLE usb_handler;

    ret = FUSB_SUCCESS;

#ifdef FMSC_TEST
    fmsc_test_init();
#elif defined(FUVC_TEST)
    //fuvc_test_init();
#else
#endif

    op_flag = USB_HOST_MODE;
    usb_handler = ddev_open(USB_DEV_NAME, &status, op_flag);
    if(DD_HANDLE_UNVALID == usb_handler)
    {
        FUSB_PRT("usb_open_failed\r\n");
    }
    else if(USB_SUCCESS == status)
    {
        FUSB_PRT("usb_open_success\r\n");
    }

#ifdef FUSB_ENABLE_USER_MAIN
    ret = rtos_create_thread(&usb_test_thread_handle,
                             5,
                             "usb_test_thread",
                             (beken_thread_function_t)usb_test_thread_main,
                             (unsigned short)2048,
                             (beken_thread_arg_t)0);
#endif // FUSB_ENABLE_USER_MAIN

    return ret;
}

#ifdef FMSC_TEST
#define TEST_BUFFER_SIZE        512
#define FIRST_BLOCK             1
#define BLOCK_COUNT             1

UINT8 test_buff[TEST_BUFFER_SIZE] = {0};

void fmsc_test_init(void)
{
    UINT32 status;
    void *parameter;
    DD_HANDLE usb_hdl;

    FUSB_PRT("fmsc_test_init\r\n");
    usb_hdl = ddev_open(USB_DEV_NAME, &status, 0);
    if(DD_HANDLE_UNVALID == usb_hdl)
    {
        goto init_failed;
    }

    parameter = (void *)fmsc_fiddle_process;
    ddev_control(usb_hdl, UCMD_MSC_REGISTER_FIDDLE_CB, parameter);
    ddev_close(usb_hdl);

init_failed:
    return;
}

void fmsc_fiddle_process(void)
{
}
#endif // FMSC_TEST

#ifdef FUVC_TEST
#include "schedule_pub.h"

#ifdef CFG_SUPPORT_UVC
#define TEST_BUFFER_SIZE        1024 * 50
#else
#define TEST_BUFFER_SIZE        1024 * 16
#endif

UINT8 test_buff[TEST_BUFFER_SIZE] = {0};

void fuvc_test_init( uint8_t LinkType )
{
    UINT32 param;
    UINT32 status;
    void *parameter;
    DD_HANDLE usb_hdl;

    usb_hdl = ddev_open(USB_DEV_NAME, &status, 0);
    if(DD_HANDLE_UNVALID == usb_hdl)
    {
        goto init_error;
    }

    parameter = (void *)fuvc_notify_uvc_configed;
    ddev_control(usb_hdl, UCMD_UVC_REGISTER_CONFIG_NOTIFY_CB, parameter);
    parameter = (void *)fuvc_fiddle_rx_vs;
    ddev_control(usb_hdl, UCMD_UVC_REGISTER_RX_VSTREAM_CB, parameter);

    parameter = (void *)test_buff;
    ddev_control(usb_hdl, UCMD_UVC_REGISTER_RX_VSTREAM_BUF_PTR, parameter);
    param = TEST_BUFFER_SIZE;
    ddev_control(usb_hdl, UCMD_UVC_REGISTER_RX_VSTREAM_BUF_LEN, &param);

    param = LinkType;
    ddev_control(usb_hdl, UCMD_UVC_REGISTER_LINK, &param);

#ifdef UVC_DEMO_SUPPORT100
    param = UVC_MUX_PARAM(U1_FRAME_640_480, FPS_30);
    ddev_control(usb_hdl, UCMD_UVC_SET_PARAM, &param);
#endif

    process_start(&fuvc_test, NULL);

init_error:
    return;
}

void fuvc_notify_uvc_configed(void)
{
    process_post(&fuvc_test, PROCESS_EVENT_MSG, NULL);
}

void fuvc_fiddle_rx_vs(void)
{
    process_post(&fuvc_test, PROCESS_EVENT_POLL, NULL);
}

PROCESS_THREAD(fuvc_test, ev, data)
{
    UINT32 status;
    static DD_HANDLE usb_hdl;

    PROCESS_BEGIN();

    while(51)
    {
        usb_hdl = ddev_open(USB_DEV_NAME, &status, 0);
        ASSERT(DD_HANDLE_UNVALID != usb_hdl);

        PROCESS_WAIT_EVENT();

        if(PROCESS_EVENT_MSG == ev)
        {
#ifdef UVC_DEMO_SUPPORT102
            UINT32 param;

            ddev_control(usb_hdl, UCMD_UVC_ENABLE_H264, 0);
            param = UVC_MUX_PARAM(UVC_FRAME_640_480, FPS_30);
            ddev_control(usb_hdl, UCMD_UVC_SET_PARAM, &param);
#endif

            ddev_control(usb_hdl, UCMD_UVC_START_STREAM, 0);
        }
        else if(PROCESS_EVENT_POLL == ev)
        {
            ddev_control(usb_hdl, UCMD_UVC_RECEIVE_VSTREAM, 0);
        }
        else if(PROCESS_EVENT_EXIT == ev)
        {
            ddev_close(usb_hdl);
            usb_hdl = 0;
        }
        else
        {
        }
    }

    PROCESS_END();
}

#endif // FUVC_TEST
#endif // CFG_USB

// eof

