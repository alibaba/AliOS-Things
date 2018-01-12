// enable RHINO_CONFIG_SCHED_RR
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>

extern void sdk_init(void);

#define TASK1_PRIORITY     20
#define TASK2_PRIORITY     20

#define TASK1_STACKSIZE    512 //512*cpu_stack_t = 2048byte
#define TASK2_STACKSIZE    512 //512*cpu_stack_t = 2048byte

static ktask_t task1_obj;
static ktask_t task2_obj;

cpu_stack_t task1_buf[TASK1_STACKSIZE];
cpu_stack_t task2_buf[TASK2_STACKSIZE];

void First_Task(void * arg)
{
	uint32_t i;
	for( i = 0; i < 20; i ++)
	{
		printf("\r\nThis is First Task! count is %d\r\n", i);
	}
}
void Second_Task(void * arg)
{
	int i;
	for( i = 0; i < 20; i ++)
	{
		printf("\r\nThis is Second Task! count is %d\r\n", i);
	}

}

int main(void)
{
    krhino_init();
    krhino_task_create(&task1_obj, "task1", 0, TASK1_PRIORITY,  
                        50, task1_buf, TASK1_STACKSIZE, First_Task,  1);                    
    krhino_task_create(&task2_obj, "task2", 0,TASK2_PRIORITY, 
                        50, task2_buf, TASK2_STACKSIZE, Second_Task, 1);
    sdk_init(); 
    
    krhino_start();
    
    return 0;
}
