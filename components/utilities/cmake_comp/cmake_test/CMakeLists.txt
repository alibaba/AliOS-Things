
cmake_minimum_required(VERSION 3.5)

PROJECT(HELLOLIB)

#告知当前使用的是交叉编译方式，必须配置
SET(CMAKE_SYSTEM_NAME Linux)
 
#指定C交叉编译器,必须配置
#或交叉编译器使用绝对地址
SET(CMAKE_C_COMPILER "arm-none-eabi-gcc")
 
#指定C++交叉编译器
SET(CMAKE_CXX_COMPILER "arm-none-eabi-g++")

#指定编译选项
SET(CMAKE_BUILD_FLAGS "-mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -mlittle-endian -mno-unaligned-access -w -fno-builtin-memcopy -fno-builtin-memmove -fno-builtin-memset -ggdb -Os  -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11")
SET(CMAKE_C_FLAGS ${CMAKE_BUILD_FLAGS})
SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

# 通过在主工程文件CMakeLists.txt中修改ADD_SUBDIRECTORY (lib) 指令来指定一个编译输出位置;

# 指定本工程中静态库libhello.so生成的位置，即 build/lib;

ADD_SUBDIRECTORY(lib)

# 也可以通过变更为其他的位置，如

# ADD_SUBDIRECTORY(lib lib_new)

# 则，静态库libhello.so生成的位置变为 build/lib_new


