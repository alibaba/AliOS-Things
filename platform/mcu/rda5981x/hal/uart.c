#include "hal/soc/soc.h"
#include "serial_api.h"
#include "rda5981x_pinconfig.h"
#include "aos/kernel.h"

#define UART_NUM 2

static serial_t serial_obj[UART_NUM];
static uint8_t is_inited[UART_NUM] = {0};

/*---------------------------------------------------------------------------*/
#define HFUART_RX_BUFFER_SIZE			(1024)

struct ringbuf {
    uint8_t *data;
    uint16_t mask;
    uint16_t put_ptr, get_ptr;
};

typedef struct {
    char *rxbuf;
    struct	ringbuf uart_rx_buf;
    uint8_t b_wait_rx;
    aos_sem_t rx_wait_event;
} uart_cfg_t;
static uart_cfg_t serial_cfg[UART_NUM];

static void ringbuf_init(struct ringbuf *r, uint8_t *dataptr, uint16_t size)
{
    r->data = dataptr;
    r->mask = size;
    r->put_ptr = 0;
    r->get_ptr = 0;
}

static int ringbuf_put(struct ringbuf *r, uint8_t c)
{
    if(((r->put_ptr+1)%r->mask)== r->get_ptr ) {
        return 0;
    }
    r->data[r->put_ptr] = c;
    r->put_ptr = (r->put_ptr + 1) % r->mask;

    if(((r->put_ptr+1)%r->mask)== r->get_ptr )
        return 2;
    return 1;
}

static int ringbuf_get(struct ringbuf *r)
{
    uint8_t c;
    if(r->put_ptr != r->get_ptr) {
        c = r->data[r->get_ptr];
        r->get_ptr +=1;
        if(r->get_ptr>=r->mask)
            r->get_ptr = r->get_ptr-r->mask;
        return c;
    } else {
        return -1;
    }
}

static int ringbuf_gets(struct ringbuf *r,uint8_t *data,int size)
{
    int cnt=0;
    uint8_t *src=r->data;

    while((r->put_ptr != r->get_ptr)&&(cnt<size)){
        data[cnt++] = src[r->get_ptr++];
        if(r->get_ptr>=r->mask)
            r->get_ptr = 0;
    }
    return cnt;
}

static void uart_rx_irq_handler(uint32_t id, SerialIrq event)
{
    int ret, fc=0;
    serial_t * p_uart=NULL;
    uart_cfg_t *uart_cfg = NULL;

    if(1 == id){
        p_uart = &serial_obj[id];
        uart_cfg = &serial_cfg[id];
    }
    else
        return;
	
    if(p_uart==NULL)
    {
        return;
    }
	
    if(RxIrq == event)
    {
        while(serial_readable(p_uart))
        {
            int c = serial_getc(p_uart);
            ret = ringbuf_put(&uart_cfg->uart_rx_buf,c);
			
            if(ret!=1)//uart cache is full
            {
            }
            if(uart_cfg->b_wait_rx)
            {
                aos_sem_signal(&uart_cfg->rx_wait_event);
                uart_cfg->b_wait_rx = 0;				
            }
        }
    }
}

uart_dev_t uart_0 = {
    0,
    {921600, DATA_WIDTH_8BIT, NO_PARITY, STOP_BITS_1, FLOW_CONTROL_DISABLED},
    NULL,
};

