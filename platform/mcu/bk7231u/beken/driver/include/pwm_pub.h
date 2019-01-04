#ifndef _PWM_PUB_H_
#define _PWM_PUB_H_

#define PWM_FAILURE                (1)
#define PWM_SUCCESS                (0)

#define PWM_DEV_NAME                "pwm"

#define PWM_CMD_MAGIC              (0xe230000)
enum
{
    CMD_PWM_UNIT_ENABLE = PWM_CMD_MAGIC + 1,
    CMD_PWM_UINT_DISABLE,
    CMD_PWM_IR_ENABLE,
    CMD_PWM_IR_DISABLE,
    CMD_PWM_IR_CLEAR,
    CMD_PWM_INIT_PARAM,
    CMD_PWM_CAP_GET,
    CMD_PWM_DEINIT_PARAM
};

enum
{
    PWM0     = 0,
    PWM1,
    PWM2,
    PWM3,
    PWM4,
    PWM5,
    PWM_COUNT
};

typedef void (*PFUNC)(UINT8);


#define PWM_ENABLE           (0x01)
#define PWM_DISABLE          (0x00)

#define PWM_INT_EN               (0x01)
#define PWM_INT_DIS              (0x00)

#define PMODE_PWM                   (0x00)
#define PMODE_TIMER                 (0x01)
#define PMODE_CAP_POS               (0x02)
#define PMODE_CAP_NEG               (0x03)

#define PWM_CLK_32K                        (0x00)
#define PWM_CLK_26M                        (0x01)

typedef struct
{
    UINT8 channel;


    /* cfg--PWM config
     * bit[0]:   PWM enable
     *          0:  PWM disable
     *          1:  PWM enable
     * bit[1]:   PWM interrupt enable
     *          0:  PWM interrupt disable
     *          1:  PWM interrupt enable
     * bit[3:2]: PWM mode selection
     *          00: PWM mode
     *          01: TIMER
     *          10: Capture Posedge
     *          11: Capture Negedge
     * bit[5:4]: PWM clock select
     *          00: PWM clock 32KHz
     *          01: PWM clock 26MHz
     *          10/11: PWM clock DPLL
     * bit[7:6]: reserved
     */
    union
    {
        UINT8 val;
        struct
        {
            UINT8 en: 1;
            UINT8 int_en: 1;
            UINT8 mode: 2;
            UINT8 clk: 2;
            UINT8 rsv: 2;
        } bits;
    } cfg;

    UINT16 end_value;
    UINT16 duty_cycle;
    PFUNC p_Int_Handler;
} pwm_param_t;

typedef struct
{
    UINT32 ucChannel;
    UINT16 value;
} pwm_capture_t;

/*******************************************************************************
* Function Declarations
*******************************************************************************/
extern void pwm_init(void);
extern void pwm_exit(void);
extern void pwm_isr(void);
#endif //_PWM_PUB_H_
