#include "include.h"
#include "arm_arch.h"

#include "uart_pub.h"
#include "uart.h"

#include "drv_model_pub.h"
#include "sys_ctrl_pub.h"
#include "mem_pub.h"
#include "icu_pub.h"
#include "gpio_pub.h"
#include "sys_version.h"

#include <stdio.h>

#include "mem_pub.h"
#include "intc_pub.h"
#if CFG_USE_STA_PS
#include "power_save_pub.h"
#endif
#include "mcu_ps_pub.h"

#if CFG_SUPPORT_RTT
#include <rtthread.h>
#include <rthw.h>
#endif

#if (!CFG_SUPPORT_ALIOS && !CFG_SUPPORT_RTT)
#include "uart1_tcp_server_demo.h"
#endif

static struct uart_callback_des uart_receive_callback[2] = {NULL, NULL};
static struct uart_callback_des uart_txfifo_needwr_callback[2] = {NULL, NULL};
static struct uart_callback_des uart_tx_end_callback[2] = {NULL, NULL};

#ifndef KEIL_SIMULATOR
#if CFG_UART_DEBUG_COMMAND_LINE
UART_S uart[2] =
{
    {0, 0, 0}, 
	{0, 0, 0}
};

static DD_OPERATIONS uart1_op =
{
    uart1_open,
    uart1_close,
    uart1_read,
    uart1_write,
    uart1_ctrl
};

static DD_OPERATIONS uart2_op =
{
    uart2_open,
    uart2_close,
    uart2_read,
    uart2_write,
    uart2_ctrl
};
#endif

UINT8 uart_is_tx_fifo_empty(UINT8 uport)
{
	UINT32 param;

    if(UART1_PORT == uport)
        param = REG_READ(REG_UART1_FIFO_STATUS);
    else
        param = REG_READ(REG_UART2_FIFO_STATUS);

    return (param & TX_FIFO_EMPTY) != 0 ? 1 : 0;
}

UINT8 uart_is_tx_fifo_full(UINT8 uport)
{
	UINT32 param;

    if(UART1_PORT == uport)
        param = REG_READ(REG_UART1_FIFO_STATUS);
    else
        param = REG_READ(REG_UART2_FIFO_STATUS);

    return (param & TX_FIFO_FULL) != 0 ? 1 : 0;
}

void bk_send_byte(UINT8 uport, UINT8 data)
{
    if(UART1_PORT == uport)
        while(!UART1_TX_WRITE_READY);
    else
        while(!UART2_TX_WRITE_READY);

    UART_WRITE_BYTE(uport, data);
}

void bk_send_string(UINT8 uport, const char *string)
{
    while(*string)
    {
        bk_send_byte(uport, *string++);
    }
}
/*uart2 as deubg port*/
void bk_printf(const char *fmt, ...)
{
#if CFG_SUPPORT_RTT
    va_list args;
    rt_size_t length;
    static char rt_log_buf[RT_CONSOLEBUF_SIZE];

    va_start(args, fmt);
    /* the return value of vsnprintf is the number of bytes that would be
     * written to buffer had if the size of the buffer been sufficiently
     * large excluding the terminating null byte. If the output string
     * would be larger than the rt_log_buf, we have to adjust the output
     * length. */
    length = rt_vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);
    if (length > RT_CONSOLEBUF_SIZE - 1)
        length = RT_CONSOLEBUF_SIZE - 1;    
    rt_kprintf("%s", rt_log_buf);
    va_end(args);
#else
    va_list ap;
    char string[128];

    va_start(ap, fmt);
    vsprintf(string, fmt, ap);
    string[127] = 0;
    bk_send_string(UART1_PORT, string);
    va_end(ap);
#endif

}


#if CFG_BACKGROUND_PRINT
INT32 uart_printf(const char *fmt, ...)
{
    INT32 rc;
    char buf[TX_RB_LENGTH];

    va_list args;
    va_start(args, fmt);
    rc = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    buf[sizeof(buf) - 1] = '\0';
    if((rc < sizeof(buf) - 2)
            && (buf[rc - 1] == '\n')
            && (buf[rc - 2] != '\r'))
    {
        buf[rc - 1] = '\r';
        buf[rc] = '\n';
        buf[rc + 1] = 0;

        rc += 1;
    }

    return kfifo_put(uart[1].tx, (UINT8 *)&buf[0], rc);
}
#endif // CFG_BACKGROUND_PRINT

