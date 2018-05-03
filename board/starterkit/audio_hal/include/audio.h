#ifndef __AUDIO_H
#define __AUDIO_H

#include <stdint.h>

int flash_write(uint32_t flash_addr, void *src, int size);
int record_to_flash(void);
int playback_from_flash(void);
int audio_init(void);

#endif /* __AUDIO_H */
