@page vfs vfs
# 概述
VFS 虚拟文件系统为各种文件（包括设备文件和普通文件）提供统一的操作接口。它是具体设备文件和文件系统之上的抽象层。其目的是允许应用程序以统一的方式访问不同类型的具体文件和设备。用户可以将ramfs,little fs等具体的文件系统注册到 VFS 中，然后使用标准操作接口（open,read,write,close 等）访问其中的文件。

组件支持以下功能：

- 普通文件操作
- 设备文件操作
- inode 管理
- 文件描述符管理

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── src
│   ├── vfs_adapt.c   # vfs OS adapt layer 具体实现代码
│   ├── vfs_aos.c     # VFS 的aos api接口实现
│   ├── vfs.c         # vfs 核心逻辑代码
│   ├── vfs_file.c    # vfs file descriptor 管理具体实现代码
│   └── vfs_inode.c   # vfs inode 管理代码
├── include
│   ├── aos
│   │   └── vfs.h     # VFS的对外AOS API
|   ├── vfs_api.h     # VFS的对外 API
|   ├── vfs_conf.h    # VFS 的参数配置
|   ├── vfs_file.h    # VFS 文件相关操作
|   ├── vfs_inode.h   # VFS inode 管理
|   ├── vfs_types.h   # VFS 数据结构定义
│   └── vfs_adapt.h   # vfs OS adapt layer declaration
├── package.yaml      # 编译配置文件
└── example
    └── vfs_example.c # VFS 示例代码
```

## 依赖组件

* 无

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> VFS 设备node数量，默认4096,  可修改yaml配置如：
```sh
def_config:
  VFS_CONFIG_DEVICE_NODES: 4096
```
> VFS FD 默认起始值，默认 512，可修改yaml配置如：
```sh
def_config:
  VFS_CONFIG_FD_OFFSET: 512
```
> VFS 路径长度最大字节数，默认256，可修改yaml配置如：
```sh
def_config:
  VFS_CONFIG_PATH_MAX: 256
```
> VFS 最大文件数量，默认50，可修改yaml配置如：
```sh
def_config:
  VFS_CONFIG_MAX_FILE_NUM: 50
```
# API说明
# 接口定义

## 初始化虚拟文件系统

```C
int vfs_init(void);
```

* 参数
  * 无
* 返回值
  * 0：成功
  * < 0：失败

## 通过路径打开文件

```C
int aos_open(const char *path, int flags);
```

* 参数
  * path: 文件或者设备的路径
  * flags: 打开操作模式
* 返回值
  * 文件描述符：成功
  * < 0：失败

* flags:

| 参数     | 描述                                                         |
| -------- | ------------------------------------------------------------ |
| O_RDONLY | 只读方式打开文件                                             |
| O_WRONLY | 只写方式打开文件                                             |
| O_RDWR   | 以读写方式打开文件                                           |
| O_CREAT  | 如果要打开的文件不存在，则建立该文件                         |
| O_APPEND | 当读写文件时会从文件尾开始移动，也就是所写入的数据会以附加的方式添加到文件的尾部 |
| O_TRUNC  | 如果文件已经存在，则清空文件中的内容                         |
| O_EXCL   | 如果存在指定文件，返回出错                                   |

## 关闭文件

```C
int aos_close(int fd);
```

* 参数
  * fd: 文件描述符
* 返回值
  * 0：成功
  * < 0：失败

## 读取文件内容

```C
ssize_t aos_read(int fd, void *buf, size_t nbytes);
```

* 参数
  * fd: 文件描述符
  * nbytes: 需要读取的字节数
  * buf: 读取到缓冲区的指针
* 返回值
  * 实际读取到的字节数
  * 0：读取数据到达文件结尾
  * < 0：失败

## 向文件写入内容

```C
ssize_t aos_write(int fd, const void *buf, size_t nbytes);
```

* 参数
  * fd: 文件描述符
  * nbytes: 需要写入的字节数
  * buf: 数据缓冲区的指针
* 返回值
  * 实际写入的字节数
  * < 0：失败

## 发送特定命令控制接口

```C
int aos_ioctl(int fd, int cmd, unsigned long arg);
```

* 参数
  * fd: 文件描述符
  * cmd: 特定控制命令
  * arg: 命令的参数
* 返回值
  * 任何返回值

## 在打开的文件描述符上执行下面描述的操作，操作由 cmd 确定

```C
int aos_fcntl(int fd, int cmd, int val);
```

* 参数
  * fd: 文件描述符
  * cmd: 文件操作命令
  * val: 依赖`cmd`的参数
* 返回值
  * 0：成功
  * < 0: 失败

## 设置下次读取文件的位置

```C
off_t aos_lseek(int fd, off_t offset, int whence);
```

* 参数
  * fd: 文件描述符
  * offset: 根据参数`whence`来移动读写位置的位移数
  * whence: SEEK_SET 参数`offset` 即为新的读写位置
  *         SEEK_CUR 以目前的读写位置往后增加`offset` 个位移量
  *         SEEK_END 将读写位置指向文件尾后再增加`offset`个位移量. 当whence 值为SEEK_CUR 或SEEK_END 时, 参数`offet`允许负值的出现.
* 返回值
  * 返回新的读写位置

## 同步文件

```C
int aos_sync(int fd);
```

* 参数
  * fd: 文件描述符
* 返回值
  * 0：成功
  * < 0: 失败

## 获取文件状态

```C
int aos_stat(const char *path, struct stat *st);
```

* 参数
  * path: 文件名
  * st: 结构指针，指向一个存放文件状态信息的结构体
* 返回值
  * 0：成功
  * < 0: 失败

## 删除指定目录下的文件

```C
int aos_unlink(const char *path);
```

* 参数
  * path: 要删除文件的路径
* 返回值
  * 0：成功
  * < 0: 失败

## 重命名文件

```C
int aos_rename(const char *oldpath, const char *newpath);
```

* 参数
  * oldpath: 旧文件名
  * newpath: 新文件名
* 返回值
  * 0：成功
  * < 0: 失败

## 打开目录

```C
aos_dir_t *aos_opendir(const char *path);
```

* 参数
  * path: 目录名
* 返回值
  * 目录流指针: 成功
  * NULL: 失败

## 关闭目录

```C
int aos_closedir(aos_dir_t *dir);
```

* 参数
  * dir: 目录流指针
* 返回值
  * 0: 成功
  * < 0: 失败

## 读取下个目录

```C
aos_dirent_t *aos_readdir(aos_dir_t *dir);
```

* 参数
  * dir: 目录流指针
* 返回值
  * 目录流指针: 成功
  * NULL: 已读到目录尾部

## 创建目录

```C
int aos_mkdir(const char *path);
```

* 参数
  * path: 目录名
* 返回值
  * 0: 成功
  * < 0: 失败

## 删除目录

```C
int aos_rmdir(const char *path);
```

* 参数
  * path: 目录名
* 返回值
  * 0: 成功
  * < 0: 失败

# 使用示例
示例代码参考example/vfs_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> vfs组件的package.yaml中添加example
```sh
source_file:
  - "example/vfs_example.c" # add vfs_example.c
```

## app中添加VFS组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - vfs: dev_aos # helloworld_demo中引入vfs组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make helloworld_demo@haas100 -c config
```

## 烧录固件
> 参考具体板子的快速开始文档。

## VFS示例测试
> CLI命令行输入：
```sh
vfs_example
```

## 关键日志
> CLI日志：
```sh
vfs example test success!
```