void fatal_print(const char *fmt, ...)
{
    os_printf(fmt);

    DEAD_WHILE();
}

void uart_hw_init(UINT8 uport)
{
    UINT32 reg, baud_div;
    UINT32 conf_reg_addr, fifo_conf_reg_addr;
    UINT32 flow_conf_reg_addr, wake_conf_reg_addr, intr_reg_addr;

    baud_div = UART_CLOCK / UART_BAUD_RATE;
    baud_div = baud_div - 1;

    if(UART1_PORT == uport)
    {
        conf_reg_addr = REG_UART1_CONFIG;
        fifo_conf_reg_addr = REG_UART1_FIFO_CONFIG;
        flow_conf_reg_addr = REG_UART1_FLOW_CONFIG;
        wake_conf_reg_addr = REG_UART1_WAKE_CONFIG;
        intr_reg_addr = REG_UART1_INTR_ENABLE;
    }
    else
    {
        conf_reg_addr = REG_UART2_CONFIG;
        fifo_conf_reg_addr = REG_UART2_FIFO_CONFIG;
        flow_conf_reg_addr = REG_UART2_FLOW_CONFIG;
        wake_conf_reg_addr = REG_UART2_WAKE_CONFIG;
        intr_reg_addr = REG_UART2_INTR_ENABLE;
    }

    reg = UART_TX_ENABLE
          | UART_RX_ENABLE
          & (~UART_IRDA)
          | ((DEF_DATA_LEN & UART_DATA_LEN_MASK) << UART_DATA_LEN_POSI)
          & (~UART_PAR_EN)
          & (~UART_PAR_ODD_MODE)
          & (~UART_STOP_LEN_2)
          | ((baud_div & UART_CLK_DIVID_MASK) << UART_CLK_DIVID_POSI);
    REG_WRITE(conf_reg_addr, reg);

    reg = ((TX_FIFO_THRD & TX_FIFO_THRESHOLD_MASK) << TX_FIFO_THRESHOLD_POSI)
          | ((RX_FIFO_THRD & RX_FIFO_THRESHOLD_MASK) << RX_FIFO_THRESHOLD_POSI)
          | ((RX_STOP_DETECT_TIME32 & RX_STOP_DETECT_TIME_MASK) << RX_STOP_DETECT_TIME_POSI);
    REG_WRITE(fifo_conf_reg_addr, reg);

    REG_WRITE(flow_conf_reg_addr, 0);
    REG_WRITE(wake_conf_reg_addr, 0);

    reg = RX_FIFO_NEED_READ_EN | UART_RX_STOP_END_EN;
    REG_WRITE(intr_reg_addr, reg);

    return;
}

