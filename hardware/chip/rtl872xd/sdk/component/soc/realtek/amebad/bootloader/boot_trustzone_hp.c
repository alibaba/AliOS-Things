/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"

//IROM (rx) : 			ORIGIN = 0x10100000, LENGTH = 0x10000	/* ROM: 64k */
//IROM_NS (rx) : 			ORIGIN = 0x10110000, LENGTH = 0x38000	/* ROM: 224k */
//DROM (rx) : 			ORIGIN = 0x101C0000, LENGTH = 0x8000	/* ROM: 32k */
//DROM_NS (rx) : 			ORIGIN = 0x101C8000, LENGTH = 0x10000	/* ROM: 64k */
//ROMBSS_RAM (rw)  : 		ORIGIN = 0x10000000, LENGTH = 0x2000	/* ROM BSS RAM: 8K */
//BOOTLOADER_RAM (rwx)  : 	ORIGIN = 0x10002000, LENGTH = 0x3000	/* BOOT Loader RAM: 12K */
//BD_RAM (rwx)  : 		ORIGIN = 0x10005000, LENGTH = 0x78000	/* MAIN RAM: 480 */
//PRINT_RAM (wx)  : 		ORIGIN = 0x1007D000, LENGTH = 0x1000	/* PRINTF BUFFER RAM: 4k */
//MSP_RAM (wx)  : 		ORIGIN = 0x1007E000, LENGTH = 0x2000	/* MSP & MSP_S RAM: 8k */

/*
// <e>Setup behaviour of Sleep and Exception Handling
*/
#define SCB_CSR_AIRCR_INIT  1

/*
//   <o> Deep Sleep can be enabled by
//     <0=>Secure and Non-Secure state
//     <1=>Secure state only
//   <i> Value for SCB->CSR register bit DEEPSLEEPS
*/
#define SCB_CSR_DEEPSLEEPS_VAL  1

/*
//   <o>System reset request accessible from
//     <0=> Secure and Non-Secure state
//     <1=> Secure state only
//   <i> Value for SCB->AIRCR register bit SYSRESETREQS
*/
#define SCB_AIRCR_SYSRESETREQS_VAL  1

/*
//   <o>Priority of Non-Secure exceptions is
//     <0=> Not altered
//     <1=> Lowered to 0x80-0xFF
//   <i> Value for SCB->AIRCR register bit PRIS
*/
#define SCB_AIRCR_PRIS_VAL      1

/*
//   <o>BusFault, HardFault, and NMI target
//     <0=> Secure state
//     <1=> Non-Secure state
//   <i> Value for SCB->AIRCR register bit BFHFNMINS
//	Notice: HardFault only behaves as a banked exception if AIRCR.BFHFNMINS is 1, otherwise it behaves as an
//	unbanked exception targeting Secure state.
*/
#define SCB_AIRCR_BFHFNMINS_VAL 1

/* this table is configured by usrcfg */
extern const TZ_CFG_TypeDef tz_config[];

/**
  *  @brief Config all SAU & IDAU based on tz_config table.
  *  @retval None       
  */
BOOT_RAM_TEXT_SECTION
void BOOT_RAM_TZCfg(void)
{
	IDAU_TypeDef* IDAU = ((IDAU_TypeDef *) KM4_IDAU_BASE);
	int idex = 0;

	for (idex = 0; idex < IDAU_ENTRYS_NUM; idex++) {
		/*  Check if search to end */
		if (tz_config[idex].Start == 0xFFFFFFFF) {
			break;
		}

		/* set IDAU */
		if (tz_config[idex].NSC == 0) {
			IDAU->ENTRY[idex].IDAU_BARx = tz_config[idex].Start;
			IDAU->ENTRY[idex].IDAU_LARx = tz_config[idex].End;
			IDAU->IDAU_CTRL |= BIT(idex);
		}
	}

	IDAU->IDAU_LOCK = 1;

	for (idex = 0; idex < SAU_ENTRYS_NUM; idex++) {
		/*  Check if search to end */
		if (tz_config[idex].Start == 0xFFFFFFFF) {
			break;
		}

#if defined (configUSE_SAU) && (configUSE_SAU == 1U)
		/* set SAU */
		SAU->RNR  =  (idex & SAU_RNR_REGION_Msk); \
		SAU->RBAR =  (tz_config[idex].Start & SAU_RBAR_BADDR_Msk); \
		SAU->RLAR =  (tz_config[idex].End & SAU_RLAR_LADDR_Msk) | \
			((tz_config[idex].NSC << SAU_RLAR_NSC_Pos)  & SAU_RLAR_NSC_Msk)   | \
			ENABLE << SAU_RLAR_ENABLE_Pos;
#endif
	}
	SAU->CTRL = ((SAU_INIT_CTRL_ENABLE << SAU_CTRL_ENABLE_Pos) & SAU_CTRL_ENABLE_Msk) |
		((SAU_INIT_CTRL_ALLNS  << SAU_CTRL_ALLNS_Pos)  & SAU_CTRL_ALLNS_Msk)   ;

	for (idex = 0; idex < IDAU_ENTRYS_NUM; idex++) {
		//DBG_8195A("IDAU_BAR%d:[%08x:%08x]\n", idex, IDAU->ENTRY[idex].IDAU_BARx, IDAU->ENTRY[idex].IDAU_LARx);
	}

	for (idex = 0; idex < SAU_ENTRYS_NUM; idex++) {
		SAU->RNR = idex;
		//DBG_8195A("SAU_BAR%d:[%08x:%08x]\n", idex, SAU->RBAR, SAU->RLAR);
	}
	
	//DBG_8195A("IDAU_CTRL:%x\n", IDAU->IDAU_CTRL);
	//DBG_8195A("IDAU_LOCK:%x\n", IDAU->IDAU_LOCK);
	//DBG_8195A("SAU_CTRL:%x\n", SAU->CTRL);

	SCB->SCR   = (SCB->SCR   & ~(SCB_SCR_SLEEPDEEPS_Msk    )) |
		((SCB_CSR_DEEPSLEEPS_VAL     << SCB_SCR_SLEEPDEEPS_Pos)     & SCB_SCR_SLEEPDEEPS_Msk);

	SCB->AIRCR = (SCB->AIRCR & 
		~(SCB_AIRCR_VECTKEY_Msk   | SCB_AIRCR_SYSRESETREQS_Msk | SCB_AIRCR_BFHFNMINS_Msk |  SCB_AIRCR_PRIS_Msk)) |
		((0x05FAU                    << SCB_AIRCR_VECTKEY_Pos)      & SCB_AIRCR_VECTKEY_Msk)      |
		//((SCB_AIRCR_SYSRESETREQS_VAL << SCB_AIRCR_SYSRESETREQS_Pos) & SCB_AIRCR_SYSRESETREQS_Msk) | /* reset both secure and non-secure */
		((SCB_AIRCR_PRIS_VAL         << SCB_AIRCR_PRIS_Pos)         & SCB_AIRCR_PRIS_Msk)         |
		((SCB_AIRCR_BFHFNMINS_VAL    << SCB_AIRCR_BFHFNMINS_Pos)    & SCB_AIRCR_BFHFNMINS_Msk);

	/* Enable all access to FPU */
	SCB->CPACR |= 0x00f00000;
	SCB_NS->CPACR |=  0x00f00000;

	/* <0=> Secure state <1=> Non-Secure state */
	NVIC->ITNS[0] = 0xFFFFFFFF; /* IRQ 0~31: Non-Secure state */
	NVIC->ITNS[1] = 0x0003FFFF; /* IRQ 32~49: Non-Secure state, 50~63 */
}
