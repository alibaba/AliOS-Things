/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_recorder.h"

#include "uvoice_os.h"
#include "uvoice_config.h"
#include "uvoice_common.h"
#include "uvoice_audio.h"
#include "uvoice_record.h"
#include "uvoice_resampler.h"

#include "audio_common.h"
#include "audio_stream.h"


static uvoice_recorder_t *g_mrecorder;

static int recorder_clr_sink(void);

static int recorder_state_post(recorder_t *recorder,
	recorder_state_t state)
{
	return 0;//uvoice_event_post(UVOICE_EV_RECORDER,
	//	UVOICE_CODE_RECORDER_STATE, state);
}

static void recorder_reset(recorder_t *recorder)
{
	recorder->stop = 0;
	recorder->error = 0;
	recorder->record_len = 0;
}

static int recorder_encode_default(void *priv,
	uint8_t *buffer, int nbytes)
{
	return nbytes;
}

static int recorder_action_default(void *priv,
	recorder_action_t action, void *arg)
{
	return 0;
}

static media_type_t recorder_type_parse(char *sink)
{
	if (!sink)
		return MEDIA_TYPE_UNKNOWN;
	else if (!strncmp(sink, "fs:", strlen("fs:")))
		return MEDIA_TYPE_FILE;
	else if (!strncmp(sink, "pt:", strlen("pt:")))
		return MEDIA_TYPE_FLASH;
	return MEDIA_TYPE_UNKNOWN;
}

static bool recorder_format_assert(media_format_t format)
{
	switch (format) {
	case MEDIA_FMT_WAV:
	case MEDIA_FMT_PCM:
	case MEDIA_FMT_SPX:
	case MEDIA_FMT_AMR:
	case MEDIA_FMT_AMRWB:
	case MEDIA_FMT_OPS:
		return true;
	default:
		return false;
	}
	return false;
}

static int recorder_stack_size(media_format_t format)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;
	int size = 4096;

	if (!recorder)
		return size;

	switch (format) {
	case MEDIA_FMT_OPS:
		size = 32768;
		break;
	case MEDIA_FMT_SPX:
		size = 8192;
		break;
	case MEDIA_FMT_AMR:
	case MEDIA_FMT_AMRWB:
		size = 20 * 1024;
	}

	if (recorder->ns_enable || recorder->ec_enable)
		size = 12 * 1024;

	if (recorder->agc_enable)
		size = 10 * 1024;

	return size;
}

static int recorder_bitrate_default(media_format_t format)
{
	if (format == MEDIA_FMT_MP3)
		return 128000;
	else if (format == MEDIA_FMT_OPS)
		return 27800;
	return 100000;
}

static int recorder_param_set(media_pcminfo_t *info,
	int rate, int channels, int bits, int frames)
{
	if (!info) {
		M_LOGE("info null !\n");
		return -1;
	}

	if (!pcm_rate_valid(rate)) {
		M_LOGE("sample rate invalid !\n");
		return -1;
	}

	if (!pcm_bits_valid(bits)) {
		M_LOGE("bits invalid !\n");
		return -1;
	}

	if (!pcm_channel_valid(channels)) {
		M_LOGE("channels invalid !\n");
		return -1;
	}

	if (frames < 0) {
		M_LOGE("frames invalid !\n");
		return -1;
	}

	info->rate = rate;
	info->channels = channels;
	info->bits = bits;
	info->frames = frames;
	return 0;
}

