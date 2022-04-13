

#ifndef DRV_TIMER_H__
#define DRV_TIMER_H__

int rt_hw_timer_init(void);
unsigned int hw_timer3_count(void);
//void timer_init(int timer, unsigned int preload);
//void timer_clear_pending(int timer);
int hw_timer0_init(int tick_per_second);
unsigned int hw_timer0_count(void);

#endif
