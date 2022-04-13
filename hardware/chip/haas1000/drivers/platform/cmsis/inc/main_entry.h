/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __MAIN_ENTRY_H__
#define __MAIN_ENTRY_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NOSTD
#define MAIN_ENTRY(...)                 _start(__VA_ARGS__)
#else
#define MAIN_ENTRY(...)                 main(__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif
