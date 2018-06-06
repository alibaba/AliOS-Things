//
//  things_utils.c
//  node_things
//
//  Created by howie on 21/01/2018.
//  Copyright © 2018 alibaba. All rights reserved.
//

#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include "be_utils.h"

int be_utils_timestr(char* buf, size_t buf_len)
{
    time_t now;
    time(&now);
    struct tm *tm_now = localtime(&now);
    char local_time[32] = {};
    strftime(local_time, sizeof(local_time), "%Y-%m-%d %H:%M:%S", tm_now);
    
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    gettimeofday(&tv, NULL);
    long timestamp_ms = tv.tv_sec * 1000LL;
    timestamp_ms += tv.tv_usec / 1000;
    
    sprintf(buf, "%s.%03ld", local_time, timestamp_ms % 1000);
    return 0;
}

unsigned char hex2num(unsigned char ch)
{
    if (ch >= 'a') {
        return ch - 'a' + 10;
    }

    return ch - '0';
}

char itoch(int val)
{
    if (val < 10) {
        return (char)('0' + val);
    }

    return (char)('a' + val - 10);
}

void num2hex(unsigned char ch, unsigned char* hex)
{
    hex[0] = itoch(ch / 16);
    hex[1] = itoch(ch % 16);
}

