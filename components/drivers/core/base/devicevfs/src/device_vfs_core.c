/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifdef USER_SPACE_DRIVER
#include <rpc.h>
#endif

#include "aos/vfs.h"
#include "aos/list.h"

#include <devicevfs/devicevfs.h>
#include <drivers/char/u_cdev_msg.h>
#include <drivers/u_mode.h>

#define MAJOR_OFFSET 120
#define NODE_PREFIX "/dev/"

static dlist_t g_fnode_valid_list;
static dlist_t g_subsys_dev_list;
typedef struct file_node {
    file_t f; // created when OPEN RPC request arrives
    void *dev_n;
    dlist_t node;
    dlist_t valid;
} file_node_t;

typedef struct subsys_dev_node {
    dlist_t n;
    struct subsys_dev dev;
    struct u_platform_device *pdev;
    struct u_platform_driver *pdrv;
    subsys_file_ops_t *fops;
    dlist_t file_head;
    int rpc_handle;
    int ref_cnt;
    bool rpc_run;
    char *rpc_name;
    char *path_name;
    /* real_open was used only when delay_init, which is member of struct subsys_dev, is set to true */
    int (*real_open)(inode_t *node, file_t *fp);
    int (*real_init)(struct u_platform_device *pdev);
    int (*real_probe)(struct u_platform_device *pdev);

} subsys_dev_node_t;

#ifdef USER_SPACE_DRIVER

#define UDRIVER_RCP_NAME_MAX_LEN (AOS_RPC_NAME_MAX_SIZE)
#define UDRIVER_RPC_NAME_FORMAT "rpc-%d-%s"

extern char *u_driver_get_main_service_name(void);



static int _cdev_fops_handler(int crpc_handle, u_cdev_msg_t *p_msg);
static int _loop_msg_handler(int crpc_handle, u_cdev_msg_t *p_msg);

static u_cdev_msg_handler _cdev_msg_handler[CDEV_MSG_MAX] = {
    [CDEV_MSG_FOPS] = _cdev_fops_handler,
    [CDEV_MSG_LOOP] = _loop_msg_handler,
};


/**
 * check whether fnode is valid or not
 * to prevent client send invalid RPC request with cached data
 *
 * @param fnode - pointer to target file_node_t
 * @return true if it is valid; false if it is invalid
 */
static bool _is_valid_fnode(file_node_t *fnode) {
    file_node_t *n = NULL;
    dlist_t *pos = NULL;
    dlist_t *tmp = NULL;

    dlist_for_each_safe(pos, tmp, &g_fnode_valid_list)
    {
        n = aos_container_of(pos, file_node_t, valid);
        if (n == fnode)
            return true;
    }
    return false;
}

/**
 * char device file operation handler
 *
 * @param crpc_handle - RPC client's handle
 * @param p_msg - pointer to operation struct
 *
 * @return 0 for success; negative for failure
 */
