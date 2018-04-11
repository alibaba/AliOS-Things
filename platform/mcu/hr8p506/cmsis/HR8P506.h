#ifndef __HR8P506_H__
#define __HR8P506_H__


#define __I volatile const /* defines 'read only' permissions */
#define __O volatile /* defines 'write only' permissions */
#define __IO volatile /* defines 'read / write' permissions */


#define __CM0_REV 0 /* Core Revision r0p0 */
#define __NVIC_PRIO_BITS 2 /* HR8P506 uses 2 Bits for the Priority Levels */
#define __Vendor_SysTickConfig 0 /* Set to 1 if different SysTick Config is used */

typedef enum IRQn
{
	/****** Cortex-M0 Processor Exceptions Numbers ******************************************************/
	RST_IRQn = -15,
	NonMaskableInt_IRQn = -14,
	HardFault_IRQn = -13,
	SVC_IRQn = -5,
	PendSV_IRQn = -2,
	SysTick_IRQn = -1,
	PINT0_IRQn = 0,
	PINT1_IRQn = 1,
	PINT2_IRQn = 2,
	PINT3_IRQn = 3,
	PINT4_IRQn = 4,
	PINT5_IRQn = 5,
	PINT6_IRQn = 6,
	PINT7_IRQn = 7,
	T16N0_IRQn = 8,
	T16N1_IRQn = 9,
	T16N2_IRQn = 10,
	T16N3_IRQn = 11,
	T32N0_IRQn = 12,
	Reserved0_IRQn = 13,
	Reserved1_IRQn = 14,
	Reserved2_IRQn = 15,
	WDT_IRQn = 16,
	RTC_IRQn = 17,
	KINT_IRQn = 18,
	ADC_IRQn = 19,
	Reserved3_IRQn = 20,
	LVD_IRQn = 21,
	PLLLK_IRQn = 22,
	UART0_IRQn = 23,
	UART1_IRQn = 24,
	EUART0_IRQn = 25,
	Reserved4_IRQn = 26,
	SPI0_IRQn = 27,
	SPI1_IRQn = 28,
	I2C0_IRQn = 29,
	Reserved5_IRQn = 30,
	CCM_IRQn = 31,
} IRQn_Type;


#include "core_cm0.h"
#include <stdint.h>


/******************************************************************************/
/*                              设备特殊寄存器结构定义                        */
/******************************************************************************/

/*   允许匿名结构和匿名联合    */
#pragma anon_unions

typedef union
{
	struct
	{
		uint32_t PROT: 1;
		uint32_t RESERVED0: 31;
	};
	uint32_t Word;
} SCU_PROT_Typedef;

typedef union
{
	struct
	{
		uint32_t NMIEN: 1;
		uint32_t NMICS: 5;
		uint32_t RESERVED0: 26;
	};
	uint32_t Word;
} SCU_NMICON_Typedef;

typedef union
{
	struct
	{
		uint32_t PORF: 1;
		uint32_t PORRCF: 1;
		uint32_t PORRSTF: 1;
		uint32_t BORF: 1;
		uint32_t WDTRSTF: 1;
		uint32_t MRSTF: 1;
		uint32_t SOFT_RSTF: 1;
		uint32_t POR_LOST: 1;
		uint32_t CFG_RST: 1;
		uint32_t RESERVED0: 23;
	};
	uint32_t Word;
} SCU_PWRC_Typedef;

typedef union
{
	struct
	{
		uint32_t FLAG0: 1;
		uint32_t FLAG1: 1;
		uint32_t FLAG2: 1;
		uint32_t RESERVED0: 29;
	};
	uint32_t Word;
} SCU_FAULTFLAG_Typedef;

typedef union
{
	struct
	{
		uint32_t ACCT: 4;
		uint32_t RESERVED0: 28;
	};
	uint32_t Word;
} SCU_FLASHWAIT_Typedef;

typedef union
{
	struct
	{
		uint32_t BORV: 4;
		uint32_t RESERVED0: 28;
	};
	uint32_t Word;
} SCU_SOFTCFG_Typedef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t FLTEN: 1;
		uint32_t RESERVED0: 2;
		uint32_t VS: 4;
		uint32_t IF: 1;
		uint32_t IE: 1;
		uint32_t IFS: 3;
		uint32_t RESERVED1: 2;
		uint32_t LVDO: 1;
		uint32_t RESERVED2: 16;
	};
	uint32_t Word;
} SCU_LVDCON_Typedef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t RESERVED0: 3;
		uint32_t IE: 1;
		uint32_t IFS: 3;
		uint32_t IF: 1;
		uint32_t RESERVED1: 7;
		uint32_t FLAG: 1;
		uint32_t RESERVED2: 15;
	};
	uint32_t Word;
} SCU_CCM_Typedef;

typedef union
{
	struct
	{
		uint32_t IE: 1;
		uint32_t RESERVED0: 3;
		uint32_t LK_IFS: 3;
		uint32_t RESERVED1: 1;
		uint32_t IF: 1;
		uint32_t RESERVED2: 7;
		uint32_t LK_FLAG: 1;
		uint32_t RESERVED3: 15;
	};
	uint32_t Word;
} SCU_PLLLKCON_Typedef;

typedef union
{
	struct
	{
		uint32_t T16N0EN: 1;
		uint32_t T16N1EN: 1;
		uint32_t T16N2EN: 1;
		uint32_t T16N3EN: 1;
		uint32_t RESERVED0: 4;
		uint32_t T32N0EN: 1;
		uint32_t RESERVED1: 23;
	};
	uint32_t Word;
} SCU_TIMEREN_Typedef;

typedef union
{
	struct
	{
		uint32_t T16N0DIS: 1;
		uint32_t T16N1DIS: 1;
		uint32_t T16N2DIS: 1;
		uint32_t T16N3DIS: 1;
		uint32_t RESERVED0: 4;
		uint32_t T32N0DIS: 1;
		uint32_t RESERVED1: 23;
	};
	uint32_t Word;
} SCU_TIMERDIS_Typedef;

typedef union
{
	struct
	{
		uint32_t CLK_SEL: 2;
		uint32_t XTAL_LP: 1;
		uint32_t RESERVED0: 5;
		uint32_t PLL_MUX: 1;
		uint32_t RESERVED1: 3;
		uint32_t SYSCLK_DIV: 3;
		uint32_t RESERVED2: 1;
		uint32_t CLKFLT_BY: 8;
		uint32_t CLKOUT0_SEL: 2;
		uint32_t CLKOUT1_SEL: 2;
		uint32_t RESERVED3: 4;
	};
	uint32_t Word;
} SCU_SCLKEN0_Typedef;

typedef union
{
	struct
	{
		uint32_t XTAL_EN: 1;
		uint32_t HRC_EN: 1;
		uint32_t RESERVED0: 6;
		uint32_t PLL_REF_SEL: 3;
		uint32_t PLL_48M_SEL: 1;
		uint32_t PLL_EN: 1;
		uint32_t PLL_BYLOCK: 1;
		uint32_t RESERVED1: 2;
		uint32_t XTAL_RDY: 1;
		uint32_t HRC_RDY: 1;
		uint32_t PLL_RDY: 1;
		uint32_t RESERVED2: 13;
	};
	uint32_t Word;
} SCU_SCLKEN1_Typedef;

typedef union
{
	struct
	{
		uint32_t SCU_EN: 1;
		uint32_t GPIO_EN: 1;
		uint32_t IAP_EN: 1;
		uint32_t RESERVED0: 1;
		uint32_t ADC_EN: 1;
		uint32_t RTC_EN: 1;
		uint32_t LCD_EN: 1;
		uint32_t WDT_EN: 1;
		uint32_t T16N0_EN: 1;
		uint32_t T16N1_EN: 1;
		uint32_t T16N2_EN: 1;
		uint32_t T16N3_EN: 1;
		uint32_t T32N0_EN: 1;
		uint32_t RESERVED1: 3;
		uint32_t UART0_EN: 1;
		uint32_t UART1_EN: 1;
		uint32_t RESERVED2: 2;
		uint32_t EUART0_EN: 1;
		uint32_t RESERVED3: 3;
		uint32_t SPI0_EN: 1;
		uint32_t SPI1_EN: 1;
		uint32_t RESERVED4: 2;
		uint32_t I2C0_EN: 1;
		uint32_t RESERVED5: 3;
	};
	uint32_t Word;
} SCU_PCLKEN_Typedef;

typedef union
{
	struct
	{
		uint32_t WAKEUPTIME: 12;
		uint32_t MOSC_EN: 1;
		uint32_t CLKFLT_EN: 1;
		uint32_t FLASHPW_PD: 1;
		uint32_t RESERVED0: 1;
		uint32_t LDOLP_VOSEL: 3;
		uint32_t RESERVED1: 13;
	};
	uint32_t Word;
} SCU_WAKEUPTIME_Typedef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t RESERVED0: 31;
	};
	uint32_t Word;
} SCU_TBLREMAPEN_Typedef;

typedef union
{
	struct
	{
		uint32_t TBLOFF: 32;
	};
	uint32_t Word;
} SCU_TBLOFF_Typedef;

typedef struct
{
	__IO SCU_PROT_Typedef PROT;
	__IO SCU_NMICON_Typedef NMICON;
	__IO SCU_PWRC_Typedef PWRC;
	__IO SCU_FAULTFLAG_Typedef FAULTFLAG;
	uint32_t RESERVED0[4] ;
	__IO SCU_FLASHWAIT_Typedef FLASHWAIT;
	__IO SCU_SOFTCFG_Typedef SOFTCFG;
	__IO SCU_LVDCON_Typedef LVDCON;
	__IO SCU_CCM_Typedef CCM;
	__IO SCU_PLLLKCON_Typedef PLLLKCON;
	__IO SCU_TIMEREN_Typedef TIMEREN;
	__IO SCU_TIMERDIS_Typedef TIMERDIS;
	uint32_t RESERVED1 ;
	__IO SCU_SCLKEN0_Typedef SCLKEN0;
	__IO SCU_SCLKEN1_Typedef SCLKEN1;
	__IO SCU_PCLKEN_Typedef PCLKEN;
	__IO SCU_WAKEUPTIME_Typedef WAKEUPTIME;
	uint32_t RESERVED2[4] ;
	__IO SCU_TBLREMAPEN_Typedef TBLREMAPEN;
	__IO SCU_TBLOFF_Typedef TBLOFF;
} SCU_TypeDef;

typedef union
{
	struct
	{
		uint32_t PORT_0: 1;
		uint32_t PORT_1: 1;
		uint32_t PORT_2: 1;
		uint32_t PORT_3: 1;
		uint32_t PORT_4: 1;
		uint32_t PORT_5: 1;
		uint32_t PORT_6: 1;
		uint32_t PORT_7: 1;
		uint32_t PORT_8: 1;
		uint32_t PORT_9: 1;
		uint32_t PORT_10: 1;
		uint32_t PORT_11: 1;
		uint32_t PORT_12: 1;
		uint32_t PORT_13: 1;
		uint32_t PORT_14: 1;
		uint32_t PORT_15: 1;
		uint32_t PORT_16: 1;
		uint32_t PORT_17: 1;
		uint32_t PORT_18: 1;
		uint32_t PORT_19: 1;
		uint32_t PORT_20: 1;
		uint32_t PORT_21: 1;
		uint32_t PORT_22: 1;
		uint32_t PORT_23: 1;
		uint32_t PORT_24: 1;
		uint32_t PORT_25: 1;
		uint32_t PORT_26: 1;
		uint32_t PORT_27: 1;
		uint32_t PORT_28: 1;
		uint32_t PORT_29: 1;
		uint32_t PORT_30: 1;
		uint32_t PORT_31: 1;
	};
	uint32_t Word;
} GPIO_PAPORT_Typedef;

typedef union
{
	struct
	{
		uint32_t DATA_0: 1;
		uint32_t DATA_1: 1;
		uint32_t DATA_2: 1;
		uint32_t DATA_3: 1;
		uint32_t DATA_4: 1;
		uint32_t DATA_5: 1;
		uint32_t DATA_6: 1;
		uint32_t DATA_7: 1;
		uint32_t DATA_8: 1;
		uint32_t DATA_9: 1;
		uint32_t DATA_10: 1;
		uint32_t DATA_11: 1;
		uint32_t DATA_12: 1;
		uint32_t DATA_13: 1;
		uint32_t DATA_14: 1;
		uint32_t DATA_15: 1;
		uint32_t DATA_16: 1;
		uint32_t DATA_17: 1;
		uint32_t DATA_18: 1;
		uint32_t DATA_19: 1;
		uint32_t DATA_20: 1;
		uint32_t DATA_21: 1;
		uint32_t DATA_22: 1;
		uint32_t DATA_23: 1;
		uint32_t DATA_24: 1;
		uint32_t DATA_25: 1;
		uint32_t DATA_26: 1;
		uint32_t DATA_27: 1;
		uint32_t DATA_28: 1;
		uint32_t DATA_29: 1;
		uint32_t DATA_30: 1;
		uint32_t DATA_31: 1;
	};
	uint32_t Word;
} GPIO_PADATA_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PADATABSR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PADATABCR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PADATABRR_Typedef;

