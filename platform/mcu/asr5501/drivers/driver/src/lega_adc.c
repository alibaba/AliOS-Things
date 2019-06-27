#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include "lega_adc.h"
#include "lega_common.h"
#include "lega_gpio.h"

#if defined ADC_ENABLE

//void spi_mst_write(uint16_t addr, uint16_t data)
//{
//    uint32_t pre = 8<<28;
//    uint32_t start = 1 <<27;
//    uint32_t readnotwrite = 0 <<26;
//    uint32_t addr_t = addr <<16;
//    uint32_t data_t = data << 0;
//    uint32_t register_00 = pre | start  | readnotwrite | addr_t | data_t ;

//    uint32_t var=0;
//    *((volatile int *) SPI_COMMAND)= register_00;

//    do{
//        var =(*((volatile int *)SPI_COMMAND)) & (0x1 << 27);
//    }while(var);
//}

//uint16_t spi_mst_read(uint16_t addr)
//{
//    uint32_t pre = 8 << 28;
//    uint32_t start = 1 <<27;
//    uint32_t readnotwrite = 1 <<26;
//    uint32_t addr_t = addr <<16;
//    uint32_t data_t = 0;
//    uint32_t register_00 = pre | start  | readnotwrite | addr_t | data_t ;

//    uint32_t var=0;
//    *((volatile uint32_t *) SPI_COMMAND) = register_00;

//    do{
//        var =( *((volatile uint32_t *) SPI_COMMAND) ) & (0x1 << 27);
//    }while(var);

//    return(  (uint16_t)*((volatile uint32_t *) SPI_RDATA) );
//}

void adc_set_reg_bit(uint16_t reg, uint8_t start_bit, uint8_t len, uint16_t src_val)
{
    uint16_t tmp,mask,val;

    if((reg < 0xFF) && (start_bit < 16) && (len <= 16)&&(src_val<(1<<len)))
    {
        tmp = spi_mst_read(reg);

        mask = (1<<len)-1;          //1. clear dst bit. eg: len=4, mask = 0xf, 1111
        mask = ~(mask<<start_bit);  //~(mask<<4):0xff0f: 1111 1111 0000 1111

        val = tmp & mask;           //2.val =spi_read() & 0xff0f, clear [7:4]

        src_val = (src_val << start_bit);
        val = val | src_val;        //3. val spi_read & 0xff0f | val << 4

        spi_mst_write(reg,val);
    }
    else
    {
        return;
    }
}

uint16_t adc_get_reg_bit(uint16_t reg, uint8_t start_bit, uint8_t len)
{
    uint16_t mask,val;

    if((reg < 0xFF) && (start_bit < 16) && (len <= 16))
    {
        val = spi_mst_read(reg);    // 1. read reg val

        mask = (1<<len)-1;          //eg: len =4, 0xf,1111
        mask = mask << start_bit;   // 0x0f00;
        val = val & mask;           // 2. get dst bit

        val = (val >> start_bit);   // 3. ror

        return val;
    }
    else
    {
        return -1;
    }
}


/**
 * founction for adc sample
 * channnel pin which is set to adc sample.
 * reference voltage 1.6V  ADC resolution 12bit
 * @param[in]  channnel pin which is set to adc sample:rang0-7 (pad4-11)
 *
 * @return  value:adc sample data rang:0-4095, -1: if channel invalid
 */
