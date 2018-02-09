#ifndef UART_UPDATE_H
#define UART_UPDATE_H

#include "PinNames.h"
#include "gpio_api.h"
//#include "hal_gpio.h"
//#include "osdep_api.h"
#include "osdep_service.h"

#define MAX_WAIT_TIME	100
#if defined(CONFIG_PLATFORM_8711B)
#define PIN_NAME		PA_5
#else
#define PIN_NAME		PC_2

typedef enum
{
	GPIO_Mode_IN				= 0x00, /*!< GPIO Input Mode             */
	GPIO_Mode_OUT				= 0x01, /*!< GPIO Output Mode                */
	GPIO_Mode_INT				= 0x02, /*!< GPIO Interrupt Mode                */
	GPIO_Mode_MAX				= 0x03,
}GPIOMode_TypeDef;

/** 
  * @brief  GPIO Configuration PullUp PullDown enumeration 
  */ 
typedef enum
{ 
	GPIO_PuPd_NOPULL	= 0x00, /*!< GPIO Interrnal HIGHZ	*/
	GPIO_PuPd_DOWN		= 0x01, /*!< GPIO Interrnal Pull DOWN	*/
	GPIO_PuPd_UP		= 0x02, /*!< GPIO Interrnal Pull UP	*/
}GPIOPuPd_TypeDef;

/**
 * @brief Setting interrupt's trigger type
 *
 * Setting interrupt's trigger type
 */
typedef enum
{
	GPIO_INT_Trigger_LEVEL = 0x0, /**< This interrupt is level trigger  */
	GPIO_INT_Trigger_EDGE  = 0x1, /**< This interrupt is edge trigger  */
}GPIOIT_LevelType;

/**
 * @brief Setting interrupt active mode
 *
 * Setting interrupt active mode
 */
typedef enum
{
	GPIO_INT_POLARITY_ACTIVE_LOW  = 0x0, /**< Setting interrupt to low active: falling edge or low level  */
	GPIO_INT_POLARITY_ACTIVE_HIGH = 0x1, /**< Setting interrupt to high active: rising edge or high level */
}GPIOIT_PolarityType;

/**
 * @brief Enable/Disable interrupt debounce mode
 *
 * Enable/Disable interrupt debounce mode
 */
typedef enum
{
	GPIO_INT_DEBOUNCE_DISABLE = 0x0, /**< Disable interrupt debounce  */
	GPIO_INT_DEBOUNCE_ENABLE  = 0x1, /**< Enable interrupt debounce   */
}GPIOIT_DebounceType;


typedef struct {
	GPIOMode_TypeDef	GPIO_Mode;		/*!< Specifies the operating mode for the selected pins. */
	GPIOPuPd_TypeDef	GPIO_PuPd;		/*!< Specifies the operating Pull-up/Pull down for the selected pins. */
	GPIOIT_LevelType	GPIO_ITTrigger;		/**< Interrupt mode is level or edge trigger */    
	GPIOIT_PolarityType	GPIO_ITPolarity;	/**< Interrupt mode is high or low active trigger */
	GPIOIT_DebounceType	GPIO_ITDebounce;	/**< Enable or disable de-bounce for interrupt */
	u32 GPIO_Pin;    // Pin: [7:5]: port number, [4:0]: pin number
}GPIO_InitTypeDef;
#endif
//======================================================

void example_uart_update();
int is_update_image_enable(gpio_t *gpio_uart_update_eable);
extern int uart_ymodem(void);

#endif

