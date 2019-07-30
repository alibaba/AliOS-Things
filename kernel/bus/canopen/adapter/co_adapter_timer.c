/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifdef AOS_CANOPEN

#include "co_adapter.h"

static timer_dev_t timer_canopen;
void timer_dispatch();

TIMEVAL last_counter_val = 0;
TIMEVAL elapsed_time = 0;

int32_t timer_init(uint8_t port)
{
  int32_t ret = -1;

  timer_canopen.port = port;
  timer_canopen.config.reload_mode = TIMER_RELOAD_AUTO;
  timer_canopen.config.period = MAX_COUNTER_VALUE;
  timer_canopen.config.cb = timer_dispatch;

  ret = hal_timer_init(&timer_canopen);
  if (ret != 0) {
      LOGD("canopen", "timer init failed!\n");
      return -1;
  }

  ret = hal_timer_start(&timer_canopen);
  if (ret != 0) {
      LOGD("canopen", "timer start failed!\n");
      return -1;
  }
  return ret;
}

/* setTimer is a protocol library interface implement */
void setTimer(TIMEVAL value)
{
    uint32_t timer = hal_timer_getcounter(&timer_canopen);
    elapsed_time += timer - last_counter_val;
    last_counter_val = MAX_COUNTER_VALUE-value;
    hal_timer_setcounter(&timer_canopen, MAX_COUNTER_VALUE-value);
}

/* getElapsedTime is a protocol library interface implement */
TIMEVAL getElapsedTime(void)
{
    uint32_t timer = hal_timer_getcounter(&timer_canopen);
    if (timer < last_counter_val) {
        timer += MAX_COUNTER_VALUE;
    }
    TIMEVAL elapsed = timer - last_counter_val + elapsed_time;
    return elapsed;
}

void timer_dispatch()
{
    last_counter_val = 0;
    elapsed_time = 0;
    TimeDispatch();
}
#endif    /* AOS_CANOPEN */
