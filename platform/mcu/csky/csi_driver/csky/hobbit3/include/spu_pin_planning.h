#include <stdint.h>
#include "drv_common.h"

//SPU device number
#define CONFIG_SPU_USART_NUM 1
#define CONFIG_SPU_SPI_NUM 1
#define CONFIG_SPU_I2C_NUM 1
#define CONFIG_SPU_CAN_NUM 1

//SPU_PGPIO information
typedef enum {
    ENUM_SPU_DEV_USART,
    ENUM_SPU_DEV_SPI,
    ENUM_SPU_DEV_I2C,
    ENUM_SPU_DEV_CAN,
} spu_dev_type_e;

typedef enum{
    ENUM_USART_TX,
    ENUM_USART_RX,
    ENUM_SPI_SCLK,
    ENUM_SPI_MOSI,
    ENUM_SPI_MISO,
    ENUM_SPI_SSEL,
    ENUM_I2C_SDA,
    ENUM_I2C_SCL,
    ENUM_CAN_TX,
    ENUM_CAN_RX,
} spu_dev_signal_e;

typedef struct{
    spu_dev_type_e dev_type;
    int32_t dev_idx;
    spu_dev_signal_e dev_signal;
} spu_pgpio_info_t;

//SPU_USART information
typedef struct{
  pin_t pin_tx;
  pin_t pin_rx;
  int32_t spu_idx;
  uint16_t function;
  int32_t pgpio_tx;
  int32_t pgpio_rx;
} spu_usart_info_t;

//SPU_SPI information
typedef struct{
  pin_t pin_sclk;
  pin_t pin_mosi;
  pin_t pin_miso;
  pin_t pin_ssel;
  int32_t spu_idx;
  uint16_t function;
  int32_t pgpio_sclk;
  int32_t pgpio_mosi;
  int32_t pgpio_miso;
  int32_t pgpio_ssel;
} spu_spi_info_t;

//SPU_I2C information
typedef struct{
  pin_t pin_sda;
  pin_t pin_scl;
  int32_t spu_idx;
  uint16_t function;
  int32_t pgpio_sda;
  int32_t pgpio_scl;
} spu_i2c_info_t;
