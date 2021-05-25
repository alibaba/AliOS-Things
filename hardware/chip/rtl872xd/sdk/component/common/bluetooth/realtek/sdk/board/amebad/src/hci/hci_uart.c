/**
 * Copyright (c) 2017, Realsil Semiconductor Corporation. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>

#include "ameba_soc.h"

#include "os_sched.h"
#include "os_pool.h"
#include "os_sync.h"
#include "os_mem.h"

#include "trace_app.h"

#include "hci_uart.h"
#include "bt_board.h"

#define HCI_UART_RX_BUF_SIZE        0x2000   /* RX buffer size 8K */
#define HCI_UART_RX_ENABLE_COUNT    (HCI_UART_RX_BUF_SIZE - 2 * (1021 + 5))   /* Enable RX */
#define HCI_UART_RX_DISABLE_COUNT   (HCI_UART_RX_BUF_SIZE - 1021 - 5 - 10)   /* Disable RX */
typedef struct
{
    UART_InitTypeDef    UART_InitStruct;
    IRQn_Type           irqn;
    uint32_t            ier;

    //tx
    uint32_t            tx_len;
    uint8_t            *tx_buf_cur;
    uint32_t            tx_len_cur;
    P_UART_TX_CB        tx_cb; 


//rx
    bool                rx_disabled;
    uint16_t            rx_read_idx;
    uint16_t            rx_write_idx;
    uint8_t             rx_buffer[HCI_UART_RX_BUF_SIZE];
    void*               rx_timer_handle;
    P_UART_RX_CB        rx_ind;

    bool                hci_uart_bridge_flag;
}T_HCI_UART;

//===========
T_HCI_UART *hci_uart_obj;

#define TX_TRASMIT_COUNT 16
#define hci_board_debug printf

#define HCI_UART_IDX  1      //(only 0, 1, 3)

/*BT CTS   PA0   ----- RTS_PIN*/
/*BT TX   PA2    ----- RX_PIN*/
/*BT RX   PA4    ----- TX_PIN*/
#if (HCI_UART_IDX == 0)
      #define HCI_UART_OUT
      #define HCI_UART_DEV  UART0_DEV
      #define HCI_UART_IRQ  UART0_IRQ
 #if 1
      #define HCI_TX_PIN    _PA_18
      #define HCI_RX_PIN    _PA_19
        //#define HCI_CTS_PIN   _PA_17
        //#define HCI_RTS_PIN   _PA_16    //BT_LOG
 #else
      #define HCI_TX_PIN    _PA_21
      #define HCI_RX_PIN    _PA_22
    //#define HCI_CTS_PIN   _PA_24
      #define HCI_RTS_PIN   _PA_23
 #endif

#elif (HCI_UART_IDX == 3)
      #define HCI_UART_OUT
      #define HCI_UART_DEV  UART3_DEV
      #define HCI_UART_IRQ  UARTLP_IRQ
      #define HCI_TX_PIN    _PA_26
      #define HCI_RX_PIN    _PA_25
    //#define HCI_CTS_PIN   _PA_25
      #define HCI_RTS_PIN   _PA_27
#else
      #define HCI_UART_DEV  UART1_DEV
      #define HCI_UART_IRQ  UART1_IRQ
#endif

#define HCIUART_IRQ_PRIO    10


//========================================HCI UART BRIDGE=================
void set_hci_uart_out(bool flag)
{
    T_HCI_UART *p_uart_obj = hci_uart_obj;
    if(p_uart_obj != NULL)
    {
        p_uart_obj->hci_uart_bridge_flag = flag;
    }
    else
    {
       hci_board_debug("set_hci_uart_out: hci_uart_obj is NULL\r\n");
    }
}

bool hci_uart_tx_bridge(uint8_t rc)
{
    UART_CharPut(HCI_UART_DEV, rc);
    return true;
}

bool hci_uart_rx_bridge(uint8_t rc)
{
//extern void bt_uart_tx(uint8_t rc);
    //bt_uart_tx(rc);
    return true;
}
uint8_t hci_rx_empty()
{
    uint16_t tmpRead = hci_uart_obj->rx_read_idx;
    uint16_t tmpWrite = hci_uart_obj->rx_write_idx;
    return (tmpRead == tmpWrite);
}

uint16_t hci_rx_data_len()
{
    return (hci_uart_obj->rx_write_idx + HCI_UART_RX_BUF_SIZE - hci_uart_obj->rx_read_idx) % HCI_UART_RX_BUF_SIZE;
}
uint16_t hci_rx_space_len()
{
    return (hci_uart_obj->rx_read_idx + HCI_UART_RX_BUF_SIZE - hci_uart_obj->rx_write_idx - 1) % HCI_UART_RX_BUF_SIZE;
}
//========================================HCI UART BRIDGE==================

