/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef MBEDTLS_TIMING_H
#define MBEDTLS_TIMING_H

#if !defined(MBEDTLS_CONFIG_FILE)
    #include "config.h"
#else
    #include MBEDTLS_CONFIG_FILE
#endif

#if !defined(MBEDTLS_TIMING_ALT)
// Regular implementation
//

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          timer structure
 */
struct mbedtls_timing_hr_time {
    unsigned char opaque[32];
};

/**
 * \brief          Context for mbedtls_timing_set/get_delay()
 */
typedef struct {
    struct mbedtls_timing_hr_time   timer;
    uint32_t                        int_ms;
    uint32_t                        fin_ms;
} mbedtls_timing_delay_context;

extern volatile int mbedtls_timing_alarmed;

/**
 * \brief          Return the CPU cycle counter value
 *
 * \warning        This is only a best effort! Do not rely on this!
 *                 In particular, it is known to be unreliable on virtual
 *                 machines.
 */
unsigned long mbedtls_timing_hardclock(void);

/**
 * \brief          Return the elapsed time in milliseconds
 *
 * \param val      points to a timer structure
 * \param reset    if set to 1, the timer is restarted
 */
unsigned long mbedtls_timing_get_timer(struct mbedtls_timing_hr_time *val, int reset);

/**
 * \brief          Setup an alarm clock
 *
 * \param seconds  delay before the "mbedtls_timing_alarmed" flag is set
 *
 * \warning        Only one alarm at a time  is supported. In a threaded
 *                 context, this means one for the whole process, not one per
 *                 thread.
 */
DLL_TLS_API void mbedtls_set_alarm(int seconds);

/**
 * \brief          Set a pair of delays to watch
 *                 (See \c mbedtls_timing_get_delay().)
 *
 * \param data     Pointer to timing data
 *                 Must point to a valid \c mbedtls_timing_delay_context struct.
 * \param int_ms   First (intermediate) delay in milliseconds.
 * \param fin_ms   Second (final) delay in milliseconds.
 *                 Pass 0 to cancel the current delay.
 */
DLL_TLS_API void mbedtls_timing_set_delay(void *data, uint32_t int_ms, uint32_t fin_ms);

/**
 * \brief          Get the status of delays
 *                 (Memory helper: number of delays passed.)
 *
 * \param data     Pointer to timing data
 *                 Must point to a valid \c mbedtls_timing_delay_context struct.
 *
 * \return         -1 if cancelled (fin_ms = 0)
 *                  0 if none of the delays are passed,
 *                  1 if only the intermediate delay is passed,
 *                  2 if the final delay is passed.
 */
DLL_TLS_API int mbedtls_timing_get_delay(void *data);

#ifdef __cplusplus
}
#endif

#else  /* MBEDTLS_TIMING_ALT */
#include "timing_alt.h"
#endif /* MBEDTLS_TIMING_ALT */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SELF_TEST)
/**
 * \brief          Checkup routine
 *
 * \return         0 if successful, or 1 if a test failed
 */
DLL_TLS_API int mbedtls_timing_self_test(int verbose);
#endif

#ifdef __cplusplus
}
#endif

#endif /* timing.h */
