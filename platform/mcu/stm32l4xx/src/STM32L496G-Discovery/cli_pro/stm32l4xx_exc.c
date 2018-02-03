#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"
#include "k_api.h"
#include "soc_init.h"
#include "stm32l4xx_exc.h"

int g_msp_value,g_psp_value;
int g_hardfault_num = 0;



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