static void recorder_task(void *arg)
{
	recorder_t *recorder = (recorder_t *)arg;
	media_encoder_t *mencoder;
	media_packer_t *mpacker;
	uint8_t *data_buffer;
	int data_buffer_size;
	int ret;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return;
	}

	mencoder = recorder->mencoder;
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		return;
	}

	mpacker = recorder->mpacker;
	if (!mpacker) {
		M_LOGE("mpacker null !\n");
		return;
	}

	os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);

	if (recorder->state == RECORDER_STAT_READY ||
		recorder->state == RECORDER_STAT_STOP) {
		if (mencoder->action(mencoder, RECORDER_START, NULL)) {
			M_LOGE("start encoder failed !\n");
			os_mutex_unlock(recorder->lock);
			return;
		}

		if (recorder->state == RECORDER_STAT_STOP)
			M_LOGI("stop->running\n");
		else
			M_LOGI("ready->running\n");
		recorder->state = RECORDER_STAT_RUNNING;
		recorder_state_post(recorder, recorder->state);
	}

	if (recorder->state != RECORDER_STAT_RUNNING) {
		M_LOGE("recorder not running ! state %d\n",
			recorder->state);
		os_mutex_unlock(recorder->lock);
		return;
	}

	os_mutex_unlock(recorder->lock);

	while (1) {
		os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
		if (recorder->stop) {
			if (recorder->state == RECORDER_STAT_RUNNING) {
				mencoder->action(mencoder, RECORDER_STOP, NULL);
				if (mpacker->action)
					mpacker->action(mpacker, RECORDER_STOP, NULL);
				in_stream_stop(recorder->in);
				recorder->state = RECORDER_STAT_STOP;
				recorder->stop = 0;
				recorder_state_post(recorder, recorder->state);
				M_LOGI("running->stop\n");
				os_mutex_unlock(recorder->lock);
				if (recorder->error) {
					M_LOGE("recorder error, release it\n");
					recorder_clr_sink();
				}
				break;
			}
		}

		if (!in_stream_configured(recorder->in)) {
			media_pcminfo_t *info = &recorder->pcm_info;
			media_pcminfo_t temp_info;

			memcpy(&temp_info, info, sizeof(temp_info));
			if (in_stream_configure(recorder->in, &temp_info)) {
				M_LOGE("configure stream failed !\n");
				recorder->stop = 1;
				recorder->error = 1;
				os_mutex_unlock(recorder->lock);
				continue;
			}

			recorder->src_rate = temp_info.rate;
			if (temp_info.rate != info->rate) {
#ifdef UVOICE_RESAMPLE_ENABLE
				if (!recorder->resampler) {
					uvoice_resampler_create(&recorder->resampler, recorder->src_rate,
						recorder->dst_rate, temp_info.channels, temp_info.bits);
					if (!recorder->resampler) {
						M_LOGE("create resampler failed !\n");
						recorder->stop = 1;
						recorder->error = 1;
						os_mutex_unlock(recorder->lock);
						continue;
					}
				} else {
					uvoice_resampler_update(recorder->resampler, recorder->src_rate,
						recorder->dst_rate, temp_info.channels, temp_info.bits);
				}
#endif
			}
		}

		ret = in_stream_read(recorder->in, recorder->buffer,
			recorder->buffer_size);
		if (ret <= 0) {
			M_LOGE("read failed %d!\n", ret);
			recorder->stop = 1;
			recorder->error = 1;
			os_mutex_unlock(recorder->lock);
			continue;
		} else if (ret != recorder->buffer_size) {
			M_LOGW("read %d ret %d\n",
				recorder->buffer_size, ret);
		}

		data_buffer_size = ret;
		data_buffer = recorder->buffer;

#ifdef UVOICE_RESAMPLE_ENABLE
		if (uvoice_resampler_process(recorder->resampler, recorder->buffer, ret,
			&data_buffer, &data_buffer_size)) {
			M_LOGE("resample error\n");
		}
#endif

		if (data_buffer_size <= 0) {
			M_LOGE("data_buffer_size err %d\n", data_buffer_size);
			os_mutex_unlock(recorder->lock);
			continue;
		}

		recorder->buffer_dirty_size = data_buffer_size;
		recorder->record_len += data_buffer_size;

		if (!mencoder->header_pack) {
			if (mencoder->header_gen &&
				mencoder->header_gen(mencoder, &recorder->pcm_info)) {
				M_LOGE("init header failed !\n");
				recorder->stop = 1;
				recorder->error = 1;
				os_mutex_unlock(recorder->lock);
				continue;
			}

			if (mencoder->header && mencoder->header_size > 0) {
				M_LOGD("header pack\n");
				ret = mpacker->pack(mpacker,
					mencoder->header, mencoder->header_size);
				if (ret < 0) {
					M_LOGE("pack header failed !\n");
					snd_free(mencoder->header);
					mencoder->header = NULL;
					mencoder->header_size = 0;
					recorder->stop = 1;
					os_mutex_unlock(recorder->lock);
					continue;			
				} else if (ret != mencoder->header_size) {
					M_LOGW("pack %d ret %d\n", mencoder->header_size, ret);
				}

				if (mencoder->header_cplt) {
					snd_free(mencoder->header);
					mencoder->header = NULL;
					mencoder->header_size = 0;
				}
			}
			mencoder->header_pack = 1;
		}
		os_mutex_unlock(recorder->lock);

		ret = mencoder->encode(mencoder,
			data_buffer, recorder->buffer_dirty_size);
		if (ret <= 0) {
			M_LOGE("encode failed %d!\n", ret);
			os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
			recorder->stop = 1;
			recorder->error = 1;
			os_mutex_unlock(recorder->lock);
			continue;			
		}
		recorder->buffer_dirty_size = ret;

		ret = mpacker->pack(mpacker, data_buffer,
				recorder->buffer_dirty_size);
		if (ret < 0) {
			M_LOGE("pack failed %d!\n", ret);
			os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
			recorder->stop = 1;
			recorder->error = 1;
			os_mutex_unlock(recorder->lock);
		} else if (ret == 0) {
			M_LOGW("pack end\n");
			os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
			recorder->stop = 1;
			os_mutex_unlock(recorder->lock);
		} else if (ret != recorder->buffer_dirty_size) {
			M_LOGW("pack %d ret %d\n",
				recorder->buffer_dirty_size, ret);
		}
	}

	os_sem_signal(recorder->cplt_sem);
	M_LOGD("exit\n");
}

