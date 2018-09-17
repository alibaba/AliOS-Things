SMP Primes application
============================

Demonstrates symmetric multiprocessing (SMP).


Description
-----------

This application uses multiple CPU cores running in parallel to find prime numbers.
It demonstrates starting multiple cores, configuring cores to participate in SMP and
locking access to shared memory resources.

It was derived from the ARM DS-5 smp_primes example.

Requirements
------------

No extra hardware is required.


Build options
-------------

None.


Code organization
-----------------

There are two source files in this application.

main.c - This file contains code to start and configure the CPU cores. The maximum
    number of CPUs to use is configure with the MAX_CPUS define. If fewer than MAX_CPUS
    are present, the actual number of CPUs will be used.

primes.c - This file contains the prime number search code in the the function calculatePrimes().
    The number of primes to search for is configured with the TARGET_COUNT define.