void uart_hw_set_change(UINT8 uport, uart_config_t *uart_config)
{
    UINT32 reg, baud_div, width;
#if CFG_SUPPORT_ALIOS
    hal_uart_parity_t 	     parity_en;
    hal_uart_stop_bits_t	  stop_bits;
    hal_uart_flow_control_t  flow_control;
#else
    uart_parity_t 	     parity_en;
    uart_stop_bits_t	  stop_bits;
    uart_flow_control_t  flow_control;
#endif
    UINT8 parity_mode = 0;
    UINT32 intr_ena_reg_addr, conf_reg_addr, fifi_conf_reg_addr;
    UINT32 flow_conf_reg_addr, wake_reg_addr;

    if(UART1_PORT == uport)
    {
        intr_ena_reg_addr = REG_UART1_INTR_ENABLE;
        conf_reg_addr = REG_UART1_CONFIG;
        fifi_conf_reg_addr = REG_UART1_FIFO_CONFIG;
        flow_conf_reg_addr = REG_UART1_FLOW_CONFIG;
        wake_reg_addr = REG_UART1_WAKE_CONFIG;
    }
    else
    {
        intr_ena_reg_addr = REG_UART2_INTR_ENABLE;
        conf_reg_addr = REG_UART2_CONFIG;
        fifi_conf_reg_addr = REG_UART2_FIFO_CONFIG;
        flow_conf_reg_addr = REG_UART2_FLOW_CONFIG;
        wake_reg_addr = REG_UART2_WAKE_CONFIG;
    }
    REG_WRITE(intr_ena_reg_addr, 0);//disable int

    baud_div = UART_CLOCK / uart_config->baud_rate;
    baud_div = baud_div - 1;
    width = uart_config->data_width;
    parity_en = uart_config->parity;
    stop_bits = uart_config->stop_bits;
    flow_control = uart_config->flow_control;

    if(parity_en)
    {
#if CFG_SUPPORT_ALIOS
        if(parity_en == ODD_PARITY)
#else
        if(parity_en == BK_PARITY_ODD)
#endif
            parity_mode = 1;
        else
            parity_mode = 0;
        parity_en = 1;
    }

    reg = UART_TX_ENABLE
          | UART_RX_ENABLE
          & (~UART_IRDA)
          | ((width & UART_DATA_LEN_MASK) << UART_DATA_LEN_POSI)
          | (parity_en << 5)
          | (parity_mode << 6)
          | (stop_bits << 7)
          | ((baud_div & UART_CLK_DIVID_MASK) << UART_CLK_DIVID_POSI);

    width = ((width & UART_DATA_LEN_MASK) << UART_DATA_LEN_POSI);
    REG_WRITE(conf_reg_addr, reg);

    reg = ((TX_FIFO_THRD & TX_FIFO_THRESHOLD_MASK) << TX_FIFO_THRESHOLD_POSI)
          | ((RX_FIFO_THRD & RX_FIFO_THRESHOLD_MASK) << RX_FIFO_THRESHOLD_POSI)
          | ((RX_STOP_DETECT_TIME32 & RX_STOP_DETECT_TIME_MASK) << RX_STOP_DETECT_TIME_POSI);
    REG_WRITE(fifi_conf_reg_addr, reg);

    REG_WRITE(flow_conf_reg_addr, 0);
    REG_WRITE(wake_reg_addr, 0);

    reg = RX_FIFO_NEED_READ_EN | UART_RX_STOP_END_EN;
    REG_WRITE(intr_ena_reg_addr, reg);
}


#if CFG_UART_DEBUG_COMMAND_LINE
UINT32 uart_sw_init(UINT8 uport)
{
    uart[uport].rx = kfifo_alloc(RX_RB_LENGTH);
    uart[uport].tx = kfifo_alloc(TX_RB_LENGTH);

    if((!uart[uport].tx) || (!uart[uport].rx))
    {
        if(uart[uport].tx)
        {
            kfifo_free(uart[uport].tx);
        }

        if(uart[uport].rx)
        {
            kfifo_free(uart[uport].rx);
        }
        return UART_FAILURE;
    }

    return UART_SUCCESS;
}

UINT32 uart_sw_uninit(UINT8 uport)
{
    if(uart[uport].tx)
    {
        kfifo_free(uart[uport].tx);
    }

    if(uart[uport].rx)
    {
        kfifo_free(uart[uport].rx);
    }

    os_memset(&uart[uport], 0, sizeof(uart[uport]));

    return UART_SUCCESS;
}

void uart_fifo_flush(UINT8 uport)
{
    UINT32 val;
    UINT32 reg;
    UINT32 reg_addr;

    if(UART1_PORT == uport)
        reg_addr = REG_UART1_CONFIG;
    else
        reg_addr = REG_UART2_CONFIG;

    val = REG_READ(reg_addr);

    reg = val & (~(UART_TX_ENABLE | UART_RX_ENABLE));

    REG_WRITE(reg_addr, reg);
    REG_WRITE(reg_addr, val);
}

