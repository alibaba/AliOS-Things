
# _thread - 多线程

* 模块功能:  
_thread 模块提供创建新线程的方法，并提供互斥锁。

## get_ident - 获取当前线程ID

* 函数原型：
> _thread.get_ident()

* 返回值：  
获取当前线程号。

* 示例：

```python
>>> _thread.get_ident()
1073495904
```

## stack_size - 设置创建新线程时所使用的栈大小

* 函数原型：
> _thread.stack_size(size)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
| size | int | 是 | 创建新线程使用的栈大小（以字节为单位），默认为8448字节，最小8192字节。 |

## start_new_thread - 创建一个新线程

* start_new_thread - 函数原型：
> _thread.start_new_thread(function, args)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|function|函数指针|是|新线程的入口函数|
|args|元组|是|传入新线程的入口函数的参数列表|

* 返回值：  
返回线程的id。

## stop_thread - 根据线程id删除一个线程

* 函数原型：
> _thread.stop_thread(thread_id)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|thread_id|int|是|创建线程时返回的线程id <br> thread_id为0则删除当前线程。不可删除主线程。|

## allocate_lock - 创建一个互斥锁对象

* 函数原型：
> _thread.allocate_lock()

* 返回值：  
返回互斥锁对象

* 示例：

```python
lock = _thread.allocate_lock()
```

## acquire - 获取锁

* 函数原型：
> lock.acquire()

* 返回值
成功获得锁返回True，否则返回False。

## release - 释放锁

* 函数原型：
> lock.release()

## locked - 返回锁的状态

* 函数原型：
> lock.locked()

* 返回值：  
返回锁的状态，True表示被某个线程占用，False则表示没有被占用。

## delete_lock - 删除锁

* 函数原型：
> _thread.delete_lock(lock)

* 示例：

```python

import _thread
import utime
import urandom

gLock = None

# 线程1处理函数
def th(name):

    while True:
        # 休眠随机时间
        utime.sleep(urandom.random())

        # 等待获取互斥锁
        gLock.acquire()

        # 打印当前系统时间
        print(name, "lock acquired at ", utime.ticks_ms())

        # 休眠随机时间
        utime.sleep(urandom.random())

        # 释放互斥锁
        gLock.release()

if __name__ == '__main__':
    #创建互斥锁
    gLock = _thread.allocate_lock()

    # 获得互斥锁
    gLock.acquire()

    # 创建线程1
    _thread.start_new_thread(th, ["th1"])
    # 创建线程2
    _thread.start_new_thread(th, ["th2"])
    # 休眠1秒
    utime.sleep(1)

    # 释放互斥锁
    gLock.release()
    
    while True:
        utime.sleep(1)

```

* 示例输出：

```python
th1 lock acquired at  6978
th2 lock acquired at  7965
th1 lock acquired at  8949
th2 lock acquired at  9405
th1 lock acquired at  10083
th2 lock acquired at  10459
th1 lock acquired at  11171
th2 lock acquired at  11980
th1 lock acquired at  12970
```