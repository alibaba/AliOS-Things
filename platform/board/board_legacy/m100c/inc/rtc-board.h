#ifndef __RTC_H
#define __RTC_H

#define LFRCO_FREQUENCY                 32768
#define WAKEUP_INTERVAL_MS              500
#define RTC_COUNT_BETWEEN_WAKEUP        (((LFRCO_FREQUENCY * WAKEUP_INTERVAL_MS) / 1000)-1)

void rtc_setup(void);

#endif
