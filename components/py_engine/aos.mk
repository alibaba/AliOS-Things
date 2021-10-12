NAME := pyengine

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := python engine

py_build_haas700 := y
py_build_video := y

ROOTDIR := ../../../../..
AOSROOT := $(ROOTDIR)/os/kernel_alios
PYENGINEROOT := $(AOSROOT)/components/python/py_engine

# modules
AUDIO_SRC := \
    modules/audio/moduvoice.c \
    modules/audio/uvoiceplayer.c \
    modules/audio/uvoicerecorder.c \
    modules/audio/uvoicesnd.c \
    modules/audio/uvoicetts.c

BLECONFIG_SRC := \
    modules/ble/modble.c

DRIVER_SRC := \
    modules/driver/adc.c \
    modules/driver/board_mgr.c  \
    modules/driver/can.c  \
    modules/driver/crypto.c  \
    modules/driver/dac.c  \
    modules/driver/gpio.c  \
    modules/driver/i2c.c  \
    modules/driver/ir.c  \
    modules/driver/keypad.c  \
    modules/driver/location.c  \
    modules/driver/pwm.c  \
    modules/driver/py.mk  \
    modules/driver/rtc.c  \
    modules/driver/spi.c  \
    modules/driver/timer.c  \
    modules/driver/uart.c  \
    modules/driver/und.c  \
    modules/driver/wdt.c

LK_SRC := \
    modules/linksdk/modlinkkit.c  \
    modules/linksdk/modlinksdk.c  \
    modules/linksdk/module_aiot_activeinfo.c  \
    modules/linksdk/module_aiot_device.c  \
    modules/linksdk/module_aiot_dynreg.c  \
    modules/linksdk/module_aiot_gateway.c  \
    modules/linksdk/module_aiot.h  \
    modules/linksdk/module_aiot_mqtt.c

MODBUS_SRC := \
    modules/modbus/modbus.c

MQTT_SRC := \
    modules/mqtt/modmqtt.c  \
    modules/mqtt/module_mqtt.c  \
    modules/mqtt/module_mqtt_client.c  \
    modules/mqtt/mqttclient.c

HTTP_SRC := \
    modules/network/http/httpclient.c  \
    modules/network/http/httputility.c  \
    modules/network/http/modhttp.c

OSS_SRC := \
    modules/oss/modoss.c

OTA_SRC := \
    modules/ota/modappota.c

SNTP_SRC := \
    modules/sntp/modsntp.c

UCAMERA_SRC := \
    modules/ucamera/moducamera.c

ULOG_SRC := \
    modules/ulog/modlog.c

KV_SRC := \
    modules/kv/modkv.c

VIDEO_SRC := \
    modules/video/ak_camera.c  \
    modules/video/ak_codec.c  \
    modules/video/ak_rtsp.c  \
    modules/video/ak_usbcam.c  \
    modules/video/modvideo.c  \
    modules/video/videocamera.c  \
    modules/video/videoplayer.c  \
    modules/video/videorecorder.c

# adapter
ADAPTER_SRC := adapter/main.c \
    adapter/haas/_frozen_mpy.c  adapter/haas/help.c         adapter/haas/machine_hw_i2c.c  \
    adapter/haas/machine_pwm.c       adapter/haas/machine_timer.c     adapter/haas/machine_wdt.c  \
    adapter/haas/moddriver.c   adapter/haas/modpm.c          adapter/haas/moduos.c     adapter/haas/mpsalport.c  \
    adapter/haas/fs.c           adapter/haas/machine_adc.c  adapter/haas/machine_hw_spi.c  adapter/haas/machine_rtc.c  \
    adapter/haas/machine_touchpad.c  adapter/haas/modbattery.c   adapter/haas/modipc.c      \
    adapter/haas/modsocket.c      adapter/haas/modutime.c   adapter/haas/mpthreadport.c     \
    adapter/haas/gccollect.c    adapter/haas/machine_dac.c  adapter/haas/machine_pin.c     \
    adapter/haas/machine_sw_timer.c  adapter/haas/machine_uart.c      adapter/haas/modcharger.c   \
    adapter/haas/modmachine.c  adapter/haas/modsysteminfo.c  adapter/haas/mphalport.c  adapter/haas/utility.c

