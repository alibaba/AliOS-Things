/**
 * Copyright (C) 2019 Alibaba.inc, All rights reserved.
 *
 * @file:    tg_audio.h
 * @brief:   vendor audio apis defination
 * @author:  
 * @date:    2019/12/09
 * @version: 1.0
 */
#ifndef _A7_TG_AUDIO_H_
#define _A7_TG_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif


/** A flag that specifies that the PCM is output or input.
 * Used in @ref tg_pcm_open.
 */
enum tg_pcm_mode {
    TG_PCM_OUT = 0,
    TG_PCM_IN = 1,
};

/** Audio sample format of a PCM.
 * The first letter specifiers whether the sample is signed or unsigned.
 * The letter 'S' means signed. The letter 'U' means unsigned.
 * The following number is the amount of bits that the sample occupies in memory.
 * Following the underscore, specifiers whether the sample is big endian or little endian.
 * The letters 'LE' mean little endian.
 * The letters 'BE' mean big endian.
 * This enumeration is used in the @ref pcm_config structure.
 */
enum tg_pcm_format {
    /** Signed, 8-bit */
    TG_PCM_FORMAT_S8 = 1,
    /** Signed 16-bit, little endian */
    TG_PCM_FORMAT_S16_LE = 0,
    /** Signed, 16-bit, big endian */
    TG_PCM_FORMAT_S16_BE = 2,
    /** Signed, 24-bit (32-bit in memory), little endian */
    TG_PCM_FORMAT_S24_LE,
    /** Signed, 24-bit (32-bit in memory), big endian */
    TG_PCM_FORMAT_S24_BE,
    /** Signed, 24-bit, little endian */
    TG_PCM_FORMAT_S24_3LE,
    /** Signed, 24-bit, big endian */
    TG_PCM_FORMAT_S24_3BE,
    /** Signed, 32-bit, little endian */
    TG_PCM_FORMAT_S32_LE,
    /** Signed, 32-bit, big endian */
    TG_PCM_FORMAT_S32_BE,
    /** Max of the enumeration list, not an actual format. */
    TG_PCM_FORMAT_MAX
};

enum tg_pcm_state {
    TG_PCM_STATE_UNDERRUN,
    TG_PCM_STATE_UNDERRUN_EXIT,
    TG_PCM_STATE_OVERRUN,
    TG_PCM_STATE_OVERRUN_EXIT,
    TG_PCM_STATE_MAX
};

typedef void (*tg_pcm_state_callback_t)(enum tg_pcm_state state, void *user, void *info);

typedef void *tg_pcm_handle_t;

/**
 * @brief Open pcm handle
 * @param[in] mode: specifies that the PCM is output or input
 * @param[in] samplerate: Audio pcm sample rate
 * @param[in] channels: Audio pcm channel count
 * @param[in] format: Audio pcm format
 * @return: pcm handle
 */
tg_pcm_handle_t tg_pcm_open(enum tg_pcm_mode mode, int samplerate, int channels, enum tg_pcm_format format);

/**
 * @brief Set state listener
 * @param[in] pcm: Audio pcm handle
 * @param[in] cbf: state listener handle
 * @param[in] user: listener private data
 */
void tg_pcm_set_state_listener(tg_pcm_handle_t pcm, tg_pcm_state_callback_t cbf, void *user);

/**
 * @brief Start pcm device
 * @param[in] pcm: Audio pcm handle
 * @return: - 0: success; - other: fail
 */
int tg_pcm_start(tg_pcm_handle_t pcm);

/**
 * @brief Write data to pcm device that opened as PCM_OUT mode
 * @param[in] pcm: Audio pcm handle
 * @param[in] buf: PCM data buffer
 * @param[in] size: buffer size in bytes, not samples
 * @return: bytes written, <0 means fail
 */
int tg_pcm_write(tg_pcm_handle_t pcm, void *buf, unsigned int size);

/**
 * @brief Read data from pcm device that opened as PCM_IN mode
 * @param[in] pcm: Audio pcm handle
 * @param[in] buf: PCM data buffer
 * @param[in] size: buffer size in bytes, not samples
 * @return: bytes read, <0 means fail
 */
int tg_pcm_read(tg_pcm_handle_t pcm, void *buf, unsigned int size);

/**
 * @brief Stop pcm device
 * @param[in] pcm: Audio pcm handle
 * @return: - 0 : success; - other: fail
 */
int tg_pcm_stop(tg_pcm_handle_t pcm);

/**
 * @brief Close pcm handle
 * @param[in] pcm: Audio pcm handle
 */
void tg_pcm_close(tg_pcm_handle_t pcm);


#ifdef __cplusplus
}
#endif

#endif /* _A7_TG_AUDIO_H_ */
