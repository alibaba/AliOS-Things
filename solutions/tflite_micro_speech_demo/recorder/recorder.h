#ifndef _RECORDER_H_
#define _RECORDER_H_

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#include "uvoice_types.h"

int32_t recorder_init(void);
int32_t recorder_uninit(void);
int32_t recorder_start(media_format_t fmt, int32_t rate, int32_t channels, int32_t bits, int32_t samples, char *sink);
int32_t recorder_stop(void);
int32_t recorder_get_stream(uint8_t *buf, int32_t read_size);

#ifdef __cplusplus
}  // extern "C"
#endif
#endif // _RECORDER_H_
