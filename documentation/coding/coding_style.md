@page coding_style 代码风格

**[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/coding/coding_style.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://help.aliyun.com/document_detail/302301.html)**

# 1. 前言
本文是AliOS Things提供的一套C语言代码规范，适用的对象为符合C99标准的C语言工程。

# 2. 命名
本节内容均为建议，不作强制要求。

## 2.1. 总则
各种命名均使用英文单词及其缩写，非特殊情况不能使用汉语拼音或其他语言。

## 2.2. 文件命名
文件名全部使用小写字母，用`_`连接。
源文件使用`.c`后缀。
头文件使用`.h`后缀。

## 2.3. 类型命名
### 2.3.1. 简单类型命名
使用`typedef`自定义的简单类型命名全部使用小写字母，用`_`连接，以`_t`结尾。例如：
```c
typedef int32_t aos_status_t;
```

### 2.3.2. 结构体和联合体命名
结构体和联合体类型命名全部使用小写字母，用`_`连接。建议使用`typedef`定义一个整体的名字，以`_t`结尾。例如：
```c
typedef struct aos_list_node {
    struct aos_list_node *prev;
    struct aos_list_node *next;
} aos_list_node_t;

static aos_list_node_t list_node;
```

### 2.3.3. 枚举命名
枚举类型命名全部使用小写字母，用`_`连接。建议使用`typedef`定义一个整体的名字，以`_t`结尾。
枚举值命名全部使用大写字母，用`_`连接，包含表示类型的前缀。
例如：
```c
typedef enum aos_socket_stage {
    AOS_SOCK_STG_DISCONNECTED,
    AOS_SOCK_STG_CONNECTED,
} aos_socket_stage_t;

static aos_socket_stage_t sock_stage = AOS_SOCK_STG_DISCONNECTED;
```

## 2.4. 变量命名
变量命名全部使用小写字母，用`_`连接。
数组名称尽量使用复数名词。例如：
```c
cfg_file_t cfg_files[NUM_CFG_FILES];
```
表示数目的变量名称使用num（number的缩写）加复数名词。例如：
```c
unsigned int num_files;
```
表示序号的变量名称使用单数名词加num或index或idx（index的缩写）。例如：
```c
unsigned int file_num;
unsigned int file_index;
```

## 2.5. 函数命名
函数命名全部使用小写字母，用`_`连接。

## 2.6. 宏命名
一般的宏命名全部使用大写字母，用`_`连接。例如：
```c
#define AOS_STRING_MAX_LEN 127
```
模拟函数使用方式的宏的命名规则与函数相同。例如：
```c
#define aos_dev_set_id(dev, x) \
    do { \
        (dev)->id = (x); \
    } while (0)
```
## 2.7. 前缀
为防止命名空间污染，公用组件中的非static函数、非static全局变量、全局类型、全局宏的命名应带有前缀。例如（假设前缀为`aos`）：
```c
void aos_cfg_file_close(int fd);
extern char **aos_process_argv;
typedef struct aos_list_node aos_list_node_t;
#define AOS_STRING_MAX_LEN 127
```

# 3. 格式
## 3.1. 文本格式
源文件、头文件、Makefile等文本文件一律采用UTF-8 without BOM编码，采用Unix风格换行格式。
文本文件末尾应有且只有一个换行符，即末尾应有且只有一个空行。

## 3.2. 行长度
每行字符数原则上不超过120。包含长路径的`#include`语句、头文件`#define`保护可以无视此规则。

### 3.2.1. 表达式换行
较长的表达式可在运算符处换行，换行处的运算符属于旧行，新行对齐到旧行中的相同逻辑层级。例如：
```c
void foo(void)
{
    if ((aos_list_next(list_node) != &list_head && !priv) ||
        !(strcmp(symbol, default_symbol) && blahblahblahblahblahblah() &&
          meomeomeomeomeomeomeomeomeomeomeomeomeomeomeomeo(NULL))) {
        /* ... */
    }
}
```

### 3.2.2. 函数换行
较长的函数定义、声明可在返回值类型和函数名称之间换行。若返回值为指针类型，`*`属于新行。例如：
```c
static unsigned long
blahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblah(void);
static const manager_priv_t
*blahblahblahblahblahblahblahblahblahblahblahblahblahblahblahblah(int index);
```
较长的函数定义、声明、调用可在参数列表中间换行，参数列表中间换行后新行应缩进至旧行第一个参数处。例如：
```c
void blahblahblahblahblahblahblahblahblah(manager_priv_t *priv, int index,
                                          const char *proc_name);

void foo(void)
{
    blahblahblahblahblahblahblahblahblahblah(get_manager_priv(manager), 0,
                                             "meomeomeomeomeomeomeomeo");
}
```

### 3.2.3. 字符串换行
较长的字符串可在空格处换行，一般情况下换行处的空格属于旧行。例如：
```c
void foo(void)
{
    printf("The GNU operating system consists of GNU packages "
           "(programs specifically released by the GNU Project) "
           "as well as free software released by third parties.\n");
}
```

## 3.3. 缩进
使用空格缩进，每次4个空格。全文不应出现制表符（tab）。例如：
```c
void foo(unsigned int nbr_processes)
{
    unsigned int i;

    while (i < nbr_processes) {
        const char *name;
        /* ... */
    }
}
```
宏定义、行尾注释、结构体、联合体、枚举等内部可缩进实现多行对齐，但不作强制要求。若有缩进，应对齐到4的整数倍。例如：
```c
/* 此处数字0缩进32个字符，即位于第33列。 */
#define STAGE_UPDATE_CONTINUE   0
#define STAGE_UPDATE_COMPLETE   1

/* 合法 */
#define EVENT_RX_FULL (1U << 0)
#define EVENT_TX_EMPTY (1U << 1)

typdef enum socket_stage {
    /* 此处等号缩进32个字符，即位于第33列。 */
    SOCK_STG_DISCONNECTED       = 0,
    SOCK_STG_CONNECTED          = 1,
} socket_stage_t;

/* 此处反斜杠缩进40个字符，即位于第41列。 */
#define aos_dev_set_flags(dev, x)       \
    do {                                \
        (dev)->flags = (x);             \
    } while (0)

/* 合法 */
#define aos_dev_set_ops(dev, x) \
    do { \
        (dev)->ops = (x); \
    } while (0)

/* 此处注释缩进24个字符，即位于第25列。 */
foo(NULL);              /* abc */
blahblahblahblahblah(); /* xyz */

/* 合法 */
foofoofoo(); /* abc */
foofoo(); /* xyz */
```
分行定义的宏，第二行起应缩进一次。例如：
```c
#define aos_dev_set_id(dev, x) \
    do { \
        (dev)->id = (x); \
    } while (0)
```
`switch`块中的`case`语句和`default`语句与`switch`语句缩进层级相同。例如：
```c
switch (stage) {
case SOCK_STG_DISCONNECTED:
    foo();
    break;
case SOCK_STG_CONNECTED:
    sock->connected = 1;
    break;
default:
    break;
}
```

## 3.4. 花括号
函数体的左花括号另起一行；其他情况下左花括号不另起一行。一般情况下左花括号后续内容另起一行；宏定义中、数组、结构体、联合体初始化时若花括号中内容较短则左花括号后续内容可以不另起一行。
一般情况下右花括号另起一行；宏定义中、数组、结构体、联合体初始化时若花括号中内容较短则右花括号可以不另起一行。右花括号与后续内容组合成一行。
例如：
```c
typedef struct manager_priv {
    int index;
    void *data;
} manager_priv_t;

#define set_manager_index(x, idx) do { (x)->priv->index = (idx); } while (0)

#ifdef __cplusplus
extern "C" {
#endif

void foo(void)
{
    int i = 0;

    /* ... */

    if (i == 0) {
        /* ... */
    } else {
        /* ... */
    }
}

manager_priv_t priv = { 0, NULL, };

#ifdef __cplusplus
}
#endif
```

## 3.5. 空格
行尾不应有空格。
三元操作符和二元操作符（获取成员的`.`和`->`操作符除外）前后留有空格。例如：
```c
x = a ? b : c;
v = w * x + y / z;
len = x.length;
priv = proc->priv;
```
一元操作符与参数之间不留空格。例如：
```c
x = *p;
p = &x;
i++;
j = --i;
```
逗号右侧若有内容，逗号与右侧内容之间应有空格。例如：
```c
void foo(int x, int y);
```
分号右侧若有内容（右圆括号或另外一个分号除外），分号与右侧内容之间应有空格。例如：
```c
for (i = 0; i < strlen(str);) {
    for (;;) {
        /* ... */
    }
}
```
圆括号内部内容与圆括号之间不留空格。例如：
```c
len = strlen(name);

for (i = 0; i < count; i++) {
    /* ... */
}
```
圆括号与左侧关键字之间应有空格。例如：
```c
while (1) {
    /* ... */
}

if (i == 0) {
    /* ... */
}
```
圆括号与左侧函数名之间不留空格。例如：
```c
int load_file(const char *name)
{
    foo(0);
    /* ... */
}
```
类型转换中的圆括号与右侧内容之间不留空格。例如：
```c
manager_priv_t *priv = (manager_priv_t *)p;
```
方括号与左侧内容、内部内容之间不留空格。例如：
```c
c = name[i];
```
左花括号左侧或右侧若有内容，左右内容与左花括号之间应有空格。右花括号左侧若有内容，左侧内容与右花括号之间应有空格；右花括号右侧若有内容（分号、逗号除外），右侧内容与右花括号之间应有空格。例如：
```c
#define set_manager_index(x, idx) do { (x)->priv->index = (idx); } while (0)

manager_priv_t priv = { 0, NULL, };
```
分行定义的宏，`\`与左侧内容之间应有空格。例如：
```c
#define set_manager_index(x, idx) \
    do { \
        (x)->priv->index = (idx); \
    } while (0)
```

## 3.6. 指针
指针声明或定义时，`*`应靠近变量名称。`*`与修饰符之间应有空格。例如：
```c
int *p;
const char *name;
void * const ptr;
void (*func)(void *arg);
```

## 3.7. 数值常量
十六进制数字`A` ~ `F`使用大写形式。
表示二进制的前缀`0b`和表示十六进制的`0x`使用小写形式。
后缀`U`和`L`使用大写形式。
后缀`f`使用小写形式。
表示幂的`e`和`p`使用小写形式。
例如：
```c
unsigned int b = 0b0101;
unsigned int x = 0xABCDEF;
unsigned int u = 0U;
long int l = 0L;
unsigned long int ul = 0UL;
float f = 1.0f;
long double ld = 1.0L;
double dd = -1.5e-5;
double xd = 0xA.Bp12;
```

## 3.8. 注释
使用C90风格的`/* */`，不使用C++风格的`//`。
`/*`或`*/`与注释正文之间应有空格。行尾的注释和代码之间应有空格。
完整语句注意首字母大写和标点符号，简单词组可以不使用标点。注意区分中英文标点。
**TODO：**使用特定注释格式可利用doxygen等自动化工具生成文档。
例如：
```c
/*
 * This source file is part of AliOS Things.
 * Zhang San <zhangsan@abc.com>
 * 2021.07.01
 */

/* Zhang San <zhangsan@abc.com>
 * 2021.07.01 */

/* This pointer must NOT be NULL. */

/* connecting */
```

# 4. 头文件
## 4.1. 路径
为避免与第三方库的头文件命名冲突，公用组件的头文件应存放于子目录中，引用时路径包含子目录名称。例如：
```c
#include <aos/file.h>
```

## 4.2. 引号和尖括号
只有包含与本源文件处于同路径中的头文件时使用引号，其他情况均使用尖括号。例如：
```c
#include <stdio.h>
#include "my_demo.h"
```

## 4.3. 包含次序
包含头文件的次序如下:
|次序   |种类|
|-:-    |:-|
|1      |C语言标准库头文件和工具链头文件|
|2      |公用组件的头文件|
|3      |本工程头文件|

例如：
```c
#include <stdio.h>
#include <pthread.h>
#include <openssl/rsa.h>
#include <aos/headers.h>
#include "my_demo.h"
```

## 4.4. 保护
所有头文件都应该使用`#define`保护来防止被重复包含。相关宏命名格式是`PATH_FILE_H`。
例如，头文件*aos/common.h*可按如下方法保护：

```c
#ifndef AOS_COMMON_H
#define AOS_COMMON_H

/* 全部内容 */

#endif /* AOS_COMMON_H */
```

## 4.5. 函数、变量声明
头文件中的函数声明不使用`extern`关键字。头文件中的全局变量声明使用`extern`关键字。例如：
```c
void aos_cfg_file_close(int fd);
extern char **aos_process_argv;
```

## 4.6. extern "C"关键字
公用头文件中声明的函数和全局变量应该使用`extern "C"`关键字修饰。
`#include`不应使用`extern "C"`关键字修饰。
`#define`、类型定义不作要求，可酌情考虑。
例如：
```c
#ifndef AOS_COMMON_H
#define AOS_COMMON_H

#include <stddef.h>

#define AOS_STRING_MAX_LEN      127
#define AOS_LSTRING_MAX_LEN     511

typedef struct aos_tm {
    unsigned int sec;
    unsigned int min;
    unsigned int hour;
    unsigned int mday;
    unsigned int mon;
    unsigned int year;
} aos_tm_t;

#ifdef __cplusplus
extern "C" {
#endif

extern char aos_process_symbol[AOS_STRING_MAX_LEN + 1];

void aos_start(void);

#ifdef __cplusplus
}
#endif

#endif /* AOS_COMMON_H */
```

# 5. 其他注意事项
只在本编译单元使用的函数、全局变量应使用`static`修饰符。
在不影响功能的前提下，指针类型的函数参数尽量使用`const`修饰符。
自增、自减运算符单独使用时采用后置形式。
数组、结构体初始化列表、枚举类型定义中的最后一个成员之后应有逗号。例如：
```c
int offsets[] = {
    0,
    1,
};
```
打印到终端的信息一律使用英文，以避免终端编码问题。
