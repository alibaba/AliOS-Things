/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "basic_types.h"
#include <stdarg.h>
#include <stddef.h>             /* Compiler defns such as size_t, NULL etc. */
#include "strproc.h"
#include "section_config.h"
#include "diag.h"
#include "ameba_soc.h"

LIBC_ROM_TEXT_SECTION
_LONG_CALL_ int _stratoi(const char * s)
{

	int num=0,flag=0;
	u32 i;
	for(i=0;i<=_strlen(s);i++)
	{
	  if(s[i] >= '0' && s[i] <= '9')
		 num = num * 10 + s[i] -'0';
	  else if(s[0] == '-' && i==0) 
		 flag =1;
	  else 
		  break;
	 }

	if(flag == 1)
	   num = num * -1;

	 return(num); 

}

