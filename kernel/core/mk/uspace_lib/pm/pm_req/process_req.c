/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <parcel.h>
#include <rpc.h>
#include <string.h>
#include <procmgmt.h>

int serive_proc_req(char *procname, int funcode)
{
    const  char *rpc_service_sum_name = PROC_SERVICE_NAME;
    int    crpc_handle;
    size_t len;
    char   name_buf[PROC_NAME_MAX];
    int    status;
    int    ret;

    memset(name_buf, 0, PROC_NAME_MAX);
    strncpy(name_buf, procname, PROC_NAME_MAX - 1);

    struct aos_parcel_t reply;

    status = aos_rpc_open_sync(rpc_service_sum_name, PROC_REV_BUF, &crpc_handle);
    if (status != 0) {
        printf("%s %d open service error\r\n", __func__, __LINE__);
        return -1;
    }

    status = aos_rpc_call_sync(crpc_handle, &reply, 2, sizeof(funcode), &funcode, PROC_NAME_MAX, name_buf);
    if (status != 0) {
        printf("%s %d service req error\r\n", __func__, __LINE__);
        ret = aos_rpc_close(crpc_handle);
        if (ret != 0) {
            printf("%s %d aos_rpc_close error\r\n", __func__, __LINE__);
        }
        return -1;
    }

    /* parse response */
    len = sizeof(status);
    ret = aos_parcel_get(AP_BUF, &status, &len, &reply);
    if (ret != 0) {
        printf("%s %d aos_parcel_get error\r\n", __func__, __LINE__);
    }

    ret = aos_rpc_close(crpc_handle);
    if (ret != 0) {
        printf("%s %d aos_rpc_close error\r\n", __func__, __LINE__);
    }

    return status;
}

int aos_process_load(char *procname)
{
    if (procname == NULL) {
        return -1;
    }

    return serive_proc_req(procname, PROC_SERVICE_LOAD);
}

int aos_process_unload(char *procname)
{
    if (procname == NULL) {
        return -1;
    }

    return serive_proc_req(procname, PROC_SERVICE_UNLOAD);
}



