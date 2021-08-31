

#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PLAYER_MP3_WELCOME,
    PLAYER_MP3_WAKEUP,
    PLAYER_MP3_LIGHT_ON,
    PLAYER_MP3_LIGHT_OFF,
    PLAYER_MP3_MAX
} player_mp3_e;

typedef void (*player_cb_t)(player_mp3_e file);

int32_t player_init(player_cb_t cb);

int32_t player_play(player_mp3_e file);
int32_t player_wait_complete(void);
int32_t player_stop(void);

#ifdef __cplusplus
}  // extern "C"
#endif
#endif