typedef union
{
	struct
	{
		uint32_t DIR_0: 1;
		uint32_t DIR_1: 1;
		uint32_t DIR_2: 1;
		uint32_t DIR_3: 1;
		uint32_t DIR_4: 1;
		uint32_t DIR_5: 1;
		uint32_t DIR_6: 1;
		uint32_t DIR_7: 1;
		uint32_t DIR_8: 1;
		uint32_t DIR_9: 1;
		uint32_t DIR_10: 1;
		uint32_t DIR_11: 1;
		uint32_t DIR_12: 1;
		uint32_t DIR_13: 1;
		uint32_t DIR_14: 1;
		uint32_t DIR_15: 1;
		uint32_t DIR_16: 1;
		uint32_t DIR_17: 1;
		uint32_t DIR_18: 1;
		uint32_t DIR_19: 1;
		uint32_t DIR_20: 1;
		uint32_t DIR_21: 1;
		uint32_t DIR_22: 1;
		uint32_t DIR_23: 1;
		uint32_t DIR_24: 1;
		uint32_t DIR_25: 1;
		uint32_t DIR_26: 1;
		uint32_t DIR_27: 1;
		uint32_t DIR_28: 1;
		uint32_t DIR_29: 1;
		uint32_t DIR_30: 1;
		uint32_t DIR_31: 1;
	};
	uint32_t Word;
} GPIO_PADIR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PADIRBSR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PADIRBCR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PADIRBRR_Typedef;

typedef union
{
	struct
	{
		uint32_t PA0: 2;
		uint32_t RESERVED0: 2;
		uint32_t PA1: 2;
		uint32_t RESERVED1: 2;
		uint32_t PA2: 2;
		uint32_t RESERVED2: 2;
		uint32_t PA3: 2;
		uint32_t RESERVED3: 2;
		uint32_t PA4: 2;
		uint32_t RESERVED4: 2;
		uint32_t PA5: 2;
		uint32_t RESERVED5: 2;
		uint32_t PA6: 2;
		uint32_t RESERVED6: 2;
		uint32_t PA7: 2;
		uint32_t RESERVED7: 2;
	};
	uint32_t Word;
} GPIO_PAFUNC0_Typedef;

typedef union
{
	struct
	{
		uint32_t PA8: 2;
		uint32_t RESERVED0: 2;
		uint32_t PA9: 2;
		uint32_t RESERVED1: 2;
		uint32_t PA10: 2;
		uint32_t RESERVED2: 2;
		uint32_t PA11: 2;
		uint32_t RESERVED3: 2;
		uint32_t PA12: 2;
		uint32_t RESERVED4: 2;
		uint32_t PA13: 2;
		uint32_t RESERVED5: 2;
		uint32_t PA14: 2;
		uint32_t RESERVED6: 2;
		uint32_t PA15: 2;
		uint32_t RESERVED7: 2;
	};
	uint32_t Word;
} GPIO_PAFUNC1_Typedef;

typedef union
{
	struct
	{
		uint32_t PA16: 2;
		uint32_t RESERVED0: 2;
		uint32_t PA17: 2;
		uint32_t RESERVED1: 2;
		uint32_t PA18: 2;
		uint32_t RESERVED2: 2;
		uint32_t PA19: 2;
		uint32_t RESERVED3: 2;
		uint32_t PA20: 2;
		uint32_t RESERVED4: 2;
		uint32_t PA21: 2;
		uint32_t RESERVED5: 2;
		uint32_t PA22: 2;
		uint32_t RESERVED6: 2;
		uint32_t PA23: 2;
		uint32_t RESERVED7: 2;
	};
	uint32_t Word;
} GPIO_PAFUNC2_Typedef;

typedef union
{
	struct
	{
		uint32_t PA24: 2;
		uint32_t RESERVED0: 2;
		uint32_t PA25: 2;
		uint32_t RESERVED1: 2;
		uint32_t PA26: 2;
		uint32_t RESERVED2: 2;
		uint32_t PA27: 2;
		uint32_t RESERVED3: 2;
		uint32_t PA28: 2;
		uint32_t RESERVED4: 2;
		uint32_t PA29: 2;
		uint32_t RESERVED5: 2;
		uint32_t PA30: 2;
		uint32_t RESERVED6: 2;
		uint32_t PA31: 2;
		uint32_t RESERVED7: 2;
	};
	uint32_t Word;
} GPIO_PAFUNC3_Typedef;

typedef union
{
	struct
	{
		uint32_t INEB_0: 1;
		uint32_t INEB_1: 1;
		uint32_t INEB_2: 1;
		uint32_t INEB_3: 1;
		uint32_t INEB_4: 1;
		uint32_t INEB_5: 1;
		uint32_t INEB_6: 1;
		uint32_t INEB_7: 1;
		uint32_t INEB_8: 1;
		uint32_t INEB_9: 1;
		uint32_t INEB_10: 1;
		uint32_t INEB_11: 1;
		uint32_t INEB_12: 1;
		uint32_t INEB_13: 1;
		uint32_t INEB_14: 1;
		uint32_t INEB_15: 1;
		uint32_t INEB_16: 1;
		uint32_t INEB_17: 1;
		uint32_t INEB_18: 1;
		uint32_t INEB_19: 1;
		uint32_t INEB_20: 1;
		uint32_t INEB_21: 1;
		uint32_t INEB_22: 1;
		uint32_t INEB_23: 1;
		uint32_t INEB_24: 1;
		uint32_t INEB_25: 1;
		uint32_t INEB_26: 1;
		uint32_t INEB_27: 1;
		uint32_t INEB_28: 1;
		uint32_t INEB_29: 1;
		uint32_t INEB_30: 1;
		uint32_t INEB_31: 1;
	};
	uint32_t Word;
} GPIO_PAINEB_Typedef;

typedef union
{
	struct
	{
		uint32_t ODE_0: 1;
		uint32_t ODE_1: 1;
		uint32_t ODE_2: 1;
		uint32_t ODE_3: 1;
		uint32_t ODE_4: 1;
		uint32_t ODE_5: 1;
		uint32_t ODE_6: 1;
		uint32_t ODE_7: 1;
		uint32_t ODE_8: 1;
		uint32_t ODE_9: 1;
		uint32_t ODE_10: 1;
		uint32_t ODE_11: 1;
		uint32_t ODE_12: 1;
		uint32_t ODE_13: 1;
		uint32_t ODE_14: 1;
		uint32_t ODE_15: 1;
		uint32_t ODE_16: 1;
		uint32_t ODE_17: 1;
		uint32_t ODE_18: 1;
		uint32_t ODE_19: 1;
		uint32_t ODE_20: 1;
		uint32_t ODE_21: 1;
		uint32_t ODE_22: 1;
		uint32_t ODE_23: 1;
		uint32_t ODE_24: 1;
		uint32_t ODE_25: 1;
		uint32_t ODE_26: 1;
		uint32_t ODE_27: 1;
		uint32_t ODE_28: 1;
		uint32_t ODE_29: 1;
		uint32_t ODE_30: 1;
		uint32_t ODE_31: 1;
	};
	uint32_t Word;
} GPIO_PAODE_Typedef;

typedef union
{
	struct
	{
		uint32_t PUE_0: 1;
		uint32_t PUE_1: 1;
		uint32_t PUE_2: 1;
		uint32_t PUE_3: 1;
		uint32_t PUE_4: 1;
		uint32_t PUE_5: 1;
		uint32_t PUE_6: 1;
		uint32_t PUE_7: 1;
		uint32_t PUE_8: 1;
		uint32_t PUE_9: 1;
		uint32_t PUE_10: 1;
		uint32_t PUE_11: 1;
		uint32_t PUE_12: 1;
		uint32_t PUE_13: 1;
		uint32_t PUE_14: 1;
		uint32_t PUE_15: 1;
		uint32_t PUE_16: 1;
		uint32_t PUE_17: 1;
		uint32_t PUE_18: 1;
		uint32_t PUE_19: 1;
		uint32_t PUE_20: 1;
		uint32_t PUE_21: 1;
		uint32_t PUE_22: 1;
		uint32_t PUE_23: 1;
		uint32_t PUE_24: 1;
		uint32_t PUE_25: 1;
		uint32_t PUE_26: 1;
		uint32_t PUE_27: 1;
		uint32_t PUE_28: 1;
		uint32_t PUE_29: 1;
		uint32_t PUE_30: 1;
		uint32_t PUE_31: 1;
	};
	uint32_t Word;
} GPIO_PAPUE_Typedef;

typedef union
{
	struct
	{
		uint32_t PDE_0: 1;
		uint32_t PDE_1: 1;
		uint32_t PDE_2: 1;
		uint32_t PDE_3: 1;
		uint32_t PDE_4: 1;
		uint32_t PDE_5: 1;
		uint32_t PDE_6: 1;
		uint32_t PDE_7: 1;
		uint32_t PDE_8: 1;
		uint32_t PDE_9: 1;
		uint32_t PDE_10: 1;
		uint32_t PDE_11: 1;
		uint32_t PDE_12: 1;
		uint32_t PDE_13: 1;
		uint32_t PDE_14: 1;
		uint32_t PDE_15: 1;
		uint32_t PDE_16: 1;
		uint32_t PDE_17: 1;
		uint32_t PDE_18: 1;
		uint32_t PDE_19: 1;
		uint32_t PDE_20: 1;
		uint32_t PDE_21: 1;
		uint32_t PDE_22: 1;
		uint32_t PDE_23: 1;
		uint32_t PDE_24: 1;
		uint32_t PDE_25: 1;
		uint32_t PDE_26: 1;
		uint32_t PDE_27: 1;
		uint32_t PDE_28: 1;
		uint32_t PDE_29: 1;
		uint32_t PDE_30: 1;
		uint32_t PDE_31: 1;
	};
	uint32_t Word;
} GPIO_PAPDE_Typedef;

typedef union
{
	struct
	{
		uint32_t DS_0: 1;
		uint32_t DS_1: 1;
		uint32_t DS_2: 1;
		uint32_t DS_3: 1;
		uint32_t DS_4: 1;
		uint32_t DS_5: 1;
		uint32_t DS_6: 1;
		uint32_t DS_7: 1;
		uint32_t DS_8: 1;
		uint32_t DS_9: 1;
		uint32_t DS_10: 1;
		uint32_t DS_11: 1;
		uint32_t DS_12: 1;
		uint32_t DS_13: 1;
		uint32_t DS_14: 1;
		uint32_t DS_15: 1;
		uint32_t DS_16: 1;
		uint32_t DS_17: 1;
		uint32_t DS_18: 1;
		uint32_t DS_19: 1;
		uint32_t DS_20: 1;
		uint32_t DS_21: 1;
		uint32_t DS_22: 1;
		uint32_t DS_23: 1;
		uint32_t DS_24: 1;
		uint32_t DS_25: 1;
		uint32_t DS_26: 1;
		uint32_t DS_27: 1;
		uint32_t DS_28: 1;
		uint32_t DS_29: 1;
		uint32_t DS_30: 1;
		uint32_t DS_31: 1;
	};
	uint32_t Word;
} GPIO_PADS_Typedef;

typedef union
{
	struct
	{
		uint32_t PORT_0: 1;
		uint32_t PORT_1: 1;
		uint32_t PORT_2: 1;
		uint32_t PORT_3: 1;
		uint32_t PORT_4: 1;
		uint32_t PORT_5: 1;
		uint32_t PORT_6: 1;
		uint32_t PORT_7: 1;
		uint32_t PORT_8: 1;
		uint32_t PORT_9: 1;
		uint32_t PORT_10: 1;
		uint32_t PORT_11: 1;
		uint32_t PORT_12: 1;
		uint32_t PORT_13: 1;
		uint32_t RESERVED0: 18;
	};
	uint32_t Word;
} GPIO_PBPORT_Typedef;

