

#ifndef _BEEPER_H_
#define _BEEPER_H_

#include <stdint.h>

void beeper_start(uint16_t port, uint16_t frequency, uint16_t duration);
void beeper_stop(uint16_t port);

#endif
