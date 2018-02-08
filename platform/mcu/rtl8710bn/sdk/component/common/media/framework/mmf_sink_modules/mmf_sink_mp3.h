#ifndef MMF_SINK_MP3_FILE_H
#define MMF_SINK_MP3_FILE_H
#include "mmf_sink.h"
#include "FreeRTOS.h"
#include "task.h"
#include "platform_opts.h"
#include "mp3/mp3_codec.h"
#include "diag.h"
#include "i2s_api.h"
#include "analogin_api.h"
#include <stdlib.h>
#include "section_config.h"
#if CONFIG_EXAMPLE_MP3_STREAM_SGTL5000
#include "sgtl5000.h"
#else
#include "alc5651.h"
#endif

#endif /* MMF_SINK_MP3_FILE_H */