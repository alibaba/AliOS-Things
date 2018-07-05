[EN](AliOS-Things-BINS-Porting-Guide)| 中文

# 目录

  * [1 多bin的编译方式](#1多bin的编译方式)
  * [2 链接脚本ld文件修改](#2链接脚本ld文件修改)
  * [3 系统的修改](#3系统的修改)
  * [4 Makefile修改](#4Makefile修改)
------
# 1、 多bin的编译方式  
以mk3060平台为例(目前只支持mk3060平台)：
 * 单bin编译： aos make helloworld@mk3060
 * app bin编译：aos make helloworld@mk3060 BINS=app
 * framework bin编译：aos make helloworld@mk3060 BINS=framework
 * kernel bin编译：aos make helloworld@mk3060 BINS=kernel

# 2、 链接脚本ld文件修改  
多bin编译意味着有app、framework、kernel bin的编译，这意味着有对应的[app.ld](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/linkinfo/mx108_app.ld.S)、[framework.ld](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/linkinfo/mx108_framework.ld.S)和[kernel.ld](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/linkinfo/mx108_kernel.ld.S)。(mk3060为例)

ld文件的实现格式和单bin一致，主要需要提供如下一点：
* 在kernel.ld文件中需要设置framework.bin的起始位置，以让kernel能够跳转到framework中执行。
* 在framework.ld文件中需要设置app.bin的起始位置，以让kernel能够跳转到app中执行。

  如果参考实现与开发者实现一致，可以直接拷贝存放在对应的平台(platform)下面。

# 3、 系统的修改
* 多bin编译方式，需要在增加syscall层，我们在kernel模块增加了[syscall_ktbl.c](https://github.com/alibaba/AliOS-Things/blob/master/kernel/ksyscall/syscall_ktbl.c)，在framework模块增加[syscall_ftbl.c](https://github.com/alibaba/AliOS-Things/blob/master/framework/fsyscall/syscall_ftbl.c)和[syscall_kapi.h](https://github.com/alibaba/AliOS-Things/blob/master/framework/fsyscall/syscall_kapi/syscall_kapi.h)，在app模块增加了[syscall_uapi.h](https://github.com/alibaba/AliOS-Things/blob/master/app/usyscall/syscall_fapi.h),
  作为kernel、framework和app的api桥梁。如果开发者有kernel模块需要暴露给framework和app的，或者framework模块需要暴露给app的，只需要在代码中增加AOS_EXPORT宏(AOS_EXPORT(返回值类型，函数名，参数类型···))，编译脚本会自动生成相应的syscall层文件（在out/syscall目录下）   
* 对kernel和app都要使用的全局变量，如在kernel中使用全局变量a，需要在syscall层中暴露get_a和set_a的函数，这样开发者可以
  在app中同时使用此全局变量。

# 4、 Makefile修改
AliOS Things可以以组件化的方式弹性组合，所以在每个组件的makefile文件中，增加了$(NAME)_TYPE变量，有如下选项：   
$(NAME)_TYPE:
app、framework、kernel、app&framework、framework&kernel、app&kernel、share（三bin共享）、默认

app：          app、app&framework、app&kernel、share、默认   
framework：framework、app&framework、framework&kernel、share   
kernel：       kernel、app&kernel、framework&kernel、share

注意：framework、kernel组件理论上必须标明组件类型，app组件可标可不标

完成上述修改后，需要修改对应平台(platform)下相关[Makefile](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/moc108.mk)。(mk3060为例)