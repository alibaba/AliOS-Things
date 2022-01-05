#
# CMakeLists.txt file for cryptoauthlib
#
cmake_minimum_required(VERSION 3.5)
set(CRYPTOAUTHLIB_DIR "cryptoauthlib/lib")
set(COMPONENT_SRCDIRS       "${CRYPTOAUTHLIB_DIR}/atcacert"
                            "${CRYPTOAUTHLIB_DIR}/basic"
                            "${CRYPTOAUTHLIB_DIR}/crypto"
                            "${CRYPTOAUTHLIB_DIR}/crypto/hashes"
                            "${CRYPTOAUTHLIB_DIR}/host"
                            "${CRYPTOAUTHLIB_DIR}/mbedtls"
                            "${CRYPTOAUTHLIB_DIR}"
                            "${CRYPTOAUTHLIB_DIR}/../app/tng"
                            "port"
                            )

set(COMPONENT_SRCS          "${COMPONENT_DIR}/cryptoauthlib/lib/hal/atca_hal.c"
                            "${COMPONENT_DIR}/cryptoauthlib/lib/hal/hal_freertos.c"
                            "${COMPONENT_DIR}/cryptoauthlib/lib/hal/hal_esp32_i2c.c"
                            "${COMPONENT_DIR}/cryptoauthlib/lib/hal/hal_esp32_timer.c"
                            )

set(COMPONENT_INCLUDEDIRS   "${CRYPTOAUTHLIB_DIR}/"
                            "${CRYPTOAUTHLIB_DIR}/hal"
                            "${CRYPTOAUTHLIB_DIR}/../app/tng"
                            "port"
                            )

set(COMPONENT_REQUIRES      "mbedtls" "freertos"  "driver")

# Don't include the default interface configurations from cryptoauthlib
set(COMPONENT_EXCLUDE_SRCS "${CRYPTOAUTHLIB_DIR}/atca_cfgs.c")
set(COMPONENT_CFLAGS "ESP32" "ATCA_HAL_I2C" "ATCA_USE_RTOS_TIMER")

idf_component_register(     SRC_DIRS        "${COMPONENT_SRCDIRS}"
                            INCLUDE_DIRS    "${COMPONENT_INCLUDEDIRS}"
                            REQUIRES        "${COMPONENT_REQUIRES}"
                            EXCLUDE_SRCS    "${COMPONENT_EXCLUDE_SRCS}"
                            )

target_sources(${COMPONENT_LIB} PRIVATE ${COMPONENT_SRCS})
target_compile_definitions(${COMPONENT_LIB} PRIVATE ${COMPONENT_CFLAGS})
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-pointer-sign)

idf_component_get_property(freertos_dir freertos COMPONENT_DIR)
set_source_files_properties(${CRYPTOAUTHLIB_DIR}/hal/hal_freertos.c PROPERTIES COMPILE_FLAGS -I${freertos_dir}/include/freertos)

# Turn off some warnings for some files that have been checked
set_source_files_properties(${CRYPTOAUTHLIB_DIR}/hal/hal_esp32_i2c.c PROPERTIES COMPILE_FLAGS "-Wno-unused-variable -Wno-unused-but-set-variable")
set_source_files_properties(${CRYPTOAUTHLIB_DIR}/basic/atca_helpers.c PROPERTIES COMPILE_FLAGS "-Wno-type-limits")
set_source_files_properties(${CRYPTOAUTHLIB_DIR}/crypto/hashes/sha1_routines.c PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing")
