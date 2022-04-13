#ifndef _AOS_MUTEX_H
#define _AOS_MUTEX_H

#include "k_api.h"
#include "aos/kernel.h"
#include "aos_config.h"
typedef void* os_mutex_handle_t;
typedef kmutex_t os_mutex__t;

#if OS_MUTEX_DEBUG_ERROR
#define mutex_debug_error(fmt,arg...)	\
	do{					\
		os_debug("[error F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(arg);	\
	  }while(0);
#else
#define mutex_debug_error(fmt,arg...)
#endif

#if OS_MUTEX_DEBUG_INFO
#define mutex_debug_info(fmt,arg...)	\
	do{					\
		os_debug("[info F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(fmt,##arg);	\
	  }while(0);
#else
#define mutex_debug_info(fmt,arg...)
#endif

int _os_mutex_delete(os_mutex_handle_t mutex_handle);
os_mutex_handle_t _os_mutex_create(void);
#endif