void hci_uart_set_baudrate(uint32_t baudrate)
{
    hci_board_debug("Set baudrate to %d\r\n", baudrate);
    UART_SetBaud(HCI_UART_DEV, baudrate);
}

void hci_uart_rx_disable(T_HCI_UART *hci_adapter)
{
    /* We disable received data available and rx timeout interrupt, then
     * the rx data will stay in UART FIFO, and RTS will be pulled high if
     * the watermark is higher than rx trigger level. */
    hci_board_debug("hci_uart_rx_disable\r\n");
    UART_INTConfig(HCI_UART_DEV, RUART_IER_ERBI | RUART_IER_ETOI, DISABLE);
    hci_adapter->rx_disabled = true;
}

void hci_uart_rx_enable(T_HCI_UART *hci_adapter)
{
    hci_board_debug("hci_uart_rx_enable\r\n");
    UART_INTConfig(HCI_UART_DEV, RUART_IER_ERBI | RUART_IER_ETOI, ENABLE);
    hci_adapter->rx_disabled = false;
}

static inline void hciuart_stop_tx(T_HCI_UART *hci_adapter)
{
    if (hci_adapter->ier & RUART_IER_ETBEI)
    {
        hci_adapter->ier &= ~RUART_IER_ETBEI;
        UART_INTConfig(HCI_UART_DEV, RUART_IER_ETBEI, DISABLE);
    }
}


static inline void transmit_chars(T_HCI_UART *hci_adapter)
{
    int count;

    if(hci_adapter == NULL)
    {
        hci_board_debug("transmit_chars: hci_adapter is NULL\r\n");
        return;
    }

    if (hci_adapter->tx_len_cur == 0)
    {
        hciuart_stop_tx(hci_adapter);
        if (hci_adapter->tx_cb)
        {
            hci_adapter->tx_cb();
        }
        return;
    }

    count = TX_TRASMIT_COUNT;
    do
    {
        UART_CharPut(HCI_UART_DEV, *(hci_adapter->tx_buf_cur));
        hci_adapter->tx_buf_cur++;
        hci_adapter->tx_len_cur--;
    }
    while (--count > 0 && hci_adapter->tx_len_cur > 0);
}

static inline void uart_insert_char(T_HCI_UART *hci_adapter, uint8_t ch)
{

    /* Should neve happen */
    if (hci_rx_space_len()==0)
    {
        hci_board_debug("uart_insert_char: rx buffer full\r\n");
        return;
    }

   // if(rltk_wlan_is_mp())
    {
        if(hci_adapter->hci_uart_bridge_flag == true)
        {
            hci_uart_rx_bridge(ch);
            return;
        }
    }
    hci_adapter->rx_buffer[hci_adapter->rx_write_idx++] = ch;
    hci_adapter->rx_write_idx %= HCI_UART_RX_BUF_SIZE;


    if (hci_rx_data_len() >= HCI_UART_RX_DISABLE_COUNT && hci_adapter->rx_disabled == false)
    {
        hci_board_debug("uart_insert_char: rx disable, data len %d\r\n", hci_rx_data_len());
        hci_uart_rx_disable(hci_adapter);
    }
}

static inline void receive_chars(T_HCI_UART *hci_adapter, int ind)
{
    int max_count = 16;
    u8 byte;

    if(hci_adapter == NULL)
    {
        UART_CharGet(HCI_UART_DEV, &byte);
        hci_board_debug("receive_chars: hci_adapter is NULL, ind:%x, data:%x\r\n", ind, byte);
        return;
    }
    /* start timer*/
    do
    {
        if (UART_Readable(HCI_UART_DEV))
        {
            UART_CharGet(HCI_UART_DEV, &byte);
            uart_insert_char(hci_adapter, byte);
        }
        else
        {
            break;
        }
    }
    while (--max_count > 0);

    /* HCI_PRINT_INFO1("receive_chars: rx_len %u", hci_adapter->rx_len); */
    /* FIXME: There is too many rx indication events ? */
    if (ind && hci_adapter->rx_ind)
    {
        hci_adapter->rx_ind();

    }
}

