@page helloworld_demo helloworld简单示例

[更正文档](https://gitee.com/alios-things/helloworld_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

# 1. 案例简介
helloworld_demo是我们提供的最简化的运行实例，该app从字面上来看功能也比较简单，即完成**hello world!**的关键字符输出，以表明系统初始化完成并能够正常输出。但是虽然功能看似简单单一，该app能够运行成功，即代码内核小系统以及基本的打印输出功能即正常运行。
其完成的主要功能包括：
- 系统板级初始化
- 内核基础组件初始化
- application_start用户入口
- 串口打印输出
- 循环睡眠打印
该示例的运行依赖下述基本功能完成对接：
- uart串口
- 内核的任务和中断运行正常
- 系统tick定时器正常运行
即helloworld_demo这个示例运行，代码系统的**任务调度**、**tick调度**以及**串口打印功能**已经OK。

# 2. 基础知识
## 2.1 基础目录结构

```tree
.
├── helloworld.c   # 该solution核心打印输出代码，入口**application_start**
├── k_app_config.h # 内核组件的配置开关，优先级低于**k_config.h**
├── maintask.c     # 系统主任务入口处理，入口**aos_maintask**
├── Makefile       # aos make编译时入口
├── package.yaml   # 编译系统配置文件
└── SConstruct     # Makefile => Scon => aostools
```

## 2.2 基本规范
solution统一以**aos_maintask**作为入口函数,从具体单板的C入口main函数开始，通过创建一个主任务来执行，即aos_maintask是系统主任务的入口函数：
```c
static void aos_main_task_entry(void)
{
    ......
    aos_maintask();
}

/* main一般为单板的C程序入口 */
int main(void)
{
    ......
    krhino_task_dyn_create(&g_main_task, "main_task", 0, OS_MAIN_TASK_PRI, 0, OS_MAIN_TASK_STACK, (task_entry_t)aos_main_task_entry, 1);

    while (1) {
        krhino_task_sleep(100);
    }
}
```
aos_maintask内实现包括板级初始化**board_init**、基础组件初始化**aos_components_init**、以及app入口**application_start**
```c
/*  For user config
    kinit.argc = 0;
    kinit.argv = NULL;
    kinit.cli_enable = 1;
*/
static kinit_t kinit = {0, NULL, 1};

void board_init(void)
{
    board_tick_init();    // tick板级初始化，实现在具体board目录内
    board_stduart_init(); // uart串口初始化，实现在具体board目录内
    board_dma_init();     // 如果使用dma相关初始化，没有置空
    board_gpio_init();    // gpio的时钟等初始化，没有可置空
}

void aos_maintask(void* arg)
{
    board_init();
    board_kinit_init(&kinit);    // 给系统参数赋值，可使用默认值
    aos_components_init(&kinit); // 系统基础组件的初始化

#ifndef AOS_BINS
    application_start(kinit.argc, kinit.argv); // app的实际实现入口
#endif
}

```
其中为了统一不同单板的板级初始化，新增单板需要统一支持board_init内各板级模块初始化，如果没有相关函数可以实现为空；
对于helloworld功能比较简单，一般需要tick和uart的初始化即可；而对于复杂的app，即需要初始化的模块则按照实际情况来增加，对应实现在具体board中添加，如：
```c
void board_stduart_init(void)
void board_tick_init(void)
void board_flash_init(void)
void board_network_init(void)
void board_gpio_init(void)
void board_wdg_init(void)
void board_ota_init(void)
void board_dma_init(void)
```
对于aos_components_init，其完成了一些基础组件如vfs、cli、kv等基础中间件的初始化，并通过组件宏开关，一旦相应该基础组件模块被加入编译，则aos_components_init即进行相关模块的初始化。
application_start是实际solution的实现，即app的统一入口。

## 2.3 基本运行流程

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01hHWuKH1gO2oeDXMtR_!!6000000004131-2-tps-399-302.png" style="max-width:800px;" />
</div>

# 3. 物料清单

## 3.1 HaaS100 硬件

[HaaS100 硬件简介](https://help.aliyun.com/document_detail/184426.html)

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01XxD6Xo217CB3FZnEU_!!6000000006937-2-tps-746-497.png" style="max-width:800px;" />
</div>

# 4. 案例实现

## 4.1 硬件连接
该案例只需要连接电源线以及串口线，如下图所示：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01S9jkJw1dihpqURQH4_!!6000000003770-0-tps-1280-960.jpg" style="max-width:800px;" />
</div>

## 4.2 软件实现
application_start实际app入口内实现较简单，主要包括：
- 基本的串口打印
- while循环睡眠10S打印计数
代码如下：
```c
int application_start(int argc, char *argv[])
{
    int count = 0;

    printf("nano entry here!\r\n");

    while(1) {
        printf("hello world! count %d \r\n", count++);
        aos_msleep(10000);
    };
}
```
其中系统能够正常打印代表uart功能正常；能够循环1S打印代表tick中断以及任务切换功能正常。

## 4.3 编译下载
开发环境的搭建请参考[《AliOS Things集成开发环境使用说明之搭建开发环境》](https://help.aliyun.com/document_detail/302378.html)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

helloworld_demo的代码下载请参考[《AliOS Things集成开发环境使用说明之创建工程》](https://help.aliyun.com/document_detail/302379.html)，

*> 选择解决方案: “helloworld简单示例”*

*> 选择开发板: Haas100 board configure*

-- 编译固件可参考[《AliOS Things集成开发环境使用说明之编译固件》](https://help.aliyun.com/document_detail/302384.html)。

-- 烧录固件可参考[《AliOS Things集成开发环境使用说明之烧录固件》](https://help.aliyun.com/document_detail/302383.html)。

## 4.4 串口输出效果
```sh
             Welcome to AliOS Things
nano entry here!
hello world! count 0
hello world! count 1
hello world! count 2
hello world! count 3
hello world! count 4
hello world! count 5
hello world! count 6
```

# 5 添加新组件
helloworld_demo作为一个基础组件，其本身依赖的组件相对较少，主要包括内核基础代码、cli以及单板和mcu相关的组件。
用户可以基于此solution作为参考，来开发属于自己的app。
如期望在helloworld_demo中增加ramfs文件系统的组件功能，操作步骤如下：

## 5.1 yaml增加组件
- 在helloworld_demo的yaml文件中添加组件依赖ramfs。由于需要使用标准vfs接口，因此还需要加上vfs组件。
```yaml
depends:
  - ramfs: master
  - vfs: master
```
至于ramfs本身依赖的组件，则在ramfs自身的yaml中需要添加完全。

- 在helloworld_demo的app入口helloworld.c中添加ramfs头文件引用以及初始化函数调用，
如下图，先注册一个根目录为**/test**的ramfs：

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01yyLyVL1mbgIjh55DK_!!6000000004973-2-tps-660-404.png" style="max-width:800px;" />
</div>

- 添加功能调用
示例：
```c
    #include <fcntl.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include "ramfs.h"

    int fd;
    int ret;
    char teststring = "1234";
    char readbuf[10];

    ramfs_register("/test");
    fd = open("/test/file1", O_RDWR);
    if(fd < 0){
        printf("ramfs open fail!\r\n");
        return fd;
    }
    ret = write(fd, teststring, 5);
    if(ret < 0){
        printf("ramfs write fail!\r\n");
        return ret;
    }
    lseek(fd, 0, SEEK_SET);
    ret = read(fd, readbuf, 5);
    if(ret < 0){
        printf("ramfs read fail!\r\n");
        return ret;
    }
    if(strncmp(readbuf, teststring, 5)){
        printf("ramfs test fail! readbuf:%s\r\n",readbuf);
    }else{
        printf("ramfs test success!\r\n");
    }
    ramfs_unregister("/test");
```
由于使用了标准文件系统的O_RDWR相关定义，需要包含#include "fcntl.h"

- 编译、运行

```sh
aos make helloworld_demo@haas100 -c config
aos make
```
运行效果：

```sh
             Welcome to AliOS Things
nano entry here!
ramfs test success!
hello world! count 0
hello world! count 1
hello world! count 2
hello world! count 3
```

# 6. 总结
helloworld_demo虽然代码较少，但是其完成了一个最小系统需要的基本功能，包括：内核启动、任务切换、tick中断，以及串口输出。一般作为单板移植的基本solution来参考对接；
同时，还可以此solution为基础，开始添加用户需要的其他组件，并定制修改自身需要的APP。