int _cdev_fops_handler(int crpc_handle, u_cdev_msg_t *p_msg) {
    int ret = 0;
    u_cdev_msg_ie_t *ie = NULL;
    struct subsys_dev_node *dev_n = NULL;
    subsys_file_ops_t *fops = NULL;
    u_fops_arg_u *fops_arg = NULL;
    file_t *fp = NULL;
    file_node_t *fnode = NULL;
    inode_t *node = NULL;
    u_fops_result_u res;

    if (!p_msg) {
        return -EINVAL;
    }
    ie = p_msg->v;

    fops_arg = (u_fops_arg_u *)ie->v;

    /* fnode is put into open.farg in u_device_rpc_open */
    fnode = (file_node_t *)fops_arg->priv.farg;
    ddkc_dbg("farg:%p\r\n", fnode);

    if (!fnode) {
        ret = -EINVAL;
        ddkc_err("fops_arg->priv.arg is NULL, ignore\r\n");
        goto err;
    }

    // do fnode's validation
    if (!_is_valid_fnode(fnode)) {
        // this case should NEVER happen, so omit this check
        BUG_ON_MSG(1, "invalid fnode:%p from rpc client:0x%x\r\n", fnode, crpc_handle);
        return  -EINVAL;
    }

    dev_n = fnode->dev_n;

    if (ie->t <= FOPS_OPEN || ie->t >= FOPS_MAX) {
        ret = -EINVAL;
        ddkc_err("invalid file operation:%d, ignore, ret:%d\r\n", ie->t, ret);
        goto err;
    }

    memset(&res, 0, sizeof(res));

    fp = &fnode->f;
    node = fp->node;
    fops = (subsys_file_ops_t *)node->ops.i_ops;

    switch(ie->t) {
        case FOPS_READ:
        {
            ssize_t r_len = 0;
            u_fops_result_u *p_res = malloc(sizeof(*p_res) + fops_arg->read.len);

            if (!p_res) {
                ddkc_err("%s - malloc for u_fops_result_u failed\r\n", dev_n->rpc_name);
                ret = -ENOMEM;
                goto err;
            }

            memset(p_res, 0, sizeof(*p_res) + fops_arg->read.len);

            /* call device's read cb */
            r_len = fops->read(fp, p_res->read.data, fops_arg->read.len);
            if (r_len < 0) {
                ret = r_len;
                ddkc_warn("fops->read ret:%d\r\n", r_len);
                free(p_res);
                goto err;
            }
            p_res->read.status = 0;
            p_res->read.len = r_len;

            ddkc_dbg("FOPS_READ, status:%d, len:%d, total len:%d, sizeof(u_fops_result_u):%d\r\n",
                      p_res->priv.status, p_res->read.len, sizeof(*p_res) + r_len, sizeof(u_fops_result_u));
#if 0
            for (int i = 0; i < sizeof(*p_res) + r_len; i++) {
                ddkc_info("%d - 0x%x\r\n", i, *((char *)p_res  + i) & 0xff);
            }
#endif
            ret = aos_rpc_resp(crpc_handle, 1, sizeof(*p_res) + r_len, p_res);
            if (ret)
                ddkc_err("%s - aos_rpc_resp to 0x%x failed, ret:0x%x\r\n", dev_n->rpc_name, crpc_handle, ret);
            free(p_res);
            return ret;
        }
        break;

        case FOPS_WRITE:
        {
            ssize_t w_len = 0;

            ddkc_dbg("%s - fops->write++\r\n", __func__);

            /* call device's write cb */
            w_len = fops->write(fp, fops_arg->write.buf, fops_arg->write.len);
            if (w_len < 0) {
                ret = w_len;
                ddkc_warn("%s - fops->write ret:%d\r\n", dev_n->rpc_name, w_len);
                goto err;
            }
            res.write.status = 0;
            res.write.len = w_len;

            ddkc_dbg("FOPS_WRITE, status:%d, len:%d\r\n", res.write.status, res.write.len);
        }
        break;

        case FOPS_IOCTL:
            /* call device's ioctl cb */
            ret = fops->ioctl(fp, fops_arg->ioctl.cmd, fops_arg->ioctl.arg);
            if (ret < 0) {
                ddkc_warn("%s - fops->ioctl ret:%d\r\n", dev_n->rpc_name, ret);
                goto err;
            }
            res.ioctl.status = ret;

            ddkc_dbg("FOPS_IOCTL, status:%d\r\n", res.ioctl.status);
            break;

        case FOPS_POLL:
            //TODO: to be implemented
            ret = -EIO;
            res.poll.status = ret;
            ddkc_err("%s - poll ops is not implemented,ret:%d\r\n", dev_n->rpc_name, ret);
            break;

        case FOPS_LSEEK:
            /* call device's lseek cb */
            ret = fops->lseek(fp, fops_arg->lseek.off, fops_arg->lseek.whence);
            if (ret < 0) {
                ddkc_warn("%s - fops->seek ret:%d\r\n", dev_n->rpc_name, ret);
            }
            res.lseek.off = ret;
            res.lseek.status = 0;
            ddkc_dbg("FOPS_LSEEK, status:%d, offset:%lld\r\n", res.lseek.status, res.lseek.off);
            break;

        case FOPS_CLOSE:
            // close is not handled here, it is handled in main rpc thread
            ret = -EIO;
            res.close.status = ret;
            ddkc_err("%s - close ops is not implemented,ret:%d\r\n", dev_n->rpc_name, ret);
            break;

        default:
            ret = -EINVAL;
            res.priv.status = ret;
            ddkc_err("%s - fops invalid ops id:%d\r\n", dev_n->rpc_name, ie->t);
            break;
    }

    /* send response back to rpc client
     * no matter actual read/write/ioctl/poll/select is done or not, response is necessary
     * */
    ret = aos_rpc_resp(crpc_handle, 1, sizeof(res), &res);
    if (ret)
        ddkc_err("%s - aos_rpc_resp to 0x%x failed, ret:0x%x\r\n", dev_n->rpc_name, crpc_handle, ret);

    return ret;

err:
    /* send error indication back to rpc client */
    res.priv.status = ret;

    ret = aos_rpc_resp(crpc_handle, 1, sizeof(res), &res);
    if (ret)
        ddkc_err("aos_rpc_resp to 0x%x failed, ret:0x%x\r\n", crpc_handle, ret);

    return ret;
}

/**
 * heartbeat message handler - for rpc service active detection
 *
 * @param crpc_handle - remote client's rpc handle
 * @param p_msg - pointer to the message
 * @return
 */
int _loop_msg_handler(int crpc_handle, u_cdev_msg_t *p_msg) {
    int ret = 0;

    ret = aos_rpc_resp(crpc_handle, 1, 1, -1);
    if (ret) {
        ddkc_warn("loop msg handler aos_rpc_resp failed, ret:%d\r\n", ret);
    }
    ddkc_warn("loop msg handler is not implemented yet\r\n");

    return 0;
}


/**
 * device rpc message handler
 * get remote rpc client's handle and receive all contents in this RPC transaction
 * parse the contents' pointer to _cdev_msg_handler for message dispatch
 *
 * @param pkt - rpc message packet
 */
