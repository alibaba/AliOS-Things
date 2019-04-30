#include "rda_ccfg_api.h"
#include "rda5981.h"

#define CLK_FREQ_40M            (0x00U)
#define CLK_FREQ_80M            (0x01U)
#define CLK_FREQ_160M           (0x02U)
#define ADDR2REG(addr)          (*((volatile unsigned int *)(addr)))

#define RF_SPI_REG              ADDR2REG(0x4001301CUL)
#define TRAP_CTRL_REG           ADDR2REG(0x40011000UL)
#define TRAP0_SRC_REG           ADDR2REG(0x40011004UL)
#define TRAP0_DST_REG           ADDR2REG(0x40011024UL)
#define TRAP1_SRC_REG           ADDR2REG(0x40011008UL)
#define TRAP1_DST_REG           ADDR2REG(0x40011028UL)
#define SPIF_CFG_REG            ADDR2REG(0x17FFF014UL)
#define MACHW_RST_REG           ADDR2REG(0x4002003CUL)

#define TRAP_RAM_BASE           (0x00100000UL)
#define SYS_CPU_CLK             CLK_FREQ_160M
#define AHB_BUS_CLK             CLK_FREQ_80M

static int ChipHwVersion = 0;

static inline void wr_rf_usb_reg(unsigned char a, unsigned short d, int isusb)
{
    while(RF_SPI_REG & (0x01UL << 31));
    while(RF_SPI_REG & (0x01UL << 31));
    RF_SPI_REG = (unsigned int)d | ((unsigned int)a << 16) | (0x01UL << 25) | ((isusb) ? (0x01UL << 27) : 0x00UL);
}

static inline void rd_rf_usb_reg(unsigned char a, unsigned short *d, int isusb)
{
    while(RF_SPI_REG & (0x01UL << 31));
    while(RF_SPI_REG & (0x01UL << 31));
    RF_SPI_REG = ((unsigned int)a << 16) | (0x01UL << 24) | (0x01UL << 25) | ((isusb) ? (0x01UL << 27) : 0x00UL);
    __asm volatile ("nop");
    while(RF_SPI_REG & (0x01UL << 31));
    while(RF_SPI_REG & (0x01UL << 31));
    *d = (unsigned short)(RF_SPI_REG & 0xFFFFUL);
}

#if RDA5991H_HW_VER == 1
/* Config UART0 RX pin */
static inline int rda_ccfg_gp26(void)
{
    int ret = 0;
    unsigned short val = 0U;
    rd_rf_usb_reg(0xCD, &val, 0);
    ret = (int)((val >> 6) & 0x01U);
    if(!ret) {
        wr_rf_usb_reg(0xCD, (val | (0x01U << 6)), 0);
    }
    return ret;
}

/* Config USB */
static inline void rda_ccfg_usb(void)
{
    wr_rf_usb_reg(0x89, 0xEEDD, 1);
}

/* Config PMU */
static inline void rda_ccfg_pmu(void)
{
    unsigned short val = 0U;
    rd_rf_usb_reg(0xC0, &val, 0);
    wr_rf_usb_reg(0xC0, (val | (0x01U << 2)), 0); /* set sido_ctrl_comp2 */
}
#endif /* RDA5991H_HW_VER */

/* Power down the debug-usage I2C */
static inline void rda_ccfg_pdi2c(void)
{
    unsigned int val = RDA_PINCFG->MUX2;
    if(0x00UL == (val & 0x3FUL)) {
        RDA_PINCFG->MUX2 = val | 0x09UL;
    }
    wr_rf_usb_reg(0xA1, 0x0000, 0);
}

