
#include "ameba_soc.h"
#include "k_api.h"

#include "board.h"


#include "device.h"
#include "gpio_api.h"
#include "aos/hal/uart.h"
#include "device.h"
#include "serial_api.h"
#include "gpio_api.h"
#include "diag.h"



#define RX_BUFFER_LENGTH    128


typedef struct{
	u8 id;
	PinName tx;
	PinName rx;
	PinName rts;
	PinName cts;
}uart_s;

typedef struct{
	u32 length;
	u8 *rxbuffer;
}uart_buffer;

static uart_buffer uart_rxbuffer[5];



/*the pin used for uart, you can change the pin as pinmux table, 0 means not use*/
static uart_s uart_obj[5]={
	{2,	PA_7,	PA_8,	0,	0},		//log uart
	{0,	PA_18,	PA_19,	PA_17,	PA_18},		//km4 uart
	{1,	PA_2,	PA_4,	PA_0,	PB_31},		//bt uart
	{3,	PA_12,	PA_13,	PA_14,	PA_15},		//km0 uart
	{3,	PB_1,	PB_2,	0,	0}
};


#define UART_TX   PB_1
#define UART_RX   PB_2

// serial_t uart_obj_t;

#define EVENT_RX_READY  ((uint32_t)1 << 0)

#if (RHINO_CONFIG_CPU_NUM > 1)
static aos_spinlock_t uart0_lock = { KRHINO_SPINLOCK_FREE_VAL, };
#else
static aos_spinlock_t uart0_lock;
#endif
static aos_event_t uart0_event;
static uint8_t uart0_rx_buf[RX_BUFFER_LENGTH];
static size_t uart0_rx_buf_head = 0;
static size_t uart0_rx_buf_tail = 0;

#define uart0_rx_buf_count() \
    ((RX_BUFFER_LENGTH + uart0_rx_buf_head - uart0_rx_buf_tail) & \
     (RX_BUFFER_LENGTH - 1))
#define uart0_rx_buf_space() \
    (RX_BUFFER_LENGTH - 1 - uart0_rx_buf_count())

size_t uart0_rx_buffer_produce(const void *buf, size_t count)
{
    size_t c = 0;
    size_t old_count;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&uart0_lock);
    old_count = uart0_rx_buf_count();

    while (c < count && uart0_rx_buf_space() > 0) {
        uart0_rx_buf[uart0_rx_buf_head++] = ((const uint8_t *)buf)[c++];
        uart0_rx_buf_head &= RX_BUFFER_LENGTH - 1;
    }

    if (old_count == 0 && c > 0)
        aos_event_set(&uart0_event, EVENT_RX_READY, AOS_EVENT_OR);

    aos_spin_unlock_irqrestore(&uart0_lock, flags);

    return c;
}

static size_t uart0_rx_buffer_consume(void *buf, size_t count)
{
    size_t c = 0;
    aos_irqsave_t flags;

    flags = aos_spin_lock_irqsave(&uart0_lock);

    while (c < count && uart0_rx_buf_count() > 0) {
        ((uint8_t *)buf)[c++] = uart0_rx_buf[uart0_rx_buf_tail++];
        uart0_rx_buf_tail &= RX_BUFFER_LENGTH - 1;
    }

    if (uart0_rx_buf_count() == 0)
        aos_event_set(&uart0_event, ~EVENT_RX_READY, AOS_EVENT_AND);

    aos_spin_unlock_irqrestore(&uart0_lock, flags);

    return c;
}

int g_uart_init = 0;



static int32_t uart_irq(void *data)
{
    volatile u8 reg_iir;
    u8 int_id;
    u32 reg_val;
    uint8_t rx_char;
    UART_TypeDef * uart_dev;

    uart_dev_t* uart = (uart_dev_t*) data;
    u8 num = uart_obj[uart->port].id;
    uart_dev = UART_DEV_TABLE[num].UARTx;



    reg_iir = UART_IntStatus(uart_dev);
    if ((reg_iir & RUART_IIR_INT_PEND) != 0)
    {
        /* No pending IRQ */
        return 0;
    }

    int_id = (reg_iir & RUART_IIR_INT_ID) >> 1;

    switch (int_id)
    {
    case RUART_RECEIVER_DATA_AVAILABLE:
    case RUART_TIME_OUT_INDICATION:
        {
            if (UART_Readable(uart_dev))
            {
                if(uart_rxbuffer[uart->port].length<RX_BUFFER_LENGTH)
                {
                    UART_CharGet(uart_dev,  (uart_rxbuffer[uart->port].rxbuffer + uart_rxbuffer[uart->port].length));
		     uart_rxbuffer[uart->port].length++;
                }
		 else
		 {
		 	DBG_8195A("UART BUFFER SIZE OVERFLOW!!!!!!!\n");
		 }
            }
        }
        break;

    case RUART_RECEIVE_LINE_STATUS:
        reg_val = (UART_LineStatusGet(uart_dev));
        DBG_8195A("data_uart_irq: LSR %08x interrupt\n", reg_val);
        break;

    default:
        DBG_8195A("data_uart_irq: Unknown interrupt type %d\n", int_id);
        break;
    }

    return 0;
}