void uart_hw_uninit(UINT8 uport)
{
    UINT32 i;
    UINT32 reg;
    UINT32 rx_count;
    UINT32 intr_ena_reg_addr, conf_reg_addr, fifostatus_reg_addr;
    if(UART1_PORT == uport)
    {
        intr_ena_reg_addr = REG_UART1_INTR_ENABLE;
        conf_reg_addr = REG_UART1_CONFIG;
        fifostatus_reg_addr = REG_UART1_FIFO_STATUS;
    }
    else
    {
        intr_ena_reg_addr = REG_UART2_INTR_ENABLE;
        conf_reg_addr = REG_UART2_CONFIG;
        fifostatus_reg_addr = REG_UART2_FIFO_STATUS;
    }
    /*disable rtx intr*/
    reg = REG_READ(intr_ena_reg_addr);
    reg &= (~(RX_FIFO_NEED_READ_EN | UART_RX_STOP_END_EN));
    REG_WRITE(intr_ena_reg_addr, reg);

    /* flush fifo*/
    uart_fifo_flush(uport);

    /* disable rtx*/
    reg = REG_READ(conf_reg_addr);
    reg = reg & (~(UART_TX_ENABLE | UART_RX_ENABLE));
    REG_WRITE(conf_reg_addr, reg);

    /* double discard fifo data*/
    reg = REG_READ(fifostatus_reg_addr);
    rx_count = (reg >> RX_FIFO_COUNT_POSI) & RX_FIFO_COUNT_MASK;
    for(i = 0; i < rx_count; i ++)
    {
        UART_READ_BYTE_DISCARD(uport);
    }
}

void uart_reset(UINT8 uport)
{
    if(UART1_PORT == uport)
    {
        uart1_exit();
        uart1_init();
    }
    else
    {
        uart2_exit();
        uart2_init();
    }
}

void uart_send_backgroud(void)
{
    /* send the buf at backgroud context*/
    uart_write_fifo_frame(UART2_PORT, uart[UART2_PORT].tx, DEBUG_PRT_MAX_CNT);
}

UINT32 uart_write_fifo_frame(UINT8 uport, KFIFO_PTR tx_ptr, UINT32 count)
{
    UINT32 len;
    UINT32 ret;
    UINT32 val;

    len = 0;

    while(1)
    {
        ret = kfifo_get(tx_ptr, (UINT8 *)&val, 1);
        if(0 == ret)
        {
            break;
        }


#if __CC_ARM
        uart_send_byte(uport, (UINT8)val);
#else
        bk_send_byte(uport, (UINT8)val);
#endif

        len += ret;
        if(len >= count)
        {
            break;
        }
    }

    return len;
}

UINT32 uart_read_fifo_frame(UINT8 uport, KFIFO_PTR rx_ptr)
{
    UINT32 val;
    UINT32 rx_count, fifo_status_reg;

    if(UART1_PORT == uport)
        fifo_status_reg = REG_UART1_FIFO_STATUS;
    else
        fifo_status_reg = REG_UART2_FIFO_STATUS;

    rx_count = 0;
    while(REG_READ(fifo_status_reg) & FIFO_RD_READY)
    {
        UART_READ_BYTE(uport, val);
        rx_count += kfifo_put(rx_ptr, (UINT8 *)&val, 1);
    }

    return rx_count;
}

void uart_set_tx_fifo_needwr_int(UINT8 uport, UINT8 set)
{
	UINT32 reg;

	if(UART1_PORT == uport)
		reg = REG_READ(REG_UART1_INTR_ENABLE);
	else
		reg = REG_READ(REG_UART2_INTR_ENABLE);
	
	if(set == 1)
	{
		reg |= (TX_FIFO_NEED_WRITE_EN);
	}
	else
	{
		reg &= ~(TX_FIFO_NEED_WRITE_EN);
	}
	
	if(UART1_PORT == uport){
        //rt_kprintf("set_tx_fifo:%d\r\n", set);
		REG_WRITE(REG_UART1_INTR_ENABLE, reg);
	}
	else
		REG_WRITE(REG_UART2_INTR_ENABLE, reg);
}

