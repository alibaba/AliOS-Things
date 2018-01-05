
#include "platform.h"
#include "platform_peripheral.h"
#include "platform_logging.h"

/******************************************************
*                    Constants
******************************************************/

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/* Structure of runtime GPIO IRQ data */
typedef struct
{
    platform_gpio_port_t*        owner_port; // GPIO port owning the IRQ line (line is shared across all GPIO ports)
    platform_gpio_irq_callback_t handler;    // User callback
    void*                        arg;        // User argument to be passed to the callbackA
} platform_gpio_irq_data_t;

/******************************************************
*               Variables Definitions
******************************************************/
/* GPIO peripheral clocks */
const uint32_t gpio_peripheral_clocks[NUMBER_OF_GPIO_PORTS] =
{
    [0] = RCC_AHB1Periph_GPIOA,
    [1] = RCC_AHB1Periph_GPIOB,
    [2] = RCC_AHB1Periph_GPIOC,
    [3] = RCC_AHB1Periph_GPIOD,
    [4] = RCC_AHB1Periph_GPIOE,
    [5] = RCC_AHB1Periph_GPIOF,
    [6] = RCC_AHB1Periph_GPIOG,
    [7] = RCC_AHB1Periph_GPIOH,
};

/* Runtime GPIO IRQ data */
static volatile platform_gpio_irq_data_t gpio_irq_data[NUMBER_OF_GPIO_IRQ_LINES];

/******************************************************
*               Function Declarations
******************************************************/

/******************************************************
*               Function Definitions
******************************************************/