/* Config CPU & Bus clock */
static inline void rda_ccfg_ck(void)
{
    unsigned short val = 0U, cfg = 0U;
#if ((SYS_CPU_CLK == CLK_FREQ_160M) && (AHB_BUS_CLK == CLK_FREQ_80M))
#if 0
    const unsigned int trap_ram_code[] = {
        /* addr_ofst, val */
        /* 0x0000 */  0xB8A6F0FEUL,
        /* 0x0004 */  0xBF006008UL,
        /* 0x0008 */  0xBF00BF00UL,
        /* 0x000C */  0xBF00BF00UL,
        /* 0x0010 */  0xBF00BF00UL,
        /* 0x0014 */  0x6848BF00UL,
        /* 0x0018 */  0xD1FC07C0UL,
        /* 0x001C */  0xF000F8DFUL,
        /* 0x0020 */  0x00001EBCUL
    };
#endif
#endif /* CLK_FREQ_160M && CLK_FREQ_80M */

    cfg = (RDA_SCU->CORECFG >> 11) & 0x07U;
    rd_rf_usb_reg(0xA4, &val, 0);
#if ((SYS_CPU_CLK == CLK_FREQ_160M) && (AHB_BUS_CLK == CLK_FREQ_80M))
    /* HCLK inv */
    if(((CLK_FREQ_40M << 1) | CLK_FREQ_40M) == cfg) {
        val |=  (0x01U << 12);
    }
#endif /* CLK_FREQ_160M && CLK_FREQ_80M */
    /* Config CPU & BUS clock */
    cfg ^=  (((SYS_CPU_CLK << 1) | AHB_BUS_CLK) & 0x07U);
    val &= ~(0x07U << 9);   /* bit[11:10] = 2'b00:40M, 2'b01:80M, 2'b1x:160M */
    val |=  (cfg   << 9);   /* bit[9] = 1'b0:40M, 1'b1:80M */
    val &= ~(0x01U);        /* i2c_wakeup_en */
    wr_rf_usb_reg(0xA4, val, 0);

#if ((SYS_CPU_CLK == CLK_FREQ_160M) && (AHB_BUS_CLK == CLK_FREQ_80M))
#if 0
    /* Load RAM code */
    for(val = 0; val < sizeof(trap_ram_code); val += 4) {
        ADDR2REG(TRAP_RAM_BASE + val) = trap_ram_code[val >> 2];
    }
#endif
#endif /* CLK_FREQ_160M && CLK_FREQ_80M */

#if RDA5991H_HW_VER == 1
    /* Trap baud rate config for bootrom */
    /* Matrix between BusClk & TrapVal:  */
    /*                                   */
    /* -----+---------+--------+-------- */
    /*  C\T | 0x2814  | 0x2834 | 0x282C  */
    /* -----+---------+--------+-------- */
    /*  40M | 921600  | 460800 | 230400  */
    /* -----+------------------+-------- */
    /*  80M | 1843200 | 921600 | 460800  */
    /* -----+---------+--------+-------- */
    /*                                   */
    TRAP0_SRC_REG  = 0x00001CA4UL;
#if (AHB_BUS_CLK == CLK_FREQ_80M)
    TRAP0_DST_REG  = 0x0000282CUL;
#else  /* CLK_FREQ_80M */
    TRAP0_DST_REG  = 0x00002834UL;
#endif /* CLK_FREQ_80M */

#if ((SYS_CPU_CLK == CLK_FREQ_160M) && (AHB_BUS_CLK == CLK_FREQ_80M))
    /* Trap simple delay after ICACHE en */
    TRAP1_SRC_REG  = 0x00001EB4UL;
#if 0
    TRAP1_DST_REG  = TRAP_RAM_BASE + 0x0000UL;
#else
    TRAP1_DST_REG  = 0x00005A8CUL;
#endif

    /* Enable Trap0, Trap1 */
    TRAP_CTRL_REG |= (0x01UL | (0x01UL << 1));
#else  /* CLK_FREQ_160M && CLK_FREQ_80M */
    /* Enable Trap0 */
    TRAP_CTRL_REG |= 0x01UL;
#endif /* CLK_FREQ_160M && CLK_FREQ_80M */
#endif /* RDA5991H_HW_VER */
}

/* Config SPI flash clock */
static inline void rda_ccfg_spifck(void)
{
    unsigned int val;
    __DSB();
    val = SPIF_CFG_REG & ~(0x00FFUL << 8);
    SPIF_CFG_REG = val |  (0x0004UL << 8); // divider
    __DSB();
}

