#include "hal/soc/soc.h"
#include "serial_api.h"
#include "rda5981x_pinconfig.h"
#include "aos/kernel.h"
#include "rda_def.h"
#include "rda_sys_wrapper.h"
#include "k_dftdbg_config.h"

#define UART_NUM 2
#define DEFAULT_UART_RECV_BUF_SIZE 512

static serial_t serial_obj[UART_NUM];
static uint8_t is_inited[UART_NUM] = {0};

r_void *uart_recv_buf_queue[UART_NUM] = {NULL, NULL};
static void console_irq_handler(uint32_t index, SerialIrq event);

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
        uart_recv_buf_queue[0] = rda_queue_create(DEFAULT_UART_RECV_BUF_SIZE, sizeof(unsigned char));
    } else {
        serial_init(uart->priv, RDA_UART1_TXD_2, RDA_UART1_RXD_2);
        uart_recv_buf_queue[1] = rda_queue_create(DEFAULT_UART_RECV_BUF_SIZE, sizeof(unsigned char));
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
    } else {

#if (RHINO_CONFIG_PANIC_CLI == 0)
        serial_irq_handler(uart->priv, console_irq_handler, (uint32_t)1);
        serial_irq_set(uart->priv, RxIrq, 1);
#endif
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
        /* To avoid opening the UART0 automatically when testing power consumption, 
           the user needs to explicitly initialize the UART0 . 
           The debug UART does not print when the UART is not initialized */
#if 0
        hal_uart_init(uart);
        is_inited[uart->port] = 1;
#endif
        return 0;
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
    uint32_t rx_count = 0;
    int ret;

    if (0 == is_inited[uart->port]) {
        /* To avoid opening the UART0 automatically when testing power consumption, 
           the user needs to explicitly initialize the UART0 . 
           The debug UART does not print when the UART is not initialized */
#if 0
        hal_uart_init(uart);
        is_inited[uart->port] = 1;
#endif
        return 0;
    }
	
    if(0 == uart->port){
	    uint8_t *recv_data = (char *)data;
        uint8_t queue_data;
        if(uart_recv_buf_queue[uart->port] == NULL){
            mbed_error_printf("uart_%d_recv_buf_queue create failed\r\n",uart->port);
            if(recv_size != NULL)
                *recv_size = 0;
            return;
        }

#if (RHINO_CONFIG_PANIC_CLI == 0)
        for(rx_count = 0; rx_count < expect_size; rx_count++){
            ret = rda_queue_recv(uart_recv_buf_queue[uart->port], &queue_data, RDA_WAIT_FOREVER);
            if(!ret) {
                *recv_data ++ = queue_data;
            } else
                break;
        }
#else

        while (rx_count < expect_size) {
            while(serial_readable(&serial_obj[0])) {
                recv_data[rx_count++] = serial_getc(&serial_obj[0]); 
                if(rx_count == expect_size)
                    break;
            }

            if(rx_count == expect_size)
                break;
        }
#endif
        if(rx_count != expect_size)
            printf("uart recv error:expect=%d recv=%d\r\n",expect_size,rx_count);

	    if (NULL != recv_size)
            *recv_size = rx_count;
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

static void console_irq_handler(uint32_t index, SerialIrq event){
    unsigned char c;
    int ret;
    if(RxIrq == event){
        while(serial_readable(&serial_obj[index])){
            c = serial_getc(&serial_obj[index]);
            ret = rda_queue_send(uart_recv_buf_queue[index], (r_u32) &c, RDA_NO_WAIT);
            if(ret != 0) {
                mbed_error_printf("uart_recv_buf_queue full\r\n");
            }
        }
    }
}
