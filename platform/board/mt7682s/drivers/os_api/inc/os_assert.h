#ifndef _OS_ASSERT_H
#define _OS_ASSERT_H

#include <stdio.h>
#define assert_debug printf

#define NULL_PARA_CHECK(x,err)	\
	do{					\
		if(NULL == x) 		\
		{	\
			assert_debug("[error]assert fail in function <%s> line <%d> file <%s>\r\n",__FUNCTION__,__LINE__,__FILE__);	\
			return err;	\
		}	\
	  }while(0);
#endif