u32 hciuart_irq(void *data)
{
    volatile u8 reg_iir;
    u8 int_id;
    u32 reg_val;
    T_HCI_UART *hci_adapter = (T_HCI_UART *) data;

    reg_iir = UART_IntStatus(HCI_UART_DEV);
    if ((reg_iir & RUART_IIR_INT_PEND) != 0)
    {
        /* No pending IRQ */
        return 0;
    }

    int_id = (reg_iir & RUART_IIR_INT_ID) >> 1;

    switch (int_id)
    {
    case RUART_LP_RX_MONITOR_DONE:
        reg_val = UART_RxMonitorSatusGet(HCI_UART_DEV);
        /* if(UART0_DEV == hci_adapter->UARTx){
         *     DUart0MonitorDone = 1;
         * } else {
         *     DUart1MonitorDone = 1;
         * } */
        hci_board_debug("hciuart_irq: monitor done\r\n");
        break;
    case RUART_MODEM_STATUS:
        reg_val = UART_ModemStatusGet(HCI_UART_DEV);
        break;
    case RUART_TX_FIFO_EMPTY:
        transmit_chars(hci_adapter);
        break;
    case RUART_RECEIVER_DATA_AVAILABLE:
        receive_chars(hci_adapter, 1);
        break;
    case RUART_TIME_OUT_INDICATION:
        receive_chars(hci_adapter, 1);
        break;
    case RUART_RECEIVE_LINE_STATUS:
        reg_val = (UART_LineStatusGet(HCI_UART_DEV));
        hci_board_debug("hciuart_irq: LSR %08x interrupt\r\n", reg_val);
        if (reg_val & RUART_LINE_STATUS_ERR_OVERRUN)
        {
            hci_board_debug("hciuart_irq: LSR over run interrupt\r\n");
        }

        if (reg_val & RUART_LINE_STATUS_ERR_PARITY)
        {
            hci_board_debug("hciuart_irq: LSR parity error interrupt\r\n");
        }

        if (reg_val & RUART_LINE_STATUS_ERR_FRAMING)
        {
            hci_board_debug("hciuart_irq: LSR frame error(stop bit error) interrupt\r\n");
        }

        if (reg_val & RUART_LINE_STATUS_ERR_BREAK)
        {
            hci_board_debug("hciuart_irq: LSR break error interrupt\r\n");
        }

        /* if (reg_val & RUART_LINE_STATUS_REG_THRE)
         *     transmit_chars(hci_adapter);
         */
        break;

    default:
        hci_board_debug("hciuart_irq: Unknown interrupt type %u\r\n", int_id);
        break;
    }

    return 0;
}

bool hci_uart_tx(uint8_t *p_buf, uint16_t len, P_UART_TX_CB tx_cb)
{

#if 0
    UART_SendData(HCI_UART_DEV, p_buf, len);
    if (tx_cb)
        tx_cb();
    return true;
#else

    T_HCI_UART *uart_obj = hci_uart_obj;

    uart_obj->tx_len  = len;
    uart_obj->tx_cb   = tx_cb;


    uart_obj->tx_buf_cur = p_buf;
    uart_obj->tx_len_cur = len;

    if (!(uart_obj->ier & RUART_IER_ETBEI))
    {
        uart_obj->ier |= RUART_IER_ETBEI;
        UART_INTConfig(HCI_UART_DEV, RUART_IER_ETBEI, ENABLE);
    }
    else
    {
        hci_board_debug("hci_uart_tx: Transmitter FIFO empty interrupt has been enabled\r\n");
        return false;
    }
#endif
    return true;
}

bool hci_uart_malloc(void)
{
    if(hci_uart_obj == NULL)
    {
        hci_uart_obj = os_mem_zalloc(RAM_TYPE_DATA_ON, sizeof(T_HCI_UART)); //reopen not need init uart

        if(!hci_uart_obj)
        {
            hci_board_debug("hci_uart_malloc: need %d, left %d\r\n", sizeof(T_HCI_UART), os_mem_peek(RAM_TYPE_DATA_ON));
            return false;
        }
        else
        {
            //ok 
        }
    }
    else
    {
        hci_board_debug("hci_uart_malloc: rx_buffer not free\r\n");
        return false;
    }
    return true;
}

bool hci_uart_free(void)
{
    if(hci_uart_obj == NULL)
    {
        hci_board_debug("hci_uart_free: hci_uart_obj = NULL, no need free\r\n");
        return true;
    }
    else
    {
        os_mem_free(hci_uart_obj);
        hci_uart_obj = NULL;
    }
    return true;
}

void hci_uart_set_rx_ind(P_UART_RX_CB rx_ind)
{
	hci_uart_obj->rx_ind = rx_ind;
}

