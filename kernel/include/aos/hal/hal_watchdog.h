/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2014
 * Chen-Yu Tsai <wens@csie.org>
 *
 * Watchdog register definitions
 */

#ifndef _HAL_WATCHDOG_H_
#define _HAL_WATCHDOG_H_

void hal_watchdog_disable(void);
void hal_watchdog_reset(int timeout);
void hal_reset_cpu(void);

#endif /* _HAL_WATCHDOG_H_ */