void u_device_rpc_msg_arrived(struct aos_parcel_t *pkt) {
    int ret = 0;
    size_t len = 0;
    int reply_handle = 0;
    char *buf = NULL;
    u_cdev_msg_t *p_msg = NULL;

    ddkc_dbg("rpc pkt arrived\r\n");

    if (!pkt || !pkt->max_size) {
        ddkc_err("invalid pkt:%p or max_size:%d\r\n", pkt, pkt ? pkt->max_size : 0);
        return;
    }
    len = sizeof(int);

    ddkc_dbg("pkt->max_size:%x\r\n", pkt->max_size);
    aos_parcel_get(AP_UVARINT, &reply_handle, &len, pkt);
    ddkc_dbg("reply_handle:%x\r\n", reply_handle);

    len = pkt->max_size;
    ddkc_dbg("max_size:%x\r\n", len);
    buf = malloc(len);
    if (!buf) {
        ddkc_err("malloc buf for rpc msg failed\r\n");
        return;
    }

    /* get RPC message's content in one RPC call with AP_ALL flag */
    ret = aos_parcel_get(AP_ALL, buf, &len, pkt);
    if (ret < 0 || len <= 0) {
        ddkc_err("rpc receive data failed, ret:%d\r\n", ret);
        // TODO: ethan - how to handle this case?
        free(buf);
        return;
    }
    p_msg = (u_cdev_msg_t *)buf;
    if (p_msg->t >= CDEV_MSG_MAX) {
        for (int i = 0; i <  len; i++) {
            ddkc_info("%d - 0x%x\r\n", i, buf[i] & 0xff);
        }
    }
    BUG_ON_MSG((p_msg->t >= CDEV_MSG_MAX),
               "p_msg->t:%d > CDEV_MSG_MAX:%d\r\n", p_msg->t, CDEV_MSG_MAX);

    if (p_msg->t < CDEV_MSG_MAX)
        /* parse rpc message into _cdev_msg_handler for message dispatch */
        ret = (_cdev_msg_handler[p_msg->t](reply_handle, p_msg));
    else {
        u_fops_result_u res;

        res.priv.status = -EINVAL;
        ret = aos_rpc_resp(reply_handle, 1, sizeof(res), &res);
        if (ret)
            ddkc_err("aos_rpc_resp to 0x%x failed, ret:0x%x\r\n", reply_handle, ret);
    }

    free(buf);
    buf = NULL;

    return;
}

/**
 * rpc service event handler
 * @param eid - error id, RPC_EID_STOP is rpc service stopped event id
 * @param event - pointer to aos_revt_param_t
 */
void u_device_rpc_event_cb(int eid, void * event) {
    struct aos_revt_param_t *param = (struct aos_revt_param_t *)event;
    int reason = *(int *)param->param;

    if (eid != RPC_EID_STOP) {
        ddkc_warn("eid:%d, reason:%d, closing rpc service - handle:0x%x\r\n", eid, reason, param->srpc_handle);
        aos_rpc_close(param->srpc_handle);
    } else
        ddkc_dbg("eid:%d, reason:%d\r\n", eid, reason);

    return;
}

/**
 *
 * open RPC message handler, called when OPEN rpc message arrived in main rpc service's handler
 *
 * @param parg - pointer to u_fops_arg_u
 * @param crpc_handle - rpc client's handle,
 * response should be send to rpc client no matter this operation succeed or not
 *
 * @return 0 for success; negative for failure
 */