typedef union
{
	struct
	{
		uint32_t DATA_0: 1;
		uint32_t DATA_1: 1;
		uint32_t DATA_2: 1;
		uint32_t DATA_3: 1;
		uint32_t DATA_4: 1;
		uint32_t DATA_5: 1;
		uint32_t DATA_6: 1;
		uint32_t DATA_7: 1;
		uint32_t DATA_8: 1;
		uint32_t DATA_9: 1;
		uint32_t DATA_10: 1;
		uint32_t DATA_11: 1;
		uint32_t DATA_12: 1;
		uint32_t DATA_13: 1;
		uint32_t RESERVED0: 18;
	};
	uint32_t Word;
} GPIO_PBDATA_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PBDATABSR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PBDATABCR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PBDATABRR_Typedef;

typedef union
{
	struct
	{
		uint32_t DIR_0: 1;
		uint32_t DIR_1: 1;
		uint32_t DIR_2: 1;
		uint32_t DIR_3: 1;
		uint32_t DIR_4: 1;
		uint32_t DIR_5: 1;
		uint32_t DIR_6: 1;
		uint32_t DIR_7: 1;
		uint32_t DIR_8: 1;
		uint32_t DIR_9: 1;
		uint32_t DIR_10: 1;
		uint32_t DIR_11: 1;
		uint32_t DIR_12: 1;
		uint32_t DIR_13: 1;
		uint32_t RESERVED0: 18;
	};
	uint32_t Word;
} GPIO_PBDIR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PBDIRBSR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PBDIRBCR_Typedef;

typedef struct
{
	uint32_t Word;
} GPIO_PBDIRBRR_Typedef;

typedef union
{
	struct
	{
		uint32_t PB0: 2;
		uint32_t RESERVED0: 2;
		uint32_t PB1: 2;
		uint32_t RESERVED1: 2;
		uint32_t PB2: 2;
		uint32_t RESERVED2: 2;
		uint32_t PB3: 2;
		uint32_t RESERVED3: 2;
		uint32_t PB4: 2;
		uint32_t RESERVED4: 2;
		uint32_t PB5: 2;
		uint32_t RESERVED5: 2;
		uint32_t PB6: 2;
		uint32_t RESERVED6: 2;
		uint32_t PB7: 2;
		uint32_t RESERVED7: 2;
	};
	uint32_t Word;
} GPIO_PBFUNC0_Typedef;

typedef union
{
	struct
	{
		uint32_t PB8: 2;
		uint32_t RESERVED0: 2;
		uint32_t PB9: 2;
		uint32_t RESERVED1: 2;
		uint32_t PB10: 2;
		uint32_t RESERVED2: 2;
		uint32_t PB11: 2;
		uint32_t RESERVED3: 2;
		uint32_t PB12: 2;
		uint32_t RESERVED4: 2;
		uint32_t PB13: 2;
		uint32_t RESERVED5: 10;
	};
	uint32_t Word;
} GPIO_PBFUNC1_Typedef;

typedef union
{
	struct
	{
		uint32_t INEB_0: 1;
		uint32_t INEB_1: 1;
		uint32_t INEB_2: 1;
		uint32_t INEB_3: 1;
		uint32_t INEB_4: 1;
		uint32_t INEB_5: 1;
		uint32_t INEB_6: 1;
		uint32_t INEB_7: 1;
		uint32_t INEB_8: 1;
		uint32_t INEB_9: 1;
		uint32_t INEB_10: 1;
		uint32_t INEB_11: 1;
		uint32_t INEB_12: 1;
		uint32_t INEB_13: 1;
		uint32_t RESERVED0: 18;
	};
	uint32_t Word;
} GPIO_PBINEB_Typedef;

typedef union
{
	struct
	{
		uint32_t ODE_0: 1;
		uint32_t ODE_1: 1;
		uint32_t ODE_2: 1;
		uint32_t ODE_3: 1;
		uint32_t ODE_4: 1;
		uint32_t ODE_5: 1;
		uint32_t ODE_6: 1;
		uint32_t ODE_7: 1;
		uint32_t ODE_8: 1;
		uint32_t ODE_9: 1;
		uint32_t ODE_10: 1;
		uint32_t ODE_11: 1;
		uint32_t ODE_12: 1;
		uint32_t ODE_13: 1;
		uint32_t RESERVED0: 18;
	};
	uint32_t Word;
} GPIO_PBODE_Typedef;

typedef union
{
	struct
	{
		uint32_t PUEN_0: 1;
		uint32_t PUEN_1: 1;
		uint32_t PUEN_2: 1;
		uint32_t PUEN_3: 1;
		uint32_t PUEN_4: 1;
		uint32_t PUEN_5: 1;
		uint32_t PUEN_6: 1;
		uint32_t PUEN_7: 1;
		uint32_t PUEN_8: 1;
		uint32_t PUEN_9: 1;
		uint32_t PUEN_10: 1;
		uint32_t PUEN_11: 1;
		uint32_t PUEN_12: 1;
		uint32_t PUEN_13: 1;
		uint32_t RESERVED0: 18;
	};
	uint32_t Word;
} GPIO_PBPUE_Typedef;

typedef union
{
	struct
	{
		uint32_t PDEN_0: 1;
		uint32_t PDEN_1: 1;
		uint32_t PDEN_2: 1;
		uint32_t PDEN_3: 1;
		uint32_t PDEN_4: 1;
		uint32_t PDEN_5: 1;
		uint32_t PDEN_6: 1;
		uint32_t PDEN_7: 1;
		uint32_t PDEN_8: 1;
		uint32_t PDEN_9: 1;
		uint32_t PDEN_10: 1;
		uint32_t PDEN_11: 1;
		uint32_t PDEN_12: 1;
		uint32_t PDEN_13: 1;
		uint32_t RESERVED0: 18;
	};
	uint32_t Word;
} GPIO_PBPDE_Typedef;

typedef union
{
	struct
	{
		uint32_t DS_0: 1;
		uint32_t DS_1: 1;
		uint32_t DS_2: 1;
		uint32_t DS_3: 1;
		uint32_t DS_4: 1;
		uint32_t DS_5: 1;
		uint32_t DS_6: 1;
		uint32_t DS_7: 1;
		uint32_t DS_8: 1;
		uint32_t DS_9: 1;
		uint32_t DS_10: 1;
		uint32_t DS_11: 1;
		uint32_t DS_12: 1;
		uint32_t DS_13: 1;
		uint32_t RESERVED0: 18;
	};
	uint32_t Word;
} GPIO_PBDS_Typedef;

typedef union
{
	struct
	{
		uint32_t PINTIE: 8;
		uint32_t PMASK: 8;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} GPIO_PINTIE_Typedef;

typedef union
{
	struct
	{
		uint32_t PINTIF_0: 1;
		uint32_t PINTIF_1: 1;
		uint32_t PINTIF_2: 1;
		uint32_t PINTIF_3: 1;
		uint32_t PINTIF_4: 1;
		uint32_t PINTIF_5: 1;
		uint32_t PINTIF_6: 1;
		uint32_t PINTIF_7: 1;
		uint32_t RESERVED0: 24;
	};
	uint32_t Word;
} GPIO_PINTIF_Typedef;

typedef union
{
	struct
	{
		uint32_t PINT0: 3;
		uint32_t RESERVED0: 1;
		uint32_t PINT1: 3;
		uint32_t RESERVED1: 1;
		uint32_t PINT2: 3;
		uint32_t RESERVED2: 1;
		uint32_t PINT3: 3;
		uint32_t RESERVED3: 1;
		uint32_t PINT4: 3;
		uint32_t RESERVED4: 1;
		uint32_t PINT5: 3;
		uint32_t RESERVED5: 1;
		uint32_t PINT6: 3;
		uint32_t RESERVED6: 1;
		uint32_t PINT7: 3;
		uint32_t RESERVED7: 1;
	};
	uint32_t Word;
} GPIO_PINTSEL_Typedef;

typedef union
{
	struct
	{
		uint32_t PINT0: 3;
		uint32_t RESERVED0: 1;
		uint32_t PINT1: 3;
		uint32_t RESERVED1: 1;
		uint32_t PINT2: 3;
		uint32_t RESERVED2: 1;
		uint32_t PINT3: 3;
		uint32_t RESERVED3: 1;
		uint32_t PINT4: 3;
		uint32_t RESERVED4: 1;
		uint32_t PINT5: 3;
		uint32_t RESERVED5: 1;
		uint32_t PINT6: 3;
		uint32_t RESERVED6: 1;
		uint32_t PINT7: 3;
		uint32_t RESERVED7: 1;
	};
	uint32_t Word;
} GPIO_PINTCFG_Typedef;

typedef union
{
	struct
	{
		uint32_t KINTIE: 8;
		uint32_t KMASK: 8;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} GPIO_KINTIE_Typedef;

typedef union
{
	struct
	{
		uint32_t KINTIF_0: 1;
		uint32_t KINTIF_1: 1;
		uint32_t KINTIF_2: 1;
		uint32_t KINTIF_3: 1;
		uint32_t KINTIF_4: 1;
		uint32_t KINTIF_5: 1;
		uint32_t KINTIF_6: 1;
		uint32_t KINTIF_7: 1;
		uint32_t RESERVED0: 24;
	};
	uint32_t Word;
} GPIO_KINTIF_Typedef;

typedef union
{
	struct
	{
		uint32_t KINT0: 3;
		uint32_t RESERVED0: 1;
		uint32_t KINT1: 3;
		uint32_t RESERVED1: 1;
		uint32_t KINT2: 3;
		uint32_t RESERVED2: 1;
		uint32_t KINT3: 3;
		uint32_t RESERVED3: 1;
		uint32_t KINT4: 3;
		uint32_t RESERVED4: 1;
		uint32_t KINT5: 3;
		uint32_t RESERVED5: 1;
		uint32_t KINT6: 3;
		uint32_t RESERVED6: 1;
		uint32_t KINT7: 3;
		uint32_t RESERVED7: 1;
	};
	uint32_t Word;
} GPIO_KINTSEL_Typedef;

typedef union
{
	struct
	{
		uint32_t KINT0: 3;
		uint32_t RESERVED0: 1;
		uint32_t KINT1: 3;
		uint32_t RESERVED1: 1;
		uint32_t KINT2: 3;
		uint32_t RESERVED2: 1;
		uint32_t KINT3: 3;
		uint32_t RESERVED3: 1;
		uint32_t KINT4: 3;
		uint32_t RESERVED4: 1;
		uint32_t KINT5: 3;
		uint32_t RESERVED5: 1;
		uint32_t KINT6: 3;
		uint32_t RESERVED6: 1;
		uint32_t KINT7: 3;
		uint32_t RESERVED7: 1;
	};
	uint32_t Word;
} GPIO_KINTCFG_Typedef;

typedef union
{
	struct
	{
		uint32_t FLT_S_0: 1;
		uint32_t FLT_S_1: 1;
		uint32_t FLT_S_2: 1;
		uint32_t FLT_S_3: 1;
		uint32_t FLT_S_4: 1;
		uint32_t FLT_S_5: 1;
		uint32_t FLT_S_6: 1;
		uint32_t FLT_S_7: 1;
		uint32_t RESERVED0: 24;
	};
	uint32_t Word;
} GPIO_IOINTFLTS_Typedef;

typedef union
{
	struct
	{
		uint32_t FLT0_SEL: 4;
		uint32_t FLT1_SEL: 4;
		uint32_t FLT2_SEL: 4;
		uint32_t FLT3_SEL: 4;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} GPIO_TMRFLTSEL_Typedef;

typedef union
{
	struct
	{
		uint32_t FLT_SEL_0: 1;
		uint32_t FLT_SEL_1: 1;
		uint32_t FLT_SEL_2: 1;
		uint32_t FLT_SEL_3: 1;
		uint32_t RESERVED0: 28;
	};
	uint32_t Word;
} GPIO_SPIFLTSEL_Typedef;

typedef union
{
	struct
	{
		uint32_t TX0PS: 2;
		uint32_t TX0PLV: 1;
		uint32_t RESERVED0: 1;
		uint32_t TX0_S0: 1;
		uint32_t TX0_S1: 1;
		uint32_t TX0_S2: 1;
		uint32_t TX0_S3: 1;
		uint32_t TX1PS: 2;
		uint32_t TX1PLV: 1;
		uint32_t RESERVED1: 1;
		uint32_t TX1_S0: 1;
		uint32_t TX1_S1: 1;
		uint32_t TX1_S2: 1;
		uint32_t TX1_S3: 1;
		uint32_t RESERVED2: 16;
	};
	uint32_t Word;
} GPIO_TXPWM_Typedef;

typedef union
{
	struct
	{
		uint32_t BUZEN: 1;
		uint32_t RESERVED0: 7;
		uint32_t BUZ_LOAD: 20;
		uint32_t RESERVED1: 4;
	};
	uint32_t Word;
} GPIO_BUZC_Typedef;

typedef struct
{
	__I GPIO_PAPORT_Typedef PAPORT;
	__IO GPIO_PADATA_Typedef PADATA;
	__O GPIO_PADATABSR_Typedef PADATABSR;
	__O GPIO_PADATABCR_Typedef PADATABCR;
	__O GPIO_PADATABRR_Typedef PADATABRR;
	__IO GPIO_PADIR_Typedef PADIR;
	__O GPIO_PADIRBSR_Typedef PADIRBSR;
	__O GPIO_PADIRBCR_Typedef PADIRBCR;
	__O GPIO_PADIRBRR_Typedef PADIRBRR;
	__IO GPIO_PAFUNC0_Typedef PAFUNC0;
	__IO GPIO_PAFUNC1_Typedef PAFUNC1;
	__IO GPIO_PAFUNC2_Typedef PAFUNC2;
	__IO GPIO_PAFUNC3_Typedef PAFUNC3;
	__IO GPIO_PAINEB_Typedef PAINEB;
	__IO GPIO_PAODE_Typedef PAODE;
	__IO GPIO_PAPUE_Typedef PAPUE;
	__IO GPIO_PAPDE_Typedef PAPDE;
	__IO GPIO_PADS_Typedef PADS;
	uint32_t RESERVED0[14] ;
	__I GPIO_PBPORT_Typedef PBPORT;
	__IO GPIO_PBDATA_Typedef PBDATA;
	__O GPIO_PBDATABSR_Typedef PBDATABSR;
	__O GPIO_PBDATABCR_Typedef PBDATABCR;
	__O GPIO_PBDATABRR_Typedef PBDATABRR;
	__IO GPIO_PBDIR_Typedef PBDIR;
	__O GPIO_PBDIRBSR_Typedef PBDIRBSR;
	__O GPIO_PBDIRBCR_Typedef PBDIRBCR;
	__O GPIO_PBDIRBRR_Typedef PBDIRBRR;
	__IO GPIO_PBFUNC0_Typedef PBFUNC0;
	__IO GPIO_PBFUNC1_Typedef PBFUNC1;
	uint32_t RESERVED1[2] ;
	__IO GPIO_PBINEB_Typedef PBINEB;
	__IO GPIO_PBODE_Typedef PBODE;
	__IO GPIO_PBPUE_Typedef PBPUE;
	__IO GPIO_PBPDE_Typedef PBPDE;
	__IO GPIO_PBDS_Typedef PBDS;
	uint32_t RESERVED2[142] ;
	__IO GPIO_PINTIE_Typedef PINTIE;
	__IO GPIO_PINTIF_Typedef PINTIF;
	__IO GPIO_PINTSEL_Typedef PINTSEL;
	__IO GPIO_PINTCFG_Typedef PINTCFG;
	__IO GPIO_KINTIE_Typedef KINTIE;
	__IO GPIO_KINTIF_Typedef KINTIF;
	__IO GPIO_KINTSEL_Typedef KINTSEL;
	__IO GPIO_KINTCFG_Typedef KINTCFG;
	uint32_t RESERVED3[4] ;
	__IO GPIO_IOINTFLTS_Typedef IOINTFLTS;
	uint32_t RESERVED4[3] ;
	__IO GPIO_TMRFLTSEL_Typedef TMRFLTSEL;
	__IO GPIO_SPIFLTSEL_Typedef SPIFLTSEL;
	uint32_t RESERVED5[14] ;
	__IO GPIO_TXPWM_Typedef TXPWM;
	__IO GPIO_BUZC_Typedef BUZC;
} GPIO_TypeDef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t RST: 1;
		uint32_t RESERVED0: 2;
		uint32_t FLASH_REQ: 1;
		uint32_t FLASH_ACK: 1;
		uint32_t RESERVED1: 1;
		uint32_t FLASH_FAIL: 1;
		uint32_t RESERVED2: 24;
	};
	uint32_t Word;
} IAP_CON_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 2;
		uint32_t IAPCA: 8;
		uint32_t IAPPA: 6;
		uint32_t RESERVED1: 4;
		uint32_t IFREN: 1;
		uint32_t RESERVED2: 11;
	};
	uint32_t Word;
} IAP_ADDR_Typedef;

