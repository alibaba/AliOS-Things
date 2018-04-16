/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "GUIDEMO.h"
#include <aos/aos.h>
#include <aos/uData.h>
#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
#include "fatfs.h"
static const char *g_string         = "Fatfs test string.";
static const char *g_filepath       = "/sdcard/test.txt";
static const char *g_dirpath        = "/sdcard/testDir";
static const char *g_dirtest_1      = "/sdcard/testDir/test_1.txt";
static const char *g_dirtest_2      = "/sdcard/testDir/test_2.txt";
static const char *g_dirtest_3      = "/sdcard/testDir/test_3.txt";
static const char *g_new_filepath   = "/sdcard/testDir/newname.txt";
#endif
#define DEMO_TASK_STACKSIZE    1024 //512*cpu_stack_t = 2048byte
#define DEMO_TASK_PRIORITY     20
#define WIFICMD_TASK_PRIORITY  21
extern void wifi_cmd_task(void *arg);
static ktask_t demo_task_obj;
static ktask_t nt_task_obj;
cpu_stack_t demo_task_buf[DEMO_TASK_STACKSIZE];
cpu_stack_t nt_task_buf[DEMO_TASK_STACKSIZE];
static kinit_t kinit;
extern int key_flag;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	GUI_KEY_A10 key = 0;

	switch (GPIO_Pin) {
		case KEY_1_Pin:
			key = GUI_KEY_1;
      // key_flag = 1;
			break;
		case KEY_2_Pin:
			key = GUI_KEY_2;
      ++key_flag;
			break;
		case KEY_3_Pin:
			key = GUI_KEY_3;
			break;
		default:
			return;
	}
	GUI_StoreKeyMsg(key, 1);
	GUI_StoreKeyMsg(key, 0);
}

void uData_report_demo(input_event_t *event, void *priv_data)
{
    udata_pkg_t buf;
    if ((event == NULL)||(event->type != EV_UDATA)) {
        return;
    }

    if(event->code == CODE_UDATA_REPORT_PUBLISH){
        int ret = 0;
        ret = uData_report_publish(&buf);
        if(ret == 0){
            barometer_data_t* data = (barometer_data_t*)buf.payload;
            printf("uData_application::::::::::::::type = (%d)\n", buf.type);
            printf("uData_application::::::::::::::data = (%d)\n", data->p);
            printf("uData_application:::::::::timestamp = (%d)\n", data->timestamp);
        }
    }
}

int application_start(int argc, char *argv[])
{
    int ret = 0;

    aos_register_event_filter(EV_UDATA, uData_report_demo, NULL);

    ret = uData_subscribe(UDATA_SERVICE_BARO);
    if(ret != 0){
        printf("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    aos_loop_run();

    return 0;
}
#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC
void test_sd_case(void)
{
	int fd, ret;
	char readBuffer[32] = {0};
	printf(" test_sd_case\n");
	
	  /* Fatfs write test */
	printf(" Fatfs write test\n");
	fd = aos_open(g_filepath, O_RDWR | O_CREAT | O_TRUNC);
	 printf("aos_open , ret = %d\n", fd);
	 if (fd > 0) {
	        ret = aos_write(fd, g_string, strlen(g_string));
	        printf("aos_write , ret = %d\n", ret);
	        ret = aos_sync(fd);
	        printf("aos_sync , ret = %d\n", ret);
	        aos_close(fd);
     }

      /* Fatfs read test */
     printf(" Fatfs read test\n");
    fd = aos_open(g_filepath, O_RDONLY);
    if (fd > 0) {
        ret = aos_read(fd, readBuffer, sizeof(readBuffer));
        printf("aos_read , readBuffer = %s\n", readBuffer);
        aos_close(fd);      
    }

	/* Fatfs mkdir test */
	printf(" Fatfs mkdir test\n");
    aos_dir_t *dp = (aos_dir_t *)aos_opendir(g_dirpath);
    if (!dp) {
        ret = aos_mkdir(g_dirpath);
        printf("aos_mkdir , ret = %d\n", ret);
    } else {
        ret = aos_closedir(dp);
        printf("aos_closedir , ret = %d\n", ret);
    }

    /* Fatfs readdir test */
	printf(" Fatfs readdir test\n");
    fd = aos_open(g_dirtest_1, O_RDWR | O_CREAT | O_TRUNC);
    if (fd > 0)
        aos_close(fd);

    fd = aos_open(g_dirtest_2, O_RDWR | O_CREAT | O_TRUNC);
    if (fd > 0)
        aos_close(fd);

    fd = aos_open(g_dirtest_3, O_RDWR | O_CREAT | O_TRUNC);
    if (fd > 0)
        aos_close(fd);

    dp = (aos_dir_t *)aos_opendir(g_dirpath);
    if (dp) {
        aos_dirent_t *out_dirent;
        while(1) {
            out_dirent = (aos_dirent_t *)aos_readdir(dp);
            if (out_dirent == NULL)
                break;

            printf("file name is %s\n", out_dirent->d_name);            
        }
    }
    aos_closedir(dp);

     /* Fatfs rename test */
	 printf(" Fatfs rename test\n");
    ret = aos_rename(g_filepath, g_new_filepath);
    printf("aos_rename , ret = %d\n", ret);

    fd = aos_open(g_filepath, O_RDONLY);
    if (fd >= 0)
        aos_close(fd);

    fd = aos_open(g_new_filepath, O_RDONLY);
     printf("aos_open , ret = %d\n", fd);
    if (fd > 0)
        aos_close(fd);

    /* Fatfs unlink test */
    ret = aos_unlink(g_new_filepath);
   printf("aos_unlink , ret = %d\n", ret);

    fd = aos_open(g_new_filepath, O_RDONLY);
     printf("aos_open , ret = %d\n", fd);
    if (fd > 0)
        aos_close(fd);
}
#endif

void demo_task(void *arg)
{
    int ret = 0;
    int count = 0;

    stm32_soc_init();

    kinit.argc = 0;
    kinit.argv = NULL; 
    kinit.cli_enable = 1;
    aos_kernel_init(&kinit);
#ifdef CONFIG_AOS_FATFS_SUPPORT_MMC	
	ret = fatfs_register();
	printf("reg_result = %d\n", ret);
	//if(ret == 0)
	//	test_sd_case();
#endif

    GUIDEMO_Main();

    while (1)
    {
        printf("hello world! count %d\n", count++);

        //sleep 1 second
        krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
    };
}

int main(void)
{
    krhino_init();
    krhino_task_create(&demo_task_obj, "demo_task", 0, DEMO_TASK_PRIORITY, 
        50, demo_task_buf, DEMO_TASK_STACKSIZE, demo_task, 1);

    krhino_task_create(&nt_task_obj, "wifi_cmd_task", 0,  WIFICMD_TASK_PRIORITY, 
        50, nt_task_buf, DEMO_TASK_STACKSIZE, wifi_cmd_task, 1);

    krhino_start();
    
    return 0;
}

