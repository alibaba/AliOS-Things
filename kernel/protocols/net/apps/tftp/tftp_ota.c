/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <hal/hal.h>
#include "lwip/ip_addr.h"
#include "lwip/debug.h"
#include "lwip/apps/tftp.h"

static void* tftp_fopen(const char* fname, const char* mode, u8_t write)
{
    void *fp = NULL;

    if (write == 0)
        return NULL;

    if (strncmp(mode, "netascii", 8) == 0 || strncmp(mode, "octet", 5) == 0) {
        int offset = 0;
        hal_ota_init(&offset);
        fp = (void*)1;
    }
    return fp;
}

static void tftp_fclose(void* handle)
{
}

static int tftp_fread(void* handle, void* buf, int bytes)
{
    return 0;
}

static int tftp_fwrite(void* handle, struct pbuf* p)
{
    uint8_t buff[512];
    int  writebytes = -1;
    pbuf_copy_partial(p, buff, p->tot_len, 0);
    writebytes = hal_ota_write(hal_ota_get_default_module(), NULL, buff, p->tot_len);
    if (writebytes == 0) writebytes = p->tot_len;
    return writebytes;
}

void ota_get_done(int error, int len)
{
    if (error == 0) {
        LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("ota get done, %d bytes reveived\n", len));
        ota_finish_param_t param;
        param.update_type = OTA_ALL;
        param.result_type = OTA_FINISH;
        hal_ota_set_boot(hal_ota_get_default_module(), &param);
    } else {
        LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("ota get failed\n"));
    }
}
const tftp_context_t ota_ctx = {
    .open = tftp_fopen,
    .close = tftp_fclose,
    .read = tftp_fread,
    .write = tftp_fwrite
};
