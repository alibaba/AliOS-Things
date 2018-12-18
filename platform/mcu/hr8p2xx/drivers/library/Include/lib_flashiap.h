/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/

#ifndef __LIBIAP_H__
#define __LIBIAP_H__

#include "HR8P2xx.h"
#include "type.h"

#define FlashIAP_RegUnLock() (IAP->UL.IAPUL = 0x000000A5)
#define FlashIAP_RegLock()   (IAP->UL.IAPUL = 0x0)

#define FlashIAP_Enable()  (IAP->CON.EN = 0x1)
#define FlashIAP_Disable() (IAP->CON.EN = 0x0)

#define FlashIAP_REQ() (IAP->CON.FLASH_REQ = 0x1)

ErrorStatus FlashIap_Unlock(void);
ErrorStatus FlashIap_WriteEnd(void);
ErrorStatus FlashIap_ErasePage(uint8_t Page_Addr);
ErrorStatus FlashIap_WriteCont(uint8_t Unit_addr, uint8_t Page_addr, uint32_t Data32);
ErrorStatus FlashIap_WriteWord(uint8_t Unit_addr, uint8_t Page_addr, uint32_t Data32);
ErrorStatus Flash_Read(uint32_t * Ram_Addr, uint32_t Flash_Addr, uint8_t Len);

#endif

/*************************END OF FILE**********************/
