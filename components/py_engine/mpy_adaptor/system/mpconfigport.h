#include <stdint.h>
#include <stdlib.h>

extern const char haas_help_text[];

// Options to control how MicroPython is built for this port,
// overriding defaults in py/mpconfig.h.

// system endianness
#define MP_ENDIANNESS_LITTLE        (1)

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER     (1)

// object representation and NLR handling
#define MICROPY_OBJ_REPR                    (MICROPY_OBJ_REPR_A)
#define MICROPY_NLR_SETJMP                  (1)

// memory allocation policies
#define MICROPY_ALLOC_PATH_MAX              (256)

// emitters
#define MICROPY_PERSISTENT_CODE_LOAD        (1)
#define MICROPY_EMIT_XTENSAWIN              (1)

// compiler configuration
#define MICROPY_COMP_MODULE_CONST           (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN    (1)

// optimisations
#define MICROPY_OPT_COMPUTED_GOTO           (1)
#define MICROPY_OPT_MPZ_BITWISE             (1)

// Python internal features
#define MICROPY_READER_VFS                  (0)
#define MICROPY_READER_POSIX                (1)
#define MICROPY_ENABLE_FINALISER            (0)
#define MICROPY_STACK_CHECK                 (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_KBD_EXCEPTION               (1)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_REPL_EMACS_KEYS             (1)
#define MICROPY_REPL_AUTO_INDENT            (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ENABLE_SOURCE_LINE          (1)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_NORMAL)
#define MICROPY_WARNINGS                    (1)
#define MICROPY_FLOAT_IMPL                  (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_CPYTHON_COMPAT              (0)
#define MICROPY_STREAMS_NON_BLOCK           (1)
#define MICROPY_STREAMS_POSIX_API           (1)
#define MICROPY_MODULE_BUILTIN_INIT         (1)
#define MICROPY_MODULE_WEAK_LINKS           (1)
#define MICROPY_MODULE_FROZEN_STR           (0)
#define MICROPY_MODULE_FROZEN_MPY           (1)
#define MICROPY_QSTR_EXTRA_POOL             mp_qstr_frozen_const_pool
#define MICROPY_CAN_OVERRIDE_BUILTINS       (1)
#define MICROPY_USE_INTERNAL_ERRNO          (0) // errno.h from xtensa-esp32-elf/sys-include/sys
#define MICROPY_USE_INTERNAL_PRINTF         (0) // ESP32 SDK requires its own printf
#define MICROPY_ENABLE_SCHEDULER            (1)
#define MICROPY_SCHEDULER_DEPTH             (8)
#define MICROPY_VFS                         (1)
#define MICROPY_VFS_FAT                     (0)
#define MICROPY_VFS_POSIX                   (0)
#define USE_STATFS                          (MICROPY_VFS_POSIX)

#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_MALLOC_USES_ALLOCATED_SIZE  (1)
#define MICROPY_MEM_STATS                   (0)

