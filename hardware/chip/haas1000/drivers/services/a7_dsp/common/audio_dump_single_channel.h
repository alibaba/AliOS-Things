#ifndef _AUDIO_DUMP_SINGLE_CHANNEL_H_
#define _AUDIO_DUMP_SINGLE_CHANNEL_H_
#include "mcu_audio.h"
#ifdef __cplusplus
extern "C" {
#endif

void audio_dump_a7_cb_register(aud_dump_cb_t cbf, void *param);

void audio_dump_a7_cb_unregister(aud_dump_cb_t cbf, void *param);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_DUMP_SINGLE_CHANNEL_H_ */
