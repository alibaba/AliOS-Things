# API INDEX

  * [1 aos_register_event_filter](#1-aos_register_event_filter)
  * [2 aos_unregister_event_filter](#2-aos_unregister_event_filter)
  * [3 aos_post_event](#3-aos_post_event)
  * [4 aos_poll_read_fd](#4-aos_poll_read_fd)
  * [5 aos_cancel_poll_read_fd](#5-aos_cancel_poll_read_fd)
  * [6 aos_post_delayed_action](#6-aos_post_delayed_action)
  * [7 aos_cancel_delayed_action](#7-aos_cancel_delayed_action)
  * [8 aos_schedule_call](#8-aos_schedule_call)
  * [9 aos_loop_init](#9-aos_loop_init)
  * [10 aos_current_loop](#10-aos_current_loop)
  * [11 aos_loop_run](#11-aos_loop_run)
  * [12 aos_loop_exit](#12-aos_loop_exit)
  * [13 aos_loop_destroy](#13-aos_loop_destroy)
  * [14 aos_loop_schedule_call](#14-aos_loop_schedule_call)
  * [15 aos_loop_schedule_work](#15-aos_loop_schedule_work)
  * [16 aos_cancel_work](#16-aos_cancel_work)

------

## 1-aos_register_event_filter

```c
int aos_register_event_filter(uint16_t type, aos_event_cb cb, void *priv)
```

- [x] **Description**

  Register system event filter callback.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | type | event type interested.   |
  | [in] | cb |   system event callback.   |
  | [in] | priv | private data past to cb. |

- [x] **Returns**

  the operation status, 0 is OK, others is error.

## 2-aos_unregister_event_filter

```c
int aos_unregister_event_filter(uint16_t type, aos_event_cb cb, void *priv)
```

- [x] **Description**

  Unregister native event callback.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | type | event type interested.   |
  | [in] | cb |   system event callback.   |
  | [in] | priv | private data past to cb. |

- [x] **Returns**

  the operation status, 0 is OK, others is error.

## 3-aos_post_event

```c
int aos_post_event(uint16_t type, uint16_t code, unsigned long  value)
```

- [x] **Description**

  Post local event.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | type |  event type.   |
  | [in] | code |  event code.   |
  | [in] | value | event value. |

- [x] **Returns**

  the operation status, 0 is OK,others is error.

## 4-aos_poll_read_fd

```c
int aos_poll_read_fd(int fd, aos_poll_call_t action, void *param)
```

- [x] **Description**

  Register a poll event in main loop.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | fd |     poll fd.   |
  | [in] | action | action to be executed.   |
  | [in] | param |  private data past to action. |

- [x] **Returns**

  the operation status, 0 is OK,others is error.

## 5-aos_cancel_poll_read_fd

```c
void aos_cancel_poll_read_fd(int fd, aos_poll_call_t action, void *param)
```

- [x] **Description**

  Cancel a poll event to be executed in main loop.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | fd |     poll fd.   |
  | [in] | action | action to be executed.   |
  | [in] | param |  private data past to action. |

- [x] **Returns**

  None.


## 6-aos_post_delayed_action

```c
int aos_post_delayed_action(int ms, aos_call_t action, void *arg)
```

- [x] **Description**

  Post a delayed action to be executed in main loop.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | ms |     milliseconds to wait.   |
  | [in] | action | action to be executed.   |
  | [in] | arg |    private data past to action. |

- [x] **Returns**

  the operation status, 0 is OK,others is error.

## 7-aos_cancel_delayed_action

```c
void aos_cancel_delayed_action(int ms, aos_call_t action, void *arg)
```

- [x] **Description**

  Cancel a delayed action to be executed in main loop.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | ms |     milliseconds to wait, -1 means don't care.   |
  | [in] | action | action to be executed.   |
  | [in] | arg |    private data past to action. |

- [x] **Returns**

  None.


## 8-aos_schedule_call

```c
int aos_schedule_call(aos_call_t action, void *arg)
```

- [x] **Description**

  Schedule a callback in next event loop.
  Unlike aos_post_delayed_action,
  this function can be called from non-aos-main-loop context.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | action | action to be executed.   |
  | [in] | arg |    private data past to action. |

- [x] **Returns**

  the operation status, <0 is error,others is OK.

## 9-aos_loop_init

```c
aos_loop_t aos_loop_init(void)
```

- [x] **Description**

  Init a per-task event loop.

- [x] **Parameters**

  None.

- [x] **Returns**

  the handler of aos_loop_t,NULL failure,others success.

## 10-aos_current_loop

```c
aos_loop_t aos_current_loop(void)
```

- [x] **Description**

  Get current event loop.

- [x] **Parameters**

  None.

- [x] **Returns**

  default event loop.

## 11-aos_loop_run

```c
void aos_loop_run(void)
```

- [x] **Description**

  Start event loop.

- [x] **Parameters**

  None.

- [x] **Returns**

  None.


## 12-aos_loop_exit

```c
void aos_loop_exit(void)
```

- [x] **Description**

  Exit event loop, aos_loop_run() will return.

- [x] **Parameters**

  None.

- [x] **Returns**

  None.


## 13-aos_loop_destroy

```c
void aos_loop_destroy(void)
```

- [x] **Description**

  Free event loop resources.

- [x] **Parameters**

  None.

- [x] **Returns**

  None.


## 14-aos_loop_schedule_call

```c
int aos_loop_schedule_call(aos_loop_t *loop, aos_call_t action, void *arg)
```

- [x] **Description**

  Schedule a callback specified event loop.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | loop |   event loop to be scheduled, NULL for default main loop.   |
  | [in] | action | action to be executed.   |
  | [in] | arg |    private data past to action. |

- [x] **Returns**

  the operation status, <0 is error,others is OK.

## 15-aos_loop_schedule_work

```c
void *aos_loop_schedule_work(int ms, aos_call_t action, void *arg1,
                             aos_call_t fini_cb, void *arg2)
```

- [x] **Description**

  Schedule a work to be executed in workqueue.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | ms |      milliseconds to delay before execution, 0 means immediately.   |
  | [in] | action |  action to be executed.   |
  | [in] | arg1 |    private data past to action.   |
  | [in] | fini_cb | finish callback to be executed after action is done in current event loop.   |
  | [in] | arg2 |    data past to fini_cb. |

- [x] **Returns**

  work handle,NULL failure,others is OK.

## 16-aos_cancel_work

```c
void aos_cancel_work(void *work, aos_call_t action, void *arg1)
```

- [x] **Description**

  Cancel a work.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | work |   work to be cancelled.   |
  | [in] | action | action to be executed.   |
  | [in] | arg1 |   private data past to action. |

- [x] **Returns**

  None.