// control over Python builtins
#define MICROPY_PY_FUNCTION_ATTRS           (1)
#define MICROPY_PY_DESCRIPTORS              (1)
#define MICROPY_PY_DELATTR_SETATTR          (1)
#define MICROPY_PY_STR_BYTES_CMP_WARN       (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE     (1)
#define MICROPY_PY_BUILTINS_STR_CENTER      (1)
#define MICROPY_PY_BUILTINS_STR_PARTITION   (1)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES  (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY       (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW      (1)
#define MICROPY_PY_BUILTINS_SET             (1)
#define MICROPY_PY_BUILTINS_SLICE           (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS     (1)
#define MICROPY_PY_BUILTINS_SLICE_INDICES   (1)
#define MICROPY_PY_BUILTINS_FROZENSET       (1)
#define MICROPY_PY_BUILTINS_PROPERTY        (1)
#define MICROPY_PY_BUILTINS_RANGE_ATTRS     (1)
#define MICROPY_PY_BUILTINS_ROUND_INT       (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS      (1)
#define MICROPY_PY_BUILTINS_COMPILE         (1)
#define MICROPY_PY_BUILTINS_ENUMERATE       (1)
#define MICROPY_PY_BUILTINS_EXECFILE        (1)
#define MICROPY_PY_BUILTINS_FILTER          (1)
#define MICROPY_PY_BUILTINS_REVERSED        (1)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED  (1)
#define MICROPY_PY_BUILTINS_INPUT           (1)
#define MICROPY_PY_BUILTINS_MIN_MAX         (1)
#define MICROPY_PY_BUILTINS_POW3            (1)
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       haas_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES    (1)
#define MICROPY_PY___FILE__                 (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO     (1)
#define MICROPY_PY_ARRAY                    (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN       (1)
#define MICROPY_PY_ATTRTUPLE                (1)
#define MICROPY_PY_COLLECTIONS              (1)
#define MICROPY_PY_COLLECTIONS_DEQUE        (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT  (1)
#define MICROPY_PY_MATH                     (1)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS   (1)
#define MICROPY_PY_MATH_ISCLOSE             (1)
#define MICROPY_PY_CMATH                    (1)
#define MICROPY_PY_GC                       (1)
#define MICROPY_PY_IO                       (1)
#define MICROPY_PY_IO_IOBASE                (1)
#define MICROPY_PY_IO_FILEIO                (0)
#define MICROPY_PY_IO_BYTESIO               (1)
#define MICROPY_PY_IO_BUFFEREDWRITER        (1)
#define MICROPY_PY_STRUCT                   (1)
#define MICROPY_PY_SYS                      (1)
#define MICROPY_PY_SYS_MAXSIZE              (1)
#define MICROPY_PY_SYS_MODULES              (1)
#define MICROPY_PY_SYS_EXIT                 (1)
#define MICROPY_PY_SYS_ATEXIT               (1)
#define MICROPY_PY_SYS_STDFILES             (0)
#define MICROPY_PY_SYS_STDIO_BUFFER         (1)
#define MICROPY_PY_UERRNO                   (1)
#define MICROPY_PY_USELECT                  (1)
#define MICROPY_PY_UTIME_MP_HAL             (1)
#define MICROPY_PY_UTIME                    (1)
#define MICROPY_PY_THREAD                   (1)
#define MICROPY_PY_THREAD_GIL               (1)
#define MICROPY_PY_THREAD_GIL_VM_DIVISOR    (32)

#define MICROPY_DEBUG_VERBOSE               (0)

// extended modules
#define MICROPY_PY_BLUETOOTH                (0)
#ifndef MICROPY_PY_BLUETOOTH
    #define MICROPY_PY_BLUETOOTH                (1)
    #define MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE (1)
    #define MICROPY_BLUETOOTH_NIMBLE            (1)
    #define MICROPY_BLUETOOTH_NIMBLE_BINDINGS_ONLY (1)
#endif
#define MICROPY_EPOCH_IS_1970               (1)
#define MICROPY_PY_UASYNCIO                 (1)
#define MICROPY_PY_UCTYPES                  (1)
#define MICROPY_PY_UZLIB                    (1)
#define MICROPY_PY_UJSON                    (1)
#define MICROPY_PY_URE                      (1)
#define MICROPY_PY_URE_SUB                  (1)
#define MICROPY_PY_UHEAPQ                   (1)
#define MICROPY_PY_UTIMEQ                   (1)
#define MICROPY_PY_UHASHLIB                 (1)
#define MICROPY_PY_UHASHLIB_SHA1            (1)
#define MICROPY_PY_UHASHLIB_SHA256          (1)
#define MICROPY_PY_UHASHLIB_MD5             (1)
#define MICROPY_PY_UCRYPTOLIB               (1)
#define MICROPY_PY_UBINASCII                (1)
#define MICROPY_PY_UBINASCII_CRC32          (1)
#define MICROPY_PY_URANDOM                  (1)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS      (1)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC   (rand())
#define MICROPY_PY_OS_DUPTERM               (1)
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new
#define MICROPY_PY_MACHINE_PULSE            (1)
#define MICROPY_PY_MACHINE_I2C              (0)
#define MICROPY_PY_MACHINE_SPI              (0)
#define MICROPY_PY_MACHINE_SPI_MSB          (0)
#define MICROPY_PY_MACHINE_SPI_LSB          (0)
#ifndef MICROPY_HW_ENABLE_SDCARD
#define MICROPY_HW_ENABLE_SDCARD            (1)
#endif
#define MICROPY_HW_SOFTSPI_MIN_DELAY        (0)
#define MICROPY_HW_SOFTSPI_MAX_BAUDRATE     (ets_get_cpu_frequency() * 1000000 / 200) // roughly
#define MICROPY_PY_USSL                     (1)
#define MICROPY_SSL_MBEDTLS                 (1)
#define MICROPY_PY_USSL_FINALISER           (1)
#define MICROPY_PY_UWEBSOCKET               (1)
#define MICROPY_PY_WEBREPL                  (1)
#define MICROPY_PY_FRAMEBUF                 (1) //  fatal error: ports/stm32/font_petme128_8x8.h: No such file or directory 
#define MICROPY_PY_BTREE                    (0) //  fatal error: btree/btree.h: No such file or directory 
#define MICROPY_PY_USOCKET_EVENTS           (MICROPY_PY_WEBREPL)
#define MICROPY_PY_BLUETOOTH_RANDOM_ADDR    (1)
#define MICROPY_PY_BLUETOOTH_DEFAULT_GAP_NAME ("HAAS")

