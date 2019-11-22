#ifndef _OS_EVENT_H
#define _OS_EVENT_H

#include "k_api.h"
#include "aos/kernel.h"
#include "os_config.h"
#include "typedef.h"
typedef void* os_event_handle_t;
typedef kevent_t os_event__t;
typedef  u32_t os_event_bits_t;
#if OS_EVENT_DEBUG_ERROR
#define event_debug_error(fmt,arg...)	\
	do{					\
		os_debug("[error F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(fmt,##arg);	\
	  }while(0);
#else
#define event_debug_error(fmt,arg...)
#endif	

#if OS_EVENT_DEBUG_INFO
#define event_debug_info(fmt,arg...)	\
	do{					\
		os_debug("[info F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(fmt,##arg);	\
	  }while(0);
#else
#define event_debug_info(fmt,arg...)
#endif

#endif