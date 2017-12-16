#include "drv_common.h"
#include "soc.h"
#include "pin_name.h"
#include "spu_pin_planning.h"
#include "pinmux.h"

#ifdef CONFIG_SPU

//==============================================================
//                           SPU->DEV
//==============================================================
const spu_pgpio_info_t PGPIO_INFO[CONFIG_SPU_NUM][32] = {
  //SPU0
  {
    {ENUM_SPU_DEV_SPI,0,ENUM_SPI_SCLK}, //pgpio0
    {ENUM_SPU_DEV_SPI,0,ENUM_SPI_MOSI}, //pgpio1
    {ENUM_SPU_DEV_SPI,0,ENUM_SPI_MISO}, //pgpio2
    {ENUM_SPU_DEV_SPI,0,ENUM_SPI_SSEL}, //pgpio3
    {}, //pgpio4
    {}, //pgpio5
    {}, //pgpio6
    {}, //pgpio7
    {ENUM_SPU_DEV_USART,0,ENUM_USART_RX}, //pgpio8
    {ENUM_SPU_DEV_USART,0,ENUM_USART_TX}, //pgpio9
    {}, //pgpio10
    {}, //pgpio11
    {}, //pgpio12
    {}, //pgpio13
    {}, //pgpio14
    {}, //pgpio15
    {}, //pgpio16
    {}, //pgpio17
    {ENUM_SPU_DEV_I2C,0,ENUM_I2C_SDA}, //pgpio18
    {ENUM_SPU_DEV_I2C,0,ENUM_I2C_SCL}, //pgpio19
    {}, //pgpio20
    {}, //pgpio21
    {}, //pgpio22
    {}, //pgpio23
    {}, //pgpio24
    {}, //pgpio25
    {}, //pgpio26
    {}, //pgpio27
    {}, //pgpio28
    {}, //pgpio29
    {}, //pgpio30
    {}  //pgpio31
  },
};

spu_pgpio_info_t get_pgpio_info(int32_t spu_idx, int32_t pgpio)
{
    return PGPIO_INFO[spu_idx][pgpio];
}

//==============================================================
//                           DEV->SPU
//==============================================================
//USART
const spu_usart_info_t USART_INFO[CONFIG_SPU_USART_NUM] = {
  //USART0
  {
    PA21_UART1TX_PWM1_SPU9_SIROUT1,  //pin_tx
    PA20_UART1RX_PWM0_SPU8_SIRIN1,  //pin_rx
    0, //spu_idx
    2,
    9, //pgpio_tx
    8, //pgpio_rx
  },
};

int32_t get_usart_info(pin_t pin_tx, pin_t pin_rx, int32_t *spu_idx, int32_t *pgpio_tx, int32_t *pgpio_rx)
{
  int32_t i;

  for(i=0;i<CONFIG_SPU_USART_NUM;i++){
    if(pin_tx==USART_INFO[i].pin_tx && pin_rx==USART_INFO[i].pin_rx){
        *spu_idx = USART_INFO[i].spu_idx;
        /*pinmux*/
        pin_mux(USART_INFO[i].pin_tx, USART_INFO[i].function);
        pin_mux(USART_INFO[i].pin_rx, USART_INFO[i].function);
        *pgpio_tx = USART_INFO[i].pgpio_tx;
        *pgpio_rx = USART_INFO[i].pgpio_rx;
        return i;
    }
  }

  return -1;
}


//SPI
const spu_spi_info_t SPI_INFO[CONFIG_SPU_SPI_NUM] = {
  //SPI0
  {
    PA0_I2C0SCL_SPI1CS1_SPU0_UART1TX,  //pin_sclk
    PA1_I2C0SDA_SPI1CS2_SPU1_UART1RX,  //pin_mosi
    PA8_UART0TX_XX_SPU2_SIROUT0,  //pin_miso
    PA9_UART0RX_XX_SPU3_SIRIN0,  //pin_ssel
    0,  //spu idx
    2,
    0,  //pgpio_sclk
    1,  //pgpio_mosi
    2,  //pgpio_miso
    3,  //pgpio_ssel
  },
};

int32_t get_spi_info(pin_t pin_sclk, pin_t pin_mosi, pin_t pin_miso, pin_t pin_ssel,
                     int32_t *spu_idx,
                     int32_t *pgpio_sclk, int32_t *pgpio_mosi, int32_t *pgpio_miso, int32_t *pgpio_ssel)
{
  int32_t i;

  for(i=0;i<CONFIG_SPU_SPI_NUM;i++){
    if(pin_sclk==SPI_INFO[i].pin_sclk && pin_mosi==SPI_INFO[i].pin_mosi && pin_miso==SPI_INFO[i].pin_miso && pin_ssel==SPI_INFO[i].pin_ssel){
        *spu_idx = SPI_INFO[i].spu_idx;
        /*pinmux*/
        pin_mux(SPI_INFO[i].pin_sclk, SPI_INFO[i].function);
        pin_mux(SPI_INFO[i].pin_mosi, SPI_INFO[i].function);
        pin_mux(SPI_INFO[i].pin_miso, SPI_INFO[i].function);
        pin_mux(SPI_INFO[i].pin_ssel, SPI_INFO[i].function);
        *pgpio_sclk = SPI_INFO[i].pgpio_sclk;
        *pgpio_mosi = SPI_INFO[i].pgpio_mosi;
        *pgpio_miso = SPI_INFO[i].pgpio_miso;
        *pgpio_ssel = SPI_INFO[i].pgpio_ssel;
        return i;
    }
  }

  return -1;
}


//I2C
const spu_i2c_info_t I2C_INFO[CONFIG_SPU_I2C_NUM] = {
  //i2c0
  {
    PA30_I2C0SDA_PWM10_SPU18_XX,  //pin_sda
    PA31_I2C0SCL_PWM11_SPU19_XX,  //pin_scl
    0, //spu_idx
    2,
    18, //pgpio_sda
    19, //pgpio_scl
  },
};

int32_t get_i2c_info(pin_t pin_sda, pin_t pin_scl, int32_t *spu_idx, int32_t *pgpio_sda, int32_t *pgpio_scl)
{
  int32_t i;

  for(i = 0; i < CONFIG_SPU_I2C_NUM; i++){
    if(pin_sda==I2C_INFO[i].pin_sda && pin_scl==I2C_INFO[i].pin_scl){
        *spu_idx = I2C_INFO[i].spu_idx;
        /*pinmux*/
        pin_mux(I2C_INFO[i].pin_scl, I2C_INFO[i].function);
        pin_mux(I2C_INFO[i].pin_sda, I2C_INFO[i].function);
        *pgpio_sda = I2C_INFO[i].pgpio_sda;
        *pgpio_scl = I2C_INFO[i].pgpio_scl;
        return i;
    }
  }

  return -1;
}


#endif
