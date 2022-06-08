#include "core_diag.h"

static void _core_diag_default_callback(void *diag_handle, uint64_t timestamp, int32_t code, uint8_t *data, uint32_t data_len);

static void *g_diag_handle = NULL;
static core_diag_callback g_diag_cb = _core_diag_default_callback;

static void _core_diag_default_callback(void *diag_handle, uint64_t timestamp, int32_t code, uint8_t *data, uint32_t data_len)
{
    
}

void core_diag_set_cb(void *diag_handle, core_diag_callback cb)
{
    if (cb == NULL) {
        g_diag_cb = _core_diag_default_callback;
        g_diag_handle = NULL;
    }else{
        g_diag_handle = diag_handle;
        g_diag_cb = cb;
    }
}

void core_diag(aiot_sysdep_portfile_t *sysdep, int32_t code, uint8_t *data, uint32_t data_len)
{
    g_diag_cb(g_diag_handle, core_log_get_timestamp(sysdep), code, data, data_len);
}
