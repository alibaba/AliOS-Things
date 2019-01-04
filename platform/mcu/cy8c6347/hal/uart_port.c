#include "hal/soc/soc.h"
#include "scb/cy_scb_common.h"
#include "scb/cy_scb_uart.h"
#include "gpio/cy_gpio.h"
#include "sysclk/cy_sysclk.h"
#include "sysint/cy_sysint.h"
#include "board.h"
#include "atcmd_config_platform.h"
#include "uart_port.h"

#include <stdio.h>
#include <k_api.h>
#include <aos/kernel.h>

uart_dev_t uart_0;

/* Allocate context for Stdio UART operation */
cy_stc_scb_uart_context_t stdio_uartContext;

/* Allocate context for AT UART operation */
cy_stc_scb_uart_context_t at_uartContext;

UART_MAPPING UART_MAPPING_TABLE[] =
{
    { PORT_UART_STD,     UART5},
    { PORT_UART_AT,      UART1},
};

static uart_dev_t console_uart={
  .port=PORT_UART_STD,
};

int default_UART_Init(void)
{
    uart_0.port                = PORT_UART_STD;
    uart_0.config.baud_rate    = STDIO_UART_BAUDRATE;
    uart_0.config.data_width   = DATA_WIDTH_8BIT;
    uart_0.config.parity       = NO_PARITY;
    uart_0.config.stop_bits    = STOP_BITS_1;
    uart_0.config.flow_control = FLOW_CONTROL_DISABLED;

    return hal_uart_init(&uart_0);
}

/* Populate configuration structure */
const cy_stc_scb_uart_config_t stdio_uartConfig =
{
    .uartMode                   = CY_SCB_UART_STANDARD,
    .enableMutliProcessorMode   = false,
    .smartCardRetryOnNack       = false,
    .irdaInvertRx               = false,
    .irdaEnableLowPowerReceiver = false,
    .oversample                 = 12UL,
    .enableMsbFirst             = false,
    .dataWidth                  = 8UL,
    .parity                     = CY_SCB_UART_PARITY_NONE,
    .stopBits                   = CY_SCB_UART_STOP_BITS_1,
    .enableInputFilter          = false,
    .breakWidth                 = 11UL,
    .dropOnFrameError           = false,
    .dropOnParityError          = false,
    .receiverAddress            = 0UL,
    .receiverAddressMask        = 0UL,
    .acceptAddrInFifo           = false,
    .enableCts                  = false,
    .ctsPolarity                = CY_SCB_UART_ACTIVE_LOW,
    .rtsRxFifoLevel             = 0UL,
    .rtsPolarity                = CY_SCB_UART_ACTIVE_LOW,
    .rxFifoTriggerLevel  = 0UL,
    .rxFifoIntEnableMask = 0UL,
    .txFifoTriggerLevel  = 0UL,
    .txFifoIntEnableMask = 0UL,
};

/* Populate configuration structure */
const cy_stc_scb_uart_config_t at_uartConfig =
{
    .uartMode                   = CY_SCB_UART_STANDARD,
    .enableMutliProcessorMode   = false,
    .smartCardRetryOnNack       = false,
    .irdaInvertRx               = false,
    .irdaEnableLowPowerReceiver = false,
    .oversample                 = 12UL,
    .enableMsbFirst             = false,
    .dataWidth                  = 8UL,
    .parity                     = CY_SCB_UART_PARITY_NONE,
    .stopBits                   = CY_SCB_UART_STOP_BITS_1,
    .enableInputFilter          = false,
    .breakWidth                 = 11UL,
    .dropOnFrameError           = false,
    .dropOnParityError          = false,
    .receiverAddress            = 0UL,
    .receiverAddressMask        = 0UL,
    .acceptAddrInFifo           = false,
    .enableCts                  = false,
    .ctsPolarity                = CY_SCB_UART_ACTIVE_LOW,
    .rtsRxFifoLevel             = 0UL,
    .rtsPolarity                = CY_SCB_UART_ACTIVE_LOW,
    .rxFifoTriggerLevel  = 0UL,
    .rxFifoIntEnableMask = 0UL,
    .txFifoTriggerLevel  = 0UL,
    .txFifoIntEnableMask = 0UL,
};

#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)

