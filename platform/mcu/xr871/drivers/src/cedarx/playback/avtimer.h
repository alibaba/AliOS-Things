/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : avtimer.h
 * Description : audio-video synchronization timer
 * History :
 *
 */

#ifndef AVTIMER_H
#define AVTIMER_H

#include <stdint.h>

//************************************************************//
//***************** define status of a timer *****************//
//************************************************************//
#define TIMER_STATUS_START 0x55
#define TIMER_STATUS_STOP  0xaa

typedef struct AvTimer AvTimer;
struct AvTimer
{
    //* set clock counting speed.
    int (*SetSpeed)(AvTimer* t, int speed);

    //* get the current counting speed.
    int (*GetSpeed)(AvTimer* t);

    //* set time.
    int (*SetTime)(AvTimer* t, int64_t time);

    //* get the current time.
    int64_t (*GetTime)(AvTimer* t);

    //* trasform pts to system time
    //* Careful: this interface should only be used by video render
    int64_t (*PtsToSystemTime)(AvTimer* t, int64_t pts);

    //* run the timer, start clock counting at 'start_time' and running at speed 'speed'.
    int (*Start)(AvTimer* t);

    //* stop the timer.
    void (*Stop)(AvTimer* t);

    int (*GetStatus)(AvTimer* t);

    int (*SetPlayRate)(AvTimer* t, float rate);

};

//* create a timer, which is implemented in file 'avtimer.c'.
AvTimer* AvTimerCreate(void);

void AvTimerDestroy(AvTimer*);

#endif
