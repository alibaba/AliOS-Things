/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
