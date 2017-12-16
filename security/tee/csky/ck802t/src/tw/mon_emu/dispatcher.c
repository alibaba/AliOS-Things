/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_types.h"
#include "tee_tos.h"
#include "tee_core.h"
#include "tee_dbg.h"
#include "tee_comm.h"

extern int32_t sw_to_lpm(void *msg);

int32_t dispatcher_main(uint32_t smcid, uint32_t subcmd, uint32_t buf, uint32_t rsvd)
{
    int32_t ret = COMM_MSG_DONE;

    if (NTW_NEW_RQST == smcid) {
        if (TEE_SUCCESS != tee_core_proc((void *)buf)) {
            ret = COMM_MSG_DONE;
        }
    }
#ifdef CONFIG_MEMORY_PROFILING
    else if (0x12345678 == smcid) {
        extern void mem_profiling_dump_stack_usage(void);
        extern void mem_profiling_dump_heap_usage(void);

        mem_profiling_dump_stack_usage();
        mem_profiling_dump_heap_usage();
    }
#endif
    else if (NTW_LPM_RQST == smcid) {
        if (TEE_SUCCESS == sw_to_lpm((void *)buf)) {
            ret = COMM_MSG_DONE;
        }
    } else {
        ret = -1;
    }

    return ret;
}
