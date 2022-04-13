/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "cmsis.h"
#include "string.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "usb_hcd.h"
#include "usb_hcd_test.h"
#include "usb_mass_storage.h"
#include "hal_location.h"
#define  USBLOG 1
#if USBLOG
#define  TEST_LOG(s, ...)     TRACE(s, ##__VA_ARGS__)
#else
#define  TEST_LOG(s, ...)     do {} while(0)
#endif

#define TEST_HCD_MAILBOX_MAX   8

#define TEST_HCD_MESSAGE_PLUG        0
#define TEST_HCD_MESSAGE_INTERRUPT   1
#define TEST_HCD_MESSAGE_CHINT       1

#define TEST_HCD_USB_ADDR            1

#define USB_HCD_TEST_BLOCK_SIZE   512

typedef struct {
	uint32_t id;
	uint32_t param0;
    bool is_used;
} TEST_HCD_MESSAGE;

typedef enum _TEST_HCD_CASE_STATE
{
    TEST_HCD_CASE_STATE_IDLE,
    //TEST_HCD_CASE_STATE_GET_PORT_STATUS,
    TEST_HCD_CASE_STATE_PORT_ENABLE,
    TEST_HCD_CASE_STATE_PORT_RESET,
    TEST_HCD_CASE_STATE_CREATE_ENDPOINT,
    TEST_HCD_CASE_STATE_SET_ADDRESS,
    TEST_HCD_CASE_STATE_GET_DESCRIPT,
    TEST_HCD_CASE_STATE_SET_CONFIG,
    TEST_HCD_CASE_STATE_SET_ITF_EP,
    TEST_HCD_CASE_STATE_CREATE_NOZEOR_ENDPOINT,
    TEST_HCD_CASE_STATE_CREATE_SCSI_INIT,
    TEST_HCD_CASE_STATE_CREATE_SCSI_GET_CAPACITY,
    TEST_HCD_CASE_STATE_CREATE_SCSI_READ_BLOCK,
    TEST_HCD_CASE_STATE_CREATE_SCSI_WRITE_BLOCK,
}TEST_HCD_CASE_STATE;


typedef struct
{
    bool is_used;
    bool is_in;
    USBHOST_ENDPOINT_DESCRIPTOR_T endpoint;
} HCD_ENDPOINT;

typedef struct
{
    int device;
    int ep_number;
    HCD_ENDPOINT ep[USB_HCD_MAX_ENDPOINTS];
}USB_DIVICE;


typedef void (*TEST_HCD_FUNC_T)(uint32_t, uint32_t);

typedef void (*TEST_HCD_THREAD)(void*);

static USB_HCD_T g_hcd;
static USB_HCD_PLUG_STATUS_T g_plug_status;
static USB_HCD_INTSTS_T  g_interrupt_status;
static USB_PORT_STATUS_T g_port_status;
static uint32_t g_frame_number;
static int g_device;
static bool is_create_ep0;
static bool is_inited = false;
static TEST_HCD_MESSAGE _hcd_msg[TEST_HCD_MAILBOX_MAX];
static uint32_t _enter_index;
static uint32_t _out_index;
static USB_DIVICE usb_device;
static TEST_HCD_CASE_STATE test_case_state;


static uint8_t _read_buffer[USB_HCD_TEST_BLOCK_SIZE];
static uint8_t _write_buffer[USB_HCD_TEST_BLOCK_SIZE];

static void test_hcd_thread(void const *argument);
static void test_app_thread(void const *argument);
osThreadDef(test_hcd_thread, osPriorityNormal, 1, 4096, "TEST_HCD_Thread");
osThreadDef(test_app_thread, osPriorityNormal, 1, 4096, "TEST_APP_Thread");

static void test_hcd_msg_init(void)
{
    uint32_t i;
    for(i = 0; i < sizeof(_hcd_msg)/sizeof(_hcd_msg[0]); i++)
    {
        _hcd_msg[i].id = 0;
        _hcd_msg[i].param0 = 0;
        _hcd_msg[i].is_used = false;
    }
    _enter_index = 0;
    _out_index = 0;
}

static TEST_HCD_MESSAGE* _get_used(void)
{
    uint32_t i;
    uint32_t index;
    TEST_HCD_MESSAGE* msg = NULL;

    ASSERT(_enter_index >= _out_index,"%s: _enter_index= 0x%x,_out_index = 0x%x.",
           __func__, _enter_index, _out_index);

    if(_enter_index == _out_index)
    {
        return NULL;
    }
    // TEST_LOG("%s:  _out_index = %d.", __func__, _out_index);
    index = _out_index % TEST_HCD_MAILBOX_MAX;
    for(i = 0; i < TEST_HCD_MAILBOX_MAX; i++)
    {
        if(_hcd_msg[index].is_used == true)
        {
            msg = &_hcd_msg[index];
            _out_index ++;
            // TEST_LOG("%s: index = %d, _out_index = %d.", __func__, index, _out_index);
            break;
        }
        index = index == TEST_HCD_MAILBOX_MAX - 1 ? 0 : index + 1;
    }
    return msg;
}

static TEST_HCD_MESSAGE* _get_free(void)
{
    uint32_t i;
    uint32_t index;
    TEST_HCD_MESSAGE* msg = NULL;

    ASSERT(_enter_index >= _out_index,"%s: _enter_index= 0x%x,_out_index = 0x%x.",
           __func__, _enter_index, _out_index);
    // TEST_LOG("%s:  _enter_index = %d.", __func__, _enter_index);
    if(_enter_index-_out_index == TEST_HCD_MAILBOX_MAX)
    {
        return NULL;
    }

    index = _enter_index % TEST_HCD_MAILBOX_MAX;

    for(i = 0; i < TEST_HCD_MAILBOX_MAX; i++)
    {
        if(_hcd_msg[index].is_used == false)
        {
            msg = &_hcd_msg[index];
            msg->is_used = true;
            _enter_index ++;
            // TEST_LOG("%s: index = %d, _enter_index = %d.", __func__, index, _enter_index);
            break;
        }
        index = index == TEST_HCD_MAILBOX_MAX - 1 ? 0 : index + 1;
    }
    return msg;
}



static bool _msg_add(uint32_t id,uint32_t param)
{
    TEST_HCD_MESSAGE* msg;
    uint32_t lock;
    bool ret = false;

    lock = int_lock_global();
    msg = _get_free();
    if(msg)
    {
        msg->id = id;
        msg->param0 = param;
        ret = true;
    }
    int_unlock_global(lock);
    return ret;
}

