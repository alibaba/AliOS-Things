
#ifndef __ASR_TIMER_H__
#define __ASR_TIMER_H__

#include <project.h>
#include <stdbool.h>


/* Callback function prototype for alarm callback */
typedef void (*asr_timer_alarm_callback_t) (void);

/* Initialize the Timer settings */
void Asr_Timer_Init(void);

/* Disable the RTC and stop alarm */
void Asr_Timer_Disable(void);

/* Set timeout alarm */
void Asr_SetTimeout(uint32 timeout);

/*Get cuurent count*/
uint32 Asr_GetCount();

/* Register the alarm callback function */
bool Asr_Timer_RegisterAlarmCallback(asr_timer_alarm_callback_t alarmCallback);

#endif /* ASR_TIMER_H */

