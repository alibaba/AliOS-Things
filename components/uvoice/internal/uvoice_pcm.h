/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_PCM_H__
#define __UVOICE_PCM_H__


/*
 * Set volume.
 *
 * Return:
 *   0 -- success or unsupport
 *  -1 -- failed
 */
int uvoice_set_volume(snd_device_t device, int volume);


/*
 * Select audio output/input device, return 0 if device switch unsupported.
 *
 * Return:
 *   0 -- success or unsupport
 *  -1 -- failed
 */
int uvoice_set_path(struct pcm_device *pcm, snd_device_t device);


/*
 * Configure external pa info if used.
 *
 * Return:
 *   0 -- success or no external pa
 *  -1 -- failed
 */
int uvoice_extpa_config(struct external_pa_info *info);


/*
 * Mute audio output/input.
 *
 * Return:
 *   0 -- success or unsupport
 *  -1 -- failed
 */
int uvoice_dev_mute(struct pcm_device *pcm, snd_device_t device, int mute);

/*
 * Called by audio driver to post some message if necessary.
 */
void uvoice_pcm_notify(pcm_message_t msg);

/*
 * Update default pcm params to adapt custom hardware.
 * you can set write/read buffer length by configure
 * pcm->config.period_size
 *
 * Return:
 *   0 -- success
 *  -1 -- failed
 */
int uvoice_pcm_setup(struct pcm_device *pcm);

/*
 * Open pcm device with configured pcm.
 *
 * Return:
 *   0 -- success
 *  -1 -- failed
 */
int uvoice_pcm_open(struct pcm_device *pcm);

/*
 * Read data in. Block reading if data not ready.
 *
 * Return read length, or negative if failed.
 */
int uvoice_pcm_read(struct pcm_device *pcm, uint8_t *buffer, int nbytes);

/*
 * Block write if free dma buffer not ready, otherwise,
 * please return after copied.
 *
 * Return writen length, or negative if failed.
 *
 */
int uvoice_pcm_write(struct pcm_device *pcm, uint8_t *buffer, int nbytes);

int uvoice_pcm_silence(struct pcm_device *pcm);

/*
 * Flush remaining data in dma buffer
 */
int uvoice_pcm_flush(struct pcm_device *pcm);

/*
 * close pcm device, release dma
 *
 * Return:
 *   0 -- success
 *  -1 -- failed
 *
 */
int uvoice_pcm_close(struct pcm_device *pcm);

/*
 * Init pcm module
 *
 * Return:
 *   0 -- success
 *  -1 -- failed
 *
 */
int uvoice_pcm_init(void);

#endif
