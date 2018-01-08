
#include "platform_peripheral.h"
#include "platform.h"
#include "platform_config.h"
#include "platform_logging.h"
#include <string.h> // For memcmp
#include "mico_rtos.h"
#include "platform_init.h"
#include "board.h"

/******************************************************
*                      Macros
******************************************************/

/******************************************************
*                    Constants
******************************************************/

#ifndef STDIO_BUFFER_SIZE
#define STDIO_BUFFER_SIZE   64
#endif

/******************************************************
*                   Enumerations
******************************************************/

/******************************************************
*                 Type Definitions
******************************************************/

/******************************************************
*                    Structures
******************************************************/

/******************************************************
*               Function Declarations
******************************************************/

extern OSStatus host_platform_init( void );

/******************************************************
*               Variables Definitions
******************************************************/
extern platform_uart_t platform_uart_peripherals[];
extern platform_uart_driver_t platform_uart_drivers[];

#ifndef MICO_DISABLE_STDIO
static const platform_uart_config_t stdio_uart_config =
{
  .baud_rate    = STDIO_UART_BUADRATE,
  .data_width   = DATA_WIDTH_8BIT,
  .parity       = NO_PARITY,
  .stop_bits    = STOP_BITS_1,
  .flow_control = FLOW_CONTROL_DISABLED,
  .flags        = 0,
};

static volatile ring_buffer_t stdio_rx_buffer;
static volatile uint8_t             stdio_rx_data[STDIO_BUFFER_SIZE];
mico_mutex_t        stdio_rx_mutex;
mico_mutex_t        stdio_tx_mutex;
#endif /* #ifndef MICO_DISABLE_STDIO */

/******************************************************
*               Function Definitions
******************************************************/
#if defined ( __ICCARM__ )
static inline void __jump_to( uint32_t addr )
{
  __asm( "MOV R1, #0x00000001" );
  __asm( "ORR R0, R0, R1" );  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  __asm( "BLX R0" );
}


#elif defined ( __GNUC__ )
__attribute__( ( always_inline ) ) static __INLINE void __jump_to( uint32_t addr )
{
  addr |= 0x00000001;  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  __ASM volatile ("BX %0" : : "r" (addr) );
}


#elif defined ( __CC_ARM )
static void __asm __jump_to( uint32_t addr )
{
                   

  
  MOV R1, #0x00000001
  ORR R0, R0, R1  /* Last bit of jump address indicates whether destination is Thumb or ARM code */
  BLX R0
}
#endif

void startApplication( uint32_t app_addr )
{
  uint32_t* stack_ptr;
  uint32_t* start_ptr;
  
  if (((*(volatile uint32_t*)app_addr) & 0x2FF80000 ) != 0x20000000)
  app_addr += 0x200;
  /* Test if user code is programmed starting from address "ApplicationAddress" */
  if (((*(volatile uint32_t*)app_addr) & 0x2FF80000 ) == 0x20000000)
  { 
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    /* Clear all interrupt enabled by bootloader */
    for (int i = 0; i < 8; i++ )
        NVIC->ICER[i] = 0xFFFFFFFF;
    
    stack_ptr = (uint32_t*) app_addr;  /* Initial stack pointer is first 4 bytes of vector table */
    start_ptr = ( stack_ptr + 1 );  /* Reset vector is second 4 bytes of vector table */

    #if defined ( __ICCARM__)
    __ASM( "MOV LR,        #0xFFFFFFFF" );
    __ASM( "MOV R1,        #0x01000000" );
    __ASM( "MSR APSR_nzcvq,     R1" );
    __ASM( "MOV R1,        #0x00000000" );
    __ASM( "MSR PRIMASK,   R1" );
    __ASM( "MSR FAULTMASK, R1" );
    __ASM( "MSR BASEPRI,   R1" );
    __ASM( "MSR CONTROL,   R1" );
    #endif
    
    __set_MSP( *stack_ptr );
    __jump_to( *start_ptr );
  }  
}

void platform_mcu_reset( void )
{
  NVIC_SystemReset();
}

