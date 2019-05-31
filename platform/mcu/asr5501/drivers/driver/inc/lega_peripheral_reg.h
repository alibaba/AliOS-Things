#ifndef _LEGA_PERIPHERAL_REG_H_
#define _LEGA_PERIPHERAL_REG_H_
#include <stdint.h>

/*
    LEGA GPIO BEGIN
*/
struct LEGA_GPIO
{
    __IO uint32_t DATA;
    __IO uint32_t DATAOUT;
    __I  uint32_t DUMMY0[2];
    __IO uint32_t OUTENSET;
    __IO uint32_t OUTENCLR;
    __I  uint32_t DUMMY1[2];
    __IO uint32_t INTENSET;
    __IO uint32_t INTENCLR;
    __IO uint32_t INTTYPESET;
    __IO uint32_t INTTYPECLR;
    __IO uint32_t INTPOLSET;
    __IO uint32_t INTPOLCLR;
    __IO uint32_t INTSTATUS;
};

#define GPIO_GROUP0_REG_BASE 0x40001000
#define GPIO_GROUP1_REG_BASE 0x40002000
#define GPIO_GROUP0 ((struct LEGA_GPIO *)(GPIO_GROUP0_REG_BASE))
#define GPIO_GROUP1 ((struct LEGA_GPIO *)(GPIO_GROUP1_REG_BASE))

#if (defined LEGA_A0V1)
#define PAD_PE_REG  0x40000050
#define PAD_PS_REG  0x40000054
#else
#define PAD_PE_REG      0x40000014
#define PAD_PS_REG      0x40000018
#define PAD_IS_REG      0x4000001C
#define HW_CTRL_PE_PS   0x40000020
#endif

/*
    LEGA GPIO END
*/

/*
    LEGA I2C BEGIN
*/
typedef struct
{
  __IO uint32_t  IC_CON;
  __IO uint32_t  IC_TAR;
  __IO uint32_t  IC_SAR;
  __IO uint32_t  IC_HS_MADDR;
  __IO uint32_t  IC_DATA_CMD; //0x10
  __IO uint32_t  IC_SS_SCL_HCNT;
  __IO uint32_t  IC_SS_SCL_LCNT;
  __IO uint32_t  IC_FS_SCL_HCNT;
  __IO uint32_t  IC_FS_SCL_LCNT; //0x20
  __IO uint32_t  IC_HS_SCL_HCNT;
  __IO uint32_t  IC_HS_SCL_LCNT;
  __IO uint32_t  IC_INTR_STAT;
  __IO uint32_t  IC_INTR_MASK;  //0x30
  __IO uint32_t  IC_RAW_INTR_STAT;
  __IO uint32_t  IC_RX_TL;
  __IO uint32_t  IC_TX_TL;
  __IO uint32_t  IC_CLR_INTR;  //0x40
  __IO uint32_t  IC_CLR_UNDER;
  __IO uint32_t  IC_CLR_RX_OVER;
  __IO uint32_t  IC_CLR_TX_OVER;
  __IO uint32_t  IC_CLR_RD_REG;  //0x50
  __IO uint32_t  IC_CLR_TX_ABRT;
  __IO uint32_t  IC_CLR_RX_DONE;
  __IO uint32_t  IC_CLR_ACTIVITY;
  __IO uint32_t  IC_CLR_STOP_DET;  //0x60
  __IO uint32_t  IC_CLR_START_DET;
  __IO uint32_t  IC_CLR_GEN_CALL;
  __IO uint32_t  IC_ENABLE;
  __IO uint32_t  IC_STATUS;  //0x70
  __IO uint32_t  IC_TXFLR;
  __IO uint32_t  IC_RXFLR;
  __IO uint32_t  IC_SDA_HOLD;
  __IO uint32_t  IC_TX_ABRT_SOURCE;  //0x80
  __IO uint32_t  IC_SLV_DATA_NACK_ONLY;
  __IO uint32_t  IC_DMA_CR;
  __IO uint32_t  IC_DMA_TDLR;
  __IO uint32_t  IC_DMA_RDLR; //0x90
  __IO uint32_t  IC_SDA_SETUP;
  __IO uint32_t  IC_ACK_GENERAL_CALL;
  __IO uint32_t  IC_ENABLE_STATUS;
  __IO uint32_t  IC_FS_SPKLEN;  //0xa0
  __IO uint32_t  IC_HS_SPKLEN;

  __IO uint32_t  IC_CLR_RESTART_DET;
  __IO uint32_t  IC_SCL_STUCK_AT_LOW_TIMEOUT;
  __IO uint32_t  IC_SDA_STUCK_AT_LOW_TIMEOUT; //0xb0
  __IO uint32_t  IC_CLR_SCL_STUCK_DET;
  __IO uint32_t  IC_DEVICE_ID;
  __IO uint32_t  IC_SMBUS_CLOCK_LOW_SEXT;
  __IO uint32_t  IC_SMBUS_CLOCK_LOW_MEXT; //0xc0
  __IO uint32_t  IC_SMBUS_THIGH_MAX_IDLE_COUNT;
  __IO uint32_t  IC_SMBUS_INTR_STAT;
  __IO uint32_t  IC_SMBUS_INTR_MASK;
  __IO uint32_t  IC_SMBUS_INTR_RAW_STATUS; //0xd0
  __IO uint32_t  IC_CLR_SMBUS_INTR;
  __IO uint32_t  IC_OPTIONAL_SAR;
  __IO uint32_t  IC_SMBUS_UDID_LSB;
  __IO uint32_t  RESERVED0; //0xe0
  __IO uint32_t  RESERVED1;
  __IO uint32_t  RESERVED2;
  __IO uint32_t  RESERVED3;
  __IO uint32_t  RESERVED4; //0xf0
  __IO uint32_t  IC_COMP_PARAM_1;
  __IO uint32_t  IC_COMP_VERSION;
  __IO uint32_t  IC_COMP_TYPE;
} I2C_TypeDef;

