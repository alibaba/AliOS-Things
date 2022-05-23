// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
// All rights reserved.

#ifndef _ESP_EQUALIZER_H
#define _ESP_EQUALIZER_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @brief      Initialize the equalizer handle
*
* @param      nch     The audio channel number
* @param      g_rate  The audio sample rate. Four sample rates are supported: 11025Hz, 22050Hz, 44100Hz and 48000Hz.
* @param      n_band  The number of audio sub-bands. Fixed number of 10 sub-bands is supported and this value should be set to 10.
* @param      use_xmms_original_freqs  Currently should be set 0
*
* @return     The equalizer handle.
*/
void *esp_equalizer_init(int nch, int g_rate, int n_band, int use_xmms_original_freqs);

/**
* @brief     Uninitialize the equalizer handle.
*
* @param      handle   The the equalizer handle
*/
void esp_equalizer_uninit(void *handle);

/**
* @brief      Process the data through the equalizer
*
* @param      handle   The the equalizer handle
* @param      pcm_buf  The audio pcm input & output buffer
* @param      length   The length of current bytes in pcm_buf
* @param      g_rate   The audio sample rate. Four sample rates are supported: 11025Hz, 22050Hz, 44100Hz and 48000Hz.
* @param      nch      The audio channel number
*
* @return     Length of pcm_buf after processing
*/
int esp_equalizer_process(void *handle, unsigned char *pcm_buf, int length, int g_rate, int nch);

/**
* @brief      Set the number of sub-bands for the equalizer
*
* @param      handle  The the equalizer handle
* @param      value   The audio sub-bands gain. unit:db. 0 means no gain.
* @param      index   The index of audio sub-bands. e.g. 0, 1, 2, 3, 4, 5, 6, 7, 8, 9.
* @param      nch     The audio channel number
*/
void esp_equalizer_set_band_value(void *handle, float value, int index, int nch);

/**
* @brief      Get the number of the equalizer sub-bands
*
* @param      handle   The the equalizer handle
*
* @return     The number of the equalizer sub-bands
*/
int esp_equalizer_get_band_count(void *handle);

/**
* @brief      Get the value of the equalizer sub-bands
*
* @param      handle  The the equalizer handle
* @param      index   The index of audio sub-bands. Currently only support 10 sub-bands, so it should be 0-9.
* @param      nch     The audio channel number
*
* @return     The number of the equalizer sub-bands
*/
float esp_equalizer_get_band_value(void *handle, int index, int nch);

#ifdef __cplusplus
}
#endif

#endif