static TEST_HCD_MESSAGE* _msg_get(void)
{
    TEST_HCD_MESSAGE* msg;
    uint32_t lock;

    lock = int_lock_global();
    msg = _get_used();
    int_unlock_global(lock);
    return msg;
}

static void _msg_free(TEST_HCD_MESSAGE* msg)
{
    uint32_t lock;

    lock = int_lock_global();
    msg->id = 0;
    msg->param0 = 0;
    msg->is_used = false;
    int_unlock_global(lock);
}


void test_usb_hcd_get_interrupt_status(void);
void test_usb_hcd_get_port_status(void);

static void test_hcd_thread(void const *argument)
{
    int i;
    //TEST_HCD_FUNC_T pfunc;
    while(1)
    {
        if(is_inited)
        {
            break;
        }
        osDelay(10);
    }

    while(1){
        TEST_HCD_MESSAGE *msg_p = _msg_get();
        if (msg_p) {

            TEST_LOG("%s: id = 0x%x, param0 = 0x%x.",
                __func__,
                msg_p->id,
                msg_p->param0);

            if(msg_p->id == TEST_HCD_MESSAGE_INTERRUPT)
            {
                test_usb_hcd_get_interrupt_status();
                test_usb_hcd_cmd_get_port_status();
                //test_case_state = TEST_HCD_CASE_STATE_GET_PORT_STATUS;
                TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_GET_PORT_STATUS", __func__);

                if(g_interrupt_status == USB_HCD_INTSTS_PRTINT)
                {
                    if(test_case_state < TEST_HCD_CASE_STATE_CREATE_ENDPOINT)
                    {
                        if(g_port_status & USB_PORT_STATUS_CONNECTION)
                        {
                            if(g_port_status & USB_PORT_STATUS_FULL_SPEED)
                            {
                                test_usb_hcd_cmd_create_point(0,0,HAL_USBHOST_EP_CTRL,0,8);
                                is_create_ep0 = true;
                                test_case_state = TEST_HCD_CASE_STATE_CREATE_ENDPOINT;
                                TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_CREATE_ENDPOINT", __func__);
                            }
                            else
                            {
                                test_usb_hcd_cmd_reset_port();
                                test_case_state = TEST_HCD_CASE_STATE_PORT_RESET;
                                TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_PORT_RESET", __func__);
                            }
                        }

                    }
                    else
                    {

                    }
                }
                else if(g_interrupt_status == USB_HCD_INTSTS_HCHINT)
                {
                    if(test_case_state ==  TEST_HCD_CASE_STATE_CREATE_ENDPOINT)
                    {
                        test_usb_hcd_cmd_get_descriptor(DESCRIPTOR_TYPE_DEVICE);

                        test_usb_hcd_cmd_set_addr();
                        test_case_state = TEST_HCD_CASE_STATE_SET_ADDRESS;
                        TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_SET_ADDRESS", __func__);
                        //osDelay(20);
                        test_usb_hcd_cmd_get_descriptor(DESCRIPTOR_TYPE_CONFIG);
                        test_case_state = TEST_HCD_CASE_STATE_GET_DESCRIPT;
                        TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_GET_DESCRIPT", __func__);

                        //osDelay(2); // for output log.
                        test_usb_hcd_cmd_set_config();
                        test_case_state = TEST_HCD_CASE_STATE_SET_CONFIG;
                        osDelay(2); // for output log.
                        //test_usb_hcd_cmd_get_descriptor(DESCRIPTOR_TYPE_STRING);
                        TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_SET_CONFIG", __func__);

                        test_usb_set_itf_ep();
                        test_case_state = TEST_HCD_CASE_STATE_SET_ITF_EP;
                        TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_SET_ITF_EP", __func__);
                        osDelay(4);

                    }
                    else if(test_case_state >= TEST_HCD_CASE_STATE_CREATE_NOZEOR_ENDPOINT)
                    {
                        TEST_LOG("%s: to process done queue.", __func__);
                        test_usb_hcd_cmd_process_done_queue();
                    }
                    else
                    {
                        TEST_LOG("%s: %d", __func__,__LINE__);
                        TEST_LOG("%s:test_case_state = %d", __func__,test_case_state);
                    }
                }
                else if(g_interrupt_status == USB_HCD_INTSTS_DISCONNINT)
                {
                    test_usb_hcd_cmd_destroy_point();
                }
            }
            else if(msg_p->id == TEST_HCD_MESSAGE_PLUG)
            {
                test_usb_hcd_get_plug_status();
            }

            _msg_free(msg_p);
        }
        else
        {
            if( test_case_state == TEST_HCD_CASE_STATE_SET_ITF_EP)
            {
                TEST_LOG("%s: %d, usb_device.ep_number = %d", __func__,__LINE__,usb_device.ep_number);
                for(i = 0; i < usb_device.ep_number; i++)
                {
                    TEST_LOG("%s: %d", __func__,__LINE__);

                    osDelay(4);
                    test_usb_hcd_cmd_create_point(g_device,
                                    usb_device.ep[i].endpoint.endpoint_address,
                                    usb_device.ep[i].endpoint.attributes,
                                    usb_device.ep[i].endpoint.interval,
                                    usb_device.ep[i].endpoint.max_packet_size
                                    );

                }
                test_case_state = TEST_HCD_CASE_STATE_CREATE_NOZEOR_ENDPOINT;
                TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_CREATE_NOZEOR_ENDPOINT", __func__);
            }
            osDelay(1);
        }
    }
}