int u_device_rpc_open(u_fops_arg_u *parg, int crpc_handle) {
    int ret = 0;
    int flag = 0;
    dlist_t *pos = NULL;
    dlist_t *tmp = NULL;
    file_t *fp = NULL;
    inode_t *node = NULL;
    file_node_t *fnode = NULL;
    struct subsys_dev_node *dev_n = NULL;
    subsys_file_ops_t *fops = NULL;
    u_fops_result_u res;
    char *path_name = NULL;

    if (!parg || !crpc_handle) {
        ddkc_err("invalid rpc handle:%d or parg:%p\r\n", crpc_handle, parg);
        ret = -EINVAL;
        goto err;
    }
    memset(&res, 0, sizeof(res));
    // MUST call aos_rpc_resp to send RPC reply to client, no matter open success or fail

    path_name = parg->open.path;
    flag = parg->open.flags;

    // search target device in g_subsys_dev_list
    dlist_for_each_safe(pos, tmp, &g_subsys_dev_list)
    {
        dev_n = aos_container_of(pos, subsys_dev_node_t, n);
        if (!strcmp(dev_n->path_name, path_name))
            break;
        dev_n = NULL;
    }

    if (!dev_n) {
        ddkc_err("device node not found for %s\r\n", path_name);
        ret = -ENODEV;
        goto err;
    }
    ddkc_dbg("device node:%p found for %s\r\n", dev_n, path_name);

    if (!dlist_empty(&dev_n->file_head)) {
        ddkc_warn("device node:%s is already opened\r\n", path_name);
        // concurrent operation by multiple processes is handled here
        // concurrent operation by multiple threads in single process is handled in VFS
    }
    ddkc_dbg("setting up for new fd\r\n");

    // set up new fd for each open RPC request to cover concurrent multiple open operation scenarios
    fnode = malloc(sizeof(file_node_t));
    node = malloc(sizeof(inode_t));
    if (!fnode || !node) {
        ddkc_err("malloc for file failed, fnode:%p, node:%p\n", fnode, node);
        ret = -ENOMEM;
        goto err;
    }
    dlist_init(&fnode->node);
    dlist_init(&fnode->valid);

    fnode->dev_n = NULL;
    fp = &fnode->f;
    memset(fp, 0, sizeof(file_t));
    memset(node, 0, sizeof(inode_t));

    fp->node = node;
    node->i_arg = dev_n->dev.user_data;
    node->i_name = dev_n->path_name;
    node->ops.i_ops = dev_n->fops;
    node->i_flags = flag;

    fops = (subsys_file_ops_t *)node->ops.i_ops;
    if (!fops || !(fops->open)) {
        ddkc_err("invalid fops:%p, or open:%p\n", fops, fops ? fops->open : NULL);
        goto err;
    }

    /* rpc service for device node might be started already */
    if (!dev_n->rpc_run) {
        ddkc_info("start rpc service named:%s, handle:0x%x\r\n", dev_n->rpc_name, dev_n->rpc_handle);
        ret = aos_rpc_run(dev_n->rpc_handle, u_device_rpc_msg_arrived, u_device_rpc_event_cb);
        if (ret) {
            ddkc_err("aos_rpc_run on service:%s fails, ret:%d\r\n", dev_n->rpc_name, ret);
            goto err;
        }
        dev_n->rpc_run = true;
    } else {
        ddkc_info("rpc service named:%s already started, handle:0x%x\r\n", dev_n->rpc_name, dev_n->rpc_handle);
    }

    ddkc_dbg("call driver's open operation\n");
    if (fops->open) {
        ret = (fops->open)(node, fp);
        if (ret) {
            ddkc_err("open %s failed, ret:%d\n", path_name, ret);
            goto err;
        }
        dev_n->ref_cnt++;
    } else {
        ddkc_warn("%s:fops->open is NULL\n", path_name);
    }
    ddkc_dbg("open %s sucess, dev_n->ref_cnt:%d\n", path_name, dev_n->ref_cnt);

    res.priv.status = ret;
    fnode->dev_n = dev_n;
    res.open.farg = fnode;

    ddkc_dbg("crpc_handle:0x%x, status:%d, open.farg:%p\r\n", crpc_handle, res.open.status, res.open.farg);
    //send open RPC result via aos_rpc_resp
    ret = aos_rpc_resp(crpc_handle, 1, sizeof(res), &res);
    if (ret) {
        ddkc_err("aos_rpc_resp on %s to handle:0x%x failed, ret:0x%x\r\n", path_name, crpc_handle, ret);
        if (fops->close)
            (fops->close)(fp);
        else {
            ddkc_warn("%s:fops->close is NULL\n", path_name);
        }
        dev_n->ref_cnt--;

        goto err;
    }
    // add node info into file_head only when all operations succeed
    dlist_add_tail(&fnode->node, &dev_n->file_head);
    dlist_add_tail(&fnode->valid, &g_fnode_valid_list);
    return 0;

err:
    /* check whether rpc service needs to be stopped */
    if (dev_n && !dev_n->ref_cnt && dev_n->rpc_run) {
        ddkc_info("stop rpc service named:%s, handle:0x%x\r\n", dev_n->rpc_name, dev_n->rpc_handle);
        ret = aos_rpc_stop(dev_n->rpc_handle);
        if (ret) {
            ddkc_err("aos_rpc_stop on %s failed, ret:%d\n", dev_n->rpc_name, ret);
        }
        dev_n->rpc_run = false;
    }

    /* clear other resoruce allocated in earlier steps */
    if (fnode) {
        free(fnode);
        fnode = NULL;
    }

    if (node) {
        free(node);
        node = NULL;
    }

    /* send back RPC response to rpc client */
    ddkc_err("open %s failed, send rpc resp:%d\r\n", path_name, ret);
    res.open.status = ret;
    res.open.farg = NULL;

    //send open result via aos_rpc_resp
    ret = aos_rpc_resp(crpc_handle, 1, sizeof(res), &res);
    if (ret)
        ddkc_err("aos_rpc_resp to 0x%x failed, ret:0x%x\r\n", crpc_handle, ret);

    return ret;
}

/**
 * close RPC message handler, called when CLOSE rpc message arrived in main rpc service's handler
 *
 * @param parg - pointer to u_fops_arg_u
 * @param crpc_handle - rpc client's handle,
 * response should be send to rpc client no matter this operation succeed or not
 *
 * @return 0 for success; negative for failure
 */
