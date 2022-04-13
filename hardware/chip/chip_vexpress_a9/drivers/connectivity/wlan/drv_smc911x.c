#include "realview.h"
#include <board.h>
#include <ethernetif.h>
#include <lwipopts.h>
#include <stdint.h>
#include "drv_smc911x.h"

#ifdef FEATURE_NETWORK_USPACE_VEXPRESS_TMP
#include "irq.h"
#include "irq/irqdesc.h"
#endif

#define MAX_ADDR_LEN                6
#define SMC911X_EMAC_DEVICE(eth)    (struct eth_device_smc911x*)(eth)

#define DRIVERNAME "EMAC"


struct eth_device_smc911x
{
    /* interface address info. */
    uint8_t enetaddr[MAX_ADDR_LEN];         /* MAC address  */

    uint32_t iobase;
    uint32_t irqno;

    /* network interface for lwip */
    struct netif *netif;
    aos_sem_t tx_ack;

    uint16_t flags;
    uint8_t  link_changed;
    uint8_t  link_status;
};

static struct eth_device_smc911x _emac;

#ifdef FEATURE_NETWORK_USPACE_VEXPRESS_TMP
#define mdelay(msec) aos_msleep(msec)
#define udelay(usec) aos_msleep(1)
#else
extern void udelay(unsigned long usec);
extern void mdelay(unsigned long msec);
#endif


static inline uint32_t smc911x_reg_read(uint32_t hw_base, uint32_t offset)
{
    return *(volatile uint32_t*)(hw_base + offset);
}

static inline void smc911x_reg_write(uint32_t hw_base, uint32_t offset, uint32_t val)
{
    *(volatile uint32_t*)(hw_base + offset) = val;
}

struct chip_id
{
    uint16_t id;
    char *name;
};

static const struct chip_id chip_ids[] =
{
    { CHIP_89218,"LAN89218" },
    { CHIP_9115, "LAN9115" },
    { CHIP_9116, "LAN9116" },
    { CHIP_9117, "LAN9117" },
    { CHIP_9118, "LAN9118" },
    { CHIP_9211, "LAN9211" },
    { CHIP_9215, "LAN9215" },
    { CHIP_9216, "LAN9216" },
    { CHIP_9217, "LAN9217" },
    { CHIP_9218, "LAN9218" },
    { CHIP_9220, "LAN9220" },
    { CHIP_9221, "LAN9221" },
    { 0, NULL },
};