/* Handle abort booting */
static inline int rda_ccfg_abort_hdlr(void)
{
    int ret = 0;
    unsigned short val = 0U;
    rd_rf_usb_reg(0xA1, &val, 0);
    ret = (int)((val >> 2) & 0x01U);
    if(ret) {
        unsigned short val2 = 0U;
        rd_rf_usb_reg(0xB2, &val2, 0);
        wr_rf_usb_reg(0xB2, (val2 | (0x01U << 11)), 0);
        RDA_GPIO->PCCTRL |= (0x01UL << 31);  // set abort flag
        for(val = 0; val < 0x00FFU; val++) { // delay
            ;
        }
        wr_rf_usb_reg(0xB2, (val2 & ~(0x01U << 11)), 0);
    }
    return ret;
}

/* Power up the always-on timer */
void rda_ccfg_aontmr(void)
{
    unsigned short val = 0U;
    rd_rf_usb_reg(0xA3, &val, 0);
    wr_rf_usb_reg(0xA3, (val | (0x01U << 12)), 0);
}

/* Config clock source of always-on timer */
void rda_ccfg_aontmr_cksrc(int cksrc)
{
    unsigned short val;
    if(0 == cksrc) { // use lpo 32K clk, hw default
        wr_rf_usb_reg(0xDD, 0x5100U, 0);
        rd_rf_usb_reg(0xD8, &val, 0);
        wr_rf_usb_reg(0xD8, (val & ~(0x03U << 14)), 0);
        rd_rf_usb_reg(0xA8, &val, 0);
        val |= ((0x01U << 10) | (0x01U << 14));
        wr_rf_usb_reg(0xA8, (val & ~(0x01U << 12)), 0);
    } else { // use 6m5xtal 32K clk, more accurate
        int idx;
        wr_rf_usb_reg(0xDD, 0x8100U, 0);
        rd_rf_usb_reg(0xD8, &val, 0);
        wr_rf_usb_reg(0xD8, (val | (0x01U << 15)), 0);
        for(idx = 0; idx < 5; idx++) { // for dealy
            rd_rf_usb_reg(0x00, &val, 0);
        }
        wr_rf_usb_reg(0xDD, 0x9100U, 0);
        rd_rf_usb_reg(0xD8, &val, 0);
        wr_rf_usb_reg(0xD8, (val | (0x01U << 15) | (0x01U << 14)), 0);
        rd_rf_usb_reg(0xA8, &val, 0);
        val &= ~((0x01U << 10) | (0x01U << 14));
        wr_rf_usb_reg(0xA8, (val | (0x01U << 12)), 0);
    }
}

/* Config GPIO6 to dig core */
void rda_ccfg_gp6(unsigned short cfg)
{
    unsigned short val = 0U;
    rd_rf_usb_reg(0xCD, &val, 0);
    val &= ~(0x01U << 11);
    wr_rf_usb_reg(0xCD, (val | ((cfg & 0x01U) << 11)), 0);
}

/* Config GPIO7 to dig core */
void rda_ccfg_gp7(unsigned short cfg)
{
    unsigned short val = 0U;
    rd_rf_usb_reg(0xB0, &val, 0);
    val &= ~(0x01U << 14);
    wr_rf_usb_reg(0xB0, (val | ((cfg & 0x01U) << 14)), 0);
}

/* Config GPIO6/7/8/9 to pmu intf or dig core */
void rda_ccfg_gp(unsigned char gp, unsigned short cfg)
{
    unsigned short val = 0U;
    unsigned char  reg = 0xCDU;
    const int ofs_lst[4] = {11, 14, 10, 9};
    if((6 > gp) || (9 < gp)) {
        return;
    }
    if(7 == gp) {
        reg = 0xB0U;
    }
    rd_rf_usb_reg(reg, &val, 0);
    val &= ~(0x01U << ofs_lst[gp - 6]);
    wr_rf_usb_reg(reg, (val | ((cfg & 0x01U) << ofs_lst[gp - 6])), 0);
}

