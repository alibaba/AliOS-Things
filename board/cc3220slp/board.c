#include <hal/soc/soc.h>
#include <aos/kernel.h>

#include "cc3220board.h"

#include <ti/boards/CC3220S_LAUNCHXL/Board.h>

#define UART_BAUD_RATE  115200
#define SYSCLK          80000000
#define CONSOLE         UARTA0_BASE
#define CONSOLE_PERIPH  PRCM_UARTA0

#define HAL_FCPU_MHZ                        80U
#define HAL_FCPU_HZ                         (1000000U * HAL_FCPU_MHZ)
#define HAL_SYSTICK_PERIOD_US               100U

void init_term()
{
        MAP_UARTConfigSetExpClk(CONSOLE,MAP_PRCMPeripheralClockGet(CONSOLE_PERIPH),
                  UART_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                   UART_CONFIG_PAR_NONE));
}

void pinmux_config(void)
{
    //
    // Enable Peripheral Clocks 
    //
    MAP_PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_55 for UART0 UART0_TX
    //
    MAP_PinTypeUART(PIN_55, PIN_MODE_3);

    //
    // Configure PIN_57 for UART0 UART0_RX
    //
    MAP_PinTypeUART(PIN_57, PIN_MODE_3);
}

void Message(const char *str)
{
    if(str != NULL)
    {
        while(*str!='\0')
        {
            MAP_UARTCharPut(CONSOLE,*str++);
        }
    }
}

/*
 *  ======== sysTickInit ========
 */
static void sysTickInit(void)
{
    MAP_IntEnable(FAULT_SYSTICK);
    MAP_SysTickIntEnable();
    MAP_SysTickPeriodSet(HAL_FCPU_HZ / HAL_SYSTICK_PERIOD_US);
    // Force a reload of the SysTick counter register
    HWREG(NVIC_ST_CURRENT) = 0;
    MAP_SysTickEnable();
}


void board_init(void)
{       
	MAP_PRCMCC3200MCUInit();

	pinmux_config();
        
        sysTickInit();

	init_term();
        
        Board_initSPI();
        
        Message("Testing UART\r\n");
}
