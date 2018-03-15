#ifndef EXAMPLE_MEDIA_TL_H
#define EXAMPLE_MEDIA_TL_H

#include "mmf_source.h"
#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include "platform_opts.h"
#include "osdep_api.h"
#include "basic_types.h"
#include "section_config.h"

#include "videodev2.h"
#include "uvcvideo.h"
#include "v4l2_driver.h"
#include "uvc_intf.h"



void example_media_tl(void);
void example_media_tl_main(void *param);
int uvc_tl_mod_handle(int);
int uvc_tl_mod_set_param(void* ctx, int cmd, int arg);
void uvc_tl_mod_close(void* ctx);
void* uvc_tl_mod_open(void);

#endif /* EXAMPLE_MEDIA_SS_H */