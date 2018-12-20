/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#include "lib_flashiap.h"

ErrorStatus IAP_Read(uint32_t * Ram_Addr, uint32_t Flash_Addr,  uint8_t Len)
{
    uint8_t  i;
    uint32_t *  ram_addr32;
    const uint32_t  *  flash_addr32;

    ram_addr32 = (uint32_t *)Ram_Addr;
    flash_addr32 = (const uint32_t  * )Flash_Addr;
    if((Len == 0)&(Len>(0x10000-Flash_Addr)/4))
    {
        return  ERROR;
    }
    for(i=0; i<Len; i++)
    {
        *ram_addr32 = *flash_addr32 ;
        ram_addr32++;
        flash_addr32++;
    }
    return  SUCCESS;
}
