/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef _AISPEECH_CMD_H_
#define _AISPEECH_CMD_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CMD_STATE_IDLE,
    CMD_ASR_START,
    CMD_VAD_ONESHOT,
    CMD_VAD_START,
    CMD_VAD_STOP,
    CMD_NATIVE_API,
    CMD_TEXT_TRIGGER,
    CMD_SYSTEM_RESET,
    INFO_SPEECH_BEGBIN,
    INFO_SPEECH_BEGBIN_TIMEOUT,
    INFO_SPEECH_END,
    INFO_SPEECH_END_TIMEOUT,
    INFO_WAKEUP_WAKEUP,
    INFO_ASR_RESULT,
    CMD_AUDIO_MUTE,
    AISPEECH_CMD_MAX,
}A7_AISPEECH_CMD_T;



#ifdef __cplusplus
}
#endif

#endif /* _AISPEECH_CMD_H_ */
