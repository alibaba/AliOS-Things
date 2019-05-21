#include "include.h"
#include "arm_arch.h"

#include "spi.h"
#include "spi_pub.h"

#include "drv_model_pub.h"
#include "intc_pub.h"
#include "mcu_ps_pub.h"

static spi_message_t spi_msg;
volatile spi_trans_t spi_trans;

static SDD_OPERATIONS spi_op =
{
    spi_ctrl
};

static void spi_active(BOOLEAN val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~SPIEN;
    }
    else if(val == 1)
    {
        value |= SPIEN;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_set_msten(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~MSTEN;
    }
    else if(val == 1)
    {
        value |= MSTEN;
    }
    REG_WRITE(SPI_CTRL, value);
    spi_msg.master = val;
}

static void spi_set_ckpha(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~CKPHA;
    }
    else if(val == 1)
    {
        value |= CKPHA;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_set_skpol(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~CKPOL;
    }
    else if(val == 1)
    {
        value |= CKPOL;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_set_bit_wdth(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~BIT_WDTH;
    }
    else if(val == 1)
    {
        value |= BIT_WDTH;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_set_nssmd(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    value &= ~(NSSMD_MASK << NSSMD_POSI);
    value |= (val << NSSMD_POSI);
    REG_WRITE(SPI_CTRL, value);
    spi_msg.nssmd = val;
}

static void spi_set_ckr(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    value &= ~(SPI_CKR_MASK << SPI_CKR_POSI);
    value |= (val << SPI_CKR_POSI);
    REG_WRITE(SPI_CTRL, value);
}

static void spi_rxint_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~RXINT_EN;
    }
    else if(val == 1)
    {
        value |= RXINT_EN;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_txint_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~TXINT_EN;
    }
    else if(val == 1)
    {
        value |= TXINT_EN;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_rxovr_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~RXOVR_EN;
    }
    else if(val == 1)
    {
        value |= RXOVR_EN;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_txovr_enable(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~TXOVR_EN;
    }
    else if(val == 1)
    {
        value |= TXOVR_EN;
    }
    REG_WRITE(SPI_CTRL, value);
}

static void spi_rxfifo_clr(void)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    value |= RXFIFO_CLR;
    REG_WRITE(SPI_CTRL, value);
}

static void spi_txfifo_clr(void)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    value |= RXFIFO_CLR;
    REG_WRITE(SPI_CTRL, value);
}

static void spi_rxint_mode(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~RXINT_MODE;
    }
    else if(val == 1)
    {
        value |= RXINT_MODE;
    }
    REG_WRITE(SPI_CTRL, value);
    spi_msg.rxint_mode = val;
}

static void spi_txint_mode(UINT8 val)
{
    UINT32 value;

    value = REG_READ(SPI_CTRL);
    if(val == 0)
    {
        value &= ~TXINT_MODE;
    }
    else if(val == 1)
    {
        value |= TXINT_MODE;
    }
    REG_WRITE(SPI_CTRL, value);
    spi_msg.txint_mode = val;
}

static void spi_gpio_configuration()
{
}

static void spi_icu_configuration()
{
}

static void spi_trans_start(void)
{
    UINT32 value;

    if (spi_msg.nssmd == 3)
    {
        value = REG_READ(SPI_CTRL);
        value &= ~(0x01UL << NSSMD_POSI);
        REG_WRITE(SPI_CTRL, value);
    }
}

static void spi_init_msten(UINT8 msten)
{
    UINT32 value = 0;

    spi_icu_configuration();
    spi_gpio_configuration();

    if(msten == slave)
    {
        value = SPIEN
                | (0x01UL << NSSMD_POSI)
                | (0x07UL << SPI_CKR_POSI)
                | RXINT_EN
                | TXINT_EN
                | RXOVR_EN
                | TXOVR_EN
                | RXFIFO_CLR
                | TXFIFO_CLR
                | RXINT_MODE
                | TXINT_MODE;
        spi_msg.master = 0;
        spi_msg.nssmd = 1;
    }
    else if(msten == master)
    {
        value = SPIEN
                | MSTEN
                | (0x03UL << NSSMD_POSI)
                | (0x07UL << SPI_CKR_POSI)
                | RXINT_EN
                | TXINT_EN
                | RXOVR_EN
                | TXOVR_EN
                | RXFIFO_CLR
                | TXFIFO_CLR
                | RXINT_MODE
                | TXINT_MODE;
        spi_msg.master = 1;
        spi_msg.nssmd = 3;
    }
    REG_WRITE(SPI_CTRL, value);
    spi_msg.txint_mode = 1;
    spi_msg.rxint_mode = 1;
}

static UINT8 spi_get_busy(void)
{
    UINT32 value;

    value = REG_READ(SPI_STAT);
    if(value & SPIBUSY)
    {
        return 1;
    }
    if(spi_trans.trans_done == 0)
    {
        return 1;
    }
    return 0;
}

