#ifndef _DRV_SPI_STATIC_LOCAL_PIC32C_H_
#define _DRV_SPI_STATIC_LOCAL_PIC32C_H_

#include "driver/spi/drv_spi.h"
#include "driver/spi/src/drv_spi_sys_queue.h"
#include "driver/spi/src/drv_spi_variant_mapping.h"
#include "osal/osal.h"


#define MAX(a,b) ((a<b) ? b : a)
#define MIN(a,b) ((b<a) ? b : a)

/* Chip Select 0 value */
#define SPI_MR_PCS_PCS0_Val  0x0E

<#if CONFIG_DRV_SPI_USE_DMA == true>
/* DMA events */
typedef enum {
    DRV_SPI_DMA_NONE = 0,
    
    DRV_SPI_DMA_DATA_INPROGRESS,
    
    DRV_SPI_DMA_DUMMY_INPROGRESS,
    
    DRV_SPI_DMA_COMPLETE,
    
    DRV_SPI_DMA_ERROR,

} DRV_SPI_DMA_STATE;
</#if>

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
typedef struct _DRV_SPI_OBJ
{    
    bool                                        txIntEnabled;
    
    bool                                        rxIntEnabled;
    
    DRV_SPI_BUFFER_EVENT_HANDLER                operationStarting;

    DRV_SPI_BUFFER_EVENT_HANDLER                operationEnded;

    DRV_SPI_SYS_QUEUE_HANDLE                    queue;

    DRV_SPI_JOB_OBJECT *                        currentJob;

    uint8_t                                     symbolsInProgress;
    
<#if CONFIG_DRV_SPI_USE_DMA == true>
    SYS_DMA_CHANNEL_HANDLE                      dmaChannelHandleRead;

    SYS_DMA_CHANNEL_HANDLE                      dmaChannelHandleWrite;
    
    DRV_SPI_DMA_STATE                           txDMAState;
    
    DRV_SPI_DMA_STATE                           rxDMAState;
</#if>
   
<#if CONFIG_USE_3RDPARTY_RTOS ==true>
    OSAL_MUTEX_DECLARE(mutexDriverInstance);
    OSAL_SEM_DECLARE(semaphoreDriverInstance);
</#if>
}DRV_SPI_OBJ;

<#macro DRV_SPI_LOCAL_STATIC_FUNCTIONS DRV_INSTANCE>
/* Local functions declaration for Instance ${DRV_INSTANCE}  */
int32_t _DRV_SPI${DRV_INSTANCE}_SetupHardware(void);
int32_t _DRV_SPI${DRV_INSTANCE}_MasterTasks(void);
int32_t _DRV_SPI${DRV_INSTANCE}_SlaveTasks(void);
<#if .vars["CONFIG_DRV_SPI_USE_DMA_IDX${DRV_INSTANCE}"] == true>
int32_t _DRV_SPI${DRV_INSTANCE}_DMATasks( DRV_SPI_BUFFER_EVENT status );
void    _DRV_SPI${DRV_INSTANCE}_DMA_EventHandler(SYS_DMA_TRANSFER_EVENT event, SYS_DMA_CHANNEL_HANDLE handle, uintptr_t contextHandle);
</#if>
</#macro>


<#if CONFIG_DRV_SPI_IDX0 == true>
    <@DRV_SPI_LOCAL_STATIC_FUNCTIONS DRV_INSTANCE="0"/>
</#if>

<#if CONFIG_DRV_SPI_IDX1 == true>
    <@DRV_SPI_LOCAL_STATIC_FUNCTIONS DRV_INSTANCE="1"/>
</#if>

#endif //_DRV_SPI_STATIC_LOCAL_PIC32C_H_
