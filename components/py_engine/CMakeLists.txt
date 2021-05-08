
cmake_minimum_required(VERSION 3.5)

PROJECT(pyEngine)

#告知当前使用的是交叉编译方式，必须配置
SET(CMAKE_SYSTEM_NAME Generic)

#指定C交叉编译器,必须配置
#或交叉编译器使用绝对地址
#SET(CMAKE_C_COMPILER "arm-none-eabi-gcc")
 
#指定C++交叉编译器
#SET(CMAKE_CXX_COMPILER "arm-none-eabi-g++")

#指定编译选项
#SET(CMAKE_BUILD_FLAGS "-mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -mlittle-endian -mno-unaligned-access -w -fno-builtin-memcopy -fno-builtin-memmove -fno-builtin-memset -ggdb -Os  -Wall -Wfatal-errors -fsigned-char -ffunction-sections -fdata-sections -fno-common -std=gnu11")
#SET(CMAKE_C_FLAGS ${CMAKE_BUILD_FLAGS})
#SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMICROPY_ENABLE_DYNRUNTIME")

# 通过在主工程文件CMakeLists.txt中修改ADD_SUBDIRECTORY (lib) 指令来指定一个编译输出位置;

# 指定本工程中静态库libhello.so生成的位置，即 build/lib;

set(AOSROOTDIR ${CMAKE_SOURCE_DIR}/../..)
set(MPYENGINEDIR ${CMAKE_SOURCE_DIR}/mpy_engine)
set(MPYADAPTORDIR ${CMAKE_SOURCE_DIR}/mpy_adaptor)
set(EXAMPLEDIR ${CMAKE_SOURCE_DIR}/example)

#Global src
file(GLOB SOURCEFILE "")

# Build Option
## os module
# option(PY_BUILD_UOS "Build http  or not" ON)

# ## network module
# option(PY_BUILD_HTTP "Build http  or not" ON)
# option(PY_BUILD_TCP "Build tcp  or not" ON)
# option(PY_BUILD_UDP "Build udp  or not" ON)
# option(PY_BUILD_NETMGR "Build netmgr  or not" ON)
# option(PY_BUILD_MQTT "Build mqtt  or not" ON)
# option(PY_BUILD_LINKSDK "Build mqtt  or not" ON)
# option(PY_BUILD_USOCKET "Build mqtt  or not" ON)

# MESSAGE( STATUS "AI_BUILD_FLAG = ${PY_BUILD_AI}")
## audio module
#option(PY_BUILD_AUDIO "Build uvoice or not" ON)


#include(${EXAMPLEDIR}/example.mk)
#include(${DRIVERDIR}/aos.mk)
include(${MPYENGINEDIR}/py.mk)
include(${MPYADAPTORDIR}/py.mk)

ADD_LIBRARY (py_engine_static STATIC ${SOURCEFILE})
SET_TARGET_PROPERTIES (py_engine_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

GET_TARGET_PROPERTY (OUTPUT_VALUE py_engine_static OUTPUT_NAME)

MESSAGE (STATUS "This is the py_engine_static OUTPUT_NAME: " ${OUTPUT_VALUE})

SET_TARGET_PROPERTIES (py_engine_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
# 也可以通过变更为其他的位置，如

# ADD_SUBDIRECTORY(lib lib_new)

# 则，静态库libhello.so生成的位置变为 build/lib_new



