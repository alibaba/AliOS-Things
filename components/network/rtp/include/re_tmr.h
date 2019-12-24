/**
 * @file re_tmr.h  Interface to timer implementation
 *
 * Copyright (C) 2010 Creytiv.com
 */

#ifndef RE_TMR_H
#define RE_TMR_H
#include "re_common.h"
struct re_printf;


/**
 * Defines the timeout handler
 *
 * @param arg Handler argument
 */
typedef void (tmr_h)(void *arg);

/** Defines a timer */
struct tmr {
	struct le le;       /**< Linked list element */
	tmr_h *th;          /**< Timeout handler     */
	void *arg;          /**< Handler argument    */
	uint64_t jfs;       /**< Jiffies for timeout */
};


void     tmr_poll(struct list *tmrl);
uint64_t tmr_jiffies(void);
uint64_t tmr_next_timeout(struct list *tmrl);
void     tmr_debug(void);
int      tmr_status(struct re_printf *pf, void *unused);

void     tmr_init(struct tmr *tmr);
void     tmr_start(struct tmr *tmr, uint64_t delay, tmr_h *th, void *arg);
void     tmr_cancel(struct tmr *tmr);
uint64_t tmr_get_expire(const struct tmr *tmr);


/**
 * Check if the timer is running
 *
 * @param tmr Timer to check
 *
 * @return true if running, false if not running
 */
static inline bool tmr_isrunning(const struct tmr *tmr)
{
	return tmr ? NULL != tmr->th : false;
}
#endif /* RE_TMR_H */
