#ifdef __cplusplus
extern "C" {
#endif

#ifndef PCM_ENC_H
#define PCM_ENC_H
#include "aenc_sw_lib.h"

extern struct __AudioENC_AC320 *AudioPCMEncInit();
extern int AudioPCMEncExit(struct __AudioENC_AC320 *p);

#endif /* PCM_ENC_H */

#ifdef __cplusplus
}
#endif
