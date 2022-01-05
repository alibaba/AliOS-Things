cmake_minimum_required(VERSION 3.1.0)
project(cryptoauth)

# Various Options for Build
option(ATCA_HAL_KIT_HID "Include the HID HAL Driver")
option(ATCA_HAL_KIT_CDC "Include the CDC HAL Driver (Legacy)" OFF)
option(ATCA_HAL_I2C "Include the I2C Hal Driver - Linux & MCU only")
option(ATCA_HAL_CUSTOM "Include support for Custom/Plug-in Hal Driver")
option(ATCA_PRINTF "Enable Debug print statements in library")
option(ATCA_PKCS11 "Build PKCS11 Library")
option(ATCA_MBEDTLS "Integrate with mbedtls")
option(ATCA_BUILD_SHARED_LIBS "Build CryptoAuthLib as shared library" ON)
set(BUILD_SHARED_LIBS ${ATCA_BUILD_SHARED_LIBS})

# Collect Library Sources
file(GLOB LIB_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c")
file(GLOB ATCACERT_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "atcacert/*.c")
file(GLOB BASIC_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "basic/*.c")
file(GLOB_RECURSE CRYPTO_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "crypto/*.c")
file(GLOB HOST_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "host/*.c")
file(GLOB JWT_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "jwt/*.c")
file(GLOB PKCS11_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "pkcs11/*.c")
file(GLOB MBEDTLS_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "mbedtls/*.c")
file(GLOB TNG_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "../app/tng/*.c")

# Add the basic sources to the library
set(CRYPTOAUTH_SRC ${LIB_SRC} ${ATCACERT_SRC} ${BASIC_SRC} ${CRYPTO_SRC} ${HOST_SRC} ${JWT_SRC} hal/atca_hal.c ${TNG_SRC})

if(ATCA_HAL_KIT_HID OR ATCA_HAL_KIT_CDC)
set(NEED_USB true)
endif()

if(WIN32)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_win_timer.c)
set(HID_SRC ../third_party/hidapi/windows/hid.c)
set(CDC_SRC hal/hal_win_kit_cdc.c)
elseif(APPLE)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_linux_timer.c)
set(HID_SRC ../third_party/hidapi/mac/hid.c)
elseif(UNIX)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/hal_linux_timer.c)
set(TWI_SRC hal/hal_linux_i2c_userspace.c)
set(LINUX TRUE)
endif()

if(LINUX AND NEED_USB)
find_path(LIBUSB_INCLUDE_DIR NAMES libusb.h PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_path(LIBUDEV_INCLUDE_DIR NAMES libudev.h PATH_SUFFIXES "include")
find_library(HAS_LIBUSB usb-1.0)
find_library(HAS_LIBUDEV udev)

if(HAS_LIBUSB AND LIBUSB_INCLUDE_DIR)
set(LIBUSB_GOOD TRUE)
endif()

if(HAS_LIBUDEV AND LIBUDEV_INCLUDE_DIR)
set(LIBUDEV_GOOD TRUE)
endif()

if(ATCA_HAL_KIT_CDC AND LIBUDEV_GOOD)
set(USE_UDEV TRUE)
elseif(LIBUSB_GOOD AND NOT ATCA_HAL_KIT_CDC)
set(USE_LIBUSB TRUE)
else()
message(FATAL_ERROR, "Missing Build Dependencies for USB - install libusb-1.0-0-dev or libudev-dev")
endif()

endif(LINUX AND NEED_USB)

if(USE_UDEV)
set(USB_INCLUDE_DIR ${LIBUDEV_INCLUDE_DIR})
set(HID_SRC ../third_party/hidapi/linux/hid.c)
set(CDC_SRC hal/hal_linux_kit_cdc.c)
endif(USE_UDEV)

if(USE_LIBUSB)
set(USB_INCLUDE_DIR ${LIBUSB_INCLUDE_DIR})
set(HID_SRC ../third_party/hidapi/libusb/hid.c)
endif(USE_LIBUSB)

if(NEED_USB)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} hal/kit_protocol.c)
endif()

if(ATCA_HAL_KIT_HID)
add_definitions(-DATCA_HAL_KIT_HID)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${HID_SRC} hal/hal_all_platforms_kit_hidapi.c)
endif(ATCA_HAL_KIT_HID)

if(ATCA_HAL_KIT_CDC)
add_definitions(-DATCA_HAL_KIT_CDC)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${CDC_SRC})
endif(ATCA_HAL_KIT_CDC)

if(ATCA_HAL_I2C)
add_definitions(-DATCA_HAL_I2C)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${TWI_SRC})
endif(ATCA_HAL_I2C)

if(ATCA_HAL_CUSTOM)
add_definitions(-DATCA_HAL_CUSTOM)
endif()

# Add Remaining Sources depending on target library type
if(ATCA_MBEDTLS)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${MBEDTLS_SRC})
endif()

if(ATCA_PKCS11)
set(CRYPTOAUTH_SRC ${CRYPTOAUTH_SRC} ${PKCS11_SRC})
endif()

add_library(cryptoauth ${CRYPTOAUTH_SRC} ${ATCACERT_DEF_SRC})

set_property(TARGET cryptoauth PROPERTY C_STANDARD 99)

# Add Remaining Sources depending on target library type
if(ATCA_PKCS11)
set_target_properties(cryptoauth PROPERTIES OUTPUT_NAME "ateccx08pkcs11")
endif(ATCA_PKCS11)

if(ATCA_PRINTF)
add_definitions(-DATCAPRINTF)
endif(ATCA_PRINTF)

include_directories(cryptoauth PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../third_party/hidapi/hidapi ${USB_INCLUDE_DIR})

if(WIN32)
set_target_properties(cryptoauth PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
target_link_libraries(cryptoauth setupapi.lib)
endif(WIN32)

if(APPLE)
find_library(IO_KIT_LIB IOKit)
find_library(CORE_LIB CoreFoundation)
target_link_libraries(cryptoauth ${IO_KIT_LIB} ${CORE_LIB})
endif()

if(LINUX)
if(USE_LIBUSB)
target_link_libraries(cryptoauth usb-1.0)
elseif(USE_UDEV)
target_link_libraries(cryptoauth udev)
endif()
target_link_libraries(cryptoauth rt)
endif(LINUX)

