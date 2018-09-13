/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __XR_DEBUG_H__
#define __XR_DEBUG_H__

#include "sys/xr_util.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define XR_DEBUG_ON


/*
 * @brief Debug level
 */
#define DBG_LEVEL_MASK (0x0F)


#define XR_LEVEL_EMERG 0

#define XR_LEVEL_ALERT 1

#define XR_LEVEL_CRIT 2

#define XR_LEVEL_ERROR 3

#define XR_LEVEL_WARNING 4

#define XR_LEVEL_NOTICE 5

#define XR_LEVEL_INFO 6

#define XR_LEVEL_DEBUG 7

#define XR_LEVEL_ALL 0x0F


/*
 * No expanded condition
 */
#define NOEXPAND 1


/*
 * module ON/OFF
 */
#define DBG_ON (1 << 4)

#define DBG_OFF (0)


/*
 * Always show message
 */
#define MOD_DBG_ALW_ON (DBG_ON | XR_LEVEL_ALL)


/************************************************************
 * XR_DEBUG INTERFACE
 ************************************************************/
#ifdef XR_DEBUG_ON

#define XR_DEBUG_PRINT(msg, arg...) printf(msg, ##arg)

#define XR_DEBUG_ABORT()	do { \
							printf("system aborted!"); \
							sys_abort(); \
						} while (0)


/*
 * @brief	The realization of showing debug messages.
 * @param	module: Contained a module On/Off and a module debug level.
 * @param	dlevel: Debug message showed level for seal like MDEBUG.
 * @param	expand: Expanded condition if level param and module ON/OFF are not
 * 			        enough for developer.
 * @param	msg: The debug message.
 * @param	arg: Arguement shown in debug message which like printf arguement.
 * @retval	None
 */
