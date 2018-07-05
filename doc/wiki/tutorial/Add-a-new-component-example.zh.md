[EN](Add-a-new-component-example) | 中文

# 1 新增模块
具体通过如下两个示例说明
## 1.1	目标一
增加一个新模块供现有的app调用，运行查看结果。
nano是aos自带的一个最简单的app例子，它实现了最简单的延时循环打印功能，我们用它来作为例子中所用的app。
新增模块名设为 info_a，功能是打印一条特定的信息。
### 1.1.1	选择新模块放置位置
组件可以放置在以下任一目录中：
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
例如在framework下创建一个 info_a 目录。
### 1.1.2	增加新模块源文件
如在 info_a 目录中创建 info_a.c :
```C
/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

void info_a_fun()
{
	printf("this is in info_a\n");
}

```
### 1.1.3	增加新模块的对外头文件
如在 info_a 目录中创建 info_a.h :
```C	
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
### 1.1.4	增加新模块的.mk文件
.mk文件是模块存在的标志。每一个模块都会有一个对应的mk文件，makefile通过搜索mk后缀的文件来查找模块。其中声明了一些针对该模块的定制化的东西。最基本的两个就是该模块所包含的源文件，头文件路径和该模块依赖的其他模块。如在info_a目录下创建 info_a.mk：
```
NAME := info_a
$(NAME)_SOURCES := info_a.c
GLOBAL_INCLUDES += .
```
### 1.1.5	修改 nano 源文件调用新接口
如：
```C
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
### 1.1.6	修改 nano的 .mk 文件依赖新模块
在nano.mk中新增一行：
```
$(NAME)_COMPONENTS += info_a
```
### 1.1.7	运行结果
打印如下：
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
## 1.2	目标二
增加一个新模块 info_b， 被另一个模块 info_a 所依赖。

前4步和目标一中的一样，修改 info_a.c调用info_b的接口，再在 info_a.mk 加上下面这句即可：
```
$(NAME)_COMPONENTS += info_b
```
运行可见 info_a 和 info_b 中的接口均被调用。
