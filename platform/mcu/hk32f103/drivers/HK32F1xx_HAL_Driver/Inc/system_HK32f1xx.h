
#ifndef __SYSTEM_HK32F10X_H
#define __SYSTEM_HK32F10X_H

#ifdef __cplusplus
 extern "C" {
#endif 


extern uint32_t SystemCoreClock;          
extern const uint8_t  AHBPrescTable[16U]; 
extern const uint8_t  APBPrescTable[8U];  

  
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_HK32F10X_H */


