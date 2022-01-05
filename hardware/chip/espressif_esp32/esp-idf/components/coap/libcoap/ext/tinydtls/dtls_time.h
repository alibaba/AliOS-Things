/*******************************************************************************
 *
 * Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Olaf Bergmann  - initial API and implementation
 *
 *******************************************************************************/

/**
 * @file dtls_time.h
 * @brief Clock Handling
 */

#ifndef _DTLS_DTLS_TIME_H_
#define _DTLS_DTLS_TIME_H_

#include <stdint.h>
#include <sys/time.h>

#include "tinydtls.h"

/**
 * @defgroup clock Clock Handling
 * Default implementation of internal clock. You should redefine this if
 * you do not have time() and gettimeofday().
 * @{
 */

#ifdef WITH_CONTIKI
#include "clock.h"

#elif defined(RIOT_VERSION)

#include <xtimer.h>

#define CLOCK_SECOND (xtimer_ticks_from_usec64(1000000UL).ticks64)

typedef uint64_t clock_time_t;
#else /* WITH_CONTIKI || RIOT_VERSION */

#include <time.h>

#ifndef CLOCK_SECOND
# define CLOCK_SECOND 1000
#endif

typedef uint32_t clock_time_t;
#endif /* WITH_CONTIKI || RIOT_VERSION */

typedef clock_time_t dtls_tick_t;

#ifndef DTLS_TICKS_PER_SECOND
#define DTLS_TICKS_PER_SECOND CLOCK_SECOND
#endif /* DTLS_TICKS_PER_SECOND */

void dtls_clock_init(void);
void dtls_ticks(dtls_tick_t *t);

/** @} */

#endif /* _DTLS_DTLS_TIME_H_ */
