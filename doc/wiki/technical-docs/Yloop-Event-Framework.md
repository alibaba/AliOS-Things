EN | [中文](Yloop-Event-Framework.zh)

## Yloop Overview

Yloop is an asynchronous event framework of AliOS Things, learned from [libuv](https://github.com/libuv/libuv) and event loop used in traditional embedded systems. Comprehensively considering usage complexity, performance and footprint, it provides an event scheduling mechanism suitable for MCU.

### Yloop context

Each Yloop instance (aos_loop_t) is bound to a specific task context. The context of the program entry of AliOS Things (application_start) is linked to the system's master Yloop instance, which is also called main task. Other tasks can also create their own Yloop instances.

### Yloop scheduling

Yloop can schedule IO, timer, callback and event uniformly.

- IO: the most popular one is Socket, but devices managed by VFS of AliOS Things are also included.
- timer: just those common timers 
- callback: specific function
- event: including system events and user events

When aos_loop_run is called, current task will stop and wait for the execution of above events.

### The principle of Yloop implementation

Yloop uses the interface (select) of protocol stack to schedule IO and timer. Besides that, AliOS Things's own protocol stack provides another specific interface (eventfd), which Yloop uses to associate with VFS's device files, achieving unified scheduling of events on the whole system.

## The use of Yloop

### Start from hello world

There's such a piece of code in [hello world example](https://github.com/alibaba/AliOS-Things/blob/master/example/helloworld/helloworld.c):

```c
static void app_delayed_action(void *arg)
{
    LOG("%s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    aos_post_delayed_action(5000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();

    return 0;
}
```

Two steps are executed in application_start:

- call aos_post_delayed_action to create a one-second timer (there's only oneshot timer in Yloop)

- call aos_loop_run to start event loop



One second later, the timer is triggered, and app_delayed_action is called, who:

- call LOG printing

- create a five-second timer to achieve the regular implement of app_delayed_action



P.S.: the program doesn't need aos_loop_init () to create a Yloop instance, since the system will automatically create the master Yloop instance.

### Connection with Socket

Take [framework/connectivity/mqtt/mqtt_client.c](https://github.com/alibaba/AliOS-Things/blob/master/framework/connectivity/mqtt/mqtt_client.c) in MQTT as an example: 

```
static int iotx_mc_connect(iotx_mc_client_t *pClient)
{
    <snip>
    rc = MQTTConnect(pClient);
    <snip>
    aos_poll_read_fd(get_ssl_fd(), cb_recv, pClient);
    <snip>
}
```

When socket connection is established with the server, aos_poll_read_fd () will be called to add the socket of MQTT as execution objects of Yloop. Once the server sends out data, cb_recv callback will be called to process the data, so that MQTT doesn't need to create a single task to handle it, thus saving memory usage. In addition, since all the processing is done in main task, no complex mutex operation is required.

### The processing of system events

AliOS Things defines a series of system events, such as WiFi events. Programs can register events executor functions through aos_register_event_filter () to carry out corresponding processing.

```
static void netmgr_events_executor(input_event_t *eventinfo, void *priv_data)
{
    switch ((eventinfo->code) {
        case CODE_WIFI_ON_CONNECTED:
            <do somthing>
        <snip>
    }
}

aos_register_event_filter(EV_WIFI, event_cb, NULL);
```

`#define EV_USER     0x1000`
events ID after EV_USER can be used for users events.

### Yloop callback

Yloop callback is used for cross-task processing. The following pseudo code is taken as an example: 

```
void do_uart_io_in_main_task(void *arg)
{
    <do something>
}

void io_recv_data_cb(char c)
{
    aos_schedule_call(do_uart_io_in_main_task, (void *)(long)c);
}
```

Suppose uart_recv_data_cb is the callback code when IO devices receive data. Once IO devices receive the data, aos_schedule_call will put do_uart_io_in_main_task into the context of main task's to execute. In this way, there is no need to consider concurrency and mutex operation when carry out the logical processing of do_uart_io_in_main_task data.

### Notes

All the APIs of Yloop([include/aos/yloop.h](AliOS-Things-API-YLOOP-Guide)) except the following ones need to be executed in its bounded context:

- aos_schedule_call
- aos_loop_schedule_call
- aos_loop_schedule_work
- aos_cancel_work
- aos_post_event

## Summary

As an event framework of AliOS Things, Yloop is highly combined with VFS and protocol stacks, which achieves a good footprint and can adapt to systems with only one main task as well as those with requirement for concurrency.
