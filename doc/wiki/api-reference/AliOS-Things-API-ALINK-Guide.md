# API INDEX

  * [1 alink_set_loglevel](#1-alink_set_loglevel)
  * [2 alink_enable_sandbox_mode](#2-alink_enable_sandbox_mode)
  * [3 alink_enable_daily_mode](#3-alink_enable_daily_mode)
  * [4 alink_start](#4-alink_start)
  * [5 alink_wait_connect](#5-alink_wait_connect)
  * [6 alink_end](#6-alink_end)
  * [7 alink_factory_reset](#7-alink_factory_reset)
  * [8 alink_report](#8-alink_report)
  * [9 alink_register_callback](#9-alink_register_callback)
  * [10 awss_start](#10-awss_start)
  * [11 awss_stop](#11-awss_stop)

------

## 1-alink_set_loglevel

```c
void alink_set_loglevel(enum ALINK_LOG_LEVEL loglevel)
```

- [x] **Description**

  Log level control

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | loglevel | ALINK_LOG_LEVEL |

- [x] **Returns**

  None.


## 2-alink_enable_sandbox_mode

```c
int alink_enable_sandbox_mode(void)
```

- [x] **Description**

  Enable sandbox mode, for debug

- [x] **Parameters**

  None.

- [x] **Returns**

  0 on success, otherwise -1 will return

## 3-alink_enable_daily_mode

```c
int alink_enable_daily_mode(const char *server_ip, int port)
```

- [x] **Description**

  Enable daily mode, for debug

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | server_ip | IP Addr   |
  | [in] | port |      server port |

- [x] **Returns**

  0 on success, otherwise -1 will return

## 4-alink_start

```c
int alink_start(void)
```

- [x] **Description**

  Entry function. start alink gateway service.

- [x] **Parameters**

  None.

- [x] **Returns**

  0 on success, otherwise -1 will return

## 5-alink_wait_connect

```c
int alink_wait_connect(int timeout_ms)
```

- [x] **Description**

  Waiting alink connect to aliyun server

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | timeout_ms | time in milliseconds, use ALINK_WAIT_FOREVER to wait until server is connected |

- [x] **Returns**

  0 when connect to server successfully, otherwise -1 will return

## 6-alink_end

```c
int alink_end(void)
```

- [x] **Description**

  Destroy alink service and free resources

- [x] **Parameters**

  None.

- [x] **Returns**

  0 on success, otherwise -1 will return

## 7-alink_factory_reset

```c
int alink_factory_reset(void)
```

- [x] **Description**

  Reset user account binding.

- [x] **Parameters**

  None.

- [x] **Returns**

  0 on success, -1 when params invalid

## 8-alink_report

```c
int alink_report(const char *method, char *json_buffer)
```

- [x] **Description**

  Report alink message, it is a fundamental func.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | method |      alink protocol method, i.e. "postDeviceRawData", "retrieveDeviceData".   |
  | [in] | json_buffer | json format buffer, like {"OnOff":"1", "Light":"80"}. |

- [x] **Returns**

  0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return

## 9-alink_register_callback

```c
int alink_register_callback(unsigned char cb_type, void *cb_func)
```

- [x] **Description**

  Register misc callback

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | cb_type | callback type.   |
  | [in] | cb_func | callback func pointer, func declaration see related comments. |

- [x] **Returns**

  0 on success, otherwise -1 will return

## 10-awss_start

```c
int awss_start(void)
```

- [x] **Description**

  Start awss service, block method, block until awss succeed, or timeout(see Note).

- [x] **Parameters**

  None.

- [x] **Returns**

  0 on success, otherwise non-zero value will return
     =  0: connect AP & DHCP success
     = -1: get ssid & passwd fail
     = -2: connect AP / DHCP fail

## 11-awss_stop

```c
int awss_stop(void)
```

- [x] **Description**

  Calling this func force awss_start exit.

- [x] **Parameters**

  None.

- [x] **Returns**

  0 on success, otherwise -1 will return