#define I2C0_BASE                   0x4001A000
#define I2C1_BASE                   0x4001B000
#define I2C0                       ((I2C_TypeDef *)I2C0_BASE)
#define I2C1                       ((I2C_TypeDef *)I2C1_BASE)

#define I2C0_SW_RESET_REG (SYS_REG_BASE + 0x938)
#define I2C1_SW_RESET_REG (SYS_REG_BASE + 0x93C)

// set pad2:3 for i2c0 func 4
#define I2C0_PINMUX_MASK0   0x0000FF00
#define I2C0_PINMUX_VALUE0  0x00004400
// set pad20:21 for i2c0 scl/sda func 1
#define I2C0_PINMUX_MASK1   0x00FF0000
#define I2C0_PINMUX_VALUE1  0x00110000
// set pad 8:9 for i2c1 func 3
#define I2C1_PINMUX_MASK0   0x000000FF
#define I2C1_PINMUX_VALUE0  0x00000033
// set pad22:23 for i2c1 scl/sda func 1
#define I2C1_PINMUX_MASK1   0xFF000000
#define I2C1_PINMUX_VALUE1  0x11000000

#define TX_EMPTY            (1 << 4)
#define MST_ACTIVITY        (1 << 5)
#define SLV_ACTIVITY        (1 << 6)
#define RX_FULL             (1 << 2)
#define RD_REQ              (1 << 5)
#define TX_ABRT             (1 << 6)
#define TFNF                0x02

#define M_STOP_DET          (1 << 9)
#define M_ACTIVITY          (1 << 8)
#define M_RX_DONE           (1 << 7)
#define M_TX_ABRT           (1 << 6)
#define M_RD_REQ            (1 << 5)
#define M_TX_EMPTY          (1 << 4)
#define M_TX_OVER           (1 << 3)
#define M_RX_FULL           (1 << 2)
#define M_RX_OVER           (1 << 1)
#define M_RX_UNDER          1

