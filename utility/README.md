## Contents

*(source code, in tree view, list major entries if too many)*

```
udata/
├── README.md
├── abs_data_model/
├── cali_data/
├── include
│   ├── abs_data_model.h
│   ├── aos/
│   ├── ...
│   └── uData_queue.h
├── service/
├── service_mgr/
├── ucube.py
├── udata.mk
├── ...
└── udata_service_task.c
```

## Introduction

*(In brief word, introduce WHAT is it and WHAT does it do.)*

### Features

*(list features)*

* Feature 1, description 1
* Feature 2, description 2
* Feature 3, description 3

### Dependencies

*(list component dependencies)*

* component 1
* component 2

## API

*(list exported APIs)*

### krhino_task_create
```
kstat_t krhino_task_create(ktask_t *task, const name_t *name, void *arg, uint8_t prio, tick_t ticks, cpu_stack_t *stack_buf, size_t stack_size, task_entry_t entry, uint8_t autorun)
```

*(description of this api, if needed)*

#### Arguments
| name | type |description |
| --- | --- | --- |
| task | ktask_t| |
| name_t |const name_t| |
| arg |void| |
| prio |uint8_t| |
| tick |tick_t| |
| stack_buf |cpu_stack_t| |
| stack_size |size_t| |
| entry |task_entry_t| |

##### Return

`ktask_t task`
|return|description|
|---| --- |
| task | task handle |
| -1| fail |

### krhino_task_sleep

### krhino_task_del

### krhino_task_find

## Reference

*(external links, resources, pictures, etc.)*