static int recorder_get_stream(uint8_t *buffer, int nbytes)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;
	media_encoder_t *mencoder;
	uint8_t *data_buffer;
	uint8_t *data_ptr;
	int data_size;
	int ret;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	if (!buffer) {
		M_LOGE("buffer null !\n");
		return -1;
	}

	os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
	mencoder = recorder->mencoder;
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		os_mutex_unlock(recorder->lock);
		return -1;
	}

	if (recorder->state == RECORDER_STAT_READY ||
		recorder->state == RECORDER_STAT_STOP) {
		if (mencoder->action(mencoder, RECORDER_START, NULL)) {
			M_LOGE("start encoder failed !\n");
			os_mutex_unlock(recorder->lock);
			return -1;
		}

		if (recorder->state == RECORDER_STAT_STOP)
			M_LOGI("stop->running\n");
		else
			M_LOGI("ready->running\n");
		recorder->state = RECORDER_STAT_RUNNING;
		
	}

	if (recorder->state != RECORDER_STAT_RUNNING) {
		M_LOGE("recorder not running ! state %d\n",
			recorder->state);
		os_mutex_unlock(recorder->lock);
		return -1;
	}

	if (!in_stream_configured(recorder->in)) {
		media_pcminfo_t *info = &recorder->pcm_info;
		media_pcminfo_t temp_info;

		if (info->frames <= 0) {
			info->frames = nbytes / (info->channels * (info->bits >> 3));
			mencoder->frames = info->frames;
		}

		memcpy(&temp_info, info, sizeof(temp_info));
		in_stream_configure(recorder->in, &temp_info);

		recorder->src_rate = temp_info.rate;
		if (temp_info.rate != info->rate) {
#ifdef UVOICE_RESAMPLE_ENABLE
			uvoice_resampler_create(&recorder->resampler, recorder->src_rate,
				recorder->dst_rate, temp_info.channels, temp_info.bits);
			if (!recorder->resampler) {
				M_LOGE("create resampler failed !\n");
				os_mutex_unlock(recorder->lock);
				return -1;
			}
#endif
		}
	}

	data_size = (nbytes * recorder->src_rate) / recorder->dst_rate;
	if (data_size > nbytes) {
		if (!recorder->rec_buffer) {
			recorder->rec_buffer = snd_zalloc(data_size, AFM_MAIN);
			if (!recorder->rec_buffer) {
				M_LOGE("alloc record buffer failed !\n");
				os_mutex_unlock(recorder->lock);
				return -1;
			}
		}
		data_buffer = recorder->rec_buffer;
	} else {
		data_buffer = buffer;
	}

	ret = in_stream_read(recorder->in, data_buffer, data_size);
	if (ret <= 0) {
		M_LOGE("read failed !\n");
		os_mutex_unlock(recorder->lock);
		return -1;
	} else if (ret != data_size) {
		M_LOGW("read %d ret %d\n", data_size, ret);
	}

	data_ptr = data_buffer;
	data_size = ret;

