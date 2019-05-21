#include "include.h"
#include "arm_arch.h"

#include "uart_pub.h"
#include "sdma_pub.h"
#include "sdma.h"

#include "mem_pub.h"
#include "intc_pub.h"

#if CFG_SDIO || CFG_SDIO_TRANS
TX_FUNC intr_tx_func = 0;
RX_FUNC intr_rx_func = 0;
CMD_FUNC intr_cmd_func = 0;

UINT8 *cmd_buf = 0;

volatile UINT32 cmd_data_dir = SDMA_DIR_IDLE;

void sdma_init(void)
{
    UINT32 reg;

#if CFG_REAL_SDIO
    intc_service_register(FIQ_SDIO_DMA, PRI_FIQ_SDIO_DMA, sdma_isr);
#endif

    reg = REG_READ(REG_SDMA_INTR_STATUS);
    REG_WRITE(REG_SDMA_INTR_STATUS, reg);
    sdma_clr_tx_valid();
}

void sdma_open(void)
{
    UINT32 reg;

    reg = SDMA_CONFIG_INTR_EN_TX
          | SDMA_CONFIG_INTR_EN_RX
          | SDMA_CONFIG_INTR_EN_CMD;
    REG_WRITE(REG_SDMA_CONFIG, reg);
}

void sdma_uninit(void)
{
}

void sdma_close(void)
{
    UINT32 reg;

    reg = REG_READ(REG_SDMA_CONFIG);
    reg &= ~(SDMA_CONFIG_INTR_EN_TX
             | SDMA_CONFIG_INTR_EN_RX
             | SDMA_CONFIG_INTR_EN_CMD);
    REG_WRITE(REG_SDMA_CONFIG, reg);
}

#ifdef SDMA_INTERACT_WITH_HOST
UINT32 sdma_get_blk_len(void)
{
    UINT32 reg;
    UINT32 power;
    UINT32 blk_len;

    reg = REG_READ(REG_SDMA_INTERACTIVE_HOST);
    power = (reg >> INTERACTIVE_BLK_LEN_POSI) & INTERACTIVE_BLK_LEN_MASK;

    blk_len = 2 << (power + 8);

    return blk_len;
}

void sdma_set_tx_valid(void)
{
    UINT32 reg;

    reg = REG_READ(REG_SDMA_INTERACTIVE_HOST);
    reg |= INTERACTIVE_RX_VALID_BIT;
    REG_WRITE(REG_SDMA_INTERACTIVE_HOST, reg);
}

void sdma_clr_tx_valid(void)
{
    UINT32 reg;

    reg = REG_READ(REG_SDMA_INTERACTIVE_HOST);
    reg &= ~INTERACTIVE_RX_VALID_BIT;
    REG_WRITE(REG_SDMA_INTERACTIVE_HOST, reg);
}

void sdma_set_tx_dat_count(UINT32 val)
{
    UINT32 reg;

    reg = REG_READ(REG_SDMA_INTERACTIVE_HOST);
    reg &= ~(INTERACTIVE_RX_COUNT_MASK << INTERACTIVE_RX_COUNT_POSI);
    reg |= (val & INTERACTIVE_RX_COUNT_MASK) << INTERACTIVE_RX_COUNT_POSI;
    REG_WRITE(REG_SDMA_INTERACTIVE_HOST, reg);
}
#endif // SDMA_INTERACT_WITH_HOST

void sdma_register_handler(TX_FUNC tx_callback,
                           RX_FUNC rx_callback,
                           CMD_FUNC cmd_callback)
{
    if(tx_callback)
    {
        intr_tx_func = tx_callback;
    }

    if(rx_callback)
    {
        intr_rx_func = rx_callback;
    }

    if(cmd_callback)
    {
        intr_cmd_func = cmd_callback;
    }

    return;
}

UINT32 sdma_dma_start(UINT8 *buf, UINT32 len)
{
    UINT32 reg;
    UINT32 addr;

    ASSERT(len);
    ASSERT(buf);

    addr  = (UINT32)buf;

    REG_WRITE(REG_SDMA_ADDR, addr);

    reg = REG_READ(REG_SDMA_CONFIG);
    reg |= SDMA_CONFIG_ADDR_READY;

    REG_WRITE(REG_SDMA_CONFIG, reg);

    return 0;
}