void uart_set_tx_stop_end_int(UINT8 uport, UINT8 set)
{
	UINT32 reg;

	if(UART1_PORT == uport)
		reg = REG_READ(REG_UART1_INTR_ENABLE);
	else
		reg = REG_READ(REG_UART2_INTR_ENABLE);
	
	if(set == 1)
	{
		reg |= (UART_TX_STOP_END_EN);
	}
	else
	{
		reg &= ~(UART_TX_STOP_END_EN);
	}
	
	if(UART1_PORT == uport)
		REG_WRITE(REG_UART1_INTR_ENABLE, reg);
	else 
		REG_WRITE(REG_UART2_INTR_ENABLE, reg);
}

/*******************************************************************/
void uart1_isr(void)
{

    UINT32 status;
    UINT32 intr_en;
    UINT32 intr_status;

    intr_en = REG_READ(REG_UART1_INTR_ENABLE);
    intr_status = REG_READ(REG_UART1_INTR_STATUS);
    REG_WRITE(REG_UART1_INTR_STATUS, intr_status);
    status = intr_status & intr_en;

    if(status & (RX_FIFO_NEED_READ_STA | UART_RX_STOP_END_STA))
    {
#if (!CFG_SUPPORT_RTT)
        uart_read_fifo_frame(UART1_PORT, uart[UART1_PORT].rx);
#endif

        if (uart_receive_callback[0].callback != 0)
        {
            void *param = uart_receive_callback[0].param;

            uart_receive_callback[0].callback(UART1_PORT, param);
        }
        else
        {
        	uart_read_byte(UART1_PORT); /*drop data for rtt*/
        }
    }
	
    if(status & TX_FIFO_NEED_WRITE_STA)
    {
        if (uart_txfifo_needwr_callback[0].callback != 0)
        {
            void *param = uart_txfifo_needwr_callback[0].param;

            uart_txfifo_needwr_callback[0].callback(UART1_PORT, param);
        } 
    }
	
    if(status & RX_FIFO_OVER_FLOW_STA)
    {
    }
	
    if(status & UART_RX_PARITY_ERR_STA)
    {
        uart_fifo_flush(UART1_PORT);
    }
	
    if(status & UART_RX_STOP_ERR_STA)
    {
    }
	
    if(status & UART_TX_STOP_END_STA)
    {
        if (uart_tx_end_callback[0].callback != 0)
        {
            void *param = uart_tx_end_callback[0].param;

            uart_tx_end_callback[0].callback(UART1_PORT, param);
        }
    }
    
	if(status & UART_RXD_WAKEUP_STA)
    {
    }
}
void uart1_init(void)
{
    UINT32 ret;
    UINT32 param;
    UINT32 intr_status;

    ret = uart_sw_init(UART1_PORT);
    ASSERT(UART_SUCCESS == ret);

    ddev_register_dev(UART1_DEV_NAME, &uart1_op);

    intc_service_register(IRQ_UART1, PRI_IRQ_UART1, uart1_isr);

    param = PWD_UART1_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);

    param = GFUNC_MODE_UART1;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
    uart_hw_init(UART1_PORT);

    /*irq enable, Be careful: it is best that irq enable at open routine*/
    intr_status = REG_READ(REG_UART1_INTR_STATUS);
    REG_WRITE(REG_UART1_INTR_STATUS, intr_status);

    param = IRQ_UART1_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);
}

void uart1_exit(void)
{
    UINT32 param;

    /*irq enable, Be careful: it is best that irq enable at close routine*/
    param = IRQ_UART1_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);

    uart_hw_uninit(UART1_PORT);

    ddev_unregister_dev(UART1_DEV_NAME);

    uart_sw_uninit(UART1_PORT);
}

UINT32 uart1_open(UINT32 op_flag)
{
    return UART_SUCCESS;
}

UINT32 uart1_close(void)
{
    return UART_SUCCESS;
}

UINT32 uart1_read(char *user_buf, UINT32 count, UINT32 op_flag)
{
    return kfifo_get(uart[UART1_PORT].rx, (UINT8 *)user_buf, count);
}

UINT32 uart1_write(char *user_buf, UINT32 count, UINT32 op_flag)
{
    return kfifo_put(uart[UART1_PORT].tx, (UINT8 *)user_buf, count);
}

