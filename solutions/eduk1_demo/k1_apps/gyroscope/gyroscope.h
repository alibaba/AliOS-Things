#ifndef __GYROSCOPE_H__
#define __GYROSCOPE_H__

#include "../menu.h"

extern MENU_TYP gyroscope;

int  gyroscope_init(void);
int  gyroscope_uninit(void);
void gyroscope_task(void);

#endif
