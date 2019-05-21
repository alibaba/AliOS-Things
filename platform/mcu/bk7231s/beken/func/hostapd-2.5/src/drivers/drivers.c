/*
 * Driver interface list
 * Copyright (c) 2004-2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "driver.h"

#ifdef CONFIG_DRIVER_BEKEN
extern struct wpa_driver_ops wpa_driver_hostap_ops; /* driver_hostap.c */
#endif /* CONFIG_DRIVER_BEKEN */

const struct wpa_driver_ops *const wpa_drivers[] =
{
#ifdef CONFIG_DRIVER_BEKEN
	&wpa_driver_hostap_ops,	
#endif /* CONFIG_DRIVER_BEKEN */

	NULL
};
