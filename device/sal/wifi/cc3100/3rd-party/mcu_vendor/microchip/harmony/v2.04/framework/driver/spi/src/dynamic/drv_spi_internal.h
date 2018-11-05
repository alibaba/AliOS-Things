#ifndef _DRV_SPI_INTERNAL_H_
#define _DRV_SPI_INTERNAL_H_

#include "system_config.h"
#include "driver/spi/drv_spi.h"
#include "osal/osal.h"
#include "driver/spi/src/drv_spi_sys_queue.h"
#include "driver/spi/src/drv_spi_variant_mapping.h"

#define PLIB_SPI_RX_8BIT_FIFO_SIZE(index) 16
#define PLIB_SPI_RX_8BIT_HW_MARK(index) 8
#define PLIB_SPI_RX_16BIT_FIFO_SIZE(index) 8
#define PLIB_SPI_RX_16BIT_HW_MARK(index) 4
#define PLIB_SPI_RX_32BIT_FIFO_SIZE(index) 4
#define PLIB_SPI_RX_32BIT_HW_MARK(index) 2

#define PLIB_SPI_TX_8BIT_FIFO_SIZE(index) 16
#define PLIB_SPI_TX_8BIT_LW_MARK(index) 8
#define PLIB_SPI_TX_16BIT_FIFO_SIZE(index) 8
#define PLIB_SPI_TX_16BIT_LW_MARK(index) 4
#define PLIB_SPI_TX_32BIT_FIFO_SIZE(index) 4
#define PLIB_SPI_TX_32BIT_LW_MARK(index) 2

#define PLIB_DMA_MAX_TRF_SIZE 2048


#define MAX(a,b) ((a<b) ? b : a)
#define MIN(a,b) ((b<a) ? b : a)

struct DRV_SPI_DRIVER_OBJECT;

typedef int32_t (*DRV_SPI_TasksFptr)(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance);
//#if DRV_SPI_DMA
//extern uint8_t sSPITxDummyBuffer[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
//extern uint8_t sSPIRxDummyBuffer[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
//
//#endif
typedef enum {
    DRV_SPI_DMA_NONE = 0,
    DRV_SPI_DMA_DATA_INPROGRESS,
    DRV_SPI_DMA_DUMMY_INPROGRESS,
    DRV_SPI_DMA_COMPLETE,
} DRV_SPI_DMA_STATE;

typedef struct _DRV_SPI_JOB_OBJECT
{
    uint8_t *txBuffer;
    uint8_t *rxBuffer;
    size_t dataTxed;
    size_t dataRxed;
    size_t dataLeftToTx;
    size_t dummyLeftToTx;
    size_t dataLeftToRx;
    size_t dummyLeftToRx;

    DRV_SPI_DMA_STATE txDMAProgressStage;
    DRV_SPI_DMA_STATE rxDMAProgressStage;

    DRV_SPI_BUFFER_EVENT status;
    DRV_SPI_BUFFER_EVENT_HANDLER completeCB;

    void * context;
    void * pClient;

}DRV_SPI_JOB_OBJECT;

struct DRV_SPI_DRIVER_OBJECT
{
    SYS_STATUS                                  status;

    SPI_MODULE_ID                               spiId;

    bool                                        isExclusive;

    uint8_t                                     numClients;

    uint8_t                                     numTrfsSmPolled;
    
    DRV_SPI_TASK_MODE                           taskMode;
    
    DRV_SPI_MODE                    		    spiMode;

    SPI_COMMUNICATION_WIDTH         		    commWidth;
    
    INT_SOURCE                                  txInterruptSource;
    bool                                        txEnabled;

    INT_SOURCE                                  rxInterruptSource;
    bool                                        rxEnabled;

    INT_SOURCE                                  errInterruptSource;
    bool                                        errEnabled;
    
    /* SPI clock source which generates required baud rate */
    SPI_BAUD_RATE_CLOCK                         baudClockSource;
    
    /* when Baud rate clock source is peripheral clock, then this element is used
    to define which peripheral bus clock is used for this particular SPI instance.  */
    CLK_BUSES_PERIPHERAL                        spiClk;
    
    /* Baud Rate Value */
    uint32_t                        		    baudRate;
    uint32_t                                    currentBaudRate;
    
    uint32_t                                    dummyByteValue;
    
    DRV_SPI_BUFFER_EVENT_HANDLER                operationStarting;

    DRV_SPI_BUFFER_EVENT_HANDLER                operationEnded;

    DRV_SPI_SYS_QUEUE_HANDLE                    queue;

    DRV_SPI_JOB_OBJECT *                        currentJob;

    DRV_SPI_TasksFptr                           vfMainTask;


    uint8_t                                     symbolsInProgress;
#if DRV_SPI_DMA
    SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER      sendDMAHander;
    SYS_DMA_CHANNEL_TRANSFER_EVENT_HANDLER      receiveDMAHander;
    /* DMA Channel for the Transmitter*/
    DMA_CHANNEL                                 txDmaChannel;
    /* DMA Channel Priority for the Transmitter*/
    DMA_CHANNEL_PRIORITY                        txDmaPriority;
    /* DMA Channel Mode Enable for the Transmitter*/
    SYS_DMA_CHANNEL_OP_MODE                     txDmaModeEnable;
    /* DMA Trigger Source for the Transmitter*/
    DMA_TRIGGER_SOURCE                          txDmaEventSrc;
    SYS_DMA_CHANNEL_HANDLE                      txDmaChannelHandle;
    /* Threshold for the minimum number of bytes to send to use DMA*/
    uint8_t                                     txDmaThreshold;

