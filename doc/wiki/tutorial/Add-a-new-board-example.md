EN| [中文](Add-a-new-board-example.zh) 

# Add a new board example

This article takes stm32433 as an example to illustrate how to add single board of this type to the existing make system based on stm32433 HelloWorld keil project. 

## 1 Add board component

If the compilation target is nano@stm32l433-nucleo, then when making system searches for dependency component, it will search for component named stm32l433-nucleo from board directory. So you need to create a board/stm32l433-nucleo directory and add a stm32l433-nucleo.mk file in it. You can refer to the existing b_l475e.mk (delete some of them that you don't need).

## 2 Add mcu component

Each board component depends on a mcu component, whose name is defined in board.mk in  HOST_MCU_FAMILY. "Make" will search for corresponding components under platform/mcu based on the value of this variable, so you need to create stm32l4xx.mk in platform/mcu/stm32l4xx directory. You can refer to stm32l475.mk, but should modify the board name. Name of the mk file must be the same as the directory name of the component, otherwise it won't be found. In addition, since there are a variety of board types in stm32l4xx, HOST_MCU_NAME is defined in stm32l433-nucleo.mk and used in stm32l4xx.mk to distinguish.

## 3 Add source file 

Generally, a piece of board has multiple keil projects, and each of them has its own source file containing "main" function. For example, "main" of HelloWorld project is in rhinorun.c. Usually, the entry main function of each board is in aos.c, so you can copy a 475 aos.c. However, different boards support different kernel functions, and 433 do not support dynamic allocation of memory at present, so you need to replace "krhino_task_dyn_create" with "krhino_task_create" in 475 aoc.c. In addition, you need to copy and add hw.c which aos.c depends on and delete some unnecessary contents.

## 4 Isolate from the original project

When adding to "make" system, there will be a keil project in advance. Generally, there will be a project such as HelloWorld, CLI and GUI under the directory of a mcu/board. When the file that joins make system and the project share the same one, since "make" searches for components based on dependency, there will be some redundancy and more source files will be added in original keil project. (For example, vacall and VFS related source files.) To enable these files to compile, you need to modify the macro switch in k_conig.h. But once these switches are changed, the original keil project can't be compiled for lack of files. As a result, you shouldn't share a source file so as not to affect the original keil project. You can create a separate runapp directory (refer to b_475e) to separate it from the original project. You can copy the original files in HelloWorld directory and place them in their own k_config.h, aos.c.

## 5 Separate different link scripts 

Link scripts of different boards should also be separated. 