int32_t hal_uart_init(uart_dev_t *uart)
{
    if (uart->port >= UART_NUM)
        return -1;

    if ((0 == uart->port) && (FLOW_CONTROL_DISABLED != uart->config.flow_control))
        return -1;

    uart->priv = &serial_obj[uart->port];

    if (0 == uart->port) {
        serial_init(uart->priv, RDA_UART0_TXD, RDA_UART0_RXD);
    } else {
        serial_init(uart->priv, RDA_UART1_TXD_2, RDA_UART1_RXD_2);
    }

    serial_baud(uart->priv, uart->config.baud_rate);
    serial_format(uart->priv, (uart->config.data_width + 5), uart->config.parity, (uart->config.stop_bits + 1));
    if(1 == uart->port)
    {
	 aos_sem_new(&serial_cfg[uart->port].rx_wait_event, 0);
	 serial_cfg[uart->port].rxbuf = (char*)aos_malloc(HFUART_RX_BUFFER_SIZE);
	 ringbuf_init(&serial_cfg[uart->port].uart_rx_buf, (uint8_t*)serial_cfg[uart->port].rxbuf, HFUART_RX_BUFFER_SIZE);
	 
        serial_irq_handler(uart->priv, uart_rx_irq_handler, (uint32_t)uart->port);
        serial_irq_set(uart->priv, RxIrq, 1);
        serial_irq_set(uart->priv, TxIrq, 0);
    }
	
    if (FLOW_CONTROL_CTS_RTS == uart->config.flow_control) {
        serial_set_flow_control(uart->priv, FLOW_CONTROL_CTS_RTS, RDA_UART1_RTS, RDA_UART1_CTS);
    }
	is_inited[uart->port] = 1;
    return 0;
}

int32_t hal_uart_send(uart_dev_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
    if (0 == is_inited[uart->port]) {
        hal_uart_init(uart);
        is_inited[uart->port] = 1;
    }

    uint8_t *send_data = (char *)data;

    while (size > 0) {
        serial_putc(uart->priv, (int)*send_data);
        send_data++;
        size--;
    }

    return 0;
}

int32_t hal_uart_recv(uart_dev_t *uart, void *data, uint32_t expect_size, uint32_t timeout)
{
    hal_uart_recv_II(uart, data, expect_size, NULL, timeout);
    return 0;
}

int32_t hal_uart_recv_II(uart_dev_t *uart, void *data, uint32_t expect_size,
                         uint32_t *recv_size, uint32_t timeout)
{

    if (0 == is_inited[uart->port]) {
        hal_uart_init(uart);
        is_inited[uart->port] = 1;
    }
	
    if(0 == uart->port){
	    uint8_t *recv_data = (char *)data;

	    while (expect_size > 0) {
	        *recv_data = (uint8_t)serial_getc(uart->priv);
	        recv_data++;
	        expect_size--;
	    }

	    if (NULL != recv_size)
	        recv_size = expect_size;
    }
    else
    {
	    uint8_t *p_data = (uint8_t*)data;
	    int ch, recvnum=0;
	    uart_cfg_t *uart_cfg = &serial_cfg[uart->port];
		
	    if((ch=ringbuf_get(&uart_cfg->uart_rx_buf))==-1){
	        uart_cfg->b_wait_rx = 1;
	        aos_sem_wait(&uart_cfg->rx_wait_event, timeout);
	        uart_cfg->b_wait_rx = 0;
	        if((ch=ringbuf_get(&uart_cfg->uart_rx_buf))==-1){
	            if(recv_size != NULL)
	                *recv_size = recvnum;
	            return 0;
	        }
	    }
           recvnum = 1;

	    *p_data = (uint8_t)ch;
	    p_data++;

	    while(recvnum<(int)expect_size)
	    {
	        if((ch=ringbuf_get(&uart_cfg->uart_rx_buf))==-1){
	            uart_cfg->b_wait_rx = 1;
	            aos_sem_wait(&uart_cfg->rx_wait_event, 20);
	            uart_cfg->b_wait_rx = 0;
	            if((ch=ringbuf_get(&uart_cfg->uart_rx_buf))==-1){
	                if(recv_size != NULL)
			      *recv_size = recvnum;
	                return 0;
	            }
	        }
			
	        recvnum++;
	        *p_data = (uint8_t)ch;
	        p_data++;
	    }
		
	    if(recv_size != NULL)
	        *recv_size = recvnum;
    }
    return 0;	
}

int32_t hal_uart_finalize(uart_dev_t *uart)
{
    return 0;
}

