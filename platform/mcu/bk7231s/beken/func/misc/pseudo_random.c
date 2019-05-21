#include "include.h"
#include "reg_mac_core.h"

uint32_t prandom_get(void)
{
    return nxmac_monotonic_counter_2_lo_get();
}

// eof