#ifdef UVOICE_RESAMPLE_ENABLE
	if (uvoice_resampler_process(recorder->resampler, data_buffer, ret,
		&data_ptr, &data_size)) {
		M_LOGE("resample error\n");
	}

	if (data_size <= 0) {
		M_LOGE("data_size err %d\n", data_size);
		os_mutex_unlock(recorder->lock);
		return -1;
	}
#endif

	recorder->buffer_dirty_size = data_size;

	ret = mencoder->encode(mencoder,
		data_ptr, recorder->buffer_dirty_size);
	if (ret <= 0) {
		M_LOGE("encode failed !\n");
		os_mutex_unlock(recorder->lock);
		return -1;
	}

	if (data_ptr != buffer)
		memcpy(buffer, data_ptr, ret);

	os_mutex_unlock(recorder->lock);
	return ret;
}

static int recorder_start(void)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);

	if (recorder->type == MEDIA_TYPE_STREAM) {
		M_LOGE("media type stream !\n");
		os_mutex_unlock(recorder->lock);
		return -1;
	}

	if (recorder->state != RECORDER_STAT_READY &&
		recorder->state != RECORDER_STAT_STOP) {
		M_LOGE("recorder not ready/stop !\n");
		os_mutex_unlock(recorder->lock);
		return -1;
	}
	os_mutex_unlock(recorder->lock);

	M_LOGD("start record task\n");

	os_task_create(&recorder->task,
		"uvoice_record_task",
		recorder_task,
		recorder,
		recorder_stack_size(recorder->format),
		UVOICE_TASK_PRI_HIGHER);

	return 0;
}

static int recorder_stop(void)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
	if (recorder->type == MEDIA_TYPE_STREAM) {
		M_LOGE("stream stop not required !\n");
		os_mutex_unlock(recorder->lock);
		return -1;
	}

	if (recorder->state != RECORDER_STAT_RUNNING) {
		M_LOGE("recorder can't stop ! state %d\n",
			recorder->state);
		os_mutex_unlock(recorder->lock);
		return -1;
	}

	if (!recorder->stop) {
		recorder->stop = 1;
		os_mutex_unlock(recorder->lock);
		if (os_sem_wait(recorder->cplt_sem, 5000)) {
			M_LOGE("wait stop timeout !\n");
			return -1;
		}

		os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
		if (recorder->state != RECORDER_STAT_STOP) {
			M_LOGE("recorder stop failed ! state %d\n",
				recorder->state);
			os_mutex_unlock(recorder->lock);
			return -1;
		} else {
			M_LOGD("recorder stop\n");
		}
	}

	os_mutex_unlock(recorder->lock);
	return 0;
}

static int recorder_get_state(recorder_state_t *state)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	if (!state) {
		M_LOGE("arg null !\n");
		return -1;
	}

	os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
	*state = recorder->state;
	os_mutex_unlock(recorder->lock);
	return 0;
}

static int recorder_get_position(int *position)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;
	media_pcminfo_t *info;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	if (!position) {
		M_LOGE("arg null !\n");
		return -1;
	}

	info = &recorder->pcm_info;

	os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
	if (recorder->state == RECORDER_STAT_RUNNING ||
		recorder->state == RECORDER_STAT_STOP) {
		*position = (recorder->record_len /
			(info->channels * (info->bits >> 3))) / info->rate;
	}

	os_mutex_unlock(recorder->lock);
	return 0;
}

