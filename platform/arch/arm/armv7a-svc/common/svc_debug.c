#include <stdio.h>
#include <k_config.h>
#include <k_arch.h>
#include <syscall_no.h>

static char is_uart;

void syscall_pre_debug(void *context)
{
    context_t *ct = context;

    if (ct->R0 == SYS_HAL_UART_SEND) {
        is_uart = 1;
        return;
    }

    printf("+++ no 0x%08x, arg_p 0x%08x, sp 0x%08x, lr 0x%08x,"
           " pc 0x%08x, spsr 0x%08x\r\n",
            ct->R0, ct->R1, ct->SP, ct->LR, ct->PC, ct->CPSR);
}


void syscall_post_debug(void *context)
{
    context_t *ct = context;

    if (is_uart) {
        is_uart = 0;
        return;
    }

    printf("--- r0 0x%08x,    r1 0x%08x, sp 0x%08x, lr 0x%08x,"
           " pc 0x%08x, spsr 0x%08x\r\n",
            ct->R0, ct->R1, ct->SP, ct->LR, ct->PC, ct->CPSR);
}