PUTCHAR_PROTOTYPE;
GETCHAR_PROTOTYPE;
    
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
    /* Add an explicit reference to the floating point printf library to allow
    the usage of floating point conversion specifier. */
    __asm (".global _printf_float");
    /***************************************************************************
    * Function Name: _write
    ***************************************************************************/
    __attribute__((weak)) int _write (int fd, const char *ptr, int len)
    {
        int nChars = 0;
        (void)fd;
        if (ptr != NULL)
        {
            for (/* Empty */; nChars < len; ++nChars)
            {
                if(*ptr == '\n')
                {
                    __io_putchar('\r');
                }
                __io_putchar((uint32_t)*ptr);
                ++ptr;
            }
        }
        return (nChars);
    }
    
    __asm (".global _scanf_float");
    __attribute__((weak)) int _read (int fd, char *ptr, int len)
    {
        int nChars = 0;
        (void)fd;
        if (ptr != NULL)
        {
            for(/* Empty */;nChars < len;++ptr)
            {
                *ptr = (char)__io_getchar();
                ++nChars;
                if(*ptr == '\n')
                {
                    break;
                }
            }
        }
        return (nChars);
    }    
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
    if (ch == '\n') {
        hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
    }
    hal_uart_send(&console_uart, &ch, 1, 30000);
    
    return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
    /* Place your implementation of fgetc here */
    /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
    uint8_t ch = 0;
    uint32_t recv_size;
    int32_t ret = 0;

    ret = hal_uart_recv_II(&console_uart, &ch, 1, &recv_size, HAL_WAIT_FOREVER);
    if(ret == 0)
    {
        return ch;
    }
    else
    {
        return -1;
    }
}

/* AT and Stdio UART Semaphore for communication with UART Callback */
aos_sem_t at_uart_tx_sema;
aos_sem_t at_uart_rx_sema;
aos_sem_t stdio_uart_tx_sema;
aos_sem_t stdio_uart_rx_sema;

//for UART driver lock, if not added, kernel test will fail
aos_mutex_t at_uart_tx_mutex;
aos_mutex_t at_uart_rx_mutex;
aos_mutex_t stdio_uart_tx_mutex;
aos_mutex_t stdio_uart_rx_mutex;

//Get UART Instanse & attribute from Logical Port
static UART_MAPPING* GetUARTMapping(const PORT_UART_TYPE port)
{
    int8_t i = 0;
    UART_MAPPING* rc = NULL;
    for(i=0; i<PORT_UART_SIZE; i++)
    {
        if(UART_MAPPING_TABLE[i].uartFuncP == port)
        {
            rc = &UART_MAPPING_TABLE[i];
            break;
        }
    }
    return rc;
}

void AT_UART_ISR(void)
{
    CySCB_Type *scb_base;
    scb_base = (CySCB_Type*)(SCB0_BASE + AT_UART*SCB_SECTION_SIZE);

    Cy_SCB_UART_Interrupt(scb_base, &at_uartContext);
}

void STDIO_UART_ISR(void)
{
    CySCB_Type *scb_base;
    scb_base = (CySCB_Type*)(SCB0_BASE + STDIO_UART*SCB_SECTION_SIZE);
    
    Cy_SCB_UART_Interrupt(scb_base, &stdio_uartContext);
}

void at_uart_callback(uint32_t event)
{
    switch(event)
    {
        case CY_SCB_UART_TRANSMIT_DONE_EVENT:	
        aos_sem_signal(&at_uart_tx_sema);
        break;
				
        case CY_SCB_UART_RECEIVE_DONE_EVENT:
        aos_sem_signal(&at_uart_rx_sema);
        break;
				
        case CY_SCB_UART_TRANSMIT_IN_FIFO_EVENT:
        break;
				
        case CY_SCB_UART_RECEIVE_ERR_EVENT:
        break;
	      
        case CY_SCB_UART_TRANSMIT_ERR_EVENT:
        break;	

        case CY_SCB_UART_RB_FULL_EVENT:
        break;
				
        default:
        break;
    }				

}

void stdio_uart_callback(uint32_t event)
{
    switch(event)
    {
        case CY_SCB_UART_TRANSMIT_DONE_EVENT:	
        aos_sem_signal(&stdio_uart_tx_sema);
        break;
				
        case CY_SCB_UART_RECEIVE_DONE_EVENT:
        aos_sem_signal(&stdio_uart_rx_sema);
        break;
				
        case CY_SCB_UART_TRANSMIT_IN_FIFO_EVENT:
        break;
				
        case CY_SCB_UART_RECEIVE_ERR_EVENT:
        break;
	      
        case CY_SCB_UART_TRANSMIT_ERR_EVENT:
        break;	

        case CY_SCB_UART_RB_FULL_EVENT:
        break;
				
        default:
        break;
    }				

}

