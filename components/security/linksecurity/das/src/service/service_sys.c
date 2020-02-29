/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <das/platform.h>
#include <das/service.h>

#define SYS_INFO_KEY_SP "sp:"

static size_t _current_sp(size_t v, ...)
{
    size_t sp;
    sp = (size_t)&v;
    return sp;
}

static int _sys_init(void)
{
    return 0;
}

static das_result_t _sys_info(char *buffer, size_t size, das_service_state_t *state)
{
    size_t len;

    len = strlen(SYS_INFO_KEY_SP);
    if (len >= size) {
        DAS_LOG("No buffer size for sys info: %lx\n", (long)len);
        return DAS_ERROR_SHORT_BUFFER;
    }

    memcpy(buffer, SYS_INFO_KEY_SP, len);
    size_t sp = _current_sp(len);
    das_hex2string(buffer + len, size - len, (uint8_t*)&sp, sizeof(size_t));

    state->status = DAS_SRV_STATUS_FINISH;

    return DAS_SUCCESS;
}

das_service_t service_sys = {
    .name = "sys",
    .init = _sys_init,
    .info = _sys_info,
    .attest  = NULL,
    .measure = NULL,
};

