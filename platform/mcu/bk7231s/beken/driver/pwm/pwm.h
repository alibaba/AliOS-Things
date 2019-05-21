#ifndef _PWM_H_
#define _PWM_H_

#define PWM_DEBUG

#ifdef PWM_DEBUG
#define PWM_PRT      os_printf
#define PWM_WARN     warning_prf
#define PWM_FATAL    fatal_prf
#else
#define PWM_PRT      null_prf
#define PWM_WARN     null_prf
#define PWM_FATAL    null_prf
#endif

#define PWM_NEW_BASE                                 (0x00802A00)

#if (CFG_SOC_NAME == SOC_BK7231)
#define PWM_BASE                                     (PWM_NEW_BASE)
#else
#define PWM_BASE                                     (PWM_NEW_BASE + 0x20 * 4 )
#endif

#define PWM_CTL                                      (PWM_BASE + 0 * 4)

#define PWM0_EN_BIT                                  (0x01UL << 0)
#define PWM0_INT_EN_BIT                              (0x01UL << 1)
#define PWM0_MODE_POSI                               (2)
#define PWM0_MODE_MASK                               (0x03)

#define PWM1_EN_BIT                                  (0x01UL << 4)
#define PWM1_INT_EN_BIT                              (0x01UL << 5)
#define PWM1_MODE_POSI                               (6)
#define PWM1_MODE_MASK                               (0x03)

#define PWM2_EN_BIT                                  (0x01UL << 8)
#define PWM2_INT_EN_BIT                              (0x01UL << 9)
#define PWM2_MODE_POSI                               (10)
#define PWM2_MODE_MASK                               (0x03)

#define PWM3_EN_BIT                                  (0x01UL << 12)
#define PWM3_INT_EN_BIT                              (0x01UL << 13)
#define PWM3_MODE_POSI                               (14)
#define PWM3_MODE_MASK                               (0x03)

#define PWM4_EN_BIT                                  (0x01UL << 16)
#define PWM4_INT_EN_BIT                              (0x01UL << 17)
#define PWM4_MODE_POSI                               (18)
#define PWM4_MODE_MASK                               (0x03)

#define PWM5_EN_BIT                                  (0x01UL << 20)
#define PWM5_INT_EN_BIT                              (0x01UL << 21)
#define PWM5_MODE_POSI                               (22)
#define PWM5_MODE_MASK                               (0x03)

#define PWM_INTERRUPT_STATUS                         (PWM_BASE + 1 * 4)

#define PWM0_INIT_BIT                                (0x01UL << 0)
#define PWM1_INIT_BIT                                (0x01UL << 1)
#define PWM2_INIT_BIT                                (0x01UL << 2)
#define PWM3_INIT_BIT                                (0x01UL << 3)
#define PWM4_INIT_BIT                                (0x01UL << 4)
#define PWM5_INIT_BIT                                (0x01UL << 5)

#define PWM0_COUNTER                                 (PWM_BASE + 2 * 4)

#if (CFG_SOC_NAME == SOC_BK7231)
#define PWM0_END_POSI                                (0)
#define PWM0_END_MASK                                (0xFFFF)
#define PWM0_DC_POSI                                 (16)
#define PWM0_DC_MASK                                 (0xFFFF)

#define PWM0_CAPTURE                                 (PWM_BASE + 3 * 4)

#define PWM0_CAP_OUT_POSI                            (0)
#define PWM0_CAP_OUT_MASK                            (0xFFFF)

#define PWM1_COUNTER                                 (PWM_BASE + 4 * 4)

#define PWM1_END_POSI                                (0)
#define PWM1_END_MASK                                (0xFFFF)
#define PWM1_DC_POSI                                 (16)
#define PWM1_DC_MASK                                 (0xFFFF)

#define PWM1_CAPTURE                                 (PWM_BASE + 5 * 4)

#define PWM1_CAP_OUT_POSI                            (0)
#define PWM1_CAP_OUT_MASK                            (0xFFFF)

#define PWM2_COUNTER                                 (PWM_BASE + 6 * 4)

#define PWM2_END_POSI                                (0)
#define PWM2_END_MASK                                (0xFFFF)
#define PWM2_DC_POSI                                 (16)
#define PWM2_DC_MASK                                 (0xFFFF)

#define PWM2_CAPTURE                                 (PWM_BASE + 7 * 4)

#define PWM2_CAP_OUT_POSI                            (0)
#define PWM2_CAP_OUT_MASK                            (0xFFFF)

#define PWM3_COUNTER                                 (PWM_BASE + 8 * 4)

#define PWM3_END_POSI                                (0)
#define PWM3_END_MASK                                (0xFFFF)
#define PWM3_DC_POSI                                 (16)
#define PWM3_DC_MASK                                 (0xFFFF)

#define PWM3_CAPTURE                                 (PWM_BASE + 9 * 4)

#define PWM3_CAP_OUT_POSI                            (0)
#define PWM3_CAP_OUT_MASK                            (0xFFFF)

#define PWM4_COUNTER                                 (PWM_BASE + 10 * 4)

#define PWM4_END_POSI                                (0)
#define PWM4_END_MASK                                (0xFFFF)
#define PWM4_DC_POSI                                 (16)
#define PWM4_DC_MASK                                 (0xFFFF)

#define PWM4_CAPTURE                                 (PWM_BASE + 11 * 4)

