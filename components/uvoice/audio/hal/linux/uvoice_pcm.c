/*
 * uVoice audio hardware adapt layer
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <alsa/asoundlib.h>
#include <uvoice_audio.h>


static int alsa_pcm_open(struct pcm_device *pcm)
{
	struct pcm_config *config;
	snd_pcm_t *pcm_handle;
	snd_pcm_stream_t stream;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_sw_params_t *sw_params;
	snd_pcm_uframes_t period_size;
	snd_pcm_uframes_t buffer_size;
	snd_pcm_uframes_t start_threshold;
	snd_pcm_access_t access;
	snd_pcm_format_t format;
	unsigned int sample_rate;
	unsigned int channels;
	unsigned int periods;
	unsigned int buffer_time;
	unsigned int period_time;
	unsigned int period_count;
	int dir = 0;
	int ret;

	config = &pcm->config;

	if (pcm->dir == PCM_OUT)
		stream = SND_PCM_STREAM_PLAYBACK;
	else if (pcm->dir == PCM_IN)
		stream = SND_PCM_STREAM_CAPTURE;
	else
		return -1;

	ret = snd_pcm_open(&pcm_handle, "default", stream, 0);
	if (ret < 0) {
		snd_err("open pcm failed %d!\n", ret);
		goto __exit;
	}

	ret = snd_pcm_hw_params_malloc(&hw_params);
	if (ret < 0) {
		snd_err("alloc hw params failed %d!\n", ret);
		goto __exit1;
	}

	ret = snd_pcm_hw_params_any(pcm_handle, hw_params);
	if (ret < 0) {
		snd_err("init hw params failed %d!\n", ret);
		goto __exit2;
	}

	access = SND_PCM_ACCESS_RW_INTERLEAVED;
	ret = snd_pcm_hw_params_set_access(pcm_handle, hw_params, access);
	if (ret < 0) {
		snd_err("set access failed %d!\n", ret);
		goto __exit2;
	}

	switch (config->format) {
	case PCM_FORMAT_S16_LE:
		format = SND_PCM_FORMAT_S16_LE;
		break;
	case PCM_FORMAT_S24_LE:
		format = SND_PCM_FORMAT_S24_LE;
		break;
	case PCM_FORMAT_S24_3LE:
		format = SND_PCM_FORMAT_S24_3LE;
		break;
	case PCM_FORMAT_S8:
		format = SND_PCM_FORMAT_S8;
		break;
	default:
		ret = -1;
		snd_err("unsupport format %d\n", config->format);
		goto __exit2;
	}

	ret = snd_pcm_hw_params_set_format(pcm_handle, hw_params,
		format);
	if (ret < 0) {
		snd_err("set format failed %d!\n", ret);
		goto __exit2;
	}

	sample_rate = config->rate;
	dir = 0;
	ret = snd_pcm_hw_params_set_rate_near(pcm_handle, hw_params, &sample_rate, &dir);
	if (ret < 0) {
		snd_err("set sample rate failed %d!\n", ret);
		goto __exit2;
	}

	channels = config->channels;
	ret = snd_pcm_hw_params_set_channels(pcm_handle, hw_params, channels);
	if (ret < 0) {
		snd_err("set channels failed %d!\n", ret);
		goto __exit2;
	}

	period_size = config->period_size;
	dir = 0;
	ret = snd_pcm_hw_params_set_period_size_near(pcm_handle, hw_params, &period_size, &dir);
	if (ret < 0) {
		snd_err("set period size failed %d!\n", ret);
		goto __exit2;
	}

	dir = 0;
	ret = snd_pcm_hw_params_get_period_size(hw_params, &period_size, &dir);
	if (ret < 0) {
		snd_err("get period size failed %d!\n", ret);
		goto __exit2;
	}
	snd_info("period_size %lu\n", period_size);

	periods = config->period_count;
	buffer_size = period_size * periods;
	ret = snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hw_params, &buffer_size);
	if (ret < 0) {
		snd_err("set buffer size failed %d!\n", ret);
		goto __exit2;
	}

	ret = snd_pcm_hw_params(pcm_handle, hw_params);
	if (ret < 0) {
		snd_err("set hw params failed %d!\n", ret);
		goto __exit2;
	}

	ret = snd_pcm_hw_params_get_channels(hw_params, &channels);
	if (ret < 0) {
		snd_err("get channels failed %d!\n", ret);
		goto __exit2;
	}
	snd_info("channels %u\n", channels);

	dir = 0;
	ret = snd_pcm_hw_params_get_rate(hw_params, &sample_rate, &dir);
	if (ret < 0) {
		snd_err("get rate failed %d!\n", ret);
		goto __exit2;
	}
	config->rate = sample_rate;
	snd_info("sample_rate %u\n", sample_rate);

	ret = snd_pcm_hw_params_get_sbits(hw_params);
	if (ret < 0) {
		snd_err("get sbits failed !\n");
		goto __exit2;
	}
	snd_info("sbits %d\n", ret);

	ret = snd_pcm_hw_params_get_format(hw_params, &format);
	if (ret < 0) {
		snd_err("get format failed !\n");
		goto __exit2;
	}
	snd_info("format %d\n", format);

	ret = snd_pcm_hw_params_get_access(hw_params, &access);
	if (ret < 0) {
		snd_err("get access failed !\n");
		goto __exit2;
	}
	snd_info("access %d\n", access);

	periods = 0;
	dir = 0;
	ret = snd_pcm_hw_params_get_periods(hw_params, &periods, &dir);
	if (ret < 0) {
		snd_err("get periods failed %d!\n", ret);
		goto __exit2;
	}
	config->period_count = periods;
	snd_info("periods %u\n", periods);

	ret = snd_pcm_hw_params_get_buffer_size(hw_params, &buffer_size);
	if (ret < 0) {
		snd_err("get buffer size failed %d!\n", ret);
		goto __exit2;
	}
	snd_info("buffer_size %lu\n", buffer_size);

	ret = snd_pcm_sw_params_malloc(&sw_params);
	if (ret < 0) {
		snd_err("alloc sw params failed %d!\n", ret);
		goto __exit1;
	}

	ret = snd_pcm_sw_params_current(pcm_handle, sw_params);
	if (ret < 0) {
		snd_err("get sw params failed %d!\n", ret);
		goto __exit3;
	}	

	start_threshold = buffer_size;
	ret = snd_pcm_sw_params_set_start_threshold(pcm_handle, sw_params, start_threshold);
	if (ret < 0) {
		snd_err("set start threshold failed %d!\n", ret);
		goto __exit3;
	}

	ret = snd_pcm_sw_params(pcm_handle, sw_params);
	if (ret < 0) {
		snd_err("set sw params failed %d!\n", ret);
		goto __exit3;
	}

	ret = snd_pcm_sw_params_get_start_threshold(sw_params, &start_threshold);
	if (ret < 0) {
		snd_err("get start threshold failed %d!\n", ret);
		goto __exit3;
	}
	snd_info("start_threshold %lu\n", start_threshold);

	snd_pcm_sw_params_free(sw_params);
	snd_pcm_hw_params_free(hw_params);
	pcm->private_data = pcm_handle;
	return 0;

__exit3:
	snd_pcm_sw_params_free(sw_params);
__exit2:
	snd_pcm_hw_params_free(hw_params);
__exit1:
	snd_pcm_close(pcm_handle);
__exit:
	return -1;
}

static int alsa_pcm_drain(struct pcm_device *pcm)
{
	snd_pcm_t *pcm_handle;

	pcm_handle = (snd_pcm_t *)pcm->private_data;
	if (!pcm_handle) {
		snd_err("pcm_handle null !\n");
		return -1;
	}

	return snd_pcm_drain(pcm_handle);
}

static int alsa_pcm_close(struct pcm_device *pcm)
{
	snd_pcm_t *pcm_handle;

	pcm_handle = (snd_pcm_t *)pcm->private_data;
	if (!pcm_handle) {
		snd_err("pcm_handle null !\n");
		return -1;
	}

	return snd_pcm_close(pcm_handle);
}

static int alsa_pcm_read(struct pcm_device *pcm, uint8_t *buffer, int nbytes)
{
	snd_pcm_t *pcm_handle;
	snd_pcm_uframes_t frames;
	int ret;

	pcm_handle = (snd_pcm_t *)pcm->private_data;
	if (!pcm_handle) {
		snd_err("pcm_handle null !\n");
		return -1;
	}

	frames = snd_pcm_bytes_to_frames(pcm_handle, nbytes);
	while ((ret = snd_pcm_readi(pcm_handle, buffer, frames)) < 0) {
		if (ret == -EPIPE) {
			snd_err("overrun occurued\n");
			snd_pcm_prepare(pcm_handle);
		} else if (ret < 0) {
			snd_err("writei error: %s\n", snd_strerror(ret));
		}
	}
	return nbytes;
}

static int alsa_pcm_write(struct pcm_device *pcm, uint8_t *buffer, int nbytes)
{
	snd_pcm_t *pcm_handle;
	snd_pcm_uframes_t frames;
	int ret;

	pcm_handle = (snd_pcm_t *)pcm->private_data;
	if (!pcm_handle) {
		snd_err("pcm_handle null !\n");
		return -1;
	}

	frames = snd_pcm_bytes_to_frames(pcm_handle, nbytes);

	while ((ret = snd_pcm_writei(pcm_handle, buffer, frames)) < 0) {
		if (ret == -EPIPE) {
			snd_err("underrun occurued\n");
			snd_pcm_prepare(pcm_handle);
		} else if (ret < 0) {
			snd_err("writei error: %s\n", snd_strerror(ret));
		}
	}
	return nbytes;
}

int uvoice_set_volume(snd_device_t device, int volume)
{
	return 0;
}

int uvoice_set_path(struct pcm_device *pcm, snd_device_t device)
{
	if (!pcm) {
		snd_err("pcm null !\n");
		return -1;
	}

	return 0;
}

int uvoice_dev_mute(struct pcm_device *pcm, snd_device_t device, int mute)
{
	if (!pcm) {
		snd_err("pcm null !\n");
		return -1;
	}

	return 0;
}

int uvoice_pcm_setup(struct pcm_device *pcm)
{
	if (!pcm) {
		snd_err("pcm null !\n");
		return -1;
	}

	if (pcm->dir == PCM_OUT) {
		pcm->config.period_count = 4;
		pcm->config.period_size = 960;
		//pcm->config.rate = 48000;
	} else if (pcm->dir == PCM_IN) {
		pcm->config.period_count = 4;
		pcm->config.period_size = 640;
	}

	pcm->state = PCM_STATE_SETUP;
	return 0;
}

int uvoice_pcm_open(struct pcm_device *pcm)
{
	snd_pcm_t *pcm_handle;
	struct pcm_config *config;
	snd_pcm_hw_params_t *hw_params;
	int ret;

	if (!pcm) {
		snd_err("pcm null !\n");
		return -1;
	}

	if (pcm->state != PCM_STATE_SETUP) {
		snd_err("pcm not setup !\n");
		goto __exit;
	}

	config = &pcm->config;

	snd_debug("rate %d channels %d format %d period_size %d period_count %d dir %u\n",
		config->rate,
		config->channels,
		config->format,
		config->period_size, config->period_count, pcm->dir);

	if (alsa_pcm_open(pcm)) {
		snd_err("open alsa pcm failed !\n");
		return -1;
	}

	pcm->state = PCM_STATE_OPEN;

__exit:
	return 0;
}

int uvoice_pcm_read(struct pcm_device *pcm, uint8_t *buffer, int nbytes)
{
	int ret = 0;

	if (!pcm) {
		snd_err("pcm null !\n");
		return -1;
	}

	if (pcm->state == PCM_STATE_OPEN)
		pcm->state = PCM_STATE_RUNNING;

	if (pcm->state == PCM_STATE_RUNNING) {
		ret = alsa_pcm_read(pcm, buffer, nbytes);
	} else {
		snd_err("pcm not running !\n");
		return -1;
	}

	return ret;
}

int uvoice_pcm_write(struct pcm_device *pcm, uint8_t *buffer, int nbytes)
{
	int ret = 0;

	if (!pcm) {
		snd_err("pcm null !\n");
		return -1;
	}

	if (pcm->state == PCM_STATE_OPEN)
		pcm->state = PCM_STATE_RUNNING;

	if (pcm->state == PCM_STATE_RUNNING) {
		ret = alsa_pcm_write(pcm, buffer, nbytes);
		if (ret < 0) {
			snd_err("write failed %d!\n", ret);
			ret = -1;
		}
	} else {
		snd_err("pcm not running !\n");
		return -1;
	}

	return ret;
}

int uvoice_pcm_silence(struct pcm_device *pcm)
{
	return 0;
}

int uvoice_pcm_flush(struct pcm_device *pcm)
{
	if (!pcm) {
		snd_err("pcm null !\n");
		return -1;
	}

	if (pcm->dir == PCM_OUT)
		alsa_pcm_drain(pcm);

	return 0;
}

int uvoice_pcm_close(struct pcm_device *pcm)
{
	if (!pcm) {
		snd_err("pcm null !\n");
		return -1;
	}

	if (pcm->state == PCM_STATE_CLOSED) {
		snd_err("close already, ignore\n");
		goto __exit;
	}

	alsa_pcm_close(pcm);

	pcm->state = PCM_STATE_CLOSED;
	snd_debug("pcm close\n");

__exit:
	return 0;
}