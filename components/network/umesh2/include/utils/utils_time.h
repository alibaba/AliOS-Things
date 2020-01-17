#ifndef _UMESH_UTILS_TIME_H_
#define _UMESH_UTILS_TIME_H_

#include <stdint.h>


uint64_t umesh_time_spend(uint64_t start);

uint64_t umesh_time_left(uint64_t end);

int umesh_time_is_expired(uint64_t time);

int  umesh_time_countdown_ms(uint64_t *time, uint32_t millisecond);
#endif

