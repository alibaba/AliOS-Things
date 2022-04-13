/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
modification history
--------------------
21jan2018,WangMin  writen.
*/

/*
DESCRIPTION
This file provides base type of cpu set and method for cpu set.
*/

#ifndef __cpuset_h__
#define __cpuset_h__

#include "k_atomic.h"

#ifdef __cplusplus
extern "C" {
#endif

/* typedefs */

typedef unsigned int             cpuset_t;

#define CPUSET_ATOMIC_SET(cpuset, n)                \
	(void) rhino_atomic_or ((atomic_t *) &(cpuset), 1 << (n))
#define CPUSET_ATOMIC_CLR(cpuset, n)                \
	(void) rhino_atomic_and ((atomic_t *) &(cpuset), ~((1 << (n))))
#define CPUSET_ATOMIC_COPY(cpusetDst, cpusetSrc)    \
    (void) rhino_atomic_set ((atomic_t *) &(cpusetDst), (atomic_t) (cpusetSrc))

#define CPUSET_CLR(cpuset, n)       ((cpuset) &= ~(1 << (n)))
#define CPUSET_ZERO(cpuset)         ((cpuset) = 0)
#define CPUSET_IS_ZERO(cpuset)      ((cpuset) == 0)
#define CPUSET_SET(cpuset, n)       ((cpuset) |= (1 << (n)))
#define CPUSET_ISSET(cpuset, n)     ((cpuset) & (1 << (n)))

#ifdef __cplusplus
}
#endif

#endif /* __cpuset_h__ */