#define IC_CON_RX_FIFO_FULL_HLD_CTRL    (1 << 9) //IC_CON
#define IC_SLAVE_DISABLE                (1 << 6)
#define IC_CON_IC_RESTART_EN            (1 << 5) //IC_CON
#define IC_CON_IC_10BITADDR_MASTER      (1 << 4)
#define IC_CON_IC_10BITADDR_SLAVE       (1 << 3)
#define IC_CON_SS_MODE                  (1 << 1)
#define IC_CON_FS_MODE                  (2 << 1)
#define IC_CON_HS_MODE                  (3 << 1)
#define IC_CON_MASTER_MODE              1

#define IC_DATA_CMD_RESTART             (1 << 10) //IC_DATA_CMD
#define IC_DATA_CMD_STOP                (1 << 9)
#define IC_DATA_CMD_WRITE               0
#define IC_DATA_CMD_READ                (1 << 8)

#define I2C_PAD_GROUP                   0
#define MASTER_CODE                     3 //default value is 1, can be 0 - 7

#define SS_SCL_HCNT 188 // +5+7
#define SS_SCL_LCNT 199 // +1
#define FS_SCL_HCNT 28//8 // +5+7
#define FS_SCL_LCNT 39//19 // +1
#define HS_SCL_HCNT 12 // +1+7
#define HS_SCL_LCNT 19 // +1

#define SAR_7BIT    0x54
#define SAR_10BIT   0x362

#if (defined LEGA_A0V1)
#define I2C0_CLK_CFG                (SYS_REG_BASE + 0x840)
#define I2C1_CLK_CFG                (SYS_REG_BASE + 0x844)
#define I2C1_CLK_EN                 (1)
#define I2C0_CLK_EN                 (1)

#elif (defined LEGA_A0V2)
#define I2C0_CLK_CFG                (SYS_REG_BASE + 0x840)
#define I2C1_CLK_CFG                (SYS_REG_BASE + 0x840)
#define I2C1_CLK_EN                 (1 << 1)
#define I2C0_CLK_EN                 (1)
#endif

/*
    LEGA I2C END
*/

/*
    LEGA PWM BEGIN
*/
struct LEGA_PWM
{
    __IO uint32_t PWMCFG;
    __IO uint32_t PWMINVERTTRIG;
    __IO uint32_t PWM01TRIG;
    __IO uint32_t PWM23TRIG;
    __IO uint32_t PWM45TRIG;
    __IO uint32_t PWMINTEN1;
    __IO uint32_t PWMINTEN2;
    __I  uint32_t PWMRIS1;
    __I  uint32_t PWMRIS2;
    __IO uint32_t PWMRIC1;

    __IO uint32_t PWMRIC2;
    __I  uint32_t PWMIS1;
    __I  uint32_t PWMIS2;
    __IO uint32_t PWMCTL;
    __IO uint32_t PWM01LOAD;
    __IO uint32_t PWM23LOAD;
    __IO uint32_t PWM45LOAD;
    __I  uint32_t PWM01COUNT;
    __I  uint32_t PWM23COUNT;
    __I  uint32_t PWM45COUNT;

    __IO uint32_t PWM0CMP;
    __IO uint32_t PWM1CMP;
    __IO uint32_t PWM2CMP;
    __IO uint32_t PWM3CMP;
    __IO uint32_t PWM4CMP;
    __IO uint32_t PWM5CMP;
    __IO uint32_t PWM01DB;
    __IO uint32_t PWM23DB;
    __IO uint32_t PWM45DB;
    __IO uint32_t CAPCTL;

    __IO uint32_t CAPINTEN;
    __I  uint32_t CAPRIS;
    __IO uint32_t CAPIC;
    __I  uint32_t CAPIS;
    __I  uint32_t CAP01T;
    __I  uint32_t CAP23T;
    __I  uint32_t CAP45T;
    __IO uint32_t CAP01MATCH;
    __IO uint32_t CAP23MATCH;
    __IO uint32_t CAP45MATCH;