// fatfs configuration
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_MULTI_PARTITION  (1)
#if MICROPY_VFS_FAT
#define mp_type_fileio mp_type_vfs_fat_fileio
#define mp_type_textio mp_type_vfs_fat_textio
#elif MICROPY_VFS_LFS1
#define mp_type_fileio mp_type_vfs_lfs1_fileio
#define mp_type_textio mp_type_vfs_lfs1_textio
#elif MICROPY_VFS_LFS2
#define mp_type_fileio mp_type_vfs_lfs2_fileio
#define mp_type_textio mp_type_vfs_lfs2_textio
#endif

// extended modules by aiot
#define MICROPY_QSTR_BYTES_IN_HASH          (1)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT      (16)
#define MICROPY_ENABLE_EXTERNAL_IMPORT      (1)

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))
#define MP_SSIZE_MAX (0x7fffffff)

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"

typedef int32_t mp_int_t; // must be pointer size
typedef uint32_t mp_uint_t; // must be pointer size
typedef long mp_off_t;
// ssize_t, off_t as required by POSIX-signatured functions in stream.h
#include <sys/types.h>

// board specifics
#define MICROPY_PY_SYS_PLATFORM "AliOS Things"

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&mp_builtin_input_obj }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#ifndef MICROPY_HW_BOARD_NAME
#define MICROPY_HW_BOARD_NAME "HaaS"
#endif

#ifndef MICROPY_HW_MCU_NAME
#define MICROPY_HW_MCU_NAME "HaaS100"
#endif

#ifdef __thumb__
#define MICROPY_MIN_USE_CORTEX_CPU (1)
#define MICROPY_MIN_USE_STM32_MCU (1)
#define MICROPY_MIN_USE_XM510_MCU (1)
#endif


// #define MICROPY_BEGIN_ATOMIC_SECTION() portENTER_CRITICAL_NESTED()
// #define MICROPY_END_ATOMIC_SECTION(state) portEXIT_CRITICAL_NESTED(state)

#if MICROPY_PY_USOCKET_EVENTS
#define MICROPY_PY_USOCKET_EVENTS_HANDLER extern void usocket_events_handler(void);
#else
#define MICROPY_PY_USOCKET_EVENTS_HANDLER
#endif

#if MICROPY_PY_THREAD
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        MICROPY_PY_USOCKET_EVENTS_HANDLER \
        MP_THREAD_GIL_EXIT(); \
        MP_THREAD_GIL_ENTER(); \
    } while (0);
#else
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        MICROPY_PY_USOCKET_EVENTS_HANDLER \
        krhino_task_sleep(1); \
    } while (0);

#define MICROPY_THREAD_YIELD()
#endif


#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];


#ifdef MICROPY_PY_UTIME
#define MICROPY_PY_UTIME_DEF
#define MICROPY_PY_UTIME_DEF_WEAK_LINKS
#else
#define MICROPY_PY_UTIME_DEF
#define MICROPY_PY_UTIME_DEF_WEAK_LINKS
#endif

extern const struct _mp_obj_module_t utime_module;
extern const struct _mp_obj_module_t minicv_module;
extern const struct _mp_obj_module_t driver_module;

#if PY_BUILD_OSS
extern const struct _mp_obj_module_t oss_module;
#define MICROPY_PY_OSS_DEF { MP_ROM_QSTR(MP_QSTR_OSS), MP_ROM_PTR(&oss_module) },
#else
#define MICROPY_PY_OSS_DEF
#endif

#if PY_BUILD_HTTP
extern const struct _mp_obj_module_t http_module;
#define MICROPY_PY_HTTP_DEF { MP_ROM_QSTR(MP_QSTR_http), MP_ROM_PTR(&http_module) },
#else
#define MICROPY_PY_HTTP_DEF
#endif

