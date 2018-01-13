/*********************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 **********************************************************/
#ifndef __TYPE_H__
#define __TYPE_H__

typedef enum {DISABLE = 0, ENABLE = !DISABLE} TYPE_FUNCEN,FuncState,TYPE_PINTIE,TYPE_PINTMASK;
typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus, PinStatus;
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;


#endif
