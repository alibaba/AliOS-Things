/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifndef UND_LOG_H
#define UND_LOG_H

#ifdef __cplusplus
extern "C" {
#endif
#ifdef BUILD_AOS
#include "linkkit/infra/infra_log.h"
#define und_debug(FMT, args...)  log_debug("[und]:", FMT, ##args)
#define und_info(FMT, args...)   log_info("[und]", FMT, ##args)
#define und_warn(FMT, args...)   log_warning("[und]", FMT,  ##args)
#define und_err(FMT, args...)    log_err("[und]", FMT, ##args)
#define und_crt(FMT, args...)    log_crt("[und]", FMT, ##args)
#else
#include <stdio.h>
#define und_debug(FMT, args...)  do {printf("[und]:"); printf(FMT "\r\n", ##args);} while(0)
#define und_info(FMT, args...)   do {printf("[und]:"); printf(FMT "\r\n", ##args);} while(0)
#define und_warn(FMT, args...)   do {printf("[und]:"); printf(FMT "\r\n", ##args);} while(0)
#define und_err(FMT, args...)    do {printf("[und]:"); printf(FMT "\r\n", ##args);} while(0)
#define und_crt(FMT, args...)    do {printf("[und]:"); printf(FMT "\r\n", ##args);} while(0)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* end of UND_LOG_H */