typedef union
{
	struct
	{
		uint32_t IAPD: 32;
	};
	uint32_t Word;
} IAP_DATA_Typedef;

typedef union
{
	struct
	{
		uint32_t TRIG: 32;
	};
	uint32_t Word;
} IAP_TRIG_Typedef;

typedef union
{
	struct
	{
		uint32_t IAPUL: 32;
	};
	uint32_t Word;
} IAP_UL_Typedef;

typedef union
{
	struct
	{
		uint32_t BSY: 1;
		uint32_t ERASE_END: 1;
		uint32_t PROG_END: 1;
		uint32_t TIMEOUT_ERR: 1;
		uint32_t RESERVED0: 28;
	};
	uint32_t Word;
} IAP_STA_Typedef;

typedef struct
{
	__IO IAP_CON_Typedef CON;
	__IO IAP_ADDR_Typedef ADDR;
	__IO IAP_DATA_Typedef DATA;
	__IO IAP_TRIG_Typedef TRIG;
	__IO IAP_UL_Typedef UL;
	__IO IAP_STA_Typedef STA;
} IAP_TypeDef;

typedef union
{
	struct
	{
		uint32_t DR: 12;
		uint32_t RESERVED0: 20;
	};
	uint32_t Word;
} ADC_DR_Typedef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t TRIG: 1;
		uint32_t ACP_EN: 1;
		uint32_t RESERVED0: 29;
	};
	uint32_t Word;
} ADC_CON0_Typedef;

typedef union
{
	struct
	{
		uint32_t CLKDIV: 3;
		uint32_t CLKS: 1;
		uint32_t RESERVED0: 4;
		uint32_t VREFP: 2;
		uint32_t VREFN: 1;
		uint32_t VRBUF_EN: 1;
		uint32_t SMPS: 1;
		uint32_t SMPON: 1;
		uint32_t HSEN: 1;
		uint32_t RESERVED1: 1;
		uint32_t ST: 5;
		uint32_t RESERVED2: 3;
		uint32_t VCMBUF_EN: 1;
		uint32_t VCMBUF_HS: 1;
		uint32_t RESERVED3: 6;
	};
	uint32_t Word;
} ADC_CON1_Typedef;

typedef union
{
	struct
	{
		uint32_t CHS: 5;
		uint32_t RESERVED0: 3;
		uint32_t VDD5_FLAG_EN: 1;
		uint32_t RESERVED1: 23;
	};
	uint32_t Word;
} ADC_CHS_Typedef;

typedef union
{
	struct
	{
		uint32_t IE: 1;
		uint32_t ACPMINIE: 1;
		uint32_t ACPMAXIE: 1;
		uint32_t ACPOVIE: 1;
		uint32_t RESERVED0: 28;
	};
	uint32_t Word;
} ADC_IE_Typedef;

typedef union
{
	struct
	{
		uint32_t IF: 1;
		uint32_t ACPMINIF: 1;
		uint32_t ACPMAXIF: 1;
		uint32_t ACPOVIF: 1;
		uint32_t RESERVED0: 28;
	};
	uint32_t Word;
} ADC_IF_Typedef;

typedef union
{
	struct
	{
		uint32_t OVFL_TIME: 12;
		uint32_t RESERVED0: 4;
		uint32_t TIMES: 2;
		uint32_t RESERVED1: 2;
		uint32_t CLKS: 1;
		uint32_t RESERVED2: 11;
	};
	uint32_t Word;
} ADC_ACPC_Typedef;

typedef union
{
	struct
	{
		uint32_t CMP_MIN: 12;
		uint32_t RESERVED0: 4;
		uint32_t CMP_MAX: 12;
		uint32_t RESERVED1: 4;
	};
	uint32_t Word;
} ADC_ACPCMP_Typedef;

typedef union
{
	struct
	{
		uint32_t MEAN_DATA: 12;
		uint32_t RESERVED0: 20;
	};
	uint32_t Word;
} ADC_ACPMEAN_Typedef;

typedef union
{
	struct
	{
		uint32_t VREF_EN: 1;
		uint32_t VREF_SEL: 1;
		uint32_t IREF_EN: 1;
		uint32_t RESERVED0: 29;
	};
	uint32_t Word;
} ADC_VREFCON_Typedef;

typedef struct
{
	__I ADC_DR_Typedef DR;
	__IO ADC_CON0_Typedef CON0;
	__IO ADC_CON1_Typedef CON1;
	__IO ADC_CHS_Typedef CHS;
	__IO ADC_IE_Typedef IE;
	__IO ADC_IF_Typedef IF;
	uint32_t RESERVED0[4] ;
	__IO ADC_ACPC_Typedef ACPC;
	uint32_t RESERVED1 ;
	__IO ADC_ACPCMP_Typedef ACPCMP;
	__I ADC_ACPMEAN_Typedef ACPMEAN;
	uint32_t RESERVED2[2] ;
	__IO ADC_VREFCON_Typedef VREFCON;
} ADC_TypeDef;

typedef union
{
	struct
	{
		uint32_t TMUP: 1;
		uint32_t TMWR: 1;
		uint32_t HSWI: 1;
		uint32_t RESERVED0: 1;
		uint32_t CLKS: 2;
		uint32_t XST: 1;
		uint32_t PON: 1;
		uint32_t RESERVED1: 24;
	};
	uint32_t Word;
} RTC_CON_Typedef;

typedef union
{
	struct
	{
		uint32_t CALF: 8;
		uint32_t DEV: 1;
		uint32_t COCR: 3;
		uint32_t CLKC: 1;
		uint32_t RESERVED0: 19;
	};
	uint32_t Word;
} RTC_CAL_Typedef;

typedef union
{
	struct
	{
		uint32_t WM: 7;
		uint32_t RESERVED0: 1;
		uint32_t WH: 6;
		uint32_t RESERVED1: 2;
		uint32_t WW: 7;
		uint32_t RESERVED2: 9;
	};
	uint32_t Word;
} RTC_WA_Typedef;

typedef union
{
	struct
	{
		uint32_t DM: 7;
		uint32_t RESERVED0: 1;
		uint32_t DH: 6;
		uint32_t RESERVED1: 18;
	};
	uint32_t Word;
} RTC_DA_Typedef;

typedef union
{
	struct
	{
		uint32_t SEC: 7;
		uint32_t RESERVED0: 1;
		uint32_t MIN: 7;
		uint32_t RESERVED1: 1;
		uint32_t HOUR: 6;
		uint32_t RESERVED2: 10;
	};
	uint32_t Word;
} RTC_HMS_Typedef;

typedef union
{
	struct
	{
		uint32_t WEAK: 7;
		uint32_t RESERVED0: 1;
		uint32_t DAY: 6;
		uint32_t RESERVED1: 2;
		uint32_t MON: 5;
		uint32_t RESERVED2: 3;
		uint32_t YEAR: 8;
	};
	uint32_t Word;
} RTC_YMDW_Typedef;

typedef union
{
	struct
	{
		uint32_t SCDIE: 1;
		uint32_t MINIE: 1;
		uint32_t HORIE: 1;
		uint32_t DAYIE: 1;
		uint32_t MONIE: 1;
		uint32_t RESERVED0: 3;
		uint32_t DALE: 1;
		uint32_t WALE: 1;
		uint32_t RESERVED1: 22;
	};
	uint32_t Word;
} RTC_IE_Typedef;

typedef union
{
	struct
	{
		uint32_t SCDIF: 1;
		uint32_t MINIF: 1;
		uint32_t HORIF: 1;
		uint32_t DAYIF: 1;
		uint32_t MONIF: 1;
		uint32_t RESERVED0: 3;
		uint32_t DAFG: 1;
		uint32_t WAFG: 1;
		uint32_t RESERVED1: 22;
	};
	uint32_t Word;
} RTC_IF_Typedef;

typedef union
{
	struct
	{
		uint32_t WP: 32;
	};
	uint32_t Word;
} RTC_WP_Typedef;

typedef struct
{
	__IO RTC_CON_Typedef CON;
	__IO RTC_CAL_Typedef CAL;
	__IO RTC_WA_Typedef WA;
	__IO RTC_DA_Typedef DA;
	__IO RTC_HMS_Typedef HMS;
	__IO RTC_YMDW_Typedef YMDW;
	__IO RTC_IE_Typedef IE;
	__IO RTC_IF_Typedef IF;
	__IO RTC_WP_Typedef WP;
} RTC_TypeDef;

