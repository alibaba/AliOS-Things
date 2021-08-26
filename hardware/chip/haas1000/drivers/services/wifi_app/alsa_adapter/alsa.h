#ifndef __ALSA_H__
#define __ALSA_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * for 48K sample rate, 1 channels, 16bit
 * 96Bytes means 1ms data size.
 * bitrate 48000*16*1/1000 = 768Kbps
 * in 1ms data size is 768*1000/1000/8 = 96 Bytes
 * dma buffer size == trigger size , start delay all dma buffer size.
 * for 48K sample rate, 1 channels, 24bit
 * 192Bytes means 1ms data size.
*/
#define ALSA_SLOT_MS                  (128)
#define ALSA_ONE_MS_PCM_SIZE          (192)
#define ALSA_PCM_DMA_BUFFER_SIZE      (ALSA_ONE_MS_PCM_SIZE*ALSA_SLOT_MS*2)

#ifdef ALSA_FILL_DATA_ON_START
#define ALSA_PCM_THR_TRIGGER_PLAY     (ALSA_PCM_DMA_BUFFER_SIZE)
#else
#define ALSA_PCM_THR_TRIGGER_PLAY     (ALSA_PCM_DMA_BUFFER_SIZE/2)
#endif

#if (ALSA_SLOT_MS < 64)
#define ALSA_PCM_QUEUE_BUFFER_SIZE    (ALSA_PCM_DMA_BUFFER_SIZE*4/ALSA_SLOT_MS*64) /* for sw gain fade out */
#else
#define ALSA_PCM_QUEUE_BUFFER_SIZE    (ALSA_PCM_DMA_BUFFER_SIZE*4)
#endif

#define ALSA_INSTANCE_COUNT           (3)

#ifdef ALIOS_THINGS_RELEASE
#define ALSA_DEFAULT_VOL_DAC          (80)
#define ALSA_DEFAULT_VOL_PERCENT      (90)
#define ALSA_MUTE_VOL                 (2)
#else
#define ALSA_DEFAULT_VOL_DAC          (10)
#define ALSA_DEFAULT_VOL_PERCENT      (50)
#define ALSA_MUTE_VOL                 (0)
#endif

typedef enum {
    ALSA_MODE_UNKNOWN = -1,
    ALSA_MODE_OUT = 0,
    ALSA_MODE_IN = 1,
    ALSA_MODE_ULTRASONIC = (1 << 1),
    ALSA_MODE_MAX,
} alsa_mode_t;

typedef enum {
    /** Signed, 8-bit */
    ALSA_PCM_FORMAT_S8 = 1,
    /** Signed 16-bit, little endian */
    ALSA_PCM_FORMAT_S16_LE = 0,
    /** Signed, 16-bit, big endian */
    ALSA_PCM_FORMAT_S16_BE = 2,
    /** Signed, 24-bit (32-bit in memory), little endian */
    ALSA_PCM_FORMAT_S24_LE,
    /** Signed, 24-bit (32-bit in memory), big endian */
    ALSA_PCM_FORMAT_S24_BE,
    /** Signed, 24-bit, little endian */
    ALSA_PCM_FORMAT_S24_3LE,
    /** Signed, 24-bit, big endian */
    ALSA_PCM_FORMAT_S24_3BE,
    /** Signed, 32-bit, little endian */
    ALSA_PCM_FORMAT_S32_LE,
    /** Signed, 32-bit, big endian */
    ALSA_PCM_FORMAT_S32_BE,
    /** Max of the enumeration list, not an actual format. */
    ALSA_PCM_FORMAT_MAX
} alsa_pcm_format_t;

typedef enum {
    ALSA_PCM_STATE_UNDERRUN,
    ALSA_PCM_STATE_UNDERRUN_EXIT,
    ALSA_PCM_STATE_OVERRUN,
    ALSA_PCM_STATE_OVERRUN_EXIT,
} alsa_pcm_state_t;

typedef void (*alsa_pcm_state_callback_t)(alsa_pcm_state_t state, void *user, void *info);

typedef struct alsa_handle alsa_handle_t;

void alsa_init(void);
alsa_handle_t * alsa_open(alsa_mode_t mode, int sample_rate, int channels, alsa_pcm_format_t format);
int alsa_write(alsa_handle_t * h, uint8_t *buf, uint32_t size);
int alsa_start(alsa_handle_t * h);
int alsa_stop(alsa_handle_t * h);
int alsa_close(alsa_handle_t * h);
void alsa_register_pcm_state_callback(alsa_handle_t * h, alsa_pcm_state_callback_t cb, void * arg);

void alsa_mute_set(void);
void alsa_mute_cancel(void);
void alsa_volume_set(uint8_t vol_percent, uint8_t vol_dac);
uint8_t alsa_volume_get(void);
uint8_t alsa_volume_percent_get(void);
uint8_t alsa_volume_adc_get(uint8_t mic_id);
void alsa_volume_adc_set(uint8_t mic_id, uint8_t vol);
uint8_t alsa_is_playing_state(void);
uint8_t alsa_is_close_state(void);

#ifdef __cplusplus
}
#endif

#endif