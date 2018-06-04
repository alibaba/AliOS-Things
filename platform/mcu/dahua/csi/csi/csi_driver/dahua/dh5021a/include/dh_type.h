/******************************************************************************

  Copyright (C), 2014-2020, ZheJiang Dahua Technology CO.,LTD.

 ******************************************************************************
  File Name     : dh_type.h
  Version       : Initial Draft
  Author        : Lv Zhuqing<lv_zhuqing@dahuatech.com>
  Created       : 2014.3.13
  Last Modified :
  Description   : The common data type defination
  Function List :
  DHstory       :
  1.Date        : 2014/3/13
    Author      : 24497
    Modification: Create
******************************************************************************/

#ifndef __DH_TYPE_H__
#define __DH_TYPE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * The common data type, will be used in the whole project.*
 *----------------------------------------------*/

typedef unsigned char           DH_U8;
typedef unsigned short          DH_U16;
typedef unsigned int            DH_U32;

typedef signed char             DH_S8;
typedef short                   DH_S16;
typedef int                     DH_S32;

#ifndef _M_IX86
    typedef unsigned long long  DH_U64;
    typedef long long           DH_S64;
#else
    typedef __int64             DH_U64;
    typedef __int64             DH_S64;
#endif

typedef char                    DH_CHAR;
typedef void                    DH_VOID;


typedef float                   DH_F32;
typedef double                  DH_F64;


/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/
typedef enum {
    DH_FALSE = 0,
    DH_TRUE  = 1,
} DH_BOOL;

typedef enum {
	DH_ELEC_VL = 0,
	DH_ELEC_VH = 1,
} DH_ELEC_LEVEL;

#ifndef NULL
    #define NULL    0L
#endif

#define DH_NULL     0L
#define DH_SUCCESS  0
#define DH_FAILURE  (-1)
#define DH_ETIMEOUT  (-2)        /* 等待超时*/
#define DH_EINTR     (-3)        /* 被信号中断*/

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DH_TYPE_H__ */


