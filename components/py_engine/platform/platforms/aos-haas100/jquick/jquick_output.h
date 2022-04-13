//
// Created by liujuncheng on 2020/12/29.
//

#ifndef APPX_JQUICK_OUTPUT_H
#define APPX_JQUICK_OUTPUT_H

//#include "ulog/ulog.h"
//
//#define FILTER_FILENAME(x) (strrchr(x, '/') ? strrchr(x, '/') + 1 : x)
//#define LOG_TAG FILTER_FILENAME(__FILE__)
//
//#define greenui_printLog(LEVEL, str, ...) \
//    ulog(LEVEL, LOG_TAG, __FILE__, __LINE__, str, ##__VA_ARGS__)

#define greenui_printLog(LEVEL, str, ...) \
    printf(str, ##__VA_ARGS__);           \
    printf("\r\n");

#endif  //APPX_JQUICK_OUTPUT_H
