/***************************************************************************
 *
 * Copyright 2019-2025 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
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