static int recorder_set_sink(media_format_t format,
	int rate, int channels,
	int bits, int frames, int bitrate, char *sink)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);
	if (recorder->state != RECORDER_STAT_IDLE) {
		M_LOGE("recorder not released !\n");
		goto __exit;
	}

	M_LOGD("sink %s\n", sink ? sink : "null");

	recorder->dst_rate = rate;
	recorder->src_rate = rate;

	if (frames <= 0 && sink)
		frames = 640;

	if (recorder_param_set(&recorder->pcm_info, recorder->dst_rate,
			channels, bits, frames)) {
		M_LOGE("set record param failed !\n");
		goto __exit;
	}

	if (sink) {
		if (format_parse_byname(sink, &recorder->format)) {
			M_LOGE("parse format failed !\n");
			goto __exit;
		}

		if (!recorder_format_assert(recorder->format)) {
			M_LOGE("format unsupport !\n");
			goto __exit;
		}
	} else {
		recorder->format = format;
	}

	if (recorder->format == MEDIA_FMT_UNKNOWN) {
		M_LOGE("format unsupport !\n");
		goto __exit;
	}

	recorder->mencoder = media_encoder_create(recorder->format);
	if (!recorder->mencoder) {
		M_LOGE("create media encoder failed !\n");
		goto __exit;
	}

	if (!recorder->mencoder->initialized) {
		recorder->mencoder->encode = recorder_encode_default;
		recorder->mencoder->action = recorder_action_default;
		recorder->mencoder->initialized = 1;
	}

	recorder->mencoder->frames = frames;
	recorder->mencoder->rate = recorder->dst_rate;
	recorder->mencoder->channels = channels;
	recorder->mencoder->bits = bits;
	recorder->mencoder->bitrate = bitrate > 0 ? bitrate :
		recorder_bitrate_default(recorder->format);

	if (sink) {
		recorder->type = recorder_type_parse(sink);
		if (recorder->type == MEDIA_TYPE_UNKNOWN) {
			M_LOGE("media type unsupport !\n");
			goto __exit2;
		}

		recorder->mpacker = media_packer_create(
			sink, recorder->type);
		if (!recorder->mpacker) {
			M_LOGE("create media packer fail !\n");
			goto __exit3;
		}

		recorder->buffer_size = frames * channels * (bits >> 3);

		M_LOGD("alloc record buffer %d\n", recorder->buffer_size);
		recorder->buffer = snd_zalloc(
				recorder->buffer_size, AFM_MAIN);
		if (!recorder->buffer) {
			M_LOGE("alloc record buffer failed !\n");
			goto __exit4;
		}
	} else {
		recorder->type = MEDIA_TYPE_STREAM;
	}

	recorder->in = in_stream_create();
	if (!recorder->in) {
		M_LOGE("create in stream failed !\n");
		if (recorder->buffer) {
			snd_free(recorder->buffer);
			recorder->buffer = NULL;
		}
		goto __exit4;
	}

	recorder->state = RECORDER_STAT_READY;
	M_LOGI("idle->ready\n");
	os_mutex_unlock(recorder->lock);
	return 0;

__exit4:
	media_packer_release(recorder->mpacker);
__exit3:
	recorder->type = MEDIA_TYPE_UNKNOWN;
__exit2:
	#ifdef UVOICE_RESAMPLE_ENABLE
	uvoice_resampler_release(recorder->resampler);
	recorder->resampler = NULL;
	#else
	M_LOGI("resampler not enable\n");
	#endif
__exit1:
	media_encoder_release(recorder->mencoder);
	recorder->mencoder = NULL;
__exit:
	os_mutex_unlock(recorder->lock);
	return -1;
}

