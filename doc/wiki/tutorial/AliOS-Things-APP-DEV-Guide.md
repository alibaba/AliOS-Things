EN | [中文](AliOS-Things-APP-DEV-Guide.zh)

# Hello World

## Introduction
This tutorial describes how to develope application from source based on AliOS Things, including environment setup, app creating, etc.

## Software/Hardware Environment
- Hardware Environment
AliOS Things supports all kinds of hardware platforms, like develpment boards, serial port tools, debuggers, flashers, etc.
- Software Environment
AliOS Things supports both [CLI tools(Command Line Tools)](AliOS-Things-uCube) and [IDE](AliOS-Things-Studio) for development, please turn to [AliOS Things Development Environment Setup](AliOS-Things-Environment-Setup).

## The Hello World Application
It is easy for development based on AliOS Things, CLI tools is cool for Linux/Mac users, while IDE is great for Windows users.

### Command Line Tools
Steps to develop a typical application using CLI tools: project creating, Makefile writing, coding, building, flashing, debugging.

#### Project Creating
The application code based on AliOS Things is normally located at `example/` directory, however you can either put it elsewhere on demand.
e.g.
```
example/
└── helloworld
    ├── helloworld.c
    └── helloworld.mk
```

#### Makefile Writing
Makefile is used to specify application name, sources, components(dependencies), local and global flags, etc.
Here's a sample below, for `helloworld.mk`:
```Makefile
NAME := helloworld  ## specify name of this application
$(NAME)_SOURCES := helloworld.c  ## source code files
$(NAME)_COMPONENTS += cli  ## depended components，this sample depends on cli
$(NAME)_DEFINES += LOCAL_MACRO ## define local flags
GLOBAL_DEFINES += GLOBAL_MACRO ## define global flags
```

#### Coding
All sources reside in the application directory, you can organise it as you wish.
The application entry of AliOS Things is `application_start`, e.g.
```c
#include <aos/aos.h>

static void app_delayed_action(void *arg)
{
    printf("%s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    aos_post_delayed_action(5000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();
    return 0;
}
```

#### Build and Run

Refer to [Quickstart](Quick-Start#run).
Of course, you can use tools like gdb, valgrind, to debug the code above.

### AliOS Things Studio
AliOS Things provides an IDE called AliOS Things Studio, it's easy and quick.
AliOS Things Studio makes it possible by introducing the application templates, you can just import them and start from there.
It supports building, firmware flashing, debugging and so on.

#### Project Creating
Refer to [Create an application using AliOS Things Studio](AliOS-Things-Studio#41-%E6%96%B0%E5%BB%BA%E9%A1%B9%E7%9B%AE).
Once created, you can start your application code development within AliOS Things Studio.

#### Building, Flashing and Debugging
Refer to [AliOS Things Studio](AliOS-Things-Studio).

## Summary
This page describes how to develop the first Hello World application.
Meanwhile, it introduces environment setup, basic steps to develop a AliOS Things application.

Please turn to [AliOS Things GitHub project page](https://github.com/alibaba/AliOS-Things) to learn more details.
