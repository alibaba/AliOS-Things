/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef __AOS_RPC__
#define __AOS_RPC__

#ifdef __cplusplus
extern "C" {
#endif

#include "k_api.h"
#include "parcel.h"

#define AOS_RPC_NAME_MAX_SIZE  (127)

enum {
    RPC_EID_ERR = 0,
    RPC_EID_STOP = 1,
    RPC_EID_CLOSE = 2,
};

struct aos_revt_param_t {
    int srpc_handle;
    void *param;
};

/* register service to service manager used by RSS (Remote Service Server) */
int aos_rpc_regist(const char *name, size_t rx_req_buf_len, int *srpc_handle);
int aos_rpc_run(int srpc_handle, void (*req_arrived)(struct aos_parcel_t *pkt), void (*event_cb)(int, void *));
int aos_rpc_stop(int srpc_handle);

/* request service from service manager used by RSC (Remote Service Client) */
int aos_rpc_open_sync(const char *name, size_t rsp_buf_len, int *crpc_handle);
int aos_rpc_open_async(const char *name, int *crpc_handle);

/* close rpc handle used by RSS and RSC */
int aos_rpc_close(int rpc_handle);

/* sync or async call inter-process service used by RSC */
int aos_rpc_call_sync(int crpc_handle, struct aos_parcel_t *reply, int nparams, ...);
int aos_rpc_call_async(int crpc_handle, int nparams, ...);

/* send reponse of serivce to request used by RSS */
int aos_rpc_resp(int crpc_handle, int nparams, ...);

#ifdef __cplusplus
}
#endif

#endif /* __AOS_RPC__ */
