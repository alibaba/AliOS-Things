
#add include
include_directories(${MPYENGINEDIR}/include)
include_directories(${MPYENGINEDIR})

#add src file
include(${MPYENGINEDIR}/py/py.mk)
include(${MPYENGINEDIR}/lib/py.mk)
include(${MPYENGINEDIR}/extmod/py.mk)
include(${MPYENGINEDIR}/drivers/py.mk)

#$(NAME)_CFLAGS += -DMICROPY_HW_BOARD_NAME=\"$(PLATFORM)\"
#$(NAME)_CFLAGS += -DMICROPY_HW_MCU_NAME=\"$(HOST_ARCH)\"
message("file: ${SOURCEFILE}")

