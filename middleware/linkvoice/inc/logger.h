//
//  logger.h
//  pal_sdk
//

#ifndef logger_h
#define logger_h

#include <stdio.h>
#include <stdlib.h>

extern int g_pal_log_level;

#ifdef PAL_DEBUG
#define log_error(M, ...) fprintf(stderr,"\033[0;31m%s|%d\t", __FUNCTION__, __LINE__);fprintf(stderr, M, ##__VA_ARGS__);fprintf(stderr, "\033[0m\n");fflush(stderr);
#define log_info(M, ...) fprintf(stdout,"%s|%d\t", __FUNCTION__, __LINE__);fprintf(stdout, M, ##__VA_ARGS__);fprintf(stdout, "\n");fflush(stdout);
#define log_debug(M, ...) fprintf(stdout,"%s|%d\t", __FUNCTION__, __LINE__);fprintf(stdout, M, ##__VA_ARGS__);fprintf(stdout, "\n");fflush(stdout);
#else
#define log_error(M, ...) fprintf(stderr,"\033[0;31m%s|%d\t", __FUNCTION__, __LINE__);fprintf(stderr, M, ##__VA_ARGS__);fprintf(stderr, "\033[0m\n");fflush(stderr);
//#define log_error(M, ...)
#define log_info(M, ...) fprintf(stdout,"%s|%d\t", __FUNCTION__, __LINE__);fprintf(stdout, M, ##__VA_ARGS__);fprintf(stdout, "\n");fflush(stdout);
#define log_debug(M, ...)
#endif


#endif /* logger_h */