/* STM32F2 common clock initialisation function
* This brings up enough clocks to allow the processor to run quickly while initialising memory.
* Other platform specific clock init can be done in init_platform() or init_architecture()
*/
void init_clocks( void )
{
  //RCC_DeInit( ); /* if not commented then the LSE PA8 output will be disabled and never comes up again */
  
  /* Configure Clocks */
  RCC_HSEConfig( HSE_SOURCE );
  RCC_WaitForHSEStartUp( );
  
  RCC_HCLKConfig( AHB_CLOCK_DIVIDER );
  RCC_PCLK2Config( APB2_CLOCK_DIVIDER );
  RCC_PCLK1Config( APB1_CLOCK_DIVIDER );
  
  /* Enable the PLL */
  FLASH_SetLatency( INT_FLASH_WAIT_STATE );
  FLASH_PrefetchBufferCmd( ENABLE );
  
  /* Use the clock configuration utility from ST to calculate these values
  * http://www.st.com/st-web-ui/static/active/en/st_prod_software_internet/resource/technical/software/utility/stsw-stm32090.zip
  */
#ifdef STM32F412xG
  RCC_PLLConfig( PLL_SOURCE, PLL_M_CONSTANT, PLL_N_CONSTANT, PLL_P_CONSTANT, PPL_Q_CONSTANT, PPL_R_CONSTANT );
#else
  RCC_PLLConfig( PLL_SOURCE, PLL_M_CONSTANT, PLL_N_CONSTANT, PLL_P_CONSTANT, PPL_Q_CONSTANT );
#endif
  RCC_PLLCmd( ENABLE );
  
  while ( RCC_GetFlagStatus( RCC_FLAG_PLLRDY ) == RESET )
  {
  }
  RCC_SYSCLKConfig( SYSTEM_CLOCK_SOURCE );
  
  while ( RCC_GetSYSCLKSource( ) != 0x08 )
  {
  }
  
  /* Configure HCLK clock as SysTick clock source. */
  SysTick_CLKSourceConfig( SYSTICK_CLOCK_SOURCE );
 
  
}

WEAK void init_memory( void )
{
  
}

void init_architecture( void )
{
  uint8_t i;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
   /*STM32 wakeup by watchdog in standby mode, re-enter standby mode in this situation*/
  if ( (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET) && RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET){
     RCC_ClearFlag();
     PWR_EnterSTANDBYMode();
   }
  PWR_ClearFlag(PWR_FLAG_SB);
  
  /* Initialise the interrupt priorities to a priority lower than 0 so that the BASEPRI register can mask them */
  for ( i = 0; i < 81; i++ )
  {
    NVIC ->IP[i] = 0xff;
  }
  
  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

  platform_init_peripheral_irq_priorities();

  /* Initialise GPIO IRQ manager */
  platform_gpio_irq_manager_init();
  
  /* Ensure 802.11 device is in reset. */
  host_platform_init( );

#ifdef BOOTLOADER
  return;
#endif
  
  /* Initialise RTC */
  platform_rtc_init( );

    SysTick_Config( SystemCoreClock / 1000 );

}



static char global_cid[25] = { 0 };
const char *mico_generate_cid( uint8_t* length )
{
  uint32_t temp0,temp1,temp2;
  uint8_t temp[12];
  temp0=*(volatile uint32_t*)(0x1FFF7A10);
  temp1=*(volatile uint32_t*)(0x1FFF7A14);
  temp2=*(volatile uint32_t*)(0x1FFF7A18);
  
  temp[0] = (uint8_t)(temp0 & 0x000000FF);
  temp[1] = (uint8_t)((temp0 & 0x0000FF00)>>8);
  temp[2] = (uint8_t)((temp0 & 0x00FF0000)>>16);
  temp[3] = (uint8_t)((temp0 & 0xFF000000)>>24);
  temp[4] = (uint8_t)(temp1 & 0x000000FF);
  temp[5] = (uint8_t)((temp1 & 0x0000FF00)>>8);
  temp[6] = (uint8_t)((temp1 & 0x00FF0000)>>16);
  temp[7] = (uint8_t)((temp1 & 0xFF000000)>>24);
  temp[8] = (uint8_t)(temp2 & 0x000000FF);
  temp[9] = (uint8_t)((temp2 & 0x0000FF00)>>8);
  temp[10] = (uint8_t)((temp2 & 0x00FF0000)>>16);
  temp[11] = (uint8_t)((temp2 & 0xFF000000)>>24);
  sprintf(global_cid, "%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X",
          temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6],
          temp[7], temp[8], temp[9], temp[10], temp[11]);
  *length = 12;
  return global_cid;
}

bool isWakeUpFlagPowerOn(void){
  return (RCC_GetFlagStatus(RCC_FLAG_SFTRST) == RESET) ? true : false;
};

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
    static int i = 0;

    i++;
    if(i != 10)
        return;
    
    i = 0;
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}


void NMI_Handler(void)
{
    while (1)
  {
  }
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
    while (1)
  {
  }
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
    while (1)
  {
  }
}