static void test_app_thread(void const *argument)
{
    int ret;
    uint32_t block_count = 0;
    uint32_t block_size = 0;

    while(1)
    {
        if(is_inited)
        {
            break;
        }
        osDelay(10);
    }

    while(1){
        if(test_case_state == TEST_HCD_CASE_STATE_CREATE_NOZEOR_ENDPOINT)
        {
            TEST_LOG("%s: test_case_state : to usb_mass_storage_init", __func__);
            ret = usb_mass_storage_init(g_device);
            if(ret)
            {
                TEST_LOG("%s: usb_mass_storage_init failed! ret = %d", __func__, ret);
            
}
            test_case_state = TEST_HCD_CASE_STATE_CREATE_SCSI_INIT;
            TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_CREATE_SCSI_INIT", __func__);
            osDelay(4);
        }
        else if(test_case_state == TEST_HCD_CASE_STATE_CREATE_SCSI_INIT)
        {
            TEST_LOG("%s: test_case_state : to usb_mass_storage_get_capacity", __func__);
            ret = usb_mass_storage_get_capacity(g_device, &block_count, &block_size);
            if(ret)
            {
                TEST_LOG("%s: usb_mass_storage_get_capacity failed! ret = %d", __func__, ret);
            
}
            else
            {
                TEST_LOG("%s: usb_mass_storage_get_capacity done. ret = %d", __func__, ret);
                TEST_LOG("%s: block_count = 0x%x, block_size = 0x%x", __func__, block_count,block_size);
            }
            test_case_state = TEST_HCD_CASE_STATE_CREATE_SCSI_GET_CAPACITY;
            TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_CREATE_SCSI_GET_CAPACITY", __func__);
        }
        else if(test_case_state == TEST_HCD_CASE_STATE_CREATE_SCSI_GET_CAPACITY)
        {
            TEST_LOG("%s: test_case_state : to usb_mass_storage_read", __func__);
            ret = usb_mass_storage_read(g_device, 0, 1,_read_buffer,sizeof(_read_buffer));
            if(ret)
            {
                TEST_LOG("%s: usb_mass_storage_read failed! ret = %d", __func__, ret);
            
}
            else
            {
                TEST_LOG("%s: usb_mass_storage_read done. ret = %d", __func__, ret);
                TEST_LOG("%s: read size = 0x%x ",__func__, sizeof(_read_buffer));
            }
            test_case_state = TEST_HCD_CASE_STATE_CREATE_SCSI_READ_BLOCK;
            TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_CREATE_SCSI_READ_BLOCK", __func__);
        }
        else if(test_case_state == TEST_HCD_CASE_STATE_CREATE_SCSI_READ_BLOCK)
        {
            TEST_LOG("%s: test_case_state : to usb_mass_storage_write", __func__);
            memcpy(_write_buffer,_read_buffer,sizeof(_write_buffer));
            ret = usb_mass_storage_write(g_device, 0, 1,_write_buffer,sizeof(_write_buffer));
            if(ret)
            {
                TEST_LOG("%s: usb_mass_storage_write failed! ret = %d", __func__, ret);
            
}
            else
            {
                TEST_LOG("%s: usb_mass_storage_write done. ret = %d", __func__, ret);
                TEST_LOG("%s: write size = 0x%x ",__func__, sizeof(_read_buffer));
            }
            test_case_state = TEST_HCD_CASE_STATE_CREATE_SCSI_WRITE_BLOCK;
            TEST_LOG("%s: test_case_state : TEST_HCD_CASE_STATE_CREATE_SCSI_WRITE_BLOCK", __func__);
        }
        else
        {
            //TEST_LOG("%s: test_case_state = %d", __func__,test_case_state);

        }
        osDelay(1);
    }
}

static int test_hcd_thread_init(void)
{
    osThreadId tid;

    test_hcd_msg_init();

    tid = osThreadCreate(osThread(test_hcd_thread), NULL);
    if (tid == NULL) {
        TEST_LOG("Failed to Create test_hcd_thread\n");
        return -1;
    }

    tid = osThreadCreate(osThread(test_app_thread), NULL);
    if (tid == NULL) {
        TEST_LOG("Failed to Create test_app_thread\n");
        return -2;
    }
    return 0;
}

static void test_hcd_interrupt_handle(void)
{
    bool ret;

    //TEST_LOG("%s", __func__);
    ret = _msg_add(TEST_HCD_MESSAGE_INTERRUPT,0);
    if(!ret)
    {
        TEST_LOG("%s: queue is full,this message is lost!", __func__);
    }
}

static void test_hcd_plug_handle(void)
{
    bool ret;

    // TEST_LOG("%s", __func__);
    ret = _msg_add(TEST_HCD_MESSAGE_PLUG,0);
    if(!ret)
    {
        TEST_LOG("%s: queue is full,this message is lost!", __func__);
    }
}


int test_usb_hcd_init(void)
{
    TEST_LOG("%s\n", __func__);
    g_plug_status = USB_HCD_PLUG_STATUS_UNKNOWN;
    g_interrupt_status = USB_HCD_INTSTS_UNKNOWN;
    g_port_status = USB_PORT_STATUS_UNKNOWN;
    g_frame_number = 0;
    is_create_ep0 = false;
    g_device = 0;
    test_case_state = TEST_HCD_CASE_STATE_IDLE;
    memset(&usb_device, 0, sizeof(usb_device));
    usb_device.device = 1;

    usb_hcd_init(&g_hcd,test_hcd_interrupt_handle,test_hcd_plug_handle);
    TEST_LOG("name = %s\n", g_hcd.name);
    TEST_LOG("address:");

    TEST_LOG("state = %d\n", g_hcd.state);
    TEST_LOG("type = %d\n", g_hcd.type);
    TEST_LOG("otg_capabilities = %d\n", g_hcd.otg_capabilities);
    TEST_LOG("root_hub_port_num = %d\n", g_hcd.root_hub_port_num);
    TEST_LOG("root_hub_port_signal:");

    TEST_LOG("task_signal = %d\n", g_hcd.task_signal);
    TEST_LOG("hcd_entry_func = 0x%x\n", (uint32_t)g_hcd.hcd_entry_func);
    TEST_LOG("root_hub_device_connection = %d\n", g_hcd.root_hub_device_connection);
    TEST_LOG("device_num = %d\n", g_hcd.device_num);
    TEST_LOG("io = %d\n", g_hcd.io);
    TEST_LOG("power_switch = %d\n", g_hcd.power_switch);
    TEST_LOG("available_bandwidth = 0x%x\n", g_hcd.available_bandwidth);
    TEST_LOG("maximum_transfer_request_size = 0x%x\n", g_hcd.maximum_transfer_request_size);
    TEST_LOG("version = 0x%x\n", g_hcd.version);
    TEST_LOG("controller_hardware = 0x%x\n", (uint32_t)g_hcd.controller_hardware);
    TEST_LOG("hcd_op = 0x%x\n", (uint32_t)g_hcd.hcd_op);
    is_inited = true;
    TEST_LOG("%s done", __func__);
    return 0;
}

int test_usb_register_interrupt_cb(void)
{
    TEST_LOG("%s\n", __func__);
    usb_register_interrupt_cb(test_hcd_interrupt_handle);
    TEST_LOG("set usb_hcd_interrupt_cb point to test_hcd_interrupt_handle.\n");
    TEST_LOG("%s done\n", __func__);
    return 0;
}