#if PY_BUILD_TCP
extern const struct _mp_obj_module_t tcp_module;
#define MICROPY_PY_TCP_DEF { MP_ROM_QSTR(MP_QSTR_tcp), MP_ROM_PTR(&tcp_module) },
#else
#define MICROPY_PY_TCP_DEF
#endif

#if PY_BUILD_UDP
extern const struct _mp_obj_module_t udp_module;
#define MICROPY_PY_UDP_DEF { MP_ROM_QSTR(MP_QSTR_udp), MP_ROM_PTR(&udp_module) },
#else
#define MICROPY_PY_UDP_DEF
#endif


#if PY_BUILD_NETMGR
extern const struct _mp_obj_module_t netmgr_module;
#define MICROPY_PY_NETMGR_DEF { MP_ROM_QSTR(MP_QSTR_netmgr), MP_ROM_PTR(&netmgr_module) },
#else
#define MICROPY_PY_NETMGR_DEF
#endif

#if PY_BUILD_BLENETCONFIG
extern const struct _mp_obj_module_t blenetconfig_module;
#define MICROPY_PY_BLE_NETCONFIG_DEF { MP_ROM_QSTR(MP_QSTR_ble_netconfig), MP_ROM_PTR(&blenetconfig_module) },
#else
#define MICROPY_PY_BLE_NETCONFIG_DEF
#endif

#if PY_BUILD_MQTT
extern const struct _mp_obj_module_t mqtt_module;
#define MICROPY_PY_MQTT_DEF { MP_ROM_QSTR(MP_QSTR_mqtt), MP_ROM_PTR(&mqtt_module) },
#else
#define MICROPY_PY_MQTT_DEF
#endif

#if PY_BUILD_LINKSDK
extern const struct _mp_obj_module_t linkkit_module;
#define MICROPY_PY_LINKSDK_DEF { MP_ROM_QSTR(MP_QSTR__linkkit), MP_ROM_PTR(&linkkit_module) },
#else
#define MICROPY_PY_LINKSDK_DEF
#endif


#if PY_BUILD_USOCKET
extern const struct _mp_obj_module_t mp_module_usocket;
#define MICROPY_PY_USOCKET_DEF { MP_ROM_QSTR(MP_QSTR_usocket), MP_ROM_PTR(&mp_module_usocket) },
#else
#define MICROPY_PY_USOCKET_DEF
#endif


#if PY_BUILD_UOS
extern const struct _mp_obj_module_t mp_module_uos;
#define MICROPY_PY_UOS_DEF { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) },
#else
#define MICROPY_PY_UOS_DEF
#endif


#if PY_BUILD_AUDIO
extern const struct _mp_obj_module_t audio_module;
#define MICROPY_PY_AUDIO_DEF {MP_ROM_QSTR(MP_QSTR_audio), MP_ROM_PTR(&audio_module)},
#else
#define MICROPY_PY_AUDIO_DEF
#endif

#if PY_BUILD_SNTP
extern const struct _mp_obj_module_t sntp_module;
#define MICROPY_PY_SNTP_DEF { MP_ROM_QSTR(MP_QSTR_sntp), MP_ROM_PTR(&sntp_module) },
#else
#define MICROPY_PY_SNTP_DEF
#endif

#if PY_BUILD_MODBUS
extern const struct _mp_obj_module_t modbus_module;
#define MICROPY_PY_MODBUS_DEF { MP_ROM_QSTR(MP_QSTR_modbus), MP_ROM_PTR(&modbus_module) },
#else
#define MICROPY_PY_MODBUS_DEF
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
        MICROPY_PY_UTIME_DEF \
        {MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&utime_module)}, \
        {MP_ROM_QSTR(MP_QSTR_minicv), MP_ROM_PTR(&minicv_module)}, \
        {MP_ROM_QSTR(MP_QSTR_driver), MP_ROM_PTR(&driver_module)}, \
        MICROPY_PY_LINKSDK_DEF \
        MICROPY_PY_HTTP_DEF \
        MICROPY_PY_UDP_DEF \
        MICROPY_PY_TCP_DEF \
        MICROPY_PY_NETMGR_DEF \
        MICROPY_PY_BLE_NETCONFIG_DEF \
        MICROPY_PY_MQTT_DEF \
        MICROPY_PY_USOCKET_DEF \
        MICROPY_PY_AUDIO_DEF \
        MICROPY_PY_UOS_DEF \
        MICROPY_PY_OSS_DEF \
        MICROPY_PY_SNTP_DEF \
        MICROPY_PY_MODBUS_DEF \





#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
        MICROPY_PY_UTIME_DEF_WEAK_LINKS \


