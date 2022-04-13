/**
 * Copyright (C) 2018 Alibaba.inc, All rights reserved.
 *
 * @file:    tgcts.h
 * @brief:
 * @date:    2019/7/10
 * @version: 1.0
 */
#ifndef __TGCTS_H__
#define __TGCTS_H__
// cpp head files
//#include <cstddef>
//#include <cstdint>
//#include <cstdio>
//#include <cstring>
#include <algorithm>
#include <list>
#include <map>
//#include <mutex>
#include <set>
#include <string>
#include <vector>

extern "C" {
// c head files
#include <assert.h>
#include <ctype.h>
//#include <execinfo.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
//#include <semaphore.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
//#include <termios.h>
#include <time.h>
#include <unistd.h>

//#include "syslog.h"

#if !defined(OS_LINUX)
// other libs
//#include "librws.h"

#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sockets.h"
#include "lwip/tcp.h"
#include "hal_trace.h"
//#include "mbedtls/certs.h"
//#include "mbedtls/ctr_drbg.h"
//#include "mbedtls/debug.h"
//#include "mbedtls/entropy.h"
//#include "mbedtls/net.h"
//#include "mbedtls/ssl.h"

//#include "opus.h"

//#include "curl/curl.h"

//#include <sntp.h>
#endif // OS_LINUX
}
#ifdef __cplusplus
extern "C" {
#endif

#define _TGCTS_VERSION "1.2.0"
#define _TGCTS_SUCCESS 0
#define _TGCTS_FAIL -1

#define _TGCTS_DEBUG

#ifdef _TGCTS_DEBUG
#define _TGCTS_LOGD(...) printf(__VA_ARGS__)
#else
#define _TGCTS_LOGD(...)
#endif
#define _TGCTS_LOGI(...) printf(__VA_ARGS__)

#define _TGCTS_TEST_BEGIN _TGCTS_LOGI("######## _TGCTS_TEST: %s :BEGIN ########\n", __func__);
#define _TGCTS_TEST_END _TGCTS_LOGI("######## _TGCTS_TEST: %s :END ########\n", __func__);
//#define _TGCTS_API_SUPPORTED(...)       _TGCTS_LOGI("AliGenie Compatibility
// Test: %s supported\n", ##__VA_ARGS__);
//#define _TGCTS_API_NOT_SUPPORTED(...)   _TGCTS_LOGI("AliGenie Compatibility
// Test: %s not supported\n", ##__VA_ARGS__);

typedef struct {
    char *name;
    int (*func)(void);
} TGCTS_TEST_CASE;

typedef struct {
    TGCTS_TEST_CASE *head;
    TGCTS_TEST_CASE *next;
} TGCTS_TEST_SUITE;
// TGCTS_TEST_CASE *tgcts_posix_test_cases;
// TGCTS_TEST_CASE *tgcts_cpp_test_cases;

void tgcts_run_test(TGCTS_TEST_CASE *test_case);
void tgcts_posix_test(void);
void tgcts_cpp_test(void);
void tgcts_librws_test(void);
void tgcts_libhttps_test(void);
void tgcts_libopus_test(void);
void tgcts_libcurl_test(void);
void tgcts_audio_test(void);
void tgcts_audiosystem_test(void);
void tgcts_system_test(void);
void tgcts_light_test();
void tgcts_input_test();
void tgcts_ota_test();
#ifdef __cplusplus
}
#endif

#endif
