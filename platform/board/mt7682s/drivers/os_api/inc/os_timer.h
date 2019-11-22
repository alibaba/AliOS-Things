#ifndef _OS_TIMER_H
#define _OS_TIMER_H

#include "k_api.h"
#include "aos/kernel.h"
#include "os_config.h"
typedef void* os_timer_handle_t;
typedef ktimer_t os_timer__t;
#define timer_debug printf

#if OS_TIMER_DEBUG_ERROR
#define timer_debug_error(fmt,arg...)	\
	do{														\
		os_debug("[error F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(fmt,##arg);	\
	  }while(0);
#else
#define timer_debug_error(fmt,arg...)
#endif	

#if OS_TIMER_DEBUG_INFO
#define timer_debug_info(fmt,arg...)	\
	do{					\
		os_debug("[info F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(fmt,##arg);	\
	  }while(0);
#else
#define timer_debug_info(fmt,arg...)
#endif

typedef void (*timer_callback_t)( void* xTimer ,void*arg);

#endif