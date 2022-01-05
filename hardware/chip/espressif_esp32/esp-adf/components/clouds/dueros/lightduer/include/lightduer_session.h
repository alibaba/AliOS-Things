// Copyright (2017) Baidu Inc. All rights reserveed.
//
// File: lightduer_session.h
// Auth: Su Hao (suhao@baidu.com)
// Desc: Manage the talk session.

#ifndef BAIDU_DUER_LIGHTDUER_MODULES_VOICE_ENGINE_LIGHTDUER_SESSION_H
#define BAIDU_DUER_LIGHTDUER_MODULES_VOICE_ENGINE_LIGHTDUER_SESSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lightduer_types.h"

extern void duer_session_initialize(void);

extern duer_u32_t duer_session_generate(void);

extern duer_bool duer_session_consume(duer_u32_t id);

extern duer_bool duer_session_is_matched(duer_u32_t id);

extern void duer_session_finalize(void);

#ifdef __cplusplus
}
#endif

#endif/*BAIDU_DUER_LIGHTDUER_MODULES_VOICE_ENGINE_LIGHTDUER_SESSION_H*/
