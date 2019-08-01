#ifndef _GPIO_PUB_H_
#define _GPIO_PUB_H_

#include "generic.h"
#include "drv_model_pub.h"

#define GPIO_FAILURE                (1)
#define GPIO_SUCCESS                (0)

#define GPIO_DEV_NAME                "gpio"

#define GPIO_CFG_PARAM(id, mode)           (id + ((mode & 0xff) << 8))
#define GPIO_CFG_PARAM_DEMUX_ID(param)      (param & 0xff)
#define GPIO_CFG_PARAM_DEMUX_MODE(param)    ((param >> 8) & 0xff)

#define GPIO_OUTPUT_PARAM(id, val)           (id + ((val & 0xff) << 8))
#define GPIO_OUTPUT_DEMUX_ID(param)          (param & 0xff)
#define GPIO_OUTPUT_DEMUX_VAL(param)         ((param >> 8) & 0xff)


#define GPIO_CMD_MAGIC              (0xcaa0000)
enum
{
    CMD_GPIO_CFG                        = GPIO_CMD_MAGIC + 0,
    CMD_GPIO_OUTPUT_REVERSE             = GPIO_CMD_MAGIC + 1,
    CMD_GPIO_ENABLE_SECOND              = GPIO_CMD_MAGIC + 2,
    CMD_GPIO_INPUT                      = GPIO_CMD_MAGIC + 3,
    CMD_GPIO_OUTPUT                     = GPIO_CMD_MAGIC + 4,
    CMD_GPIO_CLR_DPLL_UNLOOK_INT_BIT    = GPIO_CMD_MAGIC + 5,
    CMD_GPIO_EN_DPLL_UNLOOK_INT         = GPIO_CMD_MAGIC + 6,
    CMD_GPIO_INT_ENABLE 	            = GPIO_CMD_MAGIC + 7,
    CMD_GPIO_INT_DISABLE	            = GPIO_CMD_MAGIC + 8,
    CMD_GPIO_INT_CLEAR		            = GPIO_CMD_MAGIC + 9,
    CMD_GPIO_EN_USB_PLUG_IN_INT         = GPIO_CMD_MAGIC + 10,
    CMD_GPIO_EN_USB_PLUG_OUT_INT        = GPIO_CMD_MAGIC + 11,
};


enum
{
    GMODE_INPUT_PULLDOWN = 0,
    GMODE_OUTPUT,
    GMODE_SECOND_FUNC,
    GMODE_INPUT_PULLUP,
    GMODE_INPUT,
    GMODE_SECOND_FUNC_PULL_UP,//Special for uart1
    GMODE_OUTPUT_PULLUP,
    GMODE_HIGH_Z
};

typedef enum 
{
    GPIO0 = 0,
    GPIO1,
    GPIO2,
    GPIO3,
    GPIO4,
    GPIO5,
    GPIO6,
    GPIO7,
    GPIO8,
    GPIO9,
    GPIO10,
    GPIO11,
    GPIO12,
    GPIO13,
    GPIO14,
    GPIO15,
    GPIO16,
    GPIO17,
    GPIO18,
    GPIO19,
    GPIO20,
    GPIO21,
    GPIO22,
    GPIO23,
    GPIO24,
    GPIO25,
    GPIO26,
    GPIO27,
    GPIO28,
    GPIO29,
    GPIO30,
    GPIO31,
    GPIO32,
    GPIO33,
    GPIO34,
    GPIO35,
    GPIO36,
    GPIO37,
    GPIO38,
    GPIO39,    
    GPIONUM
} GPIO_INDEX ;

enum
{
    GFUNC_MODE_UART2 = 0,
    GFUNC_MODE_I2C2,
    GFUNC_MODE_I2S,
    GFUNC_MODE_ADC1,
    GFUNC_MODE_ADC2,
    GFUNC_MODE_CLK13M,
    GFUNC_MODE_PWM0,
    GFUNC_MODE_PWM1,
    GFUNC_MODE_PWM2,
    GFUNC_MODE_PWM3,
    GFUNC_MODE_WIFI_ACTIVE,
    GFUNC_MODE_BT_ACTIVE,
    GFUNC_MODE_BT_PRIORITY,
    GFUNC_MODE_UART1,
    GFUNC_MODE_SD_DMA,
    GFUNC_MODE_SD_HOST,    
    GFUNC_MODE_SPI_DMA, 
    GFUNC_MODE_SPI,    
    GFUNC_MODE_PWM4,    
    GFUNC_MODE_PWM5,    
    GFUNC_MODE_I2C1,    
    GFUNC_MODE_JTAG,    
    GFUNC_MODE_CLK26M,    
    GFUNC_MODE_ADC3,
    GFUNC_MODE_DCMI,	
    GFUNC_MODE_ADC4,
    GFUNC_MODE_ADC5,
    GFUNC_MODE_ADC6,
    GFUNC_MODE_ADC7,
    GFUNC_MODE_SD1_HOST,
    GFUNC_MODE_SPI1,
    GFUNC_MODE_SPI_DMA1,
};