bool hci_uart_init(P_UART_RX_CB rx_ind)
{
    if(!hci_uart_malloc())
    {
        return false;
    }

    //malloc
#ifdef HCI_UART_OUT
    //PINMUX THE PIN
    Pinmux_Config(HCI_TX_PIN, PINMUX_FUNCTION_UART);
    Pinmux_Config(HCI_RX_PIN, PINMUX_FUNCTION_UART);
    Pinmux_Config(HCI_RTS_PIN, PINMUX_FUNCTION_UART_RTSCTS);
    Pinmux_Config(HCI_CTS_PIN, PINMUX_FUNCTION_UART_RTSCTS);

    PAD_PullCtrl(HCI_TX_PIN, GPIO_PuPd_UP);
    PAD_PullCtrl(HCI_RX_PIN, GPIO_PuPd_NOPULL);
#else
#endif
    UART_InitTypeDef    *pUARTStruct;
    pUARTStruct = &hci_uart_obj->UART_InitStruct;

    UART_StructInit(pUARTStruct);
    pUARTStruct->WordLen = RUART_WLS_8BITS;
    pUARTStruct->StopBit = RUART_STOP_BIT_1;
    pUARTStruct->Parity = RUART_PARITY_DISABLE;
    pUARTStruct->ParityType = RUART_EVEN_PARITY;
    pUARTStruct->StickParity = RUART_STICK_PARITY_DISABLE;

    pUARTStruct->RxFifoTrigLevel = UART_RX_FIFOTRIG_LEVEL_14BYTES;
    /* UART auto-flow control
     * When the data in UART FIFO reaches rx level, RTS will be pulled high */
    pUARTStruct->FlowControl = ENABLE;

    UART_Init(HCI_UART_DEV, pUARTStruct);
    UART_SetBaud(HCI_UART_DEV, 115200);

    InterruptDis(HCI_UART_IRQ);
    InterruptUnRegister(HCI_UART_IRQ);
    InterruptRegister((IRQ_FUN)hciuart_irq, HCI_UART_IRQ, (uint32_t)hci_uart_obj, HCIUART_IRQ_PRIO);
    InterruptEn(HCI_UART_IRQ, HCIUART_IRQ_PRIO);

    hci_uart_obj->ier = RUART_IER_ERBI | RUART_IER_ETOI | RUART_IER_ELSI;
    UART_INTConfig(HCI_UART_DEV, RUART_IER_ERBI | RUART_IER_ETOI | RUART_IER_ELSI, ENABLE);

    UART_RxCmd(HCI_UART_DEV, ENABLE);

    hci_uart_obj->rx_ind = rx_ind;
    return true;
}

bool hci_uart_deinit(void)
{
    //hardware deinit
    UART_DeInit(HCI_UART_DEV);
    InterruptDis(HCI_UART_IRQ);
    InterruptUnRegister(HCI_UART_IRQ);
#ifdef UART_TIMER

#endif

#ifdef HCI_UART_DMA

#endif
    hci_uart_free();
    return true;
    //free
}


uint16_t hci_uart_recv(uint8_t *p_buf, uint16_t size)
{
    uint16_t rx_len;
    
    T_HCI_UART *p_uart_obj = hci_uart_obj;
    //hci_board_debug("hci_uart_recv: write:%d, read:%d, rx_len:%d, need:%d, space_len:%d\r\n", p_uart_obj->rx_write_idx, p_uart_obj->rx_read_idx, hci_rx_data_len(), size, hci_rx_space_len());
    
    if(p_uart_obj == NULL)
    {
        hci_board_debug("hci_uart_recv: the p_uart_obj is NULL\r\n");
        return 0;
    }
    if(hci_rx_empty())
    {
         //rx empty
         return 0;
    }
    rx_len = hci_rx_data_len();
    
    if (rx_len > size)
    {
        rx_len = size;
    }

    if (rx_len > HCI_UART_RX_BUF_SIZE - p_uart_obj->rx_read_idx)    /* index overflow */
    {
        rx_len = HCI_UART_RX_BUF_SIZE - p_uart_obj->rx_read_idx;
    }

    if (rx_len)
    {
        memcpy(p_buf, &(p_uart_obj->rx_buffer[p_uart_obj->rx_read_idx]), rx_len);

        p_uart_obj->rx_read_idx += rx_len;
        p_uart_obj->rx_read_idx %= HCI_UART_RX_BUF_SIZE;

        if (p_uart_obj->rx_disabled == true)    /* flow control */
        {
            if (hci_rx_data_len() < HCI_UART_RX_ENABLE_COUNT)
            {
                hci_board_debug("hci_uart_recv: rx enable, data len %d\r\n", hci_rx_data_len());
                hci_uart_rx_enable(p_uart_obj);
            }
        }
    }

    return rx_len;
}