static int recorder_clr_sink(void)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;
	media_encoder_t *mencoder = NULL;
	media_packer_t *mpacker = NULL;
	int ret;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	os_mutex_lock(recorder->lock, OS_WAIT_FOREVER);

	if (recorder->state == RECORDER_STAT_IDLE) {
		M_LOGW("recorder is idle\n");
		os_mutex_unlock(recorder->lock);
		return -1;
	}

	if (recorder->type != MEDIA_TYPE_STREAM) {
		if (recorder->state != RECORDER_STAT_STOP &&
			recorder->state != RECORDER_STAT_READY) {
			M_LOGE("state %d can't clr sink\n", recorder->state);
			os_mutex_unlock(recorder->lock);
			return -1;
		}
	}

	mencoder = recorder->mencoder;
	if (!mencoder) {
		M_LOGE("mencoder null !\n");
		os_mutex_unlock(recorder->lock);
		return -1;
	}

	if (recorder->type == MEDIA_TYPE_STREAM) {
		if (recorder->state == RECORDER_STAT_RUNNING) {
			mencoder->action(mencoder, RECORDER_STOP, NULL);
			in_stream_stop(recorder->in);
			recorder->state = RECORDER_STAT_STOP;
			M_LOGI("running->stop\n");
		}
	} else {
		mpacker = recorder->mpacker;
		if (!mpacker) {
			M_LOGE("mpacker null !\n");
			os_mutex_unlock(recorder->lock);
			return -1;
		}
	}

	if (recorder->state != RECORDER_STAT_STOP &&
		recorder->state != RECORDER_STAT_READY) {
		M_LOGE("recorder can't clr sink ! state %d\n",
			recorder->state);
		os_mutex_unlock(recorder->lock);
		return -1;
	}

	if (recorder->type != MEDIA_TYPE_STREAM) {
		if (!mencoder->header_cplt &&
			mencoder->header_update && mpacker->update) {
			if (mencoder->header && mencoder->header_size > 0) {
				M_LOGD("header update\n");
				mencoder->header_update(mencoder, mpacker->size);
				ret = mpacker->update(mpacker,
					mencoder->header, mencoder->header_size, 0);
				if (ret < 0)
					M_LOGE("pack header failed !\n");		

				snd_free(mencoder->header);
				mencoder->header = NULL;
				mencoder->header_size = 0;
			}
		}
	}

#ifdef UVOICE_RESAMPLE_ENABLE
	uvoice_resampler_release(recorder->resampler);
#endif
	media_encoder_release(mencoder);
	media_packer_release(mpacker);

	if (recorder->buffer) {
		snd_free(recorder->buffer);
		recorder->buffer = NULL;
	}

	if (recorder->rec_buffer) {
		snd_free(recorder->rec_buffer);
		recorder->rec_buffer = NULL;
	}

	in_stream_release(recorder->in);

	recorder->resampler = NULL;
	recorder->mencoder = NULL;
	recorder->mpacker = NULL;
	recorder->in = NULL;

	recorder_reset(recorder);
	recorder->format = MEDIA_FMT_UNKNOWN;
	recorder->type = MEDIA_TYPE_UNKNOWN;
	if (recorder->state == RECORDER_STAT_STOP)
		M_LOGI("stop->idle\n");
	else
		M_LOGI("ready->idle\n");
	recorder->state = RECORDER_STAT_IDLE;

	os_mutex_unlock(recorder->lock);
	return 0;
}

static int recorder_format_support(media_format_t format)
{
	if (recorder_format_assert(format))
		return 1;
	return 0;
}

static int recorder_ns_enable(int enable)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;
	int ret = 0;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	enable = !!enable;

	if (recorder->ns_enable != enable) {
		recorder->ns_enable = enable;
		if (recorder->ns_enable)
			ret = audio_param_set(PARAM_KEY_NOISE_SUPPRESSION,
				PARAM_VAL_ENABLE);
		else
			ret = audio_param_set(PARAM_KEY_NOISE_SUPPRESSION,
				PARAM_VAL_DISABLE);
	}

	return ret;
}

static int recorder_ec_enable(int enable)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;
	int ret = 0;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	enable = !!enable;

	if (recorder->ec_enable != enable) {
		recorder->ec_enable = enable;
		if (recorder->ec_enable)
			ret = audio_param_set(PARAM_KEY_ECHO_CANCELLATION,
				PARAM_VAL_ENABLE);
		else
			ret = audio_param_set(PARAM_KEY_ECHO_CANCELLATION,
				PARAM_VAL_DISABLE);
	}

	return ret;
}

static int recorder_agc_enable(int enable)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;
	int ret = 0;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	enable = !!enable;

	if (recorder->agc_enable != enable) {
		recorder->agc_enable = enable;
		if (recorder->agc_enable)
			ret = audio_param_set(PARAM_KEY_AUTO_GAIN_CONTROL,
				PARAM_VAL_ENABLE);
		else
			ret = audio_param_set(PARAM_KEY_AUTO_GAIN_CONTROL,
				PARAM_VAL_DISABLE);
	}

	return ret;
}

