#include "os_mem.h"
#include "os_common.h"
#include "os_task.h"
#include "typedef.h"

int os_task_create(os_task_entry_t entry,const char *name,size_t stack_size,void *arg,u8_t pri,os_task_handle_t *task_handle)
{
	
	os_task__t *hdl=NULL;
	//hdl = nos_malloc(sizeof(os_task__t));
	//NULL_PARA_CHECK(hdl,os_false);
	int ret = krhino_task_dyn_create((ktask_t **)&hdl, name, arg, pri , 0, (stack_size), entry, 1);
	if(NULL != task_handle)
	{
		*task_handle = hdl;
	}
	if(0 == ret)
	{
		task_debug_info("create task:stack(%d),pri(%d),name(%s),handle(%x)\r\n",stack_size,pri,(name == NULL)?"NULL":name,(task_handle == NULL)?"NULL":task_handle);
		return os_true;
	}
	else
	{
		task_debug_error("task create fail,name(%s),reason:%d\r\n",(name == NULL)?"NULL":name,ret);
		//if(NULL != task_handle)
		//	nos_free(task_handle);
		return os_false;
	}
}

int os_task_pri_change(os_task_handle_t task_handle,u32_t new_pri)
{
	int ret = 0;
	u8_t old_pri;
	if(NULL == task_handle)
	{
		u8_t cur_cpu_num = cpu_cur_get();
        task_handle = g_active_task[cur_cpu_num];
	}
	char *task_name=((ktask_t *)task_handle)->task_name;
	ret = krhino_task_pri_change(task_handle,(u8_t)new_pri,&old_pri);
	if(0 == ret)
	{
		task_debug_info("task %s[%x] pri change to %d from %d sucess\r\n",(task_name == NULL)?"NULL":task_name,task_handle,new_pri,old_pri);
		return  os_true;
	}
	else
	{
		task_debug_error("task %s[%x] pri change to %d from %d fail\r\n",(task_name == NULL)?"NULL":task_name,task_handle,new_pri,old_pri);
		return os_false;
	}

}

int os_task_pri_get(os_task_handle_t task_handle)
{
	if(NULL == task_handle)
	{
		u8_t cur_cpu_num = cpu_cur_get();
        task_handle = g_active_task[cur_cpu_num];
	}
	return ((ktask_t*)task_handle)->prio;
}
os_task_handle_t os_cur_task_handle(void)
{
	os_task__t *task_handle = NULL;
	task_handle = krhino_cur_task_get();
	return task_handle;
}

char *os_get_task_name(os_task_handle_t task_handle)
{
	u8_t    cur_cpu_num;
	if(NULL == task_handle)
	{
		cur_cpu_num = cpu_cur_get();
        task_handle = g_active_task[cur_cpu_num];
	}
	return ((ktask_t*)task_handle)->task_name;
}

int os_task_delete(os_task_handle_t task_handle)
{
	int ret = 0;
	u8_t    cur_cpu_num;
	if (task_handle == NULL) {
		cur_cpu_num = cpu_cur_get();
        task_handle = g_active_task[cur_cpu_num];
    }
	ret = krhino_task_dyn_del((ktask_t *)task_handle);
	if(0!=ret)
	{
		task_debug_error("task delete fail %d,task handle %x,name %s\r\n",ret,task_handle,(((ktask_t *)task_handle)->task_name == NULL)?"NULL":((ktask_t *)task_handle)->task_name);
		return os_false;
	}
//	if(NULL != task_handle)
	//	nos_free(task_handle);
	task_debug_info("task delete sucess,handle %x name %s\r\n",task_handle,(((ktask_t *)task_handle)->task_name == NULL)?"NULL":((ktask_t *)task_handle)->task_name);
	return os_true;
}
int os_task_yield(void)
{
	krhino_task_yield();
	return os_true;
}
int os_task_yield_isr(void)
{
	krhino_intrpt_enter();
    cpu_intrpt_switch();
	krhino_intrpt_exit();
	return os_true;
}

static CPSR_ALLOC();

static int g_critical_count=0;
void os_enter_critical( void )
{
	if(g_critical_count!=0)
	{
		printf("enter critical error\r\n");
	}
	g_critical_count++;
	RHINO_CRITICAL_ENTER();
}
/*-----------------------------------------------------------*/

void os_exit_critical( void )
{
	if(g_critical_count!=1)
	{
		printf("enter critical error\r\n");
	}
	g_critical_count--;
	RHINO_CRITICAL_EXIT();
}