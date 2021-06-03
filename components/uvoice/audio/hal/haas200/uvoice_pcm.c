/*
 * uVoice audio hardware adapt layer
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ulog/ulog.h"

#include <uvoice_audio.h>

int uvoice_set_volume(snd_device_t device, int volume)
{
    return 0;
}

int uvoice_set_path(struct pcm_device *pcm, snd_device_t device)
{
    return 0;
}

int uvoice_dev_mute(struct pcm_device *pcm, snd_device_t device, int mute)
{
    return 0;
}

void uvoice_pcm_notify(pcm_message_t msg)
{

}

int uvoice_pcm_setup(struct pcm_device *pcm)
{
    return 0;
}

int uvoice_pcm_open(struct pcm_device *pcm)
{
    return 0;
}

int uvoice_pcm_read(struct pcm_device *pcm, uint8_t *buffer, int nbytes)
{
    int ret = -1;

    return ret;
}

int uvoice_pcm_write(struct pcm_device *pcm, uint8_t *buffer, int nbytes)
{
    int ret = -1;
    return ret;
}

int uvoice_extpa_config(struct external_pa_info *info)
{
    return 0;
}

int uvoice_pcm_silence(struct pcm_device *pcm)
{
    return 0;
}

int uvoice_pcm_flush(struct pcm_device *pcm)
{
    return 0;
}

int uvoice_pcm_close(struct pcm_device *pcm)
{
    return 0;
}

int uvoice_pcm_init(void)
{
    return 0;
}
