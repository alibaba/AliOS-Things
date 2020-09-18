#ifndef COLOR_LOG_H
#define COLOR_LOG_H

// termial color code
#define LOG_COLOR_CODE_DEFAULT "\x1B[0m"
#define LOG_COLOR_CODE_BLACK   "\x1B[1;30m"
#define LOG_COLOR_CODE_RED     "\x1B[1;31m"
#define LOG_COLOR_CODE_GREEN   "\x1B[1;32m"
#define LOG_COLOR_CODE_YELLOW  "\x1B[1;33m"
#define LOG_COLOR_CODE_BLUE    "\x1B[1;34m"
#define LOG_COLOR_CODE_MAGENTA "\x1B[1;35m"
#define LOG_COLOR_CODE_CYAN    "\x1B[1;36m"
#define LOG_COLOR_CODE_WHITE   "\x1B[1;37m"

#ifdef _FILE_TAG_
#include "hal_trace.h"
//E W D I V  android log format
//can use https://atom.io/packages/language-log   more readable and Colourful
//usage
// ----------how to use
// 1. define _FILE_TAG_ "filename you needed"
// 2. include pbap_log.h
// ----------how to disable 
// 1.comment the _FILE_TAG_ in the file you need to disable

#ifdef DEBUG

#define LOG_N(...)    hal_trace_printf_without_crlf(__VA_ARGS__)
                  

#define LOG_E(s,...) do{ \
                        LOG_N("E/%s %s()- %04d:",_FILE_TAG_,__FUNCTION__,__LINE__); \
                        LOG_N(s,##__VA_ARGS__);\
                        LOG_N("\n");\
                    }while(0) 

#define LOG_W(s,...) do{ \
                        LOG_N("W/%s %s()- %04d:",_FILE_TAG_,__FUNCTION__,__LINE__); \
                        LOG_N(s,##__VA_ARGS__);\
                        LOG_N("\n");\
                    }while(0) 
#define LOG_D(s,...) do{ \
                        LOG_N("D/%s %s()- %04d:",_FILE_TAG_,__FUNCTION__,__LINE__); \
                        LOG_N(s,##__VA_ARGS__);\
                        LOG_N("\n");\
                    }while(0) 
#define LOG_I(s,...) do{ \
                        LOG_N("I/%s %s()- %04d:",_FILE_TAG_,__FUNCTION__,__LINE__); \
                        LOG_N(s,##__VA_ARGS__);\
                        LOG_N("\n");\
                    }while(0) 
#define LOG_V(s,...) do{ \
                        LOG_N("V/%s %s()- %04d:",_FILE_TAG_,__FUNCTION__,__LINE__); \
                        LOG_N(s,##__VA_ARGS__);\
                        LOG_N("\n");\
                    }while(0) 
#else
static inline void color_log_dummy(const char *fmt, ...) { }
#define LOG_N(...)      color_log_dummy(##__VA_ARGS__)
#define LOG_E(s,...)    color_log_dummy(s,##__VA_ARGS__)
#define LOG_W(s,...)    color_log_dummy(s,##__VA_ARGS__)
#define LOG_D(s,...)    color_log_dummy(s,##__VA_ARGS__)
#define LOG_I(s,...)    color_log_dummy(s,##__VA_ARGS__)
#define LOG_V(s,...)    color_log_dummy(s,##__VA_ARGS__)

#endif
                    
#else 

#define LOG_N(...)     ((void*)0)
#define LOG_E(s,...) ((void*)0)
#define LOG_W(s,...)   ((void*)0)
#define LOG_D(s,...)  ((void*)0)
#define LOG_I(s,...) ((void*)0)
#define LOG_V(s,...) ((void*)0)

#endif


#endif

