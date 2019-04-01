#ifndef _SSVRADIO_H_
#define _SSVRADIO_H_

#include "soc_types.h"
#include "radio.h"

#define SSV6200_MAX_PACKET_LEN     16
#define MAC_REG_WRITE(_r, _v)   hw_reg_write(_r, _v)
#define MAC_REG_READ(_r, _v)   hw_reg_read(_r, _v)
#define RADIO_TASK_SLEEP_TIME 3000 //unit is ms

#endif /* _SSVRADIO_H_ */
