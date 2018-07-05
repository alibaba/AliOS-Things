# API INDEX

  * [1 trace_hal_init](#1-trace_hal_init)
  * [2 trace_hal_send](#2-trace_hal_send)
  * [3 trace_hal_recv](#3-trace_hal_recv)
  * [4 trace_hal_deinit](#4-trace_hal_deinit)

------

## 1-trace_hal_init

```c
void *trace_hal_init(void)
```

- [x] **Description**

  trace data transfer init.

- [x] **Parameters**

  None.

- [x] **Returns**

  None.


## 2-trace_hal_send

```c
ssize_t trace_hal_send(void *handle, void *buf, size_t len)
```

- [x] **Description**

  trace data transfer send.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | handle | data transfer channel object   |
  | [in] | buf |    the buffer store data   |
  | [in] | len |    the len of data |

- [x] **Returns**

  the size send success.

## 3-trace_hal_recv

```c
ssize_t trace_hal_recv(void *handle, void *buf)
```

- [x] **Description**

  trace data transfer receive.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | handle | data transfer channel object   |
  | [in] | buf |    the buffer to store data |

- [x] **Returns**

  the size receive success.

## 4-trace_hal_deinit

```c
void trace_hal_deinit(void *handle)
```

- [x] **Description**

  trace data transfer init.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | handle | data transfer channel object |

- [x] **Returns**

  None.

