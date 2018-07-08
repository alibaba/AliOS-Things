/******************************************************************************

  Copyright (C), 2011-2016, ZheJiang Dahua Technology CO.,LTD.

 ******************************************************************************
  File Name      : api_venc.h
  Version          : Initial Draft
  Author           : Lv Zhuqing
  Created         : 2014.2.28
  Last Modified :
  Description    : all struct and maro definition for debug
  Function List  :
  DHstory        :
  1.Date          : 2014/2/28
    Author        : 24497
    Modification: Create
******************************************************************************/

#ifndef __DH_DEBUG_H__
#define __DH_DEBUG_H__

#ifndef __KERNEL__
#include <stdarg.h>
#endif

#include "dh_common.h"

#ifndef DH_MODULE_NAME
#define DH_MODULE_NAME
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* 打印字体显示的颜色 */
#define NONE_C        "\033[m"   
#define RED           "\033[0;32;31m"   
#define GREEN         "\033[0;32;32m"   
#define BLUE          "\033[0;32;34m"   
#define DARY_GRAY     "\033[1;30m"   
#define CYAN          "\033[0;36m"   
#define PURPLE        "\033[0;35m"     
#define BROWN         "\033[0;33m"   
#define YELLOW        "\033[1;33m"   
#define WHITE         "\033[1;37m" 

#define _EX__FILE_LINE(fxx,lxx) "[File]:"fxx"\n[Line]:"#lxx"\n[Info]:"
#define EX__FILE_LINE(fxx,lxx) _EX__FILE_LINE(fxx,lxx)
#define __FILE_LINE__ EX__FILE_LINE(__FILE__, __LINE__)

#define DH_DBG_EMERG      0   /* system is unusable                   */
#define DH_DBG_ALERT      1   /* action must be taken immediately     */
#define DH_DBG_CRIT       2   /* critical conditions                  */
#define DH_DBG_ERR        3   /* error conditions                     */
#define DH_DBG_WARN       4   /* warning conditions                   */
#define DH_DBG_NOTICE     5   /* normal but significant condition     */
#define DH_DBG_INFO       6   /* informational                        */
#define DH_DBG_DEBUG      7   /* debug-level messages                 */

typedef struct hiLogLevelConf
{
    MOD_ID_E  enModId;
    DH_S32    s32Level;
    DH_CHAR   szModName[16];
} LOG_LEVEL_CONF_S;

#ifndef __KERNEL__ 
/******************************************************************************
** For User Mode : DH_PRINT, DH_ASSERT, DH_TRACE
******************************************************************************/

#define DH_PRINT printf

#ifdef DH_DEBUG
    /* Using samples:   DH_ASSERT(x>y); */
    #define DH_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            printf("\nASSERT failed at:\n"\
                   "  >File name: %s\n"   \
                   "  >Function : %s\n"   \
                   "  >Line No. : %d\n"   \
                   "  >Condition: %s\n",  \
                    __FILE__,__FUNCTION__, __LINE__, #expr);\
            _exit(-1);\
        } \
    }while(0)

    /* Using samples: 
    ** DH_TRACE(DH_DBG_DEBUG, DH_ID_VI, "Test %d, %s\n", 12, "Test");
    **/
    #define DH_TRACE(level, enModId, fmt...) fprintf(stderr,##fmt)
#else
    #define DH_ASSERT(expr)
    #define DH_TRACE(level, enModId, fmt...)
#endif

#else
/******************************************************************************
** For Linux Kernel : DH_PRINT, DH_ASSERT, DH_TRACE
******************************************************************************/

#define DH_PRINT printk

extern DH_S32 DH_ChkLogLevel(DH_S32 s32Level, MOD_ID_E enModId);
//extern asmlinkage int DH_LOG(DH_S32 s32Level, MOD_ID_E enModId,const char *fmt, ...);
extern int DH_LOG(DH_S32 s32Level, MOD_ID_E enModId,const char *fmt, ...);


#ifdef DH_DEBUG
    /* Using samples:   DH_ASSERT(x>y); */
    #define DH_ASSERT(expr)               \
    do{                                   \
        if (!(expr)) {                    \
            panic("\nASSERT failed at:\n" \
                  "  >File name: %s\n"    \
                  "  >Function : %s\n"    \
                  "  >Line No. : %d\n"    \
                  "  >Condition: %s\n",   \
                    __FILE__,__FUNCTION__, __LINE__, #expr);\
        } \
    }while(0)

    /* Using samples: 
    ** DH_TRACE(DH_DBG_DEBUG, DH_ID_VI, "Test %d, %s\n", 12, "Test");
    **/
    #define DH_TRACE DH_LOG
#else
    #define DH_ASSERT(expr)
    #define DH_TRACE(level, enModId, fmt...)
#endif

#endif  /* end of __KERNEL__ */


/*=========================================================================================
 ***************************************调试打印******************************************
==========================================================================================*/
#ifdef DH_DEBUG
/* 调试打印，输出字体为紫色。*/
#define DH_DBG(fmt, args ...) \
	do \
	{ \
		DH_PRINT(PURPLE "[" DH_MODULE_NAME "] " "DEBUG (%s|%d): " fmt NONE_C, __func__, __LINE__, ## args); \
	} \
	while(0)
#else
#define DH_DBG(fmt, args ...) 
#endif

/* 出错打印，输出字体为红色，打印出文件名、函数名和行号,表明程序不能继续运行。*/
#define DH_ERROR(fmt, args ...) \
    do \
    { \
    	DH_PRINT(RED "[" DH_MODULE_NAME "] " "ERROR (%s|%s|%d): " fmt NONE_C,__FILE__, __func__, __LINE__, ## args); \
    } \
    while(0)

/* 警告打印，输出字体为黄色，打印出文件名、函数名和行号。表明程序仍可继续运行，但须警示。*/
#define DH_WARN(fmt, args ...) \
	do \
	{ \
		DH_PRINT(YELLOW "[" DH_MODULE_NAME "] " "WARN (%s|%s|%d): " fmt NONE_C, __FILE__, __func__, __LINE__, ## args); \
	} \
	while(0)

/* 信息通告打印，输出字体为绿色。*/
#define DH_INFO(fmt, args ...) \
	do \
	{ \
		DH_PRINT(GREEN "[" DH_MODULE_NAME "] " "INFO (%s|%d): " fmt NONE_C, __func__, __LINE__, ## args); \
	} \
	while(0)
/*=============================================================================================== */

#define GOTO_EXIT_IF(_CONDITION)  				  					  \
{														  					  \
    if((_CONDITION)) 									  					  \
    { 																	  \
        DH_PRINT("Error :: %s : %s : %d :: ", __FILE__, __FUNCTION__, __LINE__);  \
        DH_PRINT(" Exiting because: %s\n", #_CONDITION); 					   \
        goto exit; 															   \
    }																		   \
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DH_DEBUG_H__ */

