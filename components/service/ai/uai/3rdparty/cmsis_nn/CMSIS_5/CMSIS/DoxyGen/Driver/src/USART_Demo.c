#include "Driver_USART.h"
#include "cmsis_os.h"                   /* ARM::CMSIS:RTOS:Keil RTX */
#include <stdio.h>
#include <string.h>
 
void myUART_Thread(void const *argument);
osThreadId tid_myUART_Thread;
 
/* USART Driver */
extern ARM_DRIVER_USART Driver_USART3;
 
 
void myUSART_callback(uint32_t event)
{
  uint32_t mask;

  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;

  if (event & mask) {
    /* Success: Wakeup Thread */
    osSignalSet(tid_myUART_Thread, 0x01);
  }

  if (event & ARM_USART_EVENT_RX_TIMEOUT) {
    __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
  }

  if (event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW)) {
    __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
  }
}

 
/* CMSIS-RTOS Thread - UART command thread */
void myUART_Thread(const void* args)
{
    static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;
    ARM_DRIVER_VERSION     version;
    ARM_USART_CAPABILITIES drv_capabilities;
    char                   cmd;
 
  #ifdef DEBUG
    version = USARTdrv->GetVersion();
    if (version.api < 0x200)   /* requires at minimum API version 2.00 or higher */
    {                          /* error handling */
        return;
    }
    drv_capabilities = USARTdrv->GetCapabilities();
    if (drv_capabilities.event_tx_complete == 0)
    {                          /* error handling */
        return;
    }
  #endif
 
    /*Initialize the USART driver */
    USARTdrv->Initialize(myUSART_callback);
    /*Power up the USART peripheral */
    USARTdrv->PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 4800 Bits/sec */
    USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, 4800);
     
    /* Enable Receiver and Transmitter lines */
    USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
    USARTdrv->Control (ARM_USART_CONTROL_RX, 1);
 
    USARTdrv->Send("\nPress Enter to receive a message", 34);
    osSignalWait(0x01, osWaitForever);
     
    while (1)
    {
        USARTdrv->Receive(&cmd, 1);          /* Get byte from UART */
        osSignalWait(0x01, osWaitForever);
        if (cmd == 13)                       /* CR, send greeting  */
        {
          USARTdrv->Send("\nHello World!", 12);
          osSignalWait(0x01, osWaitForever);
        }
 
    }
}
