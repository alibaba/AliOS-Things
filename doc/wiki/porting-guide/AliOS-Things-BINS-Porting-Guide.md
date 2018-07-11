EN| [中文](AliOS-Things-FOTA-Porting-Guide.zh)

# 目录

- [1 Way of compilation for bins](#1多bin的编译方式)
- [2 Modification of  Id file](#2链接脚本ld文件修改)
- [3 Modification of system](#3系统的修改)
- [4 Modification of Makefile](#4Makefile修改)

------

# 1、Way of compilation for bins

Take mk3060 as a example ( only mk3060 has been supported currently)：

- compilation of one bin： aos make helloworld@mk3060
- compilation of app bin：aos make helloworld@mk3060 BINS=app
- conpilation of framework bin：aos make helloworld@mk3060 BINS=framework
- compilation of kernel bin：aos make helloworld@mk3060 BINS=kernel

# 2、Modification of  Id file

Compilation for app, framework and kernel bin means their corresponding [app.ld](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/linkinfo/mx108_app.ld.S), [framework.ld](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/linkinfo/mx108_framework.ld.S) and [kernel.ld](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/linkinfo/mx108_kernel.ld.S). (take mk3060 as an example)

The implementation format of the ID file is similar to single bin, but you need to set as follows:

- in  kernel.ld, you need to set the starting position of framework.bin to enable kernel to jump into the framework.

- in the framework.ld file, you need to set the starting position of the app.bin to enable kernel to jump into the app.

If the reference implementation is consistent with developer implementation, it can be directly copied and stored in corresponding platform.

# 3、Modification of system 

- Syscall layer is needed for compilation of multi bins. We have added [syscall_ktbl.c](https://github.com/alibaba/AliOS-Things/blob/master/kernel/ksyscall/syscall_ktbl.c) in kernel module,  added [syscall_ftbl.c](https://github.com/alibaba/AliOS-Things/blob/master/framework/fsyscall/syscall_ftbl.c) and [syscall_kapi.h](https://github.com/alibaba/AliOS-Things/blob/master/framework/fsyscall/syscall_kapi/syscall_kapi.h) in framework module, and added [syscall_uapi.h](https://github.com/alibaba/AliOS-Things/blob/master/app/usyscall/syscall_fapi.h) in app module.

  As API for kernel, framework and app. If developers have kernel module for framework and app, or have framework module for app, they only need to add  AOS_EXPORT in code (AOS_EXPORT (return type, function name, parameter type ---)), and corresponding syscall files will automatically be generated (in out/syscall).

  For global variables that both kernel and app need, such as a, get_a and set_a function should be given in syscall layer, so this variable can be used in app.


# 4、Modification of Makefile

AliOS Things can be combined flexibly in a component-based manner, so in makefile file of each component, the $(NAME) _TYPE variable is added with the following options:

$(NAME)_TYPE:
app、framework、kernel、app&framework、framework&kernel、app&kernel、share（share by three bins）、by default

app：          app、app&framework、app&kernel、share、by default   
framework：framework、app&framework、framework&kernel、share   
kernel：       kernel、app&kernel、framework&kernel、share

Note：The component type must be marked for framework and kernel components. For app components, it is optional.

When the above modification is completed, you need to modify the related [Makefile](https://github.com/alibaba/AliOS-Things/blob/master/platform/mcu/moc108/moc108.mk) in corresponding platform. (take mk3060 as an example).