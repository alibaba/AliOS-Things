/**
 * ant_time.h
 *
 * 时钟接口
 */
#ifndef __ANT_TIME_H__
#define __ANT_TIME_H__

#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ant_s32 year;      // [0~127]   
    ant_s32 month;     // [1~12]
    ant_s32 wday;
    ant_s32 day;
    ant_s32 hour;
    ant_s32 minute;
    ant_s32 second;
    ant_s32 isdst;  // one digit expresses a quarter of an hour, for example: 22 indicates "+5:30"
} ant_time;

/**
 * 获取系统当前日期
 *
 * @param date_time 返回系统当前日期
 * @return  0： 获取成功
 *         -1： 获取失败
 */
ant_s32 ant_get_localtime(ant_time* date_time);

/**
 * 设置当前UTC时间
 *
 * @param utc_seconds 距离1970.1.1的秒数
 */
void    ant_set_utc_seconds(ant_s64 utc_seconds);

/**
 * 获取当前UTC时间
 *
 * @return 距离1970.1.1的秒数
 */
ant_s64 ant_get_utc_seconds(void);

/**
 * 获取系统启动以来的时间
 *
 * @return 系统启动以来的时间，单位为毫秒
 */
ant_u32 ant_get_system_minsecs(void);

ant_s32  ant_striptime(ant_char * buf, ant_char * format, ant_time  * time_out);

#ifdef __cplusplus
}
#endif

#endif