int u_device_rpc_close(u_fops_arg_u *parg, int crpc_handle) {
    int ret = 0;
    bool valid = false;
    u_fops_result_u res;
    file_t *fp = NULL;
    file_node_t *fnode = NULL;
    char *path_name = NULL;
    struct subsys_dev_node *dev_n = NULL;
    subsys_file_ops_t *fops = NULL;

    if (!parg || !crpc_handle) {
        ddkc_err("invalid rpc handle:%d or parg:%p\r\n", crpc_handle, parg);
        ret = -EINVAL;
        goto err;
    }
    fnode = (file_node_t *)parg->priv.farg;

    /*
     * do fnode validation, or permission fault might happen,
     * becuase fnode might be other process' memory region
     *
     */
    valid = _is_valid_fnode(fnode);
    if (!valid) {
        ddkc_err("fnode:%p is not valid\r\n", fnode);
        ret = -EINVAL;
        goto err;
    }
    dev_n = fnode->dev_n;

    if (!dev_n->rpc_run) {
        ddkc_warn("rpc service is already stopped\r\n");
        WARN_ON_MSG(dev_n->ref_cnt, "rpc service is stopped but dev_n->ref_cnt:%d is not 0\r\n", dev_n->ref_cnt);
        WARN_ON_MSG(!dlist_empty(&dev_n->file_head), "rpc service is stopped but dev_n->file_head is not empty\r\n");
        ret = -EALREADY;
        goto err;
    }

    fp = &fnode->f;
    path_name = dev_n->path_name;
    ddkc_dbg("closing %s, dev_n->rpc_handle:0x%x\r\n", path_name, dev_n->rpc_handle);
    fops = dev_n->fops;

    ret = fops->close(fp);
    dev_n->ref_cnt--;
    if (!ret)
        ddkc_dbg("close %s success, ret:%d, dev_n->ref_cnt:%d\r\n", path_name, ret, dev_n->ref_cnt);
    else
        ddkc_err("close %s failed, ret:%d\r\n", path_name, ret);

    if (!dev_n->ref_cnt && dev_n->rpc_run) {
        ddkc_err("dev_n->ref_cnt is 0, should stop rpc service named %s\r\n", dev_n->rpc_name);
        ret = aos_rpc_stop(dev_n->rpc_handle);
        if (ret) {
            ddkc_err("aos_rpc_stop on %s failed, ret:%d\n", dev_n->rpc_name, ret);
        }
        dev_n->rpc_run = false;
    }

    // delete from device node's file list
    dlist_del(&fnode->node);
    dlist_del(&fnode->valid);

    free(fp->node);
    fp->node = NULL;
    free(fnode);
    ddkc_dbg("close %s %s, send rpc resp:%d\r\n", path_name, ret ? "fail" : "success", ret);

err:
    res.close.status = ret;

    //send close result via aos_rpc_resp
    ret = aos_rpc_resp(crpc_handle, 1, sizeof(res), &res);
    if (ret)
        ddkc_err("send rpc response to 0x%x failed, ret:0x%x\r\n", crpc_handle, ret);

    return ret;
}

#endif

/**
 *
 * device open stub for delay init required driver
 *
 * @param node - pointer to inode,  device node information
 * @param f - pointer to file, file info
 *
 * @return 0 for success; negative for failure
 */
static int _device_vfs_open (inode_t *node, file_t *f) {
    int ret = 0;
    dlist_t *pos = NULL;
    dlist_t *tmp = NULL;
    struct subsys_dev *sdev = NULL;
    struct subsys_dev_node *dev_n = (struct subsys_dev_node *)node->i_arg;

    ddkc_dbg("dev_n:%p\r\n", dev_n);
    if (!dev_n) {
        ddkc_err("node->i_arg is NULL, ignore\r\n");
        return -EINVAL;
    }

    ddkc_info("opening %s\r\n", node->i_name);

    dlist_for_each_safe(pos, tmp, &g_subsys_dev_list)
    {
        dev_n = aos_container_of(pos, subsys_dev_node_t, n);
        sdev = &dev_n->dev;
        if (!strcmp(dev_n->path_name, node->i_name))
            break;
        dev_n = NULL;
    }

    if (!dev_n) {
        ddkc_err("dev_n not found for %s\r\n", node->i_name);
        return -EIO;
    }

    sdev = &dev_n->dev;

    /* check whether real_init or real_probe is always called or not */
    if (sdev->delay_init) {
        if (dev_n->real_init) {
            ret = (dev_n->real_init)(dev_n->pdev);
        } else
            ret = (dev_n->real_probe)(dev_n->pdev);
        if (ret) {
            ddkc_err("fail to init/probe for %s, ret:%d\r\n", node->i_name, ret);
            return -EIO;
        }
        sdev->delay_init = false;
        node->i_arg = sdev->user_data;
    }
    /* call real_open provided by driver and logged in aos_dev_reg */
    if (dev_n->real_open)
        return (dev_n->real_open)(node, f);

    return 0;
}

/**
 *
 * fake driver probe callback for delay init required driver
 *
 * @param pdev - pointer to the device
 *
 * @return 0 for success; negative for failure
 */
static int _device_vfs_init(struct u_platform_device *pdev) {
    ddkc_dbg("enter\r\n");
    return 0;
}

