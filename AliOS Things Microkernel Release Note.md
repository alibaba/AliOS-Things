# AliOS Things Microkernel Release Note

# 中文
## 主要更新：

- 采用微内核架构，支持用户态文件系统、用户态协议栈、用户态驱动
- 支持应用独立编译、推送和加载
- 内核
   - 新增进程支持
   - 内核组件对象化重构，优化系统调用
   - 新增IPC功能，支持高效RPC语义
   - 新增用户态中断框架
   - 新增ARMv8-M支持
- 系统库
   - C库升级为musl libc(1.2.0)
   - 新增支持C++11标准
   - 新增Signal、读写锁等POSIX接口
   - 优化AOS接口，兼顾内核态与用户态开发
   - 支持三种组件形态：内核态、用户态相同进程、用户态不同进程
- 中间件
   - 发布“进程管理”应用，集成用户态文件系统和Flash驱动
   - 发布“网络管理”应用，集成网络管理框架、LwIP协议栈和WiFi驱动
   - 发布“驱动管理”应用，集成I2C、ADC、SPI、GPIO、PWM等驱动
   - 全新的驱动框架，提供VFS接口
   - 新增ROMFS文件系统和根目录支持
   - OTA更新，支持多模块升级
   - 升级LinkKit版本到v4.0，并更名为Link SDK
   - CLI更新
      - 优化命令自动补全/历史回滚/方向键功能
      - 新增Ctrl+C等热键功能



# English
## Highlights:

- Micro-kernel architecture, including user-space file systems, protocol stack and device drivers.
- Supported application seperately building, pushing and executing.
- Kernel
   - Added process support.
   - Re-arched to use object-oriented semantics, and optimized system call.
   - Added IPC, providing efficient RPC semantics.
   - Added user-space interrupt framework.
   - Added ARMv8-M architecture support.
- System library
   - Upgraded to musl libc (1.2.0).
   - Added C++11 support.
   - Added more POSIX APIs, like signal, rwlock, etc.
   - Optimized AOS APIs, supporting programming both in kernel-space and user-space.
   - Components can be configured in kernel space, in process, or sharing process with other compoenents without any code modification.
- Middleware
   - Published process management application, containing file system and Flash driver.
   - Published network management application, containing network management framework, protocol stack and WiFi driver.
   - Published driver management application, containing I2C, ADC, SPI, GPIO, PWM, etc.
   - Added driver framework, and provided VFS interfaces.
   - Supported ROMFS file system and root directory.
   - Enhanced OTA, supporting multi-module upgrade.
   - Upgraded LinkKit to v4.0, and renamed as Link SDK.
   - CLI new functionalities
      - Supported autocomplete/history/arrow keys.
      - Supported Linux-like key combinations, like Ctrl+C.





