#ifndef __HW_H__
#define __HW_H__

#include "debug.h"
#include "hw_conf.h"


#ifdef __cplusplus
extern "C" {
#endif
void HW_Reset(int mode);
char *HW_Get_MFT_ID(void);
uint32_t HW_Get_MFT_Baud(void);
bool HW_Set_MFT_Baud(uint32_t baud);
char *HW_Get_MFT_SN(void);
char *HW_Get_MFT_Rev(void);
char *HW_Get_MFT_Model(void);
uint8_t HW_GetBatteryLevel( void );
void HW_GetUniqueId( uint8_t *id );
uint32_t HW_GetRandomSeed( void );
void BoardDriverInit(void);
#ifdef __cplusplus
}
#endif
#endif
