
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MP3_ENC_LIB_API__
#define __MP3_ENC_LIB_API__

#include <aenc_sw_lib.h>

extern struct __AudioENC_AC320 *AudioMP3ENCEncInit(void);
extern int  AudioMP3ENCEncExit(struct __AudioENC_AC320 *p);

#endif // __MP3_ENC_LIB_API__

#ifdef __cplusplus
}
#endif
