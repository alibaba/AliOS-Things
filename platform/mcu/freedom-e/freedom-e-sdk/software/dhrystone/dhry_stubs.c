#include "platform.h"

/* The functions in this file are only meant to support Dhrystone on an
 * embedded RV32 system and are obviously incorrect in general. */

long time(void)
{
  return get_timer_value() / get_timer_freq();
}

// set the number of dhrystone iterations
void __wrap_scanf(const char* fmt, int* n)
{
  *n = 100000000;
}