OSStatus platform_gpio_init( const platform_gpio_t* gpio, platform_pin_config_t config )
{
  GPIO_InitTypeDef  gpio_init_structure;
  uint8_t           port_number;
  OSStatus          err = kNoErr;

  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);
  
  port_number = platform_gpio_get_port_number( gpio->port );

  /* Enable peripheral clock for this port */
  RCC->AHB1ENR |= gpio_peripheral_clocks[port_number];

  gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init_structure.GPIO_Mode  = ( ( config == INPUT_PULL_UP ) || ( config == INPUT_PULL_DOWN ) || ( config == INPUT_HIGH_IMPEDANCE ) ) ? GPIO_Mode_IN : GPIO_Mode_OUT;
  gpio_init_structure.GPIO_OType = ( config == OUTPUT_PUSH_PULL ) ? GPIO_OType_PP : GPIO_OType_OD;

  if ( ( config == INPUT_PULL_UP ) || ( config == OUTPUT_OPEN_DRAIN_PULL_UP ) )
  {
    gpio_init_structure.GPIO_PuPd = GPIO_PuPd_UP;
  }
  else if ( config == INPUT_PULL_DOWN )
  {
    gpio_init_structure.GPIO_PuPd = GPIO_PuPd_DOWN;
  }
  else
  {
    gpio_init_structure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  }

  gpio_init_structure.GPIO_Pin = (uint32_t) ( 1 << gpio->pin_number );

  GPIO_Init( gpio->port, &gpio_init_structure );
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_deinit( const platform_gpio_t* gpio )
{
  GPIO_InitTypeDef  gpio_init_structure;
  OSStatus          err = kNoErr;

  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  /* Set to Input high-impedance */
  gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
  gpio_init_structure.GPIO_Mode  = GPIO_Mode_IN;
  gpio_init_structure.GPIO_OType = GPIO_OType_PP; /* arbitrary. not applicable */
  gpio_init_structure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  gpio_init_structure.GPIO_Pin   = (uint32_t) ( 1 << gpio->pin_number );

  GPIO_Init( gpio->port, &gpio_init_structure );

exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_output_high( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable();

  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  gpio->port->BSRRL = (uint16_t) ( 1 << gpio->pin_number );
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_output_low( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;

  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  platform_mcu_powersave_disable();
  
  gpio->port->BSRRH = (uint16_t) ( 1 << gpio->pin_number );
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

OSStatus platform_gpio_output_trigger( const platform_gpio_t* gpio )
{
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable();

  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  gpio->port->ODR ^= (uint16_t) ( 1 << gpio->pin_number );
  
exit:
  platform_mcu_powersave_enable();
  return err;
}

bool platform_gpio_input_get( const platform_gpio_t* gpio )
{
  bool result = false;

  platform_mcu_powersave_disable();

  require_quiet( gpio != NULL, exit);

  result = ( ( gpio->port->IDR & (uint32_t) ( 1 << gpio->pin_number ) ) != 0 ) ? true : false;
  
exit:
  platform_mcu_powersave_enable();
  return result;
}

OSStatus platform_gpio_irq_enable( const platform_gpio_t* gpio, platform_gpio_irq_trigger_t trigger, platform_gpio_irq_callback_t handler, void* arg )
{
  uint32_t interrupt_line = (uint32_t) ( 1 << gpio->pin_number );
  EXTITrigger_TypeDef exti_trigger;
  OSStatus err = kNoErr;
  EXTI_InitTypeDef exti_init_structure;
  IRQn_Type        interrupt_vector = (IRQn_Type)0;

  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  switch ( trigger )
  {
    case IRQ_TRIGGER_RISING_EDGE:
    {
      exti_trigger = EXTI_Trigger_Rising;
      break;
    }
    case IRQ_TRIGGER_FALLING_EDGE:
    {
      exti_trigger = EXTI_Trigger_Falling;
      break;
    }
    case IRQ_TRIGGER_BOTH_EDGES:
    {
      exti_trigger = EXTI_Trigger_Rising_Falling;
      break;
    }
    default:
    {
      err =  kParamErr;
      goto exit;
    }
  }
  
  if (gpio->pin_number == 10) {
    /* This is STM32 Bug, please check at:
    * https://my.st.com/public/STe2ecommunities/mcu/Lists/cortex_mx_stm32/Flat.aspx?RootFolder=%2Fpublic%2FSTe2ecommunities%2Fmcu%2FLists%2Fcortex_mx_stm32%2FGPIO%20Interrupt%20Issue&FolderCTID=0x01200200770978C69A1141439FE559EB459D7580009C4E14902C3CDE46A77F0FFD06506F5B&currentviews=581#{B7C01461-8CEC-4711-B331-64F4AECF4786}
    * ERRATA sheet in section 2.1.8.
    Configuration of PH10 and PI10 as external interrupts is erroneous
    
    Description
    
    PH10 or PI10 is selected as the source for the EXTI10 external interrupt by setting bits
    EXTI10[3:0] of SYSCFG_EXTICR3 register to 0x0111 or 0x1000, respectively. However,
    this erroneous operation enables PH2 and PI2 as external interrupt inputs.
    
    As a result, it is not possible to use PH10/PI10 as interrupt sources if PH2/PI2 are not
    selected as the interrupt source, as well. This means that bits EXTI10[3:0] of
    
    SYSCFG_EXTICR3 register and bits EXTI2[3:0] of SYSCFG_EXTICR1 should be
    programmed to the same value:
    ¡ñ 0x0111 to select PH10/PH2
    ¡ñ 0x1000 to select PI10/PI2
    
    */
    if (gpio->port == GPIOH)
      SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOH, EXTI_PinSource2); 
    else if (gpio->port == GPIOI)
      SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOI, EXTI_PinSource2);
  }

  SYSCFG_EXTILineConfig( platform_gpio_get_port_number( gpio->port ), gpio->pin_number );
  
  if ( ( interrupt_line & 0x001F ) != 0 )
  {
    /* Line 0 to 4 */
    interrupt_vector = (IRQn_Type) ( EXTI0_IRQn + gpio->pin_number );
  }
  else if ( ( interrupt_line & 0x03E0 ) != 0 )
  {
    /* Line 5 to 9 */
    interrupt_vector = EXTI9_5_IRQn;
  }
  else if ( ( interrupt_line & 0xFC00 ) != 0 )
  {
    /* Line 10 to 15 */
    interrupt_vector = EXTI15_10_IRQn;
  }
  
  /* Clear interrupt flag */
  EXTI->PR = interrupt_line;
  NVIC_ClearPendingIRQ( interrupt_vector ); 
  
  exti_init_structure.EXTI_Trigger = exti_trigger;
  exti_init_structure.EXTI_Line    = interrupt_line;
  exti_init_structure.EXTI_Mode    = EXTI_Mode_Interrupt;
  exti_init_structure.EXTI_LineCmd = ENABLE;
  EXTI_Init( &exti_init_structure );

  NVIC_EnableIRQ( interrupt_vector );
  
  gpio_irq_data[gpio->pin_number].owner_port = gpio->port;
  gpio_irq_data[gpio->pin_number].handler    = handler;
  gpio_irq_data[gpio->pin_number].arg        = arg;

exit:
  platform_mcu_powersave_enable();
  return err;
}


OSStatus platform_gpio_irq_disable( const platform_gpio_t* gpio )
{
  uint16_t interrupt_line;
  OSStatus err = kNoErr;

  platform_mcu_powersave_disable();
  require_action_quiet( gpio != NULL, exit, err = kParamErr);

  interrupt_line = (uint16_t) ( 1 << gpio->pin_number );

  if ( ( EXTI->IMR & interrupt_line ) && gpio_irq_data[gpio->pin_number].owner_port == gpio->port )
  {
    bool             interrupt_line_used = 0;
    IRQn_Type        interrupt_vector    = (IRQn_Type)0;
    EXTI_InitTypeDef exti_init_structure;

    /* Disable EXTI interrupt line */
    exti_init_structure.EXTI_Line    = (uint32_t)interrupt_line;
    exti_init_structure.EXTI_LineCmd = DISABLE;
    exti_init_structure.EXTI_Mode    = EXTI_Mode_Interrupt;
    exti_init_structure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_Init( &exti_init_structure );
    exti_init_structure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_Init( &exti_init_structure );

    /* Disable NVIC interrupt */
    if ( ( interrupt_line & 0x001F ) != 0 )
    {
      /* Line 0 to 4 */
      interrupt_vector = (IRQn_Type) ( EXTI0_IRQn + gpio->pin_number );
      interrupt_line_used = false;
    }
    else if ( ( interrupt_line & 0x03E0 ) != 0 )
    {
      /* Line 5 to 9 */
      interrupt_vector = EXTI9_5_IRQn;
      interrupt_line_used = ( ( EXTI->IMR & 0x3e0U ) != 0 ) ? true : false;
    }
    else if ( ( interrupt_line & 0xFC00 ) != 0 )
    {
      /* Line 10 to 15 */
      interrupt_vector = EXTI15_10_IRQn;
      interrupt_line_used = ( ( EXTI->IMR & 0xfc00U ) != 0 ) ? true : false;
    }

    /* Some IRQ lines share a vector. Disable vector only if not used */
    if ( interrupt_line_used == false )
    {
      NVIC_DisableIRQ( interrupt_vector );
    }

    gpio_irq_data[gpio->pin_number].owner_port = 0;
    gpio_irq_data[gpio->pin_number].handler    = 0;
    gpio_irq_data[gpio->pin_number].arg        = 0;
  }

exit:
  platform_mcu_powersave_enable();
  return err;
}


/******************************************************
 *      STM32F2xx Internal Function Definitions
 ******************************************************/
OSStatus platform_gpio_irq_manager_init( void )
{
    memset( (void*)gpio_irq_data, 0, sizeof( gpio_irq_data ) );

    /* Switch on SYSCFG peripheral clock to allow writing into SYSCFG registers */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SYSCFG, ENABLE );

    return kNoErr;
}

