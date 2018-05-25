#ifndef __RATE_CONTROL_H__
#define __RATE_CONTROL_H__

#include "soc_types.h"

#define MAX_UPRATECNT 10
#define MAX_DOWNRATECNT 2
#define SHIFTBITS 16
#define CCK_SIFS_TIME  10
#define OFDM_SIFS_TIME 16
#define HT_SIFS_TIME   10
#define HT_SIGNAL_EXT  6

#define ACKLEN			14
#define RTSLEN			20
#define CTSLEN			14
#define FCSLEN          4

typedef struct t_DURATION_TABLE
{
    u32 multiplevalue;
    u8  ctrl_rate_idx;
}DURATION_TABLE;

void init_ratecontrol(u8 wsid, u8 htsupport, u8 ratenum, u8 *supportrate);
void datarate_down(u8 wsid);
void datarate_up(u8 wsid);
void setup_custom_rate(u8 rate, u8 wsid);
u8 get_current_rate(u8 wsid);

#endif
