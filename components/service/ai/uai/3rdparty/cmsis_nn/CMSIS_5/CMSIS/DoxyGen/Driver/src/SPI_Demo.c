#include "Driver_SPI.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
 
 
void mySPI_Thread(void const *argument);
osThreadId tid_mySPI_Thread;
 
 
/* SPI Driver */
extern ARM_DRIVER_SPI Driver_SPI0;
 
 
void mySPI_callback(uint32_t event)
{
    switch (event)
    {
    case ARM_SPI_EVENT_TRANSFER_COMPLETE:
        /* Success: Wakeup Thread */
        osSignalSet(tid_mySPI_Thread, 0x01);
        break;
    case ARM_SPI_EVENT_DATA_LOST:
        /*  Occurs in slave mode when data is requested/sent by master
            but send/receive/transfer operation has not been started
            and indicates that data is lost. Occurs also in master mode
            when driver cannot transfer data fast enough. */
        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    case ARM_SPI_EVENT_MODE_FAULT:
        /*  Occurs in master mode when Slave Select is deactivated and
            indicates Master Mode Fault. */
        __breakpoint(0);  /* Error: Call debugger or replace with custom error handling */
        break;
    }
}
 
/* Test data buffers */
const uint8_t testdata_out[8] = { 0, 1, 2, 3, 4, 5, 6, 7 }; 
uint8_t       testdata_in [8];
 
void mySPI_Thread(void const* arg)
{
    ARM_DRIVER_SPI* SPIdrv = &Driver_SPI0;
    osEvent evt;
 
#ifdef DEBUG
    ARM_DRIVER_VERSION   version;
    ARM_SPI_CAPABILITIES drv_capabilities;
 
    version = SPIdrv->GetVersion();
    if (version.api < 0x200) /* requires at minimum API version 2.00 or higher */
    {                        /* error handling                                 */
        return;
    }
 
    drv_capabilities = SPIdrv->GetCapabilities();
    if (drv_capabilities.event_mode_fault == 0)
    {                        /* error handling */
        return;
    }
#endif
 
    /* Initialize the SPI driver */
    SPIdrv->Initialize(mySPI_callback);
    /* Power up the SPI peripheral */
    SPIdrv->PowerControl(ARM_POWER_FULL);
    /* Configure the SPI to Master, 8-bit mode @10000 kBits/sec */
    SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_SS_MASTER_SW | ARM_SPI_DATA_BITS(8), 10000000);
 
    /* SS line = INACTIVE = HIGH */
    SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
 
    /* thread loop */
    while (1)
    {
        /* SS line = ACTIVE = LOW */
        SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
        /* Transmit some data */
        SPIdrv->Send(testdata_out, sizeof(testdata_out));
        /* Wait for completion */
        evt = osSignalWait(0x01, 100);
        if (evt.status == osEventTimeout) {
            __breakpoint(0); /* Timeout error: Call debugger */
        }
        /* SS line = INACTIVE = HIGH */
        SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);

        /* SS line = ACTIVE = LOW */
        SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
        /* Receive 8 bytes of reply */
        SPIdrv->Receive(testdata_in, 8);
        evt = osSignalWait(0x01, 100);
        if (evt.status == osEventTimeout) {
            __breakpoint(0); /* Timeout error: Call debugger */
        }
        /* SS line = INACTIVE = HIGH */
        SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
    }
}
