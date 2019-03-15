////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:types.h
//      maintainer: Halley
//		Description	:re-define language C basic data type for porting conveniently
//						over platforms
//		Changelog	:
//					2012-02-21	Create basic data type re-definition for uniform
//						platform by Robert
///////////////////////////////////////////////////////////////////////////////

/**
* @addtogroup 驱动
* @{
* @defgroup Type Type
* @{
*/

#ifndef		__TYPES_H__
#define		__TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#ifndef NULL	
#define	NULL			((void*)0)
#endif

#define	FALSE			(0)
#define	TRUE			(1)

typedef	void(*FPCALLBACK)(void);

typedef unsigned char   		bool;
typedef bool(*TerminateFunc)(void);

typedef signed char				int8_t;
typedef unsigned char			uint8_t;

typedef signed short			int16_t;
typedef unsigned short			uint16_t;

typedef signed int				int32_t;
typedef unsigned int			uint32_t;

typedef	unsigned int			size_t;


#if defined(__ICCARM__)    // For IAR compiler
#define __ATTRIBUTE__(keyword)  keyword
#define BITBAND
#define AT(address)             @ address
#define SECTION(name)           @ name
#define WEAK                    __weak
#define UNUSED
#define __NO_INIT__             __no_init
#elif defined(__CC_ARM)    // For Keil compiler
#define __ATTRIBUTE__(keyword)  __attribute__((keyword))
#define BITBAND                 bitband
#define AT(address)             at(address)
#define SECTION(name)           section(name)
#define WEAK                    weak
#define UNUSED                  unused
#define __NO_INIT__
#else                      // For other compiler
#define __ATTRIBUTE__(keyword)  keyword
#define BITBAND
#define AT(address)
#define SECTION(name)
#define WEAK
#define UNUSED
#define __NO_INIT__
#endif
	

//大小端转换宏，ARM CPU为小端模式
#define Le16ToCpu(Val)		(Val)
#define Le32ToCpu(Val)		(Val)
#define CpuToLe16(Val)		(Val)
#define CpuToLe32(Val)		(Val)

#define Be16ToCpu(Val)		(((Val) << 8) | ((Val) >> 8))
#define Be32ToCpu(Val)		(((Val) << 24) | (((Val) << 8) & 0x00FF0000) | (((Val) >> 8) & 0x0000FF00) | ((Val) >> 24))
#define CpuToBe16(Val)		(((Val) << 8) | ((Val) >> 8))
#define CpuToBe32(Val)		(((Val) << 24) | (((Val) << 8) & 0x00FF0000) | (((Val) >> 8) & 0x0000FF00) | ((Val) >> 24))


#include "irqs.h"
#include <core_cm3.h>

const unsigned char *GetLibVersionDriver(void);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif	//__TYPE_H__


/**
 * @}
 * @}
 */