/**
 *
 * register device driver
 *
 * @param sdev - pointer to subsys device
 * @param sfops - pointer to file operations provided by device driver
 * @param sdrv - pointer to device driver speicified operations
 * @param rpc_only - whether this driver is rpc driver or not - not used for the moment
 *
 * @return 0 for success; negative for failure
 */
int aos_dev_reg_with_flag (struct subsys_dev *sdev, subsys_file_ops_t *sfops, struct subsys_drv* sdrv, bool rpc_only) {
    int ret = 0;
    char *p = NULL;
    char *path_name = NULL;
    int path_name_len = 0;
    struct u_platform_driver *pdrv = NULL;
    struct u_platform_device *pdev = NULL;
    subsys_file_ops_t *fops = NULL;
    struct subsys_dev_node *dev_n = NULL;
    char *rpc_name = NULL;
    int rpc_handle = 0;
    char *main_rpc_name = NULL;

    if (!sfops || !sdev || !sdev->node_name) {
        ddkc_err("invalid sfops:%p, sdev:%p or name:%p\r\n", sfops, sdev, sdev ? sdev->node_name : NULL);
        return -EINVAL;
    }

#ifdef USER_SPACE_DRIVER
    /* register a rpc service for user space device driver
     * with pre-defined service name format - "rpc-<pid>-<node_name>"
     */
    rpc_name = (char *)malloc(UDRIVER_RCP_NAME_MAX_LEN);
    if (!rpc_name) {
        ddkc_err("malloc failed, rpc_name:%p\r\n", rpc_name);
        ret = -ENOMEM;
        goto err_malloc;
    }

    ret = snprintf(rpc_name, UDRIVER_RCP_NAME_MAX_LEN, UDRIVER_RPC_NAME_FORMAT, getpid(), sdev->node_name);
    if (ret < 0) {
        ddkc_err("rpc_name snprintf for node_name:%s failed\r\n", sdev->node_name);
        goto err_malloc;
    }

    ret = aos_rpc_regist(rpc_name, U_CDEV_RPC_BUFFER_SIZE, &rpc_handle);
    if (ret) {
        ddkc_err("register driver rpc service[%s] failed, ret:%d\r\n", rpc_name, ret);
        goto err_malloc;
    }
    ddkc_dbg("register driver rpc service[%s] for %s succeed, handle:0x%x\r\n",
              rpc_name, sdev->node_name, rpc_handle);
#endif

    pdev = (struct u_platform_device *)malloc(sizeof(*pdev));
    pdrv = (struct u_platform_driver *)malloc(sizeof(*pdrv));
    fops = (subsys_file_ops_t *)malloc(sizeof(*fops));
    path_name_len = strlen(NODE_PREFIX) + strlen(sdev->node_name) + 1;
    path_name = (char *)malloc(path_name_len);

    dev_n = (struct subsys_dev_node *)malloc(sizeof(*dev_n) + strlen(sdev->node_name) + 1);

    if (!pdev || !pdrv || !fops || !path_name || !dev_n) {
        ddkc_err("malloc failed, pdev:%p, pdrv:%p, fops:%p dev:%p, or path_name:%p\r\n", pdev, pdrv, fops, dev_n, path_name);
        ret = -ENOMEM;
        goto err_malloc;
    }

    memset(pdev, 0, sizeof(*pdev));
    memset(pdrv, 0, sizeof(*pdrv));
    memset(fops, 0, sizeof(*fops));
    memset(path_name, 0, path_name_len);
    memset(dev_n, 0, sizeof(*dev_n) + strlen(sdev->node_name) + 1);

    //TODO: use sdrv->drv_name later
    pdev->name = strdup(sdev->node_name);
    if (!pdev->name) {
        ddkc_err("dev strdup on %p failed\r\n", sdev->node_name);
        ret = -EIO;
        goto err_malloc_name;
    }
    ddkc_dbg("pdev->name:%s\r\n", pdev->name);
    pdev->id = 0;

    /* register device to u_platform bus */
    ret = u_platform_device_register(pdev);
    if (ret) {
        ddkc_err("u_platform_device_register addplatform_device_register failed, ret:%d\r\n", ret);
        ret = -EIO;
        goto err_dev_reg;
    }

    /* log driver's private user_data pointer */
    u_platform_set_user_data(pdev, sdev->user_data);
    ddkc_dbg("set user_data:%p to user_data of pdev:%p\r\n", sdev->user_data, pdev);

    if (sdrv) {
        /* for legacy driver */
        pdrv->init = (void *)sdrv->init;
        pdrv->deinit = (void *)sdrv->deinit;
        pdrv->pm = (void *)sdrv->pm;

        /* for linux like drivers */
        pdrv->probe = (void *)sdrv->probe;
        pdrv->remove = (void *)sdrv->remove;
        pdrv->suspend = (void *)sdrv->suspend;
        pdrv->resume = (void *)sdrv->resume;
        pdrv->shutdown = (void *)sdrv->shutdown;
    } else
        ddkc_dbg("sdrv callback is not set\r\n");

    pdrv->driver.name = strdup(sdev->node_name);
    if (!pdrv->driver.name) {
        ddkc_dbg("drv strdup on %p failed\r\n", sdev->node_name);
        ret = -ENOMEM;
        goto err_dev_reg;
    }
    if (sdev->delay_init) {
        dev_n->real_init = pdrv->init;
        pdrv->init = _device_vfs_init;

        dev_n->real_probe = pdrv->probe;
        pdrv->probe = _device_vfs_init;
    }

    ddkc_dbg("pdrv->name:%s\r\n", pdrv->driver.name);

    /* register driver to u_platform bus
     * driver init/probe callback will be called in this step
     * */
    ret = u_platform_driver_register(pdrv);
    if (ret) {
        ddkc_err("u_platform_driver_register failed, ret:%d\r\n", ret);
        ret = -EIO;
        goto err_drv_reg;
    }

    memcpy(fops, sfops, sizeof(*fops));

    p = path_name;
    strncpy(path_name, NODE_PREFIX, strlen(NODE_PREFIX) + 1);

    p += strlen(NODE_PREFIX);
    strncpy(p, sdev->node_name, strlen(sdev->node_name) + 1);

    dlist_init(&dev_n->n);
    memcpy(&dev_n->dev, sdev, sizeof(*sdev));
    dev_n->dev.node_name = (char *)(dev_n + 1);
    strncpy(dev_n->dev.node_name, sdev->node_name, strlen(sdev->node_name));

    // log device rpc handle and rpc_name
#ifdef USER_SPACE_DRIVER
    dev_n->rpc_handle = rpc_handle;
    dev_n->rpc_name = rpc_name;
    main_rpc_name = u_driver_get_main_service_name();
#else
    dev_n->rpc_name = NULL;
    main_rpc_name = NULL;
#endif
    dev_n->path_name = path_name;
    dev_n->fops = fops;
    dev_n->pdev = pdev;
    dev_n->pdrv = pdrv;
    dev_n->ref_cnt = 0;
    dev_n->rpc_run = false;
    dlist_init(&dev_n->file_head);

    dlist_add_tail(&dev_n->n, &g_subsys_dev_list);

    ddkc_dbg("registering %s to vfs\r\n", path_name);

    ddkc_dbg("main rpc name:%s, rpc_name:%s\r\n", main_rpc_name, dev_n->rpc_name);
    if (sdev->delay_init) {
        dev_n->real_open = fops->open;
        fops->open = _device_vfs_open;

    }
#ifdef USER_SPACE_DRIVER
    ret = aos_union_register_driver(path_name, fops, sdev->delay_init ? dev_n : sdev->user_data, main_rpc_name, dev_n->rpc_name);
#else
    ret = aos_register_driver(path_name, fops, sdev->delay_init ? dev_n : sdev->user_data);
#endif

    if (ret) {
        dlist_del(&dev_n->n);
        ddkc_info("register %s to vfs failed, rpc_only:%d, sdev->delay_init:%d, ret:%d\r\n", path_name, rpc_only, sdev->delay_init, ret);
        goto err_vfs_reg;
    }

    ddkc_dbg("register %s to vfs success, rpc_only:%d, sdev->delay_init:%d\r\n", path_name, rpc_only, sdev->delay_init);

    return 0;

err_vfs_reg:
    // unregiste rpc service and free rpc service name field
    u_platform_driver_unregister(pdrv);

err_drv_reg:
    u_platform_device_unregister(pdev);

err_dev_reg:
err_malloc_name:
    if (pdev->name)
        free((void *)pdev->name);
    pdev->name = NULL;

err_malloc:
    if (pdev) {
        free(pdev);
        pdev = NULL;
    }

    if (pdrv) {
        if (pdrv->driver.name)
            free(pdrv->driver.name);

        free(pdrv);
        pdrv = NULL;
    }

    if (path_name) {
        free(path_name);
        path_name = NULL;
    }

    if (fops) {
        free(fops);
        fops = NULL;
    }

    if (dev_n) {
        free(dev_n);
        dev_n = NULL;
    }

#ifdef USER_SPACE_DRIVER
    if (rpc_handle) {
        aos_rpc_close(rpc_handle);
        rpc_handle = 0;
    }

    if (rpc_name) {
        free(rpc_name);
        rpc_name = NULL;
    }
#endif

    ddkc_err("device register failed, ret:%d\r\n", ret);
    return ret;
}

