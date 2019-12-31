/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifdef AOS_COMP_DEBUG

#include "k_arch.h"
#include "k_api.h"
#include "debug_api.h"

#define BACK_TRACE_LIMIT 64

extern void _interrupt_return_address();

/** WARNING: to use call trace facilities, enable
 *  compiler's frame pointer feature:
 *  -fno-omit-frame-pointer
 */

/* call graph
 *
 *                                   low addr
 *                                  |___..___|
 *                               ---|___fp___|<--- previous fp
 * func A stack start -->        |  |___lr___|<--- current fp
 *                               |  |        |
 *                               |  |   ..   |
 *                               |  |________|
 *                             --|--|___fp___|
 * func B stack start -->      | -->|___lr___|
 *                             |    |        |
 *                             |    |        |
 *                             |    |___..___|
 *                           --|----|___fp___|
 * fnnc C stack start -->    |  --->|___lr___|
 *                           |      |        |
 *                           |       high addr
 *                           ------>
 *
 *                   func_C () {
 *                      ...
 *                      func_B();
 *                      ...
 *                   }
 *
 *                   func_B() {
 *                      ...
 *                      func_A();
 *                      ...
 *                   }
 */

static void getRLfromCtx(void *context, int **FP, int **LR)
{
    context_t *ptr = context;

    /* reference to cpu_task_stack_init */
    *FP = (int *)ptr->R11;
    *LR = (int *)ptr->LR;
}

/* printf call stack */
int backtrace_now(int (*print_func)(const char *fmt, ...))
{
    int  lvl;
    int *FP;
    int *LR;
    int *PC;
    char prtbuff[] = "backtrace : 0x         \n";

#if defined(__CC_ARM)
    FP = __builtin_frame_address(0);
#elif defined(__ICCARM__)
    asm volatile("mov %0, FP\n" : "=r"(FP));
#elif defined(__GNUC__)
    FP = __builtin_frame_address(0);
#endif

    print_func("========== Call stack ==========\r\n");

    for (lvl = 0; lvl < BACK_TRACE_LIMIT; lvl++) {
        LR = (int*)(*FP);
        PC = LR - 1;

        if (LR == (int*)krhino_task_deathbed) {
            print_func("backtrace : ^task entry^\n");
            break;
        }
        else if (LR == (int*)_interrupt_return_address) {
            print_func("backtrace : ^interrupt^\r\n");
            break;
        }

        k_int2str((int)PC, &prtbuff[14]);
        print_func(prtbuff);
        FP = (int*)*(FP - 1);
    }
    print_func("==========    End     ==========\r\n");

    return lvl;
}

int backtrace_task(char *taskname, int (*print_func)(const char *fmt, ...))
{
    int  lvl;
    int *FP;
    int *LR;
    ktask_t *task;
    char prtbuff[] = "backtrace : 0x         \n";

    if (print_func == NULL) {
        print_func = printf;
    }

    task = krhino_task_find(taskname);
    if (task == NULL) {
        print_func("Task not found : %s\n", taskname);
        return 0;
    }

/*
    if (debug_task_is_ready(task)) {
        print_func("Status of task \"%s\" is 'Ready', Can not backtrace!\n",
                   taskname);
        return 0;
    }
*/
    getRLfromCtx(task->task_stack, &FP, &LR);

    print_func("TaskName  : %s\n", taskname);
    print_func("========== Call stack ==========\r\n");
    k_int2str((int)(LR - 1), &prtbuff[14]);
    print_func(prtbuff);
    for (lvl = 1; lvl < BACK_TRACE_LIMIT; lvl++) {
        LR = (int*)(*FP);

        if (LR == (int*)krhino_task_deathbed) {
            print_func("backtrace : ^task entry^\n");
            break;
        }
        else if (LR == (int*)_interrupt_return_address) {
            print_func("backtrace : ^interrupt^\r\n");
            break;
        }

        k_int2str((int)(LR - 1), &prtbuff[14]);
        print_func(prtbuff);
        FP = (int*)*(FP - 1);
    }
    print_func("==========    End     ==========\r\n");

    return lvl;
}


#define REG_NAME_WIDTH 7

static fault_context_t *s_fcontext;

extern volatile uint32_t g_crash_steps;

void panicGetCtx(void *context, char **pPC, char **pLR, int **pSP)
{
    if ( context != NULL )
    {
        s_fcontext = (fault_context_t *)context;
    }

    *pSP = (int *)s_fcontext->SP;
    *pPC = (char *)s_fcontext->cntx.PC;
    *pLR = (char *)s_fcontext->cntx.LR;
}

