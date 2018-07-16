[EN](Yloop-Event-Framework) | 中文

## Yloop概要

Yloop是AliOS Things的异步事件框架。Yloop借鉴了，libuv及嵌入式业界常见的event loop，综合考虑使用复杂性，性能，及footprint，实现了一个适合于MCU的事件调度机制。

### Yloop上下文
每个Yloop实例（aos_loop_t）与特定的任务上下文绑定，AliOS Things的程序入口application_start所在的上下文与系统的主Yloop实例绑定，该上下文也称为主任务。主任务以外的任务也可以创建自己的Yloop实例。

### Yloop调度
Yloop实现了对IO，timer，callback，event的统一调度管理：
- IO：最常见的是Socket，也可以是AliOS Things的vfs管理的设备
- timer：即常见的定时器
- callback：特定的执行函数
- event：包括系统事件，用户自定义事件

当调用aos_loop_run后，当前任务将会等待上述的各类事件发生。

### Yloop实现原理
Yloop利用协议栈的select接口实现了对IO及timer的调度。AliOS Things自带的协议栈又暴露一个特殊的eventfd接口，Yloop利用此接口把VFS的设备文件，和eventfd关联起来，实现了对整个系统的事件的统一调度。

## Yloop的使用
### 从hello world开始

[hello world example](https://github.com/alibaba/AliOS-Things/blob/master/example/helloworld/helloworld.c)
里面有这样一段代码：
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

application_start里面做了两件事情：
* 调用aos_post_delayed_action创建了一个1秒的定时器（Yloop里面只有oneshot timer）
* 调用aos_loop_run进入事件循环

1秒后，定时器触发，app_delayed_action被调用，而app_delayed_action里面
* 调用LOG打印
* 再次创建一个5秒的定时器，重而实现定期执行app_delayed_action

这里注意到，程序并不需要aos_loop_init()去创建Yloop实例，因为系统会默认自动创建主Yloop实例。

### 和Socket的结合
以mqtt的[framework/connectivity/mqtt/mqtt_client.c](https://github.com/alibaba/AliOS-Things/blob/master/framework/connectivity/mqtt/mqtt_client.c)作为例子：
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
在和服务端建立好socket连接后，调用aos_poll_read_fd()把mqtt的socket加入到Yloop的监听对象里。当服务端有数据过来时，cb_recv回调将被调用，进行数据的处理。这样，mqtt就不需要一个单独的任务来处理socket，从而节省内存使用。同时，由于所有处理都是在主任务进行，不需要复杂的互斥操作。

### 系统事件的处理
AliOS Things定义了一系列系统事件，程序可以通过aos_register_event_filter()注册事件监听函数，进行相应的处理，比如WiFi事件。
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
EV_USER以后的事件ID可以用于用户自定义的事件。

### Yloop回调
Yloop回调用于跨任务的处理。以下面伪代码为例：
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
假设uart_recv_data_cb是IO设备收到数据时的回调，收到数据后通过aos_schedule_call把实际处理do_uart_io_in_main_task放到主任务上下文去执行。这样，数据的逻辑处理do_uart_io_in_main_task就不需要考虑并发，而去做复杂的互斥操作。

### 注意事项
Yloop的API([include/aos/yloop.h](AliOS-Things-API-YLOOP-Guide))除了下述API，都必须在Yloop实例所绑定的任务的上下文执行：
* aos_schedule_call
* aos_loop_schedule_call
* aos_loop_schedule_work
* aos_cancel_work
* aos_post_event

## 小结
Yloop作为AliOS Things的事件框架，和VFS，协议栈深度结合，在取得较好的footprint的同时，能较好地适应于对footprint要求较高只有一个主任务的系统，也可以适用于对处理的并发性要求较高的系统。
