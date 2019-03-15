#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "kernel/os/os_mutex.h"
#include "driver/chip/hal_codec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MANAGER_MUTEX_INIT(a)           OS_MutexCreate(a)
#define MANAGER_MUTEX_LOCK(a)           OS_MutexLock(a,OS_WAIT_FOREVER)
#define MANAGER_MUTEX_UNLOCK(a)         OS_MutexUnlock(a)
#define MANAGER_NUTEX_DESTROY(a)        OS_MutexDelete(a)
#define MANAGER_MUTEX                   OS_Mutex_t

typedef struct mgrctl mgrctl;
struct mgrctl_ops
{
	int (*volume)(mgrctl* m, uint32_t dev, uint8_t vol);
	int (*path)(mgrctl* m, uint32_t dev, uint8_t dev_en);
	int (*mute)(mgrctl* m, uint32_t dev, uint8_t mute);
	int (*eqscene)(mgrctl* m, uint8_t scene);
};

struct mgrctl
{
	struct mgrctl_ops* ops;
};

typedef struct {
	mgrctl      base;
	int8_t      is_initialize;
	int8_t      playback;
	int8_t      record;
	uint16_t    current_dev; /* bit mask of output devices */
	MANAGER_MUTEX lock;
} mgrctl_ctx;

typedef enum {
	AUDIO_DEVICE_MANAGER_VOLUME = 0,
	AUDIO_DEVICE_MANAGER_PATH,
	AUDIO_DEVICE_MANAGER_MUTE,
	AUDIO_DEVICE_MANAGER_EQSCENE,
	AUDIO_DEVICE_MANAGER_NONE,
} AudioManagerCommand;

int aud_mgr_init();
int aud_mgr_deinit();
int aud_mgr_handler(AudioManagerCommand event, uint32_t dev, uint32_t param);
int aud_mgr_maxvol();
mgrctl_ctx* aud_mgr_ctx();

#ifdef __cplusplus
}
#endif

#endif