/* Set some core config when booting */
int rda_ccfg_boot(void)
{
    int ret = 1;
    int abort_flag = rda_ccfg_abort_hdlr();

#if RDA5991H_HW_VER == 1
    if(abort_flag) {
        rda_ccfg_ck();
    } else {
        int gp26_flag = rda_ccfg_gp26();
        if(!gp26_flag) {
            rda_ccfg_usb();
            rda_ccfg_pmu();
            rda_ccfg_pdi2c();
            rda_ccfg_ck();
        }
        ret = gp26_flag;
    }
#else  /* RDA5991H_HW_VER */
    if(!abort_flag) {
        rda_ccfg_pdi2c();
    }
    rda_ccfg_ck();
#endif /* RDA5991H_HW_VER */
    /*close usb polling*/
    RDA_GPIO->CTRL &= ~(0x01UL << 12);
    /* Set flash clock */
    rda_ccfg_spifck();
    /* Set aon timer clock source */
    rda_ccfg_aontmr_cksrc(1);
    return ret;
}

/* Reset CPU & Bus clock config */
void rda_ccfg_ckrst(void)
{
    unsigned short val = 0U;

    rd_rf_usb_reg(0xA4, &val, 0);
    /* HCLK inv */
    val &= ~(0x01U << 12);
    /* Config CPU clock */
    val &= ~(0x03U << 10);
    val |=  (0x00U << 10); /* 2'b00:40M, 2'b01:80M, 2'b1x:160M */
    /* Config BUS clock */
    val &= ~(0x01U << 9);
    val |=  (0x00U << 9);  /* 1'b0:40M, 1'b1:80M */
    wr_rf_usb_reg(0xA4, val, 0);
}

/* Reset peripheral module */
void rda_ccfg_perrst(void)
{
    MACHW_RST_REG ^= 0x01UL;
}

/* Init ADC module */
void rda_ccfg_adc_init(void)
{
    unsigned short val = 0U;
    rd_rf_usb_reg(0xA3, &val, 0); /* adc en */
    wr_rf_usb_reg(0xA3, (val | (0x01U << 3)), 0);
    rd_rf_usb_reg(0xD8, &val, 0); /* clk 6p5m en */
    wr_rf_usb_reg(0xD8, (val | (0x01U << 15)), 0);
    rd_rf_usb_reg(0xB7, &val, 0); /* clk 26m en */
    wr_rf_usb_reg(0xB7, (val | (0x01U << 14)), 0);
}

/* Config GPIO6/7/8 pdn or pup for ADC usage */
void rda_ccfg_adc_gp(unsigned char gp, unsigned short cfg)
{
    unsigned short val = 0U;
    const int ofs_lst[3] = {8, 9, 7};
    if((6 > gp) || (8 < gp)) {
        return;
    }
    rd_rf_usb_reg(0xB2, &val, 0);
    val &= ~(0x01U << ofs_lst[gp - 6]);
    wr_rf_usb_reg(0xB2, (val | ((cfg & 0x01U) << ofs_lst[gp - 6])), 0);
}

/* Read ADC value */
unsigned short rda_ccfg_adc_read(unsigned char ch)
{
    unsigned short val = 0U;
    rd_rf_usb_reg(0xB6, &val, 0); /* channel select */
    val &= ~((0x03U) << 12);
    wr_rf_usb_reg(0xB6, (val | ((ch & 0x03U) << 12)), 0);

    rd_rf_usb_reg(0xB6, &val, 0); /* set read en */
    wr_rf_usb_reg(0xB6, (val | (0x01U << 2)), 0);
    for(val = 0; val < 0x0FFU; val++) { // delay
        ;
    }
    rd_rf_usb_reg(0xB6, &val, 0); /* clr read en */
    wr_rf_usb_reg(0xB6, (val & ~(0x01U << 2)), 0);

    do {
        rd_rf_usb_reg(0xB7, &val, 0); /* finish loop flag */
    } while(0x00U == (val & (0x01U << 10)));
    return (val & 0x03FFU);
}