/**
 *
 * register device array
 *
 * @param sdev pointer to devices array be register
 * @param size device array size
 * @param sfops file operations API
 * @param sdrv  - device driver operations callback
 *              - probe will be called when device exist
 *              - remove will be called when aos_dev_unreg is called
 * @return 0 for success; negative for failure
 */
int aos_devs_reg(struct subsys_dev *sdev[], int size, subsys_file_ops_t *sfops, struct subsys_drv* sdrv) {
    int i = 0;
    int ret = 0;

    for (i = 0; i < size; i++) {
        ret = aos_dev_reg(sdev[i], sfops, sdrv);
        if (ret) {
            i--;
            goto err;
        }
    }

    return 0;
err:
    for (; i >= 0; i--) {
        int ret = 0;
        ret = aos_dev_unreg(sdev[i]);
        if (ret) {
            ddkc_err("unregister device %s failed\r\n", sdev[i]->node_name);
        }
    }

    return ret;
}

/**
 * unregister device driver
 *
 * @param sdev - device identifier, name and type must be carefully set
 *
 * @return 0 if device register success; return negative error no. if device register fails
 */
int aos_dev_unreg(struct subsys_dev *sdev) {
    int ret = 0;
    char *p = NULL;
    char *path_name = NULL;
    int path_name_len = 0;
    subsys_dev_node_t *dev_n = NULL;
    dlist_t *pos = NULL;
    dlist_t *tmp = NULL;
    struct subsys_dev *d = NULL;

    path_name_len = strlen(NODE_PREFIX) + strlen(sdev->node_name) + 1;
    path_name = (char *)malloc(path_name_len);
    memset(path_name, 0, path_name_len);

    p = path_name;
    strncpy(p, NODE_PREFIX, strlen(NODE_PREFIX));

    p += strlen(NODE_PREFIX);
    strncpy(p, sdev->node_name, strlen(sdev->node_name));

    ddkc_info("unregistering %s from vfs\r\n", path_name);
#ifdef USER_SPACE_DRIVER
    ret = aos_union_unregister_driver(path_name);
#else
    ret = aos_unregister_driver(path_name);
#endif
    ddkc_info("unregister %s from vfs %s\r\n", path_name, !ret ? "success" : "failed");

    free(path_name);

    ret = -1;
    /* enumberate device list for target subsys device node to be removed */
    dlist_for_each_safe(pos, tmp, &g_subsys_dev_list) {
        dev_n = aos_container_of(pos, subsys_dev_node_t, n);
        d = &dev_n->dev;
        if(d->type == sdev->type && !strcmp(d->node_name, sdev->node_name) && d->user_data == sdev->user_data) {
            ddkc_info("sdev named with %s found, delete it\r\n", d->node_name);

            // unregiste rpc service and free rpc service name field
#ifdef USER_SPACE_DRIVER
            /* close rpc servce for user space driver */
            aos_rpc_close(dev_n->rpc_handle);
            free(dev_n->rpc_name);
#endif
            u_platform_driver_unregister(dev_n->pdrv);
            u_platform_device_unregister(dev_n->pdev);
            dlist_del(&dev_n->n);

            /* free all resources */
            free((void *)dev_n->pdev->name);
            free(dev_n->pdev);
            free(dev_n->pdrv->driver.name);
            free(dev_n->pdrv);
            free(dev_n->path_name);
            free(dev_n->fops);
            free(dev_n);

            ret = 0;
            break;
        }
    }

    ddkc_err("device unregister %s, ret:%d\r\n", ret ? "fail" : "success", ret);

    return ret;
}

