/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* ARM configuration file; HZ is 100 rather than the default 60 */

#ifndef _SYS_PARAM_H
# define _SYS_PARAM_H

#include <machine/param.h>
#include <machine/endian.h>

#ifndef NBBY
# define NBBY 8		/* number of bits in a byte */
#endif
#ifndef HZ
# define HZ (60)
#endif
#ifndef NOFILE
# define NOFILE (60)
#endif
#ifndef PATHSIZE
# define PATHSIZE (1024)
#endif

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#endif
