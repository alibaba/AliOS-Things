/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

/*
 * be_jse_feature.h
 *
 * JavaScript feature macros
 *
 */

#ifndef BE_JSE_FEATURE_H
#define BE_JSE_FEATURE_H

#define USE_GLOBAL
#define USE_INTEGER
#define USE_MATH
#define USE_JSON
#define USE_STRING
#define USE_ARRAY

#define USE_PROCESS
#define USE_MODULES

/* #define BE_JSE_SILENT */
#ifdef MINI_ENGINE
#define DUMP_SYMBL_USAGE
#endif

#define TRACE_JSE_INFO

#endif /* BE_JSE_FEATURE_H */
