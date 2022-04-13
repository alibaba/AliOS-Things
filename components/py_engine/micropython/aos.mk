
#add include
include_directories(${MICROPYTHONDIR}/include)
include_directories(${MICROPYTHONDIR})

#add src file
include(${MICROPYTHONDIR}/py/aos.mk)
include(${MICROPYTHONDIR}/lib/aos.mk)
include(${MICROPYTHONDIR}/extmod/aos.mk)
include(${MICROPYTHONDIR}/drivers/aos.mk)
include(${MICROPYTHONDIR}/port/aos.mk)

#$(NAME)_CFLAGS += -DMICROPY_HW_BOARD_NAME=\"$(PLATFORM)\"
#$(NAME)_CFLAGS += -DMICROPY_HW_MCU_NAME=\"$(HOST_ARCH)\"
message("file: ${SOURCEFILE}")

