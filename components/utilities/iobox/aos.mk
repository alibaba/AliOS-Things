NAME := iobox

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := iobox

IOBOX_FILES += cat.c
IOBOX_FILES += cp.c
IOBOX_FILES += df.c
IOBOX_FILES += hexdump.c
IOBOX_FILES += ls.c
IOBOX_FILES += mv.c
IOBOX_FILES += rm.c
IOBOX_FILES += mkdir.c
IOBOX_FILES += touch.c
IOBOX_FILES += rwcheck.c
IOBOX_FILES += rwspeed.c
IOBOX_FILES += echo.c
IOBOX_FILES += cd.c
IOBOX_FILES += pwd.c
IOBOX_FILES += lsof.c
# IOBOX_FILES += rwdev.c
# IOBOX_FILES += unlink.c
# IOBOX_FILES += access.c
# IOBOX_FILES += tail.c
# IOBOX_FILES += grep.c

$(NAME)_SOURCES += $(IOBOX_FILES)

$(NAME)_INCLUDES += ./

INCLUDES += -I $(SOURCE_ROOT)/kernel/include/posix
INCLUDES += -I ./vi/

$(NAME)_CFLAGS += $(INCLUDES)

ifeq ($(LFS_CONFIG_IN_PM),y)
GLOBAL_DEFINES += LIST_DEVICE
else
GLOBAL_DEFINES += LIST_DEVICE CONFIG_VFS_LSOPEN
endif

