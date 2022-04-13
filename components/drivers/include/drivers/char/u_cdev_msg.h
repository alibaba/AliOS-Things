/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _U_CDEV_MSG_H_
#define _U_CDEV_MSG_H_

#define U_CDEV_RPC_BUFFER_SIZE (4 * 1024 + 64)

typedef enum fops_msg_id {
    FOPS_MIN = 0,

    /* Application -> C */
    FOPS_OPEN,    /* open operation */
    FOPS_READ,    /* read operation */
    FOPS_WRITE,   /* write operation */
    FOPS_IOCTL,   /* ioctl operation */
    FOPS_POLL,    /* poll operation */
    FOPS_LSEEK,    /* lseek operation */
    FOPS_CLOSE,   /* close operation */

    FOPS_MAX
} fops_msg_id_e;

typedef enum u_cdev_msg_id {
    /* VFS<->char device */
    CDEV_MSG_FOPS,     /* file operation message id */

    /* Others */
    CDEV_MSG_LOOP,
    CDEV_MSG_MAX
} u_cdev_msg_id_e;

#pragma pack(1)
/**
 * file operation arguments
 * open.flags: equeals to flags when calling open("path", flags)
 * open.path: file's full pathname
 * read.farg: equals to the open.arg in u_fops_result returned after open operation
 * read.len: target length of the read operation
 * write.farg: equals to the open.arg in u_fops_result returned after open operation
 * write.len: target length of the write operation
 * write.buf: the data to be write
 * ioctl.farg: equals to the open.arg in u_fops_result returned after open operation
 * ioctl.cmd: cmd equals to ioctl(fd, cmd, arg)
 * ioctl.arg: arg equals to ioctl(fd, cmd, arg)
 * poll.farg: equals to the open.arg in u_fops_result returned after open operation
 * poll.flag: read/write flags
 */
typedef union u_fops_arg {
    struct {
        int flags;
        char path[0];
    } open;

    struct {
        void *farg;
        int len;
    } read;

    struct {
        void *farg;
        int len;
        char buf[0];
    } write;

    struct {
        void *farg;
        int cmd;
        unsigned long arg;
    } ioctl;

    struct {
        void *farg;
        int flag;
    } poll;

    struct {
        void *farg;
        off_t off;
        int whence;
    } lseek;

    struct {
        void *farg;
    } close;

    struct {
        void *farg;
    } priv;
} u_fops_arg_u;

/**
 * file ops result
 * status: 0 means operation success
 * open.farg: special pointer used in read/write/ioctl/poll operations
 * read.len: the actual read bytes, while data is stored into read.data
 * read.data: pointed to the buffer where the read data is stored
 * write.len: actual length written to the file succefully
 * poll.events: value of fd.events
 *
 */
typedef union u_fops_result {

    struct {
        int status;
        void *farg;
    } open;

    struct {
        int status;
        ssize_t len;
        char data[0];
    } read;

    struct {
        int status;
        ssize_t len;
    } write;

    struct {
        int status;
        short events;
    } poll;

    struct {
        int status;
        off_t off;
    } lseek;

    struct {
        int status;
    } ioctl;

    struct {
        int status;
    } close;

    struct {
        int status;
    } priv;
} u_fops_result_u;

typedef struct u_cdev_msg_ie {
    unsigned short t;
    unsigned short l;
    u_fops_arg_u v[0];
} u_cdev_msg_ie_t;

typedef u_cdev_msg_ie_t u_fops_ie_t;

/**
 * struct dev_drv_msg - driver related IPC message format
 * @msg_type: message
 * */
typedef struct u_cdev_msg {
    u_cdev_msg_id_e t;
    unsigned short l;
    u_cdev_msg_ie_t v[0];
} u_cdev_msg_t;

#pragma pack()

typedef int (*u_cdev_msg_handler)(int crpc_handle, u_cdev_msg_t *p_msg);

#define IE_FOR_EACH(p_msg) \
	for (ie = (u_cdev_msg_ie_t *)p_msg->v; \
		ie < (p_msg->v + sizeof(p_msg->l) + p_msg->l); \
		ie = (u_cdev_msg_ie_t *)(ie->v + ie->l))

#endif //_U_CDEV_MSG_H_
