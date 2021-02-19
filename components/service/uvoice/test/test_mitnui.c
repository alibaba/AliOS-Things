/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "k_api.h"

#include "uvoice_types.h"
#include "uvoice_recorder.h"

#include "uvoice_os.h"

#include "../application/mitnui/inc/nui.h"
#include "../application/mitnui/inc/nui_utest.h"


#define APP_LOGI(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)
#define APP_LOGE(fmt, ...)	printf("%s: "fmt, __func__, ##__VA_ARGS__)


static aos_task_t test_task;


static void event_callback (void *user_data, nui_event_t *event)
{
  uart_printf("event_callback event:%d\n", event->type);
  if (event->type == EVENT_ASR_RESULT ||
      event->type == EVENT_ASR_PARTIAL_RESULT ||
      event->type == EVENT_WUW ||
      event->type == EVENT_DIALOG_RESULT)
  {
    if (event->payload != NULL)
    {
      uart_printf("event_callback payload:%s\n", event->payload);
    }
  }
  else if (event->type == EVENT_END)
  {
    uart_printf("event_callback end\n");
    printf("nui demo dialog start again\n");
//    nui_start(NULL);
  }
  return;
}

#include "uvoice_init.h"
#include "uvoice_types.h"
#include "uvoice_recorder.h"

static uvoice_recorder_t *user_recorder;

static int test_mitnui_data_callback (void *user_data, char *buffer, int len)
{
  uart_printf("data_callback len:%d\n", len);
  int result = user_recorder->get_stream(buffer, len);
  uart_printf("data_callback get:%d\n", result);
  return result;
}

static void test_mitnui_state_callback (void *user_data,
                     nui_audio_state_t state)
{
  uart_printf("state_callback state:%d\n", state);
  if (state == AUDIO_STATE_OPEN) {
	user_recorder->set_sink(MEDIA_FMT_PCM, 16000, 1, 16, 0, 0, NULL);
  } else if (state == AUDIO_STATE_CLOSE) {
    user_recorder->clr_sink();
  }
  return;
}

static const char *vocabulary = "\"vocabulary\":{\"fushichen\":5}";
static const char *hot_word = "[{\"tag\":\"USER.PB\",\"word\":[{\"name\":\"fushichen\"}]}]";
static void test_mitnui_vad(void)
{
  uart_printf("\nnui_demo->nui\n");

  nui_instance_t instance;
  instance = nui_init(FLAG_INIT_DEFAULT);
  nui_set_config(instance,
                 CONFIG_CONTEXT_VOCABULARY,
                 (void *)vocabulary, strlen(vocabulary), FLAG_SET_DEFAULT);
  nui_set_config(instance,
                 CONFIG_CONTEXT_SDM_HOT_WORD,
                 (void *)hot_word, strlen(hot_word), FLAG_SET_DEFAULT);
  nui_set_config(instance,
                 CONFIG_NUI_REG_AUDIO_EVENT_CALLBACK,
                 event_callback, 0, FLAG_SET_DEFAULT);
  nui_set_config(instance,
                 CONFIG_NUI_REG_AUDIO_STATE_CALLBACK,
                 test_mitnui_state_callback, 0, FLAG_SET_DEFAULT);
  nui_set_config(instance,
                 CONFIG_NUI_REG_AUDIO_DATA_CALLBACK,
                 test_mitnui_data_callback, 0, FLAG_SET_DEFAULT);
  nui_start(instance);
  uos_sleep(10 * 200); /* 1 os tick == 5 ms */
  nui_stop(instance, FLAG_STOP_DEFAULT, NULL);
  return;
}

static void test_mitnui_task(void *arg)
{
	uart_printf("%s: enter utest main...\n", __func__);
	int count = 0;
	while (netif_get_default_nw() == NULL) {
		uart_printf("%s: wait netif to be ready ... %d\n", __func__, count);
		count++;
		aos_msleep(2000);
	}
	uart_printf("%s: network ok\n", __func__);
	if (uvoice_init()) {
		uart_printf("%s: init uvoice failed !\n", __func__);
		return;
	}
	user_recorder = uvoice_recorder_create();
	if (!user_recorder) {
		uart_printf("%s: create recorder failed !\n", __func__);
		return;
	}
	utest_main();
}

void test_mitnui_entry(void)
{
	int ret;
	ret = aos_task_new_ext(&test_task, "mitnui_test_task",
			test_mitnui_task,
			NULL,
			1024 * 20,
			220);
	if (ret) {
		uart_printf("create mitnui test task failed %d!\n", ret);
		return;
	}
}

