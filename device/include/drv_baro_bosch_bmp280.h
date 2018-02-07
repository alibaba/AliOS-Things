#ifndef __DRV_BARO_BOSCH_BMP280_H
#define __DRV_BARO_BOSCH_BMP280_H

typedef enum {
	BARO_BOSCH_BMP280_ADDR_LOW = 0,
	BARO_BOSCH_BMP280_ADDR_HIGH
} BARO_BOSCH_BMP280_ADDR_SEL;

extern int drv_baro_bosch_bmp280_init(i2c_dev_t *i2c_dev, BARO_BOSCH_BMP280_ADDR_SEL i2c_addr_sel);

#endif /* __DRV_BARO_BOSCH_BMP280_H */