UINT32 uart1_ctrl(UINT32 cmd, void *parm)
{
    UINT32 ret;
    peri_busy_count_add();

    ret = UART_SUCCESS;
    switch(cmd)
    {
    case CMD_SEND_BACKGROUND:
        //     uart_send_backgroud();
        break;

    case CMD_UART_RESET:
        uart_reset(UART1_PORT);
        break;

    case CMD_RX_COUNT:
        ret = kfifo_data_size(uart[UART1_PORT].rx);
        break;

    case CMD_RX_PEEK:
    {
        UART_PEEK_RX_PTR peek;

        peek = (UART_PEEK_RX_PTR)parm;

        if(!((URX_PEEK_SIG != peek->sig)
                || (NULLPTR == peek->ptr)
                || (0 == peek->len)))
        {
            ret = kfifo_out_peek(uart[UART1_PORT].rx, peek->ptr, peek->len);
        }

        break;
    }
    case CMD_UART_INIT:
        uart_hw_set_change(UART1_PORT, parm);
        break;
    case CMD_UART_SET_RX_CALLBACK:
        if (parm)
        {
            struct uart_callback_des *uart_callback;

            uart_callback = (struct uart_callback_des *)parm;

            uart_rx_callback_set(UART1_PORT, uart_callback->callback, uart_callback->param);
        }
        else
        {
            uart_rx_callback_set(UART1_PORT, NULL, NULL);
        }
        break;
    case CMD_UART_SET_TX_CALLBACK:
        if (parm)
        {
            struct uart_callback_des *uart_callback;

            uart_callback = (struct uart_callback_des *)parm;

            uart_tx_end_callback_set(UART1_PORT, uart_callback->callback, uart_callback->param);
        }
        else
        {
            uart_tx_end_callback_set(UART1_PORT, NULL, NULL);
        }        
        break;
    case CMD_UART_SET_TX_FIFO_NEEDWR_CALLBACK:
        if (parm)
        {
            struct uart_callback_des *uart_callback;

            uart_callback = (struct uart_callback_des *)parm;

            uart_tx_fifo_needwr_callback_set(UART1_PORT, uart_callback->callback, uart_callback->param);
        }
        else
        {
            uart_tx_fifo_needwr_callback_set(UART1_PORT, NULL, NULL);
        }        
        break;            
	case CMD_SET_STOP_END:
		uart_set_tx_stop_end_int(UART1_PORT, *(UINT8 *)parm);
		break;
        
	case CMD_SET_TX_FIFO_NEEDWR_INT:
		uart_set_tx_fifo_needwr_int(UART1_PORT, *(UINT8 *)parm);
		break;
    default:
        break;
    }

    peri_busy_count_dec();

    return ret;
}


