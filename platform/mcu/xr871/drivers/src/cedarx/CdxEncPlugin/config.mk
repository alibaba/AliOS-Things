######## Config for Your platform ###########
# You can choose follows for osplaform
#OS_LINUX
#OS_ANDROID_4_4
#OS_ANDROID_5_0
#OS_ANDROID_6_0

# You can choose follows for osplaform log level
#OPTION_LOG_LEVEL_CLOSE
#OPTION_LOG_LEVEL_ERROR
#OPTION_LOG_LEVEL_WARNING
#OPTION_LOG_LEVEL_DEFAULT
#OPTION_LOG_LEVEL_DETAIL

OS_PLATFORM            = OS_LINUX
CONFIG_LOG_LEVEL       = OPTION_LOG_LEVEL_DEFAULT
OS_CHECK_MEMORY_CONFIG = false
OS_DEBUG_CONFIG        = false
USE_IP_PROTECTION      = false


######## Config for Your compile tool ########
# You can choose follows for your compile tools
#OPTION_CC_GNUEABIHF
#OPTION_CC_GNUEABI
#OPTION_CC_ARM_GNUEABI
#OPTION_CC_ARM_OPENWRT_UCLIBCGNUEABI
#OPTION_CC_ARM_OPENWRT_MUSLGNUEABI
#OPTION_CC_UCLIBC_ARM926_STATIC
#OPTION_CC_X86_ANDROID
#OPTION_CC_AARCH64_OPENWRT_MUSL

CONFIG_CC = OPTION_CC_GNUEABI

ifeq ($(CONFIG_CC), OPTION_CC_GNUEABI)

########## set compile tool chains to be arm-linux-gnueabihf- set.
CC    = arm-linux-gnueabi-gcc
CPP   = arm-linux-gnueabi-g++
STRIP = arm-linux-gnueabi-strip
AR    = arm-linux-gnueabi-ar
AS    = arm-linux-gnueabi-as
LD    = arm-linux-gnueabi-ld
endif

ifeq ($(CONFIG_CC), OPTION_CC_AARCH64_OPENWRT_MUSL)

########## set compile tool chains to be arm-linux-gnueabihf- set.
CC    = aarch64-openwrt-linux-musl-gcc
CPP   = aarch64-openwrt-linux-musl-g++
STRIP = aarch64-openwrt-linux-musl-strip
AR    = aarch64-openwrt-linux-musl-ar
AS    = aarch64-openwrt-linux-musl-as
LD    = aarch64-openwrt-linux-musl-ld
endif

ifeq ($(CONFIG_CC), OPTION_CC_GNUEABIHF)

########## set compile tool chains to be arm-linux-gnueabihf- set.
CC    = arm-linux-gnueabihf-gcc
CPP   = arm-linux-gnueabihf-g++
STRIP = arm-linux-gnueabihf-strip
AR    = arm-linux-gnueabihf-ar
AS    = arm-linux-gnueabihf-as
LD    = arm-linux-gnueabihf-ld
endif

ifeq ($(CONFIG_CC), OPTION_CC_GNUEABI)

# set compile tool chains to be arm-none-linux-gnueabi- set.
CC    = arm-none-linux-gnueabi-gcc
CPP   = arm-none-linux-gnueabi-g++
STRIP = arm-none-linux-gnueabi-strip
AR    = arm-none-linux-gnueabi-ar
AS    = arm-none-linux-gnueabi-as
LD    = arm-none-linux-gnueabi-ld
endif

ifeq ($(CONFIG_CC), OPTION_CC_ARM_GNUEABI)

# set compile tool chains to be arm-linux-gnueabi- set.
CC    = arm-linux-gnueabi-gcc
CPP   = arm-linux-gnueabi-g++
STRIP = arm-linux-gnueabi-strip
AR    = arm-linux-gnueabi-ar
AS    = arm-linux-gnueabi-as
LD    = arm-linux-gnueabi-ld
endif

ifeq ($(CONFIG_CC), OPTION_CC_ARM_OPENWRT_UCLIBCGNUEABI)

# set compile tool chains to be arm-openwrt-linux-uclibcgnueabi- set.
CC    = arm-openwrt-linux-uclibcgnueabi-gcc
CPP   = arm-openwrt-linux-uclibcgnueabi-g++
STRIP = arm-openwrt-linux-uclibcgnueabi-strip
AR    = arm-openwrt-linux-uclibcgnueabi-ar
AS    = arm-openwrt-linux-uclibcgnueabi-as
LD    = arm-openwrt-linux-uclibcgnueabi-ld
endif

ifeq ($(CONFIG_CC), OPTION_CC_ARM_OPENWRT_MUSLGNUEABI)

# set compile tool chains to be arm-openwrt-linux-muslgnueabi- set.
CC    = arm-openwrt-linux-muslgnueabi-gcc
CPP   = arm-openwrt-linux-muslgnueabi-g++
STRIP = arm-openwrt-linux-muslgnueabi-strip
AR    = arm-openwrt-linux-muslgnueabi-ar
AS    = arm-openwrt-linux-muslgnueabi-as
LD    = arm-openwrt-linux-muslgnueabi-ld
endif

ifeq ($(CONFIG_CC), OPTION_CC_UCLIBC_ARM926_STATIC)

# set compile tool chains to be arm-buildroot-linux-uclibcgnueabi- set.
CC    = arm-buildroot-linux-uclibcgnueabi-gcc
CPP   = arm-buildroot-linux-uclibcgnueabi-g++
STRIP = arm-buildroot-linux-uclibcgnueabi-strip
AR    = arm-buildroot-linux-uclibcgnueabi-ar
AS    = arm-buildroot-linux-uclibcgnueabi-as
LD    = arm-buildroot-linux-uclibcgnueabi-ld
endif

ifeq ($(CONFIG_CC), OPTION_CC_X86_ANDROID)

# set compile tool chains to be i686-linux-android- set.
CC    = i686-linux-android-gcc
CPP   = i686-linux-android-g++
STRIP = i686-linux-android-strip
AR    = i686-linux-android-ar
AS    = i686-linux-android-as
LD    = i686-linux-android-ld
endif
########## set out lib path
LIBPATH = /home/${USER}/libs_for_linux

########## set build objects path
BUILDPATH = ./build

########## set extra compile flag
EXTRA_FLAG = -g
