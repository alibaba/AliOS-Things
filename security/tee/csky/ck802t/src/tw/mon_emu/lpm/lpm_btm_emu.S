/**
 * Copyright (C) 2015-2016 The YunOS Project. All rights reserved.
 */

#ifdef LPM_BTM_EMU
#include "ck_pmu.h"
#include "tee_addr_map.h"
#include "hal_memmap.h"

.import LPM_RESUEM_POINT_SAVED_ADDR

.global write_btm_resume_jpm_addr

write_btm_resume_jpm_addr:
    /* for lpm test */
    push r15
    lrw r0,_lpm_entry
    lrw r1,0x0
    st.w r0, (r1,0x0)
    pop r15

_lpm_entry:
    lrw r0, PLATFORM_PMU_ADDRBASE
    ld.w r1, (r0,0x0)
    btsti r1, 5
    bt STOP
    bf .
STOP:
    lrw r0, PLATFORM_PMU_ADDRBASE
    ld.w r1, (r0,0x0)

    btsti r1, 3
    bt  __goto_resume_func
    bf STANDBY

STANDBY:
    lrw r0, PLATFORM_PMU_ADDRBASE
    ld.w r1, (r0,0x0)

    btsti r1, 4
    bt  __goto_resume_func

__goto_resume_func:
    lrw  r0, LPM_RESUEM_POINT_SAVED_ADDR
    ld.w r2, (r0,0x0)
    jmp r2
#endif
