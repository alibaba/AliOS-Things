
#include "aos/kernel.h"
#include "k_api.h"
#include "errno.h"
#include "aos/hal/uart.h"
#include "app_uart.h"
#include "app_error.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_uart.h"

#define UART_TX_BUF_SIZE 512                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 128                         /**< UART RX buffer size. */

#define OS_WAIT_FOREVER		0xffffffffU

#define HAL_WAIT_FOREVER    OS_WAIT_FOREVER

uart_dev_t uart_0;

void uart_handler(app_uart_evt_t * p_event)
{
    /*
    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
        {
            uint8_t byte = 0;
            uint32_t err_code = 0;
            err_code = app_uart_get(&byte);
            if (err_code != NRF_SUCCESS)
            {
                return;
            }

            if (byte == 'q' || byte == 'Q')
            {
                printf("Exit!\r\n");
            }
            else
            {
                app_uart_put(byte);
            }
        } break;

        case APP_UART_TX_EMPTY:
        {
            // TX complete
        } break;

        case APP_UART_COMMUNICATION_ERROR:
        {
            APP_ERROR_HANDLER(p_event->data.error_communication);
        } break;

        case APP_UART_FIFO_ERROR:
        {
            APP_ERROR_HANDLER(p_event->data.error_code);
        } break;

        default:
        break;
    }*/
}

int32_t hal_uart_init(uart_dev_t* uart )
{
	uint32_t err_code;
    (void)uart;

    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        NRF_UART_BAUDRATE_115200
    };

    APP_UART_FIFO_INIT(&comm_params,
         UART_RX_BUF_SIZE,
         UART_TX_BUF_SIZE,
         uart_handler,
         APP_IRQ_PRIORITY_LOWEST,
         err_code);

    APP_ERROR_CHECK(err_code);
    return 0;
}

#if defined (__CC_ARM) && defined(__MICROLIB)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#elif defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#else
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#endif /* defined (__CC_ARM) && defined(__MICROLIB) */


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  if (ch == '\n') {
    //hal_uart_send(&console_uart, (void *)"\r", 1, 30000);
    hal_uart_send(&uart_0, (void *)"\r", 1, 30000);
  }
  hal_uart_send(&uart_0, &ch, 1, 30000);
  return ch;
}


/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. readwrite a character to the USART2 and Loop until the end of transmission */
  uint8_t ch = EOF;
  int32_t ret = -1;

  ret = hal_uart_recv(&uart_0, &ch, 1, HAL_WAIT_FOREVER);

  if (ret == 0) {
      return ch;
  } else {
      return -1;
  }
}

extern uint32_t * __isr_vector;

/*flash is protect so it is not useful*/
void os_hwi_set_handler(uint32_t irq, void *func, uint32_t param)
{
	uint32_t volatile * irgfun;
	(void)param;
	irgfun = (uint32_t *)&__isr_vector + ( 16 + irq);
	*irgfun = func;

}



#if   defined ( __CC_ARM )
#define ASM_MSR(name,reg)         __ASM volatile("MSR "#name","#reg"\r\n")
#define ASM_MRS(name,reg)         __ASM volatile("MRS "#reg","#name"\r\n")

#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define ASM_MSR(name,reg)         __ASM volatile("MSR "#name","#reg"\r\n")
#define ASM_MRS(name,reg)         __ASM volatile("MRS "#reg","#name"\r\n")

#elif defined ( __GNUC__ )
#define ASM_MSR(name,reg)         __ASM volatile("MSR "#name",%0\r\n":"=r"(reg))
#define ASM_MRS(name,reg)         __ASM volatile("MRS %0,"#name"\r\n":"=r"(reg))

#elif defined ( __ICCARM__ )
#define ASM_MSR(name,reg)         __ASM volatile("MSR "#name",%0":"=r"(reg))
#define ASM_MRS(name,reg)         __ASM volatile("MRS %0,"#name:"=r"(reg))

#endif


int g_msp_value,g_psp_value;
int g_hardfault_num = 0;


typedef struct task_context_s
{
    uint32_t R0;
    uint32_t R1;
    uint32_t R2;
    uint32_t R3;
    uint32_t R12;
    uint32_t LR;
    uint32_t PC;
    uint32_t xPSR;
#if (FPU_AVL > 0)
    uint32_t S0;
    uint32_t S1;
    uint32_t S2;
    uint32_t S3;
    uint32_t S4;
    uint32_t S5;
    uint32_t S6;
    uint32_t S7;
    uint32_t S8;
    uint32_t S9;
    uint32_t S10;
    uint32_t S11;
    uint32_t S12;
    uint32_t S13;
    uint32_t S14;
    uint32_t S15;
    uint32_t FSCR;
#endif
} kcontext_t;

void print_stack_context(kcontext_t *addr,int len)
{
    printf("err pc:0x%08x\terr lr:0x%08x\r\n",addr->PC,addr->LR);
		printf("\r\nmsp context:");
    print_stack_mem(addr, len);

}

void print_stack_mem(void* addr,int len)
{
    void    *cur, *end;
    int      i=0;
    int     *p;

    cur = (void *)addr;
    end   = (uint8_t *)cur + len;
    p = (int*)cur;
    while(p < (int*)end) {
        if(i%4==0) {
            printf("\r\n0x%08x:",(uint32_t)p);
        }
        printf("0x%08x ", *p);
        i++;
        p++;
    }
    printf("\r\n");
    return;

}


void print_sys_reg(void)
{
    uint32_t val;

    ASM_MRS(PSR,val);
    printf("\r\nPSR:0x%x",val);

    ASM_MRS(IPSR,val);
    printf("\r\nIPSR:0x%x",val);

    ASM_MRS(EPSR,val);
    printf("\r\nEPSR:0x%x",val);

    ASM_MRS(PRIMASK,val);
    printf("\r\nPRIMASK:0x%x",val);

    ASM_MRS(FAULTMASK,val);
    printf("\r\nFAULTMASK:0x%x",val);

    ASM_MRS(BASEPRI,val);
    printf("\r\nBASEPRI:0x%x",val);

    ASM_MRS(CONTROL,val);
    printf("\r\nCONTROL:0x%x",val);
}


void print_sys_variable(void)
{
    uint8_t cur_cpu_num;

    cur_cpu_num = cpu_cur_get();
    printf("\r\ng_sys_stat:0x%x",g_sys_stat);
    printf("\r\ng_sched_lock:%d",g_sched_lock[cur_cpu_num]);
    printf("\r\ng_intrpt_nested_level:%d",g_intrpt_nested_level[cur_cpu_num]);
    printf("\r\ng_active_task:%p,taskname:%s",g_active_task[cur_cpu_num],g_active_task[cur_cpu_num]->task_name);
}

__INLINE void cpu_hardfault_handle(void)
{
    /*msp\psp get and print*/
    ASM_MRS(msp,g_msp_value);

    ASM_MRS(psp,g_psp_value);

    print_stack_context((kcontext_t *)g_msp_value,sizeof(kcontext_t));
    printf("\r\npsp context:");
    print_stack_mem((void *)g_psp_value,200);

    /*once or mutitimes*/
    g_hardfault_num++;
    printf("cpu hard fault num:%d\r\n",g_hardfault_num);

    /*base sys reg*/
    print_sys_reg();

    /*cur info    task info  sp over*/
    //print_sys_variable();

    /*trace info can be get by cli cmd below*/

    /*cli*/
    //cli_main(NULL);
}


#ifndef AOS_COMP_DEBUG
void HardFault_Handler(void)
{
  //cpu_hardfault_handle();

  while (1)
  {
  }
}
#endif