static uint32_t smc911x_get_mac_csr(uint32_t hw_base, uint8_t reg)
{
    while (smc911x_reg_read(hw_base, MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY) ;

    smc911x_reg_write(hw_base, MAC_CSR_CMD, MAC_CSR_CMD_CSR_BUSY | MAC_CSR_CMD_R_NOT_W | reg);

    while (smc911x_reg_read(hw_base, MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY) ;

    return smc911x_reg_read(hw_base, MAC_CSR_DATA);
}

static void smc911x_set_mac_csr(uint32_t hw_base, uint8_t reg, uint32_t data)
{
    while (smc911x_reg_read(hw_base, MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY) ;

    smc911x_reg_write(hw_base, MAC_CSR_DATA, data);
    smc911x_reg_write(hw_base, MAC_CSR_CMD, MAC_CSR_CMD_CSR_BUSY | reg);

    while (smc911x_reg_read(hw_base, MAC_CSR_CMD) & MAC_CSR_CMD_CSR_BUSY) ;
}

static int smc911x_detect_chip(uint32_t hw_base)
{
    unsigned long val, i;

    val = smc911x_reg_read(hw_base, BYTE_TEST);
    if (val == 0xffffffff)
    {
        /* Special case -- no chip present */
        return -1;
    }
    else if (val != 0x87654321)
    {
        printf(DRIVERNAME ": Invalid chip endian 0x%08lx\n", val);
        return -1;
    }

    val = smc911x_reg_read(hw_base, ID_REV) >> 16;
    for (i = 0; chip_ids[i].id != 0; i++)
    {
        if (chip_ids[i].id == val) break;
    }

    if (!chip_ids[i].id)
    {
        printf(DRIVERNAME ": Unknown chip ID %04lx\n", val);
        return -1;
    }
    else {
        printf("found ethernet chip %s\n", chip_ids[i].name);
    }
    return 0;
}

static void smc911x_reset(uint32_t hw_base)
{
    int timeout;

    /*
    *  Take out of PM setting first
    *  Device is already wake up if PMT_CTRL_READY bit is set
    */
    if ((smc911x_reg_read(hw_base, PMT_CTRL) & PMT_CTRL_READY) == 0)
    {
        /* Write to the bytetest will take out of powerdown */
        smc911x_reg_write(hw_base, BYTE_TEST, 0x0);

        timeout = 10;

        while (timeout-- && !(smc911x_reg_read(hw_base, PMT_CTRL) & PMT_CTRL_READY))
            udelay(10);

        if (timeout < 0)
        {
            printf(DRIVERNAME
                       ": timeout waiting for PM restore\n");
            return;
        }
    }

    /* Disable interrupts */
    smc911x_reg_write(hw_base, INT_EN, 0);
    smc911x_reg_write(hw_base, HW_CFG, HW_CFG_SRST);

    timeout = 1000;
    while (timeout-- && smc911x_reg_read(hw_base, E2P_CMD) & E2P_CMD_EPC_BUSY)
        udelay(10);

    if (timeout < 0)
    {
        printf(DRIVERNAME ": reset timeout\n");
        return;
    }

    /* Reset the FIFO level and flow control settings */
    smc911x_set_mac_csr(hw_base, FLOW, FLOW_FCPT | FLOW_FCEN);
    smc911x_reg_write(hw_base, AFC_CFG, 0x0050287F);

    /* Set to LED outputs */
    smc911x_reg_write(hw_base, GPIO_CFG, 0x70070000);
}

static void smc911x_handle_mac_address(uint32_t hw_base, uint8_t *enetaddr)
{
    unsigned long addrh, addrl;
    uint8_t *m = enetaddr;

    addrl = m[0] | (m[1] << 8) | (m[2] << 16) | (m[3] << 24);
    addrh = m[4] | (m[5] << 8);

    smc911x_set_mac_csr(hw_base, ADDRL, addrl);
    smc911x_set_mac_csr(hw_base, ADDRH, addrh);
}

static int smc911x_eth_phy_read(uint32_t hw_base,
                                uint8_t phy, uint8_t reg, uint16_t *val)
{
    while (smc911x_get_mac_csr(hw_base, MII_ACC) & MII_ACC_MII_BUSY) ;

    smc911x_set_mac_csr(hw_base, MII_ACC, phy << 11 | reg << 6 | MII_ACC_MII_BUSY);

    while (smc911x_get_mac_csr(hw_base, MII_ACC) & MII_ACC_MII_BUSY) ;

    *val = smc911x_get_mac_csr(hw_base, MII_DATA);

    return 0;
}

static int smc911x_eth_phy_write(uint32_t hw_base,
                                 uint8_t phy, uint8_t reg, uint16_t  val)
{
    while (smc911x_get_mac_csr(hw_base, MII_ACC) & MII_ACC_MII_BUSY)
        ;

    smc911x_set_mac_csr(hw_base, MII_DATA, val);
    smc911x_set_mac_csr(hw_base, MII_ACC,
                        phy << 11 | reg << 6 | MII_ACC_MII_BUSY | MII_ACC_MII_WRITE);

    while (smc911x_get_mac_csr(hw_base, MII_ACC) & MII_ACC_MII_BUSY)
        ;
    return 0;
}

static int smc911x_phy_reset(uint32_t hw_base)
{
    uint32_t reg;

    reg = smc911x_reg_read(hw_base, PMT_CTRL);
    reg &= ~0xfffff030;
    reg |= PMT_CTRL_PHY_RST;
    smc911x_reg_write(hw_base, PMT_CTRL, reg);

    mdelay(100);

    return 0;
}

static void smc911x_phy_configure(uint32_t hw_base)
{
    int timeout;
    uint16_t status;

    smc911x_phy_reset(hw_base);

    smc911x_eth_phy_write(hw_base, 1, MII_BMCR, BMCR_RESET);
    mdelay(1);
    smc911x_eth_phy_write(hw_base, 1, MII_ADVERTISE, 0x01e1);
    smc911x_eth_phy_write(hw_base, 1, MII_BMCR, BMCR_ANENABLE | BMCR_ANRESTART);

    timeout = 5000;
    do
    {
        mdelay(1);
        if ((timeout--) == 0)
            goto err_out;

        if (smc911x_eth_phy_read(hw_base, 1, MII_BMSR, &status) != 0)
            goto err_out;
    }
    while (!(status & BMSR_LSTATUS));

    return;

err_out:
    printf(DRIVERNAME ": autonegotiation timed out\n");
}

static void smc911x_enable(uint32_t hw_base)
{
    /* Enable TX */
    smc911x_reg_write(hw_base, HW_CFG, 8 << 16 | HW_CFG_SF);

    smc911x_reg_write(hw_base, GPT_CFG, GPT_CFG_TIMER_EN | 10000);

    smc911x_reg_write(hw_base, TX_CFG, TX_CFG_TX_ON);

    /* no padding to start of packets */
    smc911x_reg_write(hw_base, RX_CFG, 0);

    smc911x_set_mac_csr(hw_base, MAC_CR, MAC_CR_TXEN | MAC_CR_RXEN |
                        MAC_CR_HBDIS);
}

void smc911x_isr(void)
{
    uint32_t status;
    struct eth_device_smc911x *emac = &_emac;

    status = smc911x_reg_read(emac->iobase, INT_STS);

    if (status & INT_STS_RSFL)
    {
        HAL_ETH_RxCpltCallback((void *)emac);
    }
    smc911x_reg_write(emac->iobase, INT_STS, status);

#ifdef FEATURE_NETWORK_USPACE_VEXPRESS_TMP
    extern void __irq_enable_svc(int irq);
    __irq_enable_svc(emac->irqno);
#endif
}

#if (defined(FEATURE_NETWORK_USPACE_VEXPRESS_TMP) && !defined(AOS_KERNEL_MODE))
irq_error irq_register_isr(uint16_t irq_number, isr_ptr callback)
{
    int ret;
    ret = aos_request_threaded_irq(irq_number, (irq_handler_t)callback, (irq_handler_t)callback, 0, 0, 0);
    if(ret == 0){
        return IRQ_OK;
    }else{
        return IRQ_INVALID_IRQ_ID;
    }
}

void gic_enable_interrupt(uint16_t number)
{
    aos_enable_irq(number);
}
#endif

int smc911x_emac_init(uint8_t *enetaddr)
{
    // uint32_t value;
    struct eth_device_smc911x *emac = &_emac;

    emac->iobase = VEXPRESS_ETH_BASE;
    emac->irqno  = IRQ_VEXPRESS_A9_ETH;

    if (smc911x_detect_chip(emac->iobase))
    {
        printf("no smc911x network interface found!\n");
        return -1;
    }

    /* set INT CFG */
    smc911x_reg_write(emac->iobase, INT_CFG, INT_CFG_IRQ_POL | INT_CFG_IRQ_TYPE);

    smc911x_reset(emac->iobase);

    /* Configure the PHY, initialize the link state */
    smc911x_phy_configure(emac->iobase);
    smc911x_handle_mac_address(emac->iobase, enetaddr);

    /* Turn on Tx + Rx */
    smc911x_enable(emac->iobase);

#if 1
    /* Interrupt on every received packet */
    smc911x_reg_write(emac->iobase, FIFO_INT, 0x01 << 8);
    smc911x_reg_write(emac->iobase, INT_EN, INT_EN_RDFL_EN | INT_EN_RSFL_EN);

    /* enable interrupt */
    smc911x_reg_write(emac->iobase, INT_CFG, INT_CFG_IRQ_EN | INT_CFG_IRQ_POL | INT_CFG_IRQ_TYPE);
#else

    /* disable interrupt */
    smc911x_reg_write(emac->iobase, INT_EN, 0);
    value = smc911x_reg_read(emac->iobase, INT_CFG);
    value &= ~INT_CFG_IRQ_EN;
    smc911x_reg_write(emac->iobase, INT_CFG, value);
#endif

    (void)irq_register_isr(emac->irqno, smc911x_isr);
    gic_enable_interrupt(emac->irqno);

    return 0;
}

/* Ethernet device interface */
/* transmit packet. */
static uint8_t tx_buf[2048];
int smc911x_emac_tx(struct pbuf* p)
{
    struct eth_device_smc911x *emac = &_emac;

    uint32_t *data;
    uint32_t tmplen;
    uint32_t status;
    uint32_t length;

    /* copy pbuf to a whole ETH frame */
    pbuf_copy_partial(p, tx_buf, p->tot_len, 0);

    /* send it out */
    data = (uint32_t*)tx_buf;
    length = p->tot_len;

    smc911x_reg_write(emac->iobase, TX_DATA_FIFO, TX_CMD_A_INT_FIRST_SEG | TX_CMD_A_INT_LAST_SEG | length);
    smc911x_reg_write(emac->iobase, TX_DATA_FIFO, length);

    tmplen = (length + 3) / 4;
    while (tmplen--)
    {
        smc911x_reg_write(emac->iobase, TX_DATA_FIFO, *data++);
    }

    /* wait for transmission */
    while (!((smc911x_reg_read(emac->iobase, TX_FIFO_INF) & TX_FIFO_INF_TSUSED) >> 16));

    /* get status. Ignore 'no carrier' error, it has no meaning for
     * full duplex operation
     */
    status = smc911x_reg_read(emac->iobase, TX_STATUS_FIFO) &
             (TX_STS_LOC | TX_STS_LATE_COLL | TX_STS_MANY_COLL |
              TX_STS_MANY_DEFER | TX_STS_UNDERRUN);
    if (!status) return 0;

    printf(DRIVERNAME ": failed to send packet: %s%s%s%s%s\n",
               status & TX_STS_LOC ? "TX_STS_LOC " : "",
               status & TX_STS_LATE_COLL ? "TX_STS_LATE_COLL " : "",
               status & TX_STS_MANY_COLL ? "TX_STS_MANY_COLL " : "",
               status & TX_STS_MANY_DEFER ? "TX_STS_MANY_DEFER " : "",
               status & TX_STS_UNDERRUN ? "TX_STS_UNDERRUN" : "");

    return -8;
}

/* reception packet. */
struct pbuf *smc911x_emac_rx(void)
{
    struct eth_device_smc911x *emac = &_emac;
    struct pbuf* p = NULL;

    /* take the emac buffer to the pbuf */
    if ((smc911x_reg_read(emac->iobase, RX_FIFO_INF) & RX_FIFO_INF_RXSUSED) >> 16)
    {
        uint32_t status;
        uint32_t pktlen, tmplen;

        status = smc911x_reg_read(emac->iobase, RX_STATUS_FIFO);

        /* get frame length */
        pktlen = (status & RX_STS_PKT_LEN) >> 16;

        smc911x_reg_write(emac->iobase, RX_CFG, 0);

        tmplen = (pktlen + 3) / 4;

        /* allocate pbuf */
        p = pbuf_alloc(PBUF_RAW, tmplen * 4, PBUF_RAM);
        if (p)
        {
            uint32_t *data = (uint32_t *)p->payload;
            while (tmplen--)
            {
                *data++ = smc911x_reg_read(emac->iobase, RX_DATA_FIFO);
            }
        }

        if (status & RX_STS_ES)
        {
            printf(DRIVERNAME ": dropped bad packet. Status: 0x%08x\n", status);
        }
    }

    return p;
}

#if 0
void ARM_A_network_uspace_vexpress_init(char *app_name, int pid) {
    // ARM-A not support io_map at usapce, so need map addr at kspace to support qemu-network run on uspace.
    #define NETWORK_USPACE_VEXPRESS_APP_NAME "ulwip_service"
    if (strncmp(app_name, NETWORK_USPACE_VEXPRESS_APP_NAME, sizeof(NETWORK_USPACE_VEXPRESS_APP_NAME)) == 0)
    {
        #define VEXPRESS_ETH_BASE           0x4E000000u  /* Ethernet */
        #define REALVIEW_USB_BASE           0x4F000000u  /* USB */
        int ret1 = os_mmu_mmap(VEXPRESS_ETH_BASE, VEXPRESS_ETH_BASE, REALVIEW_USB_BASE-VEXPRESS_ETH_BASE, MMU_ATTR_SO, pid, 0);
        printf("at %s: os_mmu_mmap ret = %d. Map ulwip service task(pid=%d) finished. start=%p, end=%p\r\n",
            __func__, ret1, pid, VEXPRESS_ETH_BASE, REALVIEW_USB_BASE);
    } else {
        printf("proc->preamble.app_name=%s, not os_mmu_mmap VEXPRESS_ETH_BASE.\r\n", __func__, app_name);
    }
}
#endif