void uart2_isr(void)
{
#if CFG_UART_DEBUG_COMMAND_LINE
    UINT32 status;
    UINT32 intr_en;
    UINT32 intr_status;

    intr_en = REG_READ(REG_UART2_INTR_ENABLE);
    intr_status = REG_READ(REG_UART2_INTR_STATUS);
    REG_WRITE(REG_UART2_INTR_STATUS, intr_status);
    status = intr_status & intr_en;

    if(status & (RX_FIFO_NEED_READ_STA | UART_RX_STOP_END_STA))
    {
#if (!CFG_SUPPORT_RTT)
        uart_read_fifo_frame(UART2_PORT, uart[UART2_PORT].rx);
#endif

        if (uart_receive_callback[1].callback != 0)
        {
            void *param = uart_receive_callback[1].param;

            uart_receive_callback[1].callback(UART2_PORT, param);
        }
        else
        {
        	uart_read_byte(UART2_PORT); /*drop data for rtt*/
        }
    }

	if(status & TX_FIFO_NEED_WRITE_STA)
    {
        if (uart_txfifo_needwr_callback[1].callback != 0)
        {
            void *param = uart_txfifo_needwr_callback[1].param;

            uart_txfifo_needwr_callback[1].callback(UART2_PORT, param);
        }      
    }

	if(status & RX_FIFO_OVER_FLOW_STA)
    {
    }

	if(status & UART_RX_PARITY_ERR_STA)
    {
        uart_fifo_flush(UART2_PORT);
    }
 
	if(status & UART_RX_STOP_ERR_STA)
    {
    }

	if(status & UART_TX_STOP_END_STA)
    {
        if (uart_tx_end_callback[1].callback != 0)
        {
            void *param = uart_tx_end_callback[1].param;

            uart_tx_end_callback[1].callback(UART2_PORT, param);
        }
    }

	if(status & UART_RXD_WAKEUP_STA)
    {
    }

#endif
}
void uart2_init(void)
{
    UINT32 ret;
    UINT32 param;
    UINT32 intr_status;

    ret = uart_sw_init(UART2_PORT);
    ASSERT(UART_SUCCESS == ret);

    ddev_register_dev(UART2_DEV_NAME, &uart2_op);

    intc_service_register(IRQ_UART2, PRI_IRQ_UART2, uart2_isr);

    param = PWD_UART2_CLK_BIT;
    sddev_control(ICU_DEV_NAME, CMD_CLK_PWR_UP, &param);

    param = GFUNC_MODE_UART2;
    sddev_control(GPIO_DEV_NAME, CMD_GPIO_ENABLE_SECOND, &param);
    uart_hw_init(UART2_PORT);

    /*irq enable, Be careful: it is best that irq enable at open routine*/
    intr_status = REG_READ(REG_UART2_INTR_STATUS);
    REG_WRITE(REG_UART2_INTR_STATUS, intr_status);

    param = IRQ_UART2_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

    os_printf("\r\n\r\n/*Version Information**************");
    os_printf("\r\n *        release_version:%s", RELEASE_VERSION);
    os_printf("\r\n *        release_time:%s", RELEASE_TIME);
    os_printf("\r\n *        full_mac_version:%s", FMALL_VERSION);
    os_printf("\r\n *        mac_lib_version:%s", FMAC_LIB_VERSON);
    os_printf("\r\n *        bulid date:%s, time:%s", __DATE__, __TIME__);
    os_printf("\r\n *Version Over**********************/\r\n\r\n");
}

void uart2_exit(void)
{
    UINT32 param;

    /*irq enable, Be careful: it is best that irq enable at close routine*/
    param = IRQ_UART2_BIT;
    sddev_control(ICU_DEV_NAME, CMD_ICU_INT_DISABLE, &param);

    uart_hw_uninit(UART2_PORT);

    ddev_unregister_dev(UART2_DEV_NAME);

    uart_sw_uninit(UART2_PORT);
}

UINT32 uart2_open(UINT32 op_flag)
{
    return UART_SUCCESS;
}

UINT32 uart2_close(void)
{
    return UART_SUCCESS;
}

UINT32 uart2_read(char *user_buf, UINT32 count, UINT32 op_flag)
{
    return kfifo_get(uart[UART2_PORT].rx, (UINT8 *)user_buf, count);
}

UINT32 uart2_write(char *user_buf, UINT32 count, UINT32 op_flag)
{
    return kfifo_put(uart[UART2_PORT].tx, (UINT8 *)user_buf, count);
}

