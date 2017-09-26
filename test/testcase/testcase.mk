NAME := testcase

GLOBAL_INCLUDES     += include

$(NAME)_SOURCES     := yts_main.c
$(NAME)_COMPONENTS  := yunit cjson

$(NAME)_SOURCES     += framework/coap_test.c

CONFIG_COAP_DTLS_SUPPORT := y
#CONFIG_COAP_ONLINE := y

ifeq ($(CONFIG_COAP_DTLS_SUPPORT), y)
$(NAME)_DEFINES += COAP_DTLS_SUPPORT
endif
ifeq ($(CONFIG_COAP_ONLINE), y)
$(NAME)_DEFINES += COAP_ONLINE
endif

$(NAME)_INCLUDES += ../../utility/iotx-utils/sdk-impl/imports/
$(NAME)_INCLUDES += ../../utility/iotx-utils/LITE-log
$(NAME)_INCLUDES += ../../utility/iotx-utils//LITE-utils
$(NAME)_INCLUDES += ../../utility/iotx-utils//digest
$(NAME)_INCLUDES += ../../framework/connectivity/coap/iot-coap-c/
$(NAME)_INCLUDES += ../../security/mbedtls/include

$(NAME)_COMPONENTS  += connectivity.coap fota

$(NAME)_SOURCES     += framework/mqtt_test.c
$(NAME)_COMPONENTS  += connectivity.mqtt
$(NAME)_SOURCES     += basic_test.c
$(NAME)_SOURCES     += framework/hal/hal_test.c
$(NAME)_SOURCES     += framework/yloop_test.c
$(NAME)_SOURCES     += framework/fota_test.c
$(NAME)_SOURCES     += framework/vfs_test.c
$(NAME)_SOURCES     += framework/netmgr_test.c 
$(NAME)_SOURCES     += utility/cjson_test.c
$(NAME)_SOURCES     += utility/hashtable_test.c
$(NAME)_SOURCES     += utility/digest_algorithm.c
$(NAME)_SOURCES     += tls/tls_test.c
$(NAME)_SOURCES     += tfs/tfs_test.c
$(NAME)_SOURCES     += kernel/rhino/rhino_test.c
$(NAME)_SOURCES     += kernel/rhino/arch/linux/port_test.c
$(NAME)_SOURCES     += kernel/module/kv_test.c
$(NAME)_SOURCES     += kernel/module/fatfs_test.c
$(NAME)_SOURCES     += kernel/vcall/vcall_test.c

$(NAME)_SOURCES    += aosapi/aos/kernel/test_kernel.c
$(NAME)_SOURCES    += aosapi/aos/kernel/aos_sys_test.c
$(NAME)_SOURCES    += aosapi/aos/kernel/aos_task_test.c
$(NAME)_SOURCES    += aosapi/aos/kernel/aos_mm_test.c
$(NAME)_SOURCES    += aosapi/aos/kernel/aos_mutex_test.c
$(NAME)_SOURCES    += aosapi/aos/kernel/aos_queue_test.c
$(NAME)_SOURCES    += aosapi/aos/kernel/aos_sem_test.c
$(NAME)_SOURCES    += aosapi/aos/kernel/aos_timer_test.c
$(NAME)_SOURCES    += aosapi/aos/kernel/aos_workqueue_test.c

ifeq ($(findstring linuxhost, $(BUILD_STRING)), linuxhost)
# only for for linux host now
$(NAME)_SOURCES    += alicrypto/alicrypto_test.c
endif

ifeq ($(findstring armhflinux, $(BUILD_STRING)), armhflinux)
# only for for linux host now
$(NAME)_SOURCES    += alicrypto/alicrypto_test.c
endif

$(NAME)_COMPONENTS  += mbedtls

$(NAME)_COMPONENTS  += base64 hashtable log connectivity.wsf ywss protocol.alink modules.fs.kv modules.fs.fatfs

ifneq (,$(filter protocols.mesh,$(COMPONENTS)))
$(NAME)_INCLUDES += ../../kernel/protocols/mesh/include
$(NAME)_INCLUDES += ../../tools/dda
include test/testcase/kernel/protocols/mesh/filelists.mk
$(NAME)_SOURCES += $(MESHYTSFILE)
endif

$(NAME)_CFLAGS  += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS  += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS  += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS  += -Wno-unused-value -Wno-strict-aliasing

$(NAME)_INCLUDES += ../../framework/protocol/alink/system/
$(NAME)_INCLUDES += ../../framework/fota/platform/alink/

$(NAME)_INCLUDES += ../../utility/iotx-utils/sdk-impl