uint8_t platform_gpio_get_port_number( platform_gpio_port_t* gpio_port )
{
    switch ( (uint32_t) gpio_port )
    {
        case GPIOA_BASE:
            return EXTI_PortSourceGPIOA;
        case GPIOB_BASE:
            return EXTI_PortSourceGPIOB;
        case GPIOC_BASE:
            return EXTI_PortSourceGPIOC;
        case GPIOD_BASE:
            return EXTI_PortSourceGPIOD;
        case GPIOE_BASE:
            return EXTI_PortSourceGPIOE;
        case GPIOF_BASE:
            return EXTI_PortSourceGPIOF;
        case GPIOG_BASE:
            return EXTI_PortSourceGPIOG;
        case GPIOH_BASE:
            return EXTI_PortSourceGPIOH;
        case GPIOI_BASE:
            return EXTI_PortSourceGPIOI;
        default:
            return 0xFF;
    }
}

OSStatus platform_gpio_enable_clock( const platform_gpio_t* gpio )
{
  uint8_t     port_number;
  OSStatus    err = kNoErr;

  require_action_quiet( gpio != NULL, exit, err = kParamErr);
  
  /* Enable peripheral clock for this port */
  port_number = platform_gpio_get_port_number( gpio->port );
  require_action_quiet( port_number != 0xFF, exit, err = kParamErr);

  RCC->AHB1ENR |= gpio_peripheral_clocks[port_number];  

exit:
  return err;

}


