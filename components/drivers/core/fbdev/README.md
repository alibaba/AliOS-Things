# 介绍
图形驱动框架: 连接用户层和驱动层的枢纽

# 特性
面向对象的设计理念：核心设备驱动模型采用Bus-Device-Driver分离的架构，用C语言结合面向对象的设计理念进行设计和实现。

# 目录
```sh
fbdev/
├── include          #fbdev包含的头文件
│   ├── fb_define.h
│   ├── fb_rotate.h
│   └── fb.h
├── src		        #fbdev 代码实现
│   ├── fb_rotate.c
│   └── fb.c
├── README.md
└── package.yaml     #aos编译文件
```

# 依赖
- 操作系统系统基础组件
- vfs: master
- posix: master

# 使用
厂商无需关注此模块的实现细节
对外提供的接口见udisplay组件

## 编译指令
无需单独编译

# 参考
无
