/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <drivers/bug.h>
#include <drivers/u_ld.h>
#include <drivers/char/u_device.h>
#include <drivers/char/u_cdev_msg.h>
#include <devicevfs/devicevfs_rpc.h>

#ifdef USER_SPACE_DRIVER
#include <rpc.h>

#define UDRIVER_RPC_BUFFER_SIZE (1024)
#define UDRIVER_RPC_NAME_FORMAT "rpc-%d-main"

static char g_main_rpc_name[16] = {0};
static int g_udriver_rpc_handle = 0;
static char g_main_rpc_buf[UDRIVER_RPC_BUFFER_SIZE] = {0};
static int _cdev_fops_handler(int crpc_handle, u_cdev_msg_t *p_msg);
static int _loop_msg_handler(int crpc_handle, u_cdev_msg_t *p_msg);

static u_cdev_msg_handler _cdev_msg_handler[CDEV_MSG_MAX] = {
    [CDEV_MSG_FOPS] = _cdev_fops_handler,
    [CDEV_MSG_LOOP] = _loop_msg_handler,
};


int _loop_msg_handler(int crpc_handle, u_cdev_msg_t *p_msg) {
    ddkc_info("%s\r\n", __func__);
    return 0;
}

int _cdev_fops_handler(int reply_handle, u_cdev_msg_t *p_msg) {
    int ret = 0;
    u_cdev_msg_ie_t *ie = NULL;
    u_fops_arg_u *parg = NULL;
    if (!p_msg) {
        return -EINVAL;
    }
    ie = p_msg->v;

    ddkc_dbg("sizeof(u_cdev_msg_t):%d， sizeof(u_cdev_msg_id_e):%d, sizeof(unsigned short):%d, sizeof(ie->t):%d, ie->t:%d\r\n",
           sizeof(u_cdev_msg_t), sizeof(u_cdev_msg_id_e), sizeof(unsigned short),
           sizeof(ie->t), ie->t);

    switch(ie->t) {
        case FOPS_OPEN:
            parg = (u_fops_arg_u *)ie->v;
            ret = u_device_rpc_open(parg, reply_handle);
            ddkc_dbg("open %s return %d\r\n", parg->open.path, ret);
            break;
        case FOPS_CLOSE:
            parg = (u_fops_arg_u *)ie->v;
            ret = u_device_rpc_close(parg, reply_handle);
            ddkc_dbg("close return %d\r\n", ret);
            break;
        default:
            ddkc_err("invalid file operation:%d, ignore\r\n", ie->t);
            ret = -EINVAL;
            break;
    }

    return ret;
}

void main_thread_req_arrived(struct aos_parcel_t *pkt) {
    int ret = 0;
    int reply_handle = 0;
    u_cdev_msg_t *p_msg = NULL;
    size_t len = sizeof(int);

    if (!pkt || !pkt->max_size) {
        ddkc_err("invalid pkt:%p or max_size:%d\r\n", pkt, pkt ? pkt->max_size : 0);
        return;
    }

    ddkc_dbg("pkt->max_size:%x\r\n", pkt->max_size);
    aos_parcel_get(AP_UVARINT, &reply_handle, &len, pkt);
    ddkc_dbg("reply_handle:%x\r\n", reply_handle);

    BUG_ON_MSG((pkt->max_size > sizeof(g_main_rpc_buf)),
               "rpc max_size：%d > main thread rpc buf len:%d",
               pkt->max_size, sizeof(g_main_rpc_buf));

    len = pkt->max_size;
    ddkc_dbg("max_size:%x\r\n", len);
    ret = aos_parcel_get(AP_BUF, g_main_rpc_buf, &len, pkt);
    if (ret < 0 || len <= 0) {
        ddkc_err("rpc receive data failed, ret:%d\r\n", ret);
        // TODO: how to handle this case?
        return;
    }
    p_msg = (u_cdev_msg_t *)g_main_rpc_buf;
    BUG_ON_MSG((p_msg->t >= CDEV_MSG_MAX),
                "p_msg->t:%d > CDEV_MSG_MAX:%d\r\n", p_msg->t, CDEV_MSG_MAX);
#if 0
    for (int i = 0;i < len; i++)
        ddkc_err("i:%d - 0x%x\n", i, g_main_rpc_buf[i]);
#endif
    ret = (_cdev_msg_handler[p_msg->t](reply_handle, p_msg));
    if (ret) {
        ddkc_err("rpc msg handle failed, ret:%d\r\n", ret);
    }

    return;
}


