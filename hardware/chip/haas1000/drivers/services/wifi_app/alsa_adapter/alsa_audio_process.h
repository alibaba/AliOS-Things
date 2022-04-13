#ifndef __ALSA_AUDIO_PROCESS_H__
#define __ALSA_AUDIO_PROCESS_H__

#ifdef __cplusplus
extern "C" {
#endif

int alsa_audio_process_oepn(int sample_rate, int channel_num, int bits, uint32_t dma_data_size);
int alsa_audio_process_close(void);
int alsa_audio_process(uint8_t *buf, uint32_t len);

void alsa_audio_process_enter_online_debug(void);
void alsa_audio_process_level_online_debug(void);
uint8_t alsa_audio_process_is_online_debug(void);

#ifdef AUDIO_PROCESS_CFG_DIFF_FOR_ODM
void alsa_audio_process_set_odm_cfg(char * odm_name, uint32_t odm_name_len, uint8_t odm_type);
uint8_t alsa_audio_process_get_cfg_index(void);
#endif

void alsa_audio_process_bypass_eq_set(uint8_t enable);
uint8_t alsa_audio_process_bypass_eq_get(void);

#ifdef ALSA_INSTANCE_SKIP_AUDIO_PROCESS
void alsa_skip_audio_process_set(uint8_t val);
#endif

#ifdef __cplusplus
}
#endif

#endif