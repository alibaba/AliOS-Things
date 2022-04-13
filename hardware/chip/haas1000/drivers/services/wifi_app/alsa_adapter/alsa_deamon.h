#ifndef __ALSA_DEAMON_H__
#define __ALSA_DEAMON_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ALSA_SRC_PROCESS_EN
#define ALSA_SAMPLERATE AUD_SAMPRATE_48000
#else
#define ALSA_SAMPLERATE AUD_SAMPRATE_16000
#endif

#ifdef ALSA_STEREO_TO_MONO_EN
#define ALSA_CHANNELS AUD_CHANNEL_NUM_1
#else
#define ALSA_CHANNELS AUD_CHANNEL_NUM_1
#endif

#ifdef ALSA_24BIT_PROCESS_EN
#define ALSA_BITS AUD_BITS_24
#else
#define ALSA_BITS AUD_BITS_16
#endif

typedef enum
{
    ALSA_STATE_CLOSE = 0,
    ALSA_STATE_OPEN,
    ALSA_STATE_START,
    ALSA_STATE_STOP,
} alsa_state_t;

typedef uint32_t (*ALSA_GET_USER_DATA_CB)(uint8_t *, uint32_t);
typedef int (*ALSA_USER_DATA_LEN_CHANGED_CB)(uint32_t);

void alsa_deamon_init(ALSA_GET_USER_DATA_CB alsa_get_user_data_cb, ALSA_USER_DATA_LEN_CHANGED_CB alsa_user_data_len_changed_cb);

int alsa_open_for_user(uint8_t user_id);
int alsa_start_for_user(uint8_t user_id);
int alsa_stop_for_user(uint8_t user_id);
int alsa_close_for_user(uint8_t user_id);

int alsa_check_only_ultra_playing(void);
int alsa_modify_adc_gain_for_ultra(uint8_t only_ultra_playing);

#ifdef __cplusplus
}
#endif

#endif /* __ALSA_DEAMON_H__ */