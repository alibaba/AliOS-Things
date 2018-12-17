## Introduction

**log** is an lightweight log module for AliOS-Things, there is several level to print log: `none`, `debug`, `info`, `warn`, `error`, `fatal`, in `osal/aos/include/aos/log.h` wrapped **log** APIs for application development:

| level | api         | warpped api | descripthon      |
|-------|-------------|-------------|------------------|
| none  |`LOG_IMPL()` |`LOG()`      | normal log       |
| debug |`LOGD_IMPL()`|`LOGD()`     | for debug        |
| info  |`LOGI_IMPL()`|`LOGI()`     | for information  |
| warn  |`LOGW_IMPL()`|`LOGW()`     | warnning message |
| error |`LOGE_IMPL()`|`LOGE()`     | error message    |
| fatal |`LOGF_IMPL()`|`LOGF()`     | fatal message    |

> usually, use the warpped api in development.

for example, `LOG("aos framework init.");` will print log like this:

```c
[   0.000]<V> AOS [aos_framework_init#17] : aos framework init.
```
the log string including:
* timestamps (systick)
* flag: ( V D I W E F corresponding to none, debug, info, warn, error, fatal)
* function name
* line number

also you can call [aos_set_log_level()](#aos_set_log_level) to set log level.
when the `cli` is enable, you can also input [loglevel](#loglevel) command to set log level.

### Features

* thread-safe
* 6 log level
* support cli command

### Dependencies

* cli

## APIs

### aos_set_log_level()
```c
void aos_set_log_level(aos_log_level_t log_level);
```
`aos_log_level_t`:

```c
typedef enum {
    AOS_LL_NONE,  /* disable log */
    AOS_LL_FATAL, /* fatal log will output */
    AOS_LL_ERROR, /* fatal + error log will output */
    AOS_LL_WARN,  /* fatal + warn + error log will output(default level) */
    AOS_LL_INFO,  /* info + warn + error log will output */
    AOS_LL_DEBUG, /* debug + info + warn + error + fatal log will output */
} aos_log_level_t;
```

**arguments**:

| name        | type              |description |
|:------------|:------------------|------------|
| `log_level` | `aos_log_level_t` | log level  |

## cli commands

```
# loglevel info
set log level success
```
