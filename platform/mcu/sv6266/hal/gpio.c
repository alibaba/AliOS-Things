#include "gpio/hal_gpio.h"
#include <hal/soc/gpio.h>
#include "errno.h"

static struct GPIO_STATUS_T
{
	gpio_irq_trigger_t  intc_mode;
	void *intc_data;
	void (*callback)(void *pdata);
}gpio_status[22];

int32_t hal_gpio_init(gpio_dev_t *gpio)
{
	if(gpio->port <0 || gpio->port > 22)
		return -EIO;

	//set gpio mode
	hal_gpio_set_mode(gpio->port,PIN_MODE_GPIO);


	//set input
	if(gpio->config == IRQ_MODE || 			\
		gpio->config == INPUT_PULL_UP || 	\
		gpio->config == INPUT_PULL_DOWN || 	\
		gpio->config == INPUT_PULL_DOWN  )
	{
		hal_gpio_set_dir(gpio->port,GPIO_DIR_IN);
	}
	//set output
	if(gpio->config == OUTPUT_PUSH_PULL || 			\
		gpio->config == OUTPUT_OPEN_DRAIN_NO_PULL ||\
		gpio->config == OUTPUT_OPEN_DRAIN_PULL_UP)
	{
		hal_gpio_set_dir(gpio->port,GPIO_DIR_OUT);
	}

	switch(gpio->config)
	{
		case ANALOG_MODE:
			hal_gpio_set_dir(gpio->port,GPIO_DIR_IN_OUT);
			break;				
		case INPUT_PULL_UP:
			hal_gpio_set_pull(gpio->port,GPIO_PULL_UP);
			break;
		case INPUT_PULL_DOWN:
			hal_gpio_set_pull(gpio->port,GPIO_PULL_DOWN);
			break;
		case INPUT_HIGH_IMPEDANCE:
			hal_gpio_set_pull(gpio->port,GPIO_PULL_NONE);
			break;
		case OUTPUT_PUSH_PULL:
		case IRQ_MODE:	
			hal_gpio_set_pull(gpio->port,GPIO_PULL_UP);
			break;
		case OUTPUT_OPEN_DRAIN_NO_PULL:	
		case OUTPUT_OPEN_DRAIN_PULL_UP:		
			hal_gpio_set_pull(gpio->port,GPIO_PULL_NONE);
			break;
		default:
			return EIO;
			
	}

	return 0;
}

int32_t hal_gpio_output_high(gpio_dev_t *gpio)
{
	hal_gpio_set_logic(gpio->port,GPIO_LOGIC_HIGH);
	return 0;
}

int32_t hal_gpio_output_low(gpio_dev_t *gpio)
{
	hal_gpio_set_logic(gpio->port,GPIO_LOGIC_LOW);
	return 0;
}

int32_t hal_gpio_output_toggle(gpio_dev_t *gpio)
{
	gpio_dir_t pre_dir;
	gpio_logic_t val;
//save pre dir
	pre_dir = hal_gpio_get_dir(gpio->port);
//change mode ,read logice
	hal_gpio_set_dir(gpio->port, GPIO_DIR_IN_OUT);
	val = hal_gpio_get_logic(gpio->port);
//out reverse val
	hal_gpio_set_logic(gpio->port,!val);
	hal_gpio_set_dir(gpio->port, pre_dir);
	return 0;
}

int32_t hal_gpio_input_get(gpio_dev_t *gpio, uint32_t *value)
{
	*value = hal_gpio_get_logic(gpio->port);
	return 0;
}

static void gpio_intc_cb(uint32_t irq_num)
{       
    if(gpio_status[irq_num].callback != NULL)
    {       
            gpio_status[irq_num].callback(gpio_status[irq_num].intc_data);
//            hal_gpio_intc_clear_counter((gpio_pin_t)irq_num);
    }
}

int32_t hal_gpio_enable_irq(gpio_dev_t *gpio, gpio_irq_trigger_t trigger,
                            gpio_irq_handler_t handler, void *arg)
{
	gpio_status[gpio->port].intc_mode = trigger;
	gpio_status[gpio->port].intc_data = arg;
	gpio_status[gpio->port].callback	= handler;

	if(trigger == IRQ_TRIGGER_RISING_EDGE)
		hal_gpio_intc_trigger_mode(gpio->port,GPIO_INTC_RISING_EDGE);
	else if(trigger == IRQ_TRIGGER_FALLING_EDGE)
		hal_gpio_intc_trigger_mode(gpio->port,GPIO_INTC_FALLING_EDGE);
	else
	{
		hal_gpio_intc_trigger_mode(gpio->port,GPIO_INTC_RISING_EDGE);
		hal_gpio_intc_trigger_mode(gpio->port,GPIO_INTC_FALLING_EDGE);
	}

	hal_gpio_register_isr(gpio->port,gpio_intc_cb);
	return 0;
}	

int32_t hal_gpio_disable_irq(gpio_dev_t *gpio)
{
	intc_group31_irq_disable(gpio->port);
	return 0;
}

int32_t hal_gpio_clear_irq(gpio_dev_t *gpio)
{
	hal_gpio_intc_clear_counter(gpio->port);
	return 0;
}

int32_t hal_gpio_finalize(gpio_dev_t *gpio)
{
	hal_gpio_set_mode(gpio->port,PIN_MODE_ZSTATE);
	if(gpio_status[gpio->port].callback != NULL)
	{
		gpio_status[gpio->port].callback = NULL;
		hal_gpio_register_isr(gpio->port,NULL);
	}
	return 0;
}



