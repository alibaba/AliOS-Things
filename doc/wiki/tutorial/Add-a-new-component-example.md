EN | [中文](Add-a-new-component-example.zh)

# 1 Add a new component

It will be illustrated through two examples. 

## 1.1	Task 1

Add a new component that existing apps can call

Nano is one of the apps that AOS brings, which can implement time delayed loop printing, and we take it as an example.

The new component's name is set as info_a, and the function is to print a specific message.

### 1.1.1	Select the location of the new component

The component can be put in any of the directory: 

```
COMPONENT_DIRECTORIES := . \
                         example   \
                         board     \
                         kernel    \
                         platform  \
                         utility   \
                         framework \
                         tools     \
                         test      \
                         device    \
                         security
```

For example, create a info_a content under framework.

### 1.1.2	Add source file for the new component

You can create info_a.c  in info_a: 

```c
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

void info_a_fun()
{
	printf("this is in info_a\n");
}

```

### 1.1.3	Add external header file for the new component

You can create info_a.h in info_a:

```c	
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef INFO_A_H
#define INFO_A_H

#ifdef __cplusplus
extern "C" {
#endif

extern void info_a_fun();

#ifdef __cplusplus
}
#endif

#endif  /* INFO_A_H */
```

### 1.1.4	Add .mk file for the new component

.mk file is the symbol of existence of a module. Each component has a corresponding mk file, and makefile can find it by searching its mk file. The mk file contains information that is customized to the component, and the most important things are its source file, header file root and other components it depends on. You can create info_a.mk in info_a:

```
NAME := info_a
$(NAME)_SOURCES := info_a.c
GLOBAL_INCLUDES += .
```

### 1.1.5	Modify the source file of nano to call a new interface

For example:

```c
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/aos.h>
#include "info_a.h"

static void app_delayed_action(void *arg)
{
    printf("%s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    info_a_fun();	
}

int application_start(int argc, char *argv[])
{
    do
    {
        app_delayed_action(NULL);
        aos_msleep(10);
    }while(1);
}
```

### 1.1.6	Modify the .mk file of nano to add a new dependency component

Add a new line in nano.mk:

```
$(NAME)_COMPONENTS += info_a
```

### 1.1.7	Results of operation

Printed as followed:

```
$ ./nano@linuxhost.elf 
trace should have cli to control!!!
app_delayed_action:11 app
this is in info_a
cpu num is 1
app_delayed_action:11 app
this is in info_a
app_delayed_action:11 app
this is in info_a
```

## 1.2	Task two

Add a new component info_b, which is dependent on another component info_a.

The first 4 steps are the same as in task one. You can just require info_a.c  to call the interface of info_b, and add the following sentence to info_a.mk:

```
$(NAME)_COMPONENTS += info_b
```

As it is operated, the interface of info_a and info_b are both called.
