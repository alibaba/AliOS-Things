#include "Driver_Flash.h"
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5
   
/* Flash driver instance */
extern ARM_DRIVER_FLASH Driver_Flash0;
static ARM_DRIVER_FLASH * flashDev = &Driver_Flash0;
 
/* CMSIS-RTOS2 Thread Id */
osThreadId_t Flash_Thread_Id;
 
/* Flash signal event */
void Flash_Callback(uint32_t event)
{
  if (event & ARM_FLASH_EVENT_READY) {
    /* The read/program/erase operation is completed */
    osThreadFlagsSet(Flash_Thread_Id, 1U);
  }
  if (event & ARM_FLASH_EVENT_ERROR) {
    /* The read/program/erase operation is completed with errors */
    /* Call debugger or replace with custom error handling */
    __breakpoint(0);  
  }
} 
 
/* CMSIS-RTOS2 Thread */ 
void Flash_Thread (void *argument)
{
  /* Query drivers capabilities */
  const ARM_FLASH_CAPABILITIES capabilities = flashDev->GetCapabilities();
 
  /* Initialize Flash device */
  if (capabilities.event_ready) {
    flashDev->Initialize (&Flash_Callback);
  } else {
    flashDev->Initialize (NULL);
  }
  
  /* Power-on Flash device */
  flashDev->PowerControl (ARM_POWER_FULL);
  
  /* Read data taking data_width into account */
  uint8_t buf[256U];
  flashDev->ReadData (0x1000U, buf, sizeof(buf)>>capabilities.data_width);
  
  /* Wait operation to be completed */
  if (capabilities.event_ready) {
    osThreadFlagsWait (1U, osFlagsWaitAny, 100U);
  } else {
    osDelay(100U);
  }
  
  /* Switch off gracefully */
  flashDev->PowerControl (ARM_POWER_OFF);
  flashDev->Uninitialize ();
}