typedef union
{
	struct
	{
		uint32_t COMS: 3;
		uint32_t VLCDEN: 1;
		uint32_t FLIK: 1;
		uint32_t WFS: 1;
		uint32_t CLK_SEL: 2;
		uint32_t BIAS: 2;
		uint32_t RS: 3;
		uint32_t RT: 3;
		uint32_t PRS: 6;
		uint32_t RESERVED0: 6;
		uint32_t BVS: 4;
	};
	uint32_t Word;
} LCD_CON0_Typedef;

typedef union
{
	struct
	{
		uint32_t TON: 8;
		uint32_t TOFF: 8;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} LCD_TWI_Typedef;

typedef union
{
	struct
	{
		uint32_t SEG: 28;
		uint32_t RESERVED0: 4;
	};
	uint32_t Word;
} LCD_SEL_Typedef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t SEL: 1;
		uint32_t RESERVED0: 2;
		uint32_t RST: 1;
		uint32_t RESERVED1: 27;
	};
	uint32_t Word;
} LCD_CON1_Typedef;

typedef union
{
	struct
	{
		uint32_t D0: 32;
	};
	uint32_t Word;
} LCD_D0_Typedef;

typedef union
{
	struct
	{
		uint32_t D1: 32;
	};
	uint32_t Word;
} LCD_D1_Typedef;

typedef union
{
	struct
	{
		uint32_t D2: 32;
	};
	uint32_t Word;
} LCD_D2_Typedef;

typedef union
{
	struct
	{
		uint32_t D3: 32;
	};
	uint32_t Word;
} LCD_D3_Typedef;

typedef union
{
	struct
	{
		uint32_t D4: 32;
	};
	uint32_t Word;
} LCD_D4_Typedef;

typedef union
{
	struct
	{
		uint32_t D5: 32;
	};
	uint32_t Word;
} LCD_D5_Typedef;

typedef union
{
	struct
	{
		uint32_t D6: 32;
	};
	uint32_t Word;
} LCD_D6_Typedef;

typedef struct
{
	__IO LCD_CON0_Typedef CON0;
	__IO LCD_TWI_Typedef TWI;
	__IO LCD_SEL_Typedef SEL;
	uint32_t RESERVED0 ;
	__IO LCD_CON1_Typedef CON1;
	uint32_t RESERVED1[3] ;
	__IO LCD_D0_Typedef D0;
	__IO LCD_D1_Typedef D1;
	__IO LCD_D2_Typedef D2;
	__IO LCD_D3_Typedef D3;
	__IO LCD_D4_Typedef D4;
	__IO LCD_D5_Typedef D5;
	__IO LCD_D6_Typedef D6;
} LCD_TypeDef;

typedef union
{
	struct
	{
		uint32_t MUX: 3;
		uint32_t RESERVED0: 3;
		uint32_t CLK_SEL: 2;
		uint32_t RESERVED1: 8;
		uint32_t PRS: 6;
		uint32_t RESERVED2: 10;
	};
	uint32_t Word;
} LED_CON0_Typedef;

typedef union
{
	struct
	{
		uint32_t COM: 8;
		uint32_t SEG: 8;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} LED_SEL_Typedef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t SEL: 1;
		uint32_t RESERVED0: 2;
		uint32_t RST: 1;
		uint32_t RESERVED1: 27;
	};
	uint32_t Word;
} LED_CON1_Typedef;

typedef union
{
	struct
	{
		uint32_t D0: 32;
	};
	uint32_t Word;
} LED_D0_Typedef;

typedef union
{
	struct
	{
		uint32_t D1: 32;
	};
	uint32_t Word;
} LED_D1_Typedef;

typedef struct
{
	__IO LED_CON0_Typedef CON0;
	uint32_t RESERVED0 ;
	__IO LED_SEL_Typedef SEL;
	uint32_t RESERVED1 ;
	__IO LED_CON1_Typedef CON1;
	uint32_t RESERVED2[3] ;
	__IO LED_D0_Typedef D0;
	__IO LED_D1_Typedef D1;
} LED_TypeDef;

typedef union
{
	struct
	{
		uint32_t LOAD: 32;
	};
	uint32_t Word;
} WDT_LOAD_Typedef;

typedef union
{
	struct
	{
		uint32_t VALUE: 32;
	};
	uint32_t Word;
} WDT_VALUE_Typedef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t IE: 1;
		uint32_t RSTEN: 1;
		uint32_t CLKS: 1;
		uint32_t RESERVED0: 28;
	};
	uint32_t Word;
} WDT_CON_Typedef;

typedef union
{
	struct
	{
		uint32_t INTCLR: 32;
	};
	uint32_t Word;
} WDT_INTCLR_Typedef;

typedef union
{
	struct
	{
		uint32_t WDTIF: 1;
		uint32_t RESERVED0: 31;
	};
	uint32_t Word;
} WDT_RIS_Typedef;

typedef union
{
	struct
	{
		uint32_t LOCK: 1;
		uint32_t RESERVED0: 31;
	};
	uint32_t Word;
} WDT_LOCK_Typedef;

typedef struct
{
	__O WDT_LOAD_Typedef LOAD;
	__I WDT_VALUE_Typedef VALUE;
	__IO WDT_CON_Typedef CON;
	__O WDT_INTCLR_Typedef INTCLR;
	__I WDT_RIS_Typedef RIS;
	uint32_t RESERVED0[59] ;
	__IO WDT_LOCK_Typedef LOCK;
} WDT_TypeDef;

typedef union
{
	struct
	{
		uint32_t CNT0: 16;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} T16N_CNT0_Typedef;

typedef union
{
	struct
	{
		uint32_t CNT1: 16;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} T16N_CNT1_Typedef;

typedef union
{
	struct
	{
		uint32_t PRECNT: 8;
		uint32_t RESERVED0: 24;
	};
	uint32_t Word;
} T16N_PRECNT_Typedef;

typedef union
{
	struct
	{
		uint32_t PREMAT: 8;
		uint32_t RESERVED0: 24;
	};
	uint32_t Word;
} T16N_PREMAT_Typedef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t CS: 2;
		uint32_t SYNC: 1;
		uint32_t EDGE: 2;
		uint32_t MOD: 2;
		uint32_t MAT0S: 2;
		uint32_t MAT1S: 2;
		uint32_t MAT2S: 2;
		uint32_t MAT3S: 2;
		uint32_t ASYWEN: 1;
		uint32_t RESERVED0: 15;
	};
	uint32_t Word;
} T16N_CON0_Typedef;

typedef union
{
	struct
	{
		uint32_t CAPPE: 1;
		uint32_t CAPNE: 1;
		uint32_t CAPIS0: 1;
		uint32_t CAPIS1: 1;
		uint32_t CAPT: 4;
		uint32_t CAPL0: 1;
		uint32_t CAPL1: 1;
		uint32_t RESERVED0: 22;
	};
	uint32_t Word;
} T16N_CON1_Typedef;

typedef union
{
	struct
	{
		uint32_t MOE0: 1;
		uint32_t MOE1: 1;
		uint32_t POL0: 1;
		uint32_t POL1: 1;
		uint32_t PWMMOD: 2;
		uint32_t PWMDZE: 1;
		uint32_t RESERVED0: 1;
		uint32_t MOM0: 2;
		uint32_t MOM1: 2;
		uint32_t MOM2: 2;
		uint32_t MOM3: 2;
		uint32_t PWMBKE0: 1;
		uint32_t PWMBKE1: 1;
		uint32_t PWMBKL0: 1;
		uint32_t PWMBKL1: 1;
		uint32_t RESERVED1: 2;
		uint32_t PWMBKP0: 1;
		uint32_t PWMBKP1: 1;
		uint32_t PWMBKF: 1;
		uint32_t RESERVED2: 7;
	};
	uint32_t Word;
} T16N_CON2_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT0IE: 1;
		uint32_t MAT1IE: 1;
		uint32_t MAT2IE: 1;
		uint32_t MAT3IE: 1;
		uint32_t TOP0IE: 1;
		uint32_t TOP1IE: 1;
		uint32_t CAP0IE: 1;
		uint32_t CAP1IE: 1;
		uint32_t PBK0IE: 1;
		uint32_t PBK1IE: 1;
		uint32_t RESERVED0: 22;
	};
	uint32_t Word;
} T16N_IE_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT0IF: 1;
		uint32_t MAT1IF: 1;
		uint32_t MAT2IF: 1;
		uint32_t MAT3IF: 1;
		uint32_t TOP0IF: 1;
		uint32_t TOP1IF: 1;
		uint32_t CAP0IF: 1;
		uint32_t CAP1IF: 1;
		uint32_t PBK0IF: 1;
		uint32_t PBK1IF: 1;
		uint32_t RESERVED0: 22;
	};
	uint32_t Word;
} T16N_IF_Typedef;

typedef union
{
	struct
	{
		uint32_t PDZ: 8;
		uint32_t RESERVED0: 24;
	};
	uint32_t Word;
} T16N_PDZ_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 1;
		uint32_t P0MAT0TRE: 1;
		uint32_t P0MAT1TRE: 1;
		uint32_t P0TOP0TRE: 1;
		uint32_t RESERVED1: 1;
		uint32_t P1MAT2TRE: 1;
		uint32_t P1MAT3TRE: 1;
		uint32_t P1TOP1TRE: 1;
		uint32_t RESERVED2: 24;
	};
	uint32_t Word;
} T16N_PTR_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT0: 16;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} T16N_MAT0_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT1: 16;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} T16N_MAT1_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT2: 16;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} T16N_MAT2_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT3: 16;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} T16N_MAT3_Typedef;

typedef union
{
	struct
	{
		uint32_t TOP0: 16;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} T16N_TOP0_Typedef;

typedef union
{
	struct
	{
		uint32_t TOP1: 16;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} T16N_TOP1_Typedef;

typedef struct
{
	__IO T16N_CNT0_Typedef CNT0;
	__IO T16N_CNT1_Typedef CNT1;
	__IO T16N_PRECNT_Typedef PRECNT;
	__IO T16N_PREMAT_Typedef PREMAT;
	__IO T16N_CON0_Typedef CON0;
	__IO T16N_CON1_Typedef CON1;
	__IO T16N_CON2_Typedef CON2;
	uint32_t RESERVED0 ;
	__IO T16N_IE_Typedef IE;
	__IO T16N_IF_Typedef IF;
	__IO T16N_PDZ_Typedef PDZ;
	__IO T16N_PTR_Typedef PTR;
	__IO T16N_MAT0_Typedef MAT0;
	__IO T16N_MAT1_Typedef MAT1;
	__IO T16N_MAT2_Typedef MAT2;
	__IO T16N_MAT3_Typedef MAT3;
	__IO T16N_TOP0_Typedef TOP0;
	__IO T16N_TOP1_Typedef TOP1;
} T16N_TypeDef;

typedef union
{
	struct
	{
		uint32_t CNT: 32;
	};
	uint32_t Word;
} T32N_CNT_Typedef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t CS: 2;
		uint32_t SYNC: 1;
		uint32_t EDGE: 2;
		uint32_t MOD: 2;
		uint32_t MAT0S: 2;
		uint32_t MAT1S: 2;
		uint32_t MAT2S: 2;
		uint32_t MAT3S: 2;
		uint32_t ASYNC_WREN: 1;
		uint32_t RESERVED0: 15;
	};
	uint32_t Word;
} T32N_CON0_Typedef;

typedef union
{
	struct
	{
		uint32_t CAPPE: 1;
		uint32_t CAPNE: 1;
		uint32_t CAPIS0: 1;
		uint32_t CAPIS1: 1;
		uint32_t CAPT: 4;
		uint32_t CAPL0: 1;
		uint32_t CAPL1: 1;
		uint32_t RESERVED0: 6;
		uint32_t MOE0: 1;
		uint32_t MOE1: 1;
		uint32_t RESERVED1: 6;
		uint32_t MOM0: 2;
		uint32_t MOM1: 2;
		uint32_t MOM2: 2;
		uint32_t MOM3: 2;
	};
	uint32_t Word;
} T32N_CON1_Typedef;

typedef union
{
	struct
	{
		uint32_t PRECNT: 8;
		uint32_t RESERVED0: 24;
	};
	uint32_t Word;
} T32N_PRECNT_Typedef;