    __IO uint32_t TIMINTEN;
    __I  uint32_t TIMRIS;
    __IO uint32_t TIMIC;
    __I  uint32_t TIMIS;
    __IO uint32_t TIM01LOAD;
    __IO uint32_t TIM23LOAD;
    __IO uint32_t TIM45LOAD;
    __IO uint32_t TIM01COUNT;
    __IO uint32_t TIM23COUNT;
    __IO uint32_t TIM45COUNT;
};
#define PWM_REG_BASE 0x40018000
#define PWM ((struct LEGA_PWM *)(PWM_REG_BASE))

#define PWM0_PINMUX_MASK    0x0F000000
#define PWM0_PINMUX_VALUE   0x01000000
#define PWM1_PINMUX_MASK    0x00000F00
#define PWM1_PINMUX_VALUE   0x00000100
#define PWM2_PINMUX_MASK    0xF0000000
#define PWM2_PINMUX_VALUE   0x10000000
#define PWM3_PINMUX_MASK    0x0000F000
#define PWM3_PINMUX_VALUE   0x00001000
#define PWM4_PINMUX_MASK    0x0F000000
#define PWM4_PINMUX_VALUE   0x04000000
#define PWM5_PINMUX_MASK    0x0000000F
#define PWM5_PINMUX_VALUE   0x00000004
#define PWM6_PINMUX_MASK    0xF0000000
#define PWM6_PINMUX_VALUE   0x40000000
#define PWM7_PINMUX_MASK    0x000000F0
#define PWM7_PINMUX_VALUE   0x00000040

#define COUNT_UP_MODE 0
#define COUNT_UP_DOWN_MODE 1
#define PWM_COUNT_MODE COUNT_UP_MODE

#define CNT_CLK_DIV_EN (0x00000001 << 27)
#define CNT_CLK_DIV_DIS 0
#define CLK_DIV_2 (0)
#define CLK_DIV_4 (0x00000001 << 24)
#define CLK_DIV_8 (0x00000002 << 24)
#define CLK_DIV_16 (0x00000003 << 24)
#define CLK_DIV_32 (0x00000004 << 24)
#define CLK_DIV_64 (0x00000005 << 24)
#define CLK_DIV_128 (0x00000006 << 24)
#define CLK_DIV_CFG CLK_DIV_8

#define PWM0_INVERT_EN (0x00000001 << 0)
#define PWM1_INVERT_EN (0x00000001 << 1)
#define PWM2_INVERT_EN (0x00000001 << 2)
#define PWM3_INVERT_EN (0x00000001 << 3)
#define PWM4_INVERT_EN (0x00000001 << 4)
#define PWM5_INVERT_EN (0x00000001 << 5)
#define PWM6_INVERT_EN (0x00000001 << 6)
#define PWM7_INVERT_EN (0x00000001 << 7)
#define PWMX_INVERT_EN 0
/*
    LEGA PWM END
*/

/*
    LEGA RTC BEGIN
*/
struct LEGA_RTC
{
    __IO uint32_t CTRL;
    __IO uint32_t CNT_TICK;
    __IO uint32_t CNT_DATE;
    __I  uint32_t DUMMY[3];
    __I  uint32_t CURRENT_TICK;
    __I  uint32_t CURRENT_DATE;
};

#define RTC_REG_BASE 0x40000A20
#define RTC ((struct LEGA_RTC *)(RTC_REG_BASE))

#define RTC_CNT_CYCLE_ENABLE    (1 << 15)
#define RTC_ENABLE              (1 << 14)
#define CLK32K_SEL              (1 << 12) //A0V1 bug, sel ext XO no function, same as internal RC clk
#define RTC_INT_ENABLE          (1 << 11)
#define SW_OVERRIDE_REG_POR     (1 << 4)

#define RTC_TICK_CNT            32768 //1s for asic

#define GET_RTC_CURRENT_DATE()  (RTC->CURRENT_DATE)

#define RTC_INIT_YEAR           118 //2018
#define RTC_INIT_MONTH          8 //month 9
#define RTC_INIT_DATE           12
#define RTC_INIT_HOUR           8
#define RTC_INIT_MINUTE         0
#define RTC_INIT_SECOND         0
#define RTC_INIT_WEEKDAY        0 //auto modify

