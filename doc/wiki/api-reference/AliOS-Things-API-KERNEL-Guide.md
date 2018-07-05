# API INDEX

  * [1 aos_reboot](#1-aos_reboot)
  * [2 aos_get_hz](#2-aos_get_hz)
  * [3 aos_version_get](#3-aos_version_get)
  * [4 aos_task_new](#4-aos_task_new)
  * [5 aos_task_new_ext](#5-aos_task_new_ext)
  * [6 aos_task_exit](#6-aos_task_exit)
  * [7 aos_task_name](#7-aos_task_name)
  * [8 aos_task_key_create](#8-aos_task_key_create)
  * [9 aos_task_key_delete](#9-aos_task_key_delete)
  * [10 aos_task_setspecific](#10-aos_task_setspecific)
  * [11 aos_task_getspecific](#11-aos_task_getspecific)
  * [12 aos_mutex_new](#12-aos_mutex_new)
  * [13 aos_mutex_free](#13-aos_mutex_free)
  * [14 aos_mutex_lock](#14-aos_mutex_lock)
  * [15 aos_mutex_unlock](#15-aos_mutex_unlock)
  * [16 aos_mutex_is_valid](#16-aos_mutex_is_valid)
  * [17 aos_sem_new](#17-aos_sem_new)
  * [18 aos_sem_free](#18-aos_sem_free)
  * [19 aos_sem_wait](#19-aos_sem_wait)
  * [20 aos_sem_signal](#20-aos_sem_signal)
  * [21 aos_sem_is_valid](#21-aos_sem_is_valid)
  * [22 aos_sem_signal_all](#22-aos_sem_signal_all)
  * [23 aos_queue_new](#23-aos_queue_new)
  * [24 aos_queue_free](#24-aos_queue_free)
  * [25 aos_queue_send](#25-aos_queue_send)
  * [26 aos_queue_recv](#26-aos_queue_recv)
  * [27 aos_queue_is_valid](#27-aos_queue_is_valid)
  * [28 aos_queue_buf_ptr](#28-aos_queue_buf_ptr)
  * [29 aos_sched_disable](#29-aos_sched_disable)
  * [30 aos_sched_enable](#30-aos_sched_enable)
  * [31 aos_timer_new](#31-aos_timer_new)
  * [32 aos_timer_free](#32-aos_timer_free)
  * [33 aos_timer_start](#33-aos_timer_start)
  * [34 aos_timer_stop](#34-aos_timer_stop)
  * [35 aos_timer_change](#35-aos_timer_change)
  * [36 aos_workqueue_create](#36-aos_workqueue_create)
  * [37 aos_workqueue_del](#37-aos_workqueue_del)
  * [38 aos_work_init](#38-aos_work_init)
  * [39 aos_work_destroy](#39-aos_work_destroy)
  * [40 aos_work_run](#40-aos_work_run)
  * [41 aos_work_sched](#41-aos_work_sched)
  * [42 aos_work_cancel](#42-aos_work_cancel)
  * [43 aos_realloc](#43-aos_realloc)
  * [44 aos_malloc](#44-aos_malloc)
  * [45 aos_zalloc](#45-aos_zalloc)
  * [46 aos_alloc_trace](#46-aos_alloc_trace)
  * [47 aos_free](#47-aos_free)
  * [48 aos_now](#48-aos_now)
  * [49 aos_now_ms](#49-aos_now_ms)
  * [50 aos_msleep](#50-aos_msleep)

------

## 1-aos_reboot

```c
void aos_reboot(void)
```

- [x] **Description**

  Reboot AliOS.

- [x] **Parameters**

  None.

- [x] **Returns**

  None.


## 2-aos_get_hz

```c
int aos_get_hz(void)
```

- [x] **Description**

  Get HZ(ticks per second).

- [x] **Parameters**

  None.

- [x] **Returns**

  RHINO_CONFIG_TICKS_PER_SECOND.

## 3-aos_version_get

```c
const char *aos_version_get(void)
```

- [x] **Description**

  Get kernel version.

- [x] **Parameters**

  None.

- [x] **Returns**

  SYSINFO_KERNEL_VERSION.

## 4-aos_task_new

```c
int aos_task_new(const char *name, void (*fn)(void *), void *arg, int stack_size)
```

- [x] **Description**

  Create a task.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | name |      task name.   |
  | [in] | fn |        function to run.   |
  | [in] | arg |       argument of the function.   |
  | [in] | stacksize | stack-size in bytes. |

- [x] **Returns**

  0: success.

## 5-aos_task_new_ext

```c
int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
```

- [x] **Description**

  Create a task.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | task |       handle.   |
  | [in] | name |       task name.   |
  | [in] | fn |         task function.   |
  | [in] | arg |        argument of the function..   |
  | [in] | stack_buf |  stack-buf: if stack_buf==NULL, provided by kernel.   |
  | [in] | stack_size | stack-size in bytes.   |
  | [in] | prio |       priority value, the max is RHINO_CONFIG_USER_PRI_MAX(default 60). |

- [x] **Returns**

  0: success.

## 6-aos_task_exit

```c
void aos_task_exit(int code)
```

- [x] **Description**

  Exit a task.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | code | not used now. |

- [x] **Returns**

  None.


## 7-aos_task_name

```c
const char *aos_task_name(void)
```

- [x] **Description**

  Get task name.

- [x] **Parameters**

  None.

- [x] **Returns**

  the name of the task

## 8-aos_task_key_create

```c
int aos_task_key_create(aos_task_key_t *key)
```

- [x] **Description**

  Create a task key.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | key | pointer of key object. |

- [x] **Returns**

  0: success, -EINVAL: error.

## 9-aos_task_key_delete

```c
void aos_task_key_delete(aos_task_key_t key)
```

- [x] **Description**

  Delete a task key.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | key | key object. |

- [x] **Returns**

  None.


## 10-aos_task_setspecific

```c
int aos_task_setspecific(aos_task_key_t key, void *vp)
```

- [x] **Description**

  Associate a task-specific value with a key.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | key | key object.   |
  | [in] | vp |  pointer of a task-specific value. |

- [x] **Returns**

  the check status, 0 is OK, -1 indicates invalid.

## 11-aos_task_getspecific

```c
void *aos_task_getspecific(aos_task_key_t key)
```

- [x] **Description**

  Get the value currently bound to the specified key.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | key | key object. |

- [x] **Returns**

  None.


## 12-aos_mutex_new

```c
int aos_mutex_new(aos_mutex_t *mutex)
```

- [x] **Description**

  Alloc a mutex.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | mutex | pointer of mutex object, mutex object must be alloced,          hdl pointer in aos_mutex_t will refer a kernel obj internally. |

- [x] **Returns**

  0: success.

## 13-aos_mutex_free

```c
void aos_mutex_free(aos_mutex_t *mutex)
```

- [x] **Description**

  Free a mutex.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | mutex | mutex object, mem refered by hdl pointer in aos_mutex_t will          be freed internally. |

- [x] **Returns**

  None.


## 14-aos_mutex_lock

```c
int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
```

- [x] **Description**

  Lock a mutex.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | mutex |   mutex object, it contains kernel obj pointer which aos_mutex_new alloced.   |
  | [in] | timeout | waiting until timeout in milliseconds. |

- [x] **Returns**

  0: success.

## 15-aos_mutex_unlock

```c
int aos_mutex_unlock(aos_mutex_t *mutex)
```

- [x] **Description**

  Unlock a mutex.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | mutex | mutex object, it contains kernel obj pointer which oc_mutex_new alloced. |

- [x] **Returns**

  0: success.

## 16-aos_mutex_is_valid

```c
int aos_mutex_is_valid(aos_mutex_t *mutex)
```

- [x] **Description**

  This function will check if mutex is valid.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | mutex | pointer to the mutex. |

- [x] **Returns**

  0: success.

## 17-aos_sem_new

```c
int aos_sem_new(aos_sem_t *sem, int count)
```

- [x] **Description**

  Alloc a semaphore.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [out] | sem |   pointer of semaphore object, semaphore object must be alloced,hdl pointer in aos_sem_t will refer a kernel obj internally.   |
  | [in] |  count | initial semaphore counter. |

- [x] **Returns**

  0:success.

## 18-aos_sem_free

```c
void aos_sem_free(aos_sem_t *sem)
```

- [x] **Description**

  Destroy a semaphore.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | sem | pointer of semaphore object, mem refered by hdl pointer        in aos_sem_t will be freed internally. |

- [x] **Returns**

  None.


## 19-aos_sem_wait

```c
int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
```

- [x] **Description**

  Acquire a semaphore.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | sem |     semaphore object, it contains kernel obj pointer which aos_sem_new alloced.   |
  | [in] | timeout | waiting until timeout in milliseconds. |

- [x] **Returns**

  0: success.

## 20-aos_sem_signal

```c
void aos_sem_signal(aos_sem_t *sem)
```

- [x] **Description**

  Release a semaphore.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | sem | semaphore object, it contains kernel obj pointer which aos_sem_new alloced. |

- [x] **Returns**

  None.


## 21-aos_sem_is_valid

```c
int aos_sem_is_valid(aos_sem_t *sem)
```

- [x] **Description**

  This function will check if semaphore is valid.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | sem | pointer to the semaphore. |

- [x] **Returns**

  0: success.

## 22-aos_sem_signal_all

```c
void aos_sem_signal_all(aos_sem_t *sem)
```

- [x] **Description**

  Release all semaphore.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | sem | semaphore object, it contains kernel obj pointer which aos_sem_new alloced. |

- [x] **Returns**

  None.


## 23-aos_queue_new

```c
int aos_queue_new(aos_queue_t *queue, void *buf, unsigned int size, int max_msg)
```

- [x] **Description**

  This function will create a queue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | queue |   pointer to the queue(the space is provided by user).   |
  | [in] | buf |     buf of the queue(provided by user).   |
  | [in] | size |    the bytes of the buf.   |
  | [in] | max_msg | the max size of the msg. |

- [x] **Returns**

  0: success.

## 24-aos_queue_free

```c
void aos_queue_free(aos_queue_t *queue)
```

- [x] **Description**

  This function will delete a queue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | queue | pointer to the queue. |

- [x] **Returns**

  None.


## 25-aos_queue_send

```c
int aos_queue_send(aos_queue_t *queue, void *msg, unsigned int size)
```

- [x] **Description**

  This function will send a msg to the front of a queue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | queue | pointer to the queue.   |
  | [in] | msg |   msg to send.   |
  | [in] | size |  size of the msg. |

- [x] **Returns**

  0: success.

## 26-aos_queue_recv

```c
int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg,unsigned int *size)
```

- [x] **Description**

  This function will receive msg from a queue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] |  queue | pointer to the queue.   |
  | [in] |  ms |    ms to wait before receive.   |
  | [out] | msg |   buf to save msg.   |
  | [out] | size |  size of the msg. |

- [x] **Returns**

  0: success.

## 27-aos_queue_is_valid

```c
int aos_queue_is_valid(aos_queue_t *queue)
```

- [x] **Description**

  This function will check if queue is valid.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | queue | pointer to the queue. |

- [x] **Returns**

  0: success.

## 28-aos_queue_buf_ptr

```c
void *aos_queue_buf_ptr(aos_queue_t *queue)
```

- [x] **Description**

  This function will return buf ptr if queue is inited.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | queue | pointer to the queue. |

- [x] **Returns**

  NULL: error.

## 29-aos_sched_disable

```c
int aos_sched_disable(void)
```

- [x] **Description**

  This function will disable kernel sched.

- [x] **Parameters**

  None.

- [x] **Returns**

  the operation status, 0 is OK, others is error.

## 30-aos_sched_enable

```c
int aos_sched_enable(void)
```

- [x] **Description**

  This function will enable kernel sched.

- [x] **Parameters**

  None.

- [x] **Returns**

  0: success.

## 31-aos_timer_new

```c
int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
```

- [x] **Description**

  This function will create a timer.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | timer |  pointer to the timer.   |
  | [in] | fn |     callbak of the timer.   |
  | [in] | arg |    the argument of the callback.   |
  | [in] | ms |     ms of the normal timer triger.   |
  | [in] | repeat | repeat or not when the timer is created. |

- [x] **Returns**

  0: success.

## 32-aos_timer_free

```c
void aos_timer_free(aos_timer_t *timer)
```

- [x] **Description**

  This function will delete a timer.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | timer | pointer to a timer. |

- [x] **Returns**

  None.


## 33-aos_timer_start

```c
int aos_timer_start(aos_timer_t *timer)
```

- [x] **Description**

  This function will start a timer.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | timer | pointer to the timer. |

- [x] **Returns**

  0: success.

## 34-aos_timer_stop

```c
int aos_timer_stop(aos_timer_t *timer)
```

- [x] **Description**

  This function will stop a timer.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | timer | pointer to the timer. |

- [x] **Returns**

  0: success.

## 35-aos_timer_change

```c
int aos_timer_change(aos_timer_t *timer, int ms)
```

- [x] **Description**

  This function will change attributes of a timer.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | timer | pointer to the timer.   |
  | [in] | ms |    ms of the timer triger. |

- [x] **Returns**

  0: success.

## 36-aos_workqueue_create

```c
int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size)
```

- [x] **Description**

  This function will creat a workqueue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | workqueue |  the workqueue to be created.   |
  | [in] | pri |        the priority of the worker.   |
  | [in] | stack_size | the size of the worker-stack. |

- [x] **Returns**

  0: success.

## 37-aos_workqueue_del

```c
void aos_workqueue_del(aos_workqueue_t *workqueue)
```

- [x] **Description**

  This function will delete a workqueue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | workqueue | the workqueue to be deleted. |

- [x] **Returns**

  None.


## 38-aos_work_init

```c
int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
```

- [x] **Description**

  This function will initialize a work.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | work | the work to be initialized.   |
  | [in] | fn |   the call back function to run.   |
  | [in] | arg |  the paraments of the function.   |
  | [in] | dly |  ms to delay before run. |

- [x] **Returns**

  0: success.

## 39-aos_work_destroy

```c
void aos_work_destroy(aos_work_t *work)
```

- [x] **Description**

  This function will destroy a work.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | work | the work to be destroied. |

- [x] **Returns**

  None.


## 40-aos_work_run

```c
int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
```

- [x] **Description**

  This function will run a work on a workqueue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | workqueue | the workqueue to run work.   |
  | [in] | work |      the work to run. |

- [x] **Returns**

  0: success.

## 41-aos_work_sched

```c
int aos_work_sched(aos_work_t *work)
```

- [x] **Description**

  This function will run a work on the default workqueue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | work | the work to run. |

- [x] **Returns**

  0: success.

## 42-aos_work_cancel

```c
int aos_work_cancel(aos_work_t *work)
```

- [x] **Description**

  This function will cancel a work on the default workqueue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | work | the work to cancel. |

- [x] **Returns**

  0: success.

## 43-aos_realloc

```c
void *aos_realloc(void *mem, unsigned int size)
```

- [x] **Description**

  Realloc memory.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | mem |  current memory address point.   |
  | [in] | size | new size of the mem to remalloc. |

- [x] **Returns**

  NULL: error.

## 44-aos_malloc

```c
void *aos_malloc(unsigned int size)
```

- [x] **Description**

  Alloc memory.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | size | size of the mem to malloc. |

- [x] **Returns**

  NULL: error.

## 45-aos_zalloc

```c
void *aos_zalloc(unsigned int size)
```

- [x] **Description**

  Alloc memory and clear to zero.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | size | size of the mem to malloc. |

- [x] **Returns**

  NULL: error.

## 46-aos_alloc_trace

```c
void aos_alloc_trace(void *addr, size_t allocator)
```

- [x] **Description**

  Trace alloced mems.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | addr |      pointer of the mem alloced by malloc.   |
  | [in] | allocator | buildin_address. |

- [x] **Returns**

  None.


## 47-aos_free

```c
void aos_free(void *mem)
```

- [x] **Description**

  Free memory.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | ptr | address point of the mem. |

- [x] **Returns**

  None.


## 48-aos_now

```c
long long aos_now(void)
```

- [x] **Description**

  Get current time in nano seconds.

- [x] **Parameters**

  None.

- [x] **Returns**

  elapsed time in nano seconds from system starting.

## 49-aos_now_ms

```c
long long aos_now_ms(void)
```

- [x] **Description**

  Get current time in mini seconds.

- [x] **Parameters**

  None.

- [x] **Returns**

  elapsed time in mini seconds from system starting.

## 50-aos_msleep

```c
void aos_msleep(int ms)
```

- [x] **Description**

  Msleep.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | ms | sleep time in milliseconds. |

- [x] **Returns**

  None.