/**
 * Initialises a UART interface
 *
 *
 * @param[in]  uart  the interface which should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_init(uart_dev_t *uart)
{
    if(uart->port != 0){
	 u8 num = uart_obj[uart->port].id;
	UART_InitTypeDef  UARTStruct;
	UART_TypeDef *uart_dev;
	uart_dev = UART_DEV_TABLE[num].UARTx;

	uart_rxbuffer[uart->port].length=0;

	uart_rxbuffer[uart->port].rxbuffer = aos_malloc(RX_BUFFER_LENGTH);

	/*set pin*/
	if(uart_obj[uart->port].tx){
		Pinmux_Config(uart_obj[uart->port].tx, PINMUX_FUNCTION_UART);
		PAD_PullCtrl(uart_obj[uart->port].tx, GPIO_PuPd_UP);
	}
	if(uart_obj[uart->port].rx){
		Pinmux_Config(uart_obj[uart->port].rx, PINMUX_FUNCTION_UART);
		PAD_PullCtrl(uart_obj[uart->port].rx, GPIO_PuPd_UP);
	}
	if(uart->config.flow_control == FLOW_CONTROL_CTS){
		if(uart_obj[uart->port].cts){
			Pinmux_Config(uart_obj[uart->port].cts, PINMUX_FUNCTION_UART_RTSCTS);
		}
	}else if(uart->config.flow_control == FLOW_CONTROL_RTS){
		if(uart_obj[uart->port].rts){
			Pinmux_Config(uart_obj[uart->port].rts, PINMUX_FUNCTION_UART_RTSCTS);
		}
	}else if(uart->config.flow_control == FLOW_CONTROL_CTS_RTS){
		if(uart_obj[uart->port].cts){
			Pinmux_Config(uart_obj[uart->port].cts, PINMUX_FUNCTION_UART_RTSCTS);
		}
		if(uart_obj[uart->port].rts){
			Pinmux_Config(uart_obj[uart->port].rts, PINMUX_FUNCTION_UART_RTSCTS);
		}
	}

	UART_StructInit(&UARTStruct);
	if(uart->config.data_width == DATA_WIDTH_7BIT)
		UARTStruct.WordLen = RUART_WLS_7BITS;
	else if(uart->config.data_width == DATA_WIDTH_8BIT)
		UARTStruct.WordLen = RUART_WLS_8BITS;
	else
		DBG_8195A("UNSUPPORTED DATA LENGTH\n");

	if(uart->config.stop_bits == STOP_BITS_1)
		UARTStruct.StopBit = RUART_STOP_BIT_1;
	else if(uart->config.stop_bits == STOP_BITS_2)
		UARTStruct.StopBit = RUART_STOP_BIT_2;
	else
		DBG_8195A("UNSUPPORTED  STOP BIT\n");

	if(uart->config.parity ==NO_PARITY){
		UARTStruct.Parity = RUART_PARITY_DISABLE;
	}else if(uart->config.parity == ODD_PARITY){
		UARTStruct.Parity = RUART_PARITY_ENABLE;
		UARTStruct.ParityType = RUART_ODD_PARITY;
	}else if(uart->config.parity == EVEN_PARITY){
		UARTStruct.Parity = RUART_PARITY_ENABLE;
		UARTStruct.ParityType = RUART_EVEN_PARITY;
	}else{
		DBG_8195A("UNSUPPORTED PARITY TYPE\n");
	}

	if(uart->config.flow_control == FLOW_CONTROL_DISABLED){
		UARTStruct.FlowControl =  DISABLE;
	}else{
		UARTStruct.FlowControl =  ENABLE;
	}

	UART_Init(uart_dev, &UARTStruct);
	UART_SetBaud(uart_dev, uart->config.baud_rate);

	InterruptRegister((IRQ_FUN)uart_irq, UART_DEV_TABLE[num].IrqNum, (uint32_t)uart, 5);
	InterruptEn( UART_DEV_TABLE[num].IrqNum, 5);

	UART_INTConfig(uart_dev, RUART_IER_ERBI | RUART_IER_ETOI | RUART_IER_ELSI, ENABLE);

	UART_RxCmd(uart_dev, ENABLE);

	return 0;
    }else{
  /*
    serial_init(&uart_obj_t, UART_TX, UART_RX);
    serial_baud(&uart_obj_t, 115200);
    serial_format(&uart_obj_t, 8, ParityNone, 1);
    serial_irq_handler(&uart_obj_t, uart_irq, (uint32_t)&uart_obj_t);

    serial_irq_set(&uart_obj_t, RxIrq, 1);*/

    g_uart_init = 1;
    aos_event_new(&uart0_event, 0);

#ifdef AMEBAD_TODO
    LOGUART_SetBaud_FromFlash();
#endif

    shell_init_rom(0, 0);

    shell_init_ram();
    ipc_table_init();

    /* Register Log Uart Callback function */
    extern VOID shell_uart_irq_rom(VOID * Data);
    InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, 10);
    InterruptEn(UART_LOG_IRQ_LP,10);

    return 0;

    }
}