#define RTC_REFRESH_DAY         255 //255 max unit: day
#define RTC_REFRESH_HOUR        23 // 0 - 23
#define RTC_REFRESH_MINUTE      59 // 0 - 59
#define RTC_REFRESH_SECOND      59 // 0 - 59

#define RTC_MAX_DAY             256
#define SECOND_PER_DAY          (24*3600)

#define LEGA_IRQ_STS_REG_ADDR           0x40000100

//retention ram addr need to be planned
#define RTC_TIME_RETENTION_RAM_ADDR     0x40008000

/*
    LEGA RTC END
*/

/*
    LEGA TIMER BEGIN
*/
struct LEGA_TIMER
{
    __IO uint32_t LOAD;
    __I  uint32_t VALUE;
    __IO uint32_t CONTROL;
    __O  uint32_t INTCLR;
    __I  uint32_t RIS;
    __I  uint32_t MIS;
    __IO uint32_t BGLOAD;
};

#define TIMER1_REG_BASE          0x40017000
#define TIMER2_REG_BASE          0x40017020
#define TIMER1 ((struct LEGA_TIMER *)(TIMER1_REG_BASE))
#define TIMER2 ((struct LEGA_TIMER *)(TIMER2_REG_BASE))

#define COUNTER_16BIT 0
#define COUNTER_32BIT (1 << 1)
#define TIMER_SIZE COUNTER_32BIT

#define CLOCK_DIV_1 0
#define CLOCK_DIV_16 (1 << 2)
#define CLOCK_DIV_256 (2 << 2)
#define TIMER_PRESCALE CLOCK_DIV_1

#define FREE_RUNNING_MODE 0
#define PERIODIC_MODE (1 << 6)
#define ONE_SHOT_MODE 1

#define INTERRUPT_EN (1 << 5)
#define INTERRUPT_DIS 0

#define TIMER_ENABLE (1 << 7)
#define TIMER_DISABLE 0
/*
    LEGA TIMER END
*/

/*
    LEGA UART BEGIN
*/
typedef struct
{
    union
    {
        __I  uint8_t  RBR;
        __IO  uint8_t  THR;
        __IO uint8_t  DLL;
        uint32_t RESERVED0;    //0x0
    };
    union
    {
        __IO uint8_t  DLH;
        __IO uint32_t IER;   //0x4
    };
    union
    {
        __I  uint32_t IIR;   //0x8
        __IO  uint8_t  FCR;
    };
    __IO uint8_t  LCR;   //0xc
    uint8_t  RESERVED1[3];//Reserved
    __IO uint32_t MCR; //0x10

    __I  uint8_t  LSR;  //0x14
    uint8_t  RESERVED2[3];//Reserved
    __IO uint32_t MSR;   //0x18

    __IO uint8_t  SCR;   //0x1c
    uint8_t  RESERVED3[3];//Reserved

    __IO uint32_t LPDLL;  //0x20

    __IO uint32_t LPDLH;  //0x24

    __IO uint32_t  recerved[2];

    union{

    __IO uint32_t SRBR[16];  // 0x30~60xc
    __IO uint32_t STHR[16];

    };
    __IO uint32_t  FAR;  //0x70

    __IO uint32_t  TFR;  //0x7c

    __IO uint32_t  RFW;

    __IO uint32_t  USR;

    __IO uint32_t  TFL;

    __IO uint32_t  RFL;

    __IO uint32_t  SRR;

    __IO uint32_t  SRTS;

    __IO uint32_t  SBCR;

    __IO uint32_t  SDMAM;

    __IO uint32_t  SFE;

    __IO uint32_t  SRT;

    __IO uint32_t  STET;  //0xa0

    __IO uint32_t  HTX;

    __IO uint32_t  DMASA;  //0xa8

    __IO uint32_t  reserved[18];

    __IO uint32_t  CPR;  //0xf4

    __IO uint32_t  UCV;

    __IO uint32_t  CTR;

}UART_TypeDef;