int32_t hal_uart_init(uart_dev_t *uart)
{
    CySCB_Type *scb_base;
    uint32_t divider_val;
	UART_MAPPING* uartIns = GetUARTMapping(uart->port); 
    scb_base = (CySCB_Type*)(SCB0_BASE + (uartIns->uartPhyP)*SCB_SECTION_SIZE);
	
    switch(uartIns->uartPhyP)
    {        
        case AT_UART:
        /* Connect WIFI UART function to pins */
        Cy_GPIO_SetHSIOM(AT_UART_IO_PORT, AT_UART_RX_NUM, AT_UART_RX_FUNC);
        Cy_GPIO_SetHSIOM(AT_UART_IO_PORT, AT_UART_TX_NUM, AT_UART_TX_FUNC);
        
        /* Configure pins for UART operation */
        Cy_GPIO_SetDrivemode(AT_UART_IO_PORT, AT_UART_RX_NUM, CY_GPIO_DM_HIGHZ);
        Cy_GPIO_SetDrivemode(AT_UART_IO_PORT, AT_UART_TX_NUM, CY_GPIO_DM_STRONG_IN_OFF);

        divider_val = cy_PeriClkFreqHz/((uart->config.baud_rate)*at_uartConfig.oversample);
    
        Cy_SysClk_PeriphAssignDivider(AT_UART_CLOCK, CY_SYSCLK_DIV_8_BIT, 0u);
	    Cy_SysClk_PeriphSetDivider(CY_SYSCLK_DIV_8_BIT, 0u, divider_val-1);
	    Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_8_BIT, 0u);
    
        /* Configure UART to operate */
        Cy_SCB_UART_Init(scb_base, &at_uartConfig, &at_uartContext);
        
        /* Populate configuration structure (code specific for CM4) */
        cy_stc_sysint_t at_uartIntrConfig =
        {
            .intrSrc      = AT_UART_INTR_NUM,
            .intrPriority = AT_UART_INTR_PRIORITY,
        };
        
        /* Hook interrupt service routine and enable interrupt */
        Cy_SysInt_Init(&at_uartIntrConfig, &AT_UART_ISR);
        NVIC_EnableIRQ(AT_UART_INTR_NUM);
        
        /* Enable UART to operate */
        Cy_SCB_UART_Enable(scb_base);
        Cy_SCB_UART_RegisterCallback(scb_base, at_uart_callback, &at_uartContext);
        
        /* Semaphore and mutex init */
        aos_sem_new(&at_uart_tx_sema,0);
        aos_sem_new(&at_uart_rx_sema,0);
        aos_mutex_new(&at_uart_tx_mutex);
        aos_mutex_new(&at_uart_rx_mutex);
        break;
        
        case STDIO_UART:
        Cy_GPIO_SetHSIOM(STDIO_UART_IO_PORT, STDIO_UART_RX_NUM, STDIO_UART_RX_FUNC);
        Cy_GPIO_SetHSIOM(STDIO_UART_IO_PORT, STDIO_UART_TX_NUM, STDIO_UART_TX_FUNC);
        
        /* Configure pins for UART operation */
        Cy_GPIO_SetDrivemode(STDIO_UART_IO_PORT, STDIO_UART_RX_NUM, CY_GPIO_DM_HIGHZ);
        Cy_GPIO_SetDrivemode(STDIO_UART_IO_PORT, STDIO_UART_TX_NUM, CY_GPIO_DM_STRONG_IN_OFF);
        
        divider_val = cy_PeriClkFreqHz/((uart->config.baud_rate)*stdio_uartConfig.oversample);
    
        Cy_SysClk_PeriphAssignDivider(STDIO_UART_CLOCK, CY_SYSCLK_DIV_8_BIT, 0u);
	    Cy_SysClk_PeriphSetDivider(CY_SYSCLK_DIV_8_BIT, 0u, divider_val-1);
	    Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_8_BIT, 0u);
        
        /* Configure UART to operate */
        Cy_SCB_UART_Init(scb_base, &stdio_uartConfig, &stdio_uartContext);
        
        /* Populate configuration structure (code specific for CM4) */
        cy_stc_sysint_t stdio_uartIntrConfig =
        {
            .intrSrc      = STDIO_UART_INTR_NUM,
            .intrPriority = STDIO_UART_INTR_PRIORITY,
        };
        
        /* Hook interrupt service routine and enable interrupt */
        Cy_SysInt_Init(&stdio_uartIntrConfig, &STDIO_UART_ISR);
        NVIC_EnableIRQ(STDIO_UART_INTR_NUM);
        
        /* Enable UART to operate */
        Cy_SCB_UART_Enable(scb_base);        
        Cy_SCB_UART_RegisterCallback(scb_base, stdio_uart_callback, &stdio_uartContext);

        /* Semaphore and mutex init */
        aos_sem_new(&stdio_uart_tx_sema,0);
        aos_sem_new(&stdio_uart_rx_sema,0);
        aos_mutex_new(&stdio_uart_tx_mutex);
        aos_mutex_new(&stdio_uart_rx_mutex);
        break;
        
        default:
        break;
    }
    
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    CySCB_Type *scb_base;
    UART_MAPPING* uartIns = GetUARTMapping(uart->port);  
    scb_base = (CySCB_Type*)(SCB0_BASE + (uartIns->uartPhyP)*SCB_SECTION_SIZE);
            
    if(uart == NULL || data == NULL)
    {
        return -1;
    }

    switch(uartIns->uartPhyP)
    {
        case AT_UART:
        aos_mutex_lock(&at_uart_tx_mutex, AOS_WAIT_FOREVER);
        Cy_SCB_UART_Transmit(scb_base, (void *)data, size, &at_uartContext);
        aos_sem_wait(&at_uart_tx_sema, AOS_WAIT_FOREVER);
        aos_mutex_unlock(&at_uart_tx_mutex);
        break;
        
        case STDIO_UART:
        aos_mutex_lock(&stdio_uart_tx_mutex, AOS_WAIT_FOREVER);
        Cy_SCB_UART_Transmit(scb_base, (void *)data, size, &stdio_uartContext);
        aos_sem_wait(&stdio_uart_tx_sema, AOS_WAIT_FOREVER);
        aos_mutex_unlock(&stdio_uart_tx_mutex);
        break;
        
        default:
        break;
    }

    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                      uint32_t *recv_size, uint32_t timeout)
{
    uint8_t *pdata = (uint8_t *)(data);
    int i = 0;
    uint32_t rx_count = 0;
    int32_t ret = -1;
    CySCB_Type *scb_base;
    UART_MAPPING* uartIns = GetUARTMapping(uart->port); 
    scb_base = (CySCB_Type*)(SCB0_BASE + (uartIns->uartPhyP)*SCB_SECTION_SIZE);
        
    if ((uart == NULL) || (data == NULL)) {
        return -1;
    }
    
    switch(uartIns->uartPhyP)
    {
    case AT_UART:
        aos_mutex_lock(&at_uart_rx_mutex, AOS_WAIT_FOREVER);
        Cy_SCB_UART_Receive(scb_base, pdata, expect_size, &at_uartContext);
        aos_sem_wait(&at_uart_rx_sema, timeout);
        rx_count = Cy_SCB_UART_GetNumReceived(scb_base, &at_uartContext);
        aos_mutex_unlock(&at_uart_rx_mutex);
        break;

    case STDIO_UART:
        aos_mutex_lock(&stdio_uart_rx_mutex, AOS_WAIT_FOREVER);
        Cy_SCB_UART_Receive(scb_base, pdata, expect_size, &stdio_uartContext);
        aos_sem_wait(&stdio_uart_rx_sema, timeout);
        rx_count = Cy_SCB_UART_GetNumReceived(scb_base, &stdio_uartContext);
        aos_mutex_unlock(&stdio_uart_rx_mutex);
        break;

    default:
        break;
    }

    if(recv_size != NULL)
    {
        *recv_size = rx_count;
    }

    if(rx_count != 0)
    {
        ret = 0;
    }
    else
    {
        ret = -1;
    }

    return ret;
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    CySCB_Type *scb_base;
    UART_MAPPING* uartIns = GetUARTMapping(uart->port);  
    scb_base = (CySCB_Type*)(SCB0_BASE + (uartIns->uartPhyP)*SCB_SECTION_SIZE);
          
    switch(uartIns->uartPhyP)
    {
        case AT_UART:
        Cy_SCB_UART_DeInit(scb_base);
        break;
        
        case STDIO_UART:
        Cy_SCB_UART_DeInit(scb_base);
        break;
        
        default:
        break;
    }
    
    return 0;
}