enum
{
    GPIO_INT_LEVEL_LOW = 0,
    GPIO_INT_LEVEL_HIGH = 1,
    GPIO_INT_LEVEL_RISING = 2,
    GPIO_INT_LEVEL_FALLING = 3
};

typedef struct gpio_int_st
{
	UINT32 id;
	UINT32 mode;
	void * phandler;
}GPIO_INT_ST;
				
__inline static void bk_gpio_config_input(GPIO_INDEX id)
{
    UINT32 ret;
    UINT32 param;
    
    param = GPIO_CFG_PARAM(id, GMODE_INPUT);
    ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
    
    ASSERT(GPIO_SUCCESS == ret);
}
								
__inline static void bk_gpio_config_input_pup(GPIO_INDEX id)
{
    UINT32 ret;
    UINT32 param;
    
    param = GPIO_CFG_PARAM(id, GMODE_INPUT_PULLUP);
    ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
    
    ASSERT(GPIO_SUCCESS == ret);
}
								
__inline static void bk_gpio_config_input_pdwn(GPIO_INDEX id)
{
    UINT32 ret;
	UINT32 param;
    
	param = GPIO_CFG_PARAM(id, GMODE_INPUT_PULLDOWN);
	ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
    
	ASSERT(GPIO_SUCCESS == ret);
}

__inline static uint32_t bk_gpio_input(GPIO_INDEX id)
{
    UINT32 ret;                                             
    UINT32 param = id;   
    
    ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_INPUT, &param); 
    
    return ret;                      
}

__inline static void bk_gpio_config_output(GPIO_INDEX id)
{
    UINT32 ret;
    
	UINT32 param;
    
	param = GPIO_CFG_PARAM(id, GMODE_OUTPUT);
	ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_CFG, &param);
	ASSERT(GPIO_SUCCESS == ret);  
}

__inline static void bk_gpio_output(GPIO_INDEX id,UINT32 val)
{
    UINT32 ret;                                           
    UINT32 param;
    
    param = GPIO_OUTPUT_PARAM(id, val);
    ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_OUTPUT, &param);
    ASSERT(GPIO_SUCCESS == ret);           
}

__inline static void bk_gpio_output_reverse(GPIO_INDEX id)
{
    UINT32 ret;
    UINT32 param = id;
    
    ret = sddev_control(GPIO_DEV_NAME, CMD_GPIO_OUTPUT_REVERSE, &param);
    ASSERT(GPIO_SUCCESS == ret);            
}
		
#if ((SOC_BK7231U == CFG_SOC_NAME) || (SOC_BK7221U == CFG_SOC_NAME))
#define GPIO_USB_DP_PIN               GPIO25
#define GPIO_USB_DN_PIN               GPIO28

extern void gpio_usb_second_function(void);
#endif

#if(SOC_BK7221U == CFG_SOC_NAME)
#define USB_PLUG_NO_EVENT            0
#define USB_PLUG_IN_EVENT            1
#define USB_PLUG_OUT_EVENT           2

typedef void (*usb_plug_inout_handler)(void *usr_data, UINT32 event);
typedef struct usb_plug_inout {
    usb_plug_inout_handler handler;
    void *usr_data;
}USB_PLUG_INOUT_ST;

extern USB_PLUG_INOUT_ST usb_plug;
void usb_plug_inout_isr(void);
UINT32 usb_is_plug_in(void);
#endif

extern UINT32 gpio_ctrl(UINT32 cmd, void *param);
extern UINT32 gpio_input(UINT32 id);
extern void gpio_init(void);
extern void gpio_exit(void);
void gpio_int_disable(UINT32 index);
void gpio_int_enable(UINT32 index, UINT32 mode, void (*p_Int_Handler)(unsigned char));
void gpio_config( UINT32 index, UINT32 mode ) ;
void gpio_output(UINT32 id, UINT32 val);

#endif // _GPIO_PUB_H_

// EOF

