#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include <sntp/sntp.h>

#define TIME_MODE    1	//0: for UTC with microseconds, 1: for timezone with seconds

static void show_time(void)
{
#if (TIME_MODE == 0)
	unsigned int update_tick = 0;
	long update_sec = 0, update_usec = 0;

	sntp_get_lasttime(&update_sec, &update_usec, &update_tick);

	if(update_tick) {
		long tick_diff_sec, tick_diff_ms, current_sec, current_usec;
		unsigned int current_tick = xTaskGetTickCount();

		tick_diff_sec = (current_tick - update_tick) / configTICK_RATE_HZ;
		tick_diff_ms = (current_tick - update_tick) % configTICK_RATE_HZ / portTICK_RATE_MS;
		update_sec += tick_diff_sec;
		update_usec += (tick_diff_ms * 1000);
		current_sec = update_sec + update_usec / 1000000;
		current_usec = update_usec % 1000000;
		printf("%s + %d usec\n", ctime(&current_sec), current_usec);
	}
#elif (TIME_MODE == 1)
	int timezone = 8;	// use UTC+8 timezone for example
	struct tm tm_now = sntp_gen_system_time(timezone);
	printf("%d-%d-%d %d:%d:%d UTC%s%d\n",
		tm_now.tm_year, tm_now.tm_mon, tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec,
		(timezone > 0) ? "+" : "", timezone);
#endif
}

static void example_sntp_showtime_thread(void *param)
{
	int should_stop = 0;
	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: SNTP show time\n");

	sntp_init();

	while(1) {
		show_time();
		vTaskDelay(1000);
		if(should_stop)
			break;
	}

	vTaskDelete(NULL);
}

void example_sntp_showtime(void)
{
	if(xTaskCreate(example_sntp_showtime_thread, ((const char*)"example_sntp_showtime_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate failed\n", __FUNCTION__);
}