/**
 *
 * register devices and corresponding driver into device/driver module
 *
 * @param sdev  - device identifier, name and type must be carefully set
 * @param sfops - file operations API
 * @param sdrv  - device driver operations callback
 *              - init/probe will be called when device exist
 *              - deinit/remove will be called when aos_dev_unreg is called
 * @return 0 if device register success; return negative error no. if device register fails
 */
int aos_dev_reg(struct subsys_dev *sdev, subsys_file_ops_t *sfops, struct subsys_drv* sdrv) {
    return aos_dev_reg_with_flag(sdev, sfops, sdrv, false);
}

/**
 * This function is not used for the moment
 * register remote devices/driver into device/driver module, only support rpc mode
 *
 * @param sdev  - device identifier, name and type must be carefully set
 * @param sfops - file operations API
 * @param sdrv  - device driver operations callback
 *              - probe will be called when device exist
 *              - remove will be called when aos_dev_unreg is called
 * @return 0 if device register success; return negative error no. if device register fails
 */
int aos_remote_dev_reg(struct subsys_dev *sdev, subsys_file_ops_t *sfops, struct subsys_drv* sdrv) {
    return aos_dev_reg_with_flag(sdev, sfops, sdrv, true);
}

/**
 * unregister device array
 *
 * @param sdev - pointer to devices array be register
 * @param size - device array size
 *
 * @return 0 if device register success; return negative error no. if device register fails
 */
int aos_devs_unreg(struct subsys_dev *sdev[], int size) {
    int i = 0;
    int ret = 0;

    for (i = 0; i < size; i++) {
        ret = aos_dev_unreg(sdev[i]);
    }

    return ret;
}

/**
 *
 * device vfs core layer initialization
 * declared with EARLY_DRIVER_ENTRY, which is before vendor driver initialization
 *
 * @return always return 0
 *
 */
int device_vfs_init(void) {
#ifdef AOS_COMP_VFS
    aos_vfs_init();
#endif
    dlist_init(&g_subsys_dev_list);
    dlist_init(&g_fnode_valid_list);

    return 0;
}

EARLY_DRIVER_ENTRY(device_vfs_init)
