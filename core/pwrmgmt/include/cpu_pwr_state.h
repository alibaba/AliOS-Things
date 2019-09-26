/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

/*
DESCRIPTION
This file provides base type define power state.
*/

#ifndef CPU_PWR_STATE_H
#define CPU_PWR_STATE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef enum {
    PWR_ERR = (-1),
    PWR_OK  = (0)
} pwr_status_t;

/* CPU states */
typedef enum {
    CPU_CSTATE_C0   = 0, /* Operating */
    CPU_CSTATE_C1   = 1, /* Halt -- not executing put powered on */
    CPU_CSTATE_C2   = 2, /* not execuing with lower power than C1 */
    CPU_CSTATE_C3   = 3, /* not execuing with lower power than C2 */
    CPU_CSTATE_C4   = 4, /* not execuing with lower power than C3 */
    CPU_CSTATE_C5   = 5,
    CPU_CSTATE_C6   = 6,
    CPU_CSTATE_C7   = 7,
    CPU_CSTATE_C8   = 8,
    CPU_CSTATE_C9   = 9,
    CPU_CSTATE_C10  = 10,
    CPU_CSTATE_C11  = 11,
    CPU_CSTATE_C12  = 12,
    CPU_CSTATE_C13  = 13,
    CPU_CSTATE_C14  = 14,
    CPU_CSTATE_C15  = 15,
    CPU_CSTATE_C16  = 16,
    CPU_CSTATE_C17  = 17,
    CPU_CSTATE_C18  = 18,
    CPU_CSTATE_C19  = 19,
    CPU_CSTATE_C20  = 20,
    CPU_CSTATE_C21  = 21,
    CPU_CSTATE_C22  = 22,
    CPU_CSTATE_C23  = 23,
    CPU_CSTATE_C24  = 24,
    CPU_CSTATE_C25  = 25,
    CPU_CSTATE_C26  = 26,
    CPU_CSTATE_C27  = 27,
    CPU_CSTATE_C28  = 28,
    CPU_CSTATE_C29  = 29,
    CPU_CSTATE_C30  = 30,
    CPU_CSTATE_C31  = 31,
    CPU_CSTATE_MAX  = 31,
    CPU_CSTATE_NONE = 0xFF
} cpu_cstate_t;

/* Performance states */
typedef enum {
    CPU_PSTATE_P0   = 0, /* Max power and clock */
    CPU_PSTATE_P1   = 1,
    CPU_PSTATE_P2   = 2,
    CPU_PSTATE_P3   = 3,
    CPU_PSTATE_P4   = 4,
    CPU_PSTATE_P5   = 5,
    CPU_PSTATE_P6   = 6,
    CPU_PSTATE_P7   = 7,
    CPU_PSTATE_P8   = 8,
    CPU_PSTATE_P9   = 9,
    CPU_PSTATE_P10  = 10,
    CPU_PSTATE_P11  = 11,
    CPU_PSTATE_P12  = 12,
    CPU_PSTATE_P13  = 13,
    CPU_PSTATE_P14  = 14,
    CPU_PSTATE_P15  = 15,
    CPU_PSTATE_P16  = 16,
    CPU_PSTATE_P17  = 17,
    CPU_PSTATE_P18  = 18,
    CPU_PSTATE_P19  = 19,
    CPU_PSTATE_P20  = 20,
    CPU_PSTATE_P21  = 21,
    CPU_PSTATE_P22  = 22,
    CPU_PSTATE_P23  = 23,
    CPU_PSTATE_P24  = 24,
    CPU_PSTATE_P25  = 25,
    CPU_PSTATE_P26  = 26,
    CPU_PSTATE_P27  = 27,
    CPU_PSTATE_P28  = 28,
    CPU_PSTATE_P29  = 29,
    CPU_PSTATE_P30  = 30,
    CPU_PSTATE_P31  = 31,
    CPU_PSTATE_MAX  = 32,
    CPU_PSTATE_NONE = 0xFF
} cpu_pstate_t;

#ifdef __cplusplus
}
#endif

#endif /* CPU_PWR_STATE_H */