int32_t lega_adc_get(lega_adc_dev_t *adc_config)
{
    uint16_t reg_f = 0;
    uint32_t clk_reg[5] = {0};
    uint32_t adc_base_reg[2] = {0};
    int32_t vol_value = 0;
//    float vol_val = 0;
    lega_gpio_dev_t config_gpio;

    if(adc_config->port > 7)
        return -1;
    config_gpio.port = adc_config->port+4;
    config_gpio.config = LEGA_INPUT_HIGH_IMPEDANCE;
    config_gpio.priv = NULL;
    lega_gpio_init(&config_gpio);

    //read clk reg value for restore
    clk_reg[0] = REG_PL_RD(SYS_REG_BASE_WIFI_CLK);
    clk_reg[1] = REG_PL_RD(SYS_REG_BASE_REF_ROOT_CLK);
    clk_reg[2] = REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR);
    //Enable WiFi core clock
    if(clk_reg[0] != 0x01)
    {
        REG_PL_WR(SYS_REG_BASE_WIFI_CLK,0x1);
        delay(50);
    }
    //wifi ref clk sel to 52MHz
    if(!(clk_reg[1] & 0x1))
    {
        REG_PL_WR(SYS_REG_BASE_REF_ROOT_CLK, REG_PL_RD(SYS_REG_BASE_REF_ROOT_CLK) | 0x1);
    }
    /* Enable RC clock */
    if(!(clk_reg[2] & (uint32_t)0x08000000))
    {
      REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR, (REG_PL_RD(MDM_CLKGATEFCTRL0_ADDR) & ~((uint32_t)0x08000000)) | ((uint32_t)1 << 27));
    }

    //read base reg
    adc_base_reg[0] = REG_PL_RD(SYS_REG_BASE_AUXADC);
    adc_base_reg[1] = REG_PL_RD(SYS_REG_BASE_XOCTRL2);
#if defined LEGA_A0V1 || defined LEGA_A0V2
    REG_PL_WR(SYS_REG_BASE_AUXADC, 0x040086CE);
    REG_PL_WR(SYS_REG_BASE_AUXADC, REG_PL_RD(SYS_REG_BASE_AUXADC) & (~(uint32_t)(BIT(9))));
#endif
//    //Enable AUXADC
    adc_set_reg_bit(0x06,14,1,0x0);
    //Enable XO CLK AUCADC, DFF's RB;D_RST_XO_CLK_AUXADC= 0
    reg_f = spi_mst_read(0x0f);
    spi_mst_write(0x0f,0x8000);
    //TRXTOP_LDO_BM, Tuning LDO output to 1.6V, default is 1.55V
    adc_set_reg_bit(0x1E,3,3,0x7);
    //Enable CLK AUXADC13M; D_XO_CLK_AUXADC13M_EN= 1
    adc_set_reg_bit(0x75,14,1,0x1);

    delay(500);

    //Open selected channel
#if defined LEGA_A0V1
    adc_set_reg_bit(0x23,4,4,0x0);
#elif defined LEGA_A0V2
    REG_PL_WR(SYS_REG_BASE_XOCTRL2, ((REG_PL_RD(SYS_REG_BASE_XOCTRL2)&(~(uint32_t)0x7))|(uint32_t)adc_config->port) | ((uint32_t)(BIT(9))));
#endif
//     lega_rtos_delay_milliseconds(1);
     delay(500);

#if defined LEGA_A0V1 || defined LEGA_A0V2
    REG_PL_WR(SYS_REG_BASE_AUXADC, REG_PL_RD(SYS_REG_BASE_AUXADC) | ((uint32_t)(BIT(9))));
    vol_value = (spi_mst_read(0xA4) & 0xFFF0) >> 4;
    printf("value is %ld\r\n",vol_value);
#endif
//    vol_val = 1.6 * (float)vol_value/4095;

    REG_PL_WR(SYS_REG_BASE_AUXADC,adc_base_reg[0]);
    REG_PL_WR(SYS_REG_BASE_XOCTRL2,adc_base_reg[1]);

    //Close AUXADC
    adc_set_reg_bit(0x06,14,1,0x1);
    //Close XO CLK AUCADC; D_RST_XO_CLK_AUXADC= 1
    spi_mst_write(0x0f,reg_f);
    //Close CLK AUXADC13M; D_XO_CLK_AUXADC13M_EN= 0
    adc_set_reg_bit(0x75,14,1,0x0);

    //restore clk reg
    REG_PL_WR(MDM_CLKGATEFCTRL0_ADDR,clk_reg[2]);
    REG_PL_WR(SYS_REG_BASE_REF_ROOT_CLK,clk_reg[1]);
    REG_PL_WR(SYS_REG_BASE_WIFI_CLK,clk_reg[0]);
    return vol_value;
}

#endif