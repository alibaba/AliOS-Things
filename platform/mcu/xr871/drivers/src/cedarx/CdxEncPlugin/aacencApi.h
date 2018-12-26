
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __AAC_ENC_LIB_API__
#define __AAC_ENC_LIB_API__

#include <aenc_sw_lib.h>

extern struct __AudioENC_AC320 *AudioAACENCEncInit();
extern int AudioAACENCEncExit(struct __AudioENC_AC320 *p);

#endif // __AAC_ENC_LIB_API__

#ifdef __cplusplus
}
#endif
