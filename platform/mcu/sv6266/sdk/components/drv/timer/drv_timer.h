#ifndef _DRV_TIMER_H_
#define _DRV_TIMER_H_

#include "soc_types.h"
#include "soc_defs.h"

//#include <turismo_regs.h>

typedef enum {
	US_TIMER_0,
	US_TIMER_1,
	US_TIMER_2,
	US_TIMER_3,
	MS_TIMER_0,
	MS_TIMER_1,
	MS_TIMER_2,
	MS_TIMER_3,
	TIMER_ID_MIN = US_TIMER_0,
	TIMER_ID_MAX = MS_TIMER_3
} TimerID_E;

#define SYS_TICK_TIMER					US_TIMER_1
#define MAX_HW_TIMER                    8

/**
 * Hardware Timer Operation Mode
 *
 * @ HTMR_ONESHOT: Timer counts down to 0 and then stop.
 * @ HTMR_PERIODIC: Timer counts down to 0 and the reload.
 */
typedef enum hwtmr_op_mode {
    HTMR_ONESHOT = 0,
    HTMR_PERIODIC = 1,
} TimerMode_E;

/* HW just supports 16 bits timer for now */
#if 0
void hwtmr_init(void);
#endif

typedef void (*timer_handler) (void *m_data);

void hwtmr_init (TimerID_E timer_id, u32 period, TimerMode_E mode);
void hwtmr_enable (TimerID_E timer_id, u32 enable);
u32 hwtmr_get_count (TimerID_E timer_id);
u32 hwtmr_read_count (TimerID_E timer_id);
void hwtmr_clear_irq (TimerID_E timer_id);
u32 hwtmr_get_irq (TimerID_E timer_id);
u32 hwtmr_get_status (TimerID_E timer_id);
s32 hwtmr_stop(TimerID_E);
s32 hwtmr_start(TimerID_E timer_id, u16 count, timer_handler tmr_handler,
                void *m_data, enum hwtmr_op_mode mode);

#endif /* _DRV_TIMER_H_ */

