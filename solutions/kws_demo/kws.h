#ifndef __KWS_H__
#define __KWS_H__

#include <stdint.h>
#include <stdio.h>

extern int audio_install_codec_driver();

int32_t kws_init(void);
int32_t kws_uninit(void);

#endif