void test_usb_hcd_loop(void)
{

}

int test_usb_hcd_main(void)
{
    test_hcd_thread_init();
    test_usb_hcd_init();

    //test_usb_hcd_cmd_get_port_status();
    return 0;
}

void test_usb_hcd_get_plug_status(void)
{
    uint32_t status;

    status = (uint32_t)usb_hcd_get_plug_status();
    g_plug_status = status;
    TEST_LOG("usb_hcd_get_plug_status = 0x%x\n", status);
}


void test_usb_hcd_get_interrupt_status(void)
{
    uint32_t status;
    TEST_LOG("%s\n", __func__);
    status = (uint32_t)usb_hcd_get_interrupt_status();
    g_interrupt_status = status;
    TEST_LOG("usb_hcd_get_interrupt_status = 0x%x\n", status);
    if(status & USB_HCD_INTSTS_CURMOD)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_CURMOD\n", __func__);
    }

    if(status & USB_HCD_INTSTS_RXFLVL)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_RXFLVL\n", __func__);
    }

    if(status & USB_HCD_INTSTS_FETSUSP)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_FETSUSP\n", __func__);
    }

    if(status & USB_HCD_INTSTS_PRTINT)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_PRTINT\n", __func__);
    }

    if(status & USB_HCD_INTSTS_HCHINT)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_HCHINT\n", __func__);
    }

    if(status & USB_HCD_INTSTS_PTXFEMP)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_PTXFEMP\n", __func__);
    }

    if(status & USB_HCD_INTSTS_CONLDSTSCHNG)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_CONLDSTSCHNG\n", __func__);
    }

    if(status & USB_HCD_INTSTS_DISCONNINT)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_DISCONNINT\n", __func__);
    }

    if(status & USB_HCD_INTSTS_SESSREQINT)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_SESSREQINT\n", __func__);
    }

    if(status & USB_HCD_INTSTS_WKUPINT)
    {
        TEST_LOG("%s:: USB_HCD_INTSTS_WKUPINT\n", __func__);
    }

    TEST_LOG("%s done\n", __func__);
}

void test_usb_hcd_get_port_status(void)
{
    uint32_t status;

    status = (uint32_t)usb_hcd_get_port_status();
    g_port_status = status;

    TEST_LOG("%s:: port status = 0x%x", __func__, status);
    if(status & USB_PORT_STATUS_CONNECTION)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_CONNECTION", __func__);
    }

    if(status & USB_PORT_STATUS_ENABLE)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_ENABLE", __func__);
    }

    if(status & USB_PORT_STAT_SUSPEND)
    {
        TEST_LOG("%s:: USB_PORT_STAT_SUSPEND", __func__);
    }

    if(status & USB_PORT_STATUS_OVERCURRENT)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_OVERCURRENT", __func__);
    }

    if(status & USB_PORT_STATUS_RESET)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_RESET", __func__);
    }

    if(status & USB_PORT_STATUS_POWER)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_POWER", __func__);
    }
    if(status & USB_PORT_STATUS_FULL_SPEED)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_FULL_SPEED", __func__);
    }

    if(status & USB_PORT_STATUS_HIGH_SPEED)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_HIGH_SPEED", __func__);
    }

    TEST_LOG("%s:: done", __func__);
}

int test_usb_hcd_cmd_get_port_status(void)
{
    HCD_DEVICE_EP_T device_ep = {0,0};
    uint32_t status = 0;

    status = usb_hcd_cmd(&g_hcd,HCD_CMD_GET_PORT_STATUS,&device_ep);
    g_port_status = status;

    TEST_LOG("%s:: port status = 0x%x", __func__, status);
    if(status & USB_PORT_STATUS_CONNECTION)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_CONNECTION", __func__);
    }

    if(status & USB_PORT_STATUS_ENABLE)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_ENABLE", __func__);
    }

    if(status & USB_PORT_STAT_SUSPEND)
    {
        TEST_LOG("%s:: USB_PORT_STAT_SUSPEND", __func__);
    }

    if(status & USB_PORT_STATUS_OVERCURRENT)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_OVERCURRENT", __func__);
    }

    if(status & USB_PORT_STATUS_RESET)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_RESET", __func__);
    }

    if(status & USB_PORT_STATUS_POWER)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_POWER", __func__);
    }
    if(status & USB_PORT_STATUS_FULL_SPEED)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_FULL_SPEED", __func__);
    }

    if(status & USB_PORT_STATUS_HIGH_SPEED)
    {
        TEST_LOG("%s:: USB_PORT_STATUS_HIGH_SPEED", __func__);
    }

    TEST_LOG("%s:: done", __func__);
    return 0;
}

void test_usb_hcd_cmd_create_point(uint8_t device,
                                                 uint8_t port,
                                                 uint8_t attributes,
                                                 uint8_t interval,
                                                 uint8_t max_packet_size)
{
    int result;
    HCD_CREATE_ENDPOINT_T ep;

    TEST_LOG("%s(d: %d, p: %d, att: %d, inter: %d,mp:%d)",
        __func__, device, port, attributes, interval, max_packet_size);

    ep.device = device;
    ep.ep = port;
    ep.attributes = attributes;
    ep.interval = interval;
    ep.max_packet_size = max_packet_size;

    result = usb_hcd_cmd(&g_hcd,HCD_CMD_CREATE_ENDPOINT,&ep);
    if(result < 0)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_CREATE_ENDPOINT) failed! ret = %d",
            __func__,
            result);
        return;
    }
    if(port == 0)
    {
        g_device = result;
    }
    TEST_LOG("%s done", __func__);

}

void test_usb_hcd_cmd_destroy_point(void)
{
    int result;
    HCD_DEVICE_EP_T dev_ep;

    TEST_LOG("%s", __func__);

    dev_ep.device = 1;
    dev_ep.ep = 0;

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_DESTROY_ENDPOINT, &dev_ep);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_DESTROY_ENDPOINT) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

// HCD_CMD_ENABLE_PORT
void test_usb_hcd_cmd_enable_port(void)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_ENABLE_PORT, NULL);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_ENABLE_PORT) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

// HCD_CMD_DISABLE_PORT
void test_usb_hcd_cmd_disable_port(void)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_DISABLE_PORT, NULL);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_DISABLE_PORT) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

