/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
*/

#ifndef GPS_PARSE_H
#define GPS_PARSE_H

#include "gps_hal.h"

    
#define GPS_CONV_DATA_LEN       (64)
#define GPS_CALC_BUF_LEN        (256)
#define GPS_CALC_STR_LEN        (GPS_CALC_BUF_LEN-1)
#define GPS_TYPE_NAME_LEN       (16)

#define GPS_UTC_YEAR_LEN        (4)
#define GPS_UTC_MON_LEN         (2)
#define GPS_UTC_DAY_LEN         (2)

#define GPS_UTC_HOUR_LEN        (2)
#define GPS_UTC_MIN_LEN         (2)
#define GPS_UTC_SEC_LEN         (2)
#define GPS_UTC_HSEC_LEN        (3)
#define GPS_UTC_POINT_LEN       (1)
    
    
typedef enum{
    GPS_TYPE_UINT8 = 0,
    GPS_TYPE_INT32,
    GPS_TYPE_FLOAT,
    GPS_TYPE_STR,
    GPS_TYPE_UTC
}gps_data_type;


typedef struct test_gps_data_t{
    int     type;
    void*   addr;
}test_gps_data_t;

static inline int gps_atoi(const char *str, int size, int radix)
{
    int ret = 0;
    char *str_tmp = NULL;
    char buff[GPS_CONV_DATA_LEN];

    if(size < GPS_CONV_DATA_LEN){
        memcpy(&buff[0], str, size);
        buff[size] = '\0';
        ret = strtol(&buff[0], &str_tmp, radix);
    }
    return ret;
}

static inline float gps_atof(const char *str, int len)
{
    double ret = 0;
    char *str_tmp = NULL;
    char buff[GPS_CONV_DATA_LEN];

    if(len < GPS_CONV_DATA_LEN){
        memcpy(&buff[0], str, len);
        buff[len] = '\0';
        ret = strtod(&buff[0], &str_tmp);
    }

    return (float)ret;
}

static inline char* gps_strtok(char* src, char** ret, char c,int len)
{
    int i;
    char* str_tmp = src;

    for(i = 0; i < len; i++,str_tmp++){
        if(c == *str_tmp){
            str_tmp[0] = '\0';
            *ret = str_tmp+1;
            return src;
        }
    }
    
    *ret = str_tmp;
    return src;
}

static inline int gps_utc_get(const char *str, int len, gps_time_t* res)
{
    int success = 0;
    char* str_tmp = NULL;

    switch(len){
        case (sizeof("YYYYMMDDhhmmss.sss") - 1):
            str_tmp = str;
            res->year = gps_atoi(str_tmp,GPS_UTC_YEAR_LEN,10);
            str_tmp += GPS_UTC_YEAR_LEN;
            res->mon = gps_atoi(str_tmp,GPS_UTC_MON_LEN,10);
            str_tmp += GPS_UTC_MON_LEN;
            res->day = gps_atoi(str_tmp,GPS_UTC_DAY_LEN,10);
            str_tmp += GPS_UTC_DAY_LEN;
                
            res->hour = gps_atoi(str_tmp,GPS_UTC_HOUR_LEN,10);
            str_tmp += GPS_UTC_HOUR_LEN;
            res->min = gps_atoi(str_tmp,GPS_UTC_MIN_LEN,10);
            str_tmp += GPS_UTC_MIN_LEN;
            res->sec = gps_atoi(str_tmp,GPS_UTC_SEC_LEN,10);
            str_tmp += (GPS_UTC_SEC_LEN + GPS_UTC_POINT_LEN);
            res->hsec = gps_atoi(str_tmp,3,10);
            break;

        case (sizeof("hhmmss.sss") - 1):
                        
            str_tmp = str;
            res->hour = gps_atoi(str_tmp,GPS_UTC_HOUR_LEN,10);
            str_tmp += GPS_UTC_HOUR_LEN;
            res->min = gps_atoi(str_tmp,GPS_UTC_MIN_LEN,10);
            str_tmp += GPS_UTC_MIN_LEN;
            res->sec = gps_atoi(str_tmp,GPS_UTC_SEC_LEN,10);
            str_tmp += (GPS_UTC_SEC_LEN + GPS_UTC_POINT_LEN);
            res->hsec = gps_atoi(str_tmp,3,10);
            break;
            
        default:
            LOG("Parse of UTC fail (format error)!");
            return -1;

    }

    return 0;        
}


static inline void gps_data_conv(void* str, int len,void* addr,int type)
{
    switch(type){
        case GPS_TYPE_FLOAT:
            *(float*)(addr) = (float)gps_atof(str,len); 
            //printf("f == %f\n",*(float*)(addr));
            break;
        case GPS_TYPE_INT32:
            *(int*)(addr) = (int)gps_atoi(str,len,10); 
            //printf("d == %d\n",*(int*)(addr) );
            break;
        
        case GPS_TYPE_UINT8:
            *(char*)(addr) = *(char*)str;
            //printf("c == %c\n",*(char*)(addr) );
            break;
            
        case GPS_TYPE_STR:
            memcpy(addr, str, len);
            ((char *)addr)[len] = '\0';
            //printf("S == %s\n",(char*)(addr) );
            break;

        case GPS_TYPE_UTC:
            gps_utc_get(str,len,addr);
            break;
        default: break;
    }

}


#endif /* GPS_PARSE_H */

