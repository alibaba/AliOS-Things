/**
  ******************************************************************************
  * @file    rtl8721d_trustzone.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the definations of trustzone.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */
#ifndef _TRUSTZONE_H_
#define _TRUSTZONE_H_

typedef struct
{
	u32 Start;
	u32 End;
	u32 NSC;
} TZ_CFG_TypeDef;

/*
  *   <q> Enable SAU
  *   <i> Value for SAU->CTRL register bit ENABLE
  */
#define SAU_INIT_CTRL_ENABLE   1

/*
  *   <o> When SAU is disabled
  *     <0=> All Memory is Secure
  *     <1=> All Memory is Non-Secure
  *   <i> Value for SAU->CTRL register bit ALLNS
  *   <i> When all Memory is Non-Secure (ALLNS is 1), IDAU can override memory map configuration.
  */
#define SAU_INIT_CTRL_ALLNS  0

#define SAU_ENTRYS_NUM			8
#define IDAU_ENTRYS_NUM		8

void BOOT_RAM_TZCfg(void);


/* 	The TT instruction takes a memory address and returns the configuration of the Memory Protection Unit (MPU) at that address.
	The cmse_address_info_t is declared as following:
		typedef union { 
			struct cmse_address_info { 
				unsigned mpu_region:8; 
				unsigned :8; 
				unsigned mpu_region_valid:1; 
				unsigned :1; 
				unsigned read_ok:1; 
				unsigned readwrite_ok:1; 
				unsigned :12; 
			} flags; 
			unsigned value; 
		} cmse_address_info_t;

	When executed in the secure state the result of TT instruction is extended to return the SAU and IDAU configurations at the specific address.
	The extended cmse_address_info_t is declared as following:
		typedef union {
			struct cmse_address_info {
				unsigned mpu_region:8;
				unsigned sau_region:8;
				unsigned mpu_region_valid:1;
				unsigned sau_region_valid:1;
				unsigned read_ok:1;
				unsigned readwrite_ok:1;
				unsigned nonsecure_read_ok:1;
				unsigned nonsecure_readwrite_ok:1;
				unsigned secure:1;
				unsigned idau_region_valid:1;
				unsigned idau_region:8;
			} flags;
			unsigned value;
		} cmse_address_info_t;
	
	As a result, these extended bits are only valid when executing in Secure state, and are UNDEFINED if used from Non-secure state.
*/
__STATIC_INLINE u32 TrustZone_IsSecure(void)
{
#if defined (ARM_CORE_CM4)
	cmse_address_info_t cmse_address_info = cmse_TT((void *)DiagPrintf);
	return cmse_address_info.flags.secure;
#else
	return 0;
#endif
}


#endif //_TRUSTZONE_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