OSStatus platform_gpio_set_alternate_function( platform_gpio_port_t* gpio_port, uint8_t pin_number, GPIOOType_TypeDef output_type, GPIOPuPd_TypeDef pull_up_down_type, uint8_t alternation_function )
{
    GPIO_InitTypeDef  gpio_init_structure;
    uint8_t           port_number = platform_gpio_get_port_number( gpio_port );

    platform_mcu_powersave_disable();

    /* Enable peripheral clock for this port */
    RCC->AHB1ENR |= gpio_peripheral_clocks[port_number];

    gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init_structure.GPIO_Mode  = GPIO_Mode_AF;
    gpio_init_structure.GPIO_OType = output_type;
    gpio_init_structure.GPIO_PuPd  = pull_up_down_type;
    gpio_init_structure.GPIO_Pin   = (uint32_t) ( 1 << pin_number );

    GPIO_Init( gpio_port, &gpio_init_structure );
    GPIO_PinAFConfig( gpio_port, pin_number, alternation_function );

    platform_mcu_powersave_enable();

    return kNoErr;
}

/******************************************************
 *               IRQ Handler Definitions
 ******************************************************/

/* Common IRQ handler for all GPIOs */
MICO_RTOS_DEFINE_ISR( gpio_irq )
{
    uint32_t active_interrupt_vector = (uint32_t) ( ( SCB->ICSR & 0x3fU ) - 16 );
    uint32_t gpio_number;
    uint32_t interrupt_line;

    switch ( active_interrupt_vector )
    {
        case EXTI0_IRQn:
            interrupt_line = EXTI_Line0;
            gpio_number = 0;
            break;
        case EXTI1_IRQn:
            interrupt_line = EXTI_Line1;
            gpio_number = 1;
            break;
        case EXTI2_IRQn:
            interrupt_line = EXTI_Line2;
            gpio_number = 2;
            break;
        case EXTI3_IRQn:
            interrupt_line = EXTI_Line3;
            gpio_number = 3;
            break;
        case EXTI4_IRQn:
            interrupt_line = EXTI_Line4;
            gpio_number = 4;
            break;
        case EXTI9_5_IRQn:
            interrupt_line = EXTI_Line5;
            for ( gpio_number = 5; gpio_number < 10 && ( EXTI->PR & interrupt_line ) == 0; gpio_number++ )
            {
                interrupt_line <<= 1;
            }
            break;
        case EXTI15_10_IRQn:
            interrupt_line = EXTI_Line10;
            for ( gpio_number = 10; gpio_number < 16 && ( EXTI->PR & interrupt_line ) == 0; gpio_number++ )
            {
                interrupt_line <<= 1;
            }
            break;
        default:
            return;
    }

    /* Clear interrupt flag */
    EXTI->PR = interrupt_line;

    /* Call the respective GPIO interrupt handler/callback */
    if ( gpio_irq_data[gpio_number].handler != NULL )
    {
        void * arg = gpio_irq_data[gpio_number].arg; /* Avoids undefined order of access to volatiles */
        gpio_irq_data[gpio_number].handler( arg );
    }
}

/******************************************************
 *               IRQ Handler Mapping
 ******************************************************/
MICO_RTOS_DEFINE_ISR( EXTI0_IRQHandler )
{
  gpio_irq();
}

MICO_RTOS_DEFINE_ISR( EXTI1_IRQHandler )
{
  gpio_irq();
}

MICO_RTOS_DEFINE_ISR( EXTI2_IRQHandler )
{
  gpio_irq();
}

MICO_RTOS_DEFINE_ISR( EXTI3_IRQHandler )
{
  gpio_irq();
}

MICO_RTOS_DEFINE_ISR( EXTI4_IRQHandler )
{
  gpio_irq();
}

MICO_RTOS_DEFINE_ISR( EXTI9_5_IRQHandler )
{
  gpio_irq();
}

MICO_RTOS_DEFINE_ISR( EXTI15_10_IRQHandler )
{
  gpio_irq();
}





