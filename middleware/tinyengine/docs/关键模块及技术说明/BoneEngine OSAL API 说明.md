## #API概览
下面列表为目前api的支持情况，'Y'代码支持，'N'代表不支持。

| api | AliosThings | FreeRTOS |
| --- | --- | --- |
| be\_osal\_init\_yloop | Y | Y |
| be\_osal\_start\_yloop | Y | Y |
| be\_osal\_schedule\_call | Y | Y |
| be\_osal\_post\_delayed\_action | Y | Y |
| be\_osal\_post\_event | Y | Y |
| be\_osal\_register\_event\_filter | Y | Y |
| be\_osal\_new\_task | Y | Y |
| be\_osal\_free\_task | Y | Y |
| be\_osal\_delay | Y | Y |


## #API说明

```plain
typedef struct {
    uint32_t time;
    uint16_t type;
    uint16_t code;
    size_t value;
    size_t extra;
} be_osal_input_event_t;

typedef void (*be_osal_event_cb)(be_osal_input_event_t *event, void *data);
typedef void (*be_osal_call_cb)(void *data);
typedef void (*be_osal_task)(void * data);
```

### #be\_osal\_init\_yloop

| 函数名称：int32\_t	 be\_osal\_init\_yloop(void) |
| --- |
| 功能描述：初始化yloop事件调度系统 |
| 参数说明：void |
| 函数返回：0=success  -1=fail |
| 注意事项：无 |


### #be\_osal\_start\_yloop

| 函数名称：void  be\_osal\_start\_yloop(void) |
| --- |
| 功能描述：启动yloop事件调度系统 |
| 参数说明：void |
| 函数返回：无 |
| 注意事项：无 |


### #be\_osal\_schedule\_call

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数名称：int32_t be_osal_schedule_call(be_osal_call_cb action, void *data)</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">功能描述：异步执行某回调函数</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">参数说明：</div>
          <ul data-type="unordered-list">
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">action:需要被执行的回调函数</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">data:传递给action的私有数据</div>
            </li>
          </ul>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数返回：0=success  -1=fail</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">注意事项：无</div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


### #be\_osal\_post\_delayed\_action

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数名称：int32_t be_osal_post_delayed_action(int32_t ms, be_osal_call_cb action, void *data)</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">功能描述：延迟异步执行某回调函数</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">参数说明：</div>
          <ul data-type="unordered-list">
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">ms:延迟执行的时间，单位是毫秒</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">action:需要被执行的函数</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">data:传递给action的私有数据</div>
            </li>
          </ul>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数返回：0=success  -1=fail</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">注意事项：无</div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


### #be\_osal\_post\_event

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数名称：int32_t be_osal_post_event(uint16_t type, uint16_t code, size_t value)</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">功能描述：触发事件</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">参数说明：</div>
          <ul data-type="unordered-list">
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">type:事件类型</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">code:事件编号</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">value:事件私有数据</div>
            </li>
          </ul>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数返回：0=success  -1=fail</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">注意事项：无</div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


### #be\_osal\_register\_event\_filter

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数名称：int32_t be_osal_register_event_filter(uint16_t type, be_osal_event_cb cb, void *data)</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">功能描述：注册事件</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">参数说明：</div>
          <ul data-type="unordered-list">
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">type:事件类型</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">cb:事件回调执行函数</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">data:传递给cb的私有数据</div>
            </li>
          </ul>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数返回：0=success  -1=fail</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">注意事项：无</div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


### #be\_osal\_new\_task

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数名称：int32_t be_osal_new_task(const char * name,be_osal_task task,void * data,int32_t size);</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">功能描述：创建新task</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">参数说明：</div>
          <ul data-type="unordered-list">
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">name:任务名称</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">task:task执行函数</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">data:传递给task的私有数据</div>
            </li>
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">size：栈大小</div>
            </li>
          </ul>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数返回：0=success  -1=fail</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">注意事项：无</div>
        </td>
      </tr>
    </tbody>
  </table>
</div>

### #be\_osal\_free\_task

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数名称：int32_t be_osal_free_task(void * handle);</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">功能描述：释放task</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">参数说明：</div>
          <ul data-type="unordered-list">
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">handle:task句柄</div>
            </li>
          </ul>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数返回：0=success  -1=fail</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">注意事项：无</div>
        </td>
      </tr>
    </tbody>
  </table>
</div>


### #be\_osal\_delay

<div class="bi-table">
  <table>
    <colgroup>
      <col width="90px" />
    </colgroup>
    <tbody>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数名称：void be_osal_delay(uint32_t ms);</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">功能描述：毫秒级延时</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">参数说明：</div>
          <ul data-type="unordered-list">
            <li data-type="list-item" data-list-type="unordered-list">
              <div data-type="p">ms:延时大小，单位毫秒</div>
            </li>
          </ul>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">函数返回：void</div>
        </td>
      </tr>
      <tr height="34px">
        <td rowspan="1" colspan="1">
          <div data-type="p">注意事项：无</div>
        </td>
      </tr>
    </tbody>
  </table>
</div>



## #实现原理
#### 1.AliosThings
AliosThings中实现了一套异步框架yloop([https://github.com/alibaba/AliOS-Things/wiki/Yloop-Event-Framework](https://github.com/alibaba/AliOS-Things/wiki/Yloop-Event-Framework))，能对IO、Timer、callback、event进行调度，所以针对AliosThings系统，只需要对原生API进行封装即可。

#### 2.FreeRTOS
FreeRTOS中实现架构如下：
![1.png | left | 684x333](https://cdn.yuque.com/lark/2018/png/66207/1522825001890-06dc7a08-e0e6-4a84-a2a4-9a718ee7fcd7.png "")
这里把callback、timer、event全部抽象成worker模型，利用最小堆来处理timer任务，当任务队列中没有可执行任务时，线程(或task)会进行阻塞。代码实现位于：[链接](http://gitlab.alibaba-inc.com/Gravity/LiteFrameWork/tree/gravity_lite_dev/platform/osal/freertos)

## #使用示范

使用示范请参考仓库[sample/os/vcall](http://gitlab.alibaba-inc.com/Gravity/LiteFrameWork/tree/gravity_lite_dev/samples/os/vcall)
 