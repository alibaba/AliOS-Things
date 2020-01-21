#include "rtc-board.h"
#include "delay-board.h"

void DelayMsMcu( uint32_t ms )
{
    RtcDelayMs( ms );
}

