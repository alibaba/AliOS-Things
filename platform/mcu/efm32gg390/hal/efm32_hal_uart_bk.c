
#include <stdio.h>

#include <k_api.h>
#include <hal/hal.h>
#include <hal/soc/soc.h>

#include "efm32_hal_uart.h"


#include "BSP_ExtensionPort.h"
#include "BCDS_UARTTransceiver.h"
#include "BCDS_MCU_UART.h"
#include "BCDS_MCU_UART_Handle.h"
//#include "BSP_ExtensionPort.h"

static USART_TypeDef *USART_tab[1] = {
    UART1,
};
extern uart_dev_t uart_0;

#define MAX_UART_RX_BUFFERSIZE   UINT32_C(16)
#define UART_RX_ERROR  UINT32_C(0xFFFF)
#define UART_RX_SUCCESS  UINT32_C(0)
#define END_OF_FRAME             UINT8_C(0xAA)
#define UART_BAURDRATE   UINT32_C(9600)
#define UART_SPI_DATA_TRANSFER_TIMEOUT_MILLISEC UINT32_MAX

static UARTTransceiver_T UartTranscieverInstance;

static uint8_t UartRxBuffInstance[MAX_UART_RX_BUFFERSIZE];
static uint8_t TxBuffer[MAX_UART_RX_BUFFERSIZE];


static void UartAppCallback(struct MCU_UART_Event_S event)
{
}

static bool FrameEndCheckFunc(uint8_t lastByte)
{
    BCDS_UNUSED(lastByte);
    return true;
}

void UartCallBack(UART_T uart, struct MCU_UART_Event_S event)
{
    Retcode_T retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_INVALID_PARAM);
    if (UartTranscieverInstance.handle == uart)
    {
        UARTTransceiver_LoopCallback(&UartTranscieverInstance, event);
    }
    else
    {
        Retcode_RaiseError(retcode);
    }
}

