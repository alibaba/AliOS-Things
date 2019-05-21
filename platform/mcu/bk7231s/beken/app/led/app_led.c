#include "include.h"

// led show spi data state for tianzhiheng
#if CFG_SUPPORT_TIANZHIHENG_DRONE
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_pub.h"
#include "error.h"
#include "fake_clock_pub.h"
#include "mem_pub.h"
#include "gpio_pub.h"
#include "app_led.h"


#include "uart_pub.h"
#define LED_DEBUG
#ifdef LED_DEBUG
#define LED_PRT      os_printf
#define LED_WARN     warning_prf
#define LED_FATAL    fatal_prf
#else
#define LED_PRT      null_prf
#define LED_WARN     null_prf
#define LED_FATAL    null_prf
#endif

#define LED_QITEM_COUNT             (2)
#define LED_GPIO_INDEX              GPIO29     // GPIO19
#define LED_INITIAL_VAL             1
#define LED_ON_VAL                  0          // LOW   LED ON
#define LED_DISCONNECT_VAL          125        // ms
#define LED_MONITOR_VAL             50         // ms
#define LED_SOFTAP_VAL              250        // ms

typedef struct led_st
{
    beken_timer_t led_timer;
    DEV_STATE state;
    GPIO_INDEX gpio_idx;
} LED_ST, LED_PTR;

typedef struct led_message
{
    DEV_STATE led_msg;
} LED_MSG_T;

static LED_ST ledctr;
beken_queue_t led_msg_que = NULL;
xTaskHandle led_thread_handle = NULL;

void app_led_send_msg(DEV_STATE new_msg)
{
    OSStatus ret;
    LED_MSG_T msg;

    if(led_msg_que)
    {
        msg.led_msg = new_msg;

        ret = rtos_push_to_queue(&led_msg_que, &msg, BEKEN_NO_WAIT);
        if(kNoErr != ret)
        {
            os_printf("app_led_send_msg failed\r\n");
        }
    }
}

static void app_led_timer_poll_handler(void)
{
    OSStatus err;

    bk_gpio_output_reverse(ledctr.gpio_idx);

    err = rtos_reload_timer(&ledctr.led_timer);
    ASSERT(kNoErr == err);
}

static void app_led_timer_handler(void *data)
{
    app_led_send_msg(TIMER_POLL);
}

static void app_led_poll_handler(DEV_STATE next_sta)
{
    uint32_t intval = 0;
    OSStatus err;

    //LED_PRT("state:%d, next_sta:%d\r\n", ledctr.state, next_sta);

    if(ledctr.state == next_sta)
        return;

    err = rtos_stop_timer(&ledctr.led_timer);
    ASSERT(kNoErr == err);

    switch(next_sta)
    {
    case STA_NONE:
        break;

    case POWER_ON:
    case LED_DISCONNECT:
    {
        intval = LED_DISCONNECT_VAL;
        ledctr.state = LED_DISCONNECT;
        break;
    }

    case MONITOR_MODE:
    {
        intval = LED_MONITOR_VAL;
        ledctr.state = MONITOR_MODE;
        break;
    }

    case SOFTAP_MODE:
    {
        intval = LED_SOFTAP_VAL;
        ledctr.state = SOFTAP_MODE;
        break;
    }

    case LED_CONNECT:
    {
        intval = 0;
        ledctr.state = LED_CONNECT;
        bk_gpio_output(ledctr.gpio_idx, LED_ON_VAL);
        break;
    }

    default:
        break;
    }

    if(intval)
    {
        err = rtos_change_period(&ledctr.led_timer, intval);
        ASSERT(kNoErr == err);
    }
}

static void app_led_main( beken_thread_arg_t data )
{
    OSStatus err;

    os_memset(&ledctr, 0, sizeof(LED_ST));
    ledctr.state = STA_NONE;

    ledctr.gpio_idx = LED_GPIO_INDEX;

    bk_gpio_config_output(ledctr.gpio_idx);
    bk_gpio_output(ledctr.gpio_idx, LED_INITIAL_VAL);

    err = rtos_init_timer(&ledctr.led_timer,
                          1 * 1000,
                          app_led_timer_handler,
                          (void *)0);
    ASSERT(kNoErr == err);

    err = rtos_start_timer(&ledctr.led_timer);
    ASSERT(kNoErr == err);

    while(1)
    {
        LED_MSG_T msg;
        err = rtos_pop_from_queue(&led_msg_que, &msg, BEKEN_WAIT_FOREVER);
        if(kNoErr == err)
        {
            switch(msg.led_msg)
            {
            case TIMER_POLL:
                app_led_timer_poll_handler();
                break;
            default:
                app_led_poll_handler(msg.led_msg);
                break;
            }
        }
    }

app_led_exit:
    LED_PRT("app_led_main exit\r\n");

    rtos_deinit_queue(led_msg_que);
    led_msg_que = NULL;

    led_thread_handle = NULL;
    rtos_delete_thread(NULL);

}

UINT32 app_led_init(void)
{
    int ret;

    LED_PRT("app_led_init %d\r\n");
    if((!led_thread_handle) && (!led_msg_que))
    {
        ret = rtos_init_queue(&led_msg_que,
                              "led_queue",
                              sizeof(LED_MSG_T),
                              LED_QITEM_COUNT);
        if (kNoErr != ret)
        {
            LED_PRT("temp detect ceate queue failed\r\n");
            return kGeneralErr;
        }

        ret = rtos_create_thread(&led_thread_handle,
                                 BEKEN_DEFAULT_WORKER_PRIORITY,
                                 "app led",
                                 (beken_thread_function_t)app_led_main,
                                 1024,
                                 NULL);
        if (ret != kNoErr)
        {
            rtos_deinit_queue(led_msg_que);
            led_msg_que = NULL;
            LED_PRT("Error: Failed to create app_led_init: %d\r\n", ret);
            return kGeneralErr;
        }
    }

    return kNoErr;
}
#endif