#define UART2     (UART_TypeDef*)0x40013000
#define UART1     (UART_TypeDef*)0x40012000
#define UART0     (UART_TypeDef*)0x40011000

/*UART*/
#define DW_UART_TX_TC                           (0x60)

/*UART_REST*/
#define DW_UART_SYS_RESET_LOW                   (0xA5)
#define DW_UART_SYS_RESET_HIGH                  (0x5A)
#define DW_UART_AHB_RESET_DIS                   (0x00<<1)  // hclk
#define DW_UART_AHB_RESET_EN                    (0x01<<1)  // hclk
#define DW_UART_FUN_RESET_DIS                   (0x00<<0)  // sclk
#define DW_UART_FUN_RESET_EN                    (0x01<<0)  // sclk

 /* SRR */
#define DW_UART_SRR_UR                          (0x01)
#define DW_UART_SRR_RFR                         (0x02)
#define DW_UART_SRR_XFR                         (0x04)
#define DW_UART_LCR_DPS_MASK                    (0x3F)
#define DW_UART_LCR_DLAB                        (0x80)

/* USR */
#define DW_UART_USR_BUSY                        (0x01)
#define DW_UART_USR_TFNF                        (0x02)
#define DW_UART_USR_TFE                         (0x04)
#define DW_UART_USR_RFNE                        (0x08)
#define DW_UART_USR_RFF                         (0x10)

/* IER */
#define DW_UART_DISABLE_ALL_IRQ                 (0x00)
#define DW_UART_IER_RX_INT_EN                   (0x01)
#define DW_UART_IER_XMIT_EMPTY                  (0x02)
#define DW_UART_IER_LINE_STATUS                 (0x04)
#define DW_UART_IER_MDM_STATUS                  (0x08)
#define DW_UART_IER_PTIME                       (0x80)

/* LCR */
#define DW_UART_LCR_WORD_LEN_MASK               (0x03)
#define DW_UART_LCR_STOP_BIT_MASK               (0x04)
#define DW_UART_LCR_PARITY_MASK                 (0x38)
#define DW_UART_LCR_DPS_MASK                    (0x3F)
#define DW_UART_LCR_STICK_PARITY                (0x20)
#define DW_UART_LCR_BREAK                       (0x40)
#define DW_UART_LCR_DLAB                        (0x80)

/* Word length values */
#define DW_UART_LCR_WORD_LEN5                   (0x00)
#define DW_UART_LCR_WORD_LEN6                   (0x01)
#define DW_UART_LCR_WORD_LEN7                   (0x02)
#define DW_UART_LCR_WORD_LEN8                   (0x03)

/* stop bit values */
#define DW_UART_LCR_1_STOP_BIT                  (0x00)
#define DW_UART_LCR_1D5_STOP_BIT                (0x04)
#define DW_UART_LCR_2_STOP_BIT                  (0x04)

/* Parity bit values */
#define DW_UART_LCR_PARITY_NONE                 (0x00)
#define DW_UART_LCR_PARITY_ODD                  (0x08)
#define DW_UART_LCR_PARITY_EVEN                 (0x18)
#define DW_UART_LCR_PARITY_MARK                 (0x28)
#define DW_UART_LCR_PARITY_SPACE                (0x38)

/* LSR */
#define DW_UART_LSR_DR                          (0x01)
#define DW_UART_LSR_OVERRUN                     (0x02)
#define DW_UART_LSR_PARITYERR                   (0x04)
#define DW_UART_LSR_FRAMEERR                    (0x08)
#define DW_UART_LSR_BREAKRCVD                   (0x10)
#define DW_UART_LSR_TX_THRE                     (0x20)
#define DW_UART_LSR_TX_TEMT                     (0x40)
#define DW_UART_LSR_RX_FIFOERR                  (0x80)

/* SFE */
#define DW_UART_SFE_SHADOW_FIFO_ENABLE          (0x01)