#define _XR_DEBUG(module, dlevel, expand, msg, arg...)	\
		do { \
			if ( \
				((module) & DBG_ON) && \
				(((module) & DBG_LEVEL_MASK) >= dlevel) && \
				(expand)) { \
				XR_DEBUG_PRINT(msg, ##arg); \
			} \
		} while(0)

/*
 * @brief	The realization of showing debug messages and it can't be turn off by
 * 			module ON/OFF.
 * @param	module: Contained a module On/Off and a module debug level.
 * @param	dlevel: Debug message showed level for seal.
 * @param	expand: Expanded condition if level param is not enough for developer.
 * @param	msg: The debug message.
 * @param	arg: Arguement shown in debug message which like printf arguement.
 * @retval	None
 */
#define _XR_INFO(module, dlevel, expand, msg, arg...)	\
		do { \
			if ( \
				(((int16_t)(module) & DBG_LEVEL_MASK) >= dlevel) && \
				(expand)) { \
				XR_DEBUG_PRINT(msg, ##arg); \
			} \
		} while(0)

/*
 * @brief	The realization of assert debug messages shown the assert position,
 * 			for example: "[Assert] At module_debug.h line 112 fun _MASSERT: **"
 * @param	module: Contained a module On/Off and a module debug level.
 * @param	dlevel: Debug message showed level for seal.
 * @param	msg: The debug message.
 * @param	arg: Arguement shown in debug message which like printf arguement.
 * @retval	None
 */
#define _XR_ASSERT(assert, module, dlevel, msg, arg...)	\
		_XR_DEBUG(module, dlevel, !(assert), \
				"[Assert] At %s line %d fun %s: " msg, \
				__FILE__, __LINE__, __func__, ##arg)

/*
 * @brief	The realization of assert debug messages shown the assert position,
 * 			and abort. for example: "[Assert] At module_debug.h line 112 fun
 * 			_MASSERT: ***"
 * @param	module: Contained a module On/Off and a module debug level.
 * @param	dlevel: Debug message showed level for seal.
 * @param	msg: The debug message.
 * @param	arg: Arguement shown in debug message which like printf arguement.
 * @retval	None
 */
#define _XR_ASSERT_ABORT(assert, module, dlevel, msg, arg...)	\
		do { \
			if ((((int16_t)(module) & DBG_LEVEL_MASK) >= dlevel) && !(assert)) { \
				XR_DEBUG_PRINT("[Assert] At %s line %d fun %s: " msg, \
							 __FILE__, __LINE__, __func__, ##arg); \
				XR_DEBUG_ABORT(); \
			} \
		} while(0)


/*
 * @brief	A level debug message
 * @param	module: Contained a module On/Off and a module debug level.
 * @param	expand: Expanded condition if level param and module ON/OFF are not
 * 			        enough for developer.
 * @param	msg: The debug message.
 * @param	arg: Arguement shown in debug message which like printf arguement.
 * @retval	None
 */
#define XR_ERROR(module, expand, msg, arg...) \
		_XR_DEBUG(module, XR_LEVEL_ERROR, expand, msg, ##arg)

#define XR_ALERT(module, expand, msg, arg...) \
		_XR_DEBUG(module, XR_LEVEL_ALERT, expand, msg, ##arg)

#define XR_CRIT(module, expand, msg, arg...) \
		_XR_DEBUG(module, XR_LEVEL_CRIT, expand, msg, ##arg)

#define XR_EMERG(module, expand, msg, arg...) \
		_XR_DEBUG(module, XR_LEVEL_EMERG, XR_xpand, msg, ##arg)

#define XR_WARN(module, expand, msg, arg...) \
		_XR_DEBUG(module, XR_LEVEL_WARNING, expand, msg, ##arg)

#define XR_NOTICE(module, expand, msg, arg...) \
		_XR_DEBUG(module, XR_LEVEL_NOTICE, expand, msg, ##arg)

#define XR_INFO(module, expand, msg, arg...) \
		_XR_DEBUG(module, XR_LEVEL_INFO, expand, msg, ##arg)

#define XR_DEBUG(module, expand, msg, arg...) \
		_XR_DEBUG(module, XR_LEVEL_DEBUG, expand, msg, ##arg)


/*
 * @brief	Assert a full debug message with position(file, line, etc.) without level.
 * 			for example: "[Assert] At module_debug.h line 112 fun _MASSERT: ***"
 * @param	assert: Debug condition
 * @param	module: Contained a module On/Off at least.
 * @param	msg: The debug message.
 * @param	arg: Arguement shown in debug message which like printf arguement.
 * @retval	None
 */
#define XR_ASSERT(assert, module, msg, arg...) \
		_XR_ASSERT(assert, module, XR_LEVEL_ALL, msg, ##arg)

#define XR_ASSERT_ABORT(assert, module, msg, arg...) \
		_XR_ASSERT_ABORT(assert, module, XR_LEVEL_ALL, msg, ##arg)

/*
 * @brief	Assert a full debug message with position(file, line, etc.) and
 * 			error number without level. for example:
 * 			"[Assert] At module_debug.h line 112 fun _MASSERT: condition p != NULL is fault. errno is 115."
 * @param	condition: It will assert a message if condition is fault.
 * @retval	Nuon
 */
#ifndef assert
#define assert(condition) XR_ASSERT(condition, MOD_DBG_ALW_ON, "condition %s is fault. errno is %d.\n", #condition, xr_thread_errno)
#endif

/*
// THIS REALIZATION DO NOT SEAL
#define XR_ASSERT(assert, module, msg, arg...) \
		_XR_ASSERT(assert, module, XR_LEVEL_ALL, "[%s]" msg, #module, ##arg)

#define XR_ASSERT_ABORT(assert, module, msg, arg...) \
		_XR_ASSERT_ABORT(assert, module, XR_LEVEL_ALL, "[%s]" msg, #module, ##arg)
*/


/*
 * @brief	notify the function entry and exit/return in the debug level
 * @param	module: Contained a module On/Off at least.
 * @param	mname: module name in string
 * @param	ret: return value
 * @retval	None
 */
#define XR_ENTRY(module, mname) \
		XR_DEBUG(module, NOEXPAND, mname "entry %s().\n", __func__)

#define XR_RET(module, mname, ret) \
		XR_DEBUG(module, NOEXPAND, mname "exit %s() with return %d.\n", __func__, ret)

#define XR_RET_NOVAL(module, mname) \
		XR_DEBUG(module, NOEXPAND, mname "exit %s().\n", __func__)


#else /* MDEBUG_ON */

#define XR_DEBUG_PRINT(msg, arg...)

#define XR_DEBUG_ABORT()


#define _XR_DEBUG(module, dlevel, expand, msg, arg...)

#define _XR_INFO(module, dlevel, expand, msg, arg...)

#define _XR_ASSERT(assert, module, dlevel, msg, arg...)

#define _XR_ASSERT_ABORT(assert, module, dlevel, msg, arg...)


#define XR_ERROR(module, expand, msg, arg...)

#define XR_ALERT(module, expand, msg, arg...)

#define XR_CRIT(module, expand, msg, arg...)

#define XR_EMERG(module, expand, msg, arg...)

#define XR_WARN(module, expand, msg, arg...)

#define XR_NOTICE(module, expand, msg, arg...)

#define XR_INFO(module, expand, msg, arg...)

#define XR_DEBUG(module, expand, msg, arg...)


#define XR_ASSERT(assert, module, msg, arg...)

#define XR_ASSERT_ABORT(assert, module, msg, arg...)

#ifndef assert
#define assert(condition)
#endif


#define XR_ENTRY(module, mname)

#define XR_RET(module, mname, ret)

#define XR_RET_NOVAL(module, mname)

#endif /* XR_DEBUG_ON */

/*
//$ Example of xr_debug from mqtt $

#define MQTT_MODULE (DBG_ON | XR_LEVEL_DEBUG)

#ifdef MOTT_ASSERT_ON
#define MQTT_ASSERT(assert, msg, arg...) XR_ALERT(MOD_DBG_ALW_ON, (assert), "[MQTT assert] "msg, ##arg)
#else
#define MQTT_ASSERT(assert, msg, arg...)
#endif

#ifdef MQTT_DBG_ON

#define MQTT_INFO(msg, arg...) XR_INFO(MQTT_MODULE, NOEXPAND, "[MQTT info] " msg, ##arg)

#define MQTT_WARN(msg, arg...) XR_WARN(MQTT_MODULE, NOEXPAND, "[MQTT warning] " msg, ##arg)

#define MQTT_DEBUG(msg, arg...) XR_DEBUG(MQTT_MODULE, NOEXPAND, "[MQTT debug] " msg, ##arg)



#define MQTT_ENTRY() XR_ENTRY(MQTT_MODULE, "[MQTT entry] ")

#define MQTT_EXIT(ret) XR_RET(MQTT_MODULE, "[MQTT return] ", ret)

#else

#define MQTT_INFO(msg, arg...)

#define MQTT_WARN(msg, arg...)

#define MQTT_DEBUG(msg, arg...)


#define MQTT_ENTRY()

#define MQTT_EXIT(ret)

#endif

*/

extern void print_hex_dump_bytes(const void *addr, unsigned int len);
extern void hex_dump_bytes(const void *addr, unsigned int len);

#endif /* __XR_DEBUG_H__ */