typedef union
{
	struct
	{
		uint32_t PREMAT: 8;
		uint32_t RESERVED0: 24;
	};
	uint32_t Word;
} T32N_PREMAT_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT0IE: 1;
		uint32_t MAT1IE: 1;
		uint32_t MAT2IE: 1;
		uint32_t MAT3IE: 1;
		uint32_t IE: 1;
		uint32_t CAP0IE: 1;
		uint32_t CAP1IE: 1;
		uint32_t RESERVED0: 25;
	};
	uint32_t Word;
} T32N_IE_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT0IF: 1;
		uint32_t MAT1IF: 1;
		uint32_t MAT2IF: 1;
		uint32_t MAT3IF: 1;
		uint32_t IF: 1;
		uint32_t CAP0IF: 1;
		uint32_t CAP1IF: 1;
		uint32_t RESERVED0: 25;
	};
	uint32_t Word;
} T32N_IF_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT0: 32;
	};
	uint32_t Word;
} T32N_MAT0_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT1: 32;
	};
	uint32_t Word;
} T32N_MAT1_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT2: 32;
	};
	uint32_t Word;
} T32N_MAT2_Typedef;

typedef union
{
	struct
	{
		uint32_t MAT3: 32;
	};
	uint32_t Word;
} T32N_MAT3_Typedef;

typedef struct
{
	__IO T32N_CNT_Typedef CNT;
	__IO T32N_CON0_Typedef CON0;
	__IO T32N_CON1_Typedef CON1;
	uint32_t RESERVED0 ;
	__IO T32N_PRECNT_Typedef PRECNT;
	__IO T32N_PREMAT_Typedef PREMAT;
	__IO T32N_IE_Typedef IE;
	__IO T32N_IF_Typedef IF;
	__IO T32N_MAT0_Typedef MAT0;
	__IO T32N_MAT1_Typedef MAT1;
	__IO T32N_MAT2_Typedef MAT2;
	__IO T32N_MAT3_Typedef MAT3;
} T32N_TypeDef;

typedef union
{
	struct
	{
		uint32_t TXEN: 1;
		uint32_t TRST: 1;
		uint32_t TBCLR: 1;
		uint32_t TXI: 1;
		uint32_t RESERVED0: 4;
		uint32_t TXMOD: 4;
		uint32_t TXP: 1;
		uint32_t TXFS: 1;
		uint32_t RESERVED1: 2;
		uint32_t RXEN: 1;
		uint32_t RRST: 1;
		uint32_t RBCLR: 1;
		uint32_t RXI: 1;
		uint32_t BDEN: 1;
		uint32_t IDEN: 1;
		uint32_t RESERVED2: 2;
		uint32_t RXMOD: 4;
		uint32_t RXP: 1;
		uint32_t RESERVED3: 3;
	};
	uint32_t Word;
} UART_CON0_Typedef;

typedef union
{
	struct
	{
		uint32_t TBIM: 2;
		uint32_t RESERVED0: 2;
		uint32_t RBIM: 2;
		uint32_t RESERVED1: 2;
		uint32_t BCS: 3;
		uint32_t RESERVED2: 1;
		uint32_t BDM: 2;
		uint32_t RESERVED3: 2;
		uint32_t IDM: 2;
		uint32_t RESERVED4: 14;
	};
	uint32_t Word;
} UART_CON1_Typedef;

typedef union
{
	struct
	{
		uint32_t BRFRA: 4;
		uint32_t BRINT: 12;
		uint32_t RESERVED0: 16;
	};
	uint32_t Word;
} UART_BRR_Typedef;

typedef union
{
	struct
	{
		uint32_t TBPTR: 4;
		uint32_t TBOV: 1;
		uint32_t TXBUSY: 1;
		uint32_t RESERVED0: 2;
		uint32_t RBPTR: 4;
		uint32_t RBOV: 1;
		uint32_t RXBUSY: 1;
		uint32_t RESERVED1: 2;
		uint32_t FER0: 1;
		uint32_t PER0: 1;
		uint32_t FER1: 1;
		uint32_t PER1: 1;
		uint32_t FER2: 1;
		uint32_t PER2: 1;
		uint32_t FER3: 1;
		uint32_t PER3: 1;
		uint32_t RESERVED2: 8;
	};
	uint32_t Word;
} UART_STA_Typedef;

typedef union
{
	struct
	{
		uint32_t TBIE: 1;
		uint32_t TCIE: 1;
		uint32_t RESERVED0: 6;
		uint32_t TBWEIE: 1;
		uint32_t TBWOIE: 1;
		uint32_t RESERVED1: 6;
		uint32_t RBIE: 1;
		uint32_t IDIE: 1;
		uint32_t RESERVED2: 6;
		uint32_t ROIE: 1;
		uint32_t FEIE: 1;
		uint32_t PEIE: 1;
		uint32_t BDEIE: 1;
		uint32_t RBREIE: 1;
		uint32_t RBROIE: 1;
		uint32_t RESERVED3: 2;
	};
	uint32_t Word;
} UART_IE_Typedef;

typedef union
{
	struct
	{
		uint32_t TBIF: 1;
		uint32_t TCIF: 1;
		uint32_t RESERVED0: 6;
		uint32_t TBWEIF: 1;
		uint32_t TBWOIF: 1;
		uint32_t RESERVED1: 6;
		uint32_t RBIF: 1;
		uint32_t IDIF: 1;
		uint32_t RESERVED2: 6;
		uint32_t ROIF: 1;
		uint32_t FEIF: 1;
		uint32_t PEIF: 1;
		uint32_t BDEIF: 1;
		uint32_t RBREIF: 1;
		uint32_t RBROIF: 1;
		uint32_t RESERVED3: 2;
	};
	uint32_t Word;
} UART_IF_Typedef;

typedef union
{
	uint8_t Byte[4];
	uint16_t HalfWord[2];
	uint32_t Word;
} UART_TBW_Typedef;

typedef union
{
	uint8_t Byte[4];
	uint16_t HalfWord[2];
	uint32_t Word;
} UART_RBR_Typedef;

typedef union
{
	struct
	{
		uint32_t TB0: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP0: 1;
		uint32_t TBFF0: 1;
		uint32_t RESERVED1: 18;
	};
	uint32_t Word;
} UART_TB0_Typedef;

typedef union
{
	struct
	{
		uint32_t TB1: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP1: 1;
		uint32_t TBFF1: 1;
		uint32_t RESERVED1: 18;
	};
	uint32_t Word;
} UART_TB1_Typedef;

typedef union
{
	struct
	{
		uint32_t TB2: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP2: 1;
		uint32_t TBFF2: 1;
		uint32_t RESERVED1: 18;
	};
	uint32_t Word;
} UART_TB2_Typedef;

typedef union
{
	struct
	{
		uint32_t TB3: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP3: 1;
		uint32_t TBFF3: 1;
		uint32_t RESERVED1: 18;
	};
	uint32_t Word;
} UART_TB3_Typedef;

typedef union
{
	struct
	{
		uint32_t TB4: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP4: 1;
		uint32_t TBFF4: 1;
		uint32_t RESERVED1: 18;
	};
	uint32_t Word;
} UART_TB4_Typedef;

typedef union
{
	struct
	{
		uint32_t TB5: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP5: 1;
		uint32_t TBFF5: 1;
		uint32_t RESERVED1: 18;
	};
	uint32_t Word;
} UART_TB5_Typedef;

typedef union
{
	struct
	{
		uint32_t TB6: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP6: 1;
		uint32_t TBFF6: 1;
		uint32_t RESERVED1: 18;
	};
	uint32_t Word;
} UART_TB6_Typedef;

typedef union
{
	struct
	{
		uint32_t TB7: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP7: 1;
		uint32_t TBFF7: 1;
		uint32_t RESERVED1: 18;
	};
	uint32_t Word;
} UART_TB7_Typedef;

typedef union
{
	struct
	{
		uint32_t RB0: 9;
		uint32_t RESERVED0: 3;
		uint32_t RP0: 1;
		uint32_t RBFF0: 1;
		uint32_t FE0: 1;
		uint32_t PE0: 1;
		uint32_t RESERVED1: 16;
	};
	uint32_t Word;
} UART_RB0_Typedef;

typedef union
{
	struct
	{
		uint32_t RB1: 9;
		uint32_t RESERVED0: 3;
		uint32_t RP1: 1;
		uint32_t RBFF1: 1;
		uint32_t FE1: 1;
		uint32_t PE1: 1;
		uint32_t RESERVED1: 16;
	};
	uint32_t Word;
} UART_RB1_Typedef;

typedef union
{
	struct
	{
		uint32_t RB2: 9;
		uint32_t RESERVED0: 3;
		uint32_t RP2: 1;
		uint32_t RBFF2: 1;
		uint32_t FE2: 1;
		uint32_t PE2: 1;
		uint32_t RESERVED1: 16;
	};
	uint32_t Word;
} UART_RB2_Typedef;

typedef union
{
	struct
	{
		uint32_t RB3: 9;
		uint32_t RESERVED0: 3;
		uint32_t RP3: 1;
		uint32_t RBFF3: 1;
		uint32_t FE3: 1;
		uint32_t PE3: 1;
		uint32_t RESERVED1: 16;
	};
	uint32_t Word;
} UART_RB3_Typedef;

typedef union
{
	struct
	{
		uint32_t RB4: 5;
		uint32_t RESERVED0: 3;
		uint32_t RP4: 1;
		uint32_t RBFF4: 1;
		uint32_t FE4: 1;
		uint32_t PE4: 1;
		uint32_t RESERVED1: 16;
	};
	uint32_t Word;
} UART_RB4_Typedef;

typedef union
{
	struct
	{
		uint32_t RB5: 9;
		uint32_t RESERVED0: 3;
		uint32_t RP5: 1;
		uint32_t RBFF5: 1;
		uint32_t FE5: 1;
		uint32_t PE5: 1;
		uint32_t RESERVED1: 16;
	};
	uint32_t Word;
} UART_RB5_Typedef;

typedef union
{
	struct
	{
		uint32_t RB6: 9;
		uint32_t RESERVED0: 3;
		uint32_t RP6: 1;
		uint32_t RBFF6: 1;
		uint32_t FE6: 1;
		uint32_t PE6: 1;
		uint32_t RESERVED1: 16;
	};
	uint32_t Word;
} UART_RB6_Typedef;

typedef union
{
	struct
	{
		uint32_t RB7: 9;
		uint32_t RESERVED0: 3;
		uint32_t RP7: 1;
		uint32_t RBFF7: 1;
		uint32_t FE7: 1;
		uint32_t PE7: 1;
		uint32_t RESERVED1: 16;
	};
	uint32_t Word;
} UART_RB7_Typedef;

typedef struct
{
	__IO UART_CON0_Typedef CON0;
	__IO UART_CON1_Typedef CON1;
	uint32_t RESERVED0[2] ;
	__IO UART_BRR_Typedef BRR;
	__I UART_STA_Typedef STA;
	__IO UART_IE_Typedef IE;
	__IO UART_IF_Typedef IF;
	__O UART_TBW_Typedef TBW;
	__I UART_RBR_Typedef RBR;
	uint32_t RESERVED1[6] ;
	__I UART_TB0_Typedef TB0;
	__I UART_TB1_Typedef TB1;
	__I UART_TB2_Typedef TB2;
	__I UART_TB3_Typedef TB3;
	__I UART_TB4_Typedef TB4;
	__I UART_TB5_Typedef TB5;
	__I UART_TB6_Typedef TB6;
	__I UART_TB7_Typedef TB7;
	__I UART_RB0_Typedef RB0;
	__I UART_RB1_Typedef RB1;
	__I UART_RB2_Typedef RB2;
	__I UART_RB3_Typedef RB3;
	__I UART_RB4_Typedef RB4;
	__I UART_RB5_Typedef RB5;
	__I UART_RB6_Typedef RB6;
	__I UART_RB7_Typedef RB7;
} UART_TypeDef;

typedef union
{
	struct
	{
		uint32_t TXEN: 1;
		uint32_t TRST: 1;
		uint32_t TBCLR: 1;
		uint32_t RESERVED0: 5;
		uint32_t TXMOD: 4;
		uint32_t TXP: 1;
		uint32_t TXFS: 1;
		uint32_t RESERVED1: 2;
		uint32_t RXEN: 1;
		uint32_t RRST: 1;
		uint32_t RBCLR: 1;
		uint32_t RESERVED2: 5;
		uint32_t RXMOD: 4;
		uint32_t RXP: 1;
		uint32_t RESERVED3: 3;
	};
	uint32_t Word;
} EUART_CON0_Typedef;