/* SRT */
#define DW_UART_SRT_TRIGGER_LEVEL_1_CHAR        (0x00)
#define DW_UART_SRT_TRIGGER_LEVEL_1_4_FULL      (0x01)
#define DW_UART_SRT_TRIGGER_LEVEL_1_2_FULL      (0x02)
#define DW_UART_SRT_TRIGGER_LEVEL_2_LESS_FULL   (0x03)

/* STET*/
#define DW_UART_STET_FIFO_EMPTY                 (0x00)
#define DW_UART_STET_2_CHARS_IN_FIFO            (0x01)
#define DW_UART_STET_1_4_FULL                   (0x02)
#define DW_UART_STET_1_2_FULL                   (0x03)

/* CPR*/
#define DW_UART_CPR_FIFO_STAT                   (1<<10)
#define DW_UART_CPR_FIFO_MODE_OFS               (16)
#define DW_UART_CPR_FIFO_MODE_MASK              (0xFF)
#define DW_UART_CPR_FIFO_MODE                   (0xFF0000)

/* FCR*/
#define DW_UART_FCR_FIFO_ENABLE                 (0x01)
#define DW_UART_FCR_FIFO_RCVR_RESET             (0x02)
#define DW_UART_FCR_FIFO_XMIT_RESET             (0x04)
#define DW_UART_FCR_FIFO_DMA_MODE               (0x08)

/* TX EMPTY TRIGGER */
#define DW_UART_FCR_FIFO_TX_EMPTY               (0x00)
#define DW_UART_FCR_FIFO_TX_TWO_CHAR            (0x10)
#define DW_UART_FCR_FIFO_TX_QUATER_FULL         (0x20)
#define DW_UART_FCR_FIFO_TX_HALF_FULL           (0x30)

/* RCVR TRIGGER */
#define DW_UART_FCR_FIFO_RCVR_ONE_CHAR          (0x00)
#define DW_UART_FCR_FIFO_RCVR_QUARTER_FULL      (0x40)
#define DW_UART_FCR_FIFO_RCVR_HALF_FULL         (0x80)
#define DW_UART_FCR_FIFO_RCVR_TWO_LESS          (0xC0)

/*REGISTER ADDRESS*/
#define UART_CLK_REG_ADDR                       (0x40000820)
#define UART0_SCLK_EN                           (1 << 0)
#define UART1_SCLK_EN                           (1 << 1)
#define UART2_SCLK_EN                           (1 << 2)
#define UART_SCLK_ROOT_SEL_XTAL                 (1 << 3)
#define UART_SCLK_ROOT_SEL_APLL                 (0)

#define UART0_CLK_RST_REG_ADDR                  (0x40000920)
#define UART0_HCLK_RESET                        (1 << 1)
#define UART0_SCLK_RESET                        (1)

#define UART1_CLK_RST_REG_ADDR                  (0x40000924)
#define UART1_HCLK_RESET                        (1 << 1)
#define UART1_SCLK_RESET                        (1)

#define UART2_CLK_RST_REG_ADDR                  (0x40000928)
#define UART2_HCLK_RESET                        (1 << 1)
#define UART2_SCLK_RESET                        (1)

/*UART PINMUX*/
#if (defined LEGA_A0V1)
#define UART0_PIN_MUX_REG                       PINMUX_CTRL_REG0
#define UART1_PIN_MUX_REG                       PINMUX_CTRL_REG0
#define UART2_PIN_MUX_REG                       PINMUX_CTRL_REG0

#elif (defined LEGA_A0V2)
#define UART0_PIN_MUX_REG                       PINMUX_CTRL_REG0
#define UART1_PIN_MUX_REG                       PINMUX_CTRL_REG0
#define UART2_PIN_MUX_REG                       PINMUX_CTRL_REG1
#endif

#define DW_UART0_GPIO_MASK                      (0x000000FF) //GPIO0/1
#define DW_UART0_TX                             (0x00000001)
#define DW_UART0_RX                             (0x00000010)

#define DW_UART1_GPIO_MASK                      (0x0000FF00) //GPIO2/3
#define DW_UART1_TX                             (0x00000100)
#define DW_UART1_RX                             (0x00001000)

