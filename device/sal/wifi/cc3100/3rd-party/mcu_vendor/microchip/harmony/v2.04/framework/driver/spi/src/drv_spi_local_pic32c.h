#ifndef _DRV_SPI_LOCAL_PIC32C_H_
#define _DRV_SPI_LOCAL_PIC32C_H_

#include "driver/spi/drv_spi.h"
#include "driver/spi/src/drv_spi_sys_queue.h"
#include "driver/spi/src/drv_spi_variant_mapping.h"
#include "osal/osal.h"


#define MAX(a,b) ((a<b) ? b : a)
#define MIN(a,b) ((b<a) ? b : a)

/* Chip Select 0 value */
#define SPI_MR_PCS_PCS0_Val  0x0E

struct DRV_SPI_DRIVER_OBJECT;

/* Function pointer for driver instance tasks */
typedef int32_t (*_DRV_SPI_TasksFptr)(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance);

/* SPI driver instance job object */
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

    DRV_SPI_BUFFER_EVENT status;
    
    DRV_SPI_BUFFER_EVENT_HANDLER completeCB;

    void * context;
    
    void * pClient;

}DRV_SPI_JOB_OBJECT;

/* SPI driver instance object */
struct DRV_SPI_DRIVER_OBJECT
{
    SYS_STATUS                                  status;

    spi_registers_t *                           spiId;

    bool                                        isExclusive;

    uint8_t                                     numClients;
    
    DRV_SPI_TASK_MODE                           taskMode;
    
    DRV_SPI_MODE                    		    spiMode;

    SPI_COMMUNICATION_WIDTH         		    commWidth;
    
    uint32_t                                    spiClkSrc;
    
    uint32_t                        		    baudRate;
    
    uint32_t                                    currentBaudRate;
    
    INT_SOURCE                                  interruptSPI;
    
    bool                                        txIntEnabled;
    
    bool                                        rxIntEnabled;
    
    uint32_t                                    dummyByteValue;
    
    DRV_SPI_BUFFER_EVENT_HANDLER                operationStarting;

    DRV_SPI_BUFFER_EVENT_HANDLER                operationEnded;

    DRV_SPI_SYS_QUEUE_HANDLE                    queue;

    DRV_SPI_JOB_OBJECT *                        currentJob;

    uint8_t                                     symbolsInProgress;

    _DRV_SPI_TasksFptr                          instanceTask;
};

/* SPI client object */
typedef struct _DRV_SPI_CLIENT_OBJECT
{
    struct DRV_SPI_DRIVER_OBJECT*               driverObject;

    DRV_IO_INTENT                               intent;

    struct _DRV_SPI_CLIENT_OBJECT               * pNext;
    
    DRV_SPI_BUFFER_EVENT_HANDLER                operationStarting;

    DRV_SPI_BUFFER_EVENT_HANDLER                operationEnded;
    
    uint32_t                        		    baudRate;
    
}DRV_SPI_CLIENT_OBJECT;

/* SPI driver manager */
typedef struct _DRV_SPI_MANAGER_OBJECT
{
    
    OSAL_SEM_HANDLE_TYPE managerSemaphore;
    
    DRV_SPI_SYS_QUEUE_MANAGER_HANDLE hQueueManager;
    
    DRV_SPI_CLIENT_OBJECT * pFreeClientHead;
    
}DRV_SPI_MANAGER_OBJECT;

/* Local functions declaration */
int32_t _DRV_SPI_SetupHardware(struct DRV_SPI_DRIVER_OBJECT * driverObject, const DRV_SPI_INIT * const init);
int32_t _DRV_SPI_MasterTasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );
int32_t _DRV_SPI_SlaveTasks(struct DRV_SPI_DRIVER_OBJECT * pDrvInstance );


#endif //_DRV_SPI_LOCAL_PIC32C_H_