typedef union
{
	struct
	{
		uint32_t TBIM: 2;
		uint32_t RESERVED0: 2;
		uint32_t RBIM: 2;
		uint32_t RESERVED1: 2;
		uint32_t BCS: 3;
		uint32_t RESERVED2: 5;
		uint32_t TXBUSY: 1;
		uint32_t RXBUSY: 1;
		uint32_t RESERVED3: 14;
	};
	uint32_t Word;
} EUART_CON1_Typedef;

typedef union
{
	struct
	{
		uint32_t MOD: 1;
		uint32_t ERST: 1;
		uint32_t CK0E: 1;
		uint32_t CK1E: 1;
		uint32_t CHS: 1;
		uint32_t IOC: 1;
		uint32_t DAS: 1;
		uint32_t PS: 1;
		uint32_t ARTE: 1;
		uint32_t ARRE: 1;
		uint32_t TNAS: 2;
		uint32_t ARTS: 2;
		uint32_t CKS: 2;
		uint32_t ETUS: 8;
		uint32_t BGTE: 1;
		uint32_t RESERVED0: 3;
		uint32_t RNACK: 1;
		uint32_t TXFEND: 1;
		uint32_t RESERVED1: 2;
	};
	uint32_t Word;
} EUART_CON2_Typedef;

typedef union
{
	struct
	{
		uint32_t BRR: 11;
		uint32_t RESERVED0: 21;
	};
	uint32_t Word;
} EUART_BRR_Typedef;

typedef union
{
	struct
	{
		uint32_t TBIE: 1;
		uint32_t TCIE: 1;
		uint32_t RESERVED0: 6;
		uint32_t TBWEIE: 1;
		uint32_t RESERVED1: 3;
		uint32_t ARTEIE: 1;
		uint32_t RNAIE: 1;
		uint32_t RESERVED2: 2;
		uint32_t RBIE: 1;
		uint32_t RESERVED3: 7;
		uint32_t ROIE: 1;
		uint32_t FEIE: 1;
		uint32_t PEIE: 1;
		uint32_t RESERVED4: 1;
		uint32_t RBREIE: 1;
		uint32_t RESERVED5: 3;
	};
	uint32_t Word;
} EUART_IE_Typedef;

typedef union
{
	struct
	{
		uint32_t TBIF: 1;
		uint32_t TCIF: 1;
		uint32_t RESERVED0: 6;
		uint32_t TBWEIF: 1;
		uint32_t RESERVED1: 3;
		uint32_t ARTEIF: 1;
		uint32_t RNAIF: 1;
		uint32_t RESERVED2: 2;
		uint32_t RBIF: 1;
		uint32_t RESERVED3: 7;
		uint32_t ROIF: 1;
		uint32_t FEIF: 1;
		uint32_t PEIF: 1;
		uint32_t RESERVED4: 1;
		uint32_t RBREIF: 1;
		uint32_t RESERVED5: 3;
	};
	uint32_t Word;
} EUART_IF_Typedef;

typedef union
{
	uint8_t Byte[4];
	uint16_t HalfWord[2];
	uint32_t Word;
} EUART_TBW_Typedef;

typedef union
{
	uint8_t Byte[4];
	uint16_t HalfWord[2];
	uint32_t Word;
} EUART_RBR_Typedef;

typedef union
{
	struct
	{
		uint32_t TB0: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP0: 1;
		uint32_t TBEF0: 1;
		uint32_t RESERVED1: 2;
		uint32_t TB1: 9;
		uint32_t RESERVED2: 3;
		uint32_t TP1: 1;
		uint32_t TBEF1: 1;
		uint32_t RESERVED3: 2;
	};
	uint32_t Word;
} EUART_TB01_Typedef;

typedef union
{
	struct
	{
		uint32_t TB2: 9;
		uint32_t RESERVED0: 3;
		uint32_t TP2: 1;
		uint32_t TBEF2: 1;
		uint32_t RESERVED1: 2;
		uint32_t TB3: 9;
		uint32_t RESERVED2: 3;
		uint32_t TP3: 1;
		uint32_t TBEF3: 1;
		uint32_t RESERVED3: 2;
	};
	uint32_t Word;
} EUART_TB23_Typedef;

typedef union
{
	struct
	{
		uint32_t RB0: 9;
		uint32_t RESERVED0: 3;
		uint32_t RP0: 1;
		uint32_t RBFF0: 1;
		uint32_t FE0: 1;
		uint32_t PE0: 1;
		uint32_t RB1: 9;
		uint32_t RESERVED1: 3;
		uint32_t RP1: 1;
		uint32_t RBFF1: 1;
		uint32_t FE1: 1;
		uint32_t PE1: 1;
	};
	uint32_t Word;
} EUART_RB01_Typedef;

typedef union
{
	struct
	{
		uint32_t RB2: 9;
		uint32_t RESERVED0: 3;
		uint32_t RP2: 1;
		uint32_t RBFF2: 1;
		uint32_t FE2: 1;
		uint32_t PE2: 1;
		uint32_t RB3: 9;
		uint32_t RESERVED1: 3;
		uint32_t RP3: 1;
		uint32_t RBFF3: 1;
		uint32_t FE3: 1;
		uint32_t PE3: 1;
	};
	uint32_t Word;
} EUART_RB23_Typedef;

typedef struct
{
	__IO EUART_CON0_Typedef CON0;
	__IO EUART_CON1_Typedef CON1;
	__IO EUART_CON2_Typedef CON2;
	uint32_t RESERVED0 ;
	__IO EUART_BRR_Typedef BRR;
	uint32_t RESERVED1 ;
	__IO EUART_IE_Typedef IE;
	__IO EUART_IF_Typedef IF;
	__O EUART_TBW_Typedef TBW;
	__I EUART_RBR_Typedef RBR;
	uint32_t RESERVED2[6] ;
	__I EUART_TB01_Typedef TB01;
	__I EUART_TB23_Typedef TB23;
	__I EUART_RB01_Typedef RB01;
	__I EUART_RB23_Typedef RB23;
} EUART_TypeDef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t RST: 1;
		uint32_t MS: 1;
		uint32_t REN: 1;
		uint32_t RESERVED0: 1;
		uint32_t DRE: 1;
		uint32_t DFS: 2;
		uint32_t RESERVED1: 8;
		uint32_t TME: 1;
		uint32_t TMS: 1;
		uint32_t TMP: 6;
		uint32_t DW: 3;
		uint32_t RESERVED2: 3;
		uint32_t TXCLR: 1;
		uint32_t RXCLR: 1;
	};
	uint32_t Word;
} SPI_CON_Typedef;

typedef union
{
	uint8_t Byte[4];
	uint16_t HalfWord[2];
	uint32_t Word;
} SPI_TBW_Typedef;

typedef union
{
	uint8_t Byte[4];
	uint16_t HalfWord[2];
	uint32_t Word;
} SPI_RBR_Typedef;

typedef union
{
	struct
	{
		uint32_t TBIE: 1;
		uint32_t RBIE: 1;
		uint32_t TEIE: 1;
		uint32_t ROIE: 1;
		uint32_t IDIE: 1;
		uint32_t NSSIE: 1;
		uint32_t TBWEIE: 1;
		uint32_t RESERVED0: 1;
		uint32_t TBIM: 2;
		uint32_t RBIM: 2;
		uint32_t RESERVED1: 20;
	};
	uint32_t Word;
} SPI_IE_Typedef;

typedef union
{
	struct
	{
		uint32_t TBIF: 1;
		uint32_t RBIF: 1;
		uint32_t TEIF: 1;
		uint32_t ROIF: 1;
		uint32_t IDIF: 1;
		uint32_t NSSIF: 1;
		uint32_t TBWEIF: 1;
		uint32_t RESERVED0: 25;
	};
	uint32_t Word;
} SPI_IF_Typedef;

typedef union
{
	struct
	{
		uint32_t TB0: 8;
		uint32_t TB1: 8;
		uint32_t TB2: 8;
		uint32_t TB3: 8;
	};
	uint32_t Word;
} SPI_TB_Typedef;

typedef union
{
	struct
	{
		uint32_t RB0: 8;
		uint32_t RB1: 8;
		uint32_t RB2: 8;
		uint32_t RB3: 8;
	};
	uint32_t Word;
} SPI_RB_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 7;
		uint32_t NSS: 1;
		uint32_t TBEF0: 1;
		uint32_t TBEF1: 1;
		uint32_t TBEF2: 1;
		uint32_t TBEF3: 1;
		uint32_t RBFF0: 1;
		uint32_t RBFF1: 1;
		uint32_t RBFF2: 1;
		uint32_t RBFF3: 1;
		uint32_t IDLE: 1;
		uint32_t RESERVED1: 15;
	};
	uint32_t Word;
} SPI_STA_Typedef;

typedef union
{
	struct
	{
		uint32_t CKS: 10;
		uint32_t RESERVED0: 22;
	};
	uint32_t Word;
} SPI_CKS_Typedef;

typedef struct
{
	__IO SPI_CON_Typedef CON;
	uint32_t RESERVED0 ;
	__O SPI_TBW_Typedef TBW;
	__I SPI_RBR_Typedef RBR;
	__IO SPI_IE_Typedef IE;
	__IO SPI_IF_Typedef IF;
	__I SPI_TB_Typedef TB;
	__I SPI_RB_Typedef RB;
	__I SPI_STA_Typedef STA;
	__IO SPI_CKS_Typedef CKS;
} SPI_TypeDef;

typedef union
{
	struct
	{
		uint32_t EN: 1;
		uint32_t RST: 1;
		uint32_t SCLOD: 1;
		uint32_t SDAOD: 1;
		uint32_t SCLSE: 1;
		uint32_t SDASE: 1;
		uint32_t RESERVED0: 1;
		uint32_t TJE: 1;
		uint32_t TJP: 8;
		uint32_t RW: 1;
		uint32_t SA: 7;
		uint32_t RESERVED1: 8;
	};
	uint32_t Word;
} I2C_CON_Typedef;

typedef union
{
	struct
	{
		uint32_t MS: 1;
		uint32_t RDM: 3;
		uint32_t CSE: 1;
		uint32_t ANAE: 1;
		uint32_t SRAE: 1;
		uint32_t SPAE: 1;
		uint32_t ADLY: 3;
		uint32_t ADE: 1;
		uint32_t TIS: 4;
		uint32_t SRT: 1;
		uint32_t SPT: 1;
		uint32_t RDT: 1;
		uint32_t BLD: 1;
		uint32_t RESERVED0: 4;
		uint32_t TAS: 1;
		uint32_t RESERVED1: 7;
	};
	uint32_t Word;
} I2C_MOD_Typedef;

typedef union
{
	struct
	{
		uint32_t SRIE: 1;
		uint32_t SPIE: 1;
		uint32_t TBIE: 1;
		uint32_t RBIE: 1;
		uint32_t TEIE: 1;
		uint32_t ROIE: 1;
		uint32_t NAIE: 1;
		uint32_t TBWEIE: 1;
		uint32_t TBIM: 2;
		uint32_t RBIM: 2;
		uint32_t TIDLEIE: 1;
		uint32_t RESERVED0: 19;
	};
	uint32_t Word;
} I2C_IE_Typedef;

typedef union
{
	struct
	{
		uint32_t SRIF: 1;
		uint32_t SPIF: 1;
		uint32_t TBIF: 1;
		uint32_t RBIF: 1;
		uint32_t TEIF: 1;
		uint32_t ROIF: 1;
		uint32_t NAIF: 1;
		uint32_t TBWEIF: 1;
		uint32_t RESERVED0: 4;
		uint32_t TIDLEIF: 1;
		uint32_t RESERVED1: 19;
	};
	uint32_t Word;
} I2C_IF_Typedef;

typedef union
{
	uint8_t Byte[4];
	uint16_t HalfWord[2];
	uint32_t Word;
} I2C_TBW_Typedef;

typedef union
{
	uint8_t Byte[4];
	uint16_t HalfWord[2];
	uint32_t Word;
} I2C_RBR_Typedef;

typedef union
{
	struct
	{
		uint32_t TB0: 8;
		uint32_t TB1: 8;
		uint32_t TB2: 8;
		uint32_t TB3: 8;
	};
	uint32_t Word;
} I2C_TB_Typedef;

typedef union
{
	struct
	{
		uint32_t RB0: 8;
		uint32_t RB1: 8;
		uint32_t RB2: 8;
		uint32_t RB3: 8;
	};
	uint32_t Word;
} I2C_RB_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 8;
		uint32_t TBEF0: 1;
		uint32_t TBEF1: 1;
		uint32_t TBEF2: 1;
		uint32_t TBEF3: 1;
		uint32_t RBFF0: 1;
		uint32_t RBFF1: 1;
		uint32_t RBFF2: 1;
		uint32_t RBFF3: 1;
		uint32_t ACK: 1;
		uint32_t IDLE: 1;
		uint32_t RESERVED1: 14;
	};
	uint32_t Word;
} I2C_STA_Typedef;

