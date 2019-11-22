#ifndef _OS_SEM_H
#define _OS_SEM_H

#include "k_api.h"
#include "aos/kernel.h"
#include "os_config.h"
typedef void* os_sem_handle_t;
typedef ksem_t os_sem__t;

#if OS_SEM_DEBUG_ERROR
#define sem_debug_error(fmt,arg...)	\
	do{					\
		os_debug("[error F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(fmt,##arg);	\
	  }while(0);
#else
#define sem_debug_error(fmt,arg...)
#endif	

#if OS_SEM_DEBUG_INFO
#define sem_debug_info(fmt,arg...)	\
	do{					\
		os_debug("[info F:%s L:%d]",__FUNCTION__,__LINE__);	\
		os_debug(fmt,##arg);	\
	  }while(0);
#else
#define sem_debug_info(fmt,arg...)
#endif

#endif