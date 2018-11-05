/*******************************************************************************
  WILC1000 Wireless Driver SPI Communication Support

  File Name:
    wdrv_wilc1000_spi.c

  Summary:
    Support SPI communications to the WILC1000 module.

  Description:
    Support SPI communications to the WILC1000 module.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#include "wdrv_wilc1000_api.h"

#if defined(__PIC32MZ__)
#define WDRV_DCACHE_CLEAN(addr, size) __DataCacheClean(addr, size)
#else /* !defined(__PIC32MZ__) */
#define WDRV_DCACHE_CLEAN(addr, size) do { } while (0)
#endif /* defined(__PIC32MZ__) */

#if defined(DRV_SPI_DMA)
#define SPI_DMA_DCACHE_CLEAN(addr, size) WDRV_DCACHE_CLEAN(addr, size)
#define SPI_TRANSACTION_COMPLETE_NOTIFY(sem) WDRV_SEM_GIVE_FROM_ISR(sem)
#define SPI_WAIT_FOR_TX_COMPLETION() WDRV_SEM_TAKE(&s_dmaTxSync, OSAL_WAIT_FOREVER)
#define SPI_WAIT_FOR_RX_COMPLETION() WDRV_SEM_TAKE(&s_dmaRxSync, OSAL_WAIT_FOREVER)
#define SPI_DMA_MAX_TX_SIZE DRV_SPI_DMA_TXFER_SIZE
#define SPI_DMA_MAX_RX_SIZE DRV_SPI_DMA_DUMMY_BUFFER_SIZE
#else /* defined(WDRV_USE_SPI_DMA) */
#define SPI_DMA_DCACHE_CLEAN(addr, size) do { } while (0)
#define SPI_TRANSACTION_COMPLETE_NOTIFY(sem) do { } while (0)
#define SPI_WAIT_FOR_TX_COMPLETION() _Spi_WaitForCompletion(&s_Spi_Tx_Done)
#define SPI_WAIT_FOR_RX_COMPLETION() _Spi_WaitForCompletion(&s_Spi_Rx_Done)
#define SPI_DMA_MAX_TX_SIZE -1 /* Unused. Just to avoid compilation error */
#define SPI_DMA_MAX_RX_SIZE -1 /* Unused. Just to avoid compilation error */
#endif /* defined(WDRV_USE_SPI_DMA) */

#if !defined(DRV_SPI_DMA)
static SYS_MODULE_OBJ *p_drvSPIObject = &WDRV_SPI_INSTANCE;
#endif
static DRV_HANDLE s_SpiHandle = -1;
static DRV_SPI_BUFFER_HANDLE s_SpiBufferHandleTx;
static DRV_SPI_BUFFER_HANDLE s_SpiBufferHandleRx;
static volatile bool s_Spi_Tx_Done = true;
static volatile bool s_Spi_Rx_Done = true;
OSAL_SEM_HANDLE_TYPE s_dmaTxSync;
OSAL_SEM_HANDLE_TYPE s_dmaRxSync;

#if defined(__PIC32MZ__)
static void __DataCacheClean(unsigned char *address, uint32_t size)
{
    if (IS_KVA0(address)) {
        uint32_t a = (uint32_t)address & 0xfffffff0;
        uint32_t r = (uint32_t)address & 0x0000000f;
        uint32_t s = ((size + r + 15) >> 4) << 4;

        SYS_DEVCON_DataCacheClean(a, s);
    }
}
#endif /* defined(__PIC32MZ__) */

static void CS_Init(void)
{
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_OUTPUT, WDRV_SPI_SSN_PORT_CHANNEL, WDRV_SPI_SSN_BIT_POS);
}

static void CS_Assert()
{
    SYS_PORTS_PinClear(PORTS_ID_0, WDRV_SPI_SSN_PORT_CHANNEL, WDRV_SPI_SSN_BIT_POS);
}

static void CS_Deassert()
{
    SYS_PORTS_PinSet(PORTS_ID_0, WDRV_SPI_SSN_PORT_CHANNEL, WDRV_SPI_SSN_BIT_POS);
}

