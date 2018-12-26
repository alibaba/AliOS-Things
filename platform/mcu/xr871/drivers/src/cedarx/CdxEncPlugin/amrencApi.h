
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __AMR_ENC_LIB_API__
#define __AMR_ENC_LIB_API__

#include <aenc_sw_lib.h>

struct __AudioENC_AC320 *AudioAMRENCEncInit(void);
int  AudioAMRENCEncExit(struct __AudioENC_AC320 *p);

#endif // __AMR_ENC_LIB_API__

#ifdef __cplusplus
}
#endif
