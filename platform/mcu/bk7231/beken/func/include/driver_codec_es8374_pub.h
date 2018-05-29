#ifndef _DRIVER_CODEC_ES8374_PUB_H_
#define _DRIVER_CODEC_ES8374_PUB_H_

extern void es8374_codec_init(void);
extern void es8374_codec_configure(unsigned int fs, unsigned char datawidth);
extern void es8374_codec_close(void);
extern void es8374_codec_volume_control(unsigned char volume);
extern void es8374_codec_mute_control(BOOL enable);
#endif