    /* DMA Channel for the Receiver*/
    DMA_CHANNEL                                 rxDmaChannel;
    /* DMA Channel Priority for the Receiver*/
    DMA_CHANNEL_PRIORITY                        rxDmaPriority;
    /* DMA Channel Mode Enable for the Receiver*/
    SYS_DMA_CHANNEL_OP_MODE                     rxDmaModeEnable;
    /* DMA Trigger Source for the Receiver*/
    DMA_TRIGGER_SOURCE                          rxDmaEventSrc;
    SYS_DMA_CHANNEL_HANDLE                      rxDmaChannelHandle;
    /* Threshold for the minimum number of bytes to receive to use DMA*/
    uint8_t                                     rxDmaThreshold;
#endif

};

typedef struct _DRV_SPI_CLIENT_OBJECT
{
    /* Driver Object associated with the client */
    struct DRV_SPI_DRIVER_OBJECT*                       driverObject;

    /* The intent with which the client was opened */
    DRV_IO_INTENT                                      	intent;

    struct _DRV_SPI_CLIENT_OBJECT                       * pNext;
    
    DRV_SPI_BUFFER_EVENT_HANDLER                        operationStarting;

    DRV_SPI_BUFFER_EVENT_HANDLER                        operationEnded;
    
    /* Baud Rate Value */
    uint32_t                        		            baudRate;
    
}DRV_SPI_CLIENT_OBJECT;


typedef struct _DRV_SPI_MANAGER_OBJECT
{
    OSAL_SEM_HANDLE_TYPE managerSemaphore;
    DRV_SPI_SYS_QUEUE_MANAGER_HANDLE hQueueManager;
    DRV_SPI_CLIENT_OBJECT * pFreeClientHead;
}DRV_SPI_MANAGER_OBJECT;


int32_t DRV_SPI_ISRMasterEBM8BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_ISRSlaveEBM8BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledMasterEBM8BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledSlaveEBM8BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_ISRMasterRM8BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_ISRSlaveRM8BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledMasterRM8BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledSlaveRM8BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_ISRMasterEBM16BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_ISRSlaveEBM16BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledMasterEBM16BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledSlaveEBM16BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_ISRMasterRM16BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_ISRSlaveRM16BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledMasterRM16BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledSlaveRM16BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_ISRMasterEBM32BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_ISRSlaveEBM32BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledMasterEBM32BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledSlaveEBM32BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_ISRMasterRM32BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_ISRSlaveRM32BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledMasterRM32BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_PolledSlaveRM32BitTasks ( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );


int32_t DRV_SPI_PolledErrorTasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance);
int32_t DRV_SPI_ISRErrorTasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance);

#if DRV_SPI_DMA
void DRV_SPI_ISRDMAMasterSendEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_ISRDMASlaveSendEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_ISRDMAMasterReceiveEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_ISRDMASlaveReceiveEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);

void DRV_SPI_PolledDMAMasterSendEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_PolledDMASlaveSendEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_PolledDMAMasterReceiveEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_PolledDMASlaveReceiveEventHandler8bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);

void DRV_SPI_ISRDMAMasterSendEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_ISRDMASlaveSendEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_ISRDMAMasterReceiveEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_ISRDMASlaveReceiveEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);

void DRV_SPI_PolledDMAMasterSendEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_PolledDMASlaveSendEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_PolledDMAMasterReceiveEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_PolledDMASlaveReceiveEventHandler16bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);

void DRV_SPI_ISRDMAMasterSendEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_ISRDMASlaveSendEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_ISRDMAMasterReceiveEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_ISRDMASlaveReceiveEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);

void DRV_SPI_PolledDMAMasterSendEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_PolledDMASlaveSendEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_PolledDMAMasterReceiveEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
void DRV_SPI_PolledDMASlaveReceiveEventHandler32bit(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);

#endif

int32_t DRV_SPI_MasterEBMSend8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterEBMReceive8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMSend8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMReceive8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_MasterEBMSend16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterEBMReceive16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMSend16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMReceive16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_MasterEBMSend32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterEBMReceive32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMSend32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMReceive32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_SlaveEBMSend8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveEBMReceive8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMSend8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMReceive8BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_SlaveEBMSend16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveEBMReceive16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMSend16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMReceive16BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_SlaveEBMSend32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveEBMReceive32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMSend32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMReceive32BitISR( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_MasterEBMSend8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterEBMReceive8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMSend8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMReceive8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_MasterEBMSend16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterEBMReceive16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMSend16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMReceive16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_MasterEBMSend32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterEBMReceive32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMSend32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_MasterRMReceive32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_SlaveEBMSend8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveEBMReceive8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMSend8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMReceive8BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_SlaveEBMSend16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveEBMReceive16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMSend16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMReceive16BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );

int32_t DRV_SPI_SlaveEBMSend32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveEBMReceive32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMSend32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t DRV_SPI_SlaveRMReceive32BitPolled( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );



int32_t DRV_SPI_SetupHardware(struct DRV_SPI_DRIVER_OBJECT * driverObject, const DRV_SPI_INIT * const init);
int32_t DRV_SPI_SetVTable(struct DRV_SPI_DRIVER_OBJECT * driverObject, const DRV_SPI_INIT * const init);

#if DRV_SPI_DMA
void DRV_SPI_SetupDMA( struct DRV_SPI_DRIVER_OBJECT * pDrvInstance);
extern uint8_t sDrvSpiRxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
extern uint8_t sDrvSpiTxDummy[DRV_SPI_DMA_DUMMY_BUFFER_SIZE];
#endif

#endif