static bool Spi_Init(void)
{
    if (s_SpiHandle == -1) {
        s_SpiHandle = DRV_SPI_Open(WDRV_SPI_INDEX, DRV_IO_INTENT_READWRITE|DRV_IO_INTENT_BLOCKING);
        if (s_SpiHandle == (DRV_SPI_BUFFER_HANDLE)-1)
        {
            WDRV_ASSERT(false, "SPI init failed");
            return false;
        }
    }

    WDRV_SEM_INIT(&s_dmaTxSync);
    WDRV_SEM_INIT(&s_dmaRxSync);

    return true;
}

static void Spi_Deinit(void)
{
    WDRV_SEM_DEINIT(&s_dmaTxSync);
    WDRV_SEM_DEINIT(&s_dmaRxSync);
    //DRV_SPI_Close(s_SpiHandle);
}

static void SPI_TxComplete(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void *context )
{
    s_Spi_Tx_Done = true;
    SPI_TRANSACTION_COMPLETE_NOTIFY(&s_dmaTxSync);
}

static void SPI_RxComplete(DRV_SPI_BUFFER_EVENT event, DRV_SPI_BUFFER_HANDLE bufferHandle, void *context )
{
    s_Spi_Rx_Done = true;
    SPI_TRANSACTION_COMPLETE_NOTIFY(&s_dmaRxSync);
}

#if !defined(DRV_SPI_DMA)
static void _Spi_WaitForCompletion(volatile bool *spiDone)
{
    while (*spiDone == false)
        DRV_SPI_Tasks(*p_drvSPIObject);
}
#endif

static bool _Spi_Tx(unsigned char *buf, uint32_t size)
{
    bool ret = true;

    SPI_DMA_DCACHE_CLEAN(buf, size);
    s_Spi_Tx_Done = false;
    s_SpiBufferHandleTx = DRV_SPI_BufferAddWrite(s_SpiHandle, buf, size, SPI_TxComplete, 0);
    if (s_SpiBufferHandleTx == (DRV_SPI_BUFFER_HANDLE)-1)
        ret = false;

    SPI_WAIT_FOR_TX_COMPLETION();
    return ret;
}

static bool _Spi_Rx(unsigned char *const buf, uint32_t size)
{
    bool ret = true;

    SPI_DMA_DCACHE_CLEAN(buf, size);
    s_Spi_Rx_Done = false;
    s_SpiBufferHandleRx = DRV_SPI_BufferAddRead(s_SpiHandle, buf, size, SPI_RxComplete, 0);
    if (s_SpiBufferHandleRx == (DRV_SPI_BUFFER_HANDLE)-1)
        ret = false;

    SPI_WAIT_FOR_RX_COMPLETION();
    return ret;
}

bool WDRV_STUB_SPI_Out(unsigned char *const buf, uint32_t size)
{
    bool ret = true;
    int c = 0;

    CS_Assert();

    while (size > SPI_DMA_MAX_TX_SIZE) {
        ret = _Spi_Tx((buf + c * SPI_DMA_MAX_TX_SIZE), SPI_DMA_MAX_TX_SIZE);
        size -= SPI_DMA_MAX_TX_SIZE;
        ++c;
    }

    if (size > 0)
        ret = _Spi_Tx((buf + c * SPI_DMA_MAX_TX_SIZE), size);

    CS_Deassert();

    return ret;
}

bool WDRV_STUB_SPI_In(unsigned char *const buf, uint32_t size)
{
    bool ret = true;
    int c = 0;

    CS_Assert();

    while (size > SPI_DMA_MAX_RX_SIZE) {
        ret = _Spi_Rx(buf + c * SPI_DMA_MAX_RX_SIZE, SPI_DMA_MAX_RX_SIZE);
        size -= SPI_DMA_MAX_RX_SIZE;
        ++c;
    }

    if (size > 0)
        ret = _Spi_Rx(buf + c * SPI_DMA_MAX_RX_SIZE, size);

    CS_Deassert();

    return ret;
}

void WDRV_STUB_SPI_Initialize(void)
{
    CS_Init();
    CS_Deassert();
    Spi_Init();
}

void WDRV_STUB_SPI_Deinitialize(void)
{
    Spi_Deinit();
}

//DOM-IGNORE-END
