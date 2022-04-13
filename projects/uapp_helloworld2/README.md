# 介绍
uapp_helloworld2是运行在AliOS Things微内核上的一个用户态应用示例。

# 特性
- 提供一个在AliOS Things微内核上开发用户态应用的示例。
- 循环打印hello,world.

# 目录
```sh
uapp_helloworld2/
|-- aos.mk             #AliOS Things makefile文件
|-- autobuild.json
|-- Config.in          #AliOS Things menuconfig配置文件
|-- include            #应用的头文件
|-- README.md
`-- src                #应用的初始化代码和业务代码
```
# 依赖
- uspace_lib
- ulog
- usyscall

# 使用
编译并加载改应用，如在HaaS100板子上，OS启动后，执行 `load /data/uapp_helloworld2@haas100-mk.app.bin`

# 参考
N/A
