/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* This header file provides the reentrancy.  */

/* WARNING: All identifiers here must begin with an underscore.  This file is
   included by stdio.h and others and we therefore must only use identifiers
   in the namespace allotted to us.  */

#ifndef _SYS_REENT_H_
#ifdef __cplusplus
extern "C" {
#endif
#define _SYS_REENT_H_
#include <errno.h>
/* This version of _reent is laid out with "int"s in pairs, to help
 * ports with 16-bit int's but 32-bit pointers, align nicely.  */
struct _reent
{
  /* As an exception to the above put _errno first for binary
     compatibility with non _REENT_SMALL targets.  */
  int _errno;			/* local copy of errno */

  
};

#ifdef __cplusplus
}
#endif
#endif /* _SYS_REENT_H_ */