ifeq ($(py_build_haas700), y)
ADAPTER_SRC += $(wildcard adapter/haas700/*.c)
endif

# engine source
ENGINE_DRIVERS_SRC := \
    engine/drivers/bus/softqspi.c  \
    engine/drivers/bus/softspi.c

ENGINE_EXTMOD_SRC := \
    engine/extmod/machine_i2c.c      engine/extmod/machine_spi.c   engine/extmod/modonewire.c     engine/extmod/moduhashlib.c  \
    engine/extmod/moduselect.c       engine/extmod/moduzlib.c       engine/extmod/utime_mphal.c     engine/extmod/vfs_fat_file.c   engine/extmod/vfs_posix_file.c  \
    engine/extmod/machine_mem.c      engine/extmod/modbluetooth.c  engine/extmod/moduasyncio.c    engine/extmod/moduheapq.c    \
    engine/extmod/modussl_axtls.c    engine/extmod/modwebrepl.c     engine/extmod/vfs_blockdev.c    engine/extmod/vfs_lfs.c        engine/extmod/vfs_reader.c  \
    engine/extmod/machine_pinbase.c  engine/extmod/modbtree.c      engine/extmod/modubinascii.c   engine/extmod/modujson.c     \
    engine/extmod/modussl_mbedtls.c  engine/extmod/mpbthci.c        engine/extmod/vfs.c             engine/extmod/vfs_lfsx.c       engine/extmod/virtpin.c  \
    engine/extmod/machine_pulse.c    engine/extmod/modframebuf.c   engine/extmod/moducryptolib.c  engine/extmod/modurandom.c   \
    engine/extmod/modutimeq.c        engine/extmod/network_cyw43.c  engine/extmod/vfs_fat.c         engine/extmod/vfs_lfsx_file.c  \
    engine/extmod/machine_signal.c   engine/extmod/modlwip.c       engine/extmod/moductypes.c     engine/extmod/modure.c       \
    engine/extmod/moduwebsocket.c    engine/extmod/uos_dupterm.c    engine/extmod/vfs_fat_diskio.c  engine/extmod/vfs_posix.c

ENGINE_LIB_UTILS_SRC := \
    engine/lib/utils/gchelper_generic.c  engine/lib/utils/gchelper_native.c  \
    engine/lib/utils/interrupt_char.c  engine/lib/utils/mpirq.c  \
    engine/lib/utils/printf.c  engine/lib/utils/pyexec.c  \
    engine/lib/utils/stdout_helpers.c  engine/lib/utils/sys_stdio_mphal.c

ENGINE_LIB_READLINE_SRC += \
    engine/lib/mp-readline/readline.c

ENGINE_LIB_TIMEUTILS_SRC += \
    engine/lib/timeutils/timeutils.c

ENGINE_LIB_NETUTILS_SRC += \
    engine/lib/netutils/dhcpserver.c  engine/lib/netutils/netutils.c  engine/lib/netutils/trace.c

ENGINE_PY_SRC := \
    engine/py/argcheck.c     engine/py/builtinimport.c     engine/py/emitnx86.c        \
    engine/py/modcmath.c        engine/py/mpstate.c     engine/py/objarray.c      \
    engine/py/objexcept.c        engine/py/objmodule.c      engine/py/objstr.c         \
    engine/py/persistentcode.c  engine/py/sequence.c  \
    engine/py/asmarm.c       engine/py/compile.c           engine/py/emitnxtensa.c     \
    engine/py/modcollections.c  engine/py/mpz.c         engine/py/objattrtuple.c  \
    engine/py/objfilter.c        engine/py/objnamedtuple.c  engine/py/objstringio.c    \
    engine/py/profile.c         engine/py/showbc.c  \
    engine/py/asmbase.c      engine/py/emitbc.c            engine/py/emitnxtensawin.c  \
    engine/py/modgc.c           engine/py/nativeglue.c  engine/py/objbool.c       \
    engine/py/objfloat.c         engine/py/objnone.c        engine/py/objstrunicode.c  \
    engine/py/pystack.c         engine/py/smallint.c  \
    engine/py/asmthumb.c     engine/py/emitcommon.c        engine/py/formatfloat.c     \
    engine/py/modio.c           engine/py/nlraarch64.c  engine/py/objboundmeth.c  \
    engine/py/objfun.c           engine/py/objobject.c      engine/py/objtuple.c       \
    engine/py/qstr.c            engine/py/stackctrl.c  \
    engine/py/asmx64.c       engine/py/emitglue.c          engine/py/frozenmod.c       \
    engine/py/modmath.c         engine/py/nlr.c         engine/py/obj.c           \
    engine/py/objgenerator.c     engine/py/objpolyiter.c    engine/py/objtype.c        \
    engine/py/reader.c          engine/py/stream.c  \
    engine/py/asmx86.c       engine/py/emitinlinethumb.c   engine/py/gc.c              \
    engine/py/modmicropython.c  engine/py/nlrpowerpc.c  engine/py/objcell.c       \
    engine/py/objgetitemiter.c   engine/py/objproperty.c    engine/py/objzip.c         \
    engine/py/repl.c            engine/py/unicode.c  \
    engine/py/asmxtensa.c    engine/py/emitinlinextensa.c  engine/py/lexer.c           \
    engine/py/modstruct.c       engine/py/nlrsetjmp.c   engine/py/objclosure.c    \
    engine/py/objint.c           engine/py/objrange.c       engine/py/opmethods.c      \
    engine/py/ringbuf.c         engine/py/vm.c  \
    engine/py/bc.c           engine/py/emitnarm.c          engine/py/malloc.c          \
    engine/py/modsys.c          engine/py/nlrthumb.c    engine/py/objcomplex.c    \
    engine/py/objint_longlong.c  engine/py/objreversed.c    engine/py/pairheap.c       \
    engine/py/runtime.c         engine/py/vstr.c  \
    engine/py/binary.c       engine/py/emitnative.c        engine/py/map.c             \
    engine/py/modthread.c       engine/py/nlrx64.c      engine/py/objdeque.c      \
    engine/py/objint_mpz.c       engine/py/objset.c         engine/py/parse.c          \
    engine/py/runtime_utils.c   engine/py/warning.c  \
    engine/py/builtinevex.c  engine/py/emitnthumb.c        engine/py/modarray.c        \
    engine/py/moduerrno.c       engine/py/nlrx86.c      engine/py/objdict.c       \
    engine/py/objlist.c          engine/py/objsingleton.c   engine/py/parsenumbase.c   engine/py/scheduler.c  \
    engine/py/builtinhelp.c  engine/py/emitnx64.c          engine/py/modbuiltins.c     \
    engine/py/mpprint.c         engine/py/nlrxtensa.c   engine/py/objenumerate.c  \
    engine/py/objmap.c           engine/py/objslice.c       engine/py/parsenum.c       engine/py/scope.c

ENGINE_UNZIP_SRC := \
    external/unzip/src/adler32.c   external/unzip/src/crc32.c    external/unzip/src/gzclose.c  \
    external/unzip/src/gzread.c   external/unzip/src/infback.c  external/unzip/src/inflate.c   \
    external/unzip/src/ioapi.c    external/unzip/src/miniunz.c  external/unzip/src/uncompr.c  external/unzip/src/zutil.c  \
    external/unzip/src/compress.c  external/unzip/src/deflate.c  external/unzip/src/gzlib.c    \
    external/unzip/src/gzwrite.c  external/unzip/src/inffast.c  external/unzip/src/inftrees.c  \
    external/unzip/src/iowin32.c  external/unzip/src/trees.c    external/unzip/src/unzip.c

ENGINE_AMPBOOT_SRC := \
    external/amp_boot/amp_boot.c  external/amp_boot/amp_boot_cmd.c  external/amp_boot/amp_boot_flash.c  \
    external/amp_boot/amp_boot_recovery.c  external/amp_boot/amp_boot_uart.c  external/amp_boot/amp_boot_ymodem.c

ENGINE_AMPTASK_SRC := \
    external/amp_task/amp_task.c

ENGINE_AMPUTILS_SRC := \
    external/amp_utils/amp_utils.c

ENGINE_APPMGR_SRC := \
    external/app_mgr/app_message.c  external/app_mgr/app_mgr.c  external/app_mgr/app_upgrade.c

# other
NETMGR_SRC := modules/netmgr/modnetmgr.c
NETWORK_SRC := modules/network/tcp/modtcp.c \
               modules/network/udp/modudp.c
MINICV_SRC := modules/minicv/base/modules/c/src/WrapperIHaasDataInput.cpp \
              modules/minicv/base/modules/c/src/WrapperIHaasImageCodec.cpp \
              modules/minicv/base/modules/c/src/WrapperIHaasImageProc.cpp \
              modules/minicv/base/modules/c/src/WrapperIHaasML.cpp \
              modules/minicv/base/modules/c/src/WrapperIHaasUI.cpp \
              modules/minicv/base/modules/c/src/WrapperIHaasVideoCodec.cpp \
              modules/minicv/base/modules/core/src/HaasImageUtils.cpp

ifeq ($(py_build_ai), y)
MINICV_SRC += modules/minicv/base/modules/ml/src/HaasML.cpp \
              modules/minicv/base/modules/ml/src/HaasMLCloud.cpp \
              modules/minicv/base/modules/ml/src/HaasMLMnn.cpp
GLOBAL_CFLAGS += -DPY_BUILD_AI=1
endif

ifeq ($(py_build_haas700), y)
NETWORK_SRC += modules/network/modnetwork.c \
               modules/network/qrcode.c
GLOBAL_CFLAGS += -DPY_PLATFORM_HAAS_700=1
endif

$(NAME)_SOURCES := $(DRIVER_SRC) $(ENGINE_DRIVERS_SRC) $(ENGINE_EXTMOD_SRC) $(ENGINE_LIB_UTILS_SRC) \
                   $(ENGINE_LIB_READLINE_SRC) $(ENGINE_LIB_TIMEUTILS_SRC) $(ENGINE_LIB_NETUTILS_SRC) \
                   $(ENGINE_PY_SRC) $(ENGINE_UNZIP_SRC) $(ENGINE_AMPBOOT_SRC) $(ENGINE_AMPTASK_SRC) \
                   $(ENGINE_AMPUTILS_SRC) $(NETWORK_SRC) $(ADAPTER_SRC)

ifeq ($(py_build_mqtt), y)
$(NAME)_SOURCES += $(MQTT_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_MQTT=1
endif

ifeq ($(py_build_netmgr), y)
$(NAME)_SOURCES += $(BLECONFIG_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_NETMGR=1
endif

ifeq ($(py_build_lk), y)
$(NAME)_SOURCES += $(LK_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_LINKSDK=1
endif

ifeq ($(py_build_modbus), y)
$(NAME)_SOURCES += $(MODBUS_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_MODBUS=1
endif

ifeq ($(py_build_http), y)
$(NAME)_SOURCES += $(HTTP_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_HTTP=1
endif

ifeq ($(py_build_oss), y)
$(NAME)_SOURCES += $(OSS_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_OSS=1
endif

ifeq ($(py_build_ota), y)
$(NAME)_SOURCES += $(OTA_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_OTA=1
endif

ifeq ($(py_build_sntp), y)
$(NAME)_SOURCES += $(SNTP_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_SNTP=1
endif

ifeq ($(py_build_ucamera), y)
$(NAME)_SOURCES += $(UCAMERA_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_UCAMERA=1
endif

ifeq ($(py_build_ulog), y)
$(NAME)_SOURCES += $(ULOG_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_ULOG=1
endif

ifeq ($(py_build_kv), y)
$(NAME)_SOURCES += $(KV_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_KV=1
endif

ifeq ($(py_build_system), y)
$(NAME)_SOURCES += $(SYSTEM_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_SYSTEM=1
endif

ifeq ($(py_build_video), y)
$(NAME)_SOURCES += $(VIDEO_SRC)
GLOBAL_CFLAGS += -DPY_BUILD_VIDEO=1
endif

ifeq ($(py_build_minicv), y)
$(NAME)_SOURCES += $(MINICV_SRC)
endif

$(NAME)_INCLUDES += \
    adapter/ \
    adapter/haas/ \
    modules/audio/ \
    modules/ble/ \
    modules/driver/ \
    modules/linksdk/ \
    modules/minicv/ \
    modules/modbus/ \
    modules/mqtt/ \
    modules/netmgr/ \
    modules/network/ \
    modules/oss/ \
    modules/ota/ \
    modules/sntp/ \
    modules/ucamera/ \
    modules/ulog/ \
    modules/kv/ \
    engine/ \
    engine/drivers/ \
    engine/extmod/ \
    engine/lib/ \
    external/unzip/internal/ \
    external/unzip/include/ \
    external/amp_boot/ \
    external/amp_task/ \
    external/amp_utils/ \
    external/app_mgr/ \
    external/common/ \
    modules/minicv/base/modules/core/include/

ifeq ($(py_build_haas700), y)
$(NAME)_INCLUDES += adapter/haas700/
endif

$(NAME)_INCLUDES += \
    $(ROOTDIR)/os/driver/include \
    $(ROOTDIR)/apps/zbar-0.10/out/include \
    $(ROOTDIR)/workspace/os/driver/include \
    $(ROOTDIR)/platform/core_module/include \
    $(ROOTDIR)/platform/core_module/src/include \
    $(ROOTDIR)/platform/ext_module/include \
    $(ROOTDIR)/os/kernel_alios/vendor/anyka/soc/include \
    $(ROOTDIR)/os/kernel_alios/vendor/anyka/soc/osal \
    $(ROOTDIR)/platform/ext_lib/alios/AK37D/include \
    $(ROOTDIR)/platform/ext_lib/alios/AK39EV33X/include \
    $(ROOTDIR)/os/kernel_alios/vendor/anyka/soc/of \
    $(ROOTDIR)/os/kernel_alios/kernel/include \
    $(ROOTDIR)/os/kernel_alios/kernel/include/fs \
    $(ROOTDIR)/os/kernel_alios/components/utilities \
    $(ROOTDIR)/os/kernel_alios/components/utilities/cjson \
    $(ROOTDIR)/os/kernel_alios/components/drivers/wlan \
    $(ROOTDIR)/os/kernel_alios/components/drivers/include \
    $(ROOTDIR)/os/kernel_alios/components/network/lwip/include \
    $(ROOTDIR)/os/kernel_alios/components/drivers/bus/usb/include \
    $(ROOTDIR)/os/kernel_alios/components/network/netmgr/netdev/include \
    $(ROOTDIR)/os/kernel_alios/components/drivers/bus/usb/device/class/uvc \
    $(ROOTDIR)/os/kernel_alios/components/py_engine/mpy_adaptor/component/video

ifeq ($(py_build_haas700), y)
GLOBAL_CFLAGS += -DPY_BUILD_NETWORK=1
endif
