#include <stdio.h>
#include <k_config.h>
#include <k_arch.h>
#include <syscall_no.h>
#include <svc_debug.h>

static char is_uart;

void syscall_pre_debug(void *context)
{
    context_t *ct = context;

    /* skip hal_uart_send syscall */
    if (ct->R0 == SYS_HAL_UART_SEND) {
        is_uart = 1;
        return;
    }

    printf("+++ no 0x%08x, arg_p 0x%08x, sp 0x%08x, lr 0x%08x, pc 0x%08x, spsr 0x%08x\r\n",
            ct->R0, ct->R1, ct->SP, ct->LR, ct->PC, ct->CPSR);
}


void syscall_post_debug(void *context)
{
    context_t *ct = context;

    /* skip hal_uart_send syscall */
    if (is_uart) {
        is_uart = 0;
        return;
    }

    printf("--- r0 0x%08x,    r1 0x%08x, sp 0x%08x, lr 0x%08x, pc 0x%08x, spsr 0x%08x\r\n",
            ct->R0, ct->R1, ct->SP, ct->LR, ct->PC, ct->CPSR);
}

void bad_svc_no_exception(void *context, int svc_no)
{
    context_t *ct = context;

    ct = ct;

    printf("SVC exception: bad svc nubmer 0x%x\r\n", svc_no);
}

void syscall_exception(int type, int syscall_no)
{
    switch (type) {
	case SYSCALL_EXCEPTION_NO: {
	    printf("bad syscall number: 0x%x\r\n", syscall_no);
	    break;
        }
	case SYSCALL_EXCEPTION_FUNC_NULL: {
	    printf("null syscall function pointer, syscall number 0x%x\r\n", syscall_no);
	    break;
        }
	default:
	    break;
    }
}
