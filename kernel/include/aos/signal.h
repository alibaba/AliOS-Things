/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */

#ifndef AOS_SIGNAL_H
#define AOS_SIGNAL_H

#include <stdint.h>
#include "k_signal.h"
#include <aos/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Send a signal to a process
 * @param[in]   pid  The process id that send a signal to.
 * @param[in]   sig  The signal number to be sent.
 * @return      0: success, otherwise error.
 */
int aos_kill(uint32_t pid, int sig);

/**
 * Send a signal to a task
 * @param[in]   aos_task  The task that send a signal to.
 * @param[in]   sig  The signal number to be sent.
 * @return      0: success, otherwise error.
 */
int aos_tkill(aos_hdl_t *aos_task, int sig);

/**
 * Examine pending signals
 * @param[out]   set  The returned signal set that are blocked from delivery to the calling thread
 *                    and that are pending on the process or the calling thread.
 * @return      0: success, otherwise error.
 */
int aos_sigpending(ksigset_t *set);

/**
 * Examine and change blocked signals
 * @param[in]   how  Indicates the way in which the set is changed.
 * @param[in]   set  The new signal set to be set.
 * @param[out]  old  The old blocked signal set.
 * @return      0: success, otherwise error.
 */
int aos_sigprocmask(int how, const ksigset_t *set, ksigset_t *old);

/**
 * Examine and change a signal action
 * @param[in]   sig  The signal number.
 * @param[in]   sighand  The new signal handler to be set.
 * @param[out]  oldsighand  The old signal handler for the signal.
 * @return      0: success, otherwise error.
 */
int aos_sigaction(int sig, const ksighandler_t sighand, ksighandler_t *oldsighand);

#ifdef __cplusplus
}
#endif

#endif /* AOS_SIGNAL_H */
