/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/
#include "lib_iap.h"

ErrorStatus IAP_PageErase(uint32_t address)
{
    IAP_PE IAPPageErase = (IAP_PE)(*(uint32_t *)IAP_PageErase_addr);

    ErrorStatus result;
    __ASM("CPSID i");
    result = (*IAPPageErase)(address);
    __ASM("CPSIE i");
    return  result;
}

ErrorStatus IAP_WordProgram(uint32_t address, uint32_t data)
{
    IAP_WP IAPWordProgram = (IAP_WP)(*(uint32_t *)IAP_WordProgram_addr);

    ErrorStatus result;
    __ASM("CPSID i");
    result = (*IAPWordProgram)( address,  data);
    __ASM("CPSIE i");
    return  result;
}

ErrorStatus IAP_PageProgram(uint32_t address, uint32_t data[], uint32_t length, uint32_t erase)  //@0x100001c5
{
    IAP_PP IAPPageProgram = (IAP_PP)(*(uint32_t *)IAP_PageProgram_addr);

    ErrorStatus result;
    __ASM("CPSID i");
    result = (*IAPPageProgram)(address,data,length,erase);
    __ASM("CPSIE i");
    return  result;
}

ErrorStatus IAPRead(uint32_t *Ram_Addr, uint32_t Flash_Addr, uint8_t Len)
{
    uint8_t i;
    uint32_t *ram_addr;
    const uint32_t *flash_addr;

    ram_addr = Ram_Addr;
    flash_addr = (const uint32_t *)Flash_Addr;

    if (Len == 0)
        return ERROR;

    for (i = 0; i < Len; ++i)
    {
        *ram_addr = *flash_addr;
        ++ram_addr;
        ++flash_addr;
    }

    return SUCCESS;
}

/************************END OF FILE**************************/