#if (defined LEGA_A0V1)
#define DW_UART2_GPIO_MASK                      (0xFF000000) //GPIO6/7
#define DW_UART2_TX                             (0x03000000)
#define DW_UART2_RX                             (0x30000000)
#elif (defined LEGA_A0V2)
#define DW_UART2_GPIO_MASK                      (0x00FF0000) //GPIO12/13
#define DW_UART2_TX                             (0x00030000)
#define DW_UART2_RX                             (0x00300000)
#endif

//uart clock frequency setting
#ifdef LEGA_A0V1
#ifdef XTAL_40M
    #define UART_BASE_CLOCK 20000000
#else
    #define UART_BASE_CLOCK 13000000
#endif
#else //LEGA_A0V2
    #define UART_BASE_CLOCK SYSTEM_CLOCK
#endif

extern uint8_t g_printf_uart;
#define uart_put_char(c) do{\
    if(g_printf_uart == 0) \
    {\
        ((UART0)->THR) = (c); \
        while((((UART0)->LSR) & DW_UART_TX_TC) == 0);\
    }\
    else if(g_printf_uart == 1) \
    {\
        ((UART1)->THR) = (c); \
        while((((UART1)->LSR) & DW_UART_TX_TC) == 0);\
    }\
    else if(g_printf_uart == 2) \
    {\
        ((UART2)->THR) = (c); \
        while((((UART2)->LSR) & DW_UART_TX_TC) == 0);\
    }\
}while(0)

/*
    LEGA UART END
*/

/*
    LEGA WDG BEGIN
*/
struct LEGA_WDOG
{
    __IO uint32_t LOAD;
    __I  uint32_t VALUE;
    __IO uint32_t CONTROL;
    __O  uint32_t INTCLR;
    __I  uint32_t RIS;
    __I  uint32_t MIS; //0x14
    __I  uint32_t DUMMY0[0x2FA];
    __IO uint32_t LOCK; //0xC00
    __I  uint32_t DUMMY1[0xBF];
    __IO uint32_t ITCR; //0xF00
    __O  uint32_t ITOP; //0xF04
    __I  uint32_t DUMMY2[0x32];
    __I  uint32_t PERIPHID4; //0xFD0
    __I  uint32_t PERIPHID5;
    __I  uint32_t PERIPHID6;
    __I  uint32_t PERIPHID7;
    __I  uint32_t PERIPHID0;
    __I  uint32_t PERIPHID1;
    __I  uint32_t PERIPHID2;
    __I  uint32_t PERIPHID3;
    __I  uint32_t PCELLID0;
    __I  uint32_t PCELLID1;
    __I  uint32_t PCELLID2;
    __I  uint32_t PCELLID3;
};
#define WDOG_BASE       0x40010000
#define WATCHDOG ((struct LEGA_WDOG *)(WDOG_BASE))

#define WDG_LOCK_TOKEN 0x1ACCE551
#define WDG_RESEN (1 << 1)
#define WDG_INTEN 1
/*
    LEGA WDG END
*/

/*
    LEGA EFUSE BEGIN
*/
struct LEGA_EFUSE
{
    __IO uint32_t CFG_TYPE; //0x00
    __IO uint32_t WR_TYPE;
    __IO uint32_t START;
    __IO uint32_t RD_CNT;
    __IO uint32_t WR_CNT; //0x10
    __IO uint32_t DIV_CNT;
    __IO uint32_t B_ADDR;
    __IO uint32_t PGM_DATA;
    __IO uint32_t RDBK_DATA; //0x20
    __I  uint32_t RSVD;
    __IO uint32_t INT_EN;
    __IO uint32_t INT_CLR;
    __IO uint32_t ENABLE; //0x30
};

#define EFUSE_CTRL_BASE              0x40005000
#define EFUSE ((struct LEGA_EFUSE *)(EFUSE_CTRL_BASE))
/*
    LEGA EFUSE END
*/

#endif //_LEGA_PERIPHERAL_REG_H_
