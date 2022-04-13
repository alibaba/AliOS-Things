/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <parcel.h>
#include <rpc.h>
#include <utask.h>
#include <procmgmt.h>
#include "process_service.h"

/* deserialize parameters implemented by service provider */
int rpc_service_proc_deserial(struct aos_parcel_t *pkt, int *reply_handle, int *funcode, char *procname)
{
    size_t len = sizeof(int);
    (void)aos_parcel_get(AP_UVARINT, reply_handle, &len, pkt);

    len = sizeof(*funcode);
    (void)aos_parcel_get(AP_BUF, funcode, &len, pkt);
    len = PROC_NAME_MAX;
    (void)aos_parcel_get(AP_BUF, procname, &len, pkt);

    return RHINO_SUCCESS;
}

static void service_proc_event_cb(int eid, void *val)
{
    struct aos_revt_param_t *param = (struct aos_revt_param_t *)val;
    int reason = *(int *)param->param;

    (void)eid;
    (void)aos_rpc_close(param->srpc_handle);
    printf("process service error reason:%d\r\n", reason);
}

void service_proc_req_arrived(struct aos_parcel_t *pkt)
{
    int  crpc_handle;
    int  funcode;
    char proc_name[PROC_NAME_MAX];
    int  ret = -1;
    int  pid = -1;

    /* Step-1: deserail parameters of rpc */
    rpc_service_proc_deserial(pkt, &crpc_handle, &funcode, proc_name);

    /* Step-2: Call service */
    switch (funcode) {
        case PROC_SERVICE_LOAD:
            if (proc_is_running(proc_name) == 0) {
                printf("%s is already running\r\n", proc_name);
                ret = -1;
                break;
            }
            #ifdef AOS_PROC_LOAD_FROM_USAPCE
            extern int32_t aos_uproc_load(const char *file);
            pid = aos_uproc_load(proc_name); //fs in uspace
            #else
            pid = krhino_process_load(proc_name);
            if (pid > 0) {
                proc_map_add(proc_name, pid);
            }
            #endif
            ret = pid;
            proc_map_show();
            break;
        case PROC_SERVICE_UNLOAD:
            pid = proc_map_id_get(proc_name);
            if (pid > krhino_uprocess_id_get()) {
                printf("delete process pid %d\r\n", pid);
                ret = krhino_process_unload(pid);
                if (ret == 0) {
                    proc_map_id_remove(pid);
                }
            }
            proc_map_show();
            break;
        default:
            printk("pm: this funcode is not support\r\n");
            break;
    }

    if (crpc_handle != 0) {
        /* Step-3: send response packet back (Optional) */
        if(aos_rpc_resp(crpc_handle, 1, sizeof(ret), &ret)){
            printk("pm: pm rpc resp fail!\r\n");
        }
    }
}

int serive_proc_server_setup(void)
{
    const char *rpc_service_sum_name = PROC_SERVICE_NAME;
    size_t rpc_proc_req_buf_len = PROC_REV_BUF;
    int srpc_handle;

    proc_map_init();
    /* procmamt is create by kernel */
    proc_map_add("procmgmt", krhino_uprocess_id_get());

    (void)aos_rpc_regist(rpc_service_sum_name, rpc_proc_req_buf_len, &srpc_handle);
    (void)aos_rpc_run(srpc_handle, service_proc_req_arrived, service_proc_event_cb);

    return 0;
}

