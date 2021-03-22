#include "Driver_I2C.h"
 
/* I2C driver instance */
extern ARM_DRIVER_I2C            Driver_I2C0; 
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C0;
 
static volatile uint32_t I2C_Event;
 
/* I2C Signal Event function callback */
static void I2C_SignalEvent (uint32_t event) {
  I2C_Event |= event;
}
 
int main (void) {
  uint8_t cnt = 0;
 
  /* Initialize I2C peripheral */
  I2Cdrv->Initialize(I2C_SignalEvent);
 
  /* Power-on I2C peripheral */
  I2Cdrv->PowerControl(ARM_POWER_FULL);
 
  /* Configure I2C bus */
  I2Cdrv->Control(ARM_I2C_OWN_ADDRESS, 0x78);
 
  I2C_Event = 0;

  while (1) {
    /* Receive chunk */
    I2Cdrv->SlaveReceive(&cnt, 1);
    while ((I2C_Event & ARM_I2C_EVENT_TRANSFER_DONE) == 0);
    /* Clear transfer done flag */
    I2C_Event &= ~ARM_I2C_EVENT_TRANSFER_DONE;
 
    /* Transmit chunk back */
    I2Cdrv->SlaveTransmit(&cnt, 1);
    while ((I2C_Event & ARM_I2C_EVENT_TRANSFER_DONE) == 0);
    /* Clear transfer done flag */
    I2C_Event &= ~ARM_I2C_EVENT_TRANSFER_DONE;
  }
}