void spi_init(void)
{
    intc_service_register(IRQ_SPI, PRI_IRQ_SPI, spi_isr);

    sddev_register_dev(SPI_DEV_NAME, &spi_op);
}

void spi_exit(void)
{
    sddev_unregister_dev(SPI_DEV_NAME);
}

UINT32 spi_ctrl(UINT32 cmd, void *param)
{
    UINT32 ret = SPI_SUCCESS;

    peri_busy_count_add();

    switch(cmd)
    {
    case CMD_SPI_UNIT_ENABLE:
        spi_active(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_MSTEN:
        spi_set_msten(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_CKPHA:
        spi_set_ckpha(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_CKPOL:
        spi_set_skpol(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_BITWIDTH:
        spi_set_bit_wdth(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_NSSID:
        spi_set_nssmd(*(UINT8 *)param);
        break;
    case CMD_SPI_SET_CKR:
        spi_set_ckr(*(UINT8 *)param);
        break;
    case CMD_SPI_RXINT_EN:
        spi_rxint_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_TXINT_EN:
        spi_txint_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_RXOVR_EN:
        spi_rxovr_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_TXOVR_EN:
        spi_txovr_enable(*(UINT8 *)param);
        break;
    case CMD_SPI_RXFIFO_CLR:
        spi_rxfifo_clr();
        break;
    case CMD_SPI_TXFIFO_CLR:
        spi_txfifo_clr();
        break;
    case CMD_SPI_RXINT_MODE:
        spi_rxint_mode(*(UINT8 *)param);
        break;
    case CMD_SPI_TXINT_MODE:
        spi_txint_mode(*(UINT8 *)param);
        break;
    case CMD_SPI_START_TRANS:
        spi_trans_start();
        break;
    case CMD_SPI_INIT_MSTEN:
        spi_init_msten(*(UINT8 *)param);
        break;
    case CMD_SPI_GET_BUSY:
        (*((UINT8 *)param)) = spi_get_busy();
    default:
        ret = SPI_FAILURE;
        break;
    }

    peri_busy_count_dec();

    return ret;
}

void spi_isr(void)
{
    UINT8 i;
    UINT8 txint, rxint;
    UINT32 ctrl, status;
    volatile UINT8 fifo_empty_num, data_num, rxfifo_empty;

	data_num = 0; /*fix warning by clang analyzer*/
	fifo_empty_num = 0; /*fix warning by clang analyzer*/
	
    ctrl = REG_READ(SPI_CTRL);
    status = REG_READ(SPI_STAT);

    txint = status & TXINT;
    rxint = status & RXINT;

    if(txint)
    {
        if(spi_trans.tx_remain_data_cnt == 0)
        {
            status &= ~TXINT;
            if(spi_msg.nssmd == 3)
            {
                ctrl |= (0x03UL << NSSMD_POSI);
            }
            spi_trans.trans_done = 1;
        }
        else
        {
            if(spi_msg.txint_mode == 1)
            {
                fifo_empty_num = 8;
            }
            else if(spi_msg.txint_mode == 0)
            {
                fifo_empty_num = 1;
            }

            if(spi_trans.tx_remain_data_cnt > fifo_empty_num)
            {
                data_num = fifo_empty_num;
            }
            else
            {
                data_num = spi_trans.tx_remain_data_cnt;
            }

            for(i = 0; i < data_num; i++)
            {
                REG_WRITE(SPI_DAT, *spi_trans.p_tx_buf);
                spi_trans.p_tx_buf++;
                spi_trans.tx_remain_data_cnt--;
            }

            if(spi_trans.tx_remain_data_cnt == 0)
            {
                ctrl &= ~TXINT_MODE;
                spi_msg.txint_mode = 0;
            }
        }
    }

    if(rxint)
    {
        if(spi_msg.rxint_mode == 0)
        {
            data_num = 1;
        }
        else if(spi_msg.rxint_mode == 1)
        {
            data_num = 8;
        }

        rxfifo_empty = status & RXFIFO_EMPTY;
        while(!rxfifo_empty)
        {
            *spi_trans.p_rx_buf = REG_READ(SPI_DAT);
            spi_trans.p_rx_buf++;
            spi_trans.rx_remain_data_cnt--;
            rxfifo_empty = status & RXFIFO_EMPTY;
        }

        if(spi_msg.rxint_mode && (spi_trans.rx_remain_data_cnt < data_num))
        {
            ctrl &= ~RXINT_MODE;
            spi_msg.rxint_mode = 0;
        }

        if(spi_trans.rx_remain_data_cnt == 0)
        {
            if(spi_msg.nssmd == 3)
            {
                ctrl |= (0x03UL << NSSMD_POSI);
            }
            spi_trans.trans_done = 1;
        }
    }
	
    REG_WRITE(SPI_CTRL, ctrl);
    REG_WRITE(SPI_STAT, status);
}

// eof 