void main_thread_event_cb(int eid, void * event) {
    struct aos_revt_param_t *param = (struct aos_revt_param_t *)event;
    int reason = *(int *)param->param;

    if (eid != RPC_EID_STOP) {
        ddkc_warn("eid:%d, reason:%d, closing rpc service - handle:0x%x\r\n", eid, reason, param->srpc_handle);
        aos_rpc_close(param->srpc_handle);
    } else
        ddkc_info("eid:%d, reason:%d\r\n", eid, reason);

    return;
}
#endif

/**
 * this is the only driver entry API, when a driver is selected, must call this API in proper place
 *
 * @string process identification
 *
 * @return 0 for success; negative error number for failure
 */
int u_driver_entry(char* string) {
    int ret = -1;

#ifdef USER_SPACE_DRIVER
    ddkc_dbg("%s uspace mode:%d\r\n", __func__, 1);

    /* register main thread service into PM */
    ret = snprintf(g_main_rpc_name, sizeof(g_main_rpc_name), UDRIVER_RPC_NAME_FORMAT, getpid());
    if (ret <= 0) {
        ddkc_err("snprintf for main rpc name failed\r\n");
        return ret;
    }

    ret = aos_rpc_regist(g_main_rpc_name, UDRIVER_RPC_BUFFER_SIZE, &g_udriver_rpc_handle);
    if (ret) {
        ddkc_err("register main rpc service[%s] failed, ret:%d\r\n", g_main_rpc_name, ret);
        return ret;
    }

    ddkc_dbg("register main rpc main service[%s] succeed, handle:0x%x\r\n",
              g_main_rpc_name, g_udriver_rpc_handle);

    ret = aos_rpc_run(g_udriver_rpc_handle, main_thread_req_arrived, main_thread_event_cb);
    if (ret) {
        ddkc_err("aos_rpc_run on service:%s fails, ret:%d\r\n", g_main_rpc_name, ret);
        goto error;
    }
#else
    ddkc_info("%s uspace mode:%d\r\n", __func__, 0);
#endif

#if (AOS_COMP_IRQ > 0)
    extern int aos_irq_system_init(void);
    ret = aos_irq_system_init();
    if (ret) {
        ddkc_err("irq system init failed, ret:%d\r\n", ret);
        goto error;
    }
#endif

    ret = _os_driver_entry();
    if (ret) {
        ddkc_err("_os_driver_entry error, ret:%d\r\n", ret);
        goto error;
    }

    return 0;

error:
#ifdef USER_SPACE_DRIVER
    if (g_udriver_rpc_handle) {
        aos_rpc_close(g_udriver_rpc_handle);
        g_udriver_rpc_handle = 0;
    }
#if 0
    u_irq_system_deinit();
#endif

#endif
    return ret;
}

/**
 * this is the only driver entry API, when a driver is selected, must call this API in proper place
 *
 * @string process identification
 *
 * @return 0 for success; negative error number for failure
 */
int u_post_driver_entry(char* string) {
    int ret = -1;
    ret = _os_post_driver_entry();

    return ret;
}

/**
 *
 * @return main thread's RPC service name
 */
char *u_driver_get_main_service_name(void) {
#ifdef USER_SPACE_DRIVER
    return g_main_rpc_name;
#else
    return NULL;
#endif
}