/* Free ADC module */
void rda_ccfg_adc_free(void)
{
    unsigned short val = 0U;
    rd_rf_usb_reg(0xA3, &val, 0); /* adc disable */
    wr_rf_usb_reg(0xA3, (val & ~(0x01U << 3)), 0);
    //rd_rf_usb_reg(0xD8, &val, 0); /* clk 6p5m disable */
    //wr_rf_usb_reg(0xD8, (val & ~(0x01U << 15)), 0);
    rd_rf_usb_reg(0xB7, &val, 0); /* clk 26m disable */
    wr_rf_usb_reg(0xB7, (val & ~(0x01U << 14)), 0);
}

/* Get abort flag */
int rda_ccfg_abort_flag(void)
{
    int ret = 0;
    if(0x00UL != (RDA_GPIO->PCCTRL & (0x01UL << 31))) {
        ret = 1;
    }
    return ret;
}

#if RDA5991H_HW_VER >= 2
/* Set wdt en */
void rda_ccfg_wdt_en(void)
{
    unsigned short val = 0U;
    rd_rf_usb_reg(0xC8, &val, 0);
    wr_rf_usb_reg(0xC8, (val | (0x01U << 13)), 0);
}
#endif /* RDA5991H_HW_VER */

unsigned short rf_reg_read(unsigned short addr)
{
    unsigned short val = 0U;
    if(addr <= 0x1FF) {
        if((((addr & 0xFFU) >= 0xA0) && ((addr & 0xFFU) <= 0xDF)) ||
            (addr == 0x30U) || (addr == 0x34U) || (addr == 0x35U))  { /* PMU & RF_30H/34H/35H */
            rd_rf_usb_reg((unsigned char)(addr & 0xFFU), &val, 0);
        } else { /* RF */
            char isrun = 0;
            rd_rf_usb_reg(0x30U, &val, 0);
            isrun = (val & 0x01U) ? 1 : 0;
            if(isrun) {
                wr_rf_usb_reg(0x02U, (0x5000U | addr), 0);
                rd_rf_usb_reg(0x34U, &val, 0);
            } else {
                if(addr & (0x01U << 8)) {
                    wr_rf_usb_reg(0x3FU, 1, 0);
                }
                rd_rf_usb_reg((unsigned char)(addr & 0xFFU), &val, 0);
                if(addr & (0x01U << 8)) {
                    wr_rf_usb_reg(0x3FU, 0, 0);
                }
            }
        }
    }
    return val;
}

void rf_reg_write(unsigned short addr, unsigned short val)
{
    if(addr <= 0x1FF) {
        if((((addr & 0xFFU) >= 0xA0) && ((addr & 0xFFU) <= 0xDF)) ||
            (addr == 0x30U) || (addr == 0x34U) || (addr == 0x35U)) { /* PMU & RF_30H/34H/35H */
            wr_rf_usb_reg((unsigned char)(addr & 0xFFU), val, 0);
        } else { /* RF */
            char isrun = 0;
            rd_rf_usb_reg(0x30U, &val, 0);
            isrun = (val & 0x01U) ? 1 : 0;
            if(isrun) {
                wr_rf_usb_reg(0x32U, val, 0);
                wr_rf_usb_reg(0x02U, (0x4000U | addr), 0);
            } else {
                if(addr & (0x01U << 8)) {
                    wr_rf_usb_reg(0x3FU, 1, 0);
                }
                wr_rf_usb_reg((unsigned char)(addr & 0xFFU), val, 0);
                if(addr & (0x01U << 8)) {
                    wr_rf_usb_reg(0x3FU, 0, 0);
                }
            }
        }
    }
}

/* Get chip hw version */
int rda_ccfg_hwver(void)
{
    if(0 == ChipHwVersion) {
        ChipHwVersion = (int)((RDA_GPIO->REVID >> 16) & 0xFFUL) + 1;
    }
    return ChipHwVersion;

}

