/**
 * @file lv_conf.h
 *
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#ifdef LITTLEVGL_STARTERKIT
#include "lv_conf_starterkit.h"
#endif

#ifdef LITTLEVGL_SIMULATE
#include "lv_conf_simulate.h"
#endif

#ifdef LITTLEVGL_DEVELOPERKIT
#include "lv_conf_developerkit.h"
#endif

#endif /*LV_CONF_H*/