int32_t hal_uart_init(uart_dev_t *uart_dev)
{
    
    if(NULL == uart_dev) return -1;

    BSP_ExtensionPort_Connect();
    //if(uart_dev->port == 0)
    {
        Retcode_T retcode = RETCODE_OK;
        HWHandle_T UartHandle = NULL;
        
        enum UARTTransceiver_UartType_E type;
        uint8_t rxBuffSize;
        retcode = BSP_ExtensionPort_ConnectUart();
        if (RETCODE_OK == retcode)
        {
            retcode = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_PARITY, BSP_EXTENSIONPORT_UART_NO_PARITY, NULL);
        }
        if (RETCODE_OK == retcode)
        {
            retcode = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_BAUDRATE, uart_dev->config.baud_rate, NULL);
        }
        if (RETCODE_OK == retcode)
        {
            retcode = BSP_ExtensionPort_SetUartConfig(BSP_EXTENSIONPORT_UART_STOPBITS, BSP_EXTENSIONPORT_UART_STOPBITS_ONE, NULL);
        }
        if (RETCODE_OK == retcode)
        {
            /*Get the serial uart  handle */
            UartHandle = BSP_ExtensionPort_GetUartHandle();
        }
        if (NULL == UartHandle)
        {
            retcode = RETCODE(RETCODE_SEVERITY_ERROR, RETCODE_NULL_POINTER);
        }
        else
        {
            retcode = MCU_UART_Initialize(UartHandle, UartCallBack);
        }
        if (RETCODE_OK == retcode)
        {
            type = UART_TRANSCEIVER_UART_TYPE_UART;
            rxBuffSize = (MAX_UART_RX_BUFFERSIZE - 1);
            retcode = UARTTransceiver_Initialize(&UartTranscieverInstance, UartHandle, UartRxBuffInstance, rxBuffSize, type);
        }
        if (RETCODE_OK == retcode)
        {
            retcode = BSP_ExtensionPort_EnableUart();
        }
        if (RETCODE_OK == retcode)
        {
            //retcode = UARTTransceiver_StartInAsyncMode(&UartTranscieverInstance, FrameEndCheckFunc, UartAppCallback);
            retcode = UARTTransceiver_Start(&UartTranscieverInstance, FrameEndCheckFunc);
        }
        if (RETCODE_OK != retcode)
        {
            printf("Error in initializing and starting the Uart \r\n");
        }
   }

}
#if 0
int32_t hal_uart_init(uart_dev_t *uart_dev)
{
  /* Setup UART1 in async mode for RS232*/
  USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

  // gpio enable
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Enable clock for USART module */
  CMU_ClockEnable(cmuClock_UART1, true);

  /* Configure GPIO pins */
  GPIO_PinModeSet(gpioPortB, 9, gpioModePushPull, 1); // tx
  GPIO_PinModeSet(gpioPortB, 10, gpioModeInput, 0); // rx

  /* Prepare struct for initializing UART in asynchronous mode*/
  uartInit.enable       = usartDisable;   /* Don't enable UART upon intialization */
  uartInit.refFreq      = 0;              /* Provide information on reference frequency. When set to 0, the reference frequency is */
  uartInit.baudrate     = 115200;         /* Baud rate */
  uartInit.oversampling = usartOVS16;     /* Oversampling. Range is 4x, 6x, 8x or 16x */
  uartInit.databits     = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
  uartInit.parity       = usartNoParity;  /* Parity mode */
  uartInit.stopbits     = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
  uartInit.mvdis        = false;          /* Disable majority voting */
  uartInit.prsRxEnable  = false;          /* Enable USART Rx via Peripheral Reflex System */
  uartInit.prsRxCh      = usartPrsRxCh0;  /* Select PRS channel if enabled */
  /* Initialize USART with uartInit struct */
  USART_InitAsync(UART1, &uartInit);


  /* Prepare UART Rx and Tx interrupts */
  USART_IntClear(UART1, _UART_IF_MASK);
  USART_IntEnable(UART1, UART_IF_RXDATAV);
  NVIC_ClearPendingIRQ(UART1_RX_IRQn);
  //NVIC_ClearPendingIRQ(UART1_TX_IRQn);
  NVIC_EnableIRQ(UART1_RX_IRQn);
  //NVIC_EnableIRQ(UART1_TX_IRQn);
  /* Enable I/O pins at UART1 location #2 */
  UART1->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_ROUTE_LOCATION_LOC2;
  /* Enable UART */
  USART_Enable(UART1, usartEnable);

  return 0;
}
#endif

int32_t hal_uart_send(uart_dev_t *uart_dev,
                      const void *data,
                      uint32_t size,
                      uint32_t timeout)
{
    USART_TypeDef *USARTx;

    if ((uart_dev == NULL) || (data == NULL)) {
        return -1;
    }

    
    Retcode_T retcode = RETCODE_OK;
    retcode = UARTTransceiver_WriteData(&UartTranscieverInstance, data, size, timeout);

    return retcode;
    
    //uint8_t *ch = (uint8_t*)data;
    //int i;
    //struct MCU_UART_Handle_S * tmp = BSP_ExtensionPort_GetUartHandle();
    //for (i = 0; i < size; i++) {
    //    USART_Tx(tmp->Uart_ptr, ch[i]);
    //}

    //return i;
}

//uint8_t data[100]={0};
int32_t hal_uart_recv_II(uart_dev_t *uart_dev, void *data, uint32_t expect_size,
        uint32_t *recv_size, uint32_t timeout)
{
    if ((uart_dev == NULL) || (data == NULL)) {
        return -1;
    }
    Retcode_T retcode = RETCODE_OK;
    uint32_t len =0;
    retcode = UARTTransceiver_ReadData(&UartTranscieverInstance, data, expect_size, &len, timeout);
    if(recv_size != NULL) *recv_size = len;

    return retcode;
}
