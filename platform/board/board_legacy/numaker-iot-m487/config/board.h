#ifndef __BOARD_H
#define __BOARD_H

#include "objects.h"

#define HARDWARE_REVISION   "V3.0"
#define MODEL               "NuMaker-PFM-M487"

/* Platform HAL will refer these structures are defined by board-resource. */

extern struct serial_s 	  board_uart [];
extern struct i2c_s 	  board_i2c [];
extern struct analogin_s  board_analogin [];
extern struct analogout_s board_analogout [];
extern struct gpio_s 	  board_gpio [];
extern struct pwmout_s    board_pwm [];
extern struct spi_s       board_spi [];
extern struct qspi_s      board_qspi [];
extern struct sdh_s       board_sdh [];

extern const int i32BoardMaxUartNum;
extern const int i32BoardMaxI2CNum;
extern const int i32BoardMaxADCNum;
extern const int i32BoardMaxDACNum;
extern const int i32BoardMaxGPIONum;
extern const int i32BoardMaxPWMNum;
extern const int i32BoardMaxSPINum;
extern const int i32BoardMaxQSPINum;
extern const int i32BoardMaxSDHNum;

#endif /* __BOARD */

