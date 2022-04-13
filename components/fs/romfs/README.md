# 介绍
ROMFS指的是Read-Only File System，文件系统的分区只支持读权限，没有写权限。意味着文件系统中的文件在加载之前已经固定，只可以读，不可以在代码运行过程中去新增或者修改某个文件。

在AliOS Things微内核版本中，littlefs文件系统不在kernel中，是运行在用户态的。而kernel在启动需要又有从文件系统加载1号进程 "PM进程 (process managment)"的需求，因此在kernel中引入了ROMFS的功能支持。

# 特性
- ROMFS指支持读权限，没有写权限。
- python脚本负责打包文件到ROMFS。
- 支持多分区：/bin, /dev, /etc, /lib, /proc, /system, /tmp, /usr
- 支持常见的文件操作能力（open/ close/ read/ seek/ stat/ opendir/ readdir/ etc）
- 预置的文件大小理论无限制，取决于kernel链接脚本中RO_DATA段设置的可用大小。

# 目录
```sh
romfs
|-- aos.mk             #AliOS Things的makefile
|-- Config.in          #AliOS Things的menuconfig配置文件
|-- rootfs             #预置的romfs文件镜像，用户把期望打包的文件拷贝到对应目录。
|-- mkromfs.py         #将rootfs下的文件镜像转换成romfs.c中的二进制静态数组，最终编译进内核RO_DATA段。
|-- romfs_def.h        #romfs参数定义
|-- romfs_dummy.c      #dummy参考代码，被romfs.c取代，不被编译。
|-- romfs_vfs.c        #romfs支持的fops定义。
|-- romfs.c            #由mkromfs.py自动生成，是rootfs文件系统镜像转换而成的二进制静态数组，编译进内核。
|-- README.md          #中文版README
```
# 依赖
- rhino

# 使用
(1) copy目标文件到./rootfs的期望目录下，例如拷贝"progmgmt.bin"到"./rootfs/usr"。
(2) 编译romfs时，aos.mk中会执行mkromfs.py，并将/rootfs文件系统镜像转成romfs.c中的静态二进制数组，并将romfs.c和romfs_vfs.c编译进内核。
(3) 内核启动之后，可以直接open/ close/ read 等操作ROMFS中的文件，例如"./usr/progmgmt.bin"。

# 参考
N/A