UINT32 uart2_ctrl(UINT32 cmd, void *parm)
{
    UINT32 ret;
	
    peri_busy_count_add();
        
    ret = UART_SUCCESS;
    switch(cmd)
    {
    case CMD_SEND_BACKGROUND:
        uart_send_backgroud();
        break;

    case CMD_UART_RESET:
        uart_reset(UART2_PORT);
        break;

    case CMD_RX_COUNT:
        ret = kfifo_data_size(uart[UART2_PORT].rx);
        break;

    case CMD_RX_PEEK:
    {
        UART_PEEK_RX_PTR peek;

        peek = (UART_PEEK_RX_PTR)parm;

        if(!((URX_PEEK_SIG != peek->sig)
                || (NULLPTR == peek->ptr)
                || (0 == peek->len)))
        {
            ret = kfifo_out_peek(uart[UART2_PORT].rx, peek->ptr, peek->len);
        }

        break;
    }
    case CMD_UART_INIT:
        uart_hw_set_change(UART2_PORT, parm);
        break;
    case CMD_UART_SET_RX_CALLBACK:
        if (parm)
        {
            struct uart_callback_des *uart_callback;

            uart_callback = (struct uart_callback_des *)parm;

            uart_rx_callback_set(UART2_PORT, uart_callback->callback, uart_callback->param);
        }
        else
        {
            uart_rx_callback_set(UART2_PORT, NULL, NULL);
        }
        break;
    case CMD_UART_SET_TX_CALLBACK:
        if (parm)
        {
            struct uart_callback_des *uart_callback;

            uart_callback = (struct uart_callback_des *)parm;

            uart_tx_end_callback_set(UART2_PORT, uart_callback->callback, uart_callback->param);
        }
        else
        {
            uart_tx_end_callback_set(UART2_PORT, NULL, NULL);
        }        
        break;
    case CMD_UART_SET_TX_FIFO_NEEDWR_CALLBACK:
        if (parm)
        {
            struct uart_callback_des *uart_callback;

            uart_callback = (struct uart_callback_des *)parm;

            uart_tx_fifo_needwr_callback_set(UART2_PORT, uart_callback->callback, uart_callback->param);
        }
        else
        {
            uart_tx_fifo_needwr_callback_set(UART2_PORT, NULL, NULL);
        }        
        break;            
	case CMD_SET_STOP_END:
		uart_set_tx_stop_end_int(UART2_PORT, *(UINT8 *)parm);
		break;
        
	case CMD_SET_TX_FIFO_NEEDWR_INT:
		uart_set_tx_fifo_needwr_int(UART2_PORT, *(UINT8 *)parm);
		break;
    default:
        break;
    }
    
    peri_busy_count_dec();

    return ret;
}

void uart_wait_tx_over()
{
    while (UART2_TX_FIFO_EMPTY_GET() == 0)
    {
    }	

    while (UART1_TX_FIFO_EMPTY_GET() == 0)
    {
    }	
}
#endif // (!CFG_UART_DEBUG_COMMAND_LINE)

#endif // KEIL_SIMULATOR

INT32 os_null_printf(const char *fmt, ...)
{
    return 0;
}

int uart_read_byte(int uport)
{
    int val = -1;
    UINT32 fifo_status_reg;

    if (UART1_PORT == uport)
        fifo_status_reg = REG_UART1_FIFO_STATUS;
    else
        fifo_status_reg = REG_UART2_FIFO_STATUS;

    if (REG_READ(fifo_status_reg) & FIFO_RD_READY)
        UART_READ_BYTE(uport, val);

    return val;
}

int uart_write_byte(int uport, char c)
{
    if (UART1_PORT == uport)
        while(!UART1_TX_WRITE_READY);
    else
        while(!UART2_TX_WRITE_READY);

    UART_WRITE_BYTE(uport, c);

    return (1);
}

int uart_rx_callback_set(int uport, uart_callback callback, void *param)
{
    if (uport == UART1_PORT)
    {
        uart_receive_callback[0].callback = callback;
        uart_receive_callback[0].param = param;
    }
    else if (uport == UART2_PORT)
    {
        uart_receive_callback[1].callback = callback;
        uart_receive_callback[1].param = param;
    }
    else
    {
        return -1;
    }

    return 0;
}

int uart_tx_fifo_needwr_callback_set(int uport, uart_callback callback, void *param)
{
    if (uport == UART1_PORT)
    {
        uart_txfifo_needwr_callback[0].callback = callback;
        uart_txfifo_needwr_callback[0].param = param;
    }
    else if (uport == UART2_PORT)
    {
        uart_txfifo_needwr_callback[1].callback = callback;
        uart_txfifo_needwr_callback[1].param = param;
    }
    else
    {
        return -1;
    }

    return 0;
}

int uart_tx_end_callback_set(int uport, uart_callback callback, void *param)
{
    if (uport == UART1_PORT)
    {
        uart_tx_end_callback[0].callback = callback;
        uart_tx_end_callback[0].param = param;
    }
    else if (uport == UART2_PORT)
    {
        uart_tx_end_callback[1].callback = callback;
        uart_tx_end_callback[1].param = param;
    }
    else
    {
        return -1;
    }

    return 0;
}

// EOF
