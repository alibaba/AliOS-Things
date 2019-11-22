#include "os_timer.h"
#include "os_common.h"
#define test_timer_debug printf

static os_timer_handle_t timer_hdl_onece;
static os_timer_handle_t timer_hdl_loop;
static int loop_count = 0;
static int once_count = 0;
static void timer_isr_loop(void *param1,void *param2)
{
	loop_count++;
	test_timer_debug("enter timer loop,para:%x,%s\r\n",param1,param2);
	if(loop_count==5)
	{
		if(os_true != os_timer_period_change(timer_hdl_loop,5000,0))
		{
			test_timer_debug("timer period change fail\r\n");		
		}
	}
	if(loop_count == 10)
	{
		os_timer_stop(timer_hdl_loop);
		test_timer_debug("stop timer\r\n");
		os_delay(3000);
		int ret =  os_timer_delete(timer_hdl_loop);
		test_timer_debug("delete timer,test finish!\r\n\r\n");
	}
}

static void timer_isr_once(void *param1,void *param2)
{
	once_count++;
	test_timer_debug("enter timer once,para:%x,%s\r\n",param1,param2);
	if(once_count>=2)
	{
		test_timer_debug("one shot timer test fail,more than once\r\n");
	}
}
void timer_test_task(void *param)
{
	int ret = 0;
	timer_hdl_loop = os_timer_create("loop-timer",2000,1,"ID-LOOP",timer_isr_loop);
	if(NULL == timer_hdl_loop)
	{
		test_timer_debug("loop timer create fail!\r\n");
	}
	timer_hdl_onece = os_timer_create("once-timer",2000,0,"ID-ONCE",timer_isr_once);
	if(NULL == timer_hdl_onece)
	{
		test_timer_debug("one shot timer create fail!\r\n");
	}
	test_timer_debug("timer will start after 5 seconds\r\n");
	os_delay(5000);
	test_timer_debug("start timer\r\n");
	os_timer_start(timer_hdl_loop);
	os_timer_start(timer_hdl_onece);
	os_task_delete(NULL);
}