void panicShowRegs(void *context, int (*print_func)(const char *fmt, ...))
{
    int  x, index;
    fault_context_t *flt_ctx = (fault_context_t *)context;
    long *regs;
    char s_panic_regs[REG_NAME_WIDTH + 14];
    /* PANIC_CONTEXT */
    char s_panic_ctx[] = "PC     "
                         "LR     "
                         "SP     "
                         "CPSR   "
                         "R0     "
                         "R1     "
                         "R2     "
                         "R3     "
                         "R4     "
                         "R5     "
                         "R6     "
                         "R7     "
                         "R8     "
                         "R9     "
                         "R10    "
                         "R11    "
                         "R12    "
                         ;
    /* fault_context_t map to s_panic_ctx */
#ifdef FPU_AVL
#ifdef NEON_AVL
    char aMap[] = {66,65,0,51,52,53,54,55,56,57,58,59,60,61,62,63,64};
#else
    char aMap[] = {50,49,0,35,36,37,38,39,40,41,42,43,44,45,46,47,48};
#endif
#else
    char aMap[] = {16,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
#endif

    if (context == NULL) {
        return;
    }

    print_func("========== Regs info  ==========\r\n");

    switch ( flt_ctx->exc_type )
    {
        case ARM_EXCEPT_UNDEF_INSTR :
            print_func("Exception Type: Undefined Instruction\r\n");
            break;

        case ARM_EXCEPT_PREFETCH_ABORT :
            print_func("Exception Type: Prefetch Abort\r\n");
            break;

        case ARM_EXCEPT_DATA_ABORT :
            print_func("Exception Type: Data Abort\r\n");
            break;
	case ARM_EXCEPT_SWI :
            print_func("Exception Type: SWI\r\n");
            break;

        default:
            print_func("Exception Type: Unknown\r\n");
            break;
    }

    regs = &flt_ctx->SP;
    /* show PANIC_CONTEXT */
    for (x = 0; x < sizeof(s_panic_ctx) / REG_NAME_WIDTH; x++) {
        index = aMap[x];
        memcpy(&s_panic_regs[0], &s_panic_ctx[x * REG_NAME_WIDTH],
               REG_NAME_WIDTH);
        memcpy(&s_panic_regs[REG_NAME_WIDTH], " 0x", 3);
        k_int2str(regs[index], &s_panic_regs[REG_NAME_WIDTH + 3]);
        s_panic_regs[REG_NAME_WIDTH + 11] = '\r';
        s_panic_regs[REG_NAME_WIDTH + 12] = '\n';
        s_panic_regs[REG_NAME_WIDTH + 13] = 0;
        print_func(s_panic_regs);
    }
}

void exceptionHandler(void *context)
{
    g_crash_steps++;
    if (g_crash_steps > 1) {
        printf("double exception occur!\n");
        context = NULL;
    }

    panicHandler(context);

    printf("exception occur!\n");
    /* app can add exception handler here */
    while (1)
        ;
}


#define PANIC_STACK_LIMIT    0x100000

/* backtrace start with PC and SP, find LR from stack memory
   return levels os callstack */
int panicBacktraceCaller(char *PC, int *SP,
                         int (*print_func)(const char *fmt, ...))
{
    int  lvl;
    int *FP;
    int *LR;
    char prtbuff[] = "backtrace : 0x         \n";

    FP = (int *)s_fcontext->cntx.R11;
    LR = (int *)s_fcontext->cntx.LR;

    k_int2str((int)PC, &prtbuff[14]);
    print_func(prtbuff);

    for (lvl = 1; lvl < BACK_TRACE_LIMIT; lvl++) {
        if ( (unsigned int)FP - (unsigned int)SP > PANIC_STACK_LIMIT )
        {
            print_func("backtrace error, stack may corrupt\n");
        }

        LR = (int*)(*FP);

        if (LR == (int*)krhino_task_deathbed) {
            print_func("backtrace : ^task entry^\n");
            break;
        }
        else if (LR == (int*)_interrupt_return_address) {
            print_func("backtrace : ^interrupt^\r\n");
            break;
        }

        k_int2str((int)(LR - 1), &prtbuff[14]);
        print_func(prtbuff);
        FP = (int*)*(FP - 1);
    }
    print_func("==========    End     ==========\r\n");


    return lvl;
}

/* backtrace start with PC SP and LR */
int panicBacktraceCallee(char *PC, int *SP, char *LR,
                         int (*print_func)(const char *fmt, ...))
{
    /* with frame pointer, this function is not needed */
    return 0;
}

#else   /* #ifdef AOS_COMP_DEBUG */
void exceptionHandler(void *context)
{
    while(1);
}
#endif
