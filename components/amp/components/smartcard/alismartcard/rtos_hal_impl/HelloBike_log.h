#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "aliyun_iot_smartcard_internal.h"

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

typedef enum {
    LOG_STDOUT = 0,    //output to stdout only.
    LOG_FILE,      //output to file and stdout.
    LOG_DB,        //output to db and stdout.
    LOG_FILE_DB    //output to db ,file and stdout.
} LOG_STORE_TYPE;

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERR,
    LOG_LEVEL_FATAL
} LOG_LEVEL;

typedef enum {
    LOG_MOD_BRIEF_MIN = 0,  //极简模式，仅输出函数名/行数及日志内容
    LOG_MOD_BRIEF,      //简要模式，默认情况仅输出TAG/文件名/函数名/行数/日志内容
    LOG_MOD_VERBOSE     //正式输出时，一律使用本模式
} LOG_MODE;

typedef const char cchar;

void log_set_level(LOG_LEVEL level);
void log_get_level(LOG_LEVEL *level);
void log_set_storage(bool storage);
void log_get_storage(bool *storage);

/**
 * @brief 日志模块初始化，只有初始化成功后才会进行日志存储，否则仅为输出日志内容.
 *
 * @param[in] dir: 目录名称，当存储类型为LOG_FILE, 记录保存目录名称.
 * @param[in] name: 模块名称，模块名即为存储目录名及数据库表名称.
 * @param[in] type: @LOG_STORE_TYPE 中的某一个,用来设置存储类型.
 * @param[in] mode: @LOG_MODE 中的某一个，用来设置输出样式.
 * @retval 成功返回0，失败返回错误码.
 * @note 如果需要自定义输出样式，可以根据log_printf来实现.
 *       如果需要存储到文件或数据库，可以分别设置存储参数，也可以直接使用默认参数
 *       不单独设置。
*/
int log_init(cchar *dir, cchar *name, LOG_STORE_TYPE type, LOG_MODE mode);

/**
 * @brief 日志模块销毁，程序运行结束前，一定要调用本接口，以防止内存泄漏.
*/
void log_destroy(void);

/**
 * @brief 设置文件存储参数
 *
 * @param[in] dir: 存储目录, 默认情况下@log_init的第一个参数即为存储目录名称.
 * @param[in] total_size: 分配的总体存储空间(单位是M Byte), 默认值是40M.
 * @param[in] single_size: 单个文件的最大size(单位是 M Byte), 默认值是5M.
 * @retval 成功返回0，失败返回错误码.
 * @note 默认文件夹大小超过
 *       设置大小后，会循环覆盖，删除最小时间戳的日志文件。log.INFO是软连接，默认
 *       指向最新的日志文件.
*/
uint8_t log_file_init(cchar *dir, int total_size, int single_size);

/**
 * @brief 设置文件缓冲大小，只有输出log 大小大于此缓冲时才会写入日志文件
 * 默认缓冲大小为0，不作缓存，即: 实时写入日志文件
 *
 * @param[in] buff_size:  所需设置的文件缓冲大小.
 * @retval 成功返回0，失败返回-1.
 * @note 不调用此接口设置时，默认缓冲大小为0，不作缓存，即: 实时写入日志文件
 *           此接口需要在log_init 之前调用
 *
*/
uint8_t log_file_set_cache_size(int buff_size);

/*
 * 打印颜色，用户可以直接修改该值以实现定制化.
 * see http://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix
 */
#define COL_DEF "\x1B[0m"   //white
#define COL_RED "\x1B[31m"  //red
#define COL_GRE "\x1B[32m"  //green
#define COL_BLU "\x1B[34m"  //blue
#define COL_YEL "\x1B[33m"  //yellow
#define COL_WHE "\x1B[37m"  //white
#define COL_CYN "\x1B[36m"
#define COL_MAG "\x1B[35m"

/**
 * @brief 底层日志打印函数，用户可以调用该函数实现自定义打印格式/颜色/日志等级等.
 *
 * @param[in] lvl: 本条日志的等级，目前支持等级参考@LOG_LEVEL.
 * @param[in] color: 本条日志需要使用的颜色code，参考上面的宏，NULL标明无需颜色支持.
 * @param[in] t: TAG，标示本条消息的TAG，用于快速检索及分类，NULL标明不使用TAG.
 * @param[in] f: 文件名称，标示本条消息发出所在的文件名，NULL标明不需要指明文件名.
 * @param[in] func: 函数名，标示本条消息所在的函数，NULL标明不需要指明函数名.
 * @param[in] l: 行号，标示本条消息所在的文件行数，NULL标明不需要指明行号.
 * @param[in] fmt: 格式化字符串，对应于printf的第一个参数.
 * @param[in] ...: 可变字符串，对应于printf的第二个参数.
 * @note  如果用户认为基础日志无法满足项目要求，可以封装一层接口，通过本函数实现个性化.
*/
void log_print(LOG_LEVEL lvl, cchar *color, cchar *t, cchar *f, cchar *func, int l, cchar *fmt, ...);

#define port_trace_module(module, level, format, ...)     HAL_Printf("[%s:%d]---> "format"\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

//目前提供的几组纪录log的方法
#if 0
#define log_d(tag, fmt,args...) \
    log_print(LOG_LEVEL_DEBUG,COL_WHE,tag,__FILE__,__FUNCTION__,__LINE__,fmt,##args)
#define log_i(tag, fmt,args...) \
    log_print(LOG_LEVEL_INFO,COL_GRE,tag,__FILE__,__FUNCTION__,__LINE__,fmt,##args)
#define log_w(tag, fmt,args...) \
    log_print(LOG_LEVEL_WARN,COL_CYN,tag,__FILE__,__FUNCTION__,__LINE__,fmt,##args)
#define log_e(tag,fmt,args...) \
    log_print(LOG_LEVEL_ERR,COL_YEL,tag,__FILE__,__FUNCTION__,__LINE__,fmt,##args)
#define log_f(tag,fmt,args...) \
    log_print(LOG_LEVEL_FATAL,COL_RED,tag,__FILE__,__FUNCTION__,__LINE__,fmt,##args)
#else
#define log_d(tag, fmt,args...) \
    port_trace_module(0, 3, fmt, ##args)
#define log_i(tag, fmt,args...) \
    port_trace_module(0, 3, fmt, ##args)
#define log_w(tag, fmt,args...) \
    port_trace_module(0, 2, fmt, ##args)
#define log_e(tag,fmt,args...) \
    port_trace_module(0, 1, fmt, ##args)
#define log_f(tag,fmt,args...) \
    port_trace_module(0, 1, fmt, ##args)
#endif

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif

