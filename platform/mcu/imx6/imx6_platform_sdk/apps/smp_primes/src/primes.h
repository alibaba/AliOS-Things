// ------------------------------------------------------------
// Prime Number Generator Code
// Header
//
// Copyright (c) 2011 ARM Ltd.  All rights reserved.
// ------------------------------------------------------------

#ifndef _PRIMES_H
#define _PRIMES_H

#include "utility/spinlock.h"

// Initializes shared data used during calculation of primes
// Must be called by CPU 0 only, before any CPU calls calculate_primes()
void initPrimes(void);

// Starts calculation of primes on that CPU
// Must be called by each particapting CPU
void calculatePrimes(unsigned int id);

// Shared with main to lock printfs
extern spinlock_t prime_lock;

#endif

// ------------------------------------------------------------
// End of primes.h
// ------------------------------------------------------------
