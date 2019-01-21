#ifndef LOW_POWER_H
#define LOW_POWER_H

#include "attrs.h"
#include <stdint.h>

enum lowpower_option {
    E_LOW_POWER_ACTIVE      = 0,
    E_LOW_POWER_SLEEP       = 1,  //< Tick will not accuratcy
    //E_LOW_POWER_HIBERNATE   = 2,
};
extern enum lowpower_option g_lowpower_option;
void lowpower_mode(enum lowpower_option);
void system_delay(int delaytick) ATTRIBUTE_SECTION_FAST;
int sys_sleep(uint32_t) ATTRIBUTE_SECTION_FAST;
void sys_hibernate(int, uint32_t) ATTRIBUTE_SECTION_FAST;

#endif /* end of include guard */
