#ifndef _CORE_DIAG_H_
#define _CORE_DIAG_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "core_stdinc.h"
#include "core_log.h"

typedef void (*core_diag_callback)(void * diag_handle, uint64_t timestamp, int32_t code, uint8_t *data, uint32_t data_len);

void core_diag_set_cb(void *diag_handle, core_diag_callback cb);
void core_diag(aiot_sysdep_portfile_t *sysdep, int32_t code, uint8_t *data, uint32_t data_len);

#if defined(__cplusplus)
}
#endif

#endif