/**
 * Transmit data on a UART interface
 *
 * @param[in]  uart     the UART interface
 * @param[in]  data     pointer to the start of data
 * @param[in]  size     number of bytes to transmit
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
   if(uart->port != 0){
	 u8 num = uart_obj[uart->port].id;
	UART_TypeDef *uart_dev;
	uart_dev = UART_DEV_TABLE[num].UARTx;

	UART_SendDataTO(uart_dev, data, size, 1000);

	return 0;
    }else{
        int i;
        char * cdata = (char *)data;

        for(i = 0; i < size; i++)
        {
            DiagPutChar(*cdata++);
        }

        return 0;
    }
}


/**
 * Receive data on a UART interface
 *
 * @param[in]   uart         the UART interface
 * @param[out]  data         pointer to the buffer which will store incoming data
 * @param[in]   expect_size  number of bytes to receive
 * @param[out]  recv_size    number of bytes received
 * @param[in]   timeout      timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                           if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{
     if(uart->port != 0){
	u8 num = uart_obj[uart->port].id;
	UART_TypeDef *uart_dev;
	uart_dev = UART_DEV_TABLE[num].UARTx;

	if((uart_rxbuffer[uart->port].length) <= expect_size){
		_memcpy(data, uart_rxbuffer[uart->port].rxbuffer, uart_rxbuffer[uart->port].length);
		*recv_size = uart_rxbuffer[uart->port].length;
		uart_rxbuffer[uart->port].length = 0;
	}else{
		_memcpy(data, uart_rxbuffer[uart->port].rxbuffer, expect_size);
		_memcpy(uart_rxbuffer[uart->port].rxbuffer,uart_rxbuffer[uart->port].rxbuffer+expect_size, uart_rxbuffer[uart->port].length-expect_size);
		*recv_size = expect_size;
		uart_rxbuffer[uart->port].length -= expect_size;
	}
	return 0;
    }else{
        uint32_t c = 0;

        if (data == NULL || expect_size == 0) {
            return -1;
        }

        while (1) {
            aos_status_t r;
            uint32_t val;

            c += uart0_rx_buffer_consume(&((uint8_t *)data)[c], expect_size - c);
            if (c > 0)
                break;

            if (timeout == 0)
                break;

            r = aos_event_get(&uart0_event, EVENT_RX_READY, AOS_EVENT_OR,
                              &val, timeout);
            if (r)
                break;
        }

        if (recv_size) {
            *recv_size = c;
        }

        return c > 0 ? 0 : -1;

    }

}

/**
 * Deinitialises a UART interface
 *
 * @param[in]  uart  the interface which should be deinitialised
 *
 * @return  0 : on success, EIO : if an error occurred with any step
 */
int32_t hal_uart_finalize(uart_dev_t *uart)
{
    if(uart->port != 0){
	u8 num = uart_obj[uart->port].id;
	UART_DeInit( UART_DEV_TABLE[num].UARTx);
	InterruptDis(UART_DEV_TABLE[num].IrqNum);
	InterruptUnRegister(UART_DEV_TABLE[num].IrqNum);

	aos_free(uart_rxbuffer[uart->port].rxbuffer);

	return 0;
    }else{
        if(g_uart_init == 0)
        {
            return 0;
        }

        uart0_rx_buf_head = 0;
        uart0_rx_buf_tail = 0;
        aos_event_free(&uart0_event);
        g_uart_init = 0;

        return 0;
    }
}

int hal_uart_rx_sem_take(int uart_id, int timeout)
{
    uint32_t val;

    if (uart_id != 0)
        return -EINVAL;

    return aos_event_get(&uart0_event, EVENT_RX_READY, AOS_EVENT_OR, &val, timeout);
}

int hal_uart_rx_sem_give(int uart_id)
{
    if (uart_id != 0)
        return -EINVAL;

    aos_event_set(&uart0_event, EVENT_RX_READY, AOS_EVENT_OR);

    return 0;
}
