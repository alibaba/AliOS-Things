/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/
#include "lib_flashiap.h"

ErrorStatus FlashIap_Close_WPROT(uint8_t Page)
{
    if(Page > 64)
        return ERROR;

    if(Page == 64)
    {
        IAP->WPROT2.Word = 0x00000000;
        return SUCCESS;
    }

    if(Page < 32)
    {
        IAP->WPROT0.Word &=~ ((uint32_t)0x1 << Page);
    }
    else
    {
        Page -= 32;
        IAP->WPROT1.Word &=~ ((uint32_t)0x1 << Page);
    }

    return SUCCESS;
}

ErrorStatus FlashIap_Open_WPROT(uint8_t Page)
{
    if(Page > 64)
        return ERROR;

    if(Page == 64)
    {
        IAP->WPROT2.Word = 0x00000001;
        return SUCCESS;
    }

    if(Page < 32)
    {
        IAP->WPROT0.Word &=~ ((uint32_t)0x1 << Page);
        IAP->WPROT0.Word |=  ((uint32_t)0x1 << Page);
    }
    else
    {
        Page -= 32;
        IAP->WPROT1.Word &=~ ((uint32_t)0x1 << Page);
        IAP->WPROT1.Word |=  ((uint32_t)0x1 << Page);
    }

    return SUCCESS;
}

ErrorStatus FlashIap_CloseAll_WPROT(void)
{
    IAP->WPROT0.Word = 0x00000000;
    IAP->WPROT1.Word = 0x00000000;
    IAP->WPROT2.Word = 0x00000000;

    return SUCCESS;
}

ErrorStatus FlashIap_OpenAll_WPROT(void)
{
    IAP->WPROT0.Word = 0xFFFFFFFF;
    IAP->WPROT1.Word = 0xFFFFFFFF;
    IAP->WPROT2.Word = 0xFFFFFFFF;

    return SUCCESS;
}

ErrorStatus FlashIap_Unlock(void)
{
    uint16_t Temp16;

    FlashIAP_RegUnLock();              

    FlashIAP_Enable();                 

    FlashIAP_REQ();                    

    for(Temp16 = 0; Temp16 < 0xFFFF; Temp16++)    
    {
        if(IAP->CON.FLASH_ACK != 0)
            break;
    }

    if(Temp16 == 0xFFFF)
        return ERROR;
    else
        return SUCCESS;
}

ErrorStatus FlashIap_WriteEnd(void)
{
    uint32_t  Temp32;

    FlashIAP_RegUnLock();                           

    IAP->CON.Word &= 0xFFFFFFEE;                    

    for(Temp32 = 0; Temp32 < 0xFFFF; Temp32++)      
    {
        if(IAP->CON.FLASH_ACK == 0)
            break;
    }

    if(Temp32 == 0xFFFF)
        return ERROR;
    else
        return SUCCESS;
}

ErrorStatus FlashIap_ErasePage(uint8_t Page_Addr)
{
    uint16_t  Temp16;
    uint32_t temp;

    temp = __get_PRIMASK();                 
    __disable_irq();                        

    if(FlashIap_Unlock() == ERROR)
    {
        __set_PRIMASK(temp);                
        return ERROR;
    }

    if(FlashIap_CloseAll_WPROT() == ERROR)
    {
        __set_PRIMASK(temp);
        return ERROR;
    }

    IAP->ADDR.IAPPA = Page_Addr;            

    IAP->TRIG.TRIG = 0x00005EA1;            

    for(Temp16 = 0; Temp16 < 0xFFFF; Temp16++)  
    {                                       
        if((IAP->STA.Word & (uint32_t)0x01) == (uint32_t)0x00)
            break;
    }

    if(Temp16 == 0xFFFF)
    {
        __set_PRIMASK(temp);                
        return ERROR;
    }

    for(Temp16 = 0; Temp16 < 0xFFFF; Temp16++)  
    {
        if((IAP->STA.Word & (uint32_t)0x02) == (uint32_t)0x02) 
            break;
    }

    if(Temp16 == 0xFFFF)
    {
        __set_PRIMASK(temp);                
        return ERROR;
    }

    if(FlashIap_WriteEnd() == ERROR)
    {
        __set_PRIMASK(temp);                
        return ERROR;
    }

    if(FlashIap_OpenAll_WPROT() == ERROR)
    {
        __set_PRIMASK(temp);
        return ERROR;
    }

    __set_PRIMASK(temp);                   

    return SUCCESS;
}  

ErrorStatus FlashIap_WriteCont(uint8_t Unit_addr, uint8_t Page_addr, uint32_t Data32)
{
    uint16_t  temp16;

    IAP->ADDR.IAPPA = Page_addr;       
    IAP->ADDR.IAPCA = Unit_addr;

    IAP->DATA.DATA = Data32;          
    IAP->TRIG.TRIG = 0x00005DA2;      

    for(temp16 = 0; temp16 < 0xFFFF; temp16++)
    {
        if((IAP->STA.Word & (uint32_t)0x01) == (uint32_t)0x00)  
            break;
    }

    if(temp16 == 0xFFFF)
        return ERROR;

    for(temp16 = 0; temp16 < 0xFFFF; temp16++)
    {
        if((IAP->STA.Word & 0x04)==0x04)  
            break;
    }

    if(temp16 == 0xFFFF)
        return ERROR;

    return SUCCESS;
}

ErrorStatus FlashIap_WriteWord(uint8_t Unit_addr, uint8_t Page_addr, uint32_t Data32)
{
    uint32_t temp;

    temp = __get_PRIMASK();                 
    __disable_irq();                        

    if(FlashIap_Unlock() == ERROR)
    {
        __set_PRIMASK(temp);                
        return ERROR;
    }

    if(FlashIap_CloseAll_WPROT() == ERROR)
    {
        __set_PRIMASK(temp);                
        return ERROR;
    }

    if(FlashIap_WriteCont(Unit_addr, Page_addr, Data32) == ERROR)
    {
        __set_PRIMASK(temp);                
        return ERROR;
    }

    if(FlashIap_WriteEnd() == ERROR)
    {
        __set_PRIMASK(temp);                
        return ERROR;
    }

    if(FlashIap_OpenAll_WPROT() == ERROR)
    {
        __set_PRIMASK(temp);                
        return ERROR;
    }

    __set_PRIMASK(temp);                    

    return SUCCESS;
}

ErrorStatus Flash_Read(uint32_t * Ram_Addr, uint32_t Flash_Addr,  uint8_t Len)
{
    uint8_t  i;
    uint32_t *ram_addr32;
    const uint32_t *flash_addr32;

    ram_addr32 = (uint32_t *)Ram_Addr;
    flash_addr32 = (const uint32_t *)Flash_Addr;

    if((Len == 0) & (Len>(0x20000 - Flash_Addr) / 4))  
    {
        return ERROR;
    }

    for(i=0; i<Len; i++)
    {
        *ram_addr32 = *flash_addr32;
        ram_addr32++;
        flash_addr32++;
    }

    return SUCCESS;
}

/*************************END OF FILE**********************/