#define PWM4_CAP_OUT_POSI                            (0)
#define PWM4_CAP_OUT_MASK                            (0xFFFF)

#define PWM5_COUNTER                                 (PWM_BASE + 12 * 4)

#define PWM5_END_POSI                                (0)
#define PWM5_END_MASK                                (0xFFFF)
#define PWM5_DC_POSI                                 (16)
#define PWM5_DC_MASK                                 (0xFFFF)

#define PWM5_CAPTURE                                 (PWM_BASE + 13 * 4)

#define PWM5_CAP_OUT_POSI                            (0)
#define PWM5_CAP_OUT_MASK                            (0xFFFF)

#define REG_APB_BK_PWMn_CNT_ADDR(n)         (PWM_BASE + 0x08 + 2 * 0x04 * (n))
#define REG_APB_BK_PWMn_CAP_ADDR(n)         (PWM_BASE + 0x0C + 2 * 0x04 * (n))

#else
#define PWM0_END_POSI                                (0)
#define PWM0_END_MASK                                (0xFFFFFFFF)

#define PWM0_DUTY_CYCLE                              (PWM_BASE + 3 * 4)

#define PWM0_DC_POSI                                 (0)
#define PWM0_DC_MASK                                 (0xFFFFFFFF)

#define PWM0_CAPTURE                                 (PWM_BASE + 4 * 4)

#define PWM0_CAP_OUT_POSI                            (0)
#define PWM0_CAP_OUT_MASK                            (0xFFFFFFFF)

#define PWM1_COUNTER                                 (PWM_BASE + 5 * 4)

#define PWM1_END_POSI                                (0)
#define PWM1_END_MASK                                (0xFFFFFFFF)

#define PWM1_DUTY_CYCLE                              (PWM_BASE + 6 * 4)

#define PWM1_DC_POSI                                 (0)
#define PWM1_DC_MASK                                 (0xFFFFFFFF)

#define PWM1_CAPTURE                                 (PWM_BASE + 7 * 4)

#define PWM1_CAP_OUT_POSI                            (0)
#define PWM1_CAP_OUT_MASK                            (0xFFFFFFFF)

#define PWM2_COUNTER                                 (PWM_BASE + 8 * 4)

#define PWM2_END_POSI                                (0)
#define PWM2_END_MASK                                (0xFFFFFFFF)

#define PWM2_DUTY_CYCLE                              (PWM_BASE + 9 * 4)

#define PWM2_DC_POSI                                 (0)
#define PWM2_DC_MASK                                 (0xFFFFFFFF)

#define PWM2_CAPTURE                                 (PWM_BASE + 10 * 4)

#define PWM2_CAP_OUT_POSI                            (0)
#define PWM2_CAP_OUT_MASK                            (0xFFFFFFFF)

#define PWM3_COUNTER                                 (PWM_BASE + 11 * 4)

#define PWM3_END_POSI                                (0)
#define PWM3_END_MASK                                (0xFFFFFFFF)

#define PWM3_DUTY_CYCLE                              (PWM_BASE + 12 * 4)

#define PWM3_DC_POSI                                 (0)
#define PWM3_DC_MASK                                 (0xFFFFFFFF)

#define PWM3_CAPTURE                                 (PWM_BASE + 13 * 4)

#define PWM3_CAP_OUT_POSI                            (0)
#define PWM3_CAP_OUT_MASK                            (0xFFFFFFFF)

#define PWM4_COUNTER                                 (PWM_BASE + 14 * 4)

#define PWM4_END_POSI                                (0)
#define PWM4_END_MASK                                (0xFFFFFFFF)

#define PWM4_DUTY_CYCLE                              (PWM_BASE + 15 * 4)

#define PWM4_DC_POSI                                 (0)
#define PWM4_DC_MASK                                 (0xFFFFFFFF)

#define PWM4_CAPTURE                                 (PWM_BASE + 16 * 4)

#define PWM4_CAP_OUT_POSI                            (0)
#define PWM4_CAP_OUT_MASK                            (0xFFFFFFFF)

#define PWM5_COUNTER                                 (PWM_BASE + 17 * 4)

#define PWM5_END_POSI                                (0)
#define PWM5_END_MASK                                (0xFFFFFFFF)

#define PWM5_DUTY_CYCLE                              (PWM_BASE + 18 * 4)

#define PWM5_DC_POSI                                 (0)
#define PWM5_DC_MASK                                 (0xFFFFFFFF)

#define PWM5_CAPTURE                                 (PWM_BASE + 19 * 4)

#define PWM5_CAP_OUT_POSI                            (0)
#define PWM5_CAP_OUT_MASK                            (0xFFFFFFFF)

#define REG_APB_BK_PWMn_END_ADDR(n)         (PWM_BASE + 0x08 + 3 * 0x04 * (n))
#define REG_APB_BK_PWMn_DC_ADDR(n)         (PWM_BASE + 0x0C + 3 * 0x04 * (n))
#define REG_APB_BK_PWMn_CAP_ADDR(n)         (PWM_BASE + 0x10 + 3 * 0x04 * (n))

#endif

#define CHANNEL_NO                                  6

/*******************************************************************************
* Function Declarations
*******************************************************************************/
UINT32 pwm_ctrl(UINT32 cmd, void *param);

#endif //_PWM_H_
