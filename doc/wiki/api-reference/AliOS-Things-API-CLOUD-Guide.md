# API INDEX

  * [1 aos_cloud_register_callback](#1-aos_cloud_register_callback)
  * [2 aos_cloud_report](#2-aos_cloud_report)
  * [3 aos_cloud_trigger](#3-aos_cloud_trigger)
  * [4 aos_cloud_register_backend](#4-aos_cloud_register_backend)

------

## 1-aos_cloud_register_callback

```c
int aos_cloud_register_callback(int cb_type, aos_cloud_cb_t cb)
```

- [x] **Description**

  Register cloud event callback.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | cb_type | event type interested.   |
  | [in] | cb |      cloud event callback. |

- [x] **Returns**

  the operation status, 0 is OK, others is error.

## 2-aos_cloud_report

```c
int aos_cloud_report(const char *method,
                     const char *json_buffer,
                     void (*done_cb)(void *),
                     void *arg)
```

- [x] **Description**

  Report event to cloud.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | method |      remote method name.   |
  | [in] | json_buffer | method's payload.   |
  | [in] | done_cb |     report done callback.   |
  | [in] | arg |         private data passed to done_cb. |

- [x] **Returns**

  the operation status, 0 is OK, others is error.

## 3-aos_cloud_trigger

```c
void aos_cloud_trigger(int cb_type, const char *json_buffer)
```

- [x] **Description**

  Trigger specific event, used by Cloud Backend.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | cb_type |     event type.   |
  | [in] | json_buffer | payload. |

- [x] **Returns**

  None.


## 4-aos_cloud_register_backend

```c
void aos_cloud_register_backend(int (*report)(const char *method, const char *json_buffer))
```

- [x] **Description**

  Register Cloud Backend.

- [x] **Parameters**

  | IN/OUT |  NAME  |  DESC  |
  |--------|--------|--------|
  | [in] | report | called when user do aos_cloud_report. |

- [x] **Returns**

  None.