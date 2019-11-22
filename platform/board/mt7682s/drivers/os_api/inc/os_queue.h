#ifndef _OS_QUEUE_H
#define _OS_QUEUE_H

#include "k_api.h"
#include "aos/kernel.h"
#include "os_config.h"
typedef void* os_queue_handle_t;
typedef kbuf_queue_t os_queue__t;

#if OS_QUEUE_DEBUG_ERROR
#define queue_debug_error(fmt,arg...)	\
	do{					\
		os_debug("[error F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(fmt,##arg);	\
	  }while(0);
#else
#define queue_debug_error(fmt,arg...)
#endif	

#if OS_QUEUE_DEBUG_INFO
#define queue_debug_info(fmt,arg...)	\
	do{					\
		os_debug("[info F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(fmt,##arg);	\
	  }while(0);
#else
#define queue_debug_info(fmt,arg...)
#endif


#endif