// HCD_CMD_POWER_ON_PORT
void test_usb_hcd_cmd_power_on_port(void)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_POWER_ON_PORT, NULL);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_POWER_ON_PORT) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

// HCD_CMD_POWER_OFF_PORT
void test_usb_hcd_cmd_power_off_port(void)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_POWER_OFF_PORT, NULL);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_POWER_OFF_PORT) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

// HCD_CMD_RESUME_PORT
void test_usb_hcd_cmd_resume_port(void)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_RESUME_PORT, NULL);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_RESUME_PORT) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

// HCD_CMD_SUSPEND_PORT
void test_usb_hcd_cmd_suspend_port(void)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_SUSPEND_PORT, NULL);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_SUSPEND_PORT) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

// HCD_CMD_RESET_PORT
void test_usb_hcd_cmd_reset_port(void)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_RESET_PORT, NULL);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_RESET_PORT) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

// HCD_CMD_GET_FRAME_NUMBER
void test_usb_hcd_cmd_get_frame_number_port(void)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_GET_FRAME_NUMBER, NULL);
    TEST_LOG("%s frame number = %d", __func__,result);
    g_frame_number = result;
    TEST_LOG("%s done", __func__);
}

// HCD_CMD_SET_FRAME_NUMBER
void test_usb_hcd_cmd_set_frame_number_port(uint32_t frame_number)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_SET_FRAME_NUMBER, &frame_number);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_SET_FRAME_NUMBER) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

// HCD_CMD_TRANSFER_REQUEST
void test_usb_hcd_cmd_transfer_req(void)
{
/*
typedef struct _HCD_TRANSFER
{
    uint8_t  device;
    uint8_t  ep;
    uint8_t  flags;
    uint8_t* data;
    uint32_t dat_len;
    USB_CALLBACK callback;
    void* user_data;
}HCD_TRANSFER;
*/

    int result;
    HCD_TRANSFER_T transfer;

    TEST_LOG("%s", __func__);

    transfer.device = 1;
    transfer.ep = 0;
    transfer.flags = USB_HCD_TOKEN_SETUP;
    transfer.data = NULL;
    transfer.data_len = 0;
    transfer.callback = NULL;
    transfer.user_data = NULL;
    result = usb_hcd_cmd(&g_hcd, HCD_CMD_TRANSFER_REQUEST, &transfer);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_TRANSFER_REQUEST) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}

void test_usb_hcd_cmd_set_addr(void)
{
    return;

#if 0
    int result;
    HCD_CONTROL_TRANSFER_T transfer;

    TEST_LOG("%s", __func__);
    transfer.device = 0;
    transfer.ep = 0;
    transfer.request_type = HOST_TO_DEVICE | RECIPIENT_DEVICE;
    transfer.request = SET_ADDRESS;
    transfer.value = TEST_HCD_USB_ADDR;
    transfer.index = 0;
    transfer.data = NULL;
    transfer.length = 0;
    transfer.actual_len = 0;
    transfer.callback = NULL;
    transfer.user_data = NULL;

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_CONTROL_TRANSFER_REQUEST, &transfer);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_CONTROL_TRANSFER_REQUEST) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
#endif
}

static void _get_device_descriptor(void)
{
    int result;
    HCD_CONTROL_TRANSFER_T transfer;
    USBHOST_DEVICE_DESCRIPTOR_T desc_buff;
    USBHOST_DEVICE_DESCRIPTOR_T *desc = &desc_buff;

    TEST_LOG("%s", __func__);

    transfer.device = 0;
    transfer.ep = 0;
    transfer.request_type = DEVICE_TO_HOST | RECIPIENT_DEVICE;
    transfer.request = GET_DESCRIPTOR;
    transfer.value= (DESCRIPTOR_TYPE_DEVICE << 8);   // (descriptor type << 8) | descriptor index
    transfer.index = 0;
    transfer.data = (uint8_t*)&desc_buff;
    transfer.length = 8;
    transfer.actual_len = 0;
    transfer.callback = NULL;
    transfer.user_data = NULL;
    memset(&desc_buff,0,sizeof(desc_buff));
    result = usb_hcd_cmd(&g_hcd, HCD_CMD_CONTROL_TRANSFER_REQUEST, &transfer);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_CONTROL_TRANSFER_REQUEST) failed! ret = %d",
            __func__,
            result);
        return;
    }
    TEST_LOG("transfer.actual_len = %d",transfer.actual_len);
    TEST_LOG("DEVICE :");
 #if 0
    TEST_LOG("length: 0x%x", desc->length);
    TEST_LOG("descriptor_type: 0x%x", desc->descriptor_type);
    TEST_LOG("cdusb: 0x%x", desc->cdusb);
    TEST_LOG("device_class: 0x%x", desc->device_class);
    TEST_LOG("device_sub_class: 0x%x", desc->device_sub_class);
    TEST_LOG("device_protocol: 0x%x", desc->device_protocol);
    TEST_LOG("max_packet_size: 0x%x", desc->max_packet_size);
    TEST_LOG("vendor: 0x%x", desc->vendor);
    TEST_LOG("dproduct: 0x%x", desc->dproduct);
    TEST_LOG("cddevice: 0x%x", desc->cddevice);
    TEST_LOG("manu_facturer:  0x%x", desc->manu_facturer);
    TEST_LOG("product: 0x%x", desc->product);
    TEST_LOG("serial_number: 0x%x", desc->serial_number);
    TEST_LOG("num_configurations: 0x%x", desc->num_configurations);
#else
    TEST_LOG("l:0x%x,d_t:0x%x,c:0x%x,d_c:0x%x,dsc:0x%x,d_p:0x%x,mps:0x%x,v:0x%x",
            desc->length,
            desc->descriptor_type,
            desc->cdusb,
            desc->device_class,
            desc->device_sub_class,
            desc->device_protocol,
            desc->max_packet_size,
            desc->vendor
            );
    TEST_LOG("dp:0x%x,cd:0x%x,m_f:0x%x,p:0x%x,s_n:0x%x,n_c:0x%x",
            desc->dproduct,
            desc->cddevice,
            desc->manu_facturer,
            desc->product,
            desc->serial_number,
            desc->num_configurations
            );
#endif
    transfer.device = 0;
    transfer.ep = 0;
    transfer.request_type = DEVICE_TO_HOST | RECIPIENT_DEVICE;
    transfer.request = GET_DESCRIPTOR;
    transfer.value= (DESCRIPTOR_TYPE_DEVICE << 8);   // (descriptor type << 8) | descriptor index
    transfer.index = 0;
    transfer.data = (uint8_t*)&desc_buff;
    transfer.length = sizeof(desc_buff);
    transfer.actual_len = 0;
    transfer.callback = NULL;
    transfer.user_data = NULL;
    memset(&desc_buff,0,sizeof(desc_buff));
    result = usb_hcd_cmd(&g_hcd, HCD_CMD_CONTROL_TRANSFER_REQUEST, &transfer);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_CONTROL_TRANSFER_REQUEST) failed! ret = %d",
            __func__,
            result);
        return;
    }
    TEST_LOG("transfer.actual_len = %d",transfer.actual_len);
    TEST_LOG("DEVICE :");
 #if 0
    TEST_LOG("length: 0x%x", desc->length);
    TEST_LOG("descriptor_type: 0x%x", desc->descriptor_type);
    TEST_LOG("cdusb: 0x%x", desc->cdusb);
    TEST_LOG("device_class: 0x%x", desc->device_class);
    TEST_LOG("device_sub_class: 0x%x", desc->device_sub_class);
    TEST_LOG("device_protocol: 0x%x", desc->device_protocol);
    TEST_LOG("max_packet_size: 0x%x", desc->max_packet_size);
    TEST_LOG("vendor: 0x%x", desc->vendor);
    TEST_LOG("dproduct: 0x%x", desc->dproduct);
    TEST_LOG("cddevice: 0x%x", desc->cddevice);
    TEST_LOG("manu_facturer:  0x%x", desc->manu_facturer);
    TEST_LOG("product: 0x%x", desc->product);
    TEST_LOG("serial_number: 0x%x", desc->serial_number);
    TEST_LOG("num_configurations: 0x%x", desc->num_configurations);
#else
    TEST_LOG("l:0x%x,d_t:0x%x,c:0x%x,d_c:0x%x,dsc:0x%x,d_p:0x%x,mps:0x%x,v:0x%x",
            desc->length,
            desc->descriptor_type,
            desc->cdusb,
            desc->device_class,
            desc->device_sub_class,
            desc->device_protocol,
            desc->max_packet_size,
            desc->vendor
            );
    TEST_LOG("dp:0x%x,cd:0x%x,m_f:0x%x,p:0x%x,s_n:0x%x,n_c:0x%x",
            desc->dproduct,
            desc->cddevice,
            desc->manu_facturer,
            desc->product,
            desc->serial_number,
            desc->num_configurations
            );
#endif
    TEST_LOG("%s done", __func__);
}

static uint8_t descriptor_buff[1024];

static void _get_config_descriptor(void)
{
    uint32_t i,j;
    int result;
    uint32_t offs = 0;
    HCD_CONTROL_TRANSFER_T transfer;
    USBHOST_CONFIGURATION_DICRIPTOR_T *config_desc;
    USBHOST_INTERFACE_DESCRIPTOR_T *intface_desc;
    USBHOST_ENDPOINT_DESCRIPTOR_T *end_desc;

    TEST_LOG("%s", __func__);
    transfer.device = 0;
    transfer.ep = 0;
    transfer.request_type = DEVICE_TO_HOST | RECIPIENT_DEVICE;
    transfer.request = GET_DESCRIPTOR;
    transfer.value= (DESCRIPTOR_TYPE_CONFIG << 8);   // (descriptor type << 8) | descriptor index
    transfer.index = 0;
    transfer.data = (uint8_t*)&descriptor_buff;
    transfer.length = sizeof(descriptor_buff);
    transfer.actual_len = 0;
    transfer.callback = NULL;
    transfer.user_data = NULL;

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_CONTROL_TRANSFER_REQUEST, &transfer);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_CONTROL_TRANSFER_REQUEST) failed! ret = %d",
            __func__,
            result);
        return;
    }
    osDelay(2);
    // print CONFIG Descript
    config_desc = (USBHOST_CONFIGURATION_DICRIPTOR_T*)(descriptor_buff + offs);
    TEST_LOG("CONFIG :");
#if 0
    TEST_LOG("length: 0x%x", config_desc->length);
    TEST_LOG("descriptor_type: 0x%x", config_desc->descriptor_type);
    TEST_LOG("total_length: 0x%x", config_desc->total_length);
    TEST_LOG("num_interface: 0x%x", config_desc->num_interface);
    TEST_LOG("configuration_value: 0x%x", config_desc->configuration_value);
    TEST_LOG("configuration: 0x%x", config_desc->configuration);
    TEST_LOG("attributes: 0x%x", config_desc->attributes);
    TEST_LOG("max_power: 0x%x", config_desc->max_power);
#else
    TEST_LOG("l:0x%x,d_t:0x%x,t_l:0x%x,n_i:0x%x,c_v:0x%x,c:0x%x,a:0x%x,m_p:0x%x",
            config_desc->length,
            config_desc->descriptor_type,
            config_desc->total_length,
            config_desc->num_interface,
            config_desc->configuration_value,
            config_desc->configuration,
            config_desc->attributes,
            config_desc->max_power
            );
