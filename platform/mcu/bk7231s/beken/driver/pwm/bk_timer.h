#ifndef _BK_TIMER_H_
#define _BK_TIMER_H_

#define BK_TIMER_FAILURE                (1)
#define BK_TIMER_SUCCESS                (0)

#include "pwm.h"

#if (CFG_SOC_NAME != SOC_BK7231)
#define TIMER0_CNT                                     (PWM_NEW_BASE + 0 * 4)

#define TIMER1_CNT                                     (PWM_NEW_BASE + 1 * 4)

#define TIMER2_CNT                                     (PWM_NEW_BASE + 2 * 4)

#define TIMER0_2_CTL                                   (PWM_NEW_BASE + 3 * 4)
#define TIMERCTL0_EN_BIT                               (0x01UL << 0)
#define TIMERCTL1_EN_BIT                               (0x01UL << 1)
#define TIMERCTL2_EN_BIT                               (0x01UL << 2)
#define TIMERCTLA_CLKDIV_POSI                          (3)
#define TIMERCTLA_CLKDIV_MASK                          (0x07)
#define TIMERCTL0_INT_BIT                              (0x01UL << 7)
#define TIMERCTL1_INT_BIT                              (0x01UL << 8)
#define TIMERCTL2_INT_BIT                              (0x01UL << 9)
#define TIMERCTLA_INT_POSI                             (7)
#define REG_TIMERCTLA_PERIOD_ADDR(n)                    (PWM_NEW_BASE +  0x04 * (n))

#if (CFG_SOC_NAME == SOC_BK7231U)
#define TIMER0_2_READ_CTL                             (PWM_NEW_BASE + 4 * 4)
#define TIMER0_2_READ_OP_BIT                          (1<<0)
#define TIMER0_2_READ_INDEX_POSI                       (2)
#define TIMER0_2_READ_INDEX_MASK                       (0x3)
#define TIMER0_2_READ_INDEX_0                          (0)
#define TIMER0_2_READ_INDEX_1                          (1)
#define TIMER0_2_READ_INDEX_2                          (2)

#define TIMERR0_2_READ_VALUE                           (PWM_NEW_BASE + 5 * 4)
#endif

#define TIMER3_CNT                                     (PWM_NEW_BASE + 0x10 * 4)

#define TIMER4_CNT                                     (PWM_NEW_BASE + 0x11 * 4)

#define TIMER5_CNT                                     (PWM_NEW_BASE + 0x12 * 4)

#define TIMER3_5_CTL                                   (PWM_NEW_BASE + 0x13 * 4)
#define TIMERCTL3_EN_BIT                               (0x01UL << 0)
#define TIMERCTL4_EN_BIT                               (0x01UL << 1)
#define TIMERCTL5_EN_BIT                               (0x01UL << 2)
#define TIMERCTLB_CLKDIV_POSI                          (3)
#define TIMERCTLB_CLKDIV_MASK                          (0x07)
#define TIMERCTL3_INT_BIT                              (0x01UL << 7)
#define TIMERCTL4_INT_BIT                              (0x01UL << 8)
#define TIMERCTL5_INT_BIT                              (0x01UL << 9)
#define TIMERCTLB_INT_POSI                              (7)
#define REG_TIMERCTLB_PERIOD_ADDR(n)                    (PWM_NEW_BASE + 0x10 * 4 + 0x04 * (n - 3))

#if (CFG_SOC_NAME == SOC_BK7231U)
#define TIMER3_5_READ_CTL                             (PWM_NEW_BASE + 0x14 * 4)
#define TIMER3_5_READ_OP_BIT                          (1<<0)
#define TIMER3_5_READ_INDEX_POSI                       (2)
#define TIMER3_5_READ_INDEX_MASK                       (0x3)
#define TIMER3_5_READ_INDEX_3                          (0)
#define TIMER3_5_READ_INDEX_4                          (1)
#define TIMER3_5_READ_INDEX_5                          (2)

#define TIMER3_5_READ_VALUE                           (PWM_NEW_BASE + 0x15 * 4)
#endif

#define TIMER_CHANNEL_NO                                  6
#endif
UINT32 bk_timer_ctrl(UINT32 cmd, void *param);

#endif //_TIMER_H_