static int recorder_vad_enable(int enable)
{
	recorder_t *recorder = g_mrecorder ? g_mrecorder->priv : NULL;
	int ret = 0;

	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	enable = !!enable;

	if (recorder->vad_enable != enable) {
		recorder->vad_enable = enable;
		if (recorder->vad_enable)
			ret = audio_param_set(PARAM_KEY_VOICE_ACTIVE_DETECT,
				PARAM_VAL_ENABLE);
		else
			ret = audio_param_set(PARAM_KEY_VOICE_ACTIVE_DETECT,
				PARAM_VAL_DISABLE);
	}

	return ret;
}

static int recorder_reference(recorder_t *recorder)
{
	if (!recorder)
		return -1;
	recorder->reference_count++;
	return recorder->reference_count;
}

static int recorder_dereference(recorder_t *recorder)
{
	if (!recorder)
		return -1;
	recorder->reference_count--;
	return recorder->reference_count;
}

static recorder_t *recorder_init(void)
{
	recorder_t *recorder = snd_zalloc(sizeof(recorder_t), AFM_EXTN);
	if (!recorder) {
		M_LOGE("alloc recorder failed !\n");
		return NULL;
	}

	recorder->cplt_sem = os_sem_new(0);
	recorder->lock = os_mutex_new();

	recorder->reference_count = 1;
	recorder->format = MEDIA_FMT_UNKNOWN;
	recorder->type = MEDIA_TYPE_UNKNOWN;
	return recorder;
}

static int recorder_deinit(recorder_t *recorder)
{
	if (!recorder) {
		M_LOGE("recorder null !\n");
		return -1;
	}

	if (recorder->state != RECORDER_STAT_IDLE) {
		M_LOGE("recorder not released !\n");
		return -1;
	}

	os_mutex_free(recorder->lock);
	os_sem_free(recorder->cplt_sem);
	snd_free(recorder);
	return 0;
}

uvoice_recorder_t *uvoice_recorder_create(void)
{
	uvoice_recorder_t *mrecorder = g_mrecorder;

	if (mrecorder) {
		if (recorder_reference(mrecorder->priv) < 0) {
			M_LOGE("get uvoice recorder failed !\n");
			return NULL;
		}
		return mrecorder;
	}

	mrecorder = snd_zalloc(sizeof(uvoice_recorder_t), AFM_EXTN);
	if (!mrecorder) {
		M_LOGE("alloc uvoice recorder failed !\n");
		return NULL;
	}

	mrecorder->priv = recorder_init();
	if (!mrecorder->priv) {
		M_LOGE("init recoder failed !\n");
		snd_free(mrecorder);
		return NULL;
	}

	mrecorder->start = recorder_start;
	mrecorder->stop = recorder_stop;
	mrecorder->get_stream = recorder_get_stream;
	mrecorder->set_sink = recorder_set_sink;
	mrecorder->clr_sink = recorder_clr_sink;
	mrecorder->get_state = recorder_get_state;
	mrecorder->get_position = recorder_get_position;
	mrecorder->ns_enable = recorder_ns_enable;
	mrecorder->ec_enable = recorder_ec_enable;
	mrecorder->agc_enable = recorder_agc_enable;
	mrecorder->vad_enable = recorder_vad_enable;
	mrecorder->format_support = recorder_format_support;

	g_mrecorder = mrecorder;
	M_LOGI("uvoice recorder create\n");
	return mrecorder;
}

int uvoice_recorder_release(uvoice_recorder_t *mrecorder)
{
	if (!mrecorder) {
		M_LOGE("uvoice recorder null !\n");
		return -1;
	}

	if (mrecorder != g_mrecorder) {
		M_LOGE("uvoice recorder invalid !\n");
		return -1;
	}

	if (recorder_dereference(mrecorder->priv) == 0) {
		if (recorder_deinit(mrecorder->priv)) {
			M_LOGE("free recorder failed !\n");
			return -1;
		}

		snd_free(mrecorder);
		g_mrecorder = NULL;
		M_LOGI("uvoice recorder release\n");
	}

	return 0;
}