#endif
    offs += config_desc->length;
    osDelay(2);
    // print INTERFACE Descript
    for(i = 0; i < config_desc->num_interface; i++)
    {
        intface_desc = (USBHOST_INTERFACE_DESCRIPTOR_T*)(descriptor_buff + offs);
        TEST_LOG("INTERFACE :");
#if 0
        TEST_LOG("length: 0x%x", intface_desc->length);
        TEST_LOG("descriptor_type: 0x%x", intface_desc->descriptor_type);
        TEST_LOG("interface_number: 0x%x", intface_desc->interface_number);
        TEST_LOG("alternate_setting: 0x%x", intface_desc->alternate_setting);
        TEST_LOG("num_endpoints: 0x%x", intface_desc->num_endpoints);
        TEST_LOG("interface_class: 0x%x", intface_desc->interface_class);
        TEST_LOG("interface_sub_class: 0x%x", intface_desc->interface_sub_class);
        TEST_LOG("interface_protocol: 0x%x", intface_desc->interface_protocol);
        TEST_LOG("interface: 0x%x", intface_desc->interface);
#else
        TEST_LOG("l:0x%x,d_t:0x%x,i_n:0x%x,a_s:0x%x,n_e:0x%x,i_c:0x%x,i_p:0x%x,i:0x%x",
            intface_desc->length,
            intface_desc->descriptor_type,
            intface_desc->interface_number,
            intface_desc->alternate_setting,
            intface_desc->num_endpoints,
            intface_desc->interface_class,
            intface_desc->interface_protocol,
            intface_desc->interface
            );
#endif
        offs += intface_desc->length;
        for(j = 0; j < intface_desc->num_endpoints; j++)
        {
            end_desc = (USBHOST_ENDPOINT_DESCRIPTOR_T*)(descriptor_buff + offs);
            TEST_LOG("ENDPOINT :");
#if 0
            TEST_LOG("length: 0x%x", end_desc->length);
            TEST_LOG("descriptor_type: 0x%x", end_desc->descriptor_type);
            TEST_LOG("endpoint_address: 0x%x", end_desc->endpoint_address);
            TEST_LOG("attributes: 0x%x", end_desc->attributes);
            TEST_LOG("max_packet_size: 0x%x", end_desc->max_packet_size);
            TEST_LOG("interval: 0x%x", end_desc->interval);
            if(end_desc->length == 9)
            {
                TEST_LOG("refresh: 0x%x", end_desc->refresh);
                TEST_LOG("synch_address: 0x%x", end_desc->synch_address);
            }
#else
            TEST_LOG("l:0x%x,d_t:0x%x,e_a:0x%x,a:0x%x,mpz:0x%x,i:0x%x",
            end_desc->length,
            end_desc->descriptor_type,
            end_desc->endpoint_address,
            end_desc->attributes,
            end_desc->max_packet_size,
            end_desc->interval
            );
#endif
            offs += end_desc->length;
        }
        osDelay(2);
    }

    TEST_LOG("%s done", __func__);

}

static void _get_string_descriptor(void)
{
    int result;
    uint32_t desc_count = 0;
    HCD_CONTROL_TRANSFER_T transfer;
    USBHOST_STRING_DESCRIPTION_T *desc = (USBHOST_STRING_DESCRIPTION_T*)descriptor_buff;

    TEST_LOG("%s", __func__);

    do{
        transfer.device = 0;
        transfer.ep = 0;
        transfer.request_type = DEVICE_TO_HOST | RECIPIENT_DEVICE;
        transfer.request = GET_DESCRIPTOR;
        transfer.value= (DESCRIPTOR_TYPE_STRING << 8);   // (descriptor type << 8) | descriptor index
        transfer.index = desc_count;
        transfer.data = (uint8_t*)descriptor_buff;
        transfer.length = sizeof(descriptor_buff);
        transfer.actual_len = 0;
        transfer.callback = NULL;
        transfer.user_data = NULL;

        result = usb_hcd_cmd(&g_hcd, HCD_CMD_CONTROL_TRANSFER_REQUEST, &transfer);
        if(result || desc_count >= 4)
        {
            TEST_LOG("%s: STRING Descript number = %d",
                __func__,
                desc_count);
            break;
        }

        TEST_LOG("STRING[%d] :", desc_count);
        TEST_LOG("length: 0x%x", desc->length);
        TEST_LOG("descriptor_type: 0x%x", desc->descriptor_type);
        desc_count ++;
    }while(result == 0);
    TEST_LOG("%s done", __func__);
}

void test_usb_hcd_cmd_get_descriptor(int descriptor_type)
{
    if(descriptor_type == DESCRIPTOR_TYPE_DEVICE)
    {
        _get_device_descriptor();
    }
    else if(descriptor_type == DESCRIPTOR_TYPE_CONFIG)
    {
        _get_config_descriptor();
    }
    else if(descriptor_type == DESCRIPTOR_TYPE_STRING)
    {
        _get_string_descriptor();
    }
}

//usbhost_control_transfer(device, 0 , HOST_TO_DEVICE | RECIPIENT_DEVICE, SET_CONFIGURATION, configNum, 0, 0, 0, NULL, NULL);
void test_usb_hcd_cmd_set_config(void)
{
    return;
#if 0
    int result;
    HCD_CONTROL_TRANSFER_T transfer;

    TEST_LOG("%s", __func__);
    transfer.device = 0;
    transfer.ep = 0;
    transfer.request_type = HOST_TO_DEVICE | RECIPIENT_DEVICE;
    transfer.request = SET_CONFIGURATION;
    transfer.value = 0;
    transfer.index = 0;
    transfer.data = NULL;
    transfer.length = 0;
    transfer.actual_len = 0;
    transfer.callback = NULL;
    transfer.user_data = NULL;

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_CONTROL_TRANSFER_REQUEST, &transfer);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_CONTROL_TRANSFER_REQUEST) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
#endif
}

void test_usb_set_itf_ep(void)
{
    uint32_t i,j;
    int result;
    uint32_t offs = 0;
    HCD_CONTROL_TRANSFER_T transfer;
    USBHOST_CONFIGURATION_DICRIPTOR_T *config_desc;
    USBHOST_INTERFACE_DESCRIPTOR_T *intface_desc;
    USBHOST_ENDPOINT_DESCRIPTOR_T *end_desc;

    TEST_LOG("%s", __func__);
    transfer.device = g_device;
    transfer.ep = 0;
    transfer.request_type = DEVICE_TO_HOST | RECIPIENT_DEVICE;
    transfer.request = GET_DESCRIPTOR;
    transfer.value= (DESCRIPTOR_TYPE_CONFIG << 8);   // (descriptor type << 8) | descriptor index
    transfer.index = 0;
    transfer.data = (uint8_t*)&descriptor_buff;
    transfer.length = sizeof(descriptor_buff);
    transfer.callback = NULL;
    transfer.user_data = NULL;

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_CONTROL_TRANSFER_REQUEST, &transfer);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_CONTROL_TRANSFER_REQUEST) failed! ret = %d",
            __func__,
            result);
        return;
    }
    osDelay(2);
    // print CONFIG Descript
    config_desc = (USBHOST_CONFIGURATION_DICRIPTOR_T*)(descriptor_buff + offs);
    TEST_LOG("CONFIG :");
#if 1
    TEST_LOG("length: 0x%x", config_desc->length);
    TEST_LOG("descriptor_type: 0x%x", config_desc->descriptor_type);
    TEST_LOG("total_length: 0x%x", config_desc->total_length);
    TEST_LOG("num_interface: 0x%x", config_desc->num_interface);
    TEST_LOG("configuration_value: 0x%x", config_desc->configuration_value);
    TEST_LOG("configuration: 0x%x", config_desc->configuration);
    TEST_LOG("attributes: 0x%x", config_desc->attributes);
    TEST_LOG("max_power: 0x%x", config_desc->max_power);
