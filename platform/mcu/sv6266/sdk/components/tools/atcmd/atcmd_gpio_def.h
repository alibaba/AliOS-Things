/**
 *
 * @page ATCMD AT Command Manual V2.0
 *
 * @section ATCMD_gpio GPIO AT Command
 * 
 * @subsection ATCMD_gpio_1 Set GPIO Pull High/Pull Down
 * | command     | AT+SET_GPIO_PULL=<id>, <pull> |
 * |---------------|-------------------|
 * | param         | <id>: gpio id(0-22)<br><pull>: 0:down, 1:up, 2:none         |
 * | return value  | +OK               |
 * | example       | AT+SET_GPIO_PULL=1,1 |
 *
 * @subsection ATCMD_gpio_2 Get GPIO status
 * | command     | AT+GPIO_READ=<id> |
 * |---------------|-------------------|
 * | param         | <id>: gpio id(0-22)         |
 * | return value  | +OK               |
 * | example       | AT+GPIO_READ=1 |
 *
 */
 
#ifndef ATCMD_GPIO_DEF_H__
#define ATCMD_GPIO_DEF_H__

#define ATCMD_SET_GPIO_TRI_STATE    "AT+SET_GPIO_TRI_STATE"
#define ATCMD_SET_GPIO_LOGIC        "AT+SET_GPIO_LOGIC"
#define ATCMD_SET_GPIO_PULL         "AT+SET_GPIO_PULL"
#define ATCMD_READ_GPIO             "AT+READ_GPIO"
#define ATCMD_SET_GPIO_INT          "AT+SET_GPIO_INT"

#define ATCMD_SET_PWM               "AT+SET_PWM"
#define ATCMD_SET_PWM_DISABLE       "AT+SET_PWM_DISABLE"
#define ATCMD_SET_PWM_ENABLE        "AT+SET_PWM_ENABLE"
#define ATCMD_SET_PWM_RECONFIG      "AT+SET_PWM_RECONFIG"


#endif