UINT32 sdma_start_tx(UINT8 *buf, UINT32 len)
{
    cmd_data_dir = SDMA_DIR_TX;

    return sdma_dma_start(buf, len);
}

UINT32 sdma_start_rx(UINT8 *buf, UINT32 len)
{
    cmd_data_dir = SDMA_DIR_RX;

    return sdma_dma_start(buf, len);
}

void sdma_fake_stop_dma(void)
{
    cmd_data_dir = SDMA_DIR_IDLE;
}

UINT32 sdma_start_cmd(UINT8 *cmd, UINT32 len)
{
    UINT32 reg;

    if(len != CMD_BUF_MAX_LEN)
    {
        return SDMA_FAILURE;
    }

    cmd_buf = cmd;

    /*startup dma*/
    reg = REG_READ(REG_SDMA_CONFIG);
    if(reg & SDMA_CONFIG_ADDR_READY)
    {
        return SDMA_BUSY;
    }

    reg |= SDMA_CONFIG_DMA_EN;
    REG_WRITE(REG_SDMA_CONFIG, reg);

    return 0;
}

void sdma_rd_cmd_content(void *buf, UINT32 len)
{
    UINT8 *src;

    if(len > CMD_BUF_MAX_LEN)
    {
        warning_prf("cmd_len_exception:%d\r\n", len);
        len = CMD_BUF_MAX_LEN;
    }

    src = (UINT8 *)REG_SDMA_CMD_BA;
    os_memcpy(buf, src, len);

    return;
}

void sdma_tx_isr_handler(UINT32 irq_status)
{
    if(irq_status & SDMA_INTR_TX_DATA_BIT)
    {
        SDMA_PRT("sdma_tx\r\n");
        REG_WRITE(REG_SDMA_INTR_STATUS, SDMA_INTR_TX_DATA_BIT);
        cmd_data_dir = SDMA_DIR_IDLE;
        if(intr_tx_func)
        {
            (*intr_tx_func)();
        }
    }
}

void sdma_rx_isr_handler(UINT32 irq_status)
{
    UINT32 rx_count;

    if(irq_status & SDMA_INTR_RX_DATA_BIT)
    {
        SDMA_PRT("sdma_rx\r\n");
        REG_WRITE(REG_SDMA_INTR_STATUS, SDMA_INTR_RX_DATA_BIT);

        cmd_data_dir = SDMA_DIR_IDLE;

        if(intr_rx_func)
        {
            rx_count = (REG_READ(REG_SDMA_COUNT)) & SDMA_COUNT_MASK;
            (*intr_rx_func)(rx_count);
        }
    }
}

void sdma_cmd_isr_handler(UINT32 irq_status)
{
    UINT32 rx_count;

    if(irq_status & SDMA_INTR_CMD_BIT)
    {
        SDMA_PRT("sdma_cmd\r\n");

        if(intr_cmd_func)
        {
            rx_count = (REG_READ(REG_SDMA_COUNT)) & SDMA_COUNT_MASK;

            sdma_rd_cmd_content(cmd_buf, rx_count);

            (*intr_cmd_func)(cmd_buf, rx_count);
        }
        REG_WRITE(REG_SDMA_INTR_STATUS, SDMA_INTR_CMD_BIT);
    }
}

void sdma_isr(void)
{
    UINT32 irq_sta;

    /*0, get isr status*/
    irq_sta = REG_READ(REG_SDMA_INTR_STATUS);

    /*1, handle isr branch*/
    if(SDMA_DIR_TX == cmd_data_dir)
    {
        sdma_tx_isr_handler(irq_sta);
        sdma_cmd_isr_handler(irq_sta);
        sdma_rx_isr_handler(irq_sta);
    }
    else if(SDMA_DIR_RX == cmd_data_dir)
    {
        sdma_rx_isr_handler(irq_sta);
        sdma_cmd_isr_handler(irq_sta);
        sdma_tx_isr_handler(irq_sta);
    }
    else
    {
        sdma_cmd_isr_handler(irq_sta);



        sdma_rx_isr_handler(irq_sta);
        sdma_tx_isr_handler(irq_sta);
    }

    if(!((irq_sta & SDMA_INTR_CMD_BIT)
            || (irq_sta & SDMA_INTR_RX_DATA_BIT)
            || (irq_sta & SDMA_INTR_TX_DATA_BIT)))
    {
        warning_prf("sdma:contrary to expectation:%x\r\n", irq_sta);
    }

    return;
}
#endif

// EOF