typedef struct
{
	__IO I2C_CON_Typedef CON;
	__IO I2C_MOD_Typedef MOD;
	__IO I2C_IE_Typedef IE;
	__IO I2C_IF_Typedef IF;
	__O I2C_TBW_Typedef TBW;
	__I I2C_RBR_Typedef RBR;
	__I I2C_TB_Typedef TB;
	__I I2C_RB_Typedef RB;
	__I I2C_STA_Typedef STA;
} I2C_TypeDef;

typedef union
{
	struct
	{
		uint32_t ENABLE: 1;
		uint32_t TICKINT: 1;
		uint32_t CLKSOURCE: 1;
		uint32_t RESERVED0: 13;
		uint32_t COUNTFLAG: 1;
		uint32_t RESERVED1: 15;
	};
	uint32_t Word;
} SYSTICK_CSR_Typedef;

typedef union
{
	struct
	{
		uint32_t RELOAD: 24;
		uint32_t RESERVED0: 8;
	};
	uint32_t Word;
} SYSTICK_RVR_Typedef;

typedef union
{
	struct
	{
		uint32_t CURRENT: 24;
		uint32_t RESERVED0: 8;
	};
	uint32_t Word;
} SYSTICK_CVR_Typedef;

typedef union
{
	struct
	{
		uint32_t TENMS: 24;
		uint32_t RESERVED0: 6;
		uint32_t SKEW: 1;
		uint32_t NOREF: 1;
	};
	uint32_t Word;
} SYSTICK_CALIB_Typedef;

typedef struct
{
	__IO SYSTICK_CSR_Typedef CSR;
	__IO SYSTICK_RVR_Typedef RVR;
	__IO SYSTICK_CVR_Typedef CVR;
	__IO SYSTICK_CALIB_Typedef CALIB;
} SYSTICK_TypeDef;

typedef union
{
	struct
	{
		uint32_t SETENA: 32;
	};
	uint32_t Word;
} NVIC_ISER_Typedef;

typedef union
{
	struct
	{
		uint32_t CLRENA: 32;
	};
	uint32_t Word;
} NVIC_ICER_Typedef;

typedef union
{
	struct
	{
		uint32_t SETPEND: 32;
	};
	uint32_t Word;
} NVIC_ISPR_Typedef;

typedef union
{
	struct
	{
		uint32_t CLRPEND: 32;
	};
	uint32_t Word;
} NVIC_ICPR_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 6;
		uint32_t PRI_0: 2;
		uint32_t RESERVED1: 6;
		uint32_t PRI_1: 2;
		uint32_t RESERVED2: 6;
		uint32_t PRI_2: 2;
		uint32_t RESERVED3: 6;
		uint32_t PRI_3: 2;
	};
	uint32_t Word;
} NVIC_PR0_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 6;
		uint32_t PRI_4: 2;
		uint32_t RESERVED1: 6;
		uint32_t PRI_5: 2;
		uint32_t RESERVED2: 6;
		uint32_t PRI_6: 2;
		uint32_t RESERVED3: 6;
		uint32_t PRI_7: 2;
	};
	uint32_t Word;
} NVIC_PR1_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 6;
		uint32_t PRI_8: 2;
		uint32_t RESERVED1: 6;
		uint32_t PRI_9: 2;
		uint32_t RESERVED2: 6;
		uint32_t PRI_10: 2;
		uint32_t RESERVED3: 6;
		uint32_t PRI_11: 2;
	};
	uint32_t Word;
} NVIC_PR2_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 6;
		uint32_t PRI_12: 2;
		uint32_t RESERVED1: 6;
		uint32_t PRI_13: 2;
		uint32_t RESERVED2: 6;
		uint32_t PRI_14: 2;
		uint32_t RESERVED3: 6;
		uint32_t PRI_15: 2;
	};
	uint32_t Word;
} NVIC_PR3_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 6;
		uint32_t PRI_16: 2;
		uint32_t RESERVED1: 6;
		uint32_t PRI_17: 2;
		uint32_t RESERVED2: 6;
		uint32_t PRI_18: 2;
		uint32_t RESERVED3: 6;
		uint32_t PRI_19: 2;
	};
	uint32_t Word;
} NVIC_PR4_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 6;
		uint32_t PRI_20: 2;
		uint32_t RESERVED1: 6;
		uint32_t PRI_21: 2;
		uint32_t RESERVED2: 6;
		uint32_t PRI_22: 2;
		uint32_t RESERVED3: 6;
		uint32_t PRI_23: 2;
	};
	uint32_t Word;
} NVIC_PR5_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 6;
		uint32_t PRI_24: 2;
		uint32_t RESERVED1: 6;
		uint32_t PRI_25: 2;
		uint32_t RESERVED2: 6;
		uint32_t PRI_26: 2;
		uint32_t RESERVED3: 6;
		uint32_t PRI_27: 2;
	};
	uint32_t Word;
} NVIC_PR6_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 6;
		uint32_t PRI_28: 2;
		uint32_t RESERVED1: 6;
		uint32_t PRI_29: 2;
		uint32_t RESERVED2: 6;
		uint32_t PRI_30: 2;
		uint32_t RESERVED3: 6;
		uint32_t PRI_31: 2;
	};
	uint32_t Word;
} NVIC_PR7_Typedef;

typedef struct
{
	__IO NVIC_ISER_Typedef ISER;
	uint32_t RESERVED0[31] ;
	__IO NVIC_ICER_Typedef ICER;
	uint32_t RESERVED1[31] ;
	__IO NVIC_ISPR_Typedef ISPR;
	uint32_t RESERVED2[31] ;
	__IO NVIC_ICPR_Typedef ICPR;
	uint32_t RESERVED3[95] ;
	__IO NVIC_PR0_Typedef PR0;
	__IO NVIC_PR1_Typedef PR1;
	__IO NVIC_PR2_Typedef PR2;
	__IO NVIC_PR3_Typedef PR3;
	__IO NVIC_PR4_Typedef PR4;
	__IO NVIC_PR5_Typedef PR5;
	__IO NVIC_PR6_Typedef PR6;
	__IO NVIC_PR7_Typedef PR7;
} NVIC_TypeDef;

typedef union
{
	struct
	{
		uint32_t REVISION: 4;
		uint32_t PARTNO: 12;
		uint32_t CONSTANT: 4;
		uint32_t VARIANT: 4;
		uint32_t IMPLEMENTER: 8;
	};
	uint32_t Word;
} SCB_CPUID_Typedef;

typedef union
{
	struct
	{
		uint32_t VECTACTIVE: 6;
		uint32_t RESERVED0: 6;
		uint32_t VECTPENDING: 6;
		uint32_t RESERVED1: 4;
		uint32_t ISRPENDDING: 1;
		uint32_t RESERVED2: 2;
		uint32_t PENDSTCLR: 1;
		uint32_t PENDSTSET: 1;
		uint32_t RESERVED3: 4;
		uint32_t NMIPENDSET: 1;
	};
	uint32_t Word;
} SCB_ICSR_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 1;
		uint32_t VECTCLRACTIVE: 1;
		uint32_t SYSRESETREQ: 1;
		uint32_t RESERVED1: 12;
		uint32_t ENDIANNESS: 1;
		uint32_t VECTKEY: 16;
	};
	uint32_t Word;
} SCB_AIRCR_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 1;
		uint32_t SLEEPONEXIT: 1;
		uint32_t SLEEPDEEP: 1;
		uint32_t RESERVED1: 1;
		uint32_t SEVONPEND: 1;
		uint32_t RESERVED2: 27;
	};
	uint32_t Word;
} SCB_SCR_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 3;
		uint32_t UNALIGN_TRP: 1;
		uint32_t RESERVED1: 5;
		uint32_t STKALIGN: 1;
		uint32_t RESERVED2: 22;
	};
	uint32_t Word;
} SCB_CCR_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 30;
		uint32_t PRI_11: 2;
	};
	uint32_t Word;
} SCB_SHPR2_Typedef;

typedef union
{
	struct
	{
		uint32_t RESERVED0: 22;
		uint32_t PRI_14: 2;
		uint32_t RESERVED1: 6;
		uint32_t PRI_15: 2;
	};
	uint32_t Word;
} SCB_SHPR3_Typedef;

typedef struct
{
	__I SCB_CPUID_Typedef CPUID;
	__IO SCB_ICSR_Typedef ICSR;
	uint32_t RESERVED0 ;
	__IO SCB_AIRCR_Typedef AIRCR;
	__IO SCB_SCR_Typedef SCR;
	__I SCB_CCR_Typedef CCR;
	uint32_t RESERVED1 ;
	__IO SCB_SHPR2_Typedef SHPR2;
	__IO SCB_SHPR3_Typedef SHPR3;
} SCB_TypeDef;

/******************************************************************************/
/* 物理地址映射 */
/******************************************************************************/

/* Base addresses */
#define FLASH_BASE (0x00000000UL)
#define SRAM_BASE (0x20000000UL)
#define APB_BASE (0x40000000UL)
#define RAM_BASE (0x60000000UL)
#define SYS_BASE (0xE0000000UL)

/* APB peripherals */
#define SCU_BASE (APB_BASE + 0x00000)
#define GPIO_BASE (APB_BASE + 0x00400)
#define IAP_BASE (APB_BASE + 0x00800)
#define ADC_BASE (APB_BASE + 0x01000)
#define RTC_BASE (APB_BASE + 0x01400)
#define LCD_BASE (APB_BASE + 0x01800)
#define LED_BASE (APB_BASE + 0x01800)
#define WDT_BASE (APB_BASE + 0x01C00)
#define T16N0_BASE (APB_BASE + 0x02000)
#define T16N1_BASE (APB_BASE + 0x02400)
#define T16N2_BASE (APB_BASE + 0x02800)
#define T16N3_BASE (APB_BASE + 0x02C00)
#define T32N0_BASE (APB_BASE + 0x04000)
#define UART0_BASE (APB_BASE + 0x06000)
#define UART1_BASE (APB_BASE + 0x06400)
#define EUART0_BASE (APB_BASE + 0x07000)
#define SPI0_BASE (APB_BASE + 0x08000)
#define SPI1_BASE (APB_BASE + 0x08400)
#define I2C0_BASE (APB_BASE + 0x09000)


/******************************************************************************/
/* 外设定义 */
/******************************************************************************/
#define SCU ((SCU_TypeDef *) SCU_BASE )
#define GPIO ((GPIO_TypeDef *) GPIO_BASE )
#define IAP ((IAP_TypeDef *) IAP_BASE )
#define ADC ((ADC_TypeDef *) ADC_BASE )
#define RTC ((RTC_TypeDef *) RTC_BASE )
#define LCD ((LCD_TypeDef *) LCD_BASE )
#define LED ((LED_TypeDef *) LED_BASE )
#define WDT ((WDT_TypeDef *) WDT_BASE )
#define T16N0 ((T16N_TypeDef *) T16N0_BASE )
#define T16N1 ((T16N_TypeDef *) T16N1_BASE )
#define T16N2 ((T16N_TypeDef *) T16N2_BASE )
#define T16N3 ((T16N_TypeDef *) T16N3_BASE )
#define T32N0 ((T32N_TypeDef *) T32N0_BASE )
#define UART0 ((UART_TypeDef *) UART0_BASE )
#define UART1 ((UART_TypeDef *) UART1_BASE )
#define EUART0 ((EUART_TypeDef *) EUART0_BASE )
#define SPI0 ((SPI_TypeDef *) SPI0_BASE )
#define SPI1 ((SPI_TypeDef *) SPI1_BASE )
#define I2C0 ((I2C_TypeDef *) I2C0_BASE )

/******************************************************************************/
/* 唯一码定义 */
/******************************************************************************/
#define UID11 (*(uint8_t *)0x001001D7)
#define UID10 (*(uint8_t *)0x001001D6)
#define UID9 (*(uint8_t *)0x001001D5)
#define UID8 (*(uint8_t *)0x001001D4)
#define UID7 (*(uint8_t *)0x001001D3)
#define UID6 (*(uint8_t *)0x001001D2)
#define UID5 (*(uint8_t *)0x001001D1)
#define UID4 (*(uint8_t *)0x001001D0)
#define UID3 (*(uint8_t *)0x001001C6)
#define UID2 (*(uint8_t *)0x001001C4)
#define UID1 (*(uint8_t *)0x001001C3)
#define UID0 (*(uint8_t *)0x001001C2)

#endif