#else
    TEST_LOG("l:0x%x,d_t:0x%x,t_l:0x%x,n_i:0x%x,c_v:0x%x,c:0x%x,a:0x%x,m_p:0x%x",
            config_desc->length,
            config_desc->descriptor_type,
            config_desc->total_length,
            config_desc->num_interface,
            config_desc->configuration_value,
            config_desc->configuration,
            config_desc->attributes,
            config_desc->max_power
            );
#endif
    offs += config_desc->length;
    osDelay(2);
    // print INTERFACE Descript
    for(i = 0; i < config_desc->num_interface; i++)
    {
        intface_desc = (USBHOST_INTERFACE_DESCRIPTOR_T*)(descriptor_buff + offs);
        TEST_LOG("INTERFACE :");
#if 1
        TEST_LOG("length: 0x%x", intface_desc->length);
        TEST_LOG("descriptor_type: 0x%x", intface_desc->descriptor_type);
        TEST_LOG("interface_number: 0x%x", intface_desc->interface_number);
        TEST_LOG("alternate_setting: 0x%x", intface_desc->alternate_setting);
        TEST_LOG("num_endpoints: 0x%x", intface_desc->num_endpoints);
        TEST_LOG("interface_class: 0x%x", intface_desc->interface_class);
        TEST_LOG("interface_sub_class: 0x%x", intface_desc->interface_sub_class);
        TEST_LOG("interface_protocol: 0x%x", intface_desc->interface_protocol);
        TEST_LOG("interface: 0x%x", intface_desc->interface);
#else
        TEST_LOG("l:0x%x,d_t:0x%x,i_n:0x%x,a_s:0x%x,n_e:0x%x,i_c:0x%x,i_p:0x%x,i:0x%x",
            intface_desc->length,
            intface_desc->descriptor_type,
            intface_desc->interface_number,
            intface_desc->alternate_setting,
            intface_desc->num_endpoints,
            intface_desc->interface_class,
            intface_desc->interface_protocol,
            intface_desc->interface
            );
#endif
        usb_device.ep_number = 0;
        usb_mass_storage_set_itf_num(intface_desc->interface_number);
        offs += intface_desc->length;
        for(j = 0; j < intface_desc->num_endpoints; j++)
        {
            end_desc = (USBHOST_ENDPOINT_DESCRIPTOR_T*)(descriptor_buff + offs);
            TEST_LOG("ENDPOINT :");
#if 1
            TEST_LOG("length: 0x%x", end_desc->length);
            TEST_LOG("descriptor_type: 0x%x", end_desc->descriptor_type);
            TEST_LOG("endpoint_address: 0x%x", end_desc->endpoint_address);
            TEST_LOG("attributes: 0x%x", end_desc->attributes);
            TEST_LOG("max_packet_size: 0x%x", end_desc->max_packet_size);
            TEST_LOG("interval: 0x%x", end_desc->interval);
            if(end_desc->length == 9)
            {
                TEST_LOG("refresh: 0x%x", end_desc->refresh);
                TEST_LOG("synch_address: 0x%x", end_desc->synch_address);
            }
#else
            TEST_LOG("l:0x%x,d_t:0x%x,e_a:0x%x,a:0x%x,mpz:0x%x,i:0x%x",
            end_desc->length,
            end_desc->descriptor_type,
            end_desc->endpoint_address,
            end_desc->attributes,
            end_desc->max_packet_size,
            end_desc->interval
            );
#endif
            usb_device.ep[j].is_in = (end_desc->endpoint_address & 0x80) ? true : false;
            usb_device.ep[j].is_used = true;
            usb_device.ep[j].endpoint = *end_desc;
            usb_device.ep_number ++;
            if(usb_device.ep[j].is_in)
            {
                usb_mass_storage_set_read_ep(end_desc->endpoint_address);
            }
            else
            {
                usb_mass_storage_set_write_ep(end_desc->endpoint_address);
            }
            offs += end_desc->length;
        }
        osDelay(2);
    }

    TEST_LOG("%s done", __func__);

}


/*
static void test_usb_hcd_transfer_handle(int32_t device,
                                int32_t endpoint,
                                int32_t status,
                                uint8_t* data,
                                int32_t len,
                                void* user_data);
*/
static int _usb_transfer_req(int device, int ep, uint8_t flags, uint8_t* data, int length, HCD_TRANSFER_CALLBACK callback, void* userdata)
{
    int ret;
    HCD_TRANSFER_T transfer;

    TEST_LOG("%s", __func__);
    TEST_LOG("d: %d,ep:%d,f:%d,d:0x%x,d_l:%d,cb:0x%x,u_d:0x%x", device,ep,flags,(uint32_t)data,length,(uint32_t)callback,(uint32_t)userdata);
    osDelay(10);
    transfer.device = device;
    transfer.ep = ep;
    transfer.flags = flags;
    transfer.data = data;
    transfer.data_len = length;
    transfer.callback = callback;
    transfer.user_data = userdata;
    ret = usb_hcd_cmd(&g_hcd, HCD_CMD_TRANSFER_REQUEST, &transfer);
    if(ret < 0)
    {
        if(ret == ERR_IO_PENDING)
        {
            TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_TRANSFER_REQUEST) io pending. ret = %d",
                __func__,
                ret);
            return ret;
        }
        else
        {
            TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_TRANSFER_REQUEST) failed! ret = %d",
                __func__,
                ret);
        }
    }
    TEST_LOG("%s done", __func__);
    return ret;
}

int  test_usb_bulk_tracefer_req(int device, int ep, uint8_t* data, int length, HCD_TRANSFER_CALLBACK callback, void* userdata)
{
	return _usb_transfer_req(device,ep,(ep & 0x80) | USB_TRANSFER_TYPE_BULK,data,length,callback,userdata);
}

// HCD_CMD_PROCESS_DONE_QUEUE
void test_usb_hcd_cmd_process_done_queue(void)
{
    int result;

    TEST_LOG("%s", __func__);

    result = usb_hcd_cmd(&g_hcd, HCD_CMD_PROCESS_DONE_QUEUE, NULL);
    if(result)
    {
        TEST_LOG("%s: usb_hcd_cmd(HCD_CMD_PROCESS_DONE_QUEUE) failed! ret = %d",
            __func__,
            result);
        return;
    }

    TEST_LOG("%s done", __func__);
}



