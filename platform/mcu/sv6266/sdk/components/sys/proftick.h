#ifndef _PROFTICK_H_
#define _PROFTICK_H_

#include <stdint.h>
#include "attrs.h"

void proftick_start(void);
void proftick_stop(void);
uint32_t proftick_elapsed_ms(void);

#endif
