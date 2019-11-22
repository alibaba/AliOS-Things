#include "os_test.h"
#include "os_timer.h"
#include "os_config.h"
#include "k_api.h"
#include "aos/kernel.h"
#include "os_assert.h"
#include "typedef.h"

#define TIMER_TEST_ON 0
#define TASK_TEST_ON 0
#define TASK_CRITICAL_TEST_ON 0
#define TASK_YIELD_TEST_ON 1
#define SEM_TEST_ON 0
#define MUTEX_TEST_ON 0
#define QUEUE_TEST_ON 0
#define EVENT_TEST_ON 0


static ktask_t *g_test_hdl=NULL;

static void test(u32_t m)
{
	u32_t t = (u32_t)m;

	printf("m==>%d,t==>%d\r\n",(u32_t)m,t);
}
void os_api_test(void)
{
	//timer test
	char s;
#if TIMER_TEST_ON
	krhino_task_dyn_create(&g_test_hdl, "wifi-init", 0, 8 , 0, 1024, (task_entry_t)timer_test_task, 1);
#elif TASK_TEST_ON
	krhino_task_dyn_create(&g_test_hdl, "wifi-init", 0, 8 , 0, 1024, (task_entry_t)os_test_task, 1);
#elif TASK_CRITICAL_TEST_ON
	krhino_task_dyn_create(&g_test_hdl, "wifi-init", 0, 8 , 0, 1024, (task_entry_t)task_critical_test, 1);
#elif TASK_YIELD_TEST_ON
	krhino_task_dyn_create(&g_test_hdl, "wifi-init", 0, 8 , 0, 1024, (task_entry_t)os_task_yield_test, 1);//fail
#elif SEM_TEST_ON
	krhino_task_dyn_create(&g_test_hdl, "wifi-init", 0, 8 , 0, 1024, (task_entry_t)sem_binary_test, 1);
#elif MUTEX_TEST_ON
	krhino_task_dyn_create(&g_test_hdl, "wifi-init", 0, 8 , 0, 1024, (task_entry_t)os_mutex_test, 1);
#elif QUEUE_TEST_ON
	krhino_task_dyn_create(&g_test_hdl, "wifi-init", 0, 8 , 0, 1024, (task_entry_t)os_queue_test, 1);
#elif EVENT_TEST_ON
	krhino_task_dyn_create(&g_test_hdl, "wifi-init", 0, 8 , 0, 1024, (task_entry_t)os_event_test, 1);
	//test(3000);
#endif

}
