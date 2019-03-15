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
			               printf("Badness in %s at %s:%d\n", __func__, __FILE__, __LINE__);\
			               return -1;\
			}
mgrctl_ctx g_mc;

static void aud_set_dev_mask(AUDIO_Device dev)
{
	HAL_SET_BIT(g_mc.current_dev, dev);
}

static void aud_clr_dev_mask(AUDIO_Device dev)
{
	HAL_CLR_BIT(g_mc.current_dev, dev);
}

static uint8_t aud_get_dev_status(AUDIO_Device dev)
{
	return !!HAL_GET_BIT(g_mc.current_dev, dev);
}

int aud_mgr_maxvol()
{
	return VOLUME_MAX_LEVEL;
}

static int aud_set_mute(mgrctl_ctx* mc, uint32_t dev, int mute)
{
	BUG_ON(mute > 1 || mute < 0);
	HAL_CODEC_MUTE_STATUS_Init(mute);
	if (mc->playback == 0)
		return 0;

	if (HAL_CODEC_Mute(dev,mute) != 0)
		return -1;

	return 0;
}

static int aud_set_vol(mgrctl_ctx* mc, uint32_t dev, int level)
{
	BUG_ON(level > VOLUME_MAX_LEVEL || level < VOLUME_LEVEL0);

	int volume = level;

	HAL_CODEC_INIT_VOLUME_Set(dev, volume);
	if (mc->playback == 0) {
		return 0;
	}

	if (HAL_CODEC_VOLUME_LEVEL_Set(dev,volume) != 0)
		return -1;

	return 0;
}

static int aud_set_dev(mgrctl_ctx* mc, uint32_t dev, uint8_t dev_en)
{
	BUG_ON(!(dev & AUDIO_DEV_ALL));

	uint8_t i;
	AUDIO_Device cur_dev;
	uint16_t dev_mask = dev;

	for (i = 0, cur_dev = (1 << AUDIO_IN_DEV_SHIFT); i < AUDIO_DEVICE_NUM; i++, cur_dev <<= 1) {
		if (!(cur_dev & AUDIO_IN_DEV_ALL) && !(cur_dev & AUDIO_OUT_DEV_ALL))
			cur_dev = (1 << AUDIO_OUT_DEV_SHIFT);

		if (dev_en) {//device enable
			if ((cur_dev & dev_mask) && (!aud_get_dev_status(cur_dev))) {
				if (HAL_CODEC_ROUTE_Set(cur_dev, CODEC_DEV_ENABLE) != 0)
					return -1;
				aud_set_dev_mask(cur_dev);
			}
		} else {//device disable
			if ((cur_dev & dev_mask) && aud_get_dev_status(cur_dev)) {
				if (HAL_CODEC_ROUTE_Set(cur_dev, CODEC_DEV_DISABLE) != 0)
					return -1;
				aud_clr_dev_mask(cur_dev);
			}
		}
	}

	return 0;
}

static int aud_set_eqscene(mgrctl_ctx* mc, uint8_t scene)
{
	BUG_ON(scene < 0);

	uint8_t scene_val = scene;
	if (mc->playback == 0) {
		return 0;
	}

	if (HAL_CODEC_EQ_SCENE_Set(scene_val) != 0)
		return -1;

	return 0;
}

static int __set_volume(mgrctl* m, uint32_t dev, uint8_t vol)
{
	mgrctl_ctx* mc;
	mc = (mgrctl_ctx*)m;
	if (aud_set_vol(mc, dev, vol) != 0)
		return -1;
	return 0;
}

static int __set_path(mgrctl* m, uint32_t dev, uint8_t dev_en)
{
	int ret = 0;
	mgrctl_ctx* mc;
	mc = (mgrctl_ctx*)m;
	if (aud_set_dev(mc, dev, dev_en) != 0)
		return -1;
	return ret;
}

static int __set_mute(mgrctl* m, uint32_t dev, uint8_t mute)
{
	mgrctl_ctx* mc;
	mc = (mgrctl_ctx*)m;
	if (aud_set_mute(mc, dev, mute) != 0)
		return -1;
	return 0;
}

static int __set_eqscene(mgrctl* m, uint8_t scene)
{
	mgrctl_ctx* mc;
	mc = (mgrctl_ctx*)m;
	if (aud_set_eqscene(mc, scene) != 0)
		return -1;
	return 0;
}

static struct mgrctl_ops mgr_ops =
{
	.volume 	= __set_volume,
	.path		= __set_path,
	.mute 		= __set_mute,
	.eqscene	= __set_eqscene,
};

static int set_mute(mgrctl* m, uint32_t dev, uint8_t mute)
{
	return m->ops->mute(m, dev, mute);
}

static int set_volume(mgrctl* m, uint32_t dev, uint8_t vol)
{
	return m->ops->volume(m, dev, vol);
}

static int set_path(mgrctl* m, uint32_t dev, uint8_t dev_en)
{
	return m->ops->path(m, dev, dev_en);
}

static int set_eqscene(mgrctl* m, uint8_t scene)
{
	return m->ops->eqscene(m, scene);
}

/**
 * event:
 *	0: volume event
 *	1: dev event
 */
int aud_mgr_handler(AudioManagerCommand event, uint32_t dev, uint32_t param)
{
	BUG_ON(event >= AUDIO_DEVICE_MANAGER_NONE);

	mgrctl_ctx* mc = &g_mc;
	MANAGER_MUTEX_LOCK(&(mc->lock));

	switch (event) {
		case AUDIO_DEVICE_MANAGER_VOLUME :
			set_volume(&(mc->base), dev, param);
			break;
		case AUDIO_DEVICE_MANAGER_MUTE:
			set_mute(&(mc->base), dev, param);
			break;
		case AUDIO_DEVICE_MANAGER_PATH :
			set_path(&(mc->base), dev, param);
			break;
		case AUDIO_DEVICE_MANAGER_EQSCENE :
			set_eqscene(&(mc->base), param);
			break;
		default:
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
