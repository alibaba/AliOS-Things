#include <string.h>
#include <stdio.h>
#include "kernel/os/os_mutex.h"
#include "audio/manager/audio_manager.h"

#define MANAGER_NULL          0

#if !MANAGER_NULL

#ifndef __LIKELY_
#define likely(x)             __builtin_expect(!!(x), 1)
#endif
#ifndef __UNLIKELY_
#define unlikely(x)           __builtin_expect(!!(x), 0)
#endif

#define BUG_ON(c)	if (unlikely((c)!=0)) { \
			               printf("Badness in %s at %s:%d/n", __func__, __FILE__, __LINE__);\
			               return -1;\
			}
mgrctl_ctx g_mc;

int aud_mgr_maxvol()
{
	return VOLUME_MAX_LEVEL;
}

static int aud_set_mute(mgrctl_ctx* mc, int mute)
{
	BUG_ON(mute > 1 || mute < 0);
	HAL_CODEC_MUTE_STATUS_Init(mute);
	if (mc->playback == 0)
		return 0;

	AUDIO_Device dev = mc->current_outdev;
	if (HAL_CODEC_Mute(dev,mute) != 0)
		return -1;

	return 0;
}

static int aud_set_vol(mgrctl_ctx* mc, int level)
{
	BUG_ON(level > VOLUME_MAX_LEVEL || level < VOLUME_LEVEL0);

	int volume = level;
	AUDIO_Device dev = mc->current_outdev;

	HAL_CODEC_INIT_VOLUME_Set(dev, volume);
	if (mc->playback == 0) {
		return 0;
	}

	if (HAL_CODEC_VOLUME_LEVEL_Set(dev,volume) != 0)
		return -1;

	return 0;
}

static int aud_set_outdev(mgrctl_ctx* mc, int dev)
{
	BUG_ON(dev > AUDIO_DEVICE_SPEAKER || dev == 0);

	AUDIO_Device device = dev;
	if (mc->current_outdev == device)
		return 0;
	else
		mc->current_outdev = device;

	BUG_ON(mc->playback == 0);

	if (HAL_CODEC_ROUTE_Set(device) != 0)
		return -1;

	return 0;
}

static int aud_set_indev(mgrctl_ctx* mc, int dev)
{
	BUG_ON(dev != AUDIO_DEVICE_HEADPHONEMIC && dev != AUDIO_DEVICE_MAINMIC);

	AUDIO_Device device = dev;
	if (mc->current_indev == device)
		return 0;
	else
		mc->current_indev = device;

	BUG_ON(mc->record == 0);

	if (HAL_CODEC_ROUTE_Set(device) != 0)
		return -1;

	return 0;
}

static int __set_volume(mgrctl* m, int vol)
{
	mgrctl_ctx* mc;
	mc = (mgrctl_ctx*)m;
	if (aud_set_vol(mc, vol) != 0)
		return -1;
	return 0;
}

static int __set_inpath(mgrctl* m, int dev)
{
	mgrctl_ctx* mc;
	mc = (mgrctl_ctx*)m;
	if (aud_set_indev(mc, dev) != 0)
		return -1;

	return 0;
}

static int __set_outpath(mgrctl* m, int dev)
{
	int ret = 0;
	mgrctl_ctx* mc;
	mc = (mgrctl_ctx*)m;
	mc = (mgrctl_ctx*)m;
	if (aud_set_outdev(mc, dev) != 0)
		return -1;
	return ret;
}

static int __set_mute(mgrctl* m, int mute)
{
	mgrctl_ctx* mc;
	mc = (mgrctl_ctx*)m;
	if (aud_set_mute(mc, mute) != 0)
		return -1;
	return 0;
}

static struct mgrctl_ops mgr_ops =
{
	.volume 	= __set_volume,
	.in_path	= __set_inpath,
	.out_path	= __set_outpath,
	.mute 	= __set_mute,
};

static int set_mute(mgrctl* m, int mute)
{
	return m->ops->mute(m, mute);
}

static int set_volume(mgrctl* m, int vol)
{
	return m->ops->volume(m, vol);
}

static int set_inpath(mgrctl* m, int dev)
{
	return m->ops->in_path(m, dev);
}

static int set_outpath(mgrctl* m, int dev)
{
	return m->ops->out_path(m, dev);
}

/**
 * event:
 *	0: volume event
 *	1: dev event
 */
int aud_mgr_handler(int event, int val)
{
	BUG_ON(event >= AUDIO_DEVICE_MANAGER_NONE);

	mgrctl_ctx* mc = &g_mc;
	MANAGER_MUTEX_LOCK(&(mc->lock));

	switch (event) {
		case AUDIO_DEVICE_MANAGER_VOLUME :
			set_volume(&(mc->base), val);
			break;
		case AUDIO_DEVICE_MANAGER_MUTE:
			set_mute(&(mc->base), val);
			break;
		case AUDIO_DEVICE_MANAGER_PATH :
			if (val <= AUDIO_DEVICE_SPEAKER)
				set_outpath(&(mc->base), val);
			else
				set_inpath(&(mc->base), val);
			break;
	}

	MANAGER_MUTEX_UNLOCK(&(mc->lock));
	return 0;
}

mgrctl_ctx * aud_mgr_ctx()
{
	mgrctl_ctx* mc = &g_mc;
	return mc;
}

int aud_mgr_init()
{
	mgrctl_ctx* mc = &g_mc;
	memset(mc, 0, sizeof(*mc));

	mc->current_outdev = AUDIO_DEVICE_SPEAKER;
	mc->current_indev = AUDIO_DEVICE_MAINMIC;
	mc->base.ops = &mgr_ops;

	if (MANAGER_MUTEX_INIT(&(mc->lock)) != 0)
		return -1;
	mc->is_initialize = 1;
	return 0;
}

int aud_mgr_deinit()
{
	mgrctl_ctx* mc = &g_mc;
	mc->is_initialize = 0;
	memset(mc, 0, sizeof(*mc));
	MANAGER_NUTEX_DESTROY(&(mc->lock));
	return 0;
}
#endif /* MANAGER_NULL */
