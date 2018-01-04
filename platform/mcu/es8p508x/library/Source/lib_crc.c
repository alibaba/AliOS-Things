/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/
#include "lib_crc.h"

uint32_t CRC_getTypeValue(CRC_CONValueStruType con_value)
{
	uint32_t type =0;
	type = (con_value.xorout | con_value.refout | con_value.refin |\
		con_value.mode|con_value.hs_type|con_value.init_data_type);
	return type;
}

uint32_t CRC_EmptayCheck(void* address, uint32_t data_len)
{
    CRC_EM CRC_EmptayCheck = (CRC_EM)(*(uint32_t *)CRC_EmptayCheck_addr);

    uint32_t result;
    __ASM("CPSID i");
    result = (*CRC_EmptayCheck)(address, data_len);
    __ASM("CPSIE i");
    return result;
}

uint32_t CRC_FlashVerify(void* address, uint32_t data_len, uint32_t type)
{
    CRC_FL CRC_FlashVerify = (CRC_FL)(*(uint32_t *)CRC_FlashVerify_addr);

    uint32_t result;
    __ASM("CPSID i");
    result = (*CRC_FlashVerify)(address, data_len, type);
    __ASM("CPSIE i");
    return result;
}

uint32_t CRC_UserCal(void* address, uint32_t data_len, uint32_t type)
{
    CRC_US CRC_UserCal = (CRC_US)(*(uint32_t *)CRC_UserCal_addr);

    uint32_t result;
    __ASM("CPSID i");
    result = (*CRC_UserCal)(address, data_len, type);
    __ASM("CPSIE i");
    return  result;
}

uint32_t CRC_CheckReset(void)
{
    CRC_CH CRC_CheckReset = (CRC_CH)(*(uint32_t *)CRC_CheckReset_addr);

    uint32_t result;
    __ASM("CPSID i");
    result = (*CRC_CheckReset)();
    __ASM("CPSIE i");
    return result;
}

/************************END OF FILE**************************/

