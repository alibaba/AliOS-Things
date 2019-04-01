#ifndef SPI_L3_PROTOCOL_
#define SPI_L3_PROTOCOL_

#include <stdint.h>

#define M_SPI_L3_FIXED_CFG          (64)

typedef struct _spi_l3_obj spi_l3_obj;

typedef void (*spi_l35_data_received)(spi_l3_obj *obj, uint8_t * const buf, uint16_t len);

int32_t spi_l35_send(spi_l3_obj * const obj, uint8_t * const buf, uint16_t len);
void spi_l3_deinit(spi_l3_obj * const obj);
spi_l3_obj *spi_l3_init(const spi_l35_data_received handler);

#endif /* end of include guard */

