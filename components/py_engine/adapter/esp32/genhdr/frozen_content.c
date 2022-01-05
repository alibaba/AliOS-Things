//
// Content for MICROPY_MODULE_FROZEN_STR
//
#include <stdint.h>
const char mp_frozen_str_names[] = {
"\0"};
const uint32_t mp_frozen_str_sizes[] = {
0};
const char mp_frozen_str_content[] = {
"\0"};
//
// Content for MICROPY_MODULE_FROZEN_MPY
//
#include "py/mpconfig.h"
#include "py/objint.h"
#include "py/objstr.h"
#include "py/emitglue.h"
#include "py/nativeglue.h"

#if MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE != 0
#error "incompatible MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE"
#endif

#if MICROPY_LONGINT_IMPL != 2
#error "incompatible MICROPY_LONGINT_IMPL"
#endif

#if MPZ_DIG_SIZE != 16
#error "incompatible MPZ_DIG_SIZE"
#endif

#if MICROPY_PY_BUILTINS_FLOAT
typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;
#endif

#if MICROPY_PY_BUILTINS_COMPLEX
typedef struct _mp_obj_complex_t {
    mp_obj_base_t base;
    mp_float_t real;
    mp_float_t imag;
} mp_obj_complex_t;
#endif

enum {
    MP_QSTR__boot_dot_py = MP_QSTRnumber_of,
    MP_QSTR_bdev,
    MP_QSTR_flashbdev,
    MP_QSTR_inisetup,
    MP_QSTR_setup,
    MP_QSTR_vfs,
    MP_QSTR_flashbdev_dot_py,
    MP_QSTR_apa106_dot_py,
    MP_QSTR_NeoPixel,
    MP_QSTR_neopixel,
    MP_QSTR_APA106,
    MP_QSTR_ORDER,
    MP_QSTR_inisetup_dot_py,
    MP_QSTR_check_bootsec,
    MP_QSTR_fs_corrupted,
    MP_QSTR_boot_dot_py,
    MP_QSTR_upip_dot_py,
    MP_QSTR_os,
    MP_QSTR_json,
    MP_QSTR_upip_utarfile,
    MP_QSTR_tarfile,
    MP_QSTR_debug,
    MP_QSTR_index_urls,
    MP_QSTR_install_path,
    MP_QSTR_cleanup_files,
    MP_QSTR_gzdict_sz,
    MP_QSTR_file_buf,
    MP_QSTR_NotFoundError,
    MP_QSTR_op_split,
    MP_QSTR_op_basename,
    MP_QSTR__makedirs,
    MP_QSTR_save_file,
    MP_QSTR_install_tar,
    MP_QSTR_expandhome,
    MP_QSTR_warn_ussl,
    MP_QSTR_url_open,
    MP_QSTR_get_pkg_metadata,
    MP_QSTR_fatal,
    MP_QSTR_install_pkg,
    MP_QSTR_install,
    MP_QSTR_get_install_path,
    MP_QSTR_cleanup,
    MP_QSTR_fname,
    MP_QSTR_subf,
    MP_QSTR_setup_dot_,
    MP_QSTR_PKG_hyphen_INFO,
    MP_QSTR_README,
    MP_QSTR__dot_egg_hyphen_info,
    MP_QSTR_extractfile,
    MP_QSTR_deps,
    MP_QSTR_Skipping,
    MP_QSTR_DIRTYPE,
    MP_QSTR_prefix,
    MP_QSTR__tilde__slash_,
    MP_QSTR_getenv,
    MP_QSTR__colon_,
    MP_QSTR_https_colon_,
    MP_QSTR__percent_s_slash__percent_s_slash_json,
    MP_QSTR_Error_colon_,
    MP_QSTR_msg,
    MP_QSTR_exc,
    MP_QSTR_releases,
    MP_QSTR_TarFile,
    MP_QSTR_fileobj,
    MP_QSTR_pkg_spec,
    MP_QSTR_Queue_colon_,
    MP_QSTR_to_install,
    MP_QSTR_unlink,
    MP_QSTR__hyphen_h,
    MP_QSTR__hyphen__hyphen_help,
    MP_QSTR__hyphen_p,
    MP_QSTR__hyphen_r,
    MP_QSTR__hash_,
    MP_QSTR__hyphen_i,
    MP_QSTR__hyphen__hyphen_debug,
    MP_QSTR__hyphen_,
    MP_QSTR_upip_utarfile_dot_py,
    MP_QSTR_TAR_HEADER,
    MP_QSTR_REGTYPE,
    MP_QSTR_roundup,
    MP_QSTR_FileSection,
    MP_QSTR_TarInfo,
    MP_QSTR_skip,
    MP_QSTR_content_len,
    MP_QSTR_aligned_len,
    MP_QSTR_sz,
    MP_QSTR__0x00_,
    MP_QSTR_tarinfo,
    MP_QSTR_ntptime_dot_py,
    MP_QSTR_NTP_DELTA,
    MP_QSTR_settime,
    MP_QSTR__bang_I,
    MP_QSTR_dht_dot_py,
    MP_QSTR_pyb,
    MP_QSTR_DHTBase,
    MP_QSTR_DHT11,
    MP_QSTR_DHT22,
    MP_QSTR_measure,
    MP_QSTR_humidity,
    MP_QSTR_temperature,
    MP_QSTR_onewire_dot_py,
    MP_QSTR__ow,
    MP_QSTR_OneWireError,
    MP_QSTR_OneWire,
    MP_QSTR_SEARCH_ROM,
    MP_QSTR_MATCH_ROM,
    MP_QSTR_SKIP_ROM,
    MP_QSTR_select_rom,
    MP_QSTR__search_rom,
    MP_QSTR_required,
    MP_QSTR_rom,
    MP_QSTR_l_rom,
    MP_QSTR_diff,
    MP_QSTR_ds18x20_dot_py,
    MP_QSTR_DS18X20,
    MP_QSTR_convert_temp,
    MP_QSTR_read_scratch,
    MP_QSTR_write_scratch,
    MP_QSTR_read_temp,
    MP_QSTR_ow,
    MP_QSTR_CRC_space_error,
    MP_QSTR_uasyncio_slash___init___dot_py,
    MP_QSTR_core,
    MP_QSTR___version__,
    MP_QSTR_funcs,
    MP_QSTR_wait_for,
    MP_QSTR_gather,
    MP_QSTR_event,
    MP_QSTR_Event,
    MP_QSTR_Lock,
    MP_QSTR_stream,
    MP_QSTR__attrs,
    MP_QSTR_attr,
    MP_QSTR_uasyncio_slash_core_dot_py,
    MP_QSTR_ticks,
    MP_QSTR_task,
    MP_QSTR_TimeoutError,
    MP_QSTR_message,
    MP_QSTR_exception,
    MP_QSTR_future,
    MP_QSTR__exc_context,
    MP_QSTR_SingletonGenerator,
    MP_QSTR_IOQueue,
    MP_QSTR__promote_to_task,
    MP_QSTR_create_task,
    MP_QSTR_run_until_complete,
    MP_QSTR__stopper,
    MP_QSTR__stop_task,
    MP_QSTR_Loop,
    MP_QSTR_get_event_loop,
    MP_QSTR_current_task,
    MP_QSTR_new_event_loop,
    MP_QSTR_t,
    MP_QSTR_sgen,
    MP_QSTR__enqueue,
    MP_QSTR__dequeue,
    MP_QSTR_queue_read,
    MP_QSTR_queue_write,
    MP_QSTR_wait_io_event,
    MP_QSTR_poller,
    MP_QSTR_idx,
    MP_QSTR_aw,
    MP_QSTR__io_queue,
    MP_QSTR_call_exception_handler,
    MP_QSTR_main_task,
    MP_QSTR__exc_handler,
    MP_QSTR_run_forever,
    MP_QSTR_set_exception_handler,
    MP_QSTR_get_exception_handler,
    MP_QSTR_default_exception_handler,
    MP_QSTR_future_colon_,
    MP_QSTR_coro_equals_,
    MP_QSTR_context,
    MP_QSTR_runq_len,
    MP_QSTR_waitq_len,
    MP_QSTR_uasyncio_slash_event_dot_py,
    MP_QSTR_ThreadSafeFlag,
    MP_QSTR_is_set,
    MP_QSTR_wait,
    MP_QSTR_waiting,
    MP_QSTR__flag,
    MP_QSTR_req,
    MP_QSTR_uasyncio_slash_funcs_dot_py,
    MP_QSTR_wait_for_ms,
    MP_QSTR_return_exceptions,
    MP_QSTR_runner,
    MP_QSTR_waiter,
    MP_QSTR_uasyncio_slash_lock_dot_py,
    MP_QSTR_exc_type,
    MP_QSTR_tb,
    MP_QSTR_uasyncio_slash_stream_dot_py,
    MP_QSTR_Stream,
    MP_QSTR_StreamReader,
    MP_QSTR_StreamWriter,
    MP_QSTR_open_connection,
    MP_QSTR_Server,
    MP_QSTR_start_server,
    MP_QSTR_stream_awrite,
    MP_QSTR_wait_closed,
    MP_QSTR_aclose,
    MP_QSTR_awrite,
    MP_QSTR_awritestr,
    MP_QSTR_get_extra_info,
    MP_QSTR_readexactly,
    MP_QSTR_drain,
    MP_QSTR_out_buf,
    MP_QSTR_n,
    MP_QSTR__serve,
    MP_QSTR_peername,
    MP_QSTR_backlog,
    MP_QSTR_webrepl_dot_py,
    MP_QSTR_websocket_helper,
    MP_QSTR_listen_s,
    MP_QSTR_client_s,
    MP_QSTR_setup_conn,
    MP_QSTR_accept_conn,
    MP_QSTR_start_foreground,
    MP_QSTR_0_dot_0_dot_0_dot_0,
    MP_QSTR_accept_handler,
    MP_QSTR_rejected,
    MP_QSTR_server_handshake,
    MP_QSTR_listen_sock,
    MP_QSTR_webrepl_cfg,
    MP_QSTR_PASS,
    MP_QSTR_webrepl_setup_dot_py,
    MP_QSTR__dot__slash_boot_dot_py,
    MP_QSTR_RC,
    MP_QSTR_CONFIG,
    MP_QSTR_input_choice,
    MP_QSTR_getpass,
    MP_QSTR_input_pass,
    MP_QSTR_exists,
    MP_QSTR_get_daemon_status,
    MP_QSTR_change_daemon,
    MP_QSTR_prompt,
    MP_QSTR_choices,
    MP_QSTR_webrepl,
    MP_QSTR__dot_tmp,
    MP_QSTR_action,
    MP_QSTR_enabled,
    MP_QSTR__gt__space_,
    MP_QSTR_E,
    MP_QSTR_PASS_space__equals__space__percent_r_0x0a_,
    MP_QSTR_D,
    MP_QSTR_websocket_helper_dot_py,
    MP_QSTR_binascii,
    MP_QSTR_hashlib,
    MP_QSTR_client_handshake,
    MP_QSTR_rwb,
    MP_QSTR_respkey_colon_,
    MP_QSTR__0x0d__0x0a__0x0d__0x0a_,
    MP_QSTR_sock,
    MP_QSTR_neopixel_dot_py,
    MP_QSTR_timing,
    MP_QSTR_urequests_dot_py,
    MP_QSTR_Response,
    MP_QSTR_post,
    MP_QSTR_patch,
    MP_QSTR_content,
    MP_QSTR_raw,
    MP_QSTR__cached,
    MP_QSTR_http_colon_,
    MP_QSTR_Host,
    MP_QSTR_reason,
    MP_QSTR_headers,
    MP_QSTR_upysh_dot_py,
    MP_QSTR_LS,
    MP_QSTR_PWD,
    MP_QSTR_CLEAR,
    MP_QSTR_pwd,
    MP_QSTR_ls,
    MP_QSTR_mv,
    MP_QSTR_rm,
    MP_QSTR_cat,
    MP_QSTR_newfile,
    MP_QSTR_Man,
    MP_QSTR_man,
    MP_QSTR__dot_,
    MP_QSTR__percent_s_slash__percent_s,
    MP_QSTR__percent__space_8d_space__percent_s,
    MP_QSTR__0x1b__bracket_open_2J_0x1b__bracket_open_H,
    MP_QSTR_umqtt_slash_simple_dot_py,
    MP_QSTR_MQTTException,
    MP_QSTR_MQTTClient,
    MP_QSTR__send_str,
    MP_QSTR__recv_len,
    MP_QSTR_set_callback,
    MP_QSTR_set_last_will,
    MP_QSTR_ping,
    MP_QSTR_wait_msg,
    MP_QSTR_check_msg,
    MP_QSTR_client_id,
    MP_QSTR_server,
    MP_QSTR_ssl,
    MP_QSTR_ssl_params,
    MP_QSTR_pid,
    MP_QSTR_pswd,
    MP_QSTR_keepalive,
    MP_QSTR_lw_topic,
    MP_QSTR_lw_msg,
    MP_QSTR_lw_qos,
    MP_QSTR_lw_retain,
    MP_QSTR__bang_H,
    MP_QSTR_topic,
    MP_QSTR_retain,
    MP_QSTR_qos,
    MP_QSTR_clean_session,
    MP_QSTR__bang_BH,
    MP_QSTR_umqtt_slash_robust_dot_py,
    MP_QSTR_simple,
    MP_QSTR_reconnect,
    MP_QSTR_mqtt_colon__space__percent_r,
    MP_QSTR_in_reconnect,
};

extern const qstr_pool_t mp_qstr_const_pool;
const qstr_pool_t mp_qstr_frozen_const_pool = {
    (qstr_pool_t*)&mp_qstr_const_pool, // previous pool
    MP_QSTRnumber_of, // previous pool size
    10, // allocated entries
    313, // used entries
    {
        (const byte*)"\xab\x3d\x08" "_boot.py",
        (const byte*)"\x70\x30\x04" "bdev",
        (const byte*)"\xc0\xf5\x09" "flashbdev",
        (const byte*)"\x0c\xb4\x08" "inisetup",
        (const byte*)"\xc2\x02\x05" "setup",
        (const byte*)"\x86\x7a\x03" "vfs",
        (const byte*)"\xe7\xe7\x0c" "flashbdev.py",
        (const byte*)"\xa5\xdf\x09" "apa106.py",
        (const byte*)"\x69\x83\x08" "NeoPixel",
        (const byte*)"\x69\xa0\x08" "neopixel",
        (const byte*)"\xa2\x4e\x06" "APA106",
        (const byte*)"\x4b\x69\x05" "ORDER",
        (const byte*)"\xab\x1b\x0b" "inisetup.py",
        (const byte*)"\xff\x70\x0d" "check_bootsec",
        (const byte*)"\xd3\x63\x0c" "fs_corrupted",
        (const byte*)"\x74\xb9\x07" "boot.py",
        (const byte*)"\x9e\xa2\x07" "upip.py",
        (const byte*)"\x79\x6f\x02" "os",
        (const byte*)"\xfd\xd1\x04" "json",
        (const byte*)"\x32\x80\x0d" "upip_utarfile",
        (const byte*)"\x24\xc0\x07" "tarfile",
        (const byte*)"\xd4\x55\x05" "debug",
        (const byte*)"\x7c\x79\x0a" "index_urls",
        (const byte*)"\x56\x0a\x0c" "install_path",
        (const byte*)"\x8f\x1f\x0d" "cleanup_files",
        (const byte*)"\x34\x14\x09" "gzdict_sz",
        (const byte*)"\xed\x15\x08" "file_buf",
        (const byte*)"\x9e\x1a\x0d" "NotFoundError",
        (const byte*)"\xd7\xb9\x08" "op_split",
        (const byte*)"\xb7\x56\x0b" "op_basename",
        (const byte*)"\xb4\xa9\x09" "_makedirs",
        (const byte*)"\xbd\xa0\x09" "save_file",
        (const byte*)"\x7c\xcb\x0b" "install_tar",
        (const byte*)"\x4c\x47\x0a" "expandhome",
        (const byte*)"\x49\xae\x09" "warn_ussl",
        (const byte*)"\xc5\xfa\x08" "url_open",
        (const byte*)"\xa2\x78\x10" "get_pkg_metadata",
        (const byte*)"\xdb\xd0\x05" "fatal",
        (const byte*)"\xa7\xb8\x0b" "install_pkg",
        (const byte*)"\x04\xdb\x07" "install",
        (const byte*)"\xdf\xf8\x10" "get_install_path",
        (const byte*)"\xc5\x2e\x07" "cleanup",
        (const byte*)"\xe4\x55\x05" "fname",
        (const byte*)"\x27\x31\x04" "subf",
        (const byte*)"\x2c\x5b\x06" "setup.",
        (const byte*)"\xfa\xaa\x08" "PKG-INFO",
        (const byte*)"\x5f\xeb\x06" "README",
        (const byte*)"\x2d\x61\x09" ".egg-info",
        (const byte*)"\xae\x9d\x0b" "extractfile",
        (const byte*)"\x87\xec\x04" "deps",
        (const byte*)"\x34\xd0\x08" "Skipping",
        (const byte*)"\x42\x17\x07" "DIRTYPE",
        (const byte*)"\x95\x3e\x06" "prefix",
        (const byte*)"\x14\x71\x02" "~/",
        (const byte*)"\xae\xfb\x06" "getenv",
        (const byte*)"\x9f\xb5\x01" ":",
        (const byte*)"\x34\xf9\x06" "https:",
        (const byte*)"\xdd\x3c\x0a" "%s/%s/json",
        (const byte*)"\x47\x4d\x06" "Error:",
        (const byte*)"\x7c\x46\x03" "msg",
        (const byte*)"\xdb\x24\x03" "exc",
        (const byte*)"\x1f\x8d\x08" "releases",
        (const byte*)"\x64\xfd\x07" "TarFile",
        (const byte*)"\x04\x4c\x07" "fileobj",
        (const byte*)"\x43\x9a\x08" "pkg_spec",
        (const byte*)"\x0e\x69\x06" "Queue:",
        (const byte*)"\x80\xf8\x0a" "to_install",
        (const byte*)"\xfe\x8f\x06" "unlink",
        (const byte*)"\xe0\x66\x02" "-h",
        (const byte*)"\x34\x6e\x06" "--help",
        (const byte*)"\xf8\x66\x02" "-p",
        (const byte*)"\xfa\x66\x02" "-r",
        (const byte*)"\x86\xb5\x01" "#",
        (const byte*)"\xe1\x66\x02" "-i",
        (const byte*)"\x34\x9b\x07" "--debug",
        (const byte*)"\x88\xb5\x01" "-",
        (const byte*)"\x55\x87\x10" "upip_utarfile.py",
        (const byte*)"\x82\x45\x0a" "TAR_HEADER",
        (const byte*)"\xcd\xd1\x07" "REGTYPE",
        (const byte*)"\x82\x38\x07" "roundup",
        (const byte*)"\x0a\x55\x0b" "FileSection",
        (const byte*)"\x4c\x5c\x07" "TarInfo",
        (const byte*)"\x04\xaf\x04" "skip",
        (const byte*)"\x54\x75\x0b" "content_len",
        (const byte*)"\xb1\x45\x0b" "aligned_len",
        (const byte*)"\xec\x70\x02" "sz",
        (const byte*)"\xa5\xb5\x01" "\x00",
        (const byte*)"\x0c\x60\x07" "tarinfo",
        (const byte*)"\x5d\x41\x0a" "ntptime.py",
        (const byte*)"\xe8\xfb\x09" "NTP_DELTA",
        (const byte*)"\x12\x8c\x07" "settime",
        (const byte*)"\x4d\x66\x02" "!I",
        (const byte*)"\x5a\xdd\x06" "dht.py",
        (const byte*)"\xee\x71\x03" "pyb",
        (const byte*)"\x28\x6f\x07" "DHTBase",
        (const byte*)"\x5d\x80\x05" "DHT11",
        (const byte*)"\xfd\x80\x05" "DHT22",
        (const byte*)"\x1d\xae\x07" "measure",
        (const byte*)"\x7c\xaf\x08" "humidity",
        (const byte*)"\xe9\xde\x0b" "temperature",
        (const byte*)"\x0f\xd8\x0a" "onewire.py",
        (const byte*)"\x82\x1f\x03" "_ow",
        (const byte*)"\x50\x5d\x0c" "OneWireError",
        (const byte*)"\x68\xd8\x07" "OneWire",
        (const byte*)"\xa4\x6f\x0a" "SEARCH_ROM",
        (const byte*)"\xb9\x51\x09" "MATCH_ROM",
        (const byte*)"\xcb\x11\x08" "SKIP_ROM",
        (const byte*)"\xe2\x4f\x0a" "select_rom",
        (const byte*)"\xfb\x7e\x0b" "_search_rom",
        (const byte*)"\x2c\x4c\x08" "required",
        (const byte*)"\xb5\x8b\x03" "rom",
        (const byte*)"\x26\xa3\x05" "l_rom",
        (const byte*)"\xc8\xfc\x04" "diff",
        (const byte*)"\xa6\x68\x0a" "ds18x20.py",
        (const byte*)"\x01\xb3\x07" "DS18X20",
        (const byte*)"\x41\x3c\x0c" "convert_temp",
        (const byte*)"\x54\x8e\x0c" "read_scratch",
        (const byte*)"\x3b\x20\x0d" "write_scratch",
        (const byte*)"\xe4\x2e\x09" "read_temp",
        (const byte*)"\x7d\x6f\x02" "ow",
        (const byte*)"\x4f\xcf\x09" "CRC error",
        (const byte*)"\xc2\xc5\x14" "uasyncio/__init__.py",
        (const byte*)"\xbe\xdb\x04" "core",
        (const byte*)"\x3f\x98\x0b" "__version__",
        (const byte*)"\x08\x6b\x05" "funcs",
        (const byte*)"\x2a\x9f\x08" "wait_for",
        (const byte*)"\x88\x08\x06" "gather",
        (const byte*)"\xc9\x69\x05" "event",
        (const byte*)"\xe9\x8a\x05" "Event",
        (const byte*)"\x8e\xe8\x04" "Lock",
        (const byte*)"\x59\xd4\x06" "stream",
        (const byte*)"\x1a\x5f\x06" "_attrs",
        (const byte*)"\xb6\xbc\x04" "attr",
        (const byte*)"\x03\x56\x10" "uasyncio/core.py",
        (const byte*)"\x43\x08\x05" "ticks",
        (const byte*)"\x28\x9e\x04" "task",
        (const byte*)"\x66\x4b\x0c" "TimeoutError",
        (const byte*)"\x6e\xbc\x07" "message",
        (const byte*)"\xd2\xeb\x09" "exception",
        (const byte*)"\x00\x39\x06" "future",
        (const byte*)"\x24\x33\x0c" "_exc_context",
        (const byte*)"\x39\x31\x12" "SingletonGenerator",
        (const byte*)"\x92\x65\x07" "IOQueue",
        (const byte*)"\x52\x61\x10" "_promote_to_task",
        (const byte*)"\x93\x52\x0b" "create_task",
        (const byte*)"\xef\x93\x12" "run_until_complete",
        (const byte*)"\x25\x32\x08" "_stopper",
        (const byte*)"\x10\x16\x0a" "_stop_task",
        (const byte*)"\x19\xe9\x04" "Loop",
        (const byte*)"\x03\x4e\x0e" "get_event_loop",
        (const byte*)"\x1e\x22\x0c" "current_task",
        (const byte*)"\x89\x0c\x0e" "new_event_loop",
        (const byte*)"\xd1\xb5\x01" "t",
        (const byte*)"\x9a\x7d\x04" "sgen",
        (const byte*)"\xc0\x68\x08" "_enqueue",
        (const byte*)"\x6a\xc2\x08" "_dequeue",
        (const byte*)"\x79\xcd\x0a" "queue_read",
        (const byte*)"\x16\x22\x0b" "queue_write",
        (const byte*)"\x24\x1b\x0d" "wait_io_event",
        (const byte*)"\xed\xa4\x06" "poller",
        (const byte*)"\x50\x54\x03" "idx",
        (const byte*)"\x33\x6e\x02" "aw",
        (const byte*)"\xd2\xa9\x09" "_io_queue",
        (const byte*)"\xe8\x4c\x16" "call_exception_handler",
        (const byte*)"\xfc\x7b\x09" "main_task",
        (const byte*)"\x83\xcc\x0c" "_exc_handler",
        (const byte*)"\xcc\x61\x0b" "run_forever",
        (const byte*)"\xa8\x14\x15" "set_exception_handler",
        (const byte*)"\xbc\xec\x15" "get_exception_handler",
        (const byte*)"\x41\xb3\x19" "default_exception_handler",
        (const byte*)"\x3a\x59\x07" "future:",
        (const byte*)"\x09\x52\x05" "coro=",
        (const byte*)"\xfa\xd8\x07" "context",
        (const byte*)"\x25\xce\x08" "runq_len",
        (const byte*)"\x87\x55\x09" "waitq_len",
        (const byte*)"\xd4\xe4\x11" "uasyncio/event.py",
        (const byte*)"\x56\x25\x0e" "ThreadSafeFlag",
        (const byte*)"\xc2\xe4\x06" "is_set",
        (const byte*)"\x8e\x55\x04" "wait",
        (const byte*)"\xee\x79\x07" "waiting",
        (const byte*)"\x96\x10\x05" "_flag",
        (const byte*)"\x63\x8b\x03" "req",
        (const byte*)"\xb5\x5f\x11" "uasyncio/funcs.py",
        (const byte*)"\xcb\x61\x0b" "wait_for_ms",
        (const byte*)"\xd4\xb9\x11" "return_exceptions",
        (const byte*)"\xf5\x5b\x06" "runner",
        (const byte*)"\xf9\xec\x06" "waiter",
        (const byte*)"\x93\x27\x10" "uasyncio/lock.py",
        (const byte*)"\xfc\xcb\x08" "exc_type",
        (const byte*)"\x93\x6f\x02" "tb",
        (const byte*)"\x84\xde\x12" "uasyncio/stream.py",
        (const byte*)"\x39\xd8\x06" "Stream",
        (const byte*)"\x9c\x9a\x0c" "StreamReader",
        (const byte*)"\x36\x8b\x0c" "StreamWriter",
        (const byte*)"\xd8\x63\x0f" "open_connection",
        (const byte*)"\xa0\x0b\x06" "Server",
        (const byte*)"\xbf\xf8\x0c" "start_server",
        (const byte*)"\x5a\xe8\x0d" "stream_awrite",
        (const byte*)"\xe3\x72\x0b" "wait_closed",
        (const byte*)"\x52\x60\x06" "aclose",
        (const byte*)"\xf9\xc1\x06" "awrite",
        (const byte*)"\xec\x0a\x09" "awritestr",
        (const byte*)"\xc7\x40\x0e" "get_extra_info",
        (const byte*)"\xc9\x5c\x0b" "readexactly",
        (const byte*)"\x75\x49\x05" "drain",
        (const byte*)"\x85\x75\x07" "out_buf",
        (const byte*)"\xcb\xb5\x01" "n",
        (const byte*)"\xad\xd8\x06" "_serve",
        (const byte*)"\x80\x20\x08" "peername",
        (const byte*)"\x6a\xa9\x07" "backlog",
        (const byte*)"\x99\x4f\x0a" "webrepl.py",
        (const byte*)"\xa9\x14\x10" "websocket_helper",
        (const byte*)"\x00\xea\x08" "listen_s",
        (const byte*)"\xd0\x46\x08" "client_s",
        (const byte*)"\x71\xe8\x0a" "setup_conn",
        (const byte*)"\x56\xbe\x0b" "accept_conn",
        (const byte*)"\xa1\x7a\x10" "start_foreground",
        (const byte*)"\xcb\xfa\x07" "0.0.0.0",
        (const byte*)"\xc2\x8d\x0e" "accept_handler",
        (const byte*)"\x4b\x21\x08" "rejected",
        (const byte*)"\xc8\xc4\x10" "server_handshake",
        (const byte*)"\xc7\x7d\x0b" "listen_sock",
        (const byte*)"\x23\x03\x0b" "webrepl_cfg",
        (const byte*)"\xb4\xef\x04" "PASS",
        (const byte*)"\x01\xe8\x10" "webrepl_setup.py",
        (const byte*)"\x15\x6d\x09" "./boot.py",
        (const byte*)"\x94\x74\x02" "RC",
        (const byte*)"\x8f\xbe\x06" "CONFIG",
        (const byte*)"\xe7\x42\x0c" "input_choice",
        (const byte*)"\x42\xe4\x07" "getpass",
        (const byte*)"\x7d\xe7\x0a" "input_pass",
        (const byte*)"\x05\x65\x06" "exists",
        (const byte*)"\xeb\xc0\x11" "get_daemon_status",
        (const byte*)"\x50\xd3\x0d" "change_daemon",
        (const byte*)"\xe1\x9d\x06" "prompt",
        (const byte*)"\x9d\x98\x07" "choices",
        (const byte*)"\xfe\x61\x07" "webrepl",
        (const byte*)"\x22\x7c\x04" ".tmp",
        (const byte*)"\x5b\xeb\x06" "action",
        (const byte*)"\xe0\x9e\x07" "enabled",
        (const byte*)"\xdb\x68\x02" "> ",
        (const byte*)"\xe0\xb5\x01" "E",
        (const byte*)"\x94\xfc\x0a" "\x50\x41\x53\x53\x20\x3d\x20\x25\x72\x0a",
        (const byte*)"\xe1\xb5\x01" "D",
        (const byte*)"\xee\xbd\x13" "websocket_helper.py",
        (const byte*)"\x91\x3c\x08" "binascii",
        (const byte*)"\x10\x6d\x07" "hashlib",
        (const byte*)"\x94\x95\x10" "client_handshake",
        (const byte*)"\xa2\x88\x03" "rwb",
        (const byte*)"\xbc\xba\x08" "respkey:",
        (const byte*)"\xc5\x9c\x04" "\x0d\x0a\x0d\x0a",
        (const byte*)"\xd1\x9f\x04" "sock",
        (const byte*)"\xae\xc1\x0b" "neopixel.py",
        (const byte*)"\x35\x23\x06" "timing",
        (const byte*)"\x35\x8e\x0c" "urequests.py",
        (const byte*)"\xc6\x16\x08" "Response",
        (const byte*)"\x3d\xd8\x04" "post",
        (const byte*)"\x8b\x9e\x05" "patch",
        (const byte*)"\xac\xd9\x07" "content",
        (const byte*)"\xe1\x8b\x03" "raw",
        (const byte*)"\x52\x18\x07" "_cached",
        (const byte*)"\x07\x0f\x05" "http:",
        (const byte*)"\x05\xfa\x04" "Host",
        (const byte*)"\xc1\xe7\x06" "reason",
        (const byte*)"\xa9\xab\x07" "headers",
        (const byte*)"\xc5\xc4\x08" "upysh.py",
        (const byte*)"\x5a\x73\x02" "LS",
        (const byte*)"\x46\x0d\x03" "PWD",
        (const byte*)"\x9c\x9f\x05" "CLEAR",
        (const byte*)"\x26\x70\x03" "pwd",
        (const byte*)"\x9a\x6e\x02" "ls",
        (const byte*)"\xbe\x6e\x02" "mv",
        (const byte*)"\xda\x70\x02" "rm",
        (const byte*)"\xb3\x4b\x03" "cat",
        (const byte*)"\x3f\xa2\x07" "newfile",
        (const byte*)"\xc7\xc3\x03" "Man",
        (const byte*)"\xa7\x43\x03" "man",
        (const byte*)"\x8b\xb5\x01" ".",
        (const byte*)"\x8a\x2e\x05" "%s/%s",
        (const byte*)"\x6a\x77\x07" "% 8d %s",
        (const byte*)"\x15\x00\x07" "\x1b\x5b\x32\x4a\x1b\x5b\x48",
        (const byte*)"\x0a\x9e\x0f" "umqtt/simple.py",
        (const byte*)"\xce\xf5\x0d" "MQTTException",
        (const byte*)"\x00\x0b\x0a" "MQTTClient",
        (const byte*)"\x8c\x38\x09" "_send_str",
        (const byte*)"\xe0\x88\x09" "_recv_len",
        (const byte*)"\x91\xa9\x0c" "set_callback",
        (const byte*)"\x93\x18\x0d" "set_last_will",
        (const byte*)"\x55\xf2\x04" "ping",
        (const byte*)"\xe8\xaa\x08" "wait_msg",
        (const byte*)"\x05\x99\x09" "check_msg",
        (const byte*)"\x6e\x20\x09" "client_id",
        (const byte*)"\xc0\x28\x06" "server",
        (const byte*)"\xe9\x8d\x03" "ssl",
        (const byte*)"\xea\x78\x0a" "ssl_params",
        (const byte*)"\xf8\x73\x03" "pid",
        (const byte*)"\xb5\x86\x04" "pswd",
        (const byte*)"\x69\x05\x09" "keepalive",
        (const byte*)"\x80\xe4\x08" "lw_topic",
        (const byte*)"\xf8\x43\x06" "lw_msg",
        (const byte*)"\x6c\x54\x06" "lw_qos",
        (const byte*)"\x64\x86\x09" "lw_retain",
        (const byte*)"\x4c\x66\x02" "!H",
        (const byte*)"\x04\xd9\x05" "topic",
        (const byte*)"\x60\x2b\x06" "retain",
        (const byte*)"\xe8\x77\x03" "qos",
        (const byte*)"\x01\x89\x0d" "clean_session",
        (const byte*)"\x4e\x2f\x03" "!BH",
        (const byte*)"\x09\x83\x0f" "umqtt/robust.py",
        (const byte*)"\x0b\x4c\x06" "simple",
        (const byte*)"\x6c\x11\x09" "reconnect",
        (const byte*)"\x54\xf8\x08" "mqtt: %r",
        (const byte*)"\x54\x66\x0c" "in_reconnect",
    },
};

// frozen bytecode for file _boot.py, scope _boot_<module>
STATIC const byte fun_data__boot__lt_module_gt_[117] = {
    0x2c, 0x1a,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR__boot_dot_py & 0xff, MP_QSTR__boot_dot_py >> 8,
    0x28, 0x28, 0x50, 0x23, 0x26, 0x5b, 0x48, 0x4f, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x16, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x16, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x80, 
    0x10, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_flashbdev & 0xff, MP_QSTR_flashbdev >> 8, 
    0x1c, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x16, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x59, 
    0x48, 0x18, 0x00, 
    0x11, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x44, 0x0f, 0x80, 
    0x11, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x14, MP_QSTR_mount & 0xff, MP_QSTR_mount >> 8, 
    0x11, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0x36, 0x02, 
    0x59, 
    0x4a, 0x20, 0x00, 
    0x57, 
    0x11, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x17, 0x80, 
    0x59, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_inisetup & 0xff, MP_QSTR_inisetup >> 8, 
    0x16, MP_QSTR_inisetup & 0xff, MP_QSTR_inisetup >> 8, 
    0x11, MP_QSTR_inisetup & 0xff, MP_QSTR_inisetup >> 8, 
    0x14, MP_QSTR_setup & 0xff, MP_QSTR_setup >> 8, 
    0x36, 0x00, 
    0x16, MP_QSTR_vfs & 0xff, MP_QSTR_vfs >> 8, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x11, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x14, MP_QSTR_collect & 0xff, MP_QSTR_collect >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
const mp_raw_code_t raw_code__boot__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data__boot__lt_module_gt_,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 117,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR__boot_dot_py,
        .line_info = fun_data__boot__lt_module_gt_ + 6,
        .opcodes = fun_data__boot__lt_module_gt_ + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file flashbdev.py, scope flashbdev_<module>
STATIC const byte fun_data_flashbdev__lt_module_gt_[69] = {
    0x20, 0x0e,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_flashbdev_dot_py & 0xff, MP_QSTR_flashbdev_dot_py >> 8,
    0x50, 0x38, 0x00,
    0x80, 
    0x10, MP_QSTR_Partition & 0xff, MP_QSTR_Partition >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_esp32 & 0xff, MP_QSTR_esp32 >> 8, 
    0x1c, MP_QSTR_Partition & 0xff, MP_QSTR_Partition >> 8, 
    0x16, MP_QSTR_Partition & 0xff, MP_QSTR_Partition >> 8, 
    0x59, 
    0x11, MP_QSTR_Partition & 0xff, MP_QSTR_Partition >> 8, 
    0x14, MP_QSTR_find & 0xff, MP_QSTR_find >> 8, 
    0x11, MP_QSTR_Partition & 0xff, MP_QSTR_Partition >> 8, 
    0x13, MP_QSTR_TYPE_DATA & 0xff, MP_QSTR_TYPE_DATA >> 8, 
    0x10, MP_QSTR_label & 0xff, MP_QSTR_label >> 8, 
    0x10, MP_QSTR_vfs & 0xff, MP_QSTR_vfs >> 8, 
    0x36, 0x82, 0x01, 
    0x16, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x11, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x44, 0x08, 0x80, 
    0x11, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x80, 
    0x55, 
    0x42, 0x01, 0x80, 
    0x51, 
    0x16, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x51, 
    0x63, 
};
const mp_raw_code_t raw_code_flashbdev__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_flashbdev__lt_module_gt_,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 69,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_flashbdev_dot_py,
        .line_info = fun_data_flashbdev__lt_module_gt_ + 6,
        .opcodes = fun_data_flashbdev__lt_module_gt_ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file apa106.py, scope apa106__lt_module_gt__APA106
STATIC const byte fun_data_apa106__lt_module_gt__APA106[32] = {
    0x18, 0x0e,
    MP_QSTR_APA106 & 0xff, MP_QSTR_APA106 >> 8,
    MP_QSTR_apa106_dot_py & 0xff, MP_QSTR_apa106_dot_py >> 8,
    0x8c, 0x07, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_APA106 & 0xff, MP_QSTR_APA106 >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x80, 
    0x81, 
    0x82, 
    0x83, 
    0x2a, 0x04, 
    0x16, MP_QSTR_ORDER & 0xff, MP_QSTR_ORDER >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_apa106__lt_module_gt__APA106 = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_apa106__lt_module_gt__APA106,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 32,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_APA106,
        .qstr_source_file = MP_QSTR_apa106_dot_py,
        .line_info = fun_data_apa106__lt_module_gt__APA106 + 6,
        .opcodes = fun_data_apa106__lt_module_gt__APA106 + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file apa106.py, scope apa106_<module>
STATIC const byte fun_data_apa106__lt_module_gt_[41] = {
    0x18, 0x0e,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_apa106_dot_py & 0xff, MP_QSTR_apa106_dot_py >> 8,
    0x60, 0x70, 0x00,
    0x80, 
    0x10, MP_QSTR_NeoPixel & 0xff, MP_QSTR_NeoPixel >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_neopixel & 0xff, MP_QSTR_neopixel >> 8, 
    0x1c, MP_QSTR_NeoPixel & 0xff, MP_QSTR_NeoPixel >> 8, 
    0x16, MP_QSTR_NeoPixel & 0xff, MP_QSTR_NeoPixel >> 8, 
    0x59, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_APA106 & 0xff, MP_QSTR_APA106 >> 8, 
    0x11, MP_QSTR_NeoPixel & 0xff, MP_QSTR_NeoPixel >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_APA106 & 0xff, MP_QSTR_APA106 >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_apa106__lt_module_gt_[1] = {
    MP_ROM_PTR(&raw_code_apa106__lt_module_gt__APA106),
};
const mp_raw_code_t raw_code_apa106__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_apa106__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_apa106__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 41,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_apa106_dot_py,
        .line_info = fun_data_apa106__lt_module_gt_ + 6,
        .opcodes = fun_data_apa106__lt_module_gt_ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file inisetup.py, scope inisetup__lt_module_gt__check_bootsec
STATIC const byte fun_data_inisetup__lt_module_gt__check_bootsec[87] = {
    0x40, 0x20,
    MP_QSTR_check_bootsec & 0xff, MP_QSTR_check_bootsec >> 8,
    MP_QSTR_inisetup_dot_py & 0xff, MP_QSTR_inisetup_dot_py >> 8,
    0x60, 0x40, 0x30, 0x2b, 0x22, 0x26, 0x28, 0x22, 0x2a, 0x24, 0x22, 0x00,
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x12, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x14, MP_QSTR_ioctl & 0xff, MP_QSTR_ioctl >> 8, 
    0x85, 
    0x80, 
    0x36, 0x02, 
    0x34, 0x01, 
    0xc0, 
    0x12, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x14, MP_QSTR_readblocks & 0xff, MP_QSTR_readblocks >> 8, 
    0x80, 
    0xb0, 
    0x36, 0x02, 
    0x59, 
    0x52, 
    0xc1, 
    0xb0, 
    0x5f, 
    0x4b, 0x15, 0x00, 
    0xc2, 
    0xb2, 
    0x22, 0x81, 0x7f, 
    0xdc, 
    0x44, 0x09, 0x80, 
    0x50, 
    0xc1, 
    0x59, 
    0x59, 
    0x59, 
    0x59, 
    0x42, 0x03, 0x80, 
    0x42, 0xe8, 0x7f, 
    0xb1, 
    0x44, 0x02, 0x80, 
    0x52, 
    0x63, 
    0x12, MP_QSTR_fs_corrupted & 0xff, MP_QSTR_fs_corrupted >> 8, 
    0x34, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_inisetup__lt_module_gt__check_bootsec = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_inisetup__lt_module_gt__check_bootsec,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 87,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 9,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_check_bootsec,
        .qstr_source_file = MP_QSTR_inisetup_dot_py,
        .line_info = fun_data_inisetup__lt_module_gt__check_bootsec + 6,
        .opcodes = fun_data_inisetup__lt_module_gt__check_bootsec + 18,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file inisetup.py, scope inisetup__lt_module_gt__fs_corrupted
STATIC const byte fun_data_inisetup__lt_module_gt__fs_corrupted[41] = {
    0x18, 0x18,
    MP_QSTR_fs_corrupted & 0xff, MP_QSTR_fs_corrupted >> 8,
    MP_QSTR_inisetup_dot_py & 0xff, MP_QSTR_inisetup_dot_py >> 8,
    0x80, 0x12, 0x46, 0x20, 0x23, 0x85, 0x07, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_time & 0xff, MP_QSTR_time >> 8, 
    0xc0, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x00, 
    0x34, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_sleep & 0xff, MP_QSTR_sleep >> 8, 
    0x83, 
    0x36, 0x01, 
    0x59, 
    0x42, 0xed, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_inisetup__lt_module_gt__fs_corrupted_0 = {{&mp_type_str}, 55993, 258, (const byte*)"\x54\x68\x65\x20\x66\x69\x6c\x65\x73\x79\x73\x74\x65\x6d\x20\x61\x70\x70\x65\x61\x72\x73\x20\x74\x6f\x20\x62\x65\x20\x63\x6f\x72\x72\x75\x70\x74\x65\x64\x2e\x20\x49\x66\x20\x79\x6f\x75\x20\x68\x61\x64\x20\x69\x6d\x70\x6f\x72\x74\x61\x6e\x74\x20\x64\x61\x74\x61\x20\x74\x68\x65\x72\x65\x2c\x20\x79\x6f\x75\x0a\x6d\x61\x79\x20\x77\x61\x6e\x74\x20\x74\x6f\x20\x6d\x61\x6b\x65\x20\x61\x20\x66\x6c\x61\x73\x68\x20\x73\x6e\x61\x70\x73\x68\x6f\x74\x20\x74\x6f\x20\x74\x72\x79\x20\x74\x6f\x20\x72\x65\x63\x6f\x76\x65\x72\x20\x69\x74\x2e\x20\x4f\x74\x68\x65\x72\x77\x69\x73\x65\x2c\x20\x70\x65\x72\x66\x6f\x72\x6d\x0a\x66\x61\x63\x74\x6f\x72\x79\x20\x72\x65\x70\x72\x6f\x67\x72\x61\x6d\x6d\x69\x6e\x67\x20\x6f\x66\x20\x4d\x69\x63\x72\x6f\x50\x79\x74\x68\x6f\x6e\x20\x66\x69\x72\x6d\x77\x61\x72\x65\x20\x28\x63\x6f\x6d\x70\x6c\x65\x74\x65\x6c\x79\x20\x65\x72\x61\x73\x65\x20\x66\x6c\x61\x73\x68\x2c\x20\x66\x6f\x6c\x6c\x6f\x77\x65\x64\x0a\x62\x79\x20\x66\x69\x72\x6d\x77\x61\x72\x65\x20\x70\x72\x6f\x67\x72\x61\x6d\x6d\x69\x6e\x67\x29\x2e\x0a"};
STATIC const mp_rom_obj_t const_table_data_inisetup__lt_module_gt__fs_corrupted[1] = {
    MP_ROM_PTR(&const_obj_inisetup__lt_module_gt__fs_corrupted_0),
};
STATIC const mp_raw_code_t raw_code_inisetup__lt_module_gt__fs_corrupted = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_inisetup__lt_module_gt__fs_corrupted,
    .const_table = (mp_uint_t*)const_table_data_inisetup__lt_module_gt__fs_corrupted,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 41,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_fs_corrupted,
        .qstr_source_file = MP_QSTR_inisetup_dot_py,
        .line_info = fun_data_inisetup__lt_module_gt__fs_corrupted + 6,
        .opcodes = fun_data_inisetup__lt_module_gt__fs_corrupted + 14,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file inisetup.py, scope inisetup__lt_module_gt__setup
STATIC const byte fun_data_inisetup__lt_module_gt__setup[101] = {
    0x34, 0x20,
    MP_QSTR_setup & 0xff, MP_QSTR_setup >> 8,
    MP_QSTR_inisetup_dot_py & 0xff, MP_QSTR_inisetup_dot_py >> 8,
    0x80, 0x21, 0x26, 0x28, 0x2f, 0x2c, 0x2d, 0x2f, 0x24, 0x88, 0x08, 0x00,
    0x12, MP_QSTR_check_bootsec & 0xff, MP_QSTR_check_bootsec >> 8, 
    0x34, 0x00, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x00, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x13, MP_QSTR_VfsLfs2 & 0xff, MP_QSTR_VfsLfs2 >> 8, 
    0x14, MP_QSTR_mkfs & 0xff, MP_QSTR_mkfs >> 8, 
    0x12, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x14, MP_QSTR_VfsLfs2 & 0xff, MP_QSTR_VfsLfs2 >> 8, 
    0x12, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x36, 0x01, 
    0xc0, 
    0x12, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x14, MP_QSTR_mount & 0xff, MP_QSTR_mount >> 8, 
    0xb0, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0x36, 0x02, 
    0x59, 
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0x10, MP_QSTR_boot_dot_py & 0xff, MP_QSTR_boot_dot_py >> 8, 
    0x10, MP_QSTR_w & 0xff, MP_QSTR_w >> 8, 
    0x34, 0x02, 
    0x47, 0x0b, 0x00, 
    0xc1, 
    0xb1, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x01, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x5c, 
    0x5d, 
    0xb0, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_inisetup__lt_module_gt__setup_0 = {{&mp_type_str}, 7181, 24, (const byte*)"\x50\x65\x72\x66\x6f\x72\x6d\x69\x6e\x67\x20\x69\x6e\x69\x74\x69\x61\x6c\x20\x73\x65\x74\x75\x70"};
STATIC const mp_obj_str_t const_obj_inisetup__lt_module_gt__setup_1 = {{&mp_type_str}, 18582, 139, (const byte*)"\x23\x20\x54\x68\x69\x73\x20\x66\x69\x6c\x65\x20\x69\x73\x20\x65\x78\x65\x63\x75\x74\x65\x64\x20\x6f\x6e\x20\x65\x76\x65\x72\x79\x20\x62\x6f\x6f\x74\x20\x28\x69\x6e\x63\x6c\x75\x64\x69\x6e\x67\x20\x77\x61\x6b\x65\x2d\x62\x6f\x6f\x74\x20\x66\x72\x6f\x6d\x20\x64\x65\x65\x70\x73\x6c\x65\x65\x70\x29\x0a\x23\x69\x6d\x70\x6f\x72\x74\x20\x65\x73\x70\x0a\x23\x65\x73\x70\x2e\x6f\x73\x64\x65\x62\x75\x67\x28\x4e\x6f\x6e\x65\x29\x0a\x23\x69\x6d\x70\x6f\x72\x74\x20\x77\x65\x62\x72\x65\x70\x6c\x0a\x23\x77\x65\x62\x72\x65\x70\x6c\x2e\x73\x74\x61\x72\x74\x28\x29\x0a"};
STATIC const mp_rom_obj_t const_table_data_inisetup__lt_module_gt__setup[2] = {
    MP_ROM_PTR(&const_obj_inisetup__lt_module_gt__setup_0),
    MP_ROM_PTR(&const_obj_inisetup__lt_module_gt__setup_1),
};
STATIC const mp_raw_code_t raw_code_inisetup__lt_module_gt__setup = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_inisetup__lt_module_gt__setup,
    .const_table = (mp_uint_t*)const_table_data_inisetup__lt_module_gt__setup,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 101,
    .n_obj = 2,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_setup,
        .qstr_source_file = MP_QSTR_inisetup_dot_py,
        .line_info = fun_data_inisetup__lt_module_gt__setup + 6,
        .opcodes = fun_data_inisetup__lt_module_gt__setup + 18,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file inisetup.py, scope inisetup_<module>
STATIC const byte fun_data_inisetup__lt_module_gt_[54] = {
    0x08, 0x16,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_inisetup_dot_py & 0xff, MP_QSTR_inisetup_dot_py >> 8,
    0x28, 0x70, 0x85, 0x0d, 0x85, 0x0f, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x16, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x80, 
    0x10, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_flashbdev & 0xff, MP_QSTR_flashbdev >> 8, 
    0x1c, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x16, MP_QSTR_bdev & 0xff, MP_QSTR_bdev >> 8, 
    0x59, 
    0x32, 0x00, 
    0x16, MP_QSTR_check_bootsec & 0xff, MP_QSTR_check_bootsec >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_fs_corrupted & 0xff, MP_QSTR_fs_corrupted >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_setup & 0xff, MP_QSTR_setup >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_inisetup__lt_module_gt_[3] = {
    MP_ROM_PTR(&raw_code_inisetup__lt_module_gt__check_bootsec),
    MP_ROM_PTR(&raw_code_inisetup__lt_module_gt__fs_corrupted),
    MP_ROM_PTR(&raw_code_inisetup__lt_module_gt__setup),
};
const mp_raw_code_t raw_code_inisetup__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_inisetup__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_inisetup__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 54,
    .n_obj = 0,
    .n_raw_code = 3,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_inisetup_dot_py,
        .line_info = fun_data_inisetup__lt_module_gt_ + 6,
        .opcodes = fun_data_inisetup__lt_module_gt_ + 13,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__NotFoundError
STATIC const byte fun_data_upip__lt_module_gt__NotFoundError[23] = {
    0x00, 0x0e,
    MP_QSTR_NotFoundError & 0xff, MP_QSTR_NotFoundError >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x8c, 0x1c, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_NotFoundError & 0xff, MP_QSTR_NotFoundError >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__NotFoundError = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip__lt_module_gt__NotFoundError,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_NotFoundError,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__NotFoundError + 6,
        .opcodes = fun_data_upip__lt_module_gt__NotFoundError + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__op_split
STATIC const byte fun_data_upip__lt_module_gt__op_split[82] = {
    0x31, 0x1e,
    MP_QSTR_op_split & 0xff, MP_QSTR_op_split >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0x20, 0x28, 0x29, 0x2b, 0x2b, 0x27, 0x24, 0x24, 0x24, 0x00,
    0xb0, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0xd9, 
    0x44, 0x09, 0x80, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x2a, 0x02, 
    0x63, 
    0xb0, 
    0x14, MP_QSTR_rsplit & 0xff, MP_QSTR_rsplit >> 8, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0x81, 
    0x36, 0x02, 
    0xc1, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb1, 
    0x34, 0x01, 
    0x81, 
    0xd9, 
    0x44, 0x07, 0x80, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0xb0, 
    0x2a, 0x02, 
    0x63, 
    0xb1, 
    0x80, 
    0x55, 
    0xc2, 
    0xb2, 
    0x43, 0x04, 0x80, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0xc2, 
    0xb2, 
    0xb1, 
    0x81, 
    0x55, 
    0x2a, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__op_split[1] = {
    MP_ROM_QSTR(MP_QSTR_path),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__op_split = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip__lt_module_gt__op_split,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__op_split,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 82,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_op_split,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__op_split + 6,
        .opcodes = fun_data_upip__lt_module_gt__op_split + 17,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__op_basename
STATIC const byte fun_data_upip__lt_module_gt__op_basename[18] = {
    0x11, 0x0e,
    MP_QSTR_op_basename & 0xff, MP_QSTR_op_basename >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0x2c, 0x00,
    0x12, MP_QSTR_op_split & 0xff, MP_QSTR_op_split >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x81, 
    0x55, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__op_basename[1] = {
    MP_ROM_QSTR(MP_QSTR_path),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__op_basename = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip__lt_module_gt__op_basename,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__op_basename,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 18,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_op_basename,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__op_basename + 6,
        .opcodes = fun_data_upip__lt_module_gt__op_basename + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt___makedirs
STATIC const byte fun_data_upip__lt_module_gt___makedirs[175] = {
    0x82, 0x13, 0x2c,
    MP_QSTR__makedirs & 0xff, MP_QSTR__makedirs >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0x31, 0x22, 0x24, 0x37, 0x2a, 0x24, 0x26, 0x2e, 0x26, 0x24, 0x23, 0x2a, 0x51, 0x3c, 0x22, 0x2f, 0x00,
    0x50, 
    0xc2, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0xc3, 
    0xb0, 
    0x14, MP_QSTR_rstrip & 0xff, MP_QSTR_rstrip >> 8, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0x36, 0x01, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0x36, 0x01, 
    0x51, 
    0x7f, 
    0x2e, 0x02, 
    0x55, 
    0xc4, 
    0xb4, 
    0x80, 
    0x55, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0xd9, 
    0x44, 0x04, 0x80, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0xc3, 
    0xb4, 
    0x5f, 
    0x4b, 0x64, 0x00, 
    0xc5, 
    0xb3, 
    0x44, 0x10, 0x80, 
    0xb3, 
    0x7f, 
    0x55, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0xdc, 
    0x44, 0x06, 0x80, 
    0xb3, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0xe5, 
    0xc3, 
    0xb3, 
    0xb5, 
    0xe5, 
    0xc3, 
    0x48, 0x0f, 0x00, 
    0x12, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x14, MP_QSTR_mkdir & 0xff, MP_QSTR_mkdir >> 8, 
    0xb3, 
    0x36, 0x01, 
    0x59, 
    0x52, 
    0xc2, 
    0x4a, 0x36, 0x00, 
    0x57, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x2d, 0x80, 
    0xc6, 
    0x49, 0x21, 0x00, 
    0xb6, 
    0x13, MP_QSTR_errno & 0xff, MP_QSTR_errno >> 8, 
    0x12, MP_QSTR_errno & 0xff, MP_QSTR_errno >> 8, 
    0x13, MP_QSTR_EEXIST & 0xff, MP_QSTR_EEXIST >> 8, 
    0xdc, 
    0x44, 0x10, 0x80, 
    0xb6, 
    0x13, MP_QSTR_errno & 0xff, MP_QSTR_errno >> 8, 
    0x12, MP_QSTR_errno & 0xff, MP_QSTR_errno >> 8, 
    0x13, MP_QSTR_EISDIR & 0xff, MP_QSTR_EISDIR >> 8, 
    0xdc, 
    0x44, 0x02, 0x80, 
    0xb6, 
    0x65, 
    0x50, 
    0xc2, 
    0x51, 
    0x51, 
    0xc6, 
    0x28, 0x06, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x42, 0x99, 0x7f, 
    0xb2, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt___makedirs[2] = {
    MP_ROM_QSTR(MP_QSTR_name),
    MP_ROM_QSTR(MP_QSTR_mode),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt___makedirs = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip__lt_module_gt___makedirs,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt___makedirs,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 175,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 17,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR__makedirs,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt___makedirs + 7,
        .opcodes = fun_data_upip__lt_module_gt___makedirs + 25,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__save_file
STATIC const byte fun_data_upip__lt_module_gt__save_file[64] = {
    0x4e, 0x1a,
    MP_QSTR_save_file & 0xff, MP_QSTR_save_file >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0x45, 0x20, 0x2d, 0x20, 0x2a, 0x24, 0x23, 0x00,
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0xb0, 
    0x10, MP_QSTR_wb & 0xff, MP_QSTR_wb >> 8, 
    0x34, 0x02, 
    0x47, 0x21, 0x00, 
    0xc2, 
    0xb1, 
    0x14, MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8, 
    0x12, MP_QSTR_file_buf & 0xff, MP_QSTR_file_buf >> 8, 
    0x36, 0x01, 
    0xc3, 
    0xb3, 
    0x43, 0x03, 0x80, 
    0x42, 0x0e, 0x80, 
    0xb2, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x12, MP_QSTR_file_buf & 0xff, MP_QSTR_file_buf >> 8, 
    0xb3, 
    0x36, 0x02, 
    0x59, 
    0x42, 0xe1, 0x7f, 
    0x51, 
    0x5c, 
    0x5d, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__save_file[2] = {
    MP_ROM_QSTR(MP_QSTR_fname),
    MP_ROM_QSTR(MP_QSTR_subf),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__save_file = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip__lt_module_gt__save_file,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__save_file,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 64,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 10,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_save_file,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__save_file + 6,
        .opcodes = fun_data_upip__lt_module_gt__save_file + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__install_tar
STATIC const byte fun_data_upip__lt_module_gt__install_tar[245] = {
    0x9e, 0x10, 0x3c,
    MP_QSTR_install_tar & 0xff, MP_QSTR_install_tar >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0x4f, 0x23, 0x46, 0x25, 0x23, 0x5d, 0x48, 0x22, 0x50, 0x32, 0x2b, 0x31, 0x22, 0x26, 0x2a, 0x4a, 0x24, 0x24, 0x2e, 0x26, 0x2a, 0x27, 0x28, 0x2b, 0x00,
    0x2c, 0x00, 
    0xc2, 
    0xb0, 
    0x5f, 
    0x4b, 0xca, 0x00, 
    0xc3, 
    0xb3, 
    0x13, MP_QSTR_name & 0xff, MP_QSTR_name >> 8, 
    0xc4, 
    0x48, 0x14, 0x00, 
    0xb4, 
    0xb4, 
    0x14, MP_QSTR_index & 0xff, MP_QSTR_index >> 8, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0x36, 0x01, 
    0x81, 
    0xf2, 
    0x51, 
    0x2e, 0x02, 
    0x55, 
    0xc4, 
    0x4a, 0x11, 0x00, 
    0x57, 
    0x12, MP_QSTR_ValueError & 0xff, MP_QSTR_ValueError >> 8, 
    0xdf, 
    0x44, 0x08, 0x80, 
    0x59, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0xc4, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x52, 
    0xc5, 
    0x10, MP_QSTR_setup_dot_ & 0xff, MP_QSTR_setup_dot_ >> 8, 
    0x10, MP_QSTR_PKG_hyphen_INFO & 0xff, MP_QSTR_PKG_hyphen_INFO >> 8, 
    0x10, MP_QSTR_README & 0xff, MP_QSTR_README >> 8, 
    0x2a, 0x03, 
    0x5f, 
    0x4b, 0x4b, 0x00, 
    0xc6, 
    0xb4, 
    0x14, MP_QSTR_startswith & 0xff, MP_QSTR_startswith >> 8, 
    0xb6, 
    0x36, 0x01, 
    0x43, 0x08, 0x80, 
    0x10, MP_QSTR__dot_egg_hyphen_info & 0xff, MP_QSTR__dot_egg_hyphen_info >> 8, 
    0xb4, 
    0xdd, 
    0x44, 0x35, 0x80, 
    0xb4, 
    0x14, MP_QSTR_endswith & 0xff, MP_QSTR_endswith >> 8, 
    0x23, 0x02, 
    0x36, 0x01, 
    0x44, 0x11, 0x80, 
    0xb0, 
    0x14, MP_QSTR_extractfile & 0xff, MP_QSTR_extractfile >> 8, 
    0xb3, 
    0x36, 0x01, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x36, 0x00, 
    0xb2, 
    0x10, MP_QSTR_deps & 0xff, MP_QSTR_deps >> 8, 
    0x56, 
    0x50, 
    0xc5, 
    0x12, MP_QSTR_debug & 0xff, MP_QSTR_debug >> 8, 
    0x44, 0x0a, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x10, MP_QSTR_Skipping & 0xff, MP_QSTR_Skipping >> 8, 
    0xb4, 
    0x34, 0x02, 
    0x59, 
    0x59, 
    0x59, 
    0x59, 
    0x59, 
    0x42, 0x03, 0x80, 
    0x42, 0xb2, 0x7f, 
    0xb5, 
    0x44, 0x39, 0x80, 
    0xb1, 
    0xb4, 
    0xf2, 
    0xc7, 
    0xb3, 
    0x13, MP_QSTR_type & 0xff, MP_QSTR_type >> 8, 
    0x12, MP_QSTR_tarfile & 0xff, MP_QSTR_tarfile >> 8, 
    0x13, MP_QSTR_DIRTYPE & 0xff, MP_QSTR_DIRTYPE >> 8, 
    0xdc, 
    0x44, 0x27, 0x80, 
    0x12, MP_QSTR_debug & 0xff, MP_QSTR_debug >> 8, 
    0x44, 0x0a, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x03, 
    0xb7, 
    0xf2, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR__makedirs & 0xff, MP_QSTR__makedirs >> 8, 
    0xb7, 
    0x34, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_extractfile & 0xff, MP_QSTR_extractfile >> 8, 
    0xb3, 
    0x36, 0x01, 
    0xc8, 
    0x12, MP_QSTR_save_file & 0xff, MP_QSTR_save_file >> 8, 
    0xb7, 
    0xb8, 
    0x34, 0x02, 
    0x59, 
    0x42, 0x33, 0x7f, 
    0xb2, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__install_tar_0 = {{&mp_type_str}, 36962, 13, (const byte*)"\x2f\x72\x65\x71\x75\x69\x72\x65\x73\x2e\x74\x78\x74"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__install_tar_1 = {{&mp_type_str}, 56712, 11, (const byte*)"\x45\x78\x74\x72\x61\x63\x74\x69\x6e\x67\x20"};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__install_tar[4] = {
    MP_ROM_QSTR(MP_QSTR_f),
    MP_ROM_QSTR(MP_QSTR_prefix),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__install_tar_0),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__install_tar_1),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__install_tar = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip__lt_module_gt__install_tar,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__install_tar,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 245,
    .n_obj = 2,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 20,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_install_tar,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__install_tar + 7,
        .opcodes = fun_data_upip__lt_module_gt__install_tar + 33,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__expandhome
STATIC const byte fun_data_upip__lt_module_gt__expandhome[49] = {
    0x31, 0x14,
    MP_QSTR_expandhome & 0xff, MP_QSTR_expandhome >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0x6f, 0x28, 0x2c, 0x2f, 0x00,
    0x10, MP_QSTR__tilde__slash_ & 0xff, MP_QSTR__tilde__slash_ >> 8, 
    0xb0, 
    0xdd, 
    0x44, 0x1b, 0x80, 
    0x12, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x14, MP_QSTR_getenv & 0xff, MP_QSTR_getenv >> 8, 
    0x10, MP_QSTR_HOME & 0xff, MP_QSTR_HOME >> 8, 
    0x36, 0x01, 
    0xc1, 
    0xb0, 
    0x14, MP_QSTR_replace & 0xff, MP_QSTR_replace >> 8, 
    0x10, MP_QSTR__tilde__slash_ & 0xff, MP_QSTR__tilde__slash_ >> 8, 
    0xb1, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0xf2, 
    0x36, 0x02, 
    0xc0, 
    0xb0, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__expandhome[1] = {
    MP_ROM_QSTR(MP_QSTR_s),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__expandhome = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip__lt_module_gt__expandhome,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__expandhome,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 49,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_expandhome,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__expandhome + 6,
        .opcodes = fun_data_upip__lt_module_gt__expandhome + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__url_open
STATIC const byte fun_data_upip__lt_module_gt__url_open[381] = {
    0x91, 0x12, 0x56,
    MP_QSTR_url_open & 0xff, MP_QSTR_url_open >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0x7c, 0x40, 0x26, 0x47, 0x30, 0x23, 0x24, 0x28, 0x2d, 0x27, 0x1f, 0x42, 0x55, 0x44, 0x32, 0x43, 0x4a, 0x28, 0x2f, 0x26, 0x2a, 0x64, 0x2f, 0x27, 0x2d, 0x27, 0x2e, 0x28, 0x27, 0x20, 0x27, 0x24, 0x28, 0x27, 0x55, 0x27, 0x4c, 0x00,
    0x12, MP_QSTR_debug & 0xff, MP_QSTR_debug >> 8, 
    0x44, 0x07, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0x83, 
    0x36, 0x02, 
    0x30, 0x04, 
    0xc1, 
    0xc2, 
    0xc3, 
    0xc4, 
    0x48, 0x35, 0x00, 
    0x22, 0x83, 0x3b, 
    0xc5, 
    0x10, MP_QSTR__colon_ & 0xff, MP_QSTR__colon_ >> 8, 
    0xb3, 
    0xdd, 
    0x44, 0x14, 0x80, 
    0xb3, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x10, MP_QSTR__colon_ & 0xff, MP_QSTR__colon_ >> 8, 
    0x36, 0x01, 
    0x30, 0x02, 
    0xc3, 
    0xc5, 
    0x12, MP_QSTR_int & 0xff, MP_QSTR_int >> 8, 
    0xb5, 
    0x34, 0x01, 
    0xc5, 
    0x12, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x14, MP_QSTR_getaddrinfo & 0xff, MP_QSTR_getaddrinfo >> 8, 
    0xb3, 
    0xb5, 
    0x80, 
    0x12, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x13, MP_QSTR_SOCK_STREAM & 0xff, MP_QSTR_SOCK_STREAM >> 8, 
    0x36, 0x04, 
    0xc6, 
    0x4a, 0x21, 0x00, 
    0x57, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x18, 0x80, 
    0xc7, 
    0x49, 0x0c, 0x00, 
    0x12, MP_QSTR_fatal & 0xff, MP_QSTR_fatal >> 8, 
    0x23, 0x01, 
    0xb3, 
    0xf8, 
    0xb7, 
    0x34, 0x02, 
    0x59, 
    0x51, 
    0x51, 
    0xc7, 
    0x28, 0x07, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0xb6, 
    0x80, 
    0x55, 
    0xc6, 
    0x12, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x14, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0xb6, 
    0x80, 
    0x55, 
    0xb6, 
    0x81, 
    0x55, 
    0xb6, 
    0x82, 
    0x55, 
    0x36, 0x03, 
    0xc8, 
    0x48, 0x9f, 0x00, 
    0xb8, 
    0x14, MP_QSTR_connect & 0xff, MP_QSTR_connect >> 8, 
    0xb6, 
    0x7f, 
    0x55, 
    0x36, 0x01, 
    0x59, 
    0xb1, 
    0x10, MP_QSTR_https_colon_ & 0xff, MP_QSTR_https_colon_ >> 8, 
    0xd9, 
    0x44, 0x23, 0x80, 
    0x12, MP_QSTR_ussl & 0xff, MP_QSTR_ussl >> 8, 
    0x14, MP_QSTR_wrap_socket & 0xff, MP_QSTR_wrap_socket >> 8, 
    0xb8, 
    0x10, MP_QSTR_server_hostname & 0xff, MP_QSTR_server_hostname >> 8, 
    0xb3, 
    0x36, 0x82, 0x01, 
    0xc8, 
    0x12, MP_QSTR_warn_ussl & 0xff, MP_QSTR_warn_ussl >> 8, 
    0x44, 0x0e, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0xb3, 
    0xf8, 
    0x34, 0x01, 
    0x59, 
    0x50, 
    0x17, MP_QSTR_warn_ussl & 0xff, MP_QSTR_warn_ussl >> 8, 
    0xb8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x03, 
    0xb4, 
    0xb3, 
    0xb5, 
    0x2a, 0x03, 
    0xf8, 
    0x36, 0x01, 
    0x59, 
    0xb8, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0xc9, 
    0xb9, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x51, 
    0x82, 
    0x36, 0x02, 
    0x30, 0x03, 
    0xca, 
    0xcb, 
    0xcc, 
    0xbb, 
    0x23, 0x04, 
    0xdc, 
    0x44, 0x1d, 0x80, 
    0xbb, 
    0x23, 0x05, 
    0xd9, 
    0x43, 0x07, 0x80, 
    0xbb, 
    0x23, 0x06, 
    0xd9, 
    0x44, 0x08, 0x80, 
    0x12, MP_QSTR_NotFoundError & 0xff, MP_QSTR_NotFoundError >> 8, 
    0x23, 0x07, 
    0x34, 0x01, 
    0x65, 
    0x12, MP_QSTR_ValueError & 0xff, MP_QSTR_ValueError >> 8, 
    0xbb, 
    0x34, 0x01, 
    0x65, 
    0xb8, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0xc9, 
    0xb9, 
    0x43, 0x08, 0x80, 
    0x12, MP_QSTR_ValueError & 0xff, MP_QSTR_ValueError >> 8, 
    0x23, 0x08, 
    0x34, 0x01, 
    0x65, 
    0xb9, 
    0x23, 0x09, 
    0xd9, 
    0x44, 0x03, 0x80, 
    0x42, 0x03, 0x80, 
    0x42, 0xe0, 0x7f, 
    0x4a, 0x1f, 0x00, 
    0x57, 
    0x12, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0xdf, 
    0x44, 0x16, 0x80, 
    0xc7, 
    0x49, 0x0a, 0x00, 
    0xb8, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0xb7, 
    0x65, 
    0x51, 
    0x51, 
    0xc7, 
    0x28, 0x07, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0xb8, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__url_open_0 = {{&mp_type_str}, 10889, 35, (const byte*)"\x55\x6e\x61\x62\x6c\x65\x20\x74\x6f\x20\x72\x65\x73\x6f\x6c\x76\x65\x20\x25\x73\x20\x28\x6e\x6f\x20\x49\x6e\x74\x65\x72\x6e\x65\x74\x3f\x29"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__url_open_1 = {{&mp_type_str}, 51159, 44, (const byte*)"\x57\x61\x72\x6e\x69\x6e\x67\x3a\x20\x25\x73\x20\x53\x53\x4c\x20\x63\x65\x72\x74\x69\x66\x69\x63\x61\x74\x65\x20\x69\x73\x20\x6e\x6f\x74\x20\x76\x61\x6c\x69\x64\x61\x74\x65\x64"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__url_open_2 = {{&mp_type_str}, 65109, 33, (const byte*)"\x47\x45\x54\x20\x2f\x25\x73\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a\x48\x6f\x73\x74\x3a\x20\x25\x73\x3a\x25\x73\x0d\x0a\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__url_open_3 = {{&mp_type_bytes}, 28951, 3, (const byte*)"\x32\x30\x30"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__url_open_4 = {{&mp_type_bytes}, 22421, 3, (const byte*)"\x34\x30\x34"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__url_open_5 = {{&mp_type_bytes}, 29975, 3, (const byte*)"\x33\x30\x31"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__url_open_6 = {{&mp_type_str}, 48284, 17, (const byte*)"\x50\x61\x63\x6b\x61\x67\x65\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__url_open_7 = {{&mp_type_str}, 39199, 30, (const byte*)"\x55\x6e\x65\x78\x70\x65\x63\x74\x65\x64\x20\x45\x4f\x46\x20\x69\x6e\x20\x48\x54\x54\x50\x20\x68\x65\x61\x64\x65\x72\x73"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__url_open_8 = {{&mp_type_bytes}, 27298, 2, (const byte*)"\x0d\x0a"};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__url_open[10] = {
    MP_ROM_QSTR(MP_QSTR_url),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__url_open_0),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__url_open_1),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__url_open_2),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__url_open_3),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__url_open_4),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__url_open_5),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__url_open_6),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__url_open_7),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__url_open_8),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__url_open = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip__lt_module_gt__url_open,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__url_open,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 381,
    .n_obj = 9,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 19,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_url_open,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__url_open + 7,
        .opcodes = fun_data_upip__lt_module_gt__url_open + 46,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__get_pkg_metadata
STATIC const byte fun_data_upip__lt_module_gt__get_pkg_metadata[96] = {
    0x65, 0x1c,
    MP_QSTR_get_pkg_metadata & 0xff, MP_QSTR_get_pkg_metadata >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0xae, 0x28, 0x23, 0x5a, 0x28, 0x23, 0x4b, 0x2b, 0x00,
    0x12, MP_QSTR_index_urls & 0xff, MP_QSTR_index_urls >> 8, 
    0x5f, 
    0x4b, 0x3f, 0x00, 
    0xc1, 
    0x48, 0x11, 0x00, 
    0x12, MP_QSTR_url_open & 0xff, MP_QSTR_url_open >> 8, 
    0x10, MP_QSTR__percent_s_slash__percent_s_slash_json & 0xff, MP_QSTR__percent_s_slash__percent_s_slash_json >> 8, 
    0xb1, 
    0xb0, 
    0x2a, 0x02, 
    0xf8, 
    0x34, 0x01, 
    0xc2, 
    0x4a, 0x11, 0x00, 
    0x57, 
    0x12, MP_QSTR_NotFoundError & 0xff, MP_QSTR_NotFoundError >> 8, 
    0xdf, 
    0x44, 0x08, 0x80, 
    0x59, 
    0x40, 0xdc, 0x7f, 0x01, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x49, 0x0b, 0x00, 
    0x12, MP_QSTR_json & 0xff, MP_QSTR_json >> 8, 
    0x14, MP_QSTR_load & 0xff, MP_QSTR_load >> 8, 
    0xb2, 
    0x36, 0x01, 
    0x63, 
    0x51, 
    0xb2, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x5d, 
    0x42, 0xbe, 0x7f, 
    0x12, MP_QSTR_NotFoundError & 0xff, MP_QSTR_NotFoundError >> 8, 
    0x23, 0x01, 
    0x34, 0x01, 
    0x65, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__get_pkg_metadata_0 = {{&mp_type_str}, 48284, 17, (const byte*)"\x50\x61\x63\x6b\x61\x67\x65\x20\x6e\x6f\x74\x20\x66\x6f\x75\x6e\x64"};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__get_pkg_metadata[2] = {
    MP_ROM_QSTR(MP_QSTR_name),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__get_pkg_metadata_0),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__get_pkg_metadata = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip__lt_module_gt__get_pkg_metadata,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__get_pkg_metadata,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 96,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 13,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_get_pkg_metadata,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__get_pkg_metadata + 6,
        .opcodes = fun_data_upip__lt_module_gt__get_pkg_metadata + 16,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__fatal
STATIC const byte fun_data_upip__lt_module_gt__fatal[47] = {
    0xa2, 0x01, 0x14,
    MP_QSTR_fatal & 0xff, MP_QSTR_fatal >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0xbb, 0x2a, 0x2a, 0x22, 0x00,
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x10, MP_QSTR_Error_colon_ & 0xff, MP_QSTR_Error_colon_ >> 8, 
    0xb0, 
    0x34, 0x02, 
    0x59, 
    0xb1, 
    0x44, 0x08, 0x80, 
    0x12, MP_QSTR_debug & 0xff, MP_QSTR_debug >> 8, 
    0x44, 0x02, 0x80, 
    0xb1, 
    0x65, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x14, MP_QSTR_exit & 0xff, MP_QSTR_exit >> 8, 
    0x81, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__fatal[2] = {
    MP_ROM_QSTR(MP_QSTR_msg),
    MP_ROM_QSTR(MP_QSTR_exc),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__fatal = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip__lt_module_gt__fatal,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__fatal,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 47,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_fatal,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__fatal + 7,
        .opcodes = fun_data_upip__lt_module_gt__fatal + 13,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__install_pkg
STATIC const byte fun_data_upip__lt_module_gt__install_pkg[177] = {
    0x8e, 0x10, 0x32,
    MP_QSTR_install_pkg & 0xff, MP_QSTR_install_pkg >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0xc2, 0x47, 0x2a, 0x28, 0x22, 0x29, 0x2f, 0x28, 0x2e, 0x27, 0x27, 0x23, 0x2d, 0x2e, 0x49, 0x28, 0x22, 0x22, 0x29, 0x00,
    0x12, MP_QSTR_get_pkg_metadata & 0xff, MP_QSTR_get_pkg_metadata >> 8, 
    0xb0, 
    0x34, 0x01, 
    0xc2, 
    0xb2, 
    0x10, MP_QSTR_info & 0xff, MP_QSTR_info >> 8, 
    0x55, 
    0x10, MP_QSTR_version & 0xff, MP_QSTR_version >> 8, 
    0x55, 
    0xc3, 
    0xb2, 
    0x10, MP_QSTR_releases & 0xff, MP_QSTR_releases >> 8, 
    0x55, 
    0xb3, 
    0x55, 
    0xc4, 
    0x28, 0x02, 
    0x12, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x14, MP_QSTR_collect & 0xff, MP_QSTR_collect >> 8, 
    0x36, 0x00, 
    0x59, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb4, 
    0x34, 0x01, 
    0x81, 
    0xd9, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0xb4, 
    0x80, 
    0x55, 
    0x10, MP_QSTR_url & 0xff, MP_QSTR_url >> 8, 
    0x55, 
    0xc5, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0xb0, 
    0xb3, 
    0xb5, 
    0x2a, 0x03, 
    0xf8, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_op_basename & 0xff, MP_QSTR_op_basename >> 8, 
    0xb5, 
    0x34, 0x01, 
    0xc6, 
    0x12, MP_QSTR_url_open & 0xff, MP_QSTR_url_open >> 8, 
    0xb5, 
    0x34, 0x01, 
    0xc7, 
    0x49, 0x24, 0x00, 
    0x12, MP_QSTR_uzlib & 0xff, MP_QSTR_uzlib >> 8, 
    0x14, MP_QSTR_DecompIO & 0xff, MP_QSTR_DecompIO >> 8, 
    0xb7, 
    0x12, MP_QSTR_gzdict_sz & 0xff, MP_QSTR_gzdict_sz >> 8, 
    0x36, 0x02, 
    0xc8, 
    0x12, MP_QSTR_tarfile & 0xff, MP_QSTR_tarfile >> 8, 
    0x14, MP_QSTR_TarFile & 0xff, MP_QSTR_TarFile >> 8, 
    0x10, MP_QSTR_fileobj & 0xff, MP_QSTR_fileobj >> 8, 
    0xb8, 
    0x36, 0x82, 0x00, 
    0xc9, 
    0x12, MP_QSTR_install_tar & 0xff, MP_QSTR_install_tar >> 8, 
    0xb9, 
    0xb1, 
    0x34, 0x02, 
    0xca, 
    0x51, 
    0xb7, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x5d, 
    0x28, 0x09, 
    0x28, 0x08, 
    0x12, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x14, MP_QSTR_collect & 0xff, MP_QSTR_collect >> 8, 
    0x36, 0x00, 
    0x59, 
    0xba, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__install_pkg_0 = {{&mp_type_str}, 7236, 24, (const byte*)"\x49\x6e\x73\x74\x61\x6c\x6c\x69\x6e\x67\x20\x25\x73\x20\x25\x73\x20\x66\x72\x6f\x6d\x20\x25\x73"};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__install_pkg[3] = {
    MP_ROM_QSTR(MP_QSTR_pkg_spec),
    MP_ROM_QSTR(MP_QSTR_install_path),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__install_pkg_0),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__install_pkg = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip__lt_module_gt__install_pkg,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__install_pkg,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 177,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 18,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_install_pkg,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__install_pkg + 7,
        .opcodes = fun_data_upip__lt_module_gt__install_pkg + 28,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__install
STATIC const byte fun_data_upip__lt_module_gt__install[299] = {
    0xfa, 0x03, 0x48,
    MP_QSTR_install & 0xff, MP_QSTR_install >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0xdb, 0x20, 0x32, 0x29, 0x44, 0x26, 0x26, 0x2a, 0x26, 0x2c, 0x24, 0x4a, 0x23, 0x23, 0x23, 0x26, 0x2a, 0x28, 0x26, 0x23, 0x28, 0x28, 0x26, 0x27, 0x32, 0x24, 0x32, 0x5b, 0x23, 0x2c, 0x00,
    0x12, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x14, MP_QSTR_mem_free & 0xff, MP_QSTR_mem_free >> 8, 
    0x36, 0x00, 
    0x12, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x14, MP_QSTR_mem_alloc & 0xff, MP_QSTR_mem_alloc >> 8, 
    0x36, 0x00, 
    0xf2, 
    0xc2, 
    0xb2, 
    0x22, 0x84, 0x80, 0x00, 
    0xda, 
    0x44, 0x04, 0x80, 
    0x9c, 
    0x17, MP_QSTR_gzdict_sz & 0xff, MP_QSTR_gzdict_sz >> 8, 
    0xb1, 
    0x51, 
    0xde, 
    0x44, 0x06, 0x80, 
    0x12, MP_QSTR_get_install_path & 0xff, MP_QSTR_get_install_path >> 8, 
    0x34, 0x00, 
    0xc1, 
    0xb1, 
    0x7f, 
    0x55, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0xdc, 
    0x44, 0x06, 0x80, 
    0xb1, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0xe5, 
    0xc1, 
    0x12, MP_QSTR_isinstance & 0xff, MP_QSTR_isinstance >> 8, 
    0xb0, 
    0x12, MP_QSTR_list & 0xff, MP_QSTR_list >> 8, 
    0x34, 0x02, 
    0x43, 0x04, 0x80, 
    0xb0, 
    0x2b, 0x01, 
    0xc0, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0xb1, 
    0xf2, 
    0x34, 0x01, 
    0x59, 
    0x2b, 0x00, 
    0xc3, 
    0x48, 0x78, 0x00, 
    0x42, 0x6e, 0x80, 
    0x12, MP_QSTR_debug & 0xff, MP_QSTR_debug >> 8, 
    0x44, 0x0a, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x10, MP_QSTR_Queue_colon_ & 0xff, MP_QSTR_Queue_colon_ >> 8, 
    0xb0, 
    0x34, 0x02, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_pop & 0xff, MP_QSTR_pop >> 8, 
    0x80, 
    0x36, 0x01, 
    0xc4, 
    0xb4, 
    0xb3, 
    0xdd, 
    0x44, 0x03, 0x80, 
    0x42, 0x4d, 0x80, 
    0x12, MP_QSTR_install_pkg & 0xff, MP_QSTR_install_pkg >> 8, 
    0xb4, 
    0xb1, 
    0x34, 0x02, 
    0xc5, 
    0xb3, 
    0x14, MP_QSTR_append & 0xff, MP_QSTR_append >> 8, 
    0xb4, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_debug & 0xff, MP_QSTR_debug >> 8, 
    0x44, 0x07, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0xb5, 
    0x34, 0x01, 
    0x59, 
    0xb5, 
    0x14, MP_QSTR_get & 0xff, MP_QSTR_get >> 8, 
    0x10, MP_QSTR_deps & 0xff, MP_QSTR_deps >> 8, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x36, 0x02, 
    0x14, MP_QSTR_rstrip & 0xff, MP_QSTR_rstrip >> 8, 
    0x36, 0x00, 
    0xc6, 
    0xb6, 
    0x44, 0x1a, 0x80, 
    0xb6, 
    0x14, MP_QSTR_decode & 0xff, MP_QSTR_decode >> 8, 
    0x10, MP_QSTR_utf_hyphen_8 & 0xff, MP_QSTR_utf_hyphen_8 >> 8, 
    0x36, 0x01, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x10, MP_QSTR__0x0a_ & 0xff, MP_QSTR__0x0a_ >> 8, 
    0x36, 0x01, 
    0xc6, 
    0xb0, 
    0x14, MP_QSTR_extend & 0xff, MP_QSTR_extend >> 8, 
    0xb6, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x43, 0x8e, 0x7f, 
    0x4a, 0x2f, 0x00, 
    0x57, 
    0x12, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0xdf, 
    0x44, 0x26, 0x80, 
    0xc7, 
    0x49, 0x1a, 0x00, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x03, 
    0x14, MP_QSTR_format & 0xff, MP_QSTR_format >> 8, 
    0xb4, 
    0xb7, 
    0x36, 0x02, 
    0x10, MP_QSTR_file & 0xff, MP_QSTR_file >> 8, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_stderr & 0xff, MP_QSTR_stderr >> 8, 
    0x34, 0x82, 0x01, 
    0x59, 
    0x51, 
    0x51, 
    0xc7, 
    0x28, 0x07, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__install_0 = {{&mp_type_str}, 1253, 15, (const byte*)"\x49\x6e\x73\x74\x61\x6c\x6c\x69\x6e\x67\x20\x74\x6f\x3a\x20"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__install_1 = {{&mp_type_str}, 31671, 62, (const byte*)"\x45\x72\x72\x6f\x72\x20\x69\x6e\x73\x74\x61\x6c\x6c\x69\x6e\x67\x20\x27\x7b\x7d\x27\x3a\x20\x7b\x7d\x2c\x20\x70\x61\x63\x6b\x61\x67\x65\x73\x20\x6d\x61\x79\x20\x62\x65\x20\x70\x61\x72\x74\x69\x61\x6c\x6c\x79\x20\x69\x6e\x73\x74\x61\x6c\x6c\x65\x64"};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__install[4] = {
    MP_ROM_QSTR(MP_QSTR_to_install),
    MP_ROM_QSTR(MP_QSTR_install_path),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__install_0),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__install_1),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__install = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip__lt_module_gt__install,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__install,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 299,
    .n_obj = 2,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 16,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_install,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__install + 7,
        .opcodes = fun_data_upip__lt_module_gt__install + 39,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__get_install_path
STATIC const byte fun_data_upip__lt_module_gt__get_install_path[47] = {
    0x08, 0x16,
    MP_QSTR_get_install_path & 0xff, MP_QSTR_get_install_path >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x90, 0x00, 0x20, 0x48, 0x2b, 0x2b, 0x00,
    0x12, MP_QSTR_install_path & 0xff, MP_QSTR_install_path >> 8, 
    0x51, 
    0xde, 
    0x44, 0x0b, 0x80, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_path & 0xff, MP_QSTR_path >> 8, 
    0x81, 
    0x55, 
    0x17, MP_QSTR_install_path & 0xff, MP_QSTR_install_path >> 8, 
    0x12, MP_QSTR_expandhome & 0xff, MP_QSTR_expandhome >> 8, 
    0x12, MP_QSTR_install_path & 0xff, MP_QSTR_install_path >> 8, 
    0x34, 0x01, 
    0x17, MP_QSTR_install_path & 0xff, MP_QSTR_install_path >> 8, 
    0x12, MP_QSTR_install_path & 0xff, MP_QSTR_install_path >> 8, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__get_install_path = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip__lt_module_gt__get_install_path,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 47,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_get_install_path,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__get_install_path + 6,
        .opcodes = fun_data_upip__lt_module_gt__get_install_path + 13,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__cleanup
STATIC const byte fun_data_upip__lt_module_gt__cleanup[64] = {
    0x54, 0x14,
    MP_QSTR_cleanup & 0xff, MP_QSTR_cleanup >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x90, 0x09, 0x28, 0x23, 0x56, 0x00,
    0x12, MP_QSTR_cleanup_files & 0xff, MP_QSTR_cleanup_files >> 8, 
    0x5f, 
    0x4b, 0x2b, 0x00, 
    0xc0, 
    0x48, 0x0d, 0x00, 
    0x12, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x14, MP_QSTR_unlink & 0xff, MP_QSTR_unlink >> 8, 
    0xb0, 
    0x36, 0x01, 
    0x59, 
    0x4a, 0x17, 0x00, 
    0x57, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x0e, 0x80, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x00, 
    0xb0, 
    0xf2, 
    0x34, 0x01, 
    0x59, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x42, 0xd2, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__cleanup_0 = {{&mp_type_str}, 17877, 23, (const byte*)"\x57\x61\x72\x6e\x69\x6e\x67\x3a\x20\x43\x61\x6e\x6e\x6f\x74\x20\x64\x65\x6c\x65\x74\x65\x20"};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__cleanup[1] = {
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__cleanup_0),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__cleanup = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip__lt_module_gt__cleanup,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__cleanup,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 64,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_cleanup,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__cleanup + 6,
        .opcodes = fun_data_upip__lt_module_gt__cleanup + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__help
STATIC const byte fun_data_upip__lt_module_gt__help[48] = {
    0x18, 0x18,
    MP_QSTR_help & 0xff, MP_QSTR_help >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x90, 0x11, 0x23, 0x85, 0x09, 0x30, 0x23, 0x00,
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x00, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x01, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_path & 0xff, MP_QSTR_path >> 8, 
    0x81, 
    0x55, 
    0x34, 0x02, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0x34, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__help_0 = {{&mp_type_str}, 14525, 341, (const byte*)"\x75\x70\x69\x70\x20\x2d\x20\x53\x69\x6d\x70\x6c\x65\x20\x50\x79\x50\x49\x20\x70\x61\x63\x6b\x61\x67\x65\x20\x6d\x61\x6e\x61\x67\x65\x72\x20\x66\x6f\x72\x20\x4d\x69\x63\x72\x6f\x50\x79\x74\x68\x6f\x6e\x0a\x55\x73\x61\x67\x65\x3a\x20\x6d\x69\x63\x72\x6f\x70\x79\x74\x68\x6f\x6e\x20\x2d\x6d\x20\x75\x70\x69\x70\x20\x69\x6e\x73\x74\x61\x6c\x6c\x20\x5b\x2d\x70\x20\x3c\x70\x61\x74\x68\x3e\x5d\x20\x3c\x70\x61\x63\x6b\x61\x67\x65\x3e\x2e\x2e\x2e\x20\x7c\x20\x2d\x72\x20\x3c\x72\x65\x71\x75\x69\x72\x65\x6d\x65\x6e\x74\x73\x2e\x74\x78\x74\x3e\x0a\x69\x6d\x70\x6f\x72\x74\x20\x75\x70\x69\x70\x3b\x20\x75\x70\x69\x70\x2e\x69\x6e\x73\x74\x61\x6c\x6c\x28\x70\x61\x63\x6b\x61\x67\x65\x5f\x6f\x72\x5f\x6c\x69\x73\x74\x2c\x20\x5b\x3c\x70\x61\x74\x68\x3e\x5d\x29\x0a\x0a\x49\x66\x20\x3c\x70\x61\x74\x68\x3e\x20\x69\x73\x20\x6e\x6f\x74\x20\x67\x69\x76\x65\x6e\x2c\x20\x70\x61\x63\x6b\x61\x67\x65\x73\x20\x77\x69\x6c\x6c\x20\x62\x65\x20\x69\x6e\x73\x74\x61\x6c\x6c\x65\x64\x20\x69\x6e\x74\x6f\x20\x73\x79\x73\x2e\x70\x61\x74\x68\x5b\x31\x5d\x0a\x28\x63\x61\x6e\x20\x62\x65\x20\x73\x65\x74\x20\x66\x72\x6f\x6d\x20\x4d\x49\x43\x52\x4f\x50\x59\x50\x41\x54\x48\x20\x65\x6e\x76\x69\x72\x6f\x6e\x6d\x65\x6e\x74\x20\x76\x61\x72\x69\x61\x62\x6c\x65\x2c\x20\x69\x66\x20\x63\x75\x72\x72\x65\x6e\x74\x20\x73\x79\x73\x74\x65\x6d\x0a\x73\x75\x70\x70\x6f\x72\x74\x73\x20\x74\x68\x61\x74\x29\x2e"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__help_1 = {{&mp_type_str}, 17177, 29, (const byte*)"\x43\x75\x72\x72\x65\x6e\x74\x20\x76\x61\x6c\x75\x65\x20\x6f\x66\x20\x73\x79\x73\x2e\x70\x61\x74\x68\x5b\x31\x5d\x3a"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__help_2 = {{&mp_type_str}, 34882, 146, (const byte*)"\x0a\x4e\x6f\x74\x65\x3a\x20\x6f\x6e\x6c\x79\x20\x4d\x69\x63\x72\x6f\x50\x79\x74\x68\x6f\x6e\x20\x70\x61\x63\x6b\x61\x67\x65\x73\x20\x28\x75\x73\x75\x61\x6c\x6c\x79\x2c\x20\x6e\x61\x6d\x65\x64\x20\x6d\x69\x63\x72\x6f\x70\x79\x74\x68\x6f\x6e\x2d\x2a\x29\x20\x61\x72\x65\x20\x73\x75\x70\x70\x6f\x72\x74\x65\x64\x0a\x66\x6f\x72\x20\x69\x6e\x73\x74\x61\x6c\x6c\x61\x74\x69\x6f\x6e\x2c\x20\x75\x70\x69\x70\x20\x64\x6f\x65\x73\x20\x6e\x6f\x74\x20\x73\x75\x70\x70\x6f\x72\x74\x20\x61\x72\x62\x69\x74\x72\x61\x72\x79\x20\x63\x6f\x64\x65\x20\x69\x6e\x20\x73\x65\x74\x75\x70\x2e\x70\x79\x2e\x0a"};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__help[3] = {
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__help_0),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__help_1),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__help_2),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__help = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip__lt_module_gt__help,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__help,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 48,
    .n_obj = 3,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_help,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__help + 6,
        .opcodes = fun_data_upip__lt_module_gt__help + 14,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip__lt_module_gt__main
STATIC const byte fun_data_upip__lt_module_gt__main[423] = {
    0x5c, 0x68,
    MP_QSTR_main & 0xff, MP_QSTR_main >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x90, 0x26, 0x20, 0x20, 0x20, 0x44, 0x1f, 0x2f, 0x26, 0x42, 0x2f, 0x48, 0x43, 0x22, 0x23, 0x29, 0x24, 0x30, 0x26, 0x22, 0x28, 0x2b, 0x27, 0x28, 0x29, 0x24, 0x2a, 0x20, 0x27, 0x24, 0x23, 0x2a, 0x23, 0x36, 0x28, 0x2d, 0x27, 0x28, 0x47, 0x1f, 0x4c, 0x32, 0x24, 0x26, 0x42, 0x47, 0x26, 0x00,
    0x51, 
    0x17, MP_QSTR_install_path & 0xff, MP_QSTR_install_path >> 8, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0x34, 0x01, 
    0x82, 
    0xd7, 
    0x43, 0x1e, 0x80, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0x81, 
    0x55, 
    0x10, MP_QSTR__hyphen_h & 0xff, MP_QSTR__hyphen_h >> 8, 
    0xd9, 
    0x43, 0x0f, 0x80, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0x81, 
    0x55, 
    0x10, MP_QSTR__hyphen__hyphen_help & 0xff, MP_QSTR__hyphen__hyphen_help >> 8, 
    0xd9, 
    0x44, 0x08, 0x80, 
    0x12, MP_QSTR_help & 0xff, MP_QSTR_help >> 8, 
    0x34, 0x00, 
    0x59, 
    0x51, 
    0x63, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0x81, 
    0x55, 
    0x10, MP_QSTR_install & 0xff, MP_QSTR_install >> 8, 
    0xdc, 
    0x44, 0x08, 0x80, 
    0x12, MP_QSTR_fatal & 0xff, MP_QSTR_fatal >> 8, 
    0x23, 0x00, 
    0x34, 0x01, 
    0x59, 
    0x2b, 0x00, 
    0xc0, 
    0x82, 
    0xc1, 
    0x42, 0xc4, 0x80, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0xb1, 
    0x55, 
    0xc2, 
    0xb1, 
    0x81, 
    0xe5, 
    0xc1, 
    0xb2, 
    0x10, MP_QSTR__hyphen_h & 0xff, MP_QSTR__hyphen_h >> 8, 
    0xd9, 
    0x43, 0x08, 0x80, 
    0xb2, 
    0x10, MP_QSTR__hyphen__hyphen_help & 0xff, MP_QSTR__hyphen__hyphen_help >> 8, 
    0xd9, 
    0x44, 0x08, 0x80, 
    0x12, MP_QSTR_help & 0xff, MP_QSTR_help >> 8, 
    0x34, 0x00, 
    0x59, 
    0x51, 
    0x63, 
    0xb2, 
    0x10, MP_QSTR__hyphen_p & 0xff, MP_QSTR__hyphen_p >> 8, 
    0xd9, 
    0x44, 0x12, 0x80, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0xb1, 
    0x55, 
    0x17, MP_QSTR_install_path & 0xff, MP_QSTR_install_path >> 8, 
    0xb1, 
    0x81, 
    0xe5, 
    0xc1, 
    0x42, 0x85, 0x80, 
    0xb2, 
    0x10, MP_QSTR__hyphen_r & 0xff, MP_QSTR__hyphen_r >> 8, 
    0xd9, 
    0x44, 0x48, 0x80, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0xb1, 
    0x55, 
    0xc3, 
    0xb1, 
    0x81, 
    0xe5, 
    0xc1, 
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0xb3, 
    0x34, 0x01, 
    0x47, 0x2d, 0x00, 
    0xc4, 
    0xb4, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0xc5, 
    0xb5, 
    0x43, 0x03, 0x80, 
    0x42, 0x1d, 0x80, 
    0xb5, 
    0x80, 
    0x55, 
    0x10, MP_QSTR__hash_ & 0xff, MP_QSTR__hash_ >> 8, 
    0xd9, 
    0x44, 0x03, 0x80, 
    0x42, 0x0d, 0x80, 
    0xb0, 
    0x14, MP_QSTR_append & 0xff, MP_QSTR_append >> 8, 
    0xb5, 
    0x14, MP_QSTR_rstrip & 0xff, MP_QSTR_rstrip >> 8, 
    0x36, 0x00, 
    0x36, 0x01, 
    0x59, 
    0x42, 0xd5, 0x7f, 
    0x51, 
    0x5c, 
    0x5d, 
    0x42, 0x35, 0x80, 
    0xb2, 
    0x10, MP_QSTR__hyphen_i & 0xff, MP_QSTR__hyphen_i >> 8, 
    0xd9, 
    0x44, 0x14, 0x80, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0xb1, 
    0x55, 
    0x2b, 0x01, 
    0x17, MP_QSTR_index_urls & 0xff, MP_QSTR_index_urls >> 8, 
    0xb1, 
    0x81, 
    0xe5, 
    0xc1, 
    0x42, 0x19, 0x80, 
    0xb2, 
    0x10, MP_QSTR__hyphen__hyphen_debug & 0xff, MP_QSTR__hyphen__hyphen_debug >> 8, 
    0xd9, 
    0x44, 0x07, 0x80, 
    0x52, 
    0x17, MP_QSTR_debug & 0xff, MP_QSTR_debug >> 8, 
    0x42, 0x0a, 0x80, 
    0x12, MP_QSTR_fatal & 0xff, MP_QSTR_fatal >> 8, 
    0x23, 0x01, 
    0xb2, 
    0xf2, 
    0x34, 0x01, 
    0x59, 
    0xb1, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0x34, 0x01, 
    0xd7, 
    0x44, 0x11, 0x80, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0xb1, 
    0x55, 
    0x80, 
    0x55, 
    0x10, MP_QSTR__hyphen_ & 0xff, MP_QSTR__hyphen_ >> 8, 
    0xd9, 
    0x43, 0x1b, 0x7f, 
    0xb0, 
    0x14, MP_QSTR_extend & 0xff, MP_QSTR_extend >> 8, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_argv & 0xff, MP_QSTR_argv >> 8, 
    0xb1, 
    0x51, 
    0x2e, 0x02, 
    0x55, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x43, 0x08, 0x80, 
    0x12, MP_QSTR_help & 0xff, MP_QSTR_help >> 8, 
    0x34, 0x00, 
    0x59, 
    0x51, 
    0x63, 
    0x12, MP_QSTR_install & 0xff, MP_QSTR_install >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_debug & 0xff, MP_QSTR_debug >> 8, 
    0x43, 0x06, 0x80, 
    0x12, MP_QSTR_cleanup & 0xff, MP_QSTR_cleanup >> 8, 
    0x34, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__main_0 = {{&mp_type_str}, 50937, 32, (const byte*)"\x4f\x6e\x6c\x79\x20\x27\x69\x6e\x73\x74\x61\x6c\x6c\x27\x20\x63\x6f\x6d\x6d\x61\x6e\x64\x20\x73\x75\x70\x70\x6f\x72\x74\x65\x64"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__main_1 = {{&mp_type_str}, 40302, 28, (const byte*)"\x55\x6e\x6b\x6e\x6f\x77\x6e\x2f\x75\x6e\x73\x75\x70\x70\x6f\x72\x74\x65\x64\x20\x6f\x70\x74\x69\x6f\x6e\x3a\x20"};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt__main[2] = {
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__main_0),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__main_1),
};
STATIC const mp_raw_code_t raw_code_upip__lt_module_gt__main = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip__lt_module_gt__main,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt__main,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 423,
    .n_obj = 2,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 12,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_main,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt__main + 6,
        .opcodes = fun_data_upip__lt_module_gt__main + 54,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip.py, scope upip_<module>
STATIC const byte fun_data_upip__lt_module_gt_[302] = {
    0x18, 0x72,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_upip_dot_py & 0xff, MP_QSTR_upip_dot_py >> 8,
    0x80, 0x07, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x48, 0x69, 0x24, 0x29, 0x24, 0x25, 0x44, 0x6b, 0x6e, 0x20, 0x85, 0x0c, 0x65, 0x40, 0x8b, 0x14, 0x85, 0x0a, 0x85, 0x20, 0x85, 0x07, 0x28, 0x48, 0x64, 0x85, 0x32, 0x85, 0x0d, 0x89, 0x07, 0x85, 0x18, 0x89, 0x26, 0x85, 0x09, 0x85, 0x08, 0x85, 0x15, 0x85, 0x37, 0x2a, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x16, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x16, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x16, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uerrno & 0xff, MP_QSTR_uerrno >> 8, 
    0x16, MP_QSTR_errno & 0xff, MP_QSTR_errno >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_ujson & 0xff, MP_QSTR_ujson >> 8, 
    0x16, MP_QSTR_json & 0xff, MP_QSTR_json >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uzlib & 0xff, MP_QSTR_uzlib >> 8, 
    0x16, MP_QSTR_uzlib & 0xff, MP_QSTR_uzlib >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_upip_utarfile & 0xff, MP_QSTR_upip_utarfile >> 8, 
    0x16, MP_QSTR_tarfile & 0xff, MP_QSTR_tarfile >> 8, 
    0x11, MP_QSTR_gc & 0xff, MP_QSTR_gc >> 8, 
    0x14, MP_QSTR_collect & 0xff, MP_QSTR_collect >> 8, 
    0x36, 0x00, 
    0x59, 
    0x50, 
    0x17, MP_QSTR_debug & 0xff, MP_QSTR_debug >> 8, 
    0x23, 0x00, 
    0x23, 0x01, 
    0x2b, 0x02, 
    0x17, MP_QSTR_index_urls & 0xff, MP_QSTR_index_urls >> 8, 
    0x51, 
    0x17, MP_QSTR_install_path & 0xff, MP_QSTR_install_path >> 8, 
    0x2b, 0x00, 
    0x16, MP_QSTR_cleanup_files & 0xff, MP_QSTR_cleanup_files >> 8, 
    0x9f, 
    0x17, MP_QSTR_gzdict_sz & 0xff, MP_QSTR_gzdict_sz >> 8, 
    0x11, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x22, 0x84, 0x00, 
    0x34, 0x01, 
    0x17, MP_QSTR_file_buf & 0xff, MP_QSTR_file_buf >> 8, 
    0x54, 
    0x32, 0x02, 
    0x10, MP_QSTR_NotFoundError & 0xff, MP_QSTR_NotFoundError >> 8, 
    0x11, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_NotFoundError & 0xff, MP_QSTR_NotFoundError >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_op_split & 0xff, MP_QSTR_op_split >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_op_basename & 0xff, MP_QSTR_op_basename >> 8, 
    0x22, 0x83, 0x7f, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x05, 
    0x16, MP_QSTR__makedirs & 0xff, MP_QSTR__makedirs >> 8, 
    0x32, 0x06, 
    0x16, MP_QSTR_save_file & 0xff, MP_QSTR_save_file >> 8, 
    0x32, 0x07, 
    0x16, MP_QSTR_install_tar & 0xff, MP_QSTR_install_tar >> 8, 
    0x32, 0x08, 
    0x16, MP_QSTR_expandhome & 0xff, MP_QSTR_expandhome >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_ussl & 0xff, MP_QSTR_ussl >> 8, 
    0x16, MP_QSTR_ussl & 0xff, MP_QSTR_ussl >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x16, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x52, 
    0x17, MP_QSTR_warn_ussl & 0xff, MP_QSTR_warn_ussl >> 8, 
    0x32, 0x09, 
    0x16, MP_QSTR_url_open & 0xff, MP_QSTR_url_open >> 8, 
    0x32, 0x0a, 
    0x16, MP_QSTR_get_pkg_metadata & 0xff, MP_QSTR_get_pkg_metadata >> 8, 
    0x51, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x0b, 
    0x16, MP_QSTR_fatal & 0xff, MP_QSTR_fatal >> 8, 
    0x32, 0x0c, 
    0x16, MP_QSTR_install_pkg & 0xff, MP_QSTR_install_pkg >> 8, 
    0x51, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x0d, 
    0x16, MP_QSTR_install & 0xff, MP_QSTR_install >> 8, 
    0x32, 0x0e, 
    0x16, MP_QSTR_get_install_path & 0xff, MP_QSTR_get_install_path >> 8, 
    0x32, 0x0f, 
    0x16, MP_QSTR_cleanup & 0xff, MP_QSTR_cleanup >> 8, 
    0x32, 0x10, 
    0x16, MP_QSTR_help & 0xff, MP_QSTR_help >> 8, 
    0x32, 0x11, 
    0x16, MP_QSTR_main & 0xff, MP_QSTR_main >> 8, 
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x10, MP_QSTR___main__ & 0xff, MP_QSTR___main__ >> 8, 
    0xd9, 
    0x44, 0x06, 0x80, 
    0x11, MP_QSTR_main & 0xff, MP_QSTR_main >> 8, 
    0x34, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__0 = {{&mp_type_str}, 58328, 26, (const byte*)"\x68\x74\x74\x70\x73\x3a\x2f\x2f\x6d\x69\x63\x72\x6f\x70\x79\x74\x68\x6f\x6e\x2e\x6f\x72\x67\x2f\x70\x69"};
STATIC const mp_obj_str_t const_obj_upip__lt_module_gt__1 = {{&mp_type_str}, 3759, 21, (const byte*)"\x68\x74\x74\x70\x73\x3a\x2f\x2f\x70\x79\x70\x69\x2e\x6f\x72\x67\x2f\x70\x79\x70\x69"};
STATIC const mp_rom_obj_t const_table_data_upip__lt_module_gt_[18] = {
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__0),
    MP_ROM_PTR(&const_obj_upip__lt_module_gt__1),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__NotFoundError),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__op_split),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__op_basename),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt___makedirs),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__save_file),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__install_tar),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__expandhome),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__url_open),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__get_pkg_metadata),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__fatal),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__install_pkg),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__install),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__get_install_path),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__cleanup),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__help),
    MP_ROM_PTR(&raw_code_upip__lt_module_gt__main),
};
const mp_raw_code_t raw_code_upip__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_upip__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 302,
    .n_obj = 2,
    .n_raw_code = 16,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_upip_dot_py,
        .line_info = fun_data_upip__lt_module_gt_ + 6,
        .opcodes = fun_data_upip__lt_module_gt_ + 59,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__roundup
STATIC const byte fun_data_upip_utarfile__lt_module_gt__roundup[20] = {
    0x22, 0x0e,
    MP_QSTR_roundup & 0xff, MP_QSTR_roundup >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x0d, 0x00,
    0xb0, 
    0xb1, 
    0xf2, 
    0x81, 
    0xf3, 
    0xb1, 
    0x81, 
    0xf3, 
    0xd2, 
    0xef, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__roundup[2] = {
    MP_ROM_QSTR(MP_QSTR_val),
    MP_ROM_QSTR(MP_QSTR_align),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__roundup = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__roundup,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__roundup,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 20,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_roundup,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__roundup + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__roundup + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__FileSection___init__
STATIC const byte fun_data_upip_utarfile__lt_module_gt__FileSection___init__[31] = {
    0xa8, 0x04, 0x12,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x12, 0x25, 0x25, 0x00,
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_f & 0xff, MP_QSTR_f >> 8, 
    0xb2, 
    0xb0, 
    0x18, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0xb3, 
    0xb2, 
    0xf3, 
    0xb0, 
    0x18, MP_QSTR_align & 0xff, MP_QSTR_align >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__FileSection___init__[4] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_f),
    MP_ROM_QSTR(MP_QSTR_content_len),
    MP_ROM_QSTR(MP_QSTR_aligned_len),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__FileSection___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 4,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__FileSection___init__,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__FileSection___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 31,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 4,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__FileSection___init__ + 7,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__FileSection___init__ + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__FileSection_read
STATIC const byte fun_data_upip_utarfile__lt_module_gt__FileSection_read[74] = {
    0xaa, 0x01, 0x1c,
    MP_QSTR_read & 0xff, MP_QSTR_read >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x17, 0x29, 0x23, 0x29, 0x25, 0x2b, 0x27, 0x2b, 0x00,
    0xb0, 
    0x13, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0x80, 
    0xd9, 
    0x44, 0x03, 0x80, 
    0x23, 0x02, 
    0x63, 
    0xb1, 
    0xb0, 
    0x13, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0xd8, 
    0x44, 0x05, 0x80, 
    0xb0, 
    0x13, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0xc1, 
    0xb0, 
    0x13, MP_QSTR_f & 0xff, MP_QSTR_f >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0xb1, 
    0x36, 0x01, 
    0xc2, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb2, 
    0x34, 0x01, 
    0xc1, 
    0xb0, 
    0x57, 
    0x13, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0xb1, 
    0xe6, 
    0x5a, 
    0x18, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0xb2, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip_utarfile__lt_module_gt__FileSection_read_0 = {{&mp_type_bytes}, 5381, 0, (const byte*)""};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__FileSection_read[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_sz),
    MP_ROM_PTR(&const_obj_upip_utarfile__lt_module_gt__FileSection_read_0),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__FileSection_read = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__FileSection_read,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__FileSection_read,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 74,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_read,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__FileSection_read + 7,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__FileSection_read + 17,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__FileSection_readinto
STATIC const byte fun_data_upip_utarfile__lt_module_gt__FileSection_readinto[79] = {
    0x2a, 0x1a,
    MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x21, 0x29, 0x22, 0x2e, 0x2f, 0x2b, 0x2b, 0x00,
    0xb0, 
    0x13, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0x80, 
    0xd9, 
    0x44, 0x02, 0x80, 
    0x80, 
    0x63, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb1, 
    0x34, 0x01, 
    0xb0, 
    0x13, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0xd8, 
    0x44, 0x0f, 0x80, 
    0x12, MP_QSTR_memoryview & 0xff, MP_QSTR_memoryview >> 8, 
    0xb1, 
    0x34, 0x01, 
    0x51, 
    0xb0, 
    0x13, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0x2e, 0x02, 
    0x55, 
    0xc1, 
    0xb0, 
    0x13, MP_QSTR_f & 0xff, MP_QSTR_f >> 8, 
    0x14, MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8, 
    0xb1, 
    0x36, 0x01, 
    0xc2, 
    0xb0, 
    0x57, 
    0x13, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0xb2, 
    0xe6, 
    0x5a, 
    0x18, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0xb2, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__FileSection_readinto[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_buf),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__FileSection_readinto = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__FileSection_readinto,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__FileSection_readinto,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 79,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_readinto,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__FileSection_readinto + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__FileSection_readinto + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__FileSection_skip
STATIC const byte fun_data_upip_utarfile__lt_module_gt__FileSection_skip[69] = {
    0x39, 0x1a,
    MP_QSTR_skip & 0xff, MP_QSTR_skip >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x2a, 0x2a, 0x24, 0x27, 0x23, 0x28, 0x2c, 0x00,
    0xb0, 
    0x13, MP_QSTR_content_len & 0xff, MP_QSTR_content_len >> 8, 
    0xb0, 
    0x13, MP_QSTR_align & 0xff, MP_QSTR_align >> 8, 
    0xf2, 
    0xc1, 
    0xb1, 
    0x44, 0x26, 0x80, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x90, 
    0x34, 0x01, 
    0xc2, 
    0x42, 0x18, 0x80, 
    0x12, MP_QSTR_min & 0xff, MP_QSTR_min >> 8, 
    0xb1, 
    0x90, 
    0x34, 0x02, 
    0xc3, 
    0xb0, 
    0x13, MP_QSTR_f & 0xff, MP_QSTR_f >> 8, 
    0x14, MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8, 
    0xb2, 
    0xb3, 
    0x36, 0x02, 
    0x59, 
    0xb1, 
    0xb3, 
    0xe6, 
    0xc1, 
    0xb1, 
    0x43, 0xe4, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__FileSection_skip[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__FileSection_skip = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__FileSection_skip,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__FileSection_skip,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 69,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 8,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_skip,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__FileSection_skip + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__FileSection_skip + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__FileSection
STATIC const byte fun_data_upip_utarfile__lt_module_gt__FileSection[56] = {
    0x08, 0x1a,
    MP_QSTR_FileSection & 0xff, MP_QSTR_FileSection >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x8c, 0x11, 0x65, 0x40, 0x8c, 0x0a, 0x85, 0x09, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_FileSection & 0xff, MP_QSTR_FileSection >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x22, 0x84, 0x80, 0x00, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x01, 
    0x16, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_skip & 0xff, MP_QSTR_skip >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__FileSection[4] = {
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__FileSection___init__),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__FileSection_read),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__FileSection_readinto),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__FileSection_skip),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__FileSection = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__FileSection,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__FileSection,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 56,
    .n_obj = 0,
    .n_raw_code = 4,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_FileSection,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__FileSection + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__FileSection + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__TarInfo___str__
STATIC const byte fun_data_upip_utarfile__lt_module_gt__TarInfo___str__[27] = {
    0x21, 0x0e,
    MP_QSTR___str__ & 0xff, MP_QSTR___str__ >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x35, 0x00,
    0x23, 0x01, 
    0xb0, 
    0x13, MP_QSTR_name & 0xff, MP_QSTR_name >> 8, 
    0xb0, 
    0x13, MP_QSTR_type & 0xff, MP_QSTR_type >> 8, 
    0xb0, 
    0x13, MP_QSTR_size & 0xff, MP_QSTR_size >> 8, 
    0x2a, 0x03, 
    0xf8, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upip_utarfile__lt_module_gt__TarInfo___str___0 = {{&mp_type_str}, 62189, 19, (const byte*)"\x54\x61\x72\x49\x6e\x66\x6f\x28\x25\x72\x2c\x20\x25\x73\x2c\x20\x25\x64\x29"};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__TarInfo___str__[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&const_obj_upip_utarfile__lt_module_gt__TarInfo___str___0),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__TarInfo___str__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__TarInfo___str__,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__TarInfo___str__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 27,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___str__,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__TarInfo___str__ + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__TarInfo___str__ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__TarInfo
STATIC const byte fun_data_upip_utarfile__lt_module_gt__TarInfo[28] = {
    0x00, 0x0e,
    MP_QSTR_TarInfo & 0xff, MP_QSTR_TarInfo >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x8c, 0x34, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_TarInfo & 0xff, MP_QSTR_TarInfo >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___str__ & 0xff, MP_QSTR___str__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__TarInfo[1] = {
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__TarInfo___str__),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__TarInfo = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__TarInfo,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__TarInfo,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 28,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_TarInfo,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__TarInfo + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__TarInfo + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__TarFile___init__
STATIC const byte fun_data_upip_utarfile__lt_module_gt__TarFile___init__[46] = {
    0xab, 0x80, 0x01, 0x14,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x3a, 0x24, 0x48, 0x2d, 0x00,
    0xb2, 
    0x44, 0x08, 0x80, 
    0xb2, 
    0xb0, 
    0x18, MP_QSTR_f & 0xff, MP_QSTR_f >> 8, 
    0x42, 0x0d, 0x80, 
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0xb1, 
    0x10, MP_QSTR_rb & 0xff, MP_QSTR_rb >> 8, 
    0x34, 0x02, 
    0xb0, 
    0x18, MP_QSTR_f & 0xff, MP_QSTR_f >> 8, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR_subf & 0xff, MP_QSTR_subf >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__TarFile___init__[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_name),
    MP_ROM_QSTR(MP_QSTR_fileobj),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__TarFile___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 3,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__TarFile___init__,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__TarFile___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 46,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 2,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__TarFile___init__ + 8,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__TarFile___init__ + 14,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__TarFile_next
STATIC const byte fun_data_upip_utarfile__lt_module_gt__TarFile_next[207] = {
    0x49, 0x2a,
    MP_QSTR_next & 0xff, MP_QSTR_next >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x41, 0x27, 0x2a, 0x2d, 0x24, 0x42, 0x7b, 0x2b, 0x42, 0x26, 0x38, 0x33, 0x37, 0x1f, 0x23, 0x00,
    0xb0, 
    0x13, MP_QSTR_subf & 0xff, MP_QSTR_subf >> 8, 
    0x44, 0x0a, 0x80, 
    0xb0, 
    0x13, MP_QSTR_subf & 0xff, MP_QSTR_subf >> 8, 
    0x14, MP_QSTR_skip & 0xff, MP_QSTR_skip >> 8, 
    0x36, 0x00, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_f & 0xff, MP_QSTR_f >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x22, 0x84, 0x00, 
    0x36, 0x01, 
    0xc1, 
    0xb1, 
    0x43, 0x02, 0x80, 
    0x51, 
    0x63, 
    0x12, MP_QSTR_uctypes & 0xff, MP_QSTR_uctypes >> 8, 
    0x14, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x12, MP_QSTR_uctypes & 0xff, MP_QSTR_uctypes >> 8, 
    0x14, MP_QSTR_addressof & 0xff, MP_QSTR_addressof >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x12, MP_QSTR_TAR_HEADER & 0xff, MP_QSTR_TAR_HEADER >> 8, 
    0x12, MP_QSTR_uctypes & 0xff, MP_QSTR_uctypes >> 8, 
    0x13, MP_QSTR_LITTLE_ENDIAN & 0xff, MP_QSTR_LITTLE_ENDIAN >> 8, 
    0x36, 0x03, 
    0xc2, 
    0xb2, 
    0x13, MP_QSTR_name & 0xff, MP_QSTR_name >> 8, 
    0x80, 
    0x55, 
    0x80, 
    0xd9, 
    0x44, 0x02, 0x80, 
    0x51, 
    0x63, 
    0x12, MP_QSTR_TarInfo & 0xff, MP_QSTR_TarInfo >> 8, 
    0x34, 0x00, 
    0xc3, 
    0x12, MP_QSTR_str & 0xff, MP_QSTR_str >> 8, 
    0xb2, 
    0x13, MP_QSTR_name & 0xff, MP_QSTR_name >> 8, 
    0x10, MP_QSTR_utf_hyphen_8 & 0xff, MP_QSTR_utf_hyphen_8 >> 8, 
    0x34, 0x02, 
    0x14, MP_QSTR_rstrip & 0xff, MP_QSTR_rstrip >> 8, 
    0x10, MP_QSTR__0x00_ & 0xff, MP_QSTR__0x00_ >> 8, 
    0x36, 0x01, 
    0xb3, 
    0x18, MP_QSTR_name & 0xff, MP_QSTR_name >> 8, 
    0x12, MP_QSTR_int & 0xff, MP_QSTR_int >> 8, 
    0x12, MP_QSTR_bytes & 0xff, MP_QSTR_bytes >> 8, 
    0xb2, 
    0x13, MP_QSTR_size & 0xff, MP_QSTR_size >> 8, 
    0x34, 0x01, 
    0x88, 
    0x34, 0x02, 
    0xb3, 
    0x18, MP_QSTR_size & 0xff, MP_QSTR_size >> 8, 
    0x12, MP_QSTR_REGTYPE & 0xff, MP_QSTR_REGTYPE >> 8, 
    0x12, MP_QSTR_DIRTYPE & 0xff, MP_QSTR_DIRTYPE >> 8, 
    0x2b, 0x02, 
    0xb3, 
    0x13, MP_QSTR_name & 0xff, MP_QSTR_name >> 8, 
    0x7f, 
    0x55, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0xd9, 
    0x55, 
    0xb3, 
    0x18, MP_QSTR_type & 0xff, MP_QSTR_type >> 8, 
    0x12, MP_QSTR_FileSection & 0xff, MP_QSTR_FileSection >> 8, 
    0xb0, 
    0x13, MP_QSTR_f & 0xff, MP_QSTR_f >> 8, 
    0xb3, 
    0x13, MP_QSTR_size & 0xff, MP_QSTR_size >> 8, 
    0x12, MP_QSTR_roundup & 0xff, MP_QSTR_roundup >> 8, 
    0xb3, 
    0x13, MP_QSTR_size & 0xff, MP_QSTR_size >> 8, 
    0x22, 0x84, 0x00, 
    0x34, 0x02, 
    0x34, 0x03, 
    0x57, 
    0xb0, 
    0x18, MP_QSTR_subf & 0xff, MP_QSTR_subf >> 8, 
    0xb3, 
    0x18, MP_QSTR_subf & 0xff, MP_QSTR_subf >> 8, 
    0xb3, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__TarFile_next[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__TarFile_next = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__TarFile_next,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__TarFile_next,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 207,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 10,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_next,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__TarFile_next + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__TarFile_next + 23,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__TarFile___iter__
STATIC const byte fun_data_upip_utarfile__lt_module_gt__TarFile___iter__[11] = {
    0x09, 0x0e,
    MP_QSTR___iter__ & 0xff, MP_QSTR___iter__ >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x55, 0x00,
    0xb0, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__TarFile___iter__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__TarFile___iter__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__TarFile___iter__,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__TarFile___iter__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 11,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___iter__,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__TarFile___iter__ + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__TarFile___iter__ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__TarFile___next__
STATIC const byte fun_data_upip_utarfile__lt_module_gt__TarFile___next__[31] = {
    0x19, 0x14,
    MP_QSTR___next__ & 0xff, MP_QSTR___next__ >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x58, 0x27, 0x26, 0x24, 0x00,
    0xb0, 
    0x14, MP_QSTR_next & 0xff, MP_QSTR_next >> 8, 
    0x36, 0x00, 
    0xc1, 
    0xb1, 
    0x51, 
    0xde, 
    0x44, 0x04, 0x80, 
    0x12, MP_QSTR_StopIteration & 0xff, MP_QSTR_StopIteration >> 8, 
    0x65, 
    0xb1, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__TarFile___next__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__TarFile___next__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__TarFile___next__,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__TarFile___next__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 31,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___next__,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__TarFile___next__ + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__TarFile___next__ + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__TarFile_extractfile
STATIC const byte fun_data_upip_utarfile__lt_module_gt__TarFile_extractfile[14] = {
    0x12, 0x0e,
    MP_QSTR_extractfile & 0xff, MP_QSTR_extractfile >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x80, 0x5e, 0x00,
    0xb1, 
    0x13, MP_QSTR_subf & 0xff, MP_QSTR_subf >> 8, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__TarFile_extractfile[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_tarinfo),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__TarFile_extractfile = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__TarFile_extractfile,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__TarFile_extractfile,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 14,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_extractfile,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__TarFile_extractfile + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__TarFile_extractfile + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile__lt_module_gt__TarFile
STATIC const byte fun_data_upip_utarfile__lt_module_gt__TarFile[60] = {
    0x08, 0x1c,
    MP_QSTR_TarFile & 0xff, MP_QSTR_TarFile >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x8c, 0x39, 0x8a, 0x07, 0x85, 0x14, 0x65, 0x65, 0x60, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_TarFile & 0xff, MP_QSTR_TarFile >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x51, 
    0x51, 
    0x2a, 0x02, 
    0x53, 
    0x33, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_next & 0xff, MP_QSTR_next >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR___iter__ & 0xff, MP_QSTR___iter__ >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR___next__ & 0xff, MP_QSTR___next__ >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_extractfile & 0xff, MP_QSTR_extractfile >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt__TarFile[5] = {
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__TarFile___init__),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__TarFile_next),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__TarFile___iter__),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__TarFile___next__),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__TarFile_extractfile),
};
STATIC const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt__TarFile = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip_utarfile__lt_module_gt__TarFile,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt__TarFile,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 60,
    .n_obj = 0,
    .n_raw_code = 5,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_TarFile,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt__TarFile + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt__TarFile + 16,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upip_utarfile.py, scope upip_utarfile_<module>
STATIC const byte fun_data_upip_utarfile__lt_module_gt_[132] = {
    0x18, 0x22,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_upip_utarfile_dot_py & 0xff, MP_QSTR_upip_utarfile_dot_py >> 8,
    0x68, 0x22, 0x38, 0x7b, 0x26, 0x66, 0x65, 0x20, 0x8b, 0x23, 0x6b, 0x40, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uctypes & 0xff, MP_QSTR_uctypes >> 8, 
    0x16, MP_QSTR_uctypes & 0xff, MP_QSTR_uctypes >> 8, 
    0x2c, 0x02, 
    0x11, MP_QSTR_uctypes & 0xff, MP_QSTR_uctypes >> 8, 
    0x13, MP_QSTR_ARRAY & 0xff, MP_QSTR_ARRAY >> 8, 
    0x80, 
    0xed, 
    0x11, MP_QSTR_uctypes & 0xff, MP_QSTR_uctypes >> 8, 
    0x13, MP_QSTR_UINT8 & 0xff, MP_QSTR_UINT8 >> 8, 
    0x22, 0x80, 0x64, 
    0xed, 
    0x2a, 0x02, 
    0x10, MP_QSTR_name & 0xff, MP_QSTR_name >> 8, 
    0x62, 
    0x11, MP_QSTR_uctypes & 0xff, MP_QSTR_uctypes >> 8, 
    0x13, MP_QSTR_ARRAY & 0xff, MP_QSTR_ARRAY >> 8, 
    0x22, 0x80, 0x7c, 
    0xed, 
    0x11, MP_QSTR_uctypes & 0xff, MP_QSTR_uctypes >> 8, 
    0x13, MP_QSTR_UINT8 & 0xff, MP_QSTR_UINT8 >> 8, 
    0x8b, 
    0xed, 
    0x2a, 0x02, 
    0x10, MP_QSTR_size & 0xff, MP_QSTR_size >> 8, 
    0x62, 
    0x16, MP_QSTR_TAR_HEADER & 0xff, MP_QSTR_TAR_HEADER >> 8, 
    0x10, MP_QSTR_dir & 0xff, MP_QSTR_dir >> 8, 
    0x16, MP_QSTR_DIRTYPE & 0xff, MP_QSTR_DIRTYPE >> 8, 
    0x10, MP_QSTR_file & 0xff, MP_QSTR_file >> 8, 
    0x16, MP_QSTR_REGTYPE & 0xff, MP_QSTR_REGTYPE >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR_roundup & 0xff, MP_QSTR_roundup >> 8, 
    0x54, 
    0x32, 0x01, 
    0x10, MP_QSTR_FileSection & 0xff, MP_QSTR_FileSection >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_FileSection & 0xff, MP_QSTR_FileSection >> 8, 
    0x54, 
    0x32, 0x02, 
    0x10, MP_QSTR_TarInfo & 0xff, MP_QSTR_TarInfo >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_TarInfo & 0xff, MP_QSTR_TarInfo >> 8, 
    0x54, 
    0x32, 0x03, 
    0x10, MP_QSTR_TarFile & 0xff, MP_QSTR_TarFile >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_TarFile & 0xff, MP_QSTR_TarFile >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upip_utarfile__lt_module_gt_[4] = {
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__roundup),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__FileSection),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__TarInfo),
    MP_ROM_PTR(&raw_code_upip_utarfile__lt_module_gt__TarFile),
};
const mp_raw_code_t raw_code_upip_utarfile__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upip_utarfile__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_upip_utarfile__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 132,
    .n_obj = 0,
    .n_raw_code = 4,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_upip_utarfile_dot_py,
        .line_info = fun_data_upip_utarfile__lt_module_gt_ + 6,
        .opcodes = fun_data_upip_utarfile__lt_module_gt_ + 19,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ntptime.py, scope ntptime__lt_module_gt__time
STATIC const byte fun_data_ntptime__lt_module_gt__time[135] = {
    0x64, 0x22,
    MP_QSTR_time & 0xff, MP_QSTR_time >> 8,
    MP_QSTR_ntptime_dot_py & 0xff, MP_QSTR_ntptime_dot_py >> 8,
    0x80, 0x11, 0x28, 0x24, 0x33, 0x35, 0x23, 0x28, 0x29, 0x4a, 0x28, 0x34, 0x00,
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x22, 0x30, 
    0x34, 0x01, 
    0xc0, 
    0x9b, 
    0xb0, 
    0x80, 
    0x56, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x14, MP_QSTR_getaddrinfo & 0xff, MP_QSTR_getaddrinfo >> 8, 
    0x12, MP_QSTR_host & 0xff, MP_QSTR_host >> 8, 
    0x22, 0x80, 0x7b, 
    0x36, 0x02, 
    0x80, 
    0x55, 
    0x7f, 
    0x55, 
    0xc1, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x14, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x13, MP_QSTR_AF_INET & 0xff, MP_QSTR_AF_INET >> 8, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x13, MP_QSTR_SOCK_DGRAM & 0xff, MP_QSTR_SOCK_DGRAM >> 8, 
    0x36, 0x02, 
    0xc2, 
    0x49, 0x1b, 0x00, 
    0xb2, 
    0x14, MP_QSTR_settimeout & 0xff, MP_QSTR_settimeout >> 8, 
    0x81, 
    0x36, 0x01, 
    0x59, 
    0xb2, 
    0x14, MP_QSTR_sendto & 0xff, MP_QSTR_sendto >> 8, 
    0xb0, 
    0xb1, 
    0x36, 0x02, 
    0xc3, 
    0xb2, 
    0x14, MP_QSTR_recv & 0xff, MP_QSTR_recv >> 8, 
    0x22, 0x30, 
    0x36, 0x01, 
    0xc4, 
    0x51, 
    0xb2, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x5d, 
    0x12, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x14, MP_QSTR_unpack & 0xff, MP_QSTR_unpack >> 8, 
    0x10, MP_QSTR__bang_I & 0xff, MP_QSTR__bang_I >> 8, 
    0xb4, 
    0xa8, 
    0xac, 
    0x2e, 0x02, 
    0x55, 
    0x36, 0x02, 
    0x80, 
    0x55, 
    0xc5, 
    0xb5, 
    0x12, MP_QSTR_NTP_DELTA & 0xff, MP_QSTR_NTP_DELTA >> 8, 
    0xf3, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_ntptime__lt_module_gt__time = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_ntptime__lt_module_gt__time,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 135,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 13,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_time,
        .qstr_source_file = MP_QSTR_ntptime_dot_py,
        .line_info = fun_data_ntptime__lt_module_gt__time + 6,
        .opcodes = fun_data_ntptime__lt_module_gt__time + 19,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ntptime.py, scope ntptime__lt_module_gt__settime
STATIC const byte fun_data_ntptime__lt_module_gt__settime[79] = {
    0x68, 0x16,
    MP_QSTR_settime & 0xff, MP_QSTR_settime >> 8,
    MP_QSTR_ntptime_dot_py & 0xff, MP_QSTR_ntptime_dot_py >> 8,
    0x80, 0x21, 0x26, 0x26, 0x46, 0x28, 0x00,
    0x12, MP_QSTR_time & 0xff, MP_QSTR_time >> 8, 
    0x34, 0x00, 
    0xc0, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_machine & 0xff, MP_QSTR_machine >> 8, 
    0xc1, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_utime & 0xff, MP_QSTR_utime >> 8, 
    0xc2, 
    0xb2, 
    0x14, MP_QSTR_gmtime & 0xff, MP_QSTR_gmtime >> 8, 
    0xb0, 
    0x36, 0x01, 
    0xc3, 
    0xb1, 
    0x14, MP_QSTR_RTC & 0xff, MP_QSTR_RTC >> 8, 
    0x36, 0x00, 
    0x14, MP_QSTR_datetime & 0xff, MP_QSTR_datetime >> 8, 
    0xb3, 
    0x80, 
    0x55, 
    0xb3, 
    0x81, 
    0x55, 
    0xb3, 
    0x82, 
    0x55, 
    0xb3, 
    0x86, 
    0x55, 
    0x81, 
    0xf2, 
    0xb3, 
    0x83, 
    0x55, 
    0xb3, 
    0x84, 
    0x55, 
    0xb3, 
    0x85, 
    0x55, 
    0x80, 
    0x2a, 0x08, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_ntptime__lt_module_gt__settime = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_ntptime__lt_module_gt__settime,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 79,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 14,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_settime,
        .qstr_source_file = MP_QSTR_ntptime_dot_py,
        .line_info = fun_data_ntptime__lt_module_gt__settime + 6,
        .opcodes = fun_data_ntptime__lt_module_gt__settime + 13,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ntptime.py, scope ntptime_<module>
STATIC const byte fun_data_ntptime__lt_module_gt_[93] = {
    0x24, 0x1e,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_ntptime_dot_py & 0xff, MP_QSTR_ntptime_dot_py >> 8,
    0x23, 0x4c, 0x2c, 0x23, 0x4c, 0x6c, 0x65, 0x65, 0x85, 0x10, 0x00,
    0x48, 0x0b, 0x00, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x16, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x4a, 0x0d, 0x00, 
    0x59, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x16, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x48, 0x0b, 0x00, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_ustruct & 0xff, MP_QSTR_ustruct >> 8, 
    0x16, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x4a, 0x0d, 0x00, 
    0x59, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x16, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x23, 0x00, 
    0x16, MP_QSTR_NTP_DELTA & 0xff, MP_QSTR_NTP_DELTA >> 8, 
    0x23, 0x01, 
    0x16, MP_QSTR_host & 0xff, MP_QSTR_host >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_time & 0xff, MP_QSTR_time >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_settime & 0xff, MP_QSTR_settime >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_int_t const_obj_ntptime__lt_module_gt__0 = {{&mp_type_int}, {.neg=0, .fixed_dig=1, .alloc=2, .len=2, .dig=(uint16_t*)(const uint16_t[]){0xc200,0xbc17}}};
STATIC const mp_obj_str_t const_obj_ntptime__lt_module_gt__1 = {{&mp_type_str}, 33769, 12, (const byte*)"\x70\x6f\x6f\x6c\x2e\x6e\x74\x70\x2e\x6f\x72\x67"};
STATIC const mp_rom_obj_t const_table_data_ntptime__lt_module_gt_[4] = {
    MP_ROM_PTR(&const_obj_ntptime__lt_module_gt__0),
    MP_ROM_PTR(&const_obj_ntptime__lt_module_gt__1),
    MP_ROM_PTR(&raw_code_ntptime__lt_module_gt__time),
    MP_ROM_PTR(&raw_code_ntptime__lt_module_gt__settime),
};
const mp_raw_code_t raw_code_ntptime__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_ntptime__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_ntptime__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 93,
    .n_obj = 2,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_ntptime_dot_py,
        .line_info = fun_data_ntptime__lt_module_gt_ + 6,
        .opcodes = fun_data_ntptime__lt_module_gt_ + 17,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht__lt_module_gt__DHTBase___init__
STATIC const byte fun_data_dht__lt_module_gt__DHTBase___init__[27] = {
    0x1a, 0x10,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x80, 0x0b, 0x25, 0x00,
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x85, 
    0x34, 0x01, 
    0xb0, 
    0x18, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt__DHTBase___init__[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_pin),
};
STATIC const mp_raw_code_t raw_code_dht__lt_module_gt__DHTBase___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_dht__lt_module_gt__DHTBase___init__,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt__DHTBase___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 27,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt__DHTBase___init__ + 6,
        .opcodes = fun_data_dht__lt_module_gt__DHTBase___init__ + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht__lt_module_gt__DHTBase_measure
STATIC const byte fun_data_dht__lt_module_gt__DHTBase_measure[64] = {
    0x21, 0x14,
    MP_QSTR_measure & 0xff, MP_QSTR_measure >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x80, 0x0f, 0x25, 0x2b, 0x3a, 0x00,
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0xc1, 
    0x12, MP_QSTR_dht_readinto & 0xff, MP_QSTR_dht_readinto >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0xb1, 
    0x34, 0x02, 
    0x59, 
    0xb1, 
    0x80, 
    0x55, 
    0xb1, 
    0x81, 
    0x55, 
    0xf2, 
    0xb1, 
    0x82, 
    0x55, 
    0xf2, 
    0xb1, 
    0x83, 
    0x55, 
    0xf2, 
    0x22, 0x81, 0x7f, 
    0xef, 
    0xb1, 
    0x84, 
    0x55, 
    0xdc, 
    0x44, 0x08, 0x80, 
    0x12, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0x23, 0x01, 
    0x34, 0x01, 
    0x65, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_dht__lt_module_gt__DHTBase_measure_0 = {{&mp_type_str}, 53040, 14, (const byte*)"\x63\x68\x65\x63\x6b\x73\x75\x6d\x20\x65\x72\x72\x6f\x72"};
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt__DHTBase_measure[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&const_obj_dht__lt_module_gt__DHTBase_measure_0),
};
STATIC const mp_raw_code_t raw_code_dht__lt_module_gt__DHTBase_measure = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_dht__lt_module_gt__DHTBase_measure,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt__DHTBase_measure,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 64,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_measure,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt__DHTBase_measure + 6,
        .opcodes = fun_data_dht__lt_module_gt__DHTBase_measure + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht__lt_module_gt__DHTBase
STATIC const byte fun_data_dht__lt_module_gt__DHTBase[35] = {
    0x00, 0x12,
    MP_QSTR_DHTBase & 0xff, MP_QSTR_DHTBase >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x8c, 0x0a, 0x65, 0x20, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_DHTBase & 0xff, MP_QSTR_DHTBase >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_measure & 0xff, MP_QSTR_measure >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt__DHTBase[2] = {
    MP_ROM_PTR(&raw_code_dht__lt_module_gt__DHTBase___init__),
    MP_ROM_PTR(&raw_code_dht__lt_module_gt__DHTBase_measure),
};
STATIC const mp_raw_code_t raw_code_dht__lt_module_gt__DHTBase = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_dht__lt_module_gt__DHTBase,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt__DHTBase,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 35,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_DHTBase,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt__DHTBase + 6,
        .opcodes = fun_data_dht__lt_module_gt__DHTBase + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht__lt_module_gt__DHT11_humidity
STATIC const byte fun_data_dht__lt_module_gt__DHT11_humidity[16] = {
    0x11, 0x0e,
    MP_QSTR_humidity & 0xff, MP_QSTR_humidity >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x80, 0x17, 0x00,
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x80, 
    0x55, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt__DHT11_humidity[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_dht__lt_module_gt__DHT11_humidity = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_dht__lt_module_gt__DHT11_humidity,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt__DHT11_humidity,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 16,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_humidity,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt__DHT11_humidity + 6,
        .opcodes = fun_data_dht__lt_module_gt__DHT11_humidity + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht__lt_module_gt__DHT11_temperature
STATIC const byte fun_data_dht__lt_module_gt__DHT11_temperature[16] = {
    0x11, 0x0e,
    MP_QSTR_temperature & 0xff, MP_QSTR_temperature >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x80, 0x1a, 0x00,
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x82, 
    0x55, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt__DHT11_temperature[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_dht__lt_module_gt__DHT11_temperature = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_dht__lt_module_gt__DHT11_temperature,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt__DHT11_temperature,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 16,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_temperature,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt__DHT11_temperature + 6,
        .opcodes = fun_data_dht__lt_module_gt__DHT11_temperature + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht__lt_module_gt__DHT11
STATIC const byte fun_data_dht__lt_module_gt__DHT11[34] = {
    0x00, 0x10,
    MP_QSTR_DHT11 & 0xff, MP_QSTR_DHT11 >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x8c, 0x16, 0x65, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_DHT11 & 0xff, MP_QSTR_DHT11 >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR_humidity & 0xff, MP_QSTR_humidity >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_temperature & 0xff, MP_QSTR_temperature >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt__DHT11[2] = {
    MP_ROM_PTR(&raw_code_dht__lt_module_gt__DHT11_humidity),
    MP_ROM_PTR(&raw_code_dht__lt_module_gt__DHT11_temperature),
};
STATIC const mp_raw_code_t raw_code_dht__lt_module_gt__DHT11 = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_dht__lt_module_gt__DHT11,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt__DHT11,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 34,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_DHT11,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt__DHT11 + 6,
        .opcodes = fun_data_dht__lt_module_gt__DHT11 + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht__lt_module_gt__DHT22_humidity
STATIC const byte fun_data_dht__lt_module_gt__DHT22_humidity[28] = {
    0x19, 0x0e,
    MP_QSTR_humidity & 0xff, MP_QSTR_humidity >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x80, 0x1f, 0x00,
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x80, 
    0x55, 
    0x88, 
    0xf0, 
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x81, 
    0x55, 
    0xed, 
    0x23, 0x01, 
    0xf4, 
    0x63, 
};
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B
STATIC const mp_obj_float_t const_obj_dht__lt_module_gt__DHT22_humidity_0 = {{&mp_type_float}, (mp_float_t)0.1};
#endif
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt__DHT22_humidity[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B
    MP_ROM_PTR(&const_obj_dht__lt_module_gt__DHT22_humidity_0),
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
    (mp_rom_obj_t)(0xbe4cccce),
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
    (mp_rom_obj_t)(0xbfbd99999999999a),
#endif
};
STATIC const mp_raw_code_t raw_code_dht__lt_module_gt__DHT22_humidity = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_dht__lt_module_gt__DHT22_humidity,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt__DHT22_humidity,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 28,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_humidity,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt__DHT22_humidity + 6,
        .opcodes = fun_data_dht__lt_module_gt__DHT22_humidity + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht__lt_module_gt__DHT22_temperature
STATIC const byte fun_data_dht__lt_module_gt__DHT22_temperature[53] = {
    0x21, 0x14,
    MP_QSTR_temperature & 0xff, MP_QSTR_temperature >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x80, 0x22, 0x37, 0x2d, 0x23, 0x00,
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x82, 
    0x55, 
    0x22, 0x80, 0x7f, 
    0xef, 
    0x88, 
    0xf0, 
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x83, 
    0x55, 
    0xed, 
    0x23, 0x01, 
    0xf4, 
    0xc1, 
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x82, 
    0x55, 
    0x22, 0x81, 0x00, 
    0xef, 
    0x44, 0x03, 0x80, 
    0xb1, 
    0xd1, 
    0xc1, 
    0xb1, 
    0x63, 
};
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B
STATIC const mp_obj_float_t const_obj_dht__lt_module_gt__DHT22_temperature_0 = {{&mp_type_float}, (mp_float_t)0.1};
#endif
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt__DHT22_temperature[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B
    MP_ROM_PTR(&const_obj_dht__lt_module_gt__DHT22_temperature_0),
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
    (mp_rom_obj_t)(0xbe4cccce),
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
    (mp_rom_obj_t)(0xbfbd99999999999a),
#endif
};
STATIC const mp_raw_code_t raw_code_dht__lt_module_gt__DHT22_temperature = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_dht__lt_module_gt__DHT22_temperature,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt__DHT22_temperature,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 53,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_temperature,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt__DHT22_temperature + 6,
        .opcodes = fun_data_dht__lt_module_gt__DHT22_temperature + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht__lt_module_gt__DHT22
STATIC const byte fun_data_dht__lt_module_gt__DHT22[34] = {
    0x00, 0x10,
    MP_QSTR_DHT22 & 0xff, MP_QSTR_DHT22 >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x8c, 0x1e, 0x65, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_DHT22 & 0xff, MP_QSTR_DHT22 >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR_humidity & 0xff, MP_QSTR_humidity >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_temperature & 0xff, MP_QSTR_temperature >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt__DHT22[2] = {
    MP_ROM_PTR(&raw_code_dht__lt_module_gt__DHT22_humidity),
    MP_ROM_PTR(&raw_code_dht__lt_module_gt__DHT22_temperature),
};
STATIC const mp_raw_code_t raw_code_dht__lt_module_gt__DHT22 = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_dht__lt_module_gt__DHT22,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt__DHT22,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 34,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_DHT22,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt__DHT22 + 6,
        .opcodes = fun_data_dht__lt_module_gt__DHT22 + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file dht.py, scope dht_<module>
STATIC const byte fun_data_dht__lt_module_gt_[99] = {
    0x24, 0x1a,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_dht_dot_py & 0xff, MP_QSTR_dht_dot_py >> 8,
    0x60, 0x23, 0x54, 0x74, 0x8b, 0x0c, 0x8e, 0x08, 0x00,
    0x48, 0x13, 0x00, 
    0x80, 
    0x10, MP_QSTR_dht_readinto & 0xff, MP_QSTR_dht_readinto >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_esp & 0xff, MP_QSTR_esp >> 8, 
    0x1c, MP_QSTR_dht_readinto & 0xff, MP_QSTR_dht_readinto >> 8, 
    0x16, MP_QSTR_dht_readinto & 0xff, MP_QSTR_dht_readinto >> 8, 
    0x59, 
    0x4a, 0x15, 0x00, 
    0x59, 
    0x80, 
    0x10, MP_QSTR_dht_readinto & 0xff, MP_QSTR_dht_readinto >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_pyb & 0xff, MP_QSTR_pyb >> 8, 
    0x1c, MP_QSTR_dht_readinto & 0xff, MP_QSTR_dht_readinto >> 8, 
    0x16, MP_QSTR_dht_readinto & 0xff, MP_QSTR_dht_readinto >> 8, 
    0x59, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_DHTBase & 0xff, MP_QSTR_DHTBase >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_DHTBase & 0xff, MP_QSTR_DHTBase >> 8, 
    0x54, 
    0x32, 0x01, 
    0x10, MP_QSTR_DHT11 & 0xff, MP_QSTR_DHT11 >> 8, 
    0x11, MP_QSTR_DHTBase & 0xff, MP_QSTR_DHTBase >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_DHT11 & 0xff, MP_QSTR_DHT11 >> 8, 
    0x54, 
    0x32, 0x02, 
    0x10, MP_QSTR_DHT22 & 0xff, MP_QSTR_DHT22 >> 8, 
    0x11, MP_QSTR_DHTBase & 0xff, MP_QSTR_DHTBase >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_DHT22 & 0xff, MP_QSTR_DHT22 >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_dht__lt_module_gt_[3] = {
    MP_ROM_PTR(&raw_code_dht__lt_module_gt__DHTBase),
    MP_ROM_PTR(&raw_code_dht__lt_module_gt__DHT11),
    MP_ROM_PTR(&raw_code_dht__lt_module_gt__DHT22),
};
const mp_raw_code_t raw_code_dht__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_dht__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_dht__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 99,
    .n_obj = 0,
    .n_raw_code = 3,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_dht_dot_py,
        .line_info = fun_data_dht__lt_module_gt_ + 6,
        .opcodes = fun_data_dht__lt_module_gt_ + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWireError
STATIC const byte fun_data_onewire__lt_module_gt__OneWireError[23] = {
    0x00, 0x0e,
    MP_QSTR_OneWireError & 0xff, MP_QSTR_OneWireError >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x8c, 0x07, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_OneWireError & 0xff, MP_QSTR_OneWireError >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWireError = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_onewire__lt_module_gt__OneWireError,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_OneWireError,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWireError + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWireError + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire___init__
STATIC const byte fun_data_onewire__lt_module_gt__OneWire___init__[35] = {
    0x2a, 0x10,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x10, 0x25, 0x00,
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0x14, MP_QSTR_init & 0xff, MP_QSTR_init >> 8, 
    0xb1, 
    0x13, MP_QSTR_OPEN_DRAIN & 0xff, MP_QSTR_OPEN_DRAIN >> 8, 
    0xb1, 
    0x13, MP_QSTR_PULL_UP & 0xff, MP_QSTR_PULL_UP >> 8, 
    0x36, 0x02, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire___init__[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_pin),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire___init__,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 35,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire___init__ + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire___init__ + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_reset
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_reset[40] = {
    0xaa, 0x01, 0x14,
    MP_QSTR_reset & 0xff, MP_QSTR_reset >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x14, 0x2d, 0x28, 0x24, 0x00,
    0x12, MP_QSTR__ow & 0xff, MP_QSTR__ow >> 8, 
    0x14, MP_QSTR_reset & 0xff, MP_QSTR_reset >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0x36, 0x01, 
    0xc2, 
    0xb1, 
    0x44, 0x08, 0x80, 
    0xb2, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_OneWireError & 0xff, MP_QSTR_OneWireError >> 8, 
    0x65, 
    0xb2, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_reset[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_required),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_reset = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_reset,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_reset,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 40,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_reset,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_reset + 7,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_reset + 13,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_readbit
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_readbit[22] = {
    0x19, 0x0e,
    MP_QSTR_readbit & 0xff, MP_QSTR_readbit >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x1a, 0x00,
    0x12, MP_QSTR__ow & 0xff, MP_QSTR__ow >> 8, 
    0x14, MP_QSTR_readbit & 0xff, MP_QSTR_readbit >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0x36, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_readbit[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_readbit = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_readbit,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_readbit,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 22,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_readbit,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_readbit + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_readbit + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_readbyte
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_readbyte[22] = {
    0x19, 0x0e,
    MP_QSTR_readbyte & 0xff, MP_QSTR_readbyte >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x1d, 0x00,
    0x12, MP_QSTR__ow & 0xff, MP_QSTR__ow >> 8, 
    0x14, MP_QSTR_readbyte & 0xff, MP_QSTR_readbyte >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0x36, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_readbyte[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_readbyte = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_readbyte,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_readbyte,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 22,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_readbyte,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_readbyte + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_readbyte + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_readinto
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_readinto[49] = {
    0x3a, 0x10,
    MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x20, 0x2c, 0x00,
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb1, 
    0x34, 0x01, 
    0x80, 
    0x42, 0x13, 0x80, 
    0x57, 
    0xc2, 
    0x12, MP_QSTR__ow & 0xff, MP_QSTR__ow >> 8, 
    0x14, MP_QSTR_readbyte & 0xff, MP_QSTR_readbyte >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0x36, 0x01, 
    0xb1, 
    0xb2, 
    0x56, 
    0x81, 
    0xe5, 
    0x58, 
    0x5a, 
    0xd7, 
    0x43, 0xe7, 0x7f, 
    0x59, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_readinto[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_buf),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_readinto = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_readinto,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_readinto,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 49,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 8,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_readinto,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_readinto + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_readinto + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_writebit
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_writebit[23] = {
    0x2a, 0x0e,
    MP_QSTR_writebit & 0xff, MP_QSTR_writebit >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x24, 0x00,
    0x12, MP_QSTR__ow & 0xff, MP_QSTR__ow >> 8, 
    0x14, MP_QSTR_writebit & 0xff, MP_QSTR_writebit >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0xb1, 
    0x36, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_writebit[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_value),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_writebit = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_writebit,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_writebit,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_writebit,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_writebit + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_writebit + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_writebyte
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_writebyte[23] = {
    0x2a, 0x0e,
    MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x27, 0x00,
    0x12, MP_QSTR__ow & 0xff, MP_QSTR__ow >> 8, 
    0x14, MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0xb1, 
    0x36, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_writebyte[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_value),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_writebyte = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_writebyte,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_writebyte,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_writebyte,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_writebyte + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_writebyte + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_write
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_write[35] = {
    0x52, 0x10,
    MP_QSTR_write & 0xff, MP_QSTR_write >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x2a, 0x26, 0x00,
    0xb1, 
    0x5f, 
    0x4b, 0x12, 0x00, 
    0xc2, 
    0x12, MP_QSTR__ow & 0xff, MP_QSTR__ow >> 8, 
    0x14, MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0xb2, 
    0x36, 0x02, 
    0x59, 
    0x42, 0xeb, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_write[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_buf),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_write = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_write,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_write,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 35,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_write,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_write + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_write + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_select_rom
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_select_rom[39] = {
    0x22, 0x12,
    MP_QSTR_select_rom & 0xff, MP_QSTR_select_rom >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x2e, 0x27, 0x2b, 0x00,
    0xb0, 
    0x14, MP_QSTR_reset & 0xff, MP_QSTR_reset >> 8, 
    0x36, 0x00, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8, 
    0xb0, 
    0x13, MP_QSTR_MATCH_ROM & 0xff, MP_QSTR_MATCH_ROM >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_select_rom[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_rom),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_select_rom = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_select_rom,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_select_rom,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 39,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_select_rom,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_select_rom + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_select_rom + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_scan
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_scan[77] = {
    0x49, 0x20,
    MP_QSTR_scan & 0xff, MP_QSTR_scan >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x33, 0x23, 0x24, 0x22, 0x26, 0x2c, 0x24, 0x26, 0x26, 0x2e, 0x00,
    0x2b, 0x00, 
    0xc1, 
    0x22, 0x80, 0x41, 
    0xc2, 
    0x50, 
    0xc3, 
    0x80, 
    0x42, 0x23, 0x80, 
    0x57, 
    0xc4, 
    0xb0, 
    0x14, MP_QSTR__search_rom & 0xff, MP_QSTR__search_rom >> 8, 
    0xb3, 
    0xb2, 
    0x36, 0x02, 
    0x30, 0x02, 
    0xc3, 
    0xc2, 
    0xb3, 
    0x44, 0x06, 0x80, 
    0xb1, 
    0xb3, 
    0x2b, 0x01, 
    0xe5, 
    0xc1, 
    0xb2, 
    0x80, 
    0xd9, 
    0x44, 0x03, 0x80, 
    0x42, 0x0a, 0x80, 
    0x81, 
    0xe5, 
    0x57, 
    0x22, 0x81, 0x7f, 
    0xd7, 
    0x43, 0xd5, 0x7f, 
    0x59, 
    0xb1, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_scan[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_scan = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_scan,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_scan,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 77,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 10,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_scan,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_scan + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_scan + 18,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire__search_rom
STATIC const byte fun_data_onewire__lt_module_gt__OneWire__search_rom[203] = {
    0x73, 0x3e,
    MP_QSTR__search_rom & 0xff, MP_QSTR__search_rom >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x3f, 0x29, 0x25, 0x2b, 0x24, 0x27, 0x27, 0x22, 0x24, 0x26, 0x22, 0x26, 0x27, 0x29, 0x24, 0x48, 0x24, 0x36, 0x22, 0x22, 0x28, 0x24, 0x26, 0x2d, 0x2d, 0x00,
    0xb0, 
    0x14, MP_QSTR_reset & 0xff, MP_QSTR_reset >> 8, 
    0x36, 0x00, 
    0x43, 0x05, 0x80, 
    0x51, 
    0x80, 
    0x2a, 0x02, 
    0x63, 
    0xb0, 
    0x14, MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8, 
    0xb0, 
    0x13, MP_QSTR_SEARCH_ROM & 0xff, MP_QSTR_SEARCH_ROM >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb1, 
    0x43, 0x07, 0x80, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x88, 
    0x34, 0x01, 
    0xc1, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x88, 
    0x34, 0x01, 
    0xc3, 
    0x80, 
    0xc4, 
    0x22, 0x80, 0x40, 
    0xc5, 
    0x80, 
    0x42, 0x69, 0x80, 
    0x57, 
    0xc6, 
    0x80, 
    0xc7, 
    0x80, 
    0x42, 0x54, 0x80, 
    0x57, 
    0xc8, 
    0xb0, 
    0x14, MP_QSTR_readbit & 0xff, MP_QSTR_readbit >> 8, 
    0x36, 0x00, 
    0xc9, 
    0xb0, 
    0x14, MP_QSTR_readbit & 0xff, MP_QSTR_readbit >> 8, 
    0x36, 0x00, 
    0x44, 0x0c, 0x80, 
    0xb9, 
    0x44, 0x05, 0x80, 
    0x51, 
    0x80, 
    0x2a, 0x02, 
    0x63, 
    0x42, 0x1e, 0x80, 
    0xb9, 
    0x43, 0x1a, 0x80, 
    0xb2, 
    0xb5, 
    0xd8, 
    0x43, 0x10, 0x80, 
    0xb1, 
    0xb6, 
    0x55, 
    0x81, 
    0xb8, 
    0xf0, 
    0xef, 
    0x44, 0x0a, 0x80, 
    0xb2, 
    0xb5, 
    0xdc, 
    0x44, 0x04, 0x80, 
    0x81, 
    0xc9, 
    0xb5, 
    0xc4, 
    0xb0, 
    0x14, MP_QSTR_writebit & 0xff, MP_QSTR_writebit >> 8, 
    0xb9, 
    0x36, 0x01, 
    0x59, 
    0xb9, 
    0x44, 0x06, 0x80, 
    0xb7, 
    0x81, 
    0xb8, 
    0xf0, 
    0xe0, 
    0xc7, 
    0xb5, 
    0x81, 
    0xe6, 
    0xc5, 
    0x81, 
    0xe5, 
    0x57, 
    0x88, 
    0xd7, 
    0x43, 0xa6, 0x7f, 
    0x59, 
    0xb7, 
    0xb3, 
    0xb6, 
    0x56, 
    0x81, 
    0xe5, 
    0x57, 
    0x88, 
    0xd7, 
    0x43, 0x91, 0x7f, 
    0x59, 
    0xb3, 
    0xb4, 
    0x2a, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire__search_rom[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_l_rom),
    MP_ROM_QSTR(MP_QSTR_diff),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire__search_rom = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 3,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire__search_rom,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire__search_rom,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 203,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 15,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__search_rom,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire__search_rom + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire__search_rom + 33,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire_crc8
STATIC const byte fun_data_onewire__lt_module_gt__OneWire_crc8[19] = {
    0x22, 0x0e,
    MP_QSTR_crc8 & 0xff, MP_QSTR_crc8 >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x80, 0x5b, 0x00,
    0x12, MP_QSTR__ow & 0xff, MP_QSTR__ow >> 8, 
    0x14, MP_QSTR_crc8 & 0xff, MP_QSTR_crc8 >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire_crc8[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_data),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire_crc8 = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire_crc8,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire_crc8,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 19,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_crc8,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire_crc8 + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire_crc8 + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire__lt_module_gt__OneWire
STATIC const byte fun_data_onewire__lt_module_gt__OneWire[126] = {
    0x08, 0x38,
    MP_QSTR_OneWire & 0xff, MP_QSTR_OneWire >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x8c, 0x0b, 0x26, 0x26, 0x46, 0x65, 0x20, 0x69, 0x60, 0x65, 0x65, 0x65, 0x20, 0x65, 0x65, 0x65, 0x20, 0x65, 0x40, 0x85, 0x0c, 0x85, 0x1c, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_OneWire & 0xff, MP_QSTR_OneWire >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x22, 0x81, 0x70, 
    0x16, MP_QSTR_SEARCH_ROM & 0xff, MP_QSTR_SEARCH_ROM >> 8, 
    0x22, 0x80, 0x55, 
    0x16, MP_QSTR_MATCH_ROM & 0xff, MP_QSTR_MATCH_ROM >> 8, 
    0x22, 0x81, 0x4c, 
    0x16, MP_QSTR_SKIP_ROM & 0xff, MP_QSTR_SKIP_ROM >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x50, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x01, 
    0x16, MP_QSTR_reset & 0xff, MP_QSTR_reset >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_readbit & 0xff, MP_QSTR_readbit >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_readbyte & 0xff, MP_QSTR_readbyte >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_writebit & 0xff, MP_QSTR_writebit >> 8, 
    0x32, 0x06, 
    0x16, MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8, 
    0x32, 0x07, 
    0x16, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x32, 0x08, 
    0x16, MP_QSTR_select_rom & 0xff, MP_QSTR_select_rom >> 8, 
    0x32, 0x09, 
    0x16, MP_QSTR_scan & 0xff, MP_QSTR_scan >> 8, 
    0x32, 0x0a, 
    0x16, MP_QSTR__search_rom & 0xff, MP_QSTR__search_rom >> 8, 
    0x32, 0x0b, 
    0x16, MP_QSTR_crc8 & 0xff, MP_QSTR_crc8 >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt__OneWire[12] = {
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire___init__),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_reset),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_readbit),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_readbyte),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_readinto),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_writebit),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_writebyte),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_write),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_select_rom),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_scan),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire__search_rom),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire_crc8),
};
STATIC const mp_raw_code_t raw_code_onewire__lt_module_gt__OneWire = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_onewire__lt_module_gt__OneWire,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt__OneWire,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 126,
    .n_obj = 0,
    .n_raw_code = 12,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_OneWire,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt__OneWire + 6,
        .opcodes = fun_data_onewire__lt_module_gt__OneWire + 30,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file onewire.py, scope onewire_<module>
STATIC const byte fun_data_onewire__lt_module_gt_[46] = {
    0x18, 0x12,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_onewire_dot_py & 0xff, MP_QSTR_onewire_dot_py >> 8,
    0x60, 0x68, 0x6e, 0x20, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR__onewire & 0xff, MP_QSTR__onewire >> 8, 
    0x16, MP_QSTR__ow & 0xff, MP_QSTR__ow >> 8, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_OneWireError & 0xff, MP_QSTR_OneWireError >> 8, 
    0x11, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_OneWireError & 0xff, MP_QSTR_OneWireError >> 8, 
    0x54, 
    0x32, 0x01, 
    0x10, MP_QSTR_OneWire & 0xff, MP_QSTR_OneWire >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_OneWire & 0xff, MP_QSTR_OneWire >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_onewire__lt_module_gt_[2] = {
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWireError),
    MP_ROM_PTR(&raw_code_onewire__lt_module_gt__OneWire),
};
const mp_raw_code_t raw_code_onewire__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_onewire__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_onewire__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 46,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_onewire_dot_py,
        .line_info = fun_data_onewire__lt_module_gt_ + 6,
        .opcodes = fun_data_onewire__lt_module_gt_ + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ds18x20.py, scope ds18x20__lt_module_gt__DS18X20___init__
STATIC const byte fun_data_ds18x20__lt_module_gt__DS18X20___init__[27] = {
    0x1a, 0x10,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_ds18x20_dot_py & 0xff, MP_QSTR_ds18x20_dot_py >> 8,
    0x80, 0x0c, 0x25, 0x00,
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x89, 
    0x34, 0x01, 
    0xb0, 
    0x18, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_ds18x20__lt_module_gt__DS18X20___init__[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_onewire),
};
STATIC const mp_raw_code_t raw_code_ds18x20__lt_module_gt__DS18X20___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_ds18x20__lt_module_gt__DS18X20___init__,
    .const_table = (mp_uint_t*)const_table_data_ds18x20__lt_module_gt__DS18X20___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 27,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_ds18x20_dot_py,
        .line_info = fun_data_ds18x20__lt_module_gt__DS18X20___init__ + 6,
        .opcodes = fun_data_ds18x20__lt_module_gt__DS18X20___init__ + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ds18x20.py, scope ds18x20__lt_module_gt__DS18X20_scan_<listcomp>
STATIC const byte fun_data_ds18x20__lt_module_gt__DS18X20_scan__lt_listcomp_gt_[36] = {
    0x51, 0x0e,
    MP_QSTR__lt_listcomp_gt_ & 0xff, MP_QSTR__lt_listcomp_gt_ >> 8,
    MP_QSTR_ds18x20_dot_py & 0xff, MP_QSTR_ds18x20_dot_py >> 8,
    0x80, 0x10, 0x00,
    0x2b, 0x00, 
    0xb0, 
    0x5f, 
    0x4b, 0x13, 0x00, 
    0xc1, 
    0xb1, 
    0x80, 
    0x55, 
    0x90, 
    0xa2, 
    0xa8, 
    0x2a, 0x03, 
    0xdd, 
    0x44, 0xf0, 0x7f, 
    0xb1, 
    0x2f, 0x14, 
    0x42, 0xea, 0x7f, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_ds18x20__lt_module_gt__DS18X20_scan__lt_listcomp_gt_[1] = {
    MP_ROM_QSTR(MP_QSTR__star_),
};
STATIC const mp_raw_code_t raw_code_ds18x20__lt_module_gt__DS18X20_scan__lt_listcomp_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_ds18x20__lt_module_gt__DS18X20_scan__lt_listcomp_gt_,
    .const_table = (mp_uint_t*)const_table_data_ds18x20__lt_module_gt__DS18X20_scan__lt_listcomp_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 36,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_listcomp_gt_,
        .qstr_source_file = MP_QSTR_ds18x20_dot_py,
        .line_info = fun_data_ds18x20__lt_module_gt__DS18X20_scan__lt_listcomp_gt_ + 6,
        .opcodes = fun_data_ds18x20__lt_module_gt__DS18X20_scan__lt_listcomp_gt_ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ds18x20.py, scope ds18x20__lt_module_gt__DS18X20_scan
STATIC const byte fun_data_ds18x20__lt_module_gt__DS18X20_scan[23] = {
    0x19, 0x0e,
    MP_QSTR_scan & 0xff, MP_QSTR_scan >> 8,
    MP_QSTR_ds18x20_dot_py & 0xff, MP_QSTR_ds18x20_dot_py >> 8,
    0x80, 0x10, 0x00,
    0x32, 0x01, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_scan & 0xff, MP_QSTR_scan >> 8, 
    0x36, 0x00, 
    0x34, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_ds18x20__lt_module_gt__DS18X20_scan[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&raw_code_ds18x20__lt_module_gt__DS18X20_scan__lt_listcomp_gt_),
};
STATIC const mp_raw_code_t raw_code_ds18x20__lt_module_gt__DS18X20_scan = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_ds18x20__lt_module_gt__DS18X20_scan,
    .const_table = (mp_uint_t*)const_table_data_ds18x20__lt_module_gt__DS18X20_scan,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_scan,
        .qstr_source_file = MP_QSTR_ds18x20_dot_py,
        .line_info = fun_data_ds18x20__lt_module_gt__DS18X20_scan + 6,
        .opcodes = fun_data_ds18x20__lt_module_gt__DS18X20_scan + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ds18x20.py, scope ds18x20__lt_module_gt__DS18X20_convert_temp
STATIC const byte fun_data_ds18x20__lt_module_gt__DS18X20_convert_temp[54] = {
    0x19, 0x12,
    MP_QSTR_convert_temp & 0xff, MP_QSTR_convert_temp >> 8,
    MP_QSTR_ds18x20_dot_py & 0xff, MP_QSTR_ds18x20_dot_py >> 8,
    0x80, 0x13, 0x2b, 0x31, 0x00,
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_reset & 0xff, MP_QSTR_reset >> 8, 
    0x52, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x13, MP_QSTR_SKIP_ROM & 0xff, MP_QSTR_SKIP_ROM >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8, 
    0x22, 0x80, 0x44, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_ds18x20__lt_module_gt__DS18X20_convert_temp[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_ds18x20__lt_module_gt__DS18X20_convert_temp = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_ds18x20__lt_module_gt__DS18X20_convert_temp,
    .const_table = (mp_uint_t*)const_table_data_ds18x20__lt_module_gt__DS18X20_convert_temp,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 54,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_convert_temp,
        .qstr_source_file = MP_QSTR_ds18x20_dot_py,
        .line_info = fun_data_ds18x20__lt_module_gt__DS18X20_convert_temp + 6,
        .opcodes = fun_data_ds18x20__lt_module_gt__DS18X20_convert_temp + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ds18x20.py, scope ds18x20__lt_module_gt__DS18X20_read_scratch
STATIC const byte fun_data_ds18x20__lt_module_gt__DS18X20_read_scratch[94] = {
    0x22, 0x1a,
    MP_QSTR_read_scratch & 0xff, MP_QSTR_read_scratch >> 8,
    MP_QSTR_ds18x20_dot_py & 0xff, MP_QSTR_ds18x20_dot_py >> 8,
    0x80, 0x18, 0x2b, 0x2b, 0x2d, 0x2e, 0x30, 0x29, 0x00,
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_reset & 0xff, MP_QSTR_reset >> 8, 
    0x52, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_select_rom & 0xff, MP_QSTR_select_rom >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8, 
    0x22, 0x81, 0x3e, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8, 
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_crc8 & 0xff, MP_QSTR_crc8 >> 8, 
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x36, 0x01, 
    0x44, 0x09, 0x80, 
    0x12, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0x10, MP_QSTR_CRC_space_error & 0xff, MP_QSTR_CRC_space_error >> 8, 
    0x34, 0x01, 
    0x65, 
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_ds18x20__lt_module_gt__DS18X20_read_scratch[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_rom),
};
STATIC const mp_raw_code_t raw_code_ds18x20__lt_module_gt__DS18X20_read_scratch = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_ds18x20__lt_module_gt__DS18X20_read_scratch,
    .const_table = (mp_uint_t*)const_table_data_ds18x20__lt_module_gt__DS18X20_read_scratch,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 94,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_read_scratch,
        .qstr_source_file = MP_QSTR_ds18x20_dot_py,
        .line_info = fun_data_ds18x20__lt_module_gt__DS18X20_read_scratch + 6,
        .opcodes = fun_data_ds18x20__lt_module_gt__DS18X20_read_scratch + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ds18x20.py, scope ds18x20__lt_module_gt__DS18X20_write_scratch
STATIC const byte fun_data_ds18x20__lt_module_gt__DS18X20_write_scratch[60] = {
    0x2b, 0x14,
    MP_QSTR_write_scratch & 0xff, MP_QSTR_write_scratch >> 8,
    MP_QSTR_ds18x20_dot_py & 0xff, MP_QSTR_ds18x20_dot_py >> 8,
    0x80, 0x21, 0x2b, 0x2b, 0x2d, 0x00,
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_reset & 0xff, MP_QSTR_reset >> 8, 
    0x52, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_select_rom & 0xff, MP_QSTR_select_rom >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_writebyte & 0xff, MP_QSTR_writebyte >> 8, 
    0x22, 0x80, 0x4e, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ow & 0xff, MP_QSTR_ow >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb2, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_ds18x20__lt_module_gt__DS18X20_write_scratch[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_rom),
    MP_ROM_QSTR(MP_QSTR_buf),
};
STATIC const mp_raw_code_t raw_code_ds18x20__lt_module_gt__DS18X20_write_scratch = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 3,
    .fun_data = fun_data_ds18x20__lt_module_gt__DS18X20_write_scratch,
    .const_table = (mp_uint_t*)const_table_data_ds18x20__lt_module_gt__DS18X20_write_scratch,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 60,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_write_scratch,
        .qstr_source_file = MP_QSTR_ds18x20_dot_py,
        .line_info = fun_data_ds18x20__lt_module_gt__DS18X20_write_scratch + 6,
        .opcodes = fun_data_ds18x20__lt_module_gt__DS18X20_write_scratch + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ds18x20.py, scope ds18x20__lt_module_gt__DS18X20_read_temp
STATIC const byte fun_data_ds18x20__lt_module_gt__DS18X20_read_temp[122] = {
    0x3a, 0x22,
    MP_QSTR_read_temp & 0xff, MP_QSTR_read_temp >> 8,
    MP_QSTR_ds18x20_dot_py & 0xff, MP_QSTR_ds18x20_dot_py >> 8,
    0x80, 0x27, 0x28, 0x28, 0x26, 0x2a, 0x4d, 0x26, 0x51, 0x2a, 0x29, 0x2a, 0x00,
    0xb0, 
    0x14, MP_QSTR_read_scratch & 0xff, MP_QSTR_read_scratch >> 8, 
    0xb1, 
    0x36, 0x01, 
    0xc2, 
    0xb1, 
    0x80, 
    0x55, 
    0x90, 
    0xd9, 
    0x44, 0x34, 0x80, 
    0xb2, 
    0x81, 
    0x55, 
    0x44, 0x17, 0x80, 
    0xb2, 
    0x80, 
    0x55, 
    0x81, 
    0xf1, 
    0x22, 0x81, 0x00, 
    0xed, 
    0xc3, 
    0xb3, 
    0xd2, 
    0x81, 
    0xf2, 
    0x22, 0x81, 0x7f, 
    0xef, 
    0xd1, 
    0xc3, 
    0x42, 0x06, 0x80, 
    0xb2, 
    0x80, 
    0x55, 
    0x81, 
    0xf1, 
    0xc3, 
    0xb3, 
    0x23, 0x02, 
    0xf3, 
    0xb2, 
    0x87, 
    0x55, 
    0xb2, 
    0x86, 
    0x55, 
    0xf3, 
    0xb2, 
    0x87, 
    0x55, 
    0xf7, 
    0xf2, 
    0x63, 
    0xb2, 
    0x81, 
    0x55, 
    0x88, 
    0xf0, 
    0xb2, 
    0x80, 
    0x55, 
    0xed, 
    0xc3, 
    0xb3, 
    0x22, 0x82, 0x80, 0x00, 
    0xef, 
    0x44, 0x0a, 0x80, 
    0xb3, 
    0x22, 0x83, 0xff, 0x7f, 
    0xee, 
    0x81, 
    0xf2, 
    0xd1, 
    0xc3, 
    0xb3, 
    0x90, 
    0xf7, 
    0x63, 
    0x51, 
    0x63, 
};
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B
STATIC const mp_obj_float_t const_obj_ds18x20__lt_module_gt__DS18X20_read_temp_0 = {{&mp_type_float}, (mp_float_t)0.25};
#endif
STATIC const mp_rom_obj_t const_table_data_ds18x20__lt_module_gt__DS18X20_read_temp[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_rom),
#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_A || MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_B
    MP_ROM_PTR(&const_obj_ds18x20__lt_module_gt__DS18X20_read_temp_0),
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_C
    (mp_rom_obj_t)(0xbf000002),
#elif MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
    (mp_rom_obj_t)(0xbfd4000000000000),
#endif
};
STATIC const mp_raw_code_t raw_code_ds18x20__lt_module_gt__DS18X20_read_temp = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_ds18x20__lt_module_gt__DS18X20_read_temp,
    .const_table = (mp_uint_t*)const_table_data_ds18x20__lt_module_gt__DS18X20_read_temp,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 122,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 8,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_read_temp,
        .qstr_source_file = MP_QSTR_ds18x20_dot_py,
        .line_info = fun_data_ds18x20__lt_module_gt__DS18X20_read_temp + 6,
        .opcodes = fun_data_ds18x20__lt_module_gt__DS18X20_read_temp + 19,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ds18x20.py, scope ds18x20__lt_module_gt__DS18X20
STATIC const byte fun_data_ds18x20__lt_module_gt__DS18X20[62] = {
    0x00, 0x20,
    MP_QSTR_DS18X20 & 0xff, MP_QSTR_DS18X20 >> 8,
    MP_QSTR_ds18x20_dot_py & 0xff, MP_QSTR_ds18x20_dot_py >> 8,
    0x8c, 0x0b, 0x65, 0x20, 0x65, 0x65, 0x40, 0x85, 0x09, 0x65, 0x60, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_DS18X20 & 0xff, MP_QSTR_DS18X20 >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_scan & 0xff, MP_QSTR_scan >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_convert_temp & 0xff, MP_QSTR_convert_temp >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_read_scratch & 0xff, MP_QSTR_read_scratch >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_write_scratch & 0xff, MP_QSTR_write_scratch >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_read_temp & 0xff, MP_QSTR_read_temp >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_ds18x20__lt_module_gt__DS18X20[6] = {
    MP_ROM_PTR(&raw_code_ds18x20__lt_module_gt__DS18X20___init__),
    MP_ROM_PTR(&raw_code_ds18x20__lt_module_gt__DS18X20_scan),
    MP_ROM_PTR(&raw_code_ds18x20__lt_module_gt__DS18X20_convert_temp),
    MP_ROM_PTR(&raw_code_ds18x20__lt_module_gt__DS18X20_read_scratch),
    MP_ROM_PTR(&raw_code_ds18x20__lt_module_gt__DS18X20_write_scratch),
    MP_ROM_PTR(&raw_code_ds18x20__lt_module_gt__DS18X20_read_temp),
};
STATIC const mp_raw_code_t raw_code_ds18x20__lt_module_gt__DS18X20 = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_ds18x20__lt_module_gt__DS18X20,
    .const_table = (mp_uint_t*)const_table_data_ds18x20__lt_module_gt__DS18X20,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 62,
    .n_obj = 0,
    .n_raw_code = 6,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_DS18X20,
        .qstr_source_file = MP_QSTR_ds18x20_dot_py,
        .line_info = fun_data_ds18x20__lt_module_gt__DS18X20 + 6,
        .opcodes = fun_data_ds18x20__lt_module_gt__DS18X20 + 18,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file ds18x20.py, scope ds18x20_<module>
STATIC const byte fun_data_ds18x20__lt_module_gt_[40] = {
    0x10, 0x12,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_ds18x20_dot_py & 0xff, MP_QSTR_ds18x20_dot_py >> 8,
    0x60, 0x70, 0x60, 0x20, 0x00,
    0x80, 
    0x10, MP_QSTR_const & 0xff, MP_QSTR_const >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_micropython & 0xff, MP_QSTR_micropython >> 8, 
    0x1c, MP_QSTR_const & 0xff, MP_QSTR_const >> 8, 
    0x16, MP_QSTR_const & 0xff, MP_QSTR_const >> 8, 
    0x59, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_DS18X20 & 0xff, MP_QSTR_DS18X20 >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_DS18X20 & 0xff, MP_QSTR_DS18X20 >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_ds18x20__lt_module_gt_[1] = {
    MP_ROM_PTR(&raw_code_ds18x20__lt_module_gt__DS18X20),
};
const mp_raw_code_t raw_code_ds18x20__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_ds18x20__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_ds18x20__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 40,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_ds18x20_dot_py,
        .line_info = fun_data_ds18x20__lt_module_gt_ + 6,
        .opcodes = fun_data_ds18x20__lt_module_gt_ + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/__init__.py, scope uasyncio___init____lt_module_gt____getattr__
STATIC const byte fun_data_uasyncio___init____lt_module_gt____getattr__[58] = {
    0x49, 0x0a,
    MP_QSTR___getattr__ & 0xff, MP_QSTR___getattr__ >> 8,
    MP_QSTR_uasyncio_slash___init___dot_py & 0xff, MP_QSTR_uasyncio_slash___init___dot_py >> 8,
    0x00,
    0x12, MP_QSTR__attrs & 0xff, MP_QSTR__attrs >> 8, 
    0x14, MP_QSTR_get & 0xff, MP_QSTR_get >> 8, 
    0xb0, 
    0x51, 
    0x36, 0x02, 
    0xc1, 
    0xb1, 
    0x51, 
    0xde, 
    0x44, 0x07, 0x80, 
    0x12, MP_QSTR_AttributeError & 0xff, MP_QSTR_AttributeError >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x65, 
    0x12, MP_QSTR_getattr & 0xff, MP_QSTR_getattr >> 8, 
    0x12, MP_QSTR___import__ & 0xff, MP_QSTR___import__ >> 8, 
    0xb1, 
    0x51, 
    0x51, 
    0x52, 
    0x81, 
    0x34, 0x05, 
    0xb0, 
    0x34, 0x02, 
    0xc2, 
    0xb2, 
    0x12, MP_QSTR_globals & 0xff, MP_QSTR_globals >> 8, 
    0x34, 0x00, 
    0xb0, 
    0x56, 
    0xb2, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio___init____lt_module_gt____getattr__[1] = {
    MP_ROM_QSTR(MP_QSTR_attr),
};
STATIC const mp_raw_code_t raw_code_uasyncio___init____lt_module_gt____getattr__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio___init____lt_module_gt____getattr__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio___init____lt_module_gt____getattr__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 58,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 10,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___getattr__,
        .qstr_source_file = MP_QSTR_uasyncio_slash___init___dot_py,
        .line_info = fun_data_uasyncio___init____lt_module_gt____getattr__ + 6,
        .opcodes = fun_data_uasyncio___init____lt_module_gt____getattr__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/__init__.py, scope uasyncio___init___<module>
STATIC const byte fun_data_uasyncio___init____lt_module_gt_[101] = {
    0x10, 0x0a,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_uasyncio_slash___init___dot_py & 0xff, MP_QSTR_uasyncio_slash___init___dot_py >> 8,
    0x00,
    0x81, 
    0x10, MP_QSTR__star_ & 0xff, MP_QSTR__star_ >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x69, 
    0x83, 
    0x80, 
    0x80, 
    0x2a, 0x03, 
    0x16, MP_QSTR___version__ & 0xff, MP_QSTR___version__ >> 8, 
    0x2c, 0x0a, 
    0x10, MP_QSTR_funcs & 0xff, MP_QSTR_funcs >> 8, 
    0x10, MP_QSTR_wait_for & 0xff, MP_QSTR_wait_for >> 8, 
    0x62, 
    0x10, MP_QSTR_funcs & 0xff, MP_QSTR_funcs >> 8, 
    0x23, 0x00, 
    0x62, 
    0x10, MP_QSTR_funcs & 0xff, MP_QSTR_funcs >> 8, 
    0x10, MP_QSTR_gather & 0xff, MP_QSTR_gather >> 8, 
    0x62, 
    0x10, MP_QSTR_event & 0xff, MP_QSTR_event >> 8, 
    0x10, MP_QSTR_Event & 0xff, MP_QSTR_Event >> 8, 
    0x62, 
    0x10, MP_QSTR_event & 0xff, MP_QSTR_event >> 8, 
    0x23, 0x01, 
    0x62, 
    0x10, MP_QSTR_lock & 0xff, MP_QSTR_lock >> 8, 
    0x10, MP_QSTR_Lock & 0xff, MP_QSTR_Lock >> 8, 
    0x62, 
    0x10, MP_QSTR_stream & 0xff, MP_QSTR_stream >> 8, 
    0x23, 0x02, 
    0x62, 
    0x10, MP_QSTR_stream & 0xff, MP_QSTR_stream >> 8, 
    0x23, 0x03, 
    0x62, 
    0x10, MP_QSTR_stream & 0xff, MP_QSTR_stream >> 8, 
    0x23, 0x04, 
    0x62, 
    0x10, MP_QSTR_stream & 0xff, MP_QSTR_stream >> 8, 
    0x23, 0x05, 
    0x62, 
    0x16, MP_QSTR__attrs & 0xff, MP_QSTR__attrs >> 8, 
    0x32, 0x06, 
    0x16, MP_QSTR___getattr__ & 0xff, MP_QSTR___getattr__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_uasyncio___init____lt_module_gt__0 = {{&mp_type_str}, 25035, 11, (const byte*)"\x77\x61\x69\x74\x5f\x66\x6f\x72\x5f\x6d\x73"};
STATIC const mp_obj_str_t const_obj_uasyncio___init____lt_module_gt__1 = {{&mp_type_str}, 9558, 14, (const byte*)"\x54\x68\x72\x65\x61\x64\x53\x61\x66\x65\x46\x6c\x61\x67"};
STATIC const mp_obj_str_t const_obj_uasyncio___init____lt_module_gt__2 = {{&mp_type_str}, 25560, 15, (const byte*)"\x6f\x70\x65\x6e\x5f\x63\x6f\x6e\x6e\x65\x63\x74\x69\x6f\x6e"};
STATIC const mp_obj_str_t const_obj_uasyncio___init____lt_module_gt__3 = {{&mp_type_str}, 63679, 12, (const byte*)"\x73\x74\x61\x72\x74\x5f\x73\x65\x72\x76\x65\x72"};
STATIC const mp_obj_str_t const_obj_uasyncio___init____lt_module_gt__4 = {{&mp_type_str}, 39580, 12, (const byte*)"\x53\x74\x72\x65\x61\x6d\x52\x65\x61\x64\x65\x72"};
STATIC const mp_obj_str_t const_obj_uasyncio___init____lt_module_gt__5 = {{&mp_type_str}, 35638, 12, (const byte*)"\x53\x74\x72\x65\x61\x6d\x57\x72\x69\x74\x65\x72"};
STATIC const mp_rom_obj_t const_table_data_uasyncio___init____lt_module_gt_[7] = {
    MP_ROM_PTR(&const_obj_uasyncio___init____lt_module_gt__0),
    MP_ROM_PTR(&const_obj_uasyncio___init____lt_module_gt__1),
    MP_ROM_PTR(&const_obj_uasyncio___init____lt_module_gt__2),
    MP_ROM_PTR(&const_obj_uasyncio___init____lt_module_gt__3),
    MP_ROM_PTR(&const_obj_uasyncio___init____lt_module_gt__4),
    MP_ROM_PTR(&const_obj_uasyncio___init____lt_module_gt__5),
    MP_ROM_PTR(&raw_code_uasyncio___init____lt_module_gt____getattr__),
};
const mp_raw_code_t raw_code_uasyncio___init____lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio___init____lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_uasyncio___init____lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 101,
    .n_obj = 6,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_uasyncio_slash___init___dot_py,
        .line_info = fun_data_uasyncio___init____lt_module_gt_ + 6,
        .opcodes = fun_data_uasyncio___init____lt_module_gt_ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__CancelledError
STATIC const byte fun_data_uasyncio_core__lt_module_gt__CancelledError[21] = {
    0x00, 0x0a,
    MP_QSTR_CancelledError & 0xff, MP_QSTR_CancelledError >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_CancelledError & 0xff, MP_QSTR_CancelledError >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__CancelledError = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__CancelledError,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 21,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_CancelledError,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__CancelledError + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__CancelledError + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__TimeoutError
STATIC const byte fun_data_uasyncio_core__lt_module_gt__TimeoutError[21] = {
    0x00, 0x0a,
    MP_QSTR_TimeoutError & 0xff, MP_QSTR_TimeoutError >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_TimeoutError & 0xff, MP_QSTR_TimeoutError >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__TimeoutError = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__TimeoutError,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 21,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_TimeoutError,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__TimeoutError + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__TimeoutError + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__SingletonGenerator___init__
STATIC const byte fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___init__[23] = {
    0x11, 0x0a,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x51, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x12, MP_QSTR_StopIteration & 0xff, MP_QSTR_StopIteration >> 8, 
    0x34, 0x00, 
    0xb0, 
    0x18, MP_QSTR_exc & 0xff, MP_QSTR_exc >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__SingletonGenerator___init__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__SingletonGenerator___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___init__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__SingletonGenerator___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___init__ + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___init__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__SingletonGenerator___iter__
STATIC const byte fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___iter__[9] = {
    0x09, 0x0a,
    MP_QSTR___iter__ & 0xff, MP_QSTR___iter__ >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0xb0, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__SingletonGenerator___iter__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__SingletonGenerator___iter__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___iter__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__SingletonGenerator___iter__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 9,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___iter__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___iter__ + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___iter__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__SingletonGenerator___next__
STATIC const byte fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___next__[55] = {
    0x21, 0x0a,
    MP_QSTR___next__ & 0xff, MP_QSTR___next__ >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x17, 0x80, 
    0x12, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_push_sorted & 0xff, MP_QSTR_push_sorted >> 8, 
    0x12, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0xb0, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x36, 0x02, 
    0x59, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x51, 
    0x63, 
    0x51, 
    0xb0, 
    0x13, MP_QSTR_exc & 0xff, MP_QSTR_exc >> 8, 
    0x18, MP_QSTR___traceback__ & 0xff, MP_QSTR___traceback__ >> 8, 
    0xb0, 
    0x13, MP_QSTR_exc & 0xff, MP_QSTR_exc >> 8, 
    0x65, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__SingletonGenerator___next__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__SingletonGenerator___next__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___next__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__SingletonGenerator___next__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 55,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___next__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___next__ + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator___next__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__SingletonGenerator
STATIC const byte fun_data_uasyncio_core__lt_module_gt__SingletonGenerator[36] = {
    0x00, 0x0a,
    MP_QSTR_SingletonGenerator & 0xff, MP_QSTR_SingletonGenerator >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_SingletonGenerator & 0xff, MP_QSTR_SingletonGenerator >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR___iter__ & 0xff, MP_QSTR___iter__ >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR___next__ & 0xff, MP_QSTR___next__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__SingletonGenerator[3] = {
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__SingletonGenerator___init__),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__SingletonGenerator___iter__),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__SingletonGenerator___next__),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__SingletonGenerator = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__SingletonGenerator,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 36,
    .n_obj = 0,
    .n_raw_code = 3,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_SingletonGenerator,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__SingletonGenerator + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__sleep_ms
STATIC const byte fun_data_uasyncio_core__lt_module_gt__sleep_ms[31] = {
    0xb2, 0x01, 0x0a,
    MP_QSTR_sleep_ms & 0xff, MP_QSTR_sleep_ms >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_ticks_add & 0xff, MP_QSTR_ticks_add >> 8, 
    0x12, MP_QSTR_ticks & 0xff, MP_QSTR_ticks >> 8, 
    0x34, 0x00, 
    0x12, MP_QSTR_max & 0xff, MP_QSTR_max >> 8, 
    0x80, 
    0xb0, 
    0x34, 0x02, 
    0x34, 0x02, 
    0xb1, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0xb1, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__sleep_ms[2] = {
    MP_ROM_QSTR(MP_QSTR_t),
    MP_ROM_QSTR(MP_QSTR_sgen),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__sleep_ms = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__sleep_ms,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__sleep_ms,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 31,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_sleep_ms,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__sleep_ms + 7,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__sleep_ms + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__sleep
STATIC const byte fun_data_uasyncio_core__lt_module_gt__sleep[23] = {
    0x21, 0x0a,
    MP_QSTR_sleep & 0xff, MP_QSTR_sleep >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_sleep_ms & 0xff, MP_QSTR_sleep_ms >> 8, 
    0x12, MP_QSTR_int & 0xff, MP_QSTR_int >> 8, 
    0xb0, 
    0x22, 0x87, 0x68, 
    0xf4, 
    0x34, 0x01, 
    0x34, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__sleep[1] = {
    MP_ROM_QSTR(MP_QSTR_t),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__sleep = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__sleep,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__sleep,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_sleep,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__sleep + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__sleep + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__IOQueue___init__
STATIC const byte fun_data_uasyncio_core__lt_module_gt__IOQueue___init__[27] = {
    0x11, 0x0a,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x14, MP_QSTR_poll & 0xff, MP_QSTR_poll >> 8, 
    0x36, 0x00, 
    0xb0, 
    0x18, MP_QSTR_poller & 0xff, MP_QSTR_poller >> 8, 
    0x2c, 0x00, 
    0xb0, 
    0x18, MP_QSTR_map & 0xff, MP_QSTR_map >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__IOQueue___init__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__IOQueue___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__IOQueue___init__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__IOQueue___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 27,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__IOQueue___init__ + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__IOQueue___init__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__IOQueue__enqueue
STATIC const byte fun_data_uasyncio_core__lt_module_gt__IOQueue__enqueue[132] = {
    0x4b, 0x0a,
    MP_QSTR__enqueue & 0xff, MP_QSTR__enqueue >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_id & 0xff, MP_QSTR_id >> 8, 
    0xb1, 
    0x34, 0x01, 
    0xb0, 
    0x13, MP_QSTR_map & 0xff, MP_QSTR_map >> 8, 
    0xdd, 
    0xd3, 
    0x44, 0x3b, 0x80, 
    0x51, 
    0x51, 
    0xb1, 
    0x2b, 0x03, 
    0xc3, 
    0x12, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0xb3, 
    0xb2, 
    0x56, 
    0xb3, 
    0xb0, 
    0x13, MP_QSTR_map & 0xff, MP_QSTR_map >> 8, 
    0x12, MP_QSTR_id & 0xff, MP_QSTR_id >> 8, 
    0xb1, 
    0x34, 0x01, 
    0x56, 
    0xb0, 
    0x13, MP_QSTR_poller & 0xff, MP_QSTR_poller >> 8, 
    0x14, MP_QSTR_register & 0xff, MP_QSTR_register >> 8, 
    0xb1, 
    0xb2, 
    0x80, 
    0xd9, 
    0x44, 0x09, 0x80, 
    0x12, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x13, MP_QSTR_POLLIN & 0xff, MP_QSTR_POLLIN >> 8, 
    0x42, 0x06, 0x80, 
    0x12, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x13, MP_QSTR_POLLOUT & 0xff, MP_QSTR_POLLOUT >> 8, 
    0x36, 0x02, 
    0x59, 
    0x42, 0x2a, 0x80, 
    0xb0, 
    0x13, MP_QSTR_map & 0xff, MP_QSTR_map >> 8, 
    0x12, MP_QSTR_id & 0xff, MP_QSTR_id >> 8, 
    0xb1, 
    0x34, 0x01, 
    0x55, 
    0xc4, 
    0x12, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0xb4, 
    0xb2, 
    0x56, 
    0xb0, 
    0x13, MP_QSTR_poller & 0xff, MP_QSTR_poller >> 8, 
    0x14, MP_QSTR_modify & 0xff, MP_QSTR_modify >> 8, 
    0xb1, 
    0x12, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x13, MP_QSTR_POLLIN & 0xff, MP_QSTR_POLLIN >> 8, 
    0x12, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x13, MP_QSTR_POLLOUT & 0xff, MP_QSTR_POLLOUT >> 8, 
    0xed, 
    0x36, 0x02, 
    0x59, 
    0xb0, 
    0x12, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0x18, MP_QSTR_data & 0xff, MP_QSTR_data >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__IOQueue__enqueue[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_s),
    MP_ROM_QSTR(MP_QSTR_idx),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__IOQueue__enqueue = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 3,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__IOQueue__enqueue,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__IOQueue__enqueue,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 132,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 10,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__enqueue,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__IOQueue__enqueue + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__IOQueue__enqueue + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__IOQueue__dequeue
STATIC const byte fun_data_uasyncio_core__lt_module_gt__IOQueue__dequeue[33] = {
    0x22, 0x0a,
    MP_QSTR__dequeue & 0xff, MP_QSTR__dequeue >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_map & 0xff, MP_QSTR_map >> 8, 
    0x12, MP_QSTR_id & 0xff, MP_QSTR_id >> 8, 
    0xb1, 
    0x34, 0x01, 
    0x53, 
    0x5b, 
    0x56, 
    0xb0, 
    0x13, MP_QSTR_poller & 0xff, MP_QSTR_poller >> 8, 
    0x14, MP_QSTR_unregister & 0xff, MP_QSTR_unregister >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__IOQueue__dequeue[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_s),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__IOQueue__dequeue = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__IOQueue__dequeue,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__IOQueue__dequeue,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 33,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__dequeue,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__IOQueue__dequeue + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__IOQueue__dequeue + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__IOQueue_queue_read
STATIC const byte fun_data_uasyncio_core__lt_module_gt__IOQueue_queue_read[18] = {
    0x2a, 0x0a,
    MP_QSTR_queue_read & 0xff, MP_QSTR_queue_read >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0xb0, 
    0x14, MP_QSTR__enqueue & 0xff, MP_QSTR__enqueue >> 8, 
    0xb1, 
    0x80, 
    0x36, 0x02, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__IOQueue_queue_read[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_s),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__IOQueue_queue_read = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__IOQueue_queue_read,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__IOQueue_queue_read,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 18,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_queue_read,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__IOQueue_queue_read + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__IOQueue_queue_read + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__IOQueue_queue_write
STATIC const byte fun_data_uasyncio_core__lt_module_gt__IOQueue_queue_write[18] = {
    0x2a, 0x0a,
    MP_QSTR_queue_write & 0xff, MP_QSTR_queue_write >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0xb0, 
    0x14, MP_QSTR__enqueue & 0xff, MP_QSTR__enqueue >> 8, 
    0xb1, 
    0x81, 
    0x36, 0x02, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__IOQueue_queue_write[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_s),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__IOQueue_queue_write = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__IOQueue_queue_write,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__IOQueue_queue_write,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 18,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_queue_write,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__IOQueue_queue_write + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__IOQueue_queue_write + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__IOQueue_remove
STATIC const byte fun_data_uasyncio_core__lt_module_gt__IOQueue_remove[79] = {
    0x6a, 0x0a,
    MP_QSTR_remove & 0xff, MP_QSTR_remove >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x51, 
    0xc2, 
    0xb0, 
    0x13, MP_QSTR_map & 0xff, MP_QSTR_map >> 8, 
    0x5f, 
    0x4b, 0x24, 0x00, 
    0xc3, 
    0xb0, 
    0x13, MP_QSTR_map & 0xff, MP_QSTR_map >> 8, 
    0xb3, 
    0x55, 
    0x30, 0x03, 
    0xc4, 
    0xc5, 
    0xc6, 
    0xb4, 
    0xb1, 
    0xde, 
    0x43, 0x06, 0x80, 
    0xb5, 
    0xb1, 
    0xde, 
    0x44, 0x09, 0x80, 
    0xb6, 
    0xc2, 
    0x59, 
    0x59, 
    0x59, 
    0x59, 
    0x42, 0x03, 0x80, 
    0x42, 0xd9, 0x7f, 
    0xb2, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x0b, 0x80, 
    0xb0, 
    0x14, MP_QSTR__dequeue & 0xff, MP_QSTR__dequeue >> 8, 
    0xb6, 
    0x36, 0x01, 
    0x59, 
    0x42, 0x03, 0x80, 
    0x42, 0x03, 0x80, 
    0x42, 0xba, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__IOQueue_remove[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_task),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__IOQueue_remove = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__IOQueue_remove,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__IOQueue_remove,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 79,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 14,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_remove,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__IOQueue_remove + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__IOQueue_remove + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__IOQueue_wait_io_event
STATIC const byte fun_data_uasyncio_core__lt_module_gt__IOQueue_wait_io_event[188] = {
    0x62, 0x0a,
    MP_QSTR_wait_io_event & 0xff, MP_QSTR_wait_io_event >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_poller & 0xff, MP_QSTR_poller >> 8, 
    0x14, MP_QSTR_ipoll & 0xff, MP_QSTR_ipoll >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x5f, 
    0x4b, 0xa5, 0x00, 
    0x30, 0x02, 
    0xc2, 
    0xc3, 
    0xb0, 
    0x13, MP_QSTR_map & 0xff, MP_QSTR_map >> 8, 
    0x12, MP_QSTR_id & 0xff, MP_QSTR_id >> 8, 
    0xb2, 
    0x34, 0x01, 
    0x55, 
    0xc4, 
    0xb3, 
    0x12, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x13, MP_QSTR_POLLOUT & 0xff, MP_QSTR_POLLOUT >> 8, 
    0xd2, 
    0xef, 
    0x44, 0x19, 0x80, 
    0xb4, 
    0x80, 
    0x55, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x10, 0x80, 
    0x12, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0xb4, 
    0x80, 
    0x55, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0xb4, 
    0x80, 
    0x56, 
    0xb3, 
    0x12, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x13, MP_QSTR_POLLIN & 0xff, MP_QSTR_POLLIN >> 8, 
    0xd2, 
    0xef, 
    0x44, 0x19, 0x80, 
    0xb4, 
    0x81, 
    0x55, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x10, 0x80, 
    0x12, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0xb4, 
    0x81, 
    0x55, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0xb4, 
    0x81, 
    0x56, 
    0xb4, 
    0x80, 
    0x55, 
    0x51, 
    0xde, 
    0x44, 0x13, 0x80, 
    0xb4, 
    0x81, 
    0x55, 
    0x51, 
    0xde, 
    0x44, 0x0b, 0x80, 
    0xb0, 
    0x14, MP_QSTR__dequeue & 0xff, MP_QSTR__dequeue >> 8, 
    0xb2, 
    0x36, 0x01, 
    0x59, 
    0x42, 0x2d, 0x80, 
    0xb4, 
    0x80, 
    0x55, 
    0x51, 
    0xde, 
    0x44, 0x14, 0x80, 
    0xb0, 
    0x13, MP_QSTR_poller & 0xff, MP_QSTR_poller >> 8, 
    0x14, MP_QSTR_modify & 0xff, MP_QSTR_modify >> 8, 
    0xb2, 
    0x12, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x13, MP_QSTR_POLLOUT & 0xff, MP_QSTR_POLLOUT >> 8, 
    0x36, 0x02, 
    0x59, 
    0x42, 0x11, 0x80, 
    0xb0, 
    0x13, MP_QSTR_poller & 0xff, MP_QSTR_poller >> 8, 
    0x14, MP_QSTR_modify & 0xff, MP_QSTR_modify >> 8, 
    0xb2, 
    0x12, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x13, MP_QSTR_POLLIN & 0xff, MP_QSTR_POLLIN >> 8, 
    0x36, 0x02, 
    0x59, 
    0x42, 0x58, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__IOQueue_wait_io_event[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_dt),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__IOQueue_wait_io_event = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__IOQueue_wait_io_event,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__IOQueue_wait_io_event,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 188,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 13,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_wait_io_event,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__IOQueue_wait_io_event + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__IOQueue_wait_io_event + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__IOQueue
STATIC const byte fun_data_uasyncio_core__lt_module_gt__IOQueue[56] = {
    0x00, 0x0a,
    MP_QSTR_IOQueue & 0xff, MP_QSTR_IOQueue >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_IOQueue & 0xff, MP_QSTR_IOQueue >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR__enqueue & 0xff, MP_QSTR__enqueue >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR__dequeue & 0xff, MP_QSTR__dequeue >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_queue_read & 0xff, MP_QSTR_queue_read >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_queue_write & 0xff, MP_QSTR_queue_write >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_remove & 0xff, MP_QSTR_remove >> 8, 
    0x32, 0x06, 
    0x16, MP_QSTR_wait_io_event & 0xff, MP_QSTR_wait_io_event >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__IOQueue[7] = {
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__IOQueue___init__),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__IOQueue__enqueue),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__IOQueue__dequeue),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__IOQueue_queue_read),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__IOQueue_queue_write),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__IOQueue_remove),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__IOQueue_wait_io_event),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__IOQueue = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__IOQueue,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__IOQueue,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 56,
    .n_obj = 0,
    .n_raw_code = 7,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_IOQueue,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__IOQueue + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__IOQueue + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt___promote_to_task
STATIC const byte fun_data_uasyncio_core__lt_module_gt___promote_to_task[28] = {
    0x19, 0x0a,
    MP_QSTR__promote_to_task & 0xff, MP_QSTR__promote_to_task >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_isinstance & 0xff, MP_QSTR_isinstance >> 8, 
    0xb0, 
    0x12, MP_QSTR_Task & 0xff, MP_QSTR_Task >> 8, 
    0x34, 0x02, 
    0x44, 0x02, 0x80, 
    0xb0, 
    0x63, 
    0x12, MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt___promote_to_task[1] = {
    MP_ROM_QSTR(MP_QSTR_aw),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt___promote_to_task = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt___promote_to_task,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt___promote_to_task,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 28,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__promote_to_task,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt___promote_to_task + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt___promote_to_task + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__create_task
STATIC const byte fun_data_uasyncio_core__lt_module_gt__create_task[51] = {
    0x21, 0x0a,
    MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_hasattr & 0xff, MP_QSTR_hasattr >> 8, 
    0xb0, 
    0x10, MP_QSTR_send & 0xff, MP_QSTR_send >> 8, 
    0x34, 0x02, 
    0x43, 0x08, 0x80, 
    0x12, MP_QSTR_TypeError & 0xff, MP_QSTR_TypeError >> 8, 
    0x23, 0x01, 
    0x34, 0x01, 
    0x65, 
    0x12, MP_QSTR_Task & 0xff, MP_QSTR_Task >> 8, 
    0xb0, 
    0x12, MP_QSTR_globals & 0xff, MP_QSTR_globals >> 8, 
    0x34, 0x00, 
    0x34, 0x02, 
    0xc1, 
    0x12, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0xb1, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_uasyncio_core__lt_module_gt__create_task_0 = {{&mp_type_str}, 50473, 18, (const byte*)"\x63\x6f\x72\x6f\x75\x74\x69\x6e\x65\x20\x65\x78\x70\x65\x63\x74\x65\x64"};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__create_task[2] = {
    MP_ROM_QSTR(MP_QSTR_coro),
    MP_ROM_PTR(&const_obj_uasyncio_core__lt_module_gt__create_task_0),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__create_task = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__create_task,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__create_task,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 51,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_create_task,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__create_task + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__create_task + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__run_until_complete
STATIC const byte fun_data_uasyncio_core__lt_module_gt__run_until_complete[343] = {
    0xf1, 0x03, 0x0a,
    MP_QSTR_run_until_complete & 0xff, MP_QSTR_run_until_complete >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_CancelledError & 0xff, MP_QSTR_CancelledError >> 8, 
    0x12, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0x2a, 0x02, 
    0xc1, 
    0x12, MP_QSTR_CancelledError & 0xff, MP_QSTR_CancelledError >> 8, 
    0x12, MP_QSTR_StopIteration & 0xff, MP_QSTR_StopIteration >> 8, 
    0x2a, 0x02, 
    0xc2, 
    0x81, 
    0xc3, 
    0x42, 0x3c, 0x80, 
    0x7f, 
    0xc3, 
    0x12, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_peek & 0xff, MP_QSTR_peek >> 8, 
    0x36, 0x00, 
    0xc4, 
    0xb4, 
    0x44, 0x18, 0x80, 
    0x12, MP_QSTR_max & 0xff, MP_QSTR_max >> 8, 
    0x80, 
    0x12, MP_QSTR_ticks_diff & 0xff, MP_QSTR_ticks_diff >> 8, 
    0xb4, 
    0x13, MP_QSTR_ph_key & 0xff, MP_QSTR_ph_key >> 8, 
    0x12, MP_QSTR_ticks & 0xff, MP_QSTR_ticks >> 8, 
    0x34, 0x00, 
    0x34, 0x02, 
    0x34, 0x02, 
    0xc3, 
    0x42, 0x0b, 0x80, 
    0x12, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x13, MP_QSTR_map & 0xff, MP_QSTR_map >> 8, 
    0x43, 0x02, 0x80, 
    0x51, 
    0x63, 
    0x12, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x14, MP_QSTR_wait_io_event & 0xff, MP_QSTR_wait_io_event >> 8, 
    0xb3, 
    0x36, 0x01, 
    0x59, 
    0xb3, 
    0x80, 
    0xd8, 
    0x43, 0xbe, 0x7f, 
    0x12, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_pop_head & 0xff, MP_QSTR_pop_head >> 8, 
    0x36, 0x00, 
    0xc4, 
    0xb4, 
    0x17, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0x48, 0x2a, 0x00, 
    0xb4, 
    0x13, MP_QSTR_data & 0xff, MP_QSTR_data >> 8, 
    0xc5, 
    0xb5, 
    0x43, 0x0e, 0x80, 
    0xb4, 
    0x13, MP_QSTR_coro & 0xff, MP_QSTR_coro >> 8, 
    0x14, MP_QSTR_send & 0xff, MP_QSTR_send >> 8, 
    0x51, 
    0x36, 0x01, 
    0x59, 
    0x42, 0x10, 0x80, 
    0x51, 
    0xb4, 
    0x18, MP_QSTR_data & 0xff, MP_QSTR_data >> 8, 
    0xb4, 
    0x13, MP_QSTR_coro & 0xff, MP_QSTR_coro >> 8, 
    0x14, MP_QSTR_throw & 0xff, MP_QSTR_throw >> 8, 
    0xb5, 
    0x36, 0x01, 
    0x59, 
    0x4a, 0xb7, 0x00, 
    0x57, 
    0xb1, 
    0xdf, 
    0x44, 0xb0, 0x80, 
    0xc6, 
    0x49, 0xa4, 0x00, 
    0xb4, 
    0xb0, 
    0xde, 
    0x44, 0x13, 0x80, 
    0x12, MP_QSTR_isinstance & 0xff, MP_QSTR_isinstance >> 8, 
    0xb6, 
    0x12, MP_QSTR_StopIteration & 0xff, MP_QSTR_StopIteration >> 8, 
    0x34, 0x02, 
    0x44, 0x05, 0x80, 
    0xb6, 
    0x13, MP_QSTR_value & 0xff, MP_QSTR_value >> 8, 
    0x63, 
    0xb6, 
    0x65, 
    0xb4, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x44, 0x5b, 0x80, 
    0x50, 
    0xc7, 
    0xb4, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x52, 
    0xde, 
    0x44, 0x08, 0x80, 
    0x51, 
    0xb4, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x42, 0x28, 0x80, 
    0x42, 0x14, 0x80, 
    0x12, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0xb4, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x14, MP_QSTR_pop_head & 0xff, MP_QSTR_pop_head >> 8, 
    0x36, 0x00, 
    0x36, 0x01, 
    0x59, 
    0x52, 
    0xc7, 
    0xb4, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x14, MP_QSTR_peek & 0xff, MP_QSTR_peek >> 8, 
    0x36, 0x00, 
    0x43, 0xe0, 0x7f, 
    0x50, 
    0xb4, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0xb7, 
    0x43, 0x14, 0x80, 
    0x12, MP_QSTR_isinstance & 0xff, MP_QSTR_isinstance >> 8, 
    0xb6, 
    0xb2, 
    0x34, 0x02, 
    0x43, 0x0a, 0x80, 
    0x12, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0xb4, 
    0x36, 0x01, 
    0x59, 
    0xb6, 
    0xb4, 
    0x18, MP_QSTR_data & 0xff, MP_QSTR_data >> 8, 
    0x42, 0x28, 0x80, 
    0xb4, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x51, 
    0xde, 
    0x44, 0x1f, 0x80, 
    0xb5, 
    0x12, MP_QSTR__exc_context & 0xff, MP_QSTR__exc_context >> 8, 
    0x10, MP_QSTR_exception & 0xff, MP_QSTR_exception >> 8, 
    0x56, 
    0xb4, 
    0x12, MP_QSTR__exc_context & 0xff, MP_QSTR__exc_context >> 8, 
    0x10, MP_QSTR_future & 0xff, MP_QSTR_future >> 8, 
    0x56, 
    0x12, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x14, MP_QSTR_call_exception_handler & 0xff, MP_QSTR_call_exception_handler >> 8, 
    0x12, MP_QSTR__exc_context & 0xff, MP_QSTR__exc_context >> 8, 
    0x36, 0x01, 
    0x59, 
    0x42, 0x00, 0x80, 
    0x51, 
    0x51, 
    0xc6, 
    0x28, 0x06, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x42, 0xc5, 0x7e, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__run_until_complete[1] = {
    MP_ROM_QSTR(MP_QSTR_main_task),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__run_until_complete = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__run_until_complete,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__run_until_complete,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 343,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 15,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_run_until_complete,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__run_until_complete + 7,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__run_until_complete + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__run
STATIC const byte fun_data_uasyncio_core__lt_module_gt__run[19] = {
    0x19, 0x0a,
    MP_QSTR_run & 0xff, MP_QSTR_run >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_run_until_complete & 0xff, MP_QSTR_run_until_complete >> 8, 
    0x12, MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x34, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__run[1] = {
    MP_ROM_QSTR(MP_QSTR_coro),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__run = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__run,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__run,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 19,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_run,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__run + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__run + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt___stopper
STATIC const byte fun_data_uasyncio_core__lt_module_gt___stopper[10] = {
    0x80, 0x40, 0x0a,
    MP_QSTR__stopper & 0xff, MP_QSTR__stopper >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt___stopper = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt___stopper,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 10,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__stopper,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt___stopper + 7,
        .opcodes = fun_data_uasyncio_core__lt_module_gt___stopper + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop_create_task
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop_create_task[14] = {
    0x11, 0x0a,
    MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__Loop_create_task[1] = {
    MP_ROM_QSTR(MP_QSTR_coro),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop_create_task = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop_create_task,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__Loop_create_task,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 14,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_create_task,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop_create_task + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop_create_task + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop_run_forever
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop_run_forever[36] = {
    0x10, 0x0a,
    MP_QSTR_run_forever & 0xff, MP_QSTR_run_forever >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_Task & 0xff, MP_QSTR_Task >> 8, 
    0x12, MP_QSTR__stopper & 0xff, MP_QSTR__stopper >> 8, 
    0x34, 0x00, 
    0x12, MP_QSTR_globals & 0xff, MP_QSTR_globals >> 8, 
    0x34, 0x00, 
    0x34, 0x02, 
    0x17, MP_QSTR__stop_task & 0xff, MP_QSTR__stop_task >> 8, 
    0x12, MP_QSTR_run_until_complete & 0xff, MP_QSTR_run_until_complete >> 8, 
    0x12, MP_QSTR__stop_task & 0xff, MP_QSTR__stop_task >> 8, 
    0x34, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop_run_forever = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop_run_forever,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 36,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_run_forever,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop_run_forever + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop_run_forever + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop_run_until_complete
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop_run_until_complete[19] = {
    0x19, 0x0a,
    MP_QSTR_run_until_complete & 0xff, MP_QSTR_run_until_complete >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_run_until_complete & 0xff, MP_QSTR_run_until_complete >> 8, 
    0x12, MP_QSTR__promote_to_task & 0xff, MP_QSTR__promote_to_task >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x34, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__Loop_run_until_complete[1] = {
    MP_ROM_QSTR(MP_QSTR_aw),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop_run_until_complete = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop_run_until_complete,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__Loop_run_until_complete,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 19,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_run_until_complete,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop_run_until_complete + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop_run_until_complete + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop_stop
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop_stop[34] = {
    0x10, 0x0a,
    MP_QSTR_stop & 0xff, MP_QSTR_stop >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR__stop_task & 0xff, MP_QSTR__stop_task >> 8, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x10, 0x80, 
    0x12, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0x12, MP_QSTR__stop_task & 0xff, MP_QSTR__stop_task >> 8, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x17, MP_QSTR__stop_task & 0xff, MP_QSTR__stop_task >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop_stop = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop_stop,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 34,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_stop,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop_stop + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop_stop + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop_close
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop_close[9] = {
    0x00, 0x0a,
    MP_QSTR_close & 0xff, MP_QSTR_close >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop_close = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop_close,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 9,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_close,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop_close + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop_close + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop_set_exception_handler
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop_set_exception_handler[16] = {
    0x11, 0x0a,
    MP_QSTR_set_exception_handler & 0xff, MP_QSTR_set_exception_handler >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0xb0, 
    0x12, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x18, MP_QSTR__exc_handler & 0xff, MP_QSTR__exc_handler >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__Loop_set_exception_handler[1] = {
    MP_ROM_QSTR(MP_QSTR_handler),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop_set_exception_handler = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop_set_exception_handler,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__Loop_set_exception_handler,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 16,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_set_exception_handler,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop_set_exception_handler + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop_set_exception_handler + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop_get_exception_handler
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop_get_exception_handler[14] = {
    0x00, 0x0a,
    MP_QSTR_get_exception_handler & 0xff, MP_QSTR_get_exception_handler >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x13, MP_QSTR__exc_handler & 0xff, MP_QSTR__exc_handler >> 8, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop_get_exception_handler = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop_get_exception_handler,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 14,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_get_exception_handler,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop_get_exception_handler + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop_get_exception_handler + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop_default_exception_handler
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop_default_exception_handler[59] = {
    0x3a, 0x0a,
    MP_QSTR_default_exception_handler & 0xff, MP_QSTR_default_exception_handler >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0xb1, 
    0x10, MP_QSTR_message & 0xff, MP_QSTR_message >> 8, 
    0x55, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x10, MP_QSTR_future_colon_ & 0xff, MP_QSTR_future_colon_ >> 8, 
    0xb1, 
    0x10, MP_QSTR_future & 0xff, MP_QSTR_future >> 8, 
    0x55, 
    0x10, MP_QSTR_coro_equals_ & 0xff, MP_QSTR_coro_equals_ >> 8, 
    0xb1, 
    0x10, MP_QSTR_future & 0xff, MP_QSTR_future >> 8, 
    0x55, 
    0x13, MP_QSTR_coro & 0xff, MP_QSTR_coro >> 8, 
    0x34, 0x04, 
    0x59, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x14, MP_QSTR_print_exception & 0xff, MP_QSTR_print_exception >> 8, 
    0xb1, 
    0x10, MP_QSTR_exception & 0xff, MP_QSTR_exception >> 8, 
    0x55, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__Loop_default_exception_handler[2] = {
    MP_ROM_QSTR(MP_QSTR_loop),
    MP_ROM_QSTR(MP_QSTR_context),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop_default_exception_handler = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop_default_exception_handler,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__Loop_default_exception_handler,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 59,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 8,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_default_exception_handler,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop_default_exception_handler + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop_default_exception_handler + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop_call_exception_handler
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop_call_exception_handler[31] = {
    0x19, 0x0a,
    MP_QSTR_call_exception_handler & 0xff, MP_QSTR_call_exception_handler >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x13, MP_QSTR__exc_handler & 0xff, MP_QSTR__exc_handler >> 8, 
    0x45, 0x06, 0x80, 
    0x12, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x13, MP_QSTR_default_exception_handler & 0xff, MP_QSTR_default_exception_handler >> 8, 
    0x12, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0xb0, 
    0x34, 0x02, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__Loop_call_exception_handler[1] = {
    MP_ROM_QSTR(MP_QSTR_context),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop_call_exception_handler = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop_call_exception_handler,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__Loop_call_exception_handler,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 31,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_call_exception_handler,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop_call_exception_handler + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop_call_exception_handler + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__Loop
STATIC const byte fun_data_uasyncio_core__lt_module_gt__Loop[70] = {
    0x00, 0x0a,
    MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x51, 
    0x16, MP_QSTR__exc_handler & 0xff, MP_QSTR__exc_handler >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_run_forever & 0xff, MP_QSTR_run_forever >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_run_until_complete & 0xff, MP_QSTR_run_until_complete >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_stop & 0xff, MP_QSTR_stop >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_set_exception_handler & 0xff, MP_QSTR_set_exception_handler >> 8, 
    0x32, 0x06, 
    0x16, MP_QSTR_get_exception_handler & 0xff, MP_QSTR_get_exception_handler >> 8, 
    0x32, 0x07, 
    0x16, MP_QSTR_default_exception_handler & 0xff, MP_QSTR_default_exception_handler >> 8, 
    0x32, 0x08, 
    0x16, MP_QSTR_call_exception_handler & 0xff, MP_QSTR_call_exception_handler >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__Loop[9] = {
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop_create_task),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop_run_forever),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop_run_until_complete),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop_stop),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop_close),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop_set_exception_handler),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop_get_exception_handler),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop_default_exception_handler),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop_call_exception_handler),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__Loop = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__Loop,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__Loop,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 70,
    .n_obj = 0,
    .n_raw_code = 9,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_Loop,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__Loop + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__Loop + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__get_event_loop
STATIC const byte fun_data_uasyncio_core__lt_module_gt__get_event_loop[13] = {
    0x92, 0x80, 0x01, 0x0a,
    MP_QSTR_get_event_loop & 0xff, MP_QSTR_get_event_loop >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt__get_event_loop[2] = {
    MP_ROM_QSTR(MP_QSTR_runq_len),
    MP_ROM_QSTR(MP_QSTR_waitq_len),
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__get_event_loop = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__get_event_loop,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt__get_event_loop,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 13,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 2,
        .qstr_block_name = MP_QSTR_get_event_loop,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__get_event_loop + 8,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__get_event_loop + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__current_task
STATIC const byte fun_data_uasyncio_core__lt_module_gt__current_task[11] = {
    0x00, 0x0a,
    MP_QSTR_current_task & 0xff, MP_QSTR_current_task >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__current_task = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__current_task,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 11,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_current_task,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__current_task + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__current_task + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core__lt_module_gt__new_event_loop
STATIC const byte fun_data_uasyncio_core__lt_module_gt__new_event_loop[27] = {
    0x00, 0x0a,
    MP_QSTR_new_event_loop & 0xff, MP_QSTR_new_event_loop >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_TaskQueue & 0xff, MP_QSTR_TaskQueue >> 8, 
    0x34, 0x00, 
    0x17, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x12, MP_QSTR_IOQueue & 0xff, MP_QSTR_IOQueue >> 8, 
    0x34, 0x00, 
    0x17, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x12, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt__new_event_loop = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt__new_event_loop,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 27,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_new_event_loop,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt__new_event_loop + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt__new_event_loop + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/core.py, scope uasyncio_core_<module>
STATIC const byte fun_data_uasyncio_core__lt_module_gt_[279] = {
    0x2c, 0x0a,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_uasyncio_slash_core_dot_py & 0xff, MP_QSTR_uasyncio_slash_core_dot_py >> 8,
    0x00,
    0x80, 
    0x10, MP_QSTR_ticks_ms & 0xff, MP_QSTR_ticks_ms >> 8, 
    0x10, MP_QSTR_ticks_diff & 0xff, MP_QSTR_ticks_diff >> 8, 
    0x10, MP_QSTR_ticks_add & 0xff, MP_QSTR_ticks_add >> 8, 
    0x2a, 0x03, 
    0x1b, MP_QSTR_time & 0xff, MP_QSTR_time >> 8, 
    0x1c, MP_QSTR_ticks_ms & 0xff, MP_QSTR_ticks_ms >> 8, 
    0x16, MP_QSTR_ticks & 0xff, MP_QSTR_ticks >> 8, 
    0x1c, MP_QSTR_ticks_diff & 0xff, MP_QSTR_ticks_diff >> 8, 
    0x16, MP_QSTR_ticks_diff & 0xff, MP_QSTR_ticks_diff >> 8, 
    0x1c, MP_QSTR_ticks_add & 0xff, MP_QSTR_ticks_add >> 8, 
    0x16, MP_QSTR_ticks_add & 0xff, MP_QSTR_ticks_add >> 8, 
    0x59, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x16, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x16, MP_QSTR_select & 0xff, MP_QSTR_select >> 8, 
    0x48, 0x1c, 0x00, 
    0x80, 
    0x10, MP_QSTR_TaskQueue & 0xff, MP_QSTR_TaskQueue >> 8, 
    0x10, MP_QSTR_Task & 0xff, MP_QSTR_Task >> 8, 
    0x2a, 0x02, 
    0x1b, MP_QSTR__uasyncio & 0xff, MP_QSTR__uasyncio >> 8, 
    0x1c, MP_QSTR_TaskQueue & 0xff, MP_QSTR_TaskQueue >> 8, 
    0x16, MP_QSTR_TaskQueue & 0xff, MP_QSTR_TaskQueue >> 8, 
    0x1c, MP_QSTR_Task & 0xff, MP_QSTR_Task >> 8, 
    0x16, MP_QSTR_Task & 0xff, MP_QSTR_Task >> 8, 
    0x59, 
    0x4a, 0x1e, 0x00, 
    0x59, 
    0x81, 
    0x10, MP_QSTR_TaskQueue & 0xff, MP_QSTR_TaskQueue >> 8, 
    0x10, MP_QSTR_Task & 0xff, MP_QSTR_Task >> 8, 
    0x2a, 0x02, 
    0x1b, MP_QSTR_task & 0xff, MP_QSTR_task >> 8, 
    0x1c, MP_QSTR_TaskQueue & 0xff, MP_QSTR_TaskQueue >> 8, 
    0x16, MP_QSTR_TaskQueue & 0xff, MP_QSTR_TaskQueue >> 8, 
    0x1c, MP_QSTR_Task & 0xff, MP_QSTR_Task >> 8, 
    0x16, MP_QSTR_Task & 0xff, MP_QSTR_Task >> 8, 
    0x59, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x54, 
    0x32, 0x01, 
    0x10, MP_QSTR_CancelledError & 0xff, MP_QSTR_CancelledError >> 8, 
    0x11, MP_QSTR_BaseException & 0xff, MP_QSTR_BaseException >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_CancelledError & 0xff, MP_QSTR_CancelledError >> 8, 
    0x54, 
    0x32, 0x02, 
    0x10, MP_QSTR_TimeoutError & 0xff, MP_QSTR_TimeoutError >> 8, 
    0x11, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_TimeoutError & 0xff, MP_QSTR_TimeoutError >> 8, 
    0x2c, 0x03, 
    0x23, 0x00, 
    0x10, MP_QSTR_message & 0xff, MP_QSTR_message >> 8, 
    0x62, 
    0x51, 
    0x10, MP_QSTR_exception & 0xff, MP_QSTR_exception >> 8, 
    0x62, 
    0x51, 
    0x10, MP_QSTR_future & 0xff, MP_QSTR_future >> 8, 
    0x62, 
    0x16, MP_QSTR__exc_context & 0xff, MP_QSTR__exc_context >> 8, 
    0x54, 
    0x32, 0x03, 
    0x10, MP_QSTR_SingletonGenerator & 0xff, MP_QSTR_SingletonGenerator >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_SingletonGenerator & 0xff, MP_QSTR_SingletonGenerator >> 8, 
    0x11, MP_QSTR_SingletonGenerator & 0xff, MP_QSTR_SingletonGenerator >> 8, 
    0x34, 0x00, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x04, 
    0x16, MP_QSTR_sleep_ms & 0xff, MP_QSTR_sleep_ms >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_sleep & 0xff, MP_QSTR_sleep >> 8, 
    0x54, 
    0x32, 0x06, 
    0x10, MP_QSTR_IOQueue & 0xff, MP_QSTR_IOQueue >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_IOQueue & 0xff, MP_QSTR_IOQueue >> 8, 
    0x32, 0x07, 
    0x16, MP_QSTR__promote_to_task & 0xff, MP_QSTR__promote_to_task >> 8, 
    0x32, 0x08, 
    0x16, MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8, 
    0x51, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x09, 
    0x16, MP_QSTR_run_until_complete & 0xff, MP_QSTR_run_until_complete >> 8, 
    0x32, 0x0a, 
    0x16, MP_QSTR_run & 0xff, MP_QSTR_run >> 8, 
    0x32, 0x0b, 
    0x16, MP_QSTR__stopper & 0xff, MP_QSTR__stopper >> 8, 
    0x51, 
    0x17, MP_QSTR__stop_task & 0xff, MP_QSTR__stop_task >> 8, 
    0x54, 
    0x32, 0x0c, 
    0x10, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_Loop & 0xff, MP_QSTR_Loop >> 8, 
    0x80, 
    0x80, 
    0x2a, 0x02, 
    0x53, 
    0x33, 0x0d, 
    0x16, MP_QSTR_get_event_loop & 0xff, MP_QSTR_get_event_loop >> 8, 
    0x32, 0x0e, 
    0x16, MP_QSTR_current_task & 0xff, MP_QSTR_current_task >> 8, 
    0x32, 0x0f, 
    0x16, MP_QSTR_new_event_loop & 0xff, MP_QSTR_new_event_loop >> 8, 
    0x11, MP_QSTR_new_event_loop & 0xff, MP_QSTR_new_event_loop >> 8, 
    0x34, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_uasyncio_core__lt_module_gt__0 = {{&mp_type_str}, 64973, 31, (const byte*)"\x54\x61\x73\x6b\x20\x65\x78\x63\x65\x70\x74\x69\x6f\x6e\x20\x77\x61\x73\x6e\x27\x74\x20\x72\x65\x74\x72\x69\x65\x76\x65\x64"};
STATIC const mp_rom_obj_t const_table_data_uasyncio_core__lt_module_gt_[16] = {
    MP_ROM_PTR(&const_obj_uasyncio_core__lt_module_gt__0),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__CancelledError),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__TimeoutError),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__SingletonGenerator),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__sleep_ms),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__sleep),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__IOQueue),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt___promote_to_task),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__create_task),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__run_until_complete),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__run),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt___stopper),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__Loop),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__get_event_loop),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__current_task),
    MP_ROM_PTR(&raw_code_uasyncio_core__lt_module_gt__new_event_loop),
};
const mp_raw_code_t raw_code_uasyncio_core__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_core__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_core__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 279,
    .n_obj = 1,
    .n_raw_code = 15,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_uasyncio_slash_core_dot_py,
        .line_info = fun_data_uasyncio_core__lt_module_gt_ + 6,
        .opcodes = fun_data_uasyncio_core__lt_module_gt_ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__Event___init__
STATIC const byte fun_data_uasyncio_event__lt_module_gt__Event___init__[26] = {
    0x11, 0x0a,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0x50, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x14, MP_QSTR_TaskQueue & 0xff, MP_QSTR_TaskQueue >> 8, 
    0x36, 0x00, 
    0xb0, 
    0x18, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__Event___init__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__Event___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__Event___init__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__Event___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 26,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__Event___init__ + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__Event___init__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__Event_is_set
STATIC const byte fun_data_uasyncio_event__lt_module_gt__Event_is_set[12] = {
    0x09, 0x0a,
    MP_QSTR_is_set & 0xff, MP_QSTR_is_set >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__Event_is_set[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__Event_is_set = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__Event_is_set,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__Event_is_set,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 12,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_is_set,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__Event_is_set + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__Event_is_set + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__Event_set
STATIC const byte fun_data_uasyncio_event__lt_module_gt__Event_set[50] = {
    0x21, 0x0a,
    MP_QSTR_set & 0xff, MP_QSTR_set >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0x42, 0x15, 0x80, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0xb0, 
    0x13, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x14, MP_QSTR_pop_head & 0xff, MP_QSTR_pop_head >> 8, 
    0x36, 0x00, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x14, MP_QSTR_peek & 0xff, MP_QSTR_peek >> 8, 
    0x36, 0x00, 
    0x43, 0xdf, 0x7f, 
    0x52, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__Event_set[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__Event_set = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__Event_set,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__Event_set,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 50,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_set,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__Event_set + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__Event_set + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__Event_clear
STATIC const byte fun_data_uasyncio_event__lt_module_gt__Event_clear[14] = {
    0x11, 0x0a,
    MP_QSTR_clear & 0xff, MP_QSTR_clear >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0x50, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__Event_clear[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__Event_clear = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__Event_clear,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__Event_clear,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 14,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_clear,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__Event_clear + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__Event_clear + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__Event_wait
STATIC const byte fun_data_uasyncio_event__lt_module_gt__Event_wait[49] = {
    0x99, 0x40, 0x0a,
    MP_QSTR_wait & 0xff, MP_QSTR_wait >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x43, 0x20, 0x80, 
    0xb0, 
    0x13, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0x18, MP_QSTR_data & 0xff, MP_QSTR_data >> 8, 
    0x51, 
    0x67, 
    0x59, 
    0x52, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__Event_wait[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__Event_wait = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__Event_wait,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__Event_wait,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 49,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_wait,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__Event_wait + 7,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__Event_wait + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__Event
STATIC const byte fun_data_uasyncio_event__lt_module_gt__Event[46] = {
    0x00, 0x0a,
    MP_QSTR_Event & 0xff, MP_QSTR_Event >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_Event & 0xff, MP_QSTR_Event >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_is_set & 0xff, MP_QSTR_is_set >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_set & 0xff, MP_QSTR_set >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_clear & 0xff, MP_QSTR_clear >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_wait & 0xff, MP_QSTR_wait >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__Event[5] = {
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__Event___init__),
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__Event_is_set),
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__Event_set),
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__Event_clear),
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__Event_wait),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__Event = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__Event,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__Event,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 46,
    .n_obj = 0,
    .n_raw_code = 5,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_Event,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__Event + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__Event + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__ThreadSafeFlag___init__
STATIC const byte fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag___init__[14] = {
    0x11, 0x0a,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0x80, 
    0xb0, 
    0x18, MP_QSTR__flag & 0xff, MP_QSTR__flag >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag___init__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag___init__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 14,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag___init__ + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag___init__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__ThreadSafeFlag_ioctl
STATIC const byte fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_ioctl[22] = {
    0x23, 0x0a,
    MP_QSTR_ioctl & 0xff, MP_QSTR_ioctl >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0xb1, 
    0x83, 
    0xd9, 
    0x44, 0x07, 0x80, 
    0xb0, 
    0x13, MP_QSTR__flag & 0xff, MP_QSTR__flag >> 8, 
    0xb2, 
    0xf4, 
    0x63, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_ioctl[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_req),
    MP_ROM_QSTR(MP_QSTR_flags),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag_ioctl = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 3,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_ioctl,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_ioctl,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 22,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_ioctl,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_ioctl + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_ioctl + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__ThreadSafeFlag_set
STATIC const byte fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_set[14] = {
    0x11, 0x0a,
    MP_QSTR_set & 0xff, MP_QSTR_set >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0x81, 
    0xb0, 
    0x18, MP_QSTR__flag & 0xff, MP_QSTR__flag >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_set[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag_set = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_set,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_set,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 14,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_set,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_set + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_set + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__ThreadSafeFlag_wait
STATIC const byte fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_wait[36] = {
    0x99, 0x40, 0x0a,
    MP_QSTR_wait & 0xff, MP_QSTR_wait >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR__flag & 0xff, MP_QSTR__flag >> 8, 
    0x43, 0x0e, 0x80, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x14, MP_QSTR_queue_read & 0xff, MP_QSTR_queue_read >> 8, 
    0xb0, 
    0x36, 0x01, 
    0x67, 
    0x59, 
    0x80, 
    0xb0, 
    0x18, MP_QSTR__flag & 0xff, MP_QSTR__flag >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_wait[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag_wait = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_wait,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_wait,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 36,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_wait,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_wait + 7,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag_wait + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event__lt_module_gt__ThreadSafeFlag
STATIC const byte fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag[41] = {
    0x00, 0x0a,
    MP_QSTR_ThreadSafeFlag & 0xff, MP_QSTR_ThreadSafeFlag >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_ThreadSafeFlag & 0xff, MP_QSTR_ThreadSafeFlag >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_ioctl & 0xff, MP_QSTR_ioctl >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_set & 0xff, MP_QSTR_set >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_wait & 0xff, MP_QSTR_wait >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag[4] = {
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag___init__),
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag_ioctl),
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag_set),
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag_wait),
};
STATIC const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt__ThreadSafeFlag,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 41,
    .n_obj = 0,
    .n_raw_code = 4,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_ThreadSafeFlag,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt__ThreadSafeFlag + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/event.py, scope uasyncio_event_<module>
STATIC const byte fun_data_uasyncio_event__lt_module_gt_[80] = {
    0x2c, 0x0a,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_uasyncio_slash_event_dot_py & 0xff, MP_QSTR_uasyncio_slash_event_dot_py >> 8,
    0x00,
    0x81, 
    0x10, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x1c, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x16, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x59, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_Event & 0xff, MP_QSTR_Event >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_Event & 0xff, MP_QSTR_Event >> 8, 
    0x48, 0x1c, 0x00, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uio & 0xff, MP_QSTR_uio >> 8, 
    0x16, MP_QSTR_uio & 0xff, MP_QSTR_uio >> 8, 
    0x54, 
    0x32, 0x01, 
    0x10, MP_QSTR_ThreadSafeFlag & 0xff, MP_QSTR_ThreadSafeFlag >> 8, 
    0x11, MP_QSTR_uio & 0xff, MP_QSTR_uio >> 8, 
    0x13, MP_QSTR_IOBase & 0xff, MP_QSTR_IOBase >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_ThreadSafeFlag & 0xff, MP_QSTR_ThreadSafeFlag >> 8, 
    0x4a, 0x0d, 0x00, 
    0x57, 
    0x11, MP_QSTR_ImportError & 0xff, MP_QSTR_ImportError >> 8, 
    0xdf, 
    0x44, 0x04, 0x80, 
    0x59, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_event__lt_module_gt_[2] = {
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__Event),
    MP_ROM_PTR(&raw_code_uasyncio_event__lt_module_gt__ThreadSafeFlag),
};
const mp_raw_code_t raw_code_uasyncio_event__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_event__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_event__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 80,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_uasyncio_slash_event_dot_py,
        .line_info = fun_data_uasyncio_event__lt_module_gt_ + 6,
        .opcodes = fun_data_uasyncio_event__lt_module_gt_ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/funcs.py, scope uasyncio_funcs__lt_module_gt__wait_for_runner
STATIC const byte fun_data_uasyncio_funcs__lt_module_gt__wait_for_runner[65] = {
    0xd8, 0x46, 0x0a,
    MP_QSTR_runner & 0xff, MP_QSTR_runner >> 8,
    MP_QSTR_uasyncio_slash_funcs_dot_py & 0xff, MP_QSTR_uasyncio_slash_funcs_dot_py >> 8,
    0x00,
    0x48, 0x0b, 0x00, 
    0xb3, 
    0x5e, 
    0x51, 
    0x68, 
    0x27, 0x01, 
    0x52, 
    0xc4, 
    0x4a, 0x18, 0x00, 
    0x57, 
    0x12, MP_QSTR_BaseException & 0xff, MP_QSTR_BaseException >> 8, 
    0xdf, 
    0x44, 0x0f, 0x80, 
    0xc5, 
    0x49, 0x03, 0x00, 
    0xb5, 
    0xc4, 
    0x51, 
    0x51, 
    0xc5, 
    0x28, 0x05, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x25, 0x00, 
    0x51, 
    0xde, 
    0x44, 0x0a, 0x80, 
    0xb4, 
    0x27, 0x00, 
    0xb2, 
    0x14, MP_QSTR_cancel & 0xff, MP_QSTR_cancel >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_funcs__lt_module_gt__wait_for_runner[4] = {
    MP_ROM_QSTR(MP_QSTR__star_),
    MP_ROM_QSTR(MP_QSTR__star_),
    MP_ROM_QSTR(MP_QSTR_waiter),
    MP_ROM_QSTR(MP_QSTR_aw),
};
STATIC const mp_raw_code_t raw_code_uasyncio_funcs__lt_module_gt__wait_for_runner = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 4,
    .fun_data = fun_data_uasyncio_funcs__lt_module_gt__wait_for_runner,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_funcs__lt_module_gt__wait_for_runner,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 65,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 12,
        .n_exc_stack = 2,
        .scope_flags = 1,
        .n_pos_args = 4,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_runner,
        .qstr_source_file = MP_QSTR_uasyncio_slash_funcs_dot_py,
        .line_info = fun_data_uasyncio_funcs__lt_module_gt__wait_for_runner + 7,
        .opcodes = fun_data_uasyncio_funcs__lt_module_gt__wait_for_runner + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/funcs.py, scope uasyncio_funcs__lt_module_gt__wait_for
STATIC const byte fun_data_uasyncio_funcs__lt_module_gt__wait_for[161] = {
    0xeb, 0x43, 0x8a, 0x01,
    MP_QSTR_wait_for & 0xff, MP_QSTR_wait_for >> 8,
    MP_QSTR_uasyncio_slash_funcs_dot_py & 0xff, MP_QSTR_uasyncio_slash_funcs_dot_py >> 8,
    0x00, 0x06, 0x07,
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x14, MP_QSTR__promote_to_task & 0xff, MP_QSTR__promote_to_task >> 8, 
    0xb0, 
    0x36, 0x01, 
    0xc0, 
    0xb1, 
    0x51, 
    0xde, 
    0x44, 0x05, 0x80, 
    0xb0, 
    0x5e, 
    0x51, 
    0x68, 
    0x63, 
    0xb6, 
    0xb7, 
    0x20, 0x03, 0x02, 
    0xc3, 
    0x51, 
    0x27, 0x06, 
    0x51, 
    0x27, 0x07, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x14, MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8, 
    0xb3, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0xb0, 
    0x34, 0x02, 
    0x36, 0x01, 
    0xc4, 
    0x48, 0x0b, 0x00, 
    0xb2, 
    0xb1, 
    0x34, 0x01, 
    0x5e, 
    0x51, 
    0x68, 
    0x59, 
    0x4a, 0x3c, 0x00, 
    0x57, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_CancelledError & 0xff, MP_QSTR_CancelledError >> 8, 
    0xdf, 
    0x44, 0x30, 0x80, 
    0xc5, 
    0x49, 0x24, 0x00, 
    0x25, 0x06, 
    0x52, 
    0xde, 
    0x44, 0x03, 0x80, 
    0x25, 0x07, 
    0x63, 
    0x25, 0x06, 
    0x51, 
    0xde, 
    0x44, 0x0f, 0x80, 
    0x52, 
    0x27, 0x06, 
    0xb4, 
    0x14, MP_QSTR_cancel & 0xff, MP_QSTR_cancel >> 8, 
    0x36, 0x00, 
    0x59, 
    0xb5, 
    0x65, 
    0x42, 0x03, 0x80, 
    0x25, 0x06, 
    0x65, 
    0x51, 
    0x51, 
    0xc5, 
    0x28, 0x05, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x52, 
    0x27, 0x06, 
    0xb4, 
    0x14, MP_QSTR_cancel & 0xff, MP_QSTR_cancel >> 8, 
    0x36, 0x00, 
    0x59, 
    0xb4, 
    0x5e, 
    0x51, 
    0x68, 
    0x59, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_TimeoutError & 0xff, MP_QSTR_TimeoutError >> 8, 
    0x65, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_funcs__lt_module_gt__wait_for[4] = {
    MP_ROM_QSTR(MP_QSTR_aw),
    MP_ROM_QSTR(MP_QSTR_timeout),
    MP_ROM_QSTR(MP_QSTR_sleep),
    MP_ROM_PTR(&raw_code_uasyncio_funcs__lt_module_gt__wait_for_runner),
};
STATIC const mp_raw_code_t raw_code_uasyncio_funcs__lt_module_gt__wait_for = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 3,
    .fun_data = fun_data_uasyncio_funcs__lt_module_gt__wait_for,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_funcs__lt_module_gt__wait_for,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 161,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 14,
        .n_exc_stack = 2,
        .scope_flags = 1,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_wait_for,
        .qstr_source_file = MP_QSTR_uasyncio_slash_funcs_dot_py,
        .line_info = fun_data_uasyncio_funcs__lt_module_gt__wait_for + 8,
        .opcodes = fun_data_uasyncio_funcs__lt_module_gt__wait_for + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/funcs.py, scope uasyncio_funcs__lt_module_gt__wait_for_ms
STATIC const byte fun_data_uasyncio_funcs__lt_module_gt__wait_for_ms[21] = {
    0x2a, 0x0a,
    MP_QSTR_wait_for_ms & 0xff, MP_QSTR_wait_for_ms >> 8,
    MP_QSTR_uasyncio_slash_funcs_dot_py & 0xff, MP_QSTR_uasyncio_slash_funcs_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_wait_for & 0xff, MP_QSTR_wait_for >> 8, 
    0xb0, 
    0xb1, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_sleep_ms & 0xff, MP_QSTR_sleep_ms >> 8, 
    0x34, 0x03, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_funcs__lt_module_gt__wait_for_ms[2] = {
    MP_ROM_QSTR(MP_QSTR_aw),
    MP_ROM_QSTR(MP_QSTR_timeout),
};
STATIC const mp_raw_code_t raw_code_uasyncio_funcs__lt_module_gt__wait_for_ms = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_funcs__lt_module_gt__wait_for_ms,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_funcs__lt_module_gt__wait_for_ms,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 21,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_wait_for_ms,
        .qstr_source_file = MP_QSTR_uasyncio_slash_funcs_dot_py,
        .line_info = fun_data_uasyncio_funcs__lt_module_gt__wait_for_ms + 6,
        .opcodes = fun_data_uasyncio_funcs__lt_module_gt__wait_for_ms + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/funcs.py, scope uasyncio_funcs__lt_module_gt__gather_<listcomp>
STATIC const byte fun_data_uasyncio_funcs__lt_module_gt__gather__lt_listcomp_gt_[30] = {
    0x49, 0x0a,
    MP_QSTR__lt_listcomp_gt_ & 0xff, MP_QSTR__lt_listcomp_gt_ >> 8,
    MP_QSTR_uasyncio_slash_funcs_dot_py & 0xff, MP_QSTR_uasyncio_slash_funcs_dot_py >> 8,
    0x00,
    0x2b, 0x00, 
    0xb0, 
    0x5f, 
    0x4b, 0x0f, 0x00, 
    0xc1, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x14, MP_QSTR__promote_to_task & 0xff, MP_QSTR__promote_to_task >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x2f, 0x14, 
    0x42, 0xee, 0x7f, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_funcs__lt_module_gt__gather__lt_listcomp_gt_[1] = {
    MP_ROM_QSTR(MP_QSTR__star_),
};
STATIC const mp_raw_code_t raw_code_uasyncio_funcs__lt_module_gt__gather__lt_listcomp_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_funcs__lt_module_gt__gather__lt_listcomp_gt_,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_funcs__lt_module_gt__gather__lt_listcomp_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 30,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 10,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_listcomp_gt_,
        .qstr_source_file = MP_QSTR_uasyncio_slash_funcs_dot_py,
        .line_info = fun_data_uasyncio_funcs__lt_module_gt__gather__lt_listcomp_gt_ + 6,
        .opcodes = fun_data_uasyncio_funcs__lt_module_gt__gather__lt_listcomp_gt_ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/funcs.py, scope uasyncio_funcs__lt_module_gt__gather
STATIC const byte fun_data_uasyncio_funcs__lt_module_gt__gather[91] = {
    0xe0, 0xca, 0x80, 0xc0, 0x40, 0x0a,
    MP_QSTR_gather & 0xff, MP_QSTR_gather >> 8,
    MP_QSTR_uasyncio_slash_funcs_dot_py & 0xff, MP_QSTR_uasyncio_slash_funcs_dot_py >> 8,
    0x00,
    0x32, 0x01, 
    0xb1, 
    0x34, 0x01, 
    0xc2, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb2, 
    0x34, 0x01, 
    0x80, 
    0x42, 0x36, 0x80, 
    0x57, 
    0xc3, 
    0x48, 0x0c, 0x00, 
    0xb2, 
    0xb3, 
    0x55, 
    0x5e, 
    0x51, 
    0x68, 
    0xb2, 
    0xb3, 
    0x56, 
    0x4a, 0x23, 0x00, 
    0x57, 
    0x12, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0xdf, 
    0x44, 0x1a, 0x80, 
    0xc4, 
    0x49, 0x0e, 0x00, 
    0xb0, 
    0x44, 0x07, 0x80, 
    0xb4, 
    0xb2, 
    0xb3, 
    0x56, 
    0x42, 0x02, 0x80, 
    0xb4, 
    0x65, 
    0x51, 
    0x51, 
    0xc4, 
    0x28, 0x04, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x81, 
    0xe5, 
    0x58, 
    0x5a, 
    0xd7, 
    0x43, 0xc4, 0x7f, 
    0x59, 
    0x59, 
    0xb2, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_funcs__lt_module_gt__gather[2] = {
    MP_ROM_QSTR(MP_QSTR_return_exceptions),
    MP_ROM_PTR(&raw_code_uasyncio_funcs__lt_module_gt__gather__lt_listcomp_gt_),
};
STATIC const mp_raw_code_t raw_code_uasyncio_funcs__lt_module_gt__gather = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x0d,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_funcs__lt_module_gt__gather,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_funcs__lt_module_gt__gather,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 91,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 13,
        .n_exc_stack = 2,
        .scope_flags = 13,
        .n_pos_args = 0,
        .n_kwonly_args = 1,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_gather,
        .qstr_source_file = MP_QSTR_uasyncio_slash_funcs_dot_py,
        .line_info = fun_data_uasyncio_funcs__lt_module_gt__gather + 10,
        .opcodes = fun_data_uasyncio_funcs__lt_module_gt__gather + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/funcs.py, scope uasyncio_funcs_<module>
STATIC const byte fun_data_uasyncio_funcs__lt_module_gt_[57] = {
    0x18, 0x0a,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_uasyncio_slash_funcs_dot_py & 0xff, MP_QSTR_uasyncio_slash_funcs_dot_py >> 8,
    0x00,
    0x81, 
    0x10, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x1c, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x16, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x59, 
    0x11, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_sleep & 0xff, MP_QSTR_sleep >> 8, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x00, 
    0x16, MP_QSTR_wait_for & 0xff, MP_QSTR_wait_for >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_wait_for_ms & 0xff, MP_QSTR_wait_for_ms >> 8, 
    0x53, 
    0x2c, 0x00, 
    0x50, 
    0x10, MP_QSTR_return_exceptions & 0xff, MP_QSTR_return_exceptions >> 8, 
    0x62, 
    0x33, 0x02, 
    0x16, MP_QSTR_gather & 0xff, MP_QSTR_gather >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_funcs__lt_module_gt_[3] = {
    MP_ROM_PTR(&raw_code_uasyncio_funcs__lt_module_gt__wait_for),
    MP_ROM_PTR(&raw_code_uasyncio_funcs__lt_module_gt__wait_for_ms),
    MP_ROM_PTR(&raw_code_uasyncio_funcs__lt_module_gt__gather),
};
const mp_raw_code_t raw_code_uasyncio_funcs__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_funcs__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_funcs__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 57,
    .n_obj = 0,
    .n_raw_code = 3,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_uasyncio_slash_funcs_dot_py,
        .line_info = fun_data_uasyncio_funcs__lt_module_gt_ + 6,
        .opcodes = fun_data_uasyncio_funcs__lt_module_gt_ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/lock.py, scope uasyncio_lock__lt_module_gt__Lock___init__
STATIC const byte fun_data_uasyncio_lock__lt_module_gt__Lock___init__[26] = {
    0x11, 0x0a,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_uasyncio_slash_lock_dot_py & 0xff, MP_QSTR_uasyncio_slash_lock_dot_py >> 8,
    0x00,
    0x80, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x14, MP_QSTR_TaskQueue & 0xff, MP_QSTR_TaskQueue >> 8, 
    0x36, 0x00, 
    0xb0, 
    0x18, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_lock__lt_module_gt__Lock___init__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_lock__lt_module_gt__Lock___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_lock__lt_module_gt__Lock___init__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_lock__lt_module_gt__Lock___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 26,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_lock_dot_py,
        .line_info = fun_data_uasyncio_lock__lt_module_gt__Lock___init__ + 6,
        .opcodes = fun_data_uasyncio_lock__lt_module_gt__Lock___init__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/lock.py, scope uasyncio_lock__lt_module_gt__Lock_locked
STATIC const byte fun_data_uasyncio_lock__lt_module_gt__Lock_locked[14] = {
    0x11, 0x0a,
    MP_QSTR_locked & 0xff, MP_QSTR_locked >> 8,
    MP_QSTR_uasyncio_slash_lock_dot_py & 0xff, MP_QSTR_uasyncio_slash_lock_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x81, 
    0xd9, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_lock__lt_module_gt__Lock_locked[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_lock__lt_module_gt__Lock_locked = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_lock__lt_module_gt__Lock_locked,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_lock__lt_module_gt__Lock_locked,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 14,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_locked,
        .qstr_source_file = MP_QSTR_uasyncio_slash_lock_dot_py,
        .line_info = fun_data_uasyncio_lock__lt_module_gt__Lock_locked + 6,
        .opcodes = fun_data_uasyncio_lock__lt_module_gt__Lock_locked + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/lock.py, scope uasyncio_lock__lt_module_gt__Lock_release
STATIC const byte fun_data_uasyncio_lock__lt_module_gt__Lock_release[75] = {
    0x19, 0x0a,
    MP_QSTR_release & 0xff, MP_QSTR_release >> 8,
    MP_QSTR_uasyncio_slash_lock_dot_py & 0xff, MP_QSTR_uasyncio_slash_lock_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x81, 
    0xdc, 
    0x44, 0x08, 0x80, 
    0x12, MP_QSTR_RuntimeError & 0xff, MP_QSTR_RuntimeError >> 8, 
    0x23, 0x01, 
    0x34, 0x01, 
    0x65, 
    0xb0, 
    0x13, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x14, MP_QSTR_peek & 0xff, MP_QSTR_peek >> 8, 
    0x36, 0x00, 
    0x44, 0x20, 0x80, 
    0xb0, 
    0x13, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x14, MP_QSTR_pop_head & 0xff, MP_QSTR_pop_head >> 8, 
    0x36, 0x00, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__task_queue & 0xff, MP_QSTR__task_queue >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0xb0, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x36, 0x01, 
    0x59, 
    0x42, 0x05, 0x80, 
    0x80, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_uasyncio_lock__lt_module_gt__Lock_release_0 = {{&mp_type_str}, 43943, 17, (const byte*)"\x4c\x6f\x63\x6b\x20\x6e\x6f\x74\x20\x61\x63\x71\x75\x69\x72\x65\x64"};
STATIC const mp_rom_obj_t const_table_data_uasyncio_lock__lt_module_gt__Lock_release[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&const_obj_uasyncio_lock__lt_module_gt__Lock_release_0),
};
STATIC const mp_raw_code_t raw_code_uasyncio_lock__lt_module_gt__Lock_release = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_lock__lt_module_gt__Lock_release,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_lock__lt_module_gt__Lock_release,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 75,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_release,
        .qstr_source_file = MP_QSTR_uasyncio_slash_lock_dot_py,
        .line_info = fun_data_uasyncio_lock__lt_module_gt__Lock_release + 6,
        .opcodes = fun_data_uasyncio_lock__lt_module_gt__Lock_release + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/lock.py, scope uasyncio_lock__lt_module_gt__Lock_acquire
STATIC const byte fun_data_uasyncio_lock__lt_module_gt__Lock_acquire[115] = {
    0xb9, 0x42, 0x0a,
    MP_QSTR_acquire & 0xff, MP_QSTR_acquire >> 8,
    MP_QSTR_uasyncio_slash_lock_dot_py & 0xff, MP_QSTR_uasyncio_slash_lock_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x80, 
    0xdc, 
    0x44, 0x5b, 0x80, 
    0xb0, 
    0x13, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x14, MP_QSTR_push_head & 0xff, MP_QSTR_push_head >> 8, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_waiting & 0xff, MP_QSTR_waiting >> 8, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0x18, MP_QSTR_data & 0xff, MP_QSTR_data >> 8, 
    0x48, 0x06, 0x00, 
    0x51, 
    0x67, 
    0x59, 
    0x4a, 0x35, 0x00, 
    0x57, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_CancelledError & 0xff, MP_QSTR_CancelledError >> 8, 
    0xdf, 
    0x44, 0x29, 0x80, 
    0xc1, 
    0x49, 0x1d, 0x00, 
    0xb0, 
    0x13, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_cur_task & 0xff, MP_QSTR_cur_task >> 8, 
    0xd9, 
    0x44, 0x0c, 0x80, 
    0x81, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0xb0, 
    0x14, MP_QSTR_release & 0xff, MP_QSTR_release >> 8, 
    0x36, 0x00, 
    0x59, 
    0xb1, 
    0x65, 
    0x51, 
    0x51, 
    0xc1, 
    0x28, 0x01, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x81, 
    0xb0, 
    0x18, MP_QSTR_state & 0xff, MP_QSTR_state >> 8, 
    0x52, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_lock__lt_module_gt__Lock_acquire[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_lock__lt_module_gt__Lock_acquire = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_lock__lt_module_gt__Lock_acquire,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_lock__lt_module_gt__Lock_acquire,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 115,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 8,
        .n_exc_stack = 2,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_acquire,
        .qstr_source_file = MP_QSTR_uasyncio_slash_lock_dot_py,
        .line_info = fun_data_uasyncio_lock__lt_module_gt__Lock_acquire + 7,
        .opcodes = fun_data_uasyncio_lock__lt_module_gt__Lock_acquire + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/lock.py, scope uasyncio_lock__lt_module_gt__Lock___aenter__
STATIC const byte fun_data_uasyncio_lock__lt_module_gt__Lock___aenter__[18] = {
    0x91, 0x40, 0x0a,
    MP_QSTR___aenter__ & 0xff, MP_QSTR___aenter__ >> 8,
    MP_QSTR_uasyncio_slash_lock_dot_py & 0xff, MP_QSTR_uasyncio_slash_lock_dot_py >> 8,
    0x00,
    0xb0, 
    0x14, MP_QSTR_acquire & 0xff, MP_QSTR_acquire >> 8, 
    0x36, 0x00, 
    0x5e, 
    0x51, 
    0x68, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_lock__lt_module_gt__Lock___aenter__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_lock__lt_module_gt__Lock___aenter__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_lock__lt_module_gt__Lock___aenter__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_lock__lt_module_gt__Lock___aenter__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 18,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___aenter__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_lock_dot_py,
        .line_info = fun_data_uasyncio_lock__lt_module_gt__Lock___aenter__ + 7,
        .opcodes = fun_data_uasyncio_lock__lt_module_gt__Lock___aenter__ + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/lock.py, scope uasyncio_lock__lt_module_gt__Lock___aexit__
STATIC const byte fun_data_uasyncio_lock__lt_module_gt__Lock___aexit__[15] = {
    0xa8, 0x44, 0x0a,
    MP_QSTR___aexit__ & 0xff, MP_QSTR___aexit__ >> 8,
    MP_QSTR_uasyncio_slash_lock_dot_py & 0xff, MP_QSTR_uasyncio_slash_lock_dot_py >> 8,
    0x00,
    0xb0, 
    0x14, MP_QSTR_release & 0xff, MP_QSTR_release >> 8, 
    0x36, 0x00, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_lock__lt_module_gt__Lock___aexit__[4] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_exc_type),
    MP_ROM_QSTR(MP_QSTR_exc),
    MP_ROM_QSTR(MP_QSTR_tb),
};
STATIC const mp_raw_code_t raw_code_uasyncio_lock__lt_module_gt__Lock___aexit__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 4,
    .fun_data = fun_data_uasyncio_lock__lt_module_gt__Lock___aexit__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_lock__lt_module_gt__Lock___aexit__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 15,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 4,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___aexit__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_lock_dot_py,
        .line_info = fun_data_uasyncio_lock__lt_module_gt__Lock___aexit__ + 7,
        .opcodes = fun_data_uasyncio_lock__lt_module_gt__Lock___aexit__ + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/lock.py, scope uasyncio_lock__lt_module_gt__Lock
STATIC const byte fun_data_uasyncio_lock__lt_module_gt__Lock[51] = {
    0x00, 0x0a,
    MP_QSTR_Lock & 0xff, MP_QSTR_Lock >> 8,
    MP_QSTR_uasyncio_slash_lock_dot_py & 0xff, MP_QSTR_uasyncio_slash_lock_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_Lock & 0xff, MP_QSTR_Lock >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_locked & 0xff, MP_QSTR_locked >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_release & 0xff, MP_QSTR_release >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_acquire & 0xff, MP_QSTR_acquire >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR___aenter__ & 0xff, MP_QSTR___aenter__ >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR___aexit__ & 0xff, MP_QSTR___aexit__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_lock__lt_module_gt__Lock[6] = {
    MP_ROM_PTR(&raw_code_uasyncio_lock__lt_module_gt__Lock___init__),
    MP_ROM_PTR(&raw_code_uasyncio_lock__lt_module_gt__Lock_locked),
    MP_ROM_PTR(&raw_code_uasyncio_lock__lt_module_gt__Lock_release),
    MP_ROM_PTR(&raw_code_uasyncio_lock__lt_module_gt__Lock_acquire),
    MP_ROM_PTR(&raw_code_uasyncio_lock__lt_module_gt__Lock___aenter__),
    MP_ROM_PTR(&raw_code_uasyncio_lock__lt_module_gt__Lock___aexit__),
};
STATIC const mp_raw_code_t raw_code_uasyncio_lock__lt_module_gt__Lock = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_lock__lt_module_gt__Lock,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_lock__lt_module_gt__Lock,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 51,
    .n_obj = 0,
    .n_raw_code = 6,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_Lock,
        .qstr_source_file = MP_QSTR_uasyncio_slash_lock_dot_py,
        .line_info = fun_data_uasyncio_lock__lt_module_gt__Lock + 6,
        .opcodes = fun_data_uasyncio_lock__lt_module_gt__Lock + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/lock.py, scope uasyncio_lock_<module>
STATIC const byte fun_data_uasyncio_lock__lt_module_gt_[36] = {
    0x10, 0x0a,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_uasyncio_slash_lock_dot_py & 0xff, MP_QSTR_uasyncio_slash_lock_dot_py >> 8,
    0x00,
    0x81, 
    0x10, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x1c, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x16, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x59, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_Lock & 0xff, MP_QSTR_Lock >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_Lock & 0xff, MP_QSTR_Lock >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_lock__lt_module_gt_[1] = {
    MP_ROM_PTR(&raw_code_uasyncio_lock__lt_module_gt__Lock),
};
const mp_raw_code_t raw_code_uasyncio_lock__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_lock__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_lock__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 36,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_uasyncio_slash_lock_dot_py,
        .line_info = fun_data_uasyncio_lock__lt_module_gt_ + 6,
        .opcodes = fun_data_uasyncio_lock__lt_module_gt_ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream___init__
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream___init__[26] = {
    0xa3, 0x01, 0x0a,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0xb2, 
    0xb0, 
    0x18, MP_QSTR_e & 0xff, MP_QSTR_e >> 8, 
    0x23, 0x03, 
    0xb0, 
    0x18, MP_QSTR_out_buf & 0xff, MP_QSTR_out_buf >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_uasyncio_stream__lt_module_gt__Stream___init___0 = {{&mp_type_bytes}, 5381, 0, (const byte*)""};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream___init__[4] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_s),
    MP_ROM_QSTR(MP_QSTR_e),
    MP_ROM_PTR(&const_obj_uasyncio_stream__lt_module_gt__Stream___init___0),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 3,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream___init__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 26,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream___init__ + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream___init__ + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream_get_extra_info
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream_get_extra_info[14] = {
    0x1a, 0x0a,
    MP_QSTR_get_extra_info & 0xff, MP_QSTR_get_extra_info >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_e & 0xff, MP_QSTR_e >> 8, 
    0xb1, 
    0x55, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream_get_extra_info[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_v),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream_get_extra_info = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream_get_extra_info,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream_get_extra_info,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 14,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_get_extra_info,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream_get_extra_info + 6,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream_get_extra_info + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream___aenter__
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream___aenter__[10] = {
    0x89, 0x40, 0x0a,
    MP_QSTR___aenter__ & 0xff, MP_QSTR___aenter__ >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb0, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream___aenter__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream___aenter__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream___aenter__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream___aenter__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 10,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___aenter__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream___aenter__ + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream___aenter__ + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream___aexit__
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream___aexit__[20] = {
    0xa8, 0x44, 0x0a,
    MP_QSTR___aexit__ & 0xff, MP_QSTR___aexit__ >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb0, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x5e, 
    0x51, 
    0x68, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream___aexit__[4] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_exc_type),
    MP_ROM_QSTR(MP_QSTR_exc),
    MP_ROM_QSTR(MP_QSTR_tb),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream___aexit__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 4,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream___aexit__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream___aexit__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 20,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 4,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___aexit__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream___aexit__ + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream___aexit__ + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream_close
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream_close[9] = {
    0x09, 0x0a,
    MP_QSTR_close & 0xff, MP_QSTR_close >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream_close[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream_close = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream_close,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream_close,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 9,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_close,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream_close + 6,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream_close + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream_wait_closed
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream_wait_closed[20] = {
    0x91, 0x40, 0x0a,
    MP_QSTR_wait_closed & 0xff, MP_QSTR_wait_closed >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream_wait_closed[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream_wait_closed = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream_wait_closed,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream_wait_closed,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 20,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_wait_closed,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream_wait_closed + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream_wait_closed + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream_read
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream_read[36] = {
    0xa2, 0x40, 0x0a,
    MP_QSTR_read & 0xff, MP_QSTR_read >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x14, MP_QSTR_queue_read & 0xff, MP_QSTR_queue_read >> 8, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x36, 0x01, 
    0x67, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream_read[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_n),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream_read = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream_read,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream_read,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 36,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_read,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream_read + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream_read + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream_readinto
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream_readinto[36] = {
    0xa2, 0x40, 0x0a,
    MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x14, MP_QSTR_queue_read & 0xff, MP_QSTR_queue_read >> 8, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x36, 0x01, 
    0x67, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x14, MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream_readinto[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_buf),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream_readinto = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream_readinto,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream_readinto,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 36,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_readinto,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream_readinto + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream_readinto + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream_readexactly
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream_readexactly[81] = {
    0xb2, 0x40, 0x0a,
    MP_QSTR_readexactly & 0xff, MP_QSTR_readexactly >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x23, 0x02, 
    0xc2, 
    0x42, 0x3d, 0x80, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x14, MP_QSTR_queue_read & 0xff, MP_QSTR_queue_read >> 8, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x36, 0x01, 
    0x67, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0xb1, 
    0x36, 0x01, 
    0xc3, 
    0xb3, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x1a, 0x80, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb3, 
    0x34, 0x01, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_EOFError & 0xff, MP_QSTR_EOFError >> 8, 
    0x65, 
    0xb2, 
    0xb3, 
    0xe5, 
    0xc2, 
    0xb1, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb3, 
    0x34, 0x01, 
    0xe6, 
    0xc1, 
    0xb1, 
    0x43, 0xbf, 0x7f, 
    0xb2, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_uasyncio_stream__lt_module_gt__Stream_readexactly_0 = {{&mp_type_bytes}, 5381, 0, (const byte*)""};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream_readexactly[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_n),
    MP_ROM_PTR(&const_obj_uasyncio_stream__lt_module_gt__Stream_readexactly_0),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream_readexactly = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream_readexactly,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream_readexactly,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 81,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_readexactly,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream_readexactly + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream_readexactly + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream_readline
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream_readline[61] = {
    0xa9, 0x40, 0x0a,
    MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x23, 0x01, 
    0xc1, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x14, MP_QSTR_queue_read & 0xff, MP_QSTR_queue_read >> 8, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x36, 0x01, 
    0x67, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0xc2, 
    0xb1, 
    0xb2, 
    0xe5, 
    0xc1, 
    0xb2, 
    0x44, 0x08, 0x80, 
    0xb1, 
    0x7f, 
    0x55, 
    0x8a, 
    0xd9, 
    0x44, 0x02, 0x80, 
    0xb1, 
    0x63, 
    0x42, 0xd0, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_uasyncio_stream__lt_module_gt__Stream_readline_0 = {{&mp_type_bytes}, 5381, 0, (const byte*)""};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream_readline[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&const_obj_uasyncio_stream__lt_module_gt__Stream_readline_0),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream_readline = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream_readline,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream_readline,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 61,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_readline,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream_readline + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream_readline + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream_write
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream_write[20] = {
    0x22, 0x0a,
    MP_QSTR_write & 0xff, MP_QSTR_write >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb0, 
    0x57, 
    0x13, MP_QSTR_out_buf & 0xff, MP_QSTR_out_buf >> 8, 
    0xb1, 
    0xe5, 
    0x5a, 
    0x18, MP_QSTR_out_buf & 0xff, MP_QSTR_out_buf >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream_write[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_buf),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream_write = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream_write,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream_write,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 20,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_write,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream_write + 6,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream_write + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream_drain
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream_drain[86] = {
    0xc1, 0x40, 0x0a,
    MP_QSTR_drain & 0xff, MP_QSTR_drain >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_memoryview & 0xff, MP_QSTR_memoryview >> 8, 
    0xb0, 
    0x13, MP_QSTR_out_buf & 0xff, MP_QSTR_out_buf >> 8, 
    0x34, 0x01, 
    0xc1, 
    0x80, 
    0xc2, 
    0x42, 0x2c, 0x80, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x14, MP_QSTR_queue_write & 0xff, MP_QSTR_queue_write >> 8, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x36, 0x01, 
    0x67, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_s & 0xff, MP_QSTR_s >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb1, 
    0xb2, 
    0x51, 
    0x2e, 0x02, 
    0x55, 
    0x36, 0x01, 
    0xc3, 
    0xb3, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x04, 0x80, 
    0xb2, 
    0xb3, 
    0xe5, 
    0xc2, 
    0xb2, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb1, 
    0x34, 0x01, 
    0xd7, 
    0x43, 0xc9, 0x7f, 
    0x23, 0x01, 
    0xb0, 
    0x18, MP_QSTR_out_buf & 0xff, MP_QSTR_out_buf >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_uasyncio_stream__lt_module_gt__Stream_drain_0 = {{&mp_type_bytes}, 5381, 0, (const byte*)""};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream_drain[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&const_obj_uasyncio_stream__lt_module_gt__Stream_drain_0),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream_drain = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream_drain,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream_drain,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 86,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 9,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_drain,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream_drain + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream_drain + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Stream
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Stream[86] = {
    0x08, 0x0a,
    MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x2c, 0x00, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_get_extra_info & 0xff, MP_QSTR_get_extra_info >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR___aenter__ & 0xff, MP_QSTR___aenter__ >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR___aexit__ & 0xff, MP_QSTR___aexit__ >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_wait_closed & 0xff, MP_QSTR_wait_closed >> 8, 
    0x32, 0x06, 
    0x16, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x32, 0x07, 
    0x16, MP_QSTR_readinto & 0xff, MP_QSTR_readinto >> 8, 
    0x32, 0x08, 
    0x16, MP_QSTR_readexactly & 0xff, MP_QSTR_readexactly >> 8, 
    0x32, 0x09, 
    0x16, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x32, 0x0a, 
    0x16, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x32, 0x0b, 
    0x16, MP_QSTR_drain & 0xff, MP_QSTR_drain >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Stream[12] = {
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream___init__),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream_get_extra_info),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream___aenter__),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream___aexit__),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream_close),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream_wait_closed),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream_read),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream_readinto),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream_readexactly),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream_readline),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream_write),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream_drain),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Stream = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Stream,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Stream,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 86,
    .n_obj = 0,
    .n_raw_code = 12,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_Stream,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Stream + 6,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Stream + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__open_connection
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__open_connection[143] = {
    0xea, 0x42, 0x0a,
    MP_QSTR_open_connection & 0xff, MP_QSTR_open_connection >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x80, 
    0x10, MP_QSTR_EINPROGRESS & 0xff, MP_QSTR_EINPROGRESS >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_uerrno & 0xff, MP_QSTR_uerrno >> 8, 
    0x1c, MP_QSTR_EINPROGRESS & 0xff, MP_QSTR_EINPROGRESS >> 8, 
    0xc2, 
    0x59, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0xc3, 
    0xb3, 
    0x14, MP_QSTR_getaddrinfo & 0xff, MP_QSTR_getaddrinfo >> 8, 
    0xb0, 
    0xb1, 
    0x80, 
    0xb3, 
    0x13, MP_QSTR_SOCK_STREAM & 0xff, MP_QSTR_SOCK_STREAM >> 8, 
    0x36, 0x04, 
    0x80, 
    0x55, 
    0xc4, 
    0xb3, 
    0x14, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0xb4, 
    0x80, 
    0x55, 
    0xb4, 
    0x81, 
    0x55, 
    0xb4, 
    0x82, 
    0x55, 
    0x36, 0x03, 
    0xc5, 
    0xb5, 
    0x14, MP_QSTR_setblocking & 0xff, MP_QSTR_setblocking >> 8, 
    0x50, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0xb5, 
    0x34, 0x01, 
    0xc6, 
    0x48, 0x0d, 0x00, 
    0xb5, 
    0x14, MP_QSTR_connect & 0xff, MP_QSTR_connect >> 8, 
    0xb4, 
    0x7f, 
    0x55, 
    0x36, 0x01, 
    0x59, 
    0x4a, 0x21, 0x00, 
    0x57, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x18, 0x80, 
    0xc7, 
    0x49, 0x0c, 0x00, 
    0xb7, 
    0x13, MP_QSTR_errno & 0xff, MP_QSTR_errno >> 8, 
    0xb2, 
    0xdc, 
    0x44, 0x02, 0x80, 
    0xb7, 
    0x65, 
    0x51, 
    0x51, 
    0xc7, 
    0x28, 0x07, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x14, MP_QSTR_queue_write & 0xff, MP_QSTR_queue_write >> 8, 
    0xb5, 
    0x36, 0x01, 
    0x67, 
    0x59, 
    0xb6, 
    0xb6, 
    0x2a, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__open_connection[2] = {
    MP_ROM_QSTR(MP_QSTR_host),
    MP_ROM_QSTR(MP_QSTR_port),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__open_connection = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 2,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__open_connection,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__open_connection,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 143,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 14,
        .n_exc_stack = 2,
        .scope_flags = 1,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_open_connection,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__open_connection + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__open_connection + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Server___aenter__
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Server___aenter__[10] = {
    0x89, 0x40, 0x0a,
    MP_QSTR___aenter__ & 0xff, MP_QSTR___aenter__ >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb0, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Server___aenter__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Server___aenter__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Server___aenter__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Server___aenter__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 10,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___aenter__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Server___aenter__ + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Server___aenter__ + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Server___aexit__
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Server___aexit__[27] = {
    0xa8, 0x44, 0x0a,
    MP_QSTR___aexit__ & 0xff, MP_QSTR___aexit__ >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb0, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_wait_closed & 0xff, MP_QSTR_wait_closed >> 8, 
    0x36, 0x00, 
    0x5e, 
    0x51, 
    0x68, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Server___aexit__[4] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_exc_type),
    MP_ROM_QSTR(MP_QSTR_exc),
    MP_ROM_QSTR(MP_QSTR_tb),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Server___aexit__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 4,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Server___aexit__,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Server___aexit__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 27,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 4,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___aexit__,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Server___aexit__ + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Server___aexit__ + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Server_close
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Server_close[19] = {
    0x11, 0x0a,
    MP_QSTR_close & 0xff, MP_QSTR_close >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_task & 0xff, MP_QSTR_task >> 8, 
    0x14, MP_QSTR_cancel & 0xff, MP_QSTR_cancel >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Server_close[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Server_close = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Server_close,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Server_close,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 19,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_close,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Server_close + 6,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Server_close + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Server_wait_closed
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Server_wait_closed[18] = {
    0x91, 0x40, 0x0a,
    MP_QSTR_wait_closed & 0xff, MP_QSTR_wait_closed >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_task & 0xff, MP_QSTR_task >> 8, 
    0x5e, 
    0x51, 
    0x68, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Server_wait_closed[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Server_wait_closed = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 1,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Server_wait_closed,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Server_wait_closed,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 18,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_wait_closed,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Server_wait_closed + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Server_wait_closed + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Server__serve
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Server__serve[119] = {
    0xdf, 0x40, 0x0a,
    MP_QSTR__serve & 0xff, MP_QSTR__serve >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x48, 0x11, 0x00, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR__io_queue & 0xff, MP_QSTR__io_queue >> 8, 
    0x14, MP_QSTR_queue_read & 0xff, MP_QSTR_queue_read >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x67, 
    0x59, 
    0x4a, 0x19, 0x00, 
    0x57, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x13, MP_QSTR_CancelledError & 0xff, MP_QSTR_CancelledError >> 8, 
    0xdf, 
    0x44, 0x0d, 0x80, 
    0x59, 
    0xb1, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0x63, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x48, 0x0d, 0x00, 
    0xb1, 
    0x14, MP_QSTR_accept & 0xff, MP_QSTR_accept >> 8, 
    0x36, 0x00, 
    0x30, 0x02, 
    0xc3, 
    0xc4, 
    0x4a, 0x09, 0x00, 
    0x59, 
    0x40, 0x29, 0x80, 0x01, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0xb3, 
    0x14, MP_QSTR_setblocking & 0xff, MP_QSTR_setblocking >> 8, 
    0x50, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0xb3, 
    0x2c, 0x01, 
    0xb4, 
    0x10, MP_QSTR_peername & 0xff, MP_QSTR_peername >> 8, 
    0x62, 
    0x34, 0x02, 
    0xc5, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x14, MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8, 
    0xb2, 
    0xb5, 
    0xb5, 
    0x34, 0x02, 
    0x36, 0x01, 
    0x59, 
    0x42, 0x93, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Server__serve[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_s),
    MP_ROM_QSTR(MP_QSTR_cb),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Server__serve = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 3,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Server__serve,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Server__serve,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 119,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 12,
        .n_exc_stack = 1,
        .scope_flags = 1,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__serve,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Server__serve + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Server__serve + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__Server
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__Server[46] = {
    0x00, 0x0a,
    MP_QSTR_Server & 0xff, MP_QSTR_Server >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_Server & 0xff, MP_QSTR_Server >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___aenter__ & 0xff, MP_QSTR___aenter__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR___aexit__ & 0xff, MP_QSTR___aexit__ >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_wait_closed & 0xff, MP_QSTR_wait_closed >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR__serve & 0xff, MP_QSTR__serve >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__Server[5] = {
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Server___aenter__),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Server___aexit__),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Server_close),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Server_wait_closed),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Server__serve),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__Server = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__Server,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__Server,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 46,
    .n_obj = 0,
    .n_raw_code = 5,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_Server,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__Server + 6,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__Server + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__start_server
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__start_server[102] = {
    0xe0, 0x45, 0x0a,
    MP_QSTR_start_server & 0xff, MP_QSTR_start_server >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0xc4, 
    0xb4, 
    0x14, MP_QSTR_getaddrinfo & 0xff, MP_QSTR_getaddrinfo >> 8, 
    0xb1, 
    0xb2, 
    0x36, 0x02, 
    0x80, 
    0x55, 
    0xc1, 
    0xb4, 
    0x14, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x36, 0x00, 
    0xc5, 
    0xb5, 
    0x14, MP_QSTR_setblocking & 0xff, MP_QSTR_setblocking >> 8, 
    0x50, 
    0x36, 0x01, 
    0x59, 
    0xb5, 
    0x14, MP_QSTR_setsockopt & 0xff, MP_QSTR_setsockopt >> 8, 
    0xb4, 
    0x13, MP_QSTR_SOL_SOCKET & 0xff, MP_QSTR_SOL_SOCKET >> 8, 
    0xb4, 
    0x13, MP_QSTR_SO_REUSEADDR & 0xff, MP_QSTR_SO_REUSEADDR >> 8, 
    0x81, 
    0x36, 0x03, 
    0x59, 
    0xb5, 
    0x14, MP_QSTR_bind & 0xff, MP_QSTR_bind >> 8, 
    0xb1, 
    0x7f, 
    0x55, 
    0x36, 0x01, 
    0x59, 
    0xb5, 
    0x14, MP_QSTR_listen & 0xff, MP_QSTR_listen >> 8, 
    0xb3, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_Server & 0xff, MP_QSTR_Server >> 8, 
    0x34, 0x00, 
    0xc6, 
    0x12, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x14, MP_QSTR_create_task & 0xff, MP_QSTR_create_task >> 8, 
    0xb6, 
    0x14, MP_QSTR__serve & 0xff, MP_QSTR__serve >> 8, 
    0xb5, 
    0xb0, 
    0x36, 0x02, 
    0x36, 0x01, 
    0xb6, 
    0x18, MP_QSTR_task & 0xff, MP_QSTR_task >> 8, 
    0xb6, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__start_server[4] = {
    MP_ROM_QSTR(MP_QSTR_cb),
    MP_ROM_QSTR(MP_QSTR_host),
    MP_ROM_QSTR(MP_QSTR_port),
    MP_ROM_QSTR(MP_QSTR_backlog),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__start_server = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 4,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__start_server,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__start_server,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 102,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 13,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 4,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_start_server,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__start_server + 7,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__start_server + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream__lt_module_gt__stream_awrite
STATIC const byte fun_data_uasyncio_stream__lt_module_gt__stream_awrite[70] = {
    0xb8, 0xc4, 0x01, 0x0a,
    MP_QSTR_stream_awrite & 0xff, MP_QSTR_stream_awrite >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0xb2, 
    0x80, 
    0xdc, 
    0x43, 0x06, 0x80, 
    0xb3, 
    0x7f, 
    0xdc, 
    0x44, 0x1d, 0x80, 
    0x12, MP_QSTR_memoryview & 0xff, MP_QSTR_memoryview >> 8, 
    0xb1, 
    0x34, 0x01, 
    0xc1, 
    0xb3, 
    0x7f, 
    0xd9, 
    0x44, 0x07, 0x80, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb1, 
    0x34, 0x01, 
    0xc3, 
    0xb1, 
    0xb2, 
    0xb2, 
    0xb3, 
    0xf2, 
    0x2e, 0x02, 
    0x55, 
    0xc1, 
    0xb0, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_drain & 0xff, MP_QSTR_drain >> 8, 
    0x36, 0x00, 
    0x5e, 
    0x51, 
    0x68, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt__stream_awrite[4] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_buf),
    MP_ROM_QSTR(MP_QSTR_off),
    MP_ROM_QSTR(MP_QSTR_sz),
};
STATIC const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt__stream_awrite = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 4,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt__stream_awrite,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt__stream_awrite,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 70,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 8,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 4,
        .n_kwonly_args = 0,
        .n_def_pos_args = 2,
        .qstr_block_name = MP_QSTR_stream_awrite,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt__stream_awrite + 8,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt__stream_awrite + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file uasyncio/stream.py, scope uasyncio_stream_<module>
STATIC const byte fun_data_uasyncio_stream__lt_module_gt_[113] = {
    0x10, 0x0a,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_uasyncio_slash_stream_dot_py & 0xff, MP_QSTR_uasyncio_slash_stream_dot_py >> 8,
    0x00,
    0x81, 
    0x10, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x1c, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x16, MP_QSTR_core & 0xff, MP_QSTR_core >> 8, 
    0x59, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0x11, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0x16, MP_QSTR_StreamReader & 0xff, MP_QSTR_StreamReader >> 8, 
    0x11, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0x16, MP_QSTR_StreamWriter & 0xff, MP_QSTR_StreamWriter >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_open_connection & 0xff, MP_QSTR_open_connection >> 8, 
    0x54, 
    0x32, 0x02, 
    0x10, MP_QSTR_Server & 0xff, MP_QSTR_Server >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_Server & 0xff, MP_QSTR_Server >> 8, 
    0x85, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x03, 
    0x16, MP_QSTR_start_server & 0xff, MP_QSTR_start_server >> 8, 
    0x80, 
    0x7f, 
    0x2a, 0x02, 
    0x53, 
    0x33, 0x04, 
    0x16, MP_QSTR_stream_awrite & 0xff, MP_QSTR_stream_awrite >> 8, 
    0x11, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0x13, MP_QSTR_wait_closed & 0xff, MP_QSTR_wait_closed >> 8, 
    0x11, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0x18, MP_QSTR_aclose & 0xff, MP_QSTR_aclose >> 8, 
    0x11, MP_QSTR_stream_awrite & 0xff, MP_QSTR_stream_awrite >> 8, 
    0x11, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0x18, MP_QSTR_awrite & 0xff, MP_QSTR_awrite >> 8, 
    0x11, MP_QSTR_stream_awrite & 0xff, MP_QSTR_stream_awrite >> 8, 
    0x11, MP_QSTR_Stream & 0xff, MP_QSTR_Stream >> 8, 
    0x18, MP_QSTR_awritestr & 0xff, MP_QSTR_awritestr >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_uasyncio_stream__lt_module_gt_[5] = {
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Stream),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__open_connection),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__Server),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__start_server),
    MP_ROM_PTR(&raw_code_uasyncio_stream__lt_module_gt__stream_awrite),
};
const mp_raw_code_t raw_code_uasyncio_stream__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_uasyncio_stream__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_uasyncio_stream__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 113,
    .n_obj = 0,
    .n_raw_code = 5,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_uasyncio_slash_stream_dot_py,
        .line_info = fun_data_uasyncio_stream__lt_module_gt_ + 6,
        .opcodes = fun_data_uasyncio_stream__lt_module_gt_ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl.py, scope webrepl__lt_module_gt__setup_conn
STATIC const byte fun_data_webrepl__lt_module_gt__setup_conn[180] = {
    0x6a, 0x28,
    MP_QSTR_setup_conn & 0xff, MP_QSTR_setup_conn >> 8,
    MP_QSTR_webrepl_dot_py & 0xff, MP_QSTR_webrepl_dot_py >> 8,
    0x80, 0x0d, 0x20, 0x2b, 0x56, 0x2d, 0x46, 0x2a, 0x2a, 0x24, 0x31, 0x33, 0x2a, 0x29, 0x37, 0x00,
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x14, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x36, 0x00, 
    0x17, MP_QSTR_listen_s & 0xff, MP_QSTR_listen_s >> 8, 
    0x12, MP_QSTR_listen_s & 0xff, MP_QSTR_listen_s >> 8, 
    0x14, MP_QSTR_setsockopt & 0xff, MP_QSTR_setsockopt >> 8, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x13, MP_QSTR_SOL_SOCKET & 0xff, MP_QSTR_SOL_SOCKET >> 8, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x13, MP_QSTR_SO_REUSEADDR & 0xff, MP_QSTR_SO_REUSEADDR >> 8, 
    0x81, 
    0x36, 0x03, 
    0x59, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x14, MP_QSTR_getaddrinfo & 0xff, MP_QSTR_getaddrinfo >> 8, 
    0x10, MP_QSTR_0_dot_0_dot_0_dot_0 & 0xff, MP_QSTR_0_dot_0_dot_0_dot_0 >> 8, 
    0xb0, 
    0x36, 0x02, 
    0xc2, 
    0xb2, 
    0x80, 
    0x55, 
    0x84, 
    0x55, 
    0xc3, 
    0x12, MP_QSTR_listen_s & 0xff, MP_QSTR_listen_s >> 8, 
    0x14, MP_QSTR_bind & 0xff, MP_QSTR_bind >> 8, 
    0xb3, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_listen_s & 0xff, MP_QSTR_listen_s >> 8, 
    0x14, MP_QSTR_listen & 0xff, MP_QSTR_listen >> 8, 
    0x81, 
    0x36, 0x01, 
    0x59, 
    0xb1, 
    0x44, 0x11, 0x80, 
    0x12, MP_QSTR_listen_s & 0xff, MP_QSTR_listen_s >> 8, 
    0x14, MP_QSTR_setsockopt & 0xff, MP_QSTR_setsockopt >> 8, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x13, MP_QSTR_SOL_SOCKET & 0xff, MP_QSTR_SOL_SOCKET >> 8, 
    0x94, 
    0xb1, 
    0x36, 0x03, 
    0x59, 
    0x12, MP_QSTR_network & 0xff, MP_QSTR_network >> 8, 
    0x13, MP_QSTR_AP_IF & 0xff, MP_QSTR_AP_IF >> 8, 
    0x12, MP_QSTR_network & 0xff, MP_QSTR_network >> 8, 
    0x13, MP_QSTR_STA_IF & 0xff, MP_QSTR_STA_IF >> 8, 
    0x2a, 0x02, 
    0x5f, 
    0x4b, 0x2b, 0x00, 
    0xc4, 
    0x12, MP_QSTR_network & 0xff, MP_QSTR_network >> 8, 
    0x14, MP_QSTR_WLAN & 0xff, MP_QSTR_WLAN >> 8, 
    0xb4, 
    0x36, 0x01, 
    0xc5, 
    0xb5, 
    0x14, MP_QSTR_active & 0xff, MP_QSTR_active >> 8, 
    0x36, 0x00, 
    0x44, 0x14, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0xb5, 
    0x14, MP_QSTR_ifconfig & 0xff, MP_QSTR_ifconfig >> 8, 
    0x36, 0x00, 
    0x80, 
    0x55, 
    0xb0, 
    0x2a, 0x02, 
    0xf8, 
    0x34, 0x01, 
    0x59, 
    0x42, 0xd2, 0x7f, 
    0x12, MP_QSTR_listen_s & 0xff, MP_QSTR_listen_s >> 8, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_webrepl__lt_module_gt__setup_conn_0 = {{&mp_type_str}, 6721, 36, (const byte*)"\x57\x65\x62\x52\x45\x50\x4c\x20\x64\x61\x65\x6d\x6f\x6e\x20\x73\x74\x61\x72\x74\x65\x64\x20\x6f\x6e\x20\x77\x73\x3a\x2f\x2f\x25\x73\x3a\x25\x64"};
STATIC const mp_rom_obj_t const_table_data_webrepl__lt_module_gt__setup_conn[3] = {
    MP_ROM_QSTR(MP_QSTR_port),
    MP_ROM_QSTR(MP_QSTR_accept_handler),
    MP_ROM_PTR(&const_obj_webrepl__lt_module_gt__setup_conn_0),
};
STATIC const mp_raw_code_t raw_code_webrepl__lt_module_gt__setup_conn = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_webrepl__lt_module_gt__setup_conn,
    .const_table = (mp_uint_t*)const_table_data_webrepl__lt_module_gt__setup_conn,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 180,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 14,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_setup_conn,
        .qstr_source_file = MP_QSTR_webrepl_dot_py,
        .line_info = fun_data_webrepl__lt_module_gt__setup_conn + 6,
        .opcodes = fun_data_webrepl__lt_module_gt__setup_conn + 22,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl.py, scope webrepl__lt_module_gt__accept_conn
STATIC const byte fun_data_webrepl__lt_module_gt__accept_conn[177] = {
    0x49, 0x2e,
    MP_QSTR_accept_conn & 0xff, MP_QSTR_accept_conn >> 8,
    MP_QSTR_webrepl_dot_py & 0xff, MP_QSTR_webrepl_dot_py >> 8,
    0x80, 0x20, 0x20, 0x2a, 0x2a, 0x2a, 0x24, 0x2c, 0x27, 0x22, 0x29, 0x24, 0x2a, 0x2b, 0x2a, 0x48, 0x2d, 0x34, 0x00,
    0xb0, 
    0x14, MP_QSTR_accept & 0xff, MP_QSTR_accept >> 8, 
    0x36, 0x00, 
    0x30, 0x02, 
    0xc1, 
    0xc2, 
    0x12, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x14, MP_QSTR_dupterm & 0xff, MP_QSTR_dupterm >> 8, 
    0x51, 
    0x36, 0x01, 
    0xc3, 
    0x12, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x14, MP_QSTR_dupterm & 0xff, MP_QSTR_dupterm >> 8, 
    0xb3, 
    0x36, 0x01, 
    0x59, 
    0xb3, 
    0x44, 0x15, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x01, 
    0xb2, 
    0x10, MP_QSTR_rejected & 0xff, MP_QSTR_rejected >> 8, 
    0x34, 0x03, 
    0x59, 
    0xb1, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0x63, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0xb2, 
    0x34, 0x02, 
    0x59, 
    0xb1, 
    0x17, MP_QSTR_client_s & 0xff, MP_QSTR_client_s >> 8, 
    0x12, MP_QSTR_websocket_helper & 0xff, MP_QSTR_websocket_helper >> 8, 
    0x14, MP_QSTR_server_handshake & 0xff, MP_QSTR_server_handshake >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_uwebsocket & 0xff, MP_QSTR_uwebsocket >> 8, 
    0x14, MP_QSTR_websocket & 0xff, MP_QSTR_websocket >> 8, 
    0xb1, 
    0x52, 
    0x36, 0x02, 
    0xc4, 
    0x12, MP_QSTR__webrepl & 0xff, MP_QSTR__webrepl >> 8, 
    0x14, MP_QSTR__webrepl & 0xff, MP_QSTR__webrepl >> 8, 
    0xb4, 
    0x36, 0x01, 
    0xc4, 
    0xb1, 
    0x14, MP_QSTR_setblocking & 0xff, MP_QSTR_setblocking >> 8, 
    0x50, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_hasattr & 0xff, MP_QSTR_hasattr >> 8, 
    0x12, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x23, 0x03, 
    0x34, 0x02, 
    0x44, 0x14, 0x80, 
    0xb1, 
    0x14, MP_QSTR_setsockopt & 0xff, MP_QSTR_setsockopt >> 8, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x13, MP_QSTR_SOL_SOCKET & 0xff, MP_QSTR_SOL_SOCKET >> 8, 
    0x94, 
    0x12, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x13, MP_QSTR_dupterm_notify & 0xff, MP_QSTR_dupterm_notify >> 8, 
    0x36, 0x03, 
    0x59, 
    0x12, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x14, MP_QSTR_dupterm & 0xff, MP_QSTR_dupterm >> 8, 
    0xb4, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_webrepl__lt_module_gt__accept_conn_0 = {{&mp_type_str}, 16607, 35, (const byte*)"\x0a\x43\x6f\x6e\x63\x75\x72\x72\x65\x6e\x74\x20\x57\x65\x62\x52\x45\x50\x4c\x20\x63\x6f\x6e\x6e\x65\x63\x74\x69\x6f\x6e\x20\x66\x72\x6f\x6d"};
STATIC const mp_obj_str_t const_obj_webrepl__lt_module_gt__accept_conn_1 = {{&mp_type_str}, 13454, 25, (const byte*)"\x0a\x57\x65\x62\x52\x45\x50\x4c\x20\x63\x6f\x6e\x6e\x65\x63\x74\x69\x6f\x6e\x20\x66\x72\x6f\x6d\x3a"};
STATIC const mp_obj_str_t const_obj_webrepl__lt_module_gt__accept_conn_2 = {{&mp_type_str}, 30646, 14, (const byte*)"\x64\x75\x70\x74\x65\x72\x6d\x5f\x6e\x6f\x74\x69\x66\x79"};
STATIC const mp_rom_obj_t const_table_data_webrepl__lt_module_gt__accept_conn[4] = {
    MP_ROM_QSTR(MP_QSTR_listen_sock),
    MP_ROM_PTR(&const_obj_webrepl__lt_module_gt__accept_conn_0),
    MP_ROM_PTR(&const_obj_webrepl__lt_module_gt__accept_conn_1),
    MP_ROM_PTR(&const_obj_webrepl__lt_module_gt__accept_conn_2),
};
STATIC const mp_raw_code_t raw_code_webrepl__lt_module_gt__accept_conn = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_webrepl__lt_module_gt__accept_conn,
    .const_table = (mp_uint_t*)const_table_data_webrepl__lt_module_gt__accept_conn,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 177,
    .n_obj = 3,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 10,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_accept_conn,
        .qstr_source_file = MP_QSTR_webrepl_dot_py,
        .line_info = fun_data_webrepl__lt_module_gt__accept_conn + 6,
        .opcodes = fun_data_webrepl__lt_module_gt__accept_conn + 25,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl.py, scope webrepl__lt_module_gt__stop
STATIC const byte fun_data_webrepl__lt_module_gt__stop[56] = {
    0x10, 0x18,
    MP_QSTR_stop & 0xff, MP_QSTR_stop >> 8,
    MP_QSTR_webrepl_dot_py & 0xff, MP_QSTR_webrepl_dot_py >> 8,
    0x80, 0x35, 0x20, 0x2a, 0x26, 0x29, 0x26, 0x00,
    0x12, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x14, MP_QSTR_dupterm & 0xff, MP_QSTR_dupterm >> 8, 
    0x51, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_client_s & 0xff, MP_QSTR_client_s >> 8, 
    0x44, 0x09, 0x80, 
    0x12, MP_QSTR_client_s & 0xff, MP_QSTR_client_s >> 8, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x12, MP_QSTR_listen_s & 0xff, MP_QSTR_listen_s >> 8, 
    0x44, 0x09, 0x80, 
    0x12, MP_QSTR_listen_s & 0xff, MP_QSTR_listen_s >> 8, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_webrepl__lt_module_gt__stop = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_webrepl__lt_module_gt__stop,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 56,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_stop,
        .qstr_source_file = MP_QSTR_webrepl_dot_py,
        .line_info = fun_data_webrepl__lt_module_gt__stop + 6,
        .opcodes = fun_data_webrepl__lt_module_gt__stop + 14,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl.py, scope webrepl__lt_module_gt__start
STATIC const byte fun_data_webrepl__lt_module_gt__start[122] = {
    0xbe, 0x80, 0x01, 0x22,
    MP_QSTR_start & 0xff, MP_QSTR_start >> 8,
    MP_QSTR_webrepl_dot_py & 0xff, MP_QSTR_webrepl_dot_py >> 8,
    0x80, 0x3e, 0x26, 0x26, 0x23, 0x46, 0x2d, 0x2a, 0x4c, 0x4f, 0x2a, 0x2a, 0x00,
    0x12, MP_QSTR_stop & 0xff, MP_QSTR_stop >> 8, 
    0x34, 0x00, 
    0x59, 
    0xb1, 
    0x51, 
    0xde, 
    0x44, 0x3b, 0x80, 
    0x48, 0x28, 0x00, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_webrepl_cfg & 0xff, MP_QSTR_webrepl_cfg >> 8, 
    0xc2, 
    0x12, MP_QSTR__webrepl & 0xff, MP_QSTR__webrepl >> 8, 
    0x14, MP_QSTR_password & 0xff, MP_QSTR_password >> 8, 
    0xb2, 
    0x13, MP_QSTR_PASS & 0xff, MP_QSTR_PASS >> 8, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_setup_conn & 0xff, MP_QSTR_setup_conn >> 8, 
    0xb0, 
    0x12, MP_QSTR_accept_conn & 0xff, MP_QSTR_accept_conn >> 8, 
    0x34, 0x02, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0x34, 0x01, 
    0x59, 
    0x4a, 0x0d, 0x00, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x03, 
    0x34, 0x01, 
    0x59, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x42, 0x1c, 0x80, 
    0x12, MP_QSTR__webrepl & 0xff, MP_QSTR__webrepl >> 8, 
    0x14, MP_QSTR_password & 0xff, MP_QSTR_password >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_setup_conn & 0xff, MP_QSTR_setup_conn >> 8, 
    0xb0, 
    0x12, MP_QSTR_accept_conn & 0xff, MP_QSTR_accept_conn >> 8, 
    0x34, 0x02, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x04, 
    0x34, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_webrepl__lt_module_gt__start_0 = {{&mp_type_str}, 16616, 30, (const byte*)"\x53\x74\x61\x72\x74\x65\x64\x20\x77\x65\x62\x72\x65\x70\x6c\x20\x69\x6e\x20\x6e\x6f\x72\x6d\x61\x6c\x20\x6d\x6f\x64\x65"};
STATIC const mp_obj_str_t const_obj_webrepl__lt_module_gt__start_1 = {{&mp_type_str}, 39750, 53, (const byte*)"\x57\x65\x62\x52\x45\x50\x4c\x20\x69\x73\x20\x6e\x6f\x74\x20\x63\x6f\x6e\x66\x69\x67\x75\x72\x65\x64\x2c\x20\x72\x75\x6e\x20\x27\x69\x6d\x70\x6f\x72\x74\x20\x77\x65\x62\x72\x65\x70\x6c\x5f\x73\x65\x74\x75\x70\x27"};
STATIC const mp_obj_str_t const_obj_webrepl__lt_module_gt__start_2 = {{&mp_type_str}, 57813, 39, (const byte*)"\x53\x74\x61\x72\x74\x65\x64\x20\x77\x65\x62\x72\x65\x70\x6c\x20\x69\x6e\x20\x6d\x61\x6e\x75\x61\x6c\x20\x6f\x76\x65\x72\x72\x69\x64\x65\x20\x6d\x6f\x64\x65"};
STATIC const mp_rom_obj_t const_table_data_webrepl__lt_module_gt__start[5] = {
    MP_ROM_QSTR(MP_QSTR_port),
    MP_ROM_QSTR(MP_QSTR_password),
    MP_ROM_PTR(&const_obj_webrepl__lt_module_gt__start_0),
    MP_ROM_PTR(&const_obj_webrepl__lt_module_gt__start_1),
    MP_ROM_PTR(&const_obj_webrepl__lt_module_gt__start_2),
};
STATIC const mp_raw_code_t raw_code_webrepl__lt_module_gt__start = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_webrepl__lt_module_gt__start,
    .const_table = (mp_uint_t*)const_table_data_webrepl__lt_module_gt__start,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 122,
    .n_obj = 3,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 8,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 2,
        .qstr_block_name = MP_QSTR_start,
        .qstr_source_file = MP_QSTR_webrepl_dot_py,
        .line_info = fun_data_webrepl__lt_module_gt__start + 8,
        .opcodes = fun_data_webrepl__lt_module_gt__start + 21,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl.py, scope webrepl__lt_module_gt__start_foreground
STATIC const byte fun_data_webrepl__lt_module_gt__start_foreground[35] = {
    0xa1, 0x01, 0x12,
    MP_QSTR_start_foreground & 0xff, MP_QSTR_start_foreground >> 8,
    MP_QSTR_webrepl_dot_py & 0xff, MP_QSTR_webrepl_dot_py >> 8,
    0x80, 0x4f, 0x26, 0x28, 0x00,
    0x12, MP_QSTR_stop & 0xff, MP_QSTR_stop >> 8, 
    0x34, 0x00, 
    0x59, 
    0x12, MP_QSTR_setup_conn & 0xff, MP_QSTR_setup_conn >> 8, 
    0xb0, 
    0x51, 
    0x34, 0x02, 
    0xc1, 
    0x12, MP_QSTR_accept_conn & 0xff, MP_QSTR_accept_conn >> 8, 
    0xb1, 
    0x34, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_webrepl__lt_module_gt__start_foreground[1] = {
    MP_ROM_QSTR(MP_QSTR_port),
};
STATIC const mp_raw_code_t raw_code_webrepl__lt_module_gt__start_foreground = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_webrepl__lt_module_gt__start_foreground,
    .const_table = (mp_uint_t*)const_table_data_webrepl__lt_module_gt__start_foreground,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 35,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_start_foreground,
        .qstr_source_file = MP_QSTR_webrepl_dot_py,
        .line_info = fun_data_webrepl__lt_module_gt__start_foreground + 7,
        .opcodes = fun_data_webrepl__lt_module_gt__start_foreground + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl.py, scope webrepl_<module>
STATIC const byte fun_data_webrepl__lt_module_gt_[122] = {
    0x08, 0x2c,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_webrepl_dot_py & 0xff, MP_QSTR_webrepl_dot_py >> 8,
    0x20, 0x28, 0x28, 0x28, 0x28, 0x28, 0x48, 0x24, 0x64, 0x85, 0x13, 0x85, 0x15, 0x85, 0x09, 0x8d, 0x11, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x16, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x16, MP_QSTR_uos & 0xff, MP_QSTR_uos >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_network & 0xff, MP_QSTR_network >> 8, 
    0x16, MP_QSTR_network & 0xff, MP_QSTR_network >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uwebsocket & 0xff, MP_QSTR_uwebsocket >> 8, 
    0x16, MP_QSTR_uwebsocket & 0xff, MP_QSTR_uwebsocket >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_websocket_helper & 0xff, MP_QSTR_websocket_helper >> 8, 
    0x16, MP_QSTR_websocket_helper & 0xff, MP_QSTR_websocket_helper >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR__webrepl & 0xff, MP_QSTR__webrepl >> 8, 
    0x16, MP_QSTR__webrepl & 0xff, MP_QSTR__webrepl >> 8, 
    0x51, 
    0x17, MP_QSTR_listen_s & 0xff, MP_QSTR_listen_s >> 8, 
    0x51, 
    0x17, MP_QSTR_client_s & 0xff, MP_QSTR_client_s >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR_setup_conn & 0xff, MP_QSTR_setup_conn >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_accept_conn & 0xff, MP_QSTR_accept_conn >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_stop & 0xff, MP_QSTR_stop >> 8, 
    0x22, 0x80, 0xc0, 0x4a, 
    0x51, 
    0x2a, 0x02, 
    0x53, 
    0x33, 0x03, 
    0x16, MP_QSTR_start & 0xff, MP_QSTR_start >> 8, 
    0x22, 0x80, 0xc0, 0x4a, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x04, 
    0x16, MP_QSTR_start_foreground & 0xff, MP_QSTR_start_foreground >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_webrepl__lt_module_gt_[5] = {
    MP_ROM_PTR(&raw_code_webrepl__lt_module_gt__setup_conn),
    MP_ROM_PTR(&raw_code_webrepl__lt_module_gt__accept_conn),
    MP_ROM_PTR(&raw_code_webrepl__lt_module_gt__stop),
    MP_ROM_PTR(&raw_code_webrepl__lt_module_gt__start),
    MP_ROM_PTR(&raw_code_webrepl__lt_module_gt__start_foreground),
};
const mp_raw_code_t raw_code_webrepl__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_webrepl__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_webrepl__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 122,
    .n_obj = 0,
    .n_raw_code = 5,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_webrepl_dot_py,
        .line_info = fun_data_webrepl__lt_module_gt_ + 6,
        .opcodes = fun_data_webrepl__lt_module_gt_ + 24,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl_setup.py, scope webrepl_setup__lt_module_gt__input_choice
STATIC const byte fun_data_webrepl_setup__lt_module_gt__input_choice[32] = {
    0x22, 0x14,
    MP_QSTR_input_choice & 0xff, MP_QSTR_input_choice >> 8,
    MP_QSTR_webrepl_setup_dot_py & 0xff, MP_QSTR_webrepl_setup_dot_py >> 8,
    0x80, 0x0b, 0x20, 0x27, 0x26, 0x00,
    0x12, MP_QSTR_input & 0xff, MP_QSTR_input >> 8, 
    0xb0, 
    0x34, 0x01, 
    0xc2, 
    0xb2, 
    0xb1, 
    0xdd, 
    0x44, 0x02, 0x80, 
    0xb2, 
    0x63, 
    0x42, 0xee, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_webrepl_setup__lt_module_gt__input_choice[2] = {
    MP_ROM_QSTR(MP_QSTR_prompt),
    MP_ROM_QSTR(MP_QSTR_choices),
};
STATIC const mp_raw_code_t raw_code_webrepl_setup__lt_module_gt__input_choice = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_webrepl_setup__lt_module_gt__input_choice,
    .const_table = (mp_uint_t*)const_table_data_webrepl_setup__lt_module_gt__input_choice,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 32,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_input_choice,
        .qstr_source_file = MP_QSTR_webrepl_setup_dot_py,
        .line_info = fun_data_webrepl_setup__lt_module_gt__input_choice + 6,
        .opcodes = fun_data_webrepl_setup__lt_module_gt__input_choice + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl_setup.py, scope webrepl_setup__lt_module_gt__getpass
STATIC const byte fun_data_webrepl_setup__lt_module_gt__getpass[16] = {
    0x11, 0x0e,
    MP_QSTR_getpass & 0xff, MP_QSTR_getpass >> 8,
    MP_QSTR_webrepl_setup_dot_py & 0xff, MP_QSTR_webrepl_setup_dot_py >> 8,
    0x80, 0x12, 0x00,
    0x12, MP_QSTR_input & 0xff, MP_QSTR_input >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_webrepl_setup__lt_module_gt__getpass[1] = {
    MP_ROM_QSTR(MP_QSTR_prompt),
};
STATIC const mp_raw_code_t raw_code_webrepl_setup__lt_module_gt__getpass = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_webrepl_setup__lt_module_gt__getpass,
    .const_table = (mp_uint_t*)const_table_data_webrepl_setup__lt_module_gt__getpass,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 16,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_getpass,
        .qstr_source_file = MP_QSTR_webrepl_setup_dot_py,
        .line_info = fun_data_webrepl_setup__lt_module_gt__getpass + 6,
        .opcodes = fun_data_webrepl_setup__lt_module_gt__getpass + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl_setup.py, scope webrepl_setup__lt_module_gt__input_pass
STATIC const byte fun_data_webrepl_setup__lt_module_gt__input_pass[87] = {
    0x18, 0x1e,
    MP_QSTR_input_pass & 0xff, MP_QSTR_input_pass >> 8,
    MP_QSTR_webrepl_setup_dot_py & 0xff, MP_QSTR_webrepl_setup_dot_py >> 8,
    0x80, 0x16, 0x20, 0x28, 0x36, 0x28, 0x23, 0x28, 0x26, 0x22, 0x00,
    0x12, MP_QSTR_getpass & 0xff, MP_QSTR_getpass >> 8, 
    0x23, 0x00, 
    0x34, 0x01, 
    0xc0, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x84, 
    0xd7, 
    0x43, 0x0b, 0x80, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x89, 
    0xd8, 
    0x44, 0x0b, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x01, 
    0x34, 0x01, 
    0x59, 
    0x42, 0x18, 0x80, 
    0x12, MP_QSTR_getpass & 0xff, MP_QSTR_getpass >> 8, 
    0x23, 0x02, 
    0x34, 0x01, 
    0xc1, 
    0xb0, 
    0xb1, 
    0xd9, 
    0x44, 0x02, 0x80, 
    0xb0, 
    0x63, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x03, 
    0x34, 0x01, 
    0x59, 
    0x42, 0xbc, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__input_pass_0 = {{&mp_type_str}, 12822, 26, (const byte*)"\x4e\x65\x77\x20\x70\x61\x73\x73\x77\x6f\x72\x64\x20\x28\x34\x2d\x39\x20\x63\x68\x61\x72\x73\x29\x3a\x20"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__input_pass_1 = {{&mp_type_str}, 22679, 23, (const byte*)"\x49\x6e\x76\x61\x6c\x69\x64\x20\x70\x61\x73\x73\x77\x6f\x72\x64\x20\x6c\x65\x6e\x67\x74\x68"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__input_pass_2 = {{&mp_type_str}, 25106, 18, (const byte*)"\x43\x6f\x6e\x66\x69\x72\x6d\x20\x70\x61\x73\x73\x77\x6f\x72\x64\x3a\x20"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__input_pass_3 = {{&mp_type_str}, 36388, 22, (const byte*)"\x50\x61\x73\x73\x77\x6f\x72\x64\x73\x20\x64\x6f\x20\x6e\x6f\x74\x20\x6d\x61\x74\x63\x68"};
STATIC const mp_rom_obj_t const_table_data_webrepl_setup__lt_module_gt__input_pass[4] = {
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__input_pass_0),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__input_pass_1),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__input_pass_2),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__input_pass_3),
};
STATIC const mp_raw_code_t raw_code_webrepl_setup__lt_module_gt__input_pass = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_webrepl_setup__lt_module_gt__input_pass,
    .const_table = (mp_uint_t*)const_table_data_webrepl_setup__lt_module_gt__input_pass,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 87,
    .n_obj = 4,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_input_pass,
        .qstr_source_file = MP_QSTR_webrepl_setup_dot_py,
        .line_info = fun_data_webrepl_setup__lt_module_gt__input_pass + 6,
        .opcodes = fun_data_webrepl_setup__lt_module_gt__input_pass + 17,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl_setup.py, scope webrepl_setup__lt_module_gt__exists
STATIC const byte fun_data_webrepl_setup__lt_module_gt__exists[52] = {
    0xb1, 0x02, 0x16,
    MP_QSTR_exists & 0xff, MP_QSTR_exists >> 8,
    MP_QSTR_webrepl_setup_dot_py & 0xff, MP_QSTR_webrepl_setup_dot_py >> 8,
    0x80, 0x22, 0x23, 0x2a, 0x23, 0x4e, 0x00,
    0x48, 0x12, 0x00, 
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x47, 0x02, 0x00, 
    0x59, 
    0x51, 
    0x5c, 
    0x5d, 
    0x52, 
    0x63, 
    0x4a, 0x0f, 0x00, 
    0x57, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x06, 0x80, 
    0x59, 
    0x50, 
    0x63, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_webrepl_setup__lt_module_gt__exists[1] = {
    MP_ROM_QSTR(MP_QSTR_fname),
};
STATIC const mp_raw_code_t raw_code_webrepl_setup__lt_module_gt__exists = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_webrepl_setup__lt_module_gt__exists,
    .const_table = (mp_uint_t*)const_table_data_webrepl_setup__lt_module_gt__exists,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 52,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_exists,
        .qstr_source_file = MP_QSTR_webrepl_setup_dot_py,
        .line_info = fun_data_webrepl_setup__lt_module_gt__exists + 7,
        .opcodes = fun_data_webrepl_setup__lt_module_gt__exists + 14,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl_setup.py, scope webrepl_setup__lt_module_gt__get_daemon_status
STATIC const byte fun_data_webrepl_setup__lt_module_gt__get_daemon_status[67] = {
    0x54, 0x1a,
    MP_QSTR_get_daemon_status & 0xff, MP_QSTR_get_daemon_status >> 8,
    MP_QSTR_webrepl_setup_dot_py & 0xff, MP_QSTR_webrepl_setup_dot_py >> 8,
    0x80, 0x2b, 0x2c, 0x26, 0x28, 0x2c, 0x22, 0x25, 0x00,
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0x12, MP_QSTR_RC & 0xff, MP_QSTR_RC >> 8, 
    0x34, 0x01, 
    0x47, 0x25, 0x00, 
    0xc0, 
    0xb0, 
    0x5f, 
    0x4b, 0x1c, 0x00, 
    0xc1, 
    0x10, MP_QSTR_webrepl & 0xff, MP_QSTR_webrepl >> 8, 
    0xb1, 
    0xdd, 
    0x44, 0x10, 0x80, 
    0xb1, 
    0x14, MP_QSTR_startswith & 0xff, MP_QSTR_startswith >> 8, 
    0x10, MP_QSTR__hash_ & 0xff, MP_QSTR__hash_ >> 8, 
    0x36, 0x01, 
    0x44, 0x02, 0x80, 
    0x50, 
    0x63, 
    0x52, 
    0x63, 
    0x42, 0xe1, 0x7f, 
    0x51, 
    0x63, 
    0x51, 
    0x5c, 
    0x5d, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_webrepl_setup__lt_module_gt__get_daemon_status = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_webrepl_setup__lt_module_gt__get_daemon_status,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 67,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_get_daemon_status,
        .qstr_source_file = MP_QSTR_webrepl_setup_dot_py,
        .line_info = fun_data_webrepl_setup__lt_module_gt__get_daemon_status + 6,
        .opcodes = fun_data_webrepl_setup__lt_module_gt__get_daemon_status + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl_setup.py, scope webrepl_setup__lt_module_gt__change_daemon
STATIC const byte fun_data_webrepl_setup__lt_module_gt__change_daemon[202] = {
    0xa9, 0x12, 0x2c,
    MP_QSTR_change_daemon & 0xff, MP_QSTR_change_daemon >> 8,
    MP_QSTR_webrepl_setup_dot_py & 0xff, MP_QSTR_webrepl_setup_dot_py >> 8,
    0x80, 0x35, 0x27, 0x3f, 0x22, 0x26, 0x26, 0x26, 0x22, 0x30, 0x2e, 0x2c, 0x2c, 0x2b, 0x24, 0x6f, 0x2c, 0x00,
    0x23, 0x01, 
    0x23, 0x02, 
    0x2a, 0x02, 
    0xc1, 
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0x12, MP_QSTR_RC & 0xff, MP_QSTR_RC >> 8, 
    0x34, 0x01, 
    0x47, 0x7c, 0x00, 
    0xc2, 
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0x12, MP_QSTR_RC & 0xff, MP_QSTR_RC >> 8, 
    0x10, MP_QSTR__dot_tmp & 0xff, MP_QSTR__dot_tmp >> 8, 
    0xf2, 
    0x10, MP_QSTR_w & 0xff, MP_QSTR_w >> 8, 
    0x34, 0x02, 
    0x47, 0x66, 0x00, 
    0xc3, 
    0x50, 
    0xc4, 
    0xb2, 
    0x5f, 
    0x4b, 0x50, 0x00, 
    0xc5, 
    0xb1, 
    0x5f, 
    0x4b, 0x3f, 0x00, 
    0xc6, 
    0xb6, 
    0xb5, 
    0xdd, 
    0x44, 0x35, 0x80, 
    0x52, 
    0xc4, 
    0xb0, 
    0x44, 0x16, 0x80, 
    0xb5, 
    0x14, MP_QSTR_startswith & 0xff, MP_QSTR_startswith >> 8, 
    0x10, MP_QSTR__hash_ & 0xff, MP_QSTR__hash_ >> 8, 
    0x36, 0x01, 
    0x44, 0x0a, 0x80, 
    0xb5, 
    0x81, 
    0x51, 
    0x2e, 0x02, 
    0x55, 
    0xc5, 
    0x42, 0x19, 0x80, 
    0xb0, 
    0x43, 0x15, 0x80, 
    0xb5, 
    0x14, MP_QSTR_startswith & 0xff, MP_QSTR_startswith >> 8, 
    0x10, MP_QSTR__hash_ & 0xff, MP_QSTR__hash_ >> 8, 
    0x36, 0x01, 
    0x43, 0x09, 0x80, 
    0x10, MP_QSTR__hash_ & 0xff, MP_QSTR__hash_ >> 8, 
    0xb5, 
    0xf2, 
    0xc5, 
    0x42, 0x00, 0x80, 
    0x42, 0xbe, 0x7f, 
    0xb3, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb5, 
    0x36, 0x01, 
    0x59, 
    0x42, 0xad, 0x7f, 
    0xb4, 
    0x43, 0x09, 0x80, 
    0xb3, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x03, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x5c, 
    0x5d, 
    0x51, 
    0x5c, 
    0x5d, 
    0x12, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x14, MP_QSTR_remove & 0xff, MP_QSTR_remove >> 8, 
    0x12, MP_QSTR_RC & 0xff, MP_QSTR_RC >> 8, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x14, MP_QSTR_rename & 0xff, MP_QSTR_rename >> 8, 
    0x12, MP_QSTR_RC & 0xff, MP_QSTR_RC >> 8, 
    0x10, MP_QSTR__dot_tmp & 0xff, MP_QSTR__dot_tmp >> 8, 
    0xf2, 
    0x12, MP_QSTR_RC & 0xff, MP_QSTR_RC >> 8, 
    0x36, 0x02, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__change_daemon_0 = {{&mp_type_str}, 42115, 14, (const byte*)"\x69\x6d\x70\x6f\x72\x74\x20\x77\x65\x62\x72\x65\x70\x6c"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__change_daemon_1 = {{&mp_type_str}, 30641, 15, (const byte*)"\x77\x65\x62\x72\x65\x70\x6c\x2e\x73\x74\x61\x72\x74\x28\x29"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__change_daemon_2 = {{&mp_type_str}, 34423, 31, (const byte*)"\x69\x6d\x70\x6f\x72\x74\x20\x77\x65\x62\x72\x65\x70\x6c\x0a\x77\x65\x62\x72\x65\x70\x6c\x2e\x73\x74\x61\x72\x74\x28\x29\x0a"};
STATIC const mp_rom_obj_t const_table_data_webrepl_setup__lt_module_gt__change_daemon[4] = {
    MP_ROM_QSTR(MP_QSTR_action),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__change_daemon_0),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__change_daemon_1),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__change_daemon_2),
};
STATIC const mp_raw_code_t raw_code_webrepl_setup__lt_module_gt__change_daemon = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_webrepl_setup__lt_module_gt__change_daemon,
    .const_table = (mp_uint_t*)const_table_data_webrepl_setup__lt_module_gt__change_daemon,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 202,
    .n_obj = 3,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 22,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_change_daemon,
        .qstr_source_file = MP_QSTR_webrepl_setup_dot_py,
        .line_info = fun_data_webrepl_setup__lt_module_gt__change_daemon + 7,
        .opcodes = fun_data_webrepl_setup__lt_module_gt__change_daemon + 25,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl_setup.py, scope webrepl_setup__lt_module_gt__main
STATIC const byte fun_data_webrepl_setup__lt_module_gt__main[298] = {
    0x54, 0x3c,
    MP_QSTR_main & 0xff, MP_QSTR_main >> 8,
    MP_QSTR_webrepl_setup_dot_py & 0xff, MP_QSTR_webrepl_setup_dot_py >> 8,
    0x80, 0x4a, 0x46, 0x35, 0x28, 0x28, 0x4e, 0x28, 0x2b, 0x23, 0x73, 0x28, 0x44, 0x28, 0x26, 0x2f, 0x4f, 0x1f, 0x27, 0x28, 0x49, 0x4b, 0x28, 0x33, 0x28, 0x00,
    0x12, MP_QSTR_get_daemon_status & 0xff, MP_QSTR_get_daemon_status >> 8, 
    0x34, 0x00, 
    0xc0, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x00, 
    0xb0, 
    0x44, 0x06, 0x80, 
    0x10, MP_QSTR_enabled & 0xff, MP_QSTR_enabled >> 8, 
    0x42, 0x03, 0x80, 
    0x10, MP_QSTR_disabled & 0xff, MP_QSTR_disabled >> 8, 
    0x34, 0x02, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x01, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_input & 0xff, MP_QSTR_input >> 8, 
    0x10, MP_QSTR__gt__space_ & 0xff, MP_QSTR__gt__space_ >> 8, 
    0x34, 0x01, 
    0x14, MP_QSTR_upper & 0xff, MP_QSTR_upper >> 8, 
    0x36, 0x00, 
    0xc1, 
    0xb1, 
    0x10, MP_QSTR_E & 0xff, MP_QSTR_E >> 8, 
    0xd9, 
    0x44, 0x59, 0x80, 
    0x12, MP_QSTR_exists & 0xff, MP_QSTR_exists >> 8, 
    0x12, MP_QSTR_CONFIG & 0xff, MP_QSTR_CONFIG >> 8, 
    0x34, 0x01, 
    0x44, 0x16, 0x80, 
    0x12, MP_QSTR_input_choice & 0xff, MP_QSTR_input_choice >> 8, 
    0x23, 0x03, 
    0x10, MP_QSTR_y & 0xff, MP_QSTR_y >> 8, 
    0x10, MP_QSTR_n & 0xff, MP_QSTR_n >> 8, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x2a, 0x03, 
    0x34, 0x02, 
    0xc2, 
    0x42, 0x0c, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x04, 
    0x34, 0x01, 
    0x59, 
    0x10, MP_QSTR_y & 0xff, MP_QSTR_y >> 8, 
    0xc2, 
    0xb2, 
    0x10, MP_QSTR_y & 0xff, MP_QSTR_y >> 8, 
    0xd9, 
    0x44, 0x24, 0x80, 
    0x12, MP_QSTR_input_pass & 0xff, MP_QSTR_input_pass >> 8, 
    0x34, 0x00, 
    0xc3, 
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0x12, MP_QSTR_CONFIG & 0xff, MP_QSTR_CONFIG >> 8, 
    0x10, MP_QSTR_w & 0xff, MP_QSTR_w >> 8, 
    0x34, 0x02, 
    0x47, 0x0e, 0x00, 
    0xc4, 
    0xb4, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x10, MP_QSTR_PASS_space__equals__space__percent_r_0x0a_ & 0xff, MP_QSTR_PASS_space__equals__space__percent_r_0x0a_ >> 8, 
    0xb3, 
    0xf8, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x5c, 
    0x5d, 
    0xb1, 
    0x10, MP_QSTR_D & 0xff, MP_QSTR_D >> 8, 
    0x10, MP_QSTR_E & 0xff, MP_QSTR_E >> 8, 
    0x2a, 0x02, 
    0xdd, 
    0xd3, 
    0x43, 0x18, 0x80, 
    0xb1, 
    0x10, MP_QSTR_D & 0xff, MP_QSTR_D >> 8, 
    0xd9, 
    0x44, 0x04, 0x80, 
    0xb0, 
    0x44, 0x0c, 0x80, 
    0xb1, 
    0x10, MP_QSTR_E & 0xff, MP_QSTR_E >> 8, 
    0xd9, 
    0x44, 0x15, 0x80, 
    0xb0, 
    0x44, 0x11, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x05, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x14, MP_QSTR_exit & 0xff, MP_QSTR_exit >> 8, 
    0x36, 0x00, 
    0x59, 
    0x12, MP_QSTR_change_daemon & 0xff, MP_QSTR_change_daemon >> 8, 
    0xb1, 
    0x10, MP_QSTR_E & 0xff, MP_QSTR_E >> 8, 
    0xd9, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x06, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_input_choice & 0xff, MP_QSTR_input_choice >> 8, 
    0x23, 0x07, 
    0x10, MP_QSTR_y & 0xff, MP_QSTR_y >> 8, 
    0x10, MP_QSTR_n & 0xff, MP_QSTR_n >> 8, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x2a, 0x03, 
    0x34, 0x02, 
    0xc1, 
    0xb1, 
    0x10, MP_QSTR_y & 0xff, MP_QSTR_y >> 8, 
    0xd9, 
    0x44, 0x09, 0x80, 
    0x12, MP_QSTR_machine & 0xff, MP_QSTR_machine >> 8, 
    0x14, MP_QSTR_reset & 0xff, MP_QSTR_reset >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__main_0 = {{&mp_type_str}, 24126, 33, (const byte*)"\x57\x65\x62\x52\x45\x50\x4c\x20\x64\x61\x65\x6d\x6f\x6e\x20\x61\x75\x74\x6f\x2d\x73\x74\x61\x72\x74\x20\x73\x74\x61\x74\x75\x73\x3a"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__main_1 = {{&mp_type_str}, 9123, 60, (const byte*)"\x0a\x57\x6f\x75\x6c\x64\x20\x79\x6f\x75\x20\x6c\x69\x6b\x65\x20\x74\x6f\x20\x28\x45\x29\x6e\x61\x62\x6c\x65\x20\x6f\x72\x20\x28\x44\x29\x69\x73\x61\x62\x6c\x65\x20\x69\x74\x20\x72\x75\x6e\x6e\x69\x6e\x67\x20\x6f\x6e\x20\x62\x6f\x6f\x74\x3f"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__main_2 = {{&mp_type_str}, 39613, 20, (const byte*)"\x28\x45\x6d\x70\x74\x79\x20\x6c\x69\x6e\x65\x20\x74\x6f\x20\x71\x75\x69\x74\x29"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__main_3 = {{&mp_type_str}, 49111, 49, (const byte*)"\x57\x6f\x75\x6c\x64\x20\x79\x6f\x75\x20\x6c\x69\x6b\x65\x20\x74\x6f\x20\x63\x68\x61\x6e\x67\x65\x20\x57\x65\x62\x52\x45\x50\x4c\x20\x70\x61\x73\x73\x77\x6f\x72\x64\x3f\x20\x28\x79\x2f\x6e\x29\x20"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__main_4 = {{&mp_type_str}, 7023, 47, (const byte*)"\x54\x6f\x20\x65\x6e\x61\x62\x6c\x65\x20\x57\x65\x62\x52\x45\x50\x4c\x2c\x20\x79\x6f\x75\x20\x6d\x75\x73\x74\x20\x73\x65\x74\x20\x70\x61\x73\x73\x77\x6f\x72\x64\x20\x66\x6f\x72\x20\x69\x74"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__main_5 = {{&mp_type_str}, 24025, 26, (const byte*)"\x4e\x6f\x20\x66\x75\x72\x74\x68\x65\x72\x20\x61\x63\x74\x69\x6f\x6e\x20\x72\x65\x71\x75\x69\x72\x65\x64"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__main_6 = {{&mp_type_str}, 15057, 38, (const byte*)"\x43\x68\x61\x6e\x67\x65\x73\x20\x77\x69\x6c\x6c\x20\x62\x65\x20\x61\x63\x74\x69\x76\x61\x74\x65\x64\x20\x61\x66\x74\x65\x72\x20\x72\x65\x62\x6f\x6f\x74"};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__main_7 = {{&mp_type_str}, 7554, 36, (const byte*)"\x57\x6f\x75\x6c\x64\x20\x79\x6f\x75\x20\x6c\x69\x6b\x65\x20\x74\x6f\x20\x72\x65\x62\x6f\x6f\x74\x20\x6e\x6f\x77\x3f\x20\x28\x79\x2f\x6e\x29\x20"};
STATIC const mp_rom_obj_t const_table_data_webrepl_setup__lt_module_gt__main[8] = {
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__main_0),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__main_1),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__main_2),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__main_3),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__main_4),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__main_5),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__main_6),
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__main_7),
};
STATIC const mp_raw_code_t raw_code_webrepl_setup__lt_module_gt__main = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_webrepl_setup__lt_module_gt__main,
    .const_table = (mp_uint_t*)const_table_data_webrepl_setup__lt_module_gt__main,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 298,
    .n_obj = 8,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_main,
        .qstr_source_file = MP_QSTR_webrepl_setup_dot_py,
        .line_info = fun_data_webrepl_setup__lt_module_gt__main + 6,
        .opcodes = fun_data_webrepl_setup__lt_module_gt__main + 32,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file webrepl_setup.py, scope webrepl_setup_<module>
STATIC const byte fun_data_webrepl_setup__lt_module_gt_[104] = {
    0x08, 0x30,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_webrepl_setup_dot_py & 0xff, MP_QSTR_webrepl_setup_dot_py >> 8,
    0x68, 0x28, 0x48, 0x26, 0x65, 0x85, 0x07, 0x65, 0x20, 0x85, 0x0c, 0x85, 0x09, 0x85, 0x0a, 0x85, 0x15, 0x85, 0x22, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x16, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x16, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_machine & 0xff, MP_QSTR_machine >> 8, 
    0x16, MP_QSTR_machine & 0xff, MP_QSTR_machine >> 8, 
    0x10, MP_QSTR__dot__slash_boot_dot_py & 0xff, MP_QSTR__dot__slash_boot_dot_py >> 8, 
    0x16, MP_QSTR_RC & 0xff, MP_QSTR_RC >> 8, 
    0x23, 0x00, 
    0x16, MP_QSTR_CONFIG & 0xff, MP_QSTR_CONFIG >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_input_choice & 0xff, MP_QSTR_input_choice >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_getpass & 0xff, MP_QSTR_getpass >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_input_pass & 0xff, MP_QSTR_input_pass >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_exists & 0xff, MP_QSTR_exists >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_get_daemon_status & 0xff, MP_QSTR_get_daemon_status >> 8, 
    0x32, 0x06, 
    0x16, MP_QSTR_change_daemon & 0xff, MP_QSTR_change_daemon >> 8, 
    0x32, 0x07, 
    0x16, MP_QSTR_main & 0xff, MP_QSTR_main >> 8, 
    0x11, MP_QSTR_main & 0xff, MP_QSTR_main >> 8, 
    0x34, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_webrepl_setup__lt_module_gt__0 = {{&mp_type_str}, 32613, 16, (const byte*)"\x2e\x2f\x77\x65\x62\x72\x65\x70\x6c\x5f\x63\x66\x67\x2e\x70\x79"};
STATIC const mp_rom_obj_t const_table_data_webrepl_setup__lt_module_gt_[8] = {
    MP_ROM_PTR(&const_obj_webrepl_setup__lt_module_gt__0),
    MP_ROM_PTR(&raw_code_webrepl_setup__lt_module_gt__input_choice),
    MP_ROM_PTR(&raw_code_webrepl_setup__lt_module_gt__getpass),
    MP_ROM_PTR(&raw_code_webrepl_setup__lt_module_gt__input_pass),
    MP_ROM_PTR(&raw_code_webrepl_setup__lt_module_gt__exists),
    MP_ROM_PTR(&raw_code_webrepl_setup__lt_module_gt__get_daemon_status),
    MP_ROM_PTR(&raw_code_webrepl_setup__lt_module_gt__change_daemon),
    MP_ROM_PTR(&raw_code_webrepl_setup__lt_module_gt__main),
};
const mp_raw_code_t raw_code_webrepl_setup__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_webrepl_setup__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_webrepl_setup__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 104,
    .n_obj = 1,
    .n_raw_code = 7,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_webrepl_setup_dot_py,
        .line_info = fun_data_webrepl_setup__lt_module_gt_ + 6,
        .opcodes = fun_data_webrepl_setup__lt_module_gt_ + 26,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file websocket_helper.py, scope websocket_helper__lt_module_gt__server_handshake_<listcomp>
STATIC const byte fun_data_websocket_helper__lt_module_gt__server_handshake__lt_listcomp_gt_[29] = {
    0x41, 0x0e,
    MP_QSTR__lt_listcomp_gt_ & 0xff, MP_QSTR__lt_listcomp_gt_ >> 8,
    MP_QSTR_websocket_helper_dot_py & 0xff, MP_QSTR_websocket_helper_dot_py >> 8,
    0x80, 0x1f, 0x00,
    0x2b, 0x00, 
    0xb0, 
    0x5f, 
    0x4b, 0x0c, 0x00, 
    0xc1, 
    0xb1, 
    0x14, MP_QSTR_strip & 0xff, MP_QSTR_strip >> 8, 
    0x36, 0x00, 
    0x2f, 0x14, 
    0x42, 0xf1, 0x7f, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_websocket_helper__lt_module_gt__server_handshake__lt_listcomp_gt_[1] = {
    MP_ROM_QSTR(MP_QSTR__star_),
};
STATIC const mp_raw_code_t raw_code_websocket_helper__lt_module_gt__server_handshake__lt_listcomp_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_websocket_helper__lt_module_gt__server_handshake__lt_listcomp_gt_,
    .const_table = (mp_uint_t*)const_table_data_websocket_helper__lt_module_gt__server_handshake__lt_listcomp_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 29,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 9,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_listcomp_gt_,
        .qstr_source_file = MP_QSTR_websocket_helper_dot_py,
        .line_info = fun_data_websocket_helper__lt_module_gt__server_handshake__lt_listcomp_gt_ + 6,
        .opcodes = fun_data_websocket_helper__lt_module_gt__server_handshake__lt_listcomp_gt_ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file websocket_helper.py, scope websocket_helper__lt_module_gt__server_handshake
STATIC const byte fun_data_websocket_helper__lt_module_gt__server_handshake[250] = {
    0x61, 0x46,
    MP_QSTR_server_handshake & 0xff, MP_QSTR_server_handshake >> 8,
    MP_QSTR_websocket_helper_dot_py & 0xff, MP_QSTR_websocket_helper_dot_py >> 8,
    0x80, 0x12, 0x2b, 0x67, 0x42, 0x20, 0x27, 0x24, 0x28, 0x27, 0x43, 0x31, 0x26, 0x2a, 0x27, 0x45, 0x24, 0x48, 0x26, 0x4f, 0x2a, 0x29, 0x27, 0x2f, 0x26, 0x4a, 0x24, 0x65, 0x60, 0x28, 0x00,
    0xb0, 
    0x14, MP_QSTR_makefile & 0xff, MP_QSTR_makefile >> 8, 
    0x10, MP_QSTR_rwb & 0xff, MP_QSTR_rwb >> 8, 
    0x80, 
    0x36, 0x02, 
    0xc1, 
    0xb1, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0xc2, 
    0x51, 
    0xc3, 
    0xb1, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0xc2, 
    0xb2, 
    0x43, 0x08, 0x80, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0x23, 0x01, 
    0x34, 0x01, 
    0x65, 
    0xb2, 
    0x23, 0x02, 
    0xd9, 
    0x44, 0x03, 0x80, 
    0x42, 0x2d, 0x80, 
    0x32, 0x09, 
    0xb2, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x23, 0x03, 
    0x81, 
    0x36, 0x02, 
    0x34, 0x01, 
    0x30, 0x02, 
    0xc4, 
    0xc5, 
    0x12, MP_QSTR_DEBUG & 0xff, MP_QSTR_DEBUG >> 8, 
    0x44, 0x0a, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0xb4, 
    0xb5, 
    0x2a, 0x02, 
    0x34, 0x01, 
    0x59, 
    0xb4, 
    0x23, 0x04, 
    0xd9, 
    0x44, 0x02, 0x80, 
    0xb5, 
    0xc3, 
    0x42, 0xb6, 0x7f, 
    0xb3, 
    0x43, 0x08, 0x80, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0x23, 0x05, 
    0x34, 0x01, 
    0x65, 
    0x12, MP_QSTR_DEBUG & 0xff, MP_QSTR_DEBUG >> 8, 
    0x44, 0x0f, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x06, 
    0xb3, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb3, 
    0x34, 0x01, 
    0x34, 0x03, 
    0x59, 
    0x12, MP_QSTR_hashlib & 0xff, MP_QSTR_hashlib >> 8, 
    0x14, MP_QSTR_sha1 & 0xff, MP_QSTR_sha1 >> 8, 
    0xb3, 
    0x36, 0x01, 
    0xc6, 
    0xb6, 
    0x14, MP_QSTR_update & 0xff, MP_QSTR_update >> 8, 
    0x23, 0x07, 
    0x36, 0x01, 
    0x59, 
    0xb6, 
    0x14, MP_QSTR_digest & 0xff, MP_QSTR_digest >> 8, 
    0x36, 0x00, 
    0xc7, 
    0x12, MP_QSTR_binascii & 0xff, MP_QSTR_binascii >> 8, 
    0x14, MP_QSTR_b2a_base64 & 0xff, MP_QSTR_b2a_base64 >> 8, 
    0xb7, 
    0x36, 0x01, 
    0x51, 
    0x7f, 
    0x2e, 0x02, 
    0x55, 
    0xc7, 
    0x12, MP_QSTR_DEBUG & 0xff, MP_QSTR_DEBUG >> 8, 
    0x44, 0x0a, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x10, MP_QSTR_respkey_colon_ & 0xff, MP_QSTR_respkey_colon_ >> 8, 
    0xb7, 
    0x34, 0x02, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_send & 0xff, MP_QSTR_send >> 8, 
    0x23, 0x08, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_send & 0xff, MP_QSTR_send >> 8, 
    0xb7, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_send & 0xff, MP_QSTR_send >> 8, 
    0x10, MP_QSTR__0x0d__0x0a__0x0d__0x0a_ & 0xff, MP_QSTR__0x0d__0x0a__0x0d__0x0a_ >> 8, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__server_handshake_0 = {{&mp_type_str}, 54338, 14, (const byte*)"\x45\x4f\x46\x20\x69\x6e\x20\x68\x65\x61\x64\x65\x72\x73"};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__server_handshake_1 = {{&mp_type_bytes}, 27298, 2, (const byte*)"\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__server_handshake_2 = {{&mp_type_bytes}, 46495, 1, (const byte*)"\x3a"};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__server_handshake_3 = {{&mp_type_bytes}, 24914, 17, (const byte*)"\x53\x65\x63\x2d\x57\x65\x62\x53\x6f\x63\x6b\x65\x74\x2d\x4b\x65\x79"};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__server_handshake_4 = {{&mp_type_str}, 60085, 23, (const byte*)"\x4e\x6f\x74\x20\x61\x20\x77\x65\x62\x73\x6f\x63\x6b\x65\x74\x20\x72\x65\x71\x75\x65\x73\x74"};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__server_handshake_5 = {{&mp_type_str}, 35752, 18, (const byte*)"\x53\x65\x63\x2d\x57\x65\x62\x53\x6f\x63\x6b\x65\x74\x2d\x4b\x65\x79\x3a"};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__server_handshake_6 = {{&mp_type_bytes}, 43888, 36, (const byte*)"\x32\x35\x38\x45\x41\x46\x41\x35\x2d\x45\x39\x31\x34\x2d\x34\x37\x44\x41\x2d\x39\x35\x43\x41\x2d\x43\x35\x41\x42\x30\x44\x43\x38\x35\x42\x31\x31"};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__server_handshake_7 = {{&mp_type_bytes}, 65413, 97, (const byte*)"\x48\x54\x54\x50\x2f\x31\x2e\x31\x20\x31\x30\x31\x20\x53\x77\x69\x74\x63\x68\x69\x6e\x67\x20\x50\x72\x6f\x74\x6f\x63\x6f\x6c\x73\x0d\x0a\x55\x70\x67\x72\x61\x64\x65\x3a\x20\x77\x65\x62\x73\x6f\x63\x6b\x65\x74\x0d\x0a\x43\x6f\x6e\x6e\x65\x63\x74\x69\x6f\x6e\x3a\x20\x55\x70\x67\x72\x61\x64\x65\x0d\x0a\x53\x65\x63\x2d\x57\x65\x62\x53\x6f\x63\x6b\x65\x74\x2d\x41\x63\x63\x65\x70\x74\x3a\x20"};
STATIC const mp_rom_obj_t const_table_data_websocket_helper__lt_module_gt__server_handshake[10] = {
    MP_ROM_QSTR(MP_QSTR_sock),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__server_handshake_0),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__server_handshake_1),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__server_handshake_2),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__server_handshake_3),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__server_handshake_4),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__server_handshake_5),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__server_handshake_6),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__server_handshake_7),
    MP_ROM_PTR(&raw_code_websocket_helper__lt_module_gt__server_handshake__lt_listcomp_gt_),
};
STATIC const mp_raw_code_t raw_code_websocket_helper__lt_module_gt__server_handshake = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_websocket_helper__lt_module_gt__server_handshake,
    .const_table = (mp_uint_t*)const_table_data_websocket_helper__lt_module_gt__server_handshake,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 250,
    .n_obj = 8,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 13,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_server_handshake,
        .qstr_source_file = MP_QSTR_websocket_helper_dot_py,
        .line_info = fun_data_websocket_helper__lt_module_gt__server_handshake + 6,
        .opcodes = fun_data_websocket_helper__lt_module_gt__server_handshake + 37,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file websocket_helper.py, scope websocket_helper__lt_module_gt__client_handshake
STATIC const byte fun_data_websocket_helper__lt_module_gt__client_handshake[66] = {
    0x31, 0x1e,
    MP_QSTR_client_handshake & 0xff, MP_QSTR_client_handshake >> 8,
    MP_QSTR_websocket_helper_dot_py & 0xff, MP_QSTR_websocket_helper_dot_py >> 8,
    0x80, 0x41, 0x2b, 0x24, 0x85, 0x09, 0x47, 0x20, 0x27, 0x27, 0x00,
    0xb0, 
    0x14, MP_QSTR_makefile & 0xff, MP_QSTR_makefile >> 8, 
    0x10, MP_QSTR_rwb & 0xff, MP_QSTR_rwb >> 8, 
    0x80, 
    0x36, 0x02, 
    0xc1, 
    0xb1, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x01, 
    0x36, 0x01, 
    0x59, 
    0xb1, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0xc2, 
    0xb1, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0xc2, 
    0xb2, 
    0x23, 0x02, 
    0xd9, 
    0x44, 0x03, 0x80, 
    0x42, 0x03, 0x80, 
    0x42, 0xec, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__client_handshake_0 = {{&mp_type_bytes}, 52281, 109, (const byte*)"\x47\x45\x54\x20\x2f\x20\x48\x54\x54\x50\x2f\x31\x2e\x31\x0d\x0a\x48\x6f\x73\x74\x3a\x20\x65\x63\x68\x6f\x2e\x77\x65\x62\x73\x6f\x63\x6b\x65\x74\x2e\x6f\x72\x67\x0d\x0a\x43\x6f\x6e\x6e\x65\x63\x74\x69\x6f\x6e\x3a\x20\x55\x70\x67\x72\x61\x64\x65\x0d\x0a\x55\x70\x67\x72\x61\x64\x65\x3a\x20\x77\x65\x62\x73\x6f\x63\x6b\x65\x74\x0d\x0a\x53\x65\x63\x2d\x57\x65\x62\x53\x6f\x63\x6b\x65\x74\x2d\x4b\x65\x79\x3a\x20\x66\x6f\x6f\x0d\x0a\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_websocket_helper__lt_module_gt__client_handshake_1 = {{&mp_type_bytes}, 27298, 2, (const byte*)"\x0d\x0a"};
STATIC const mp_rom_obj_t const_table_data_websocket_helper__lt_module_gt__client_handshake[3] = {
    MP_ROM_QSTR(MP_QSTR_sock),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__client_handshake_0),
    MP_ROM_PTR(&const_obj_websocket_helper__lt_module_gt__client_handshake_1),
};
STATIC const mp_raw_code_t raw_code_websocket_helper__lt_module_gt__client_handshake = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_websocket_helper__lt_module_gt__client_handshake,
    .const_table = (mp_uint_t*)const_table_data_websocket_helper__lt_module_gt__client_handshake,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 66,
    .n_obj = 2,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_client_handshake,
        .qstr_source_file = MP_QSTR_websocket_helper_dot_py,
        .line_info = fun_data_websocket_helper__lt_module_gt__client_handshake + 6,
        .opcodes = fun_data_websocket_helper__lt_module_gt__client_handshake + 17,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file websocket_helper.py, scope websocket_helper_<module>
STATIC const byte fun_data_websocket_helper__lt_module_gt_[124] = {
    0x2c, 0x22,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_websocket_helper_dot_py & 0xff, MP_QSTR_websocket_helper_dot_py >> 8,
    0x23, 0x54, 0x4c, 0x23, 0x4c, 0x2c, 0x23, 0x4c, 0x4c, 0x64, 0x85, 0x2f, 0x00,
    0x48, 0x0b, 0x00, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_usys & 0xff, MP_QSTR_usys >> 8, 
    0x16, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x4a, 0x15, 0x00, 
    0x57, 
    0x11, MP_QSTR_ImportError & 0xff, MP_QSTR_ImportError >> 8, 
    0xdf, 
    0x44, 0x0c, 0x80, 
    0x59, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x16, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x48, 0x0b, 0x00, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_ubinascii & 0xff, MP_QSTR_ubinascii >> 8, 
    0x16, MP_QSTR_binascii & 0xff, MP_QSTR_binascii >> 8, 
    0x4a, 0x0d, 0x00, 
    0x59, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_binascii & 0xff, MP_QSTR_binascii >> 8, 
    0x16, MP_QSTR_binascii & 0xff, MP_QSTR_binascii >> 8, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x48, 0x0b, 0x00, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_uhashlib & 0xff, MP_QSTR_uhashlib >> 8, 
    0x16, MP_QSTR_hashlib & 0xff, MP_QSTR_hashlib >> 8, 
    0x4a, 0x0d, 0x00, 
    0x59, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_hashlib & 0xff, MP_QSTR_hashlib >> 8, 
    0x16, MP_QSTR_hashlib & 0xff, MP_QSTR_hashlib >> 8, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x80, 
    0x16, MP_QSTR_DEBUG & 0xff, MP_QSTR_DEBUG >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR_server_handshake & 0xff, MP_QSTR_server_handshake >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_client_handshake & 0xff, MP_QSTR_client_handshake >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_websocket_helper__lt_module_gt_[2] = {
    MP_ROM_PTR(&raw_code_websocket_helper__lt_module_gt__server_handshake),
    MP_ROM_PTR(&raw_code_websocket_helper__lt_module_gt__client_handshake),
};
const mp_raw_code_t raw_code_websocket_helper__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_websocket_helper__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_websocket_helper__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 124,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_websocket_helper_dot_py,
        .line_info = fun_data_websocket_helper__lt_module_gt_ + 6,
        .opcodes = fun_data_websocket_helper__lt_module_gt_ + 19,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file neopixel.py, scope neopixel__lt_module_gt__NeoPixel___init__
STATIC const byte fun_data_neopixel__lt_module_gt__NeoPixel___init__[107] = {
    0xc1, 0x84, 0x01, 0x0a,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_neopixel_dot_py & 0xff, MP_QSTR_neopixel_dot_py >> 8,
    0x00,
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0xb2, 
    0xb0, 
    0x18, MP_QSTR_n & 0xff, MP_QSTR_n >> 8, 
    0xb3, 
    0xb0, 
    0x18, MP_QSTR_bpp & 0xff, MP_QSTR_bpp >> 8, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0xb2, 
    0xb3, 
    0xf4, 
    0x34, 0x01, 
    0xb0, 
    0x18, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0x14, MP_QSTR_init & 0xff, MP_QSTR_init >> 8, 
    0xb1, 
    0x13, MP_QSTR_OUT & 0xff, MP_QSTR_OUT >> 8, 
    0x36, 0x01, 
    0x59, 
    0x12, MP_QSTR_isinstance & 0xff, MP_QSTR_isinstance >> 8, 
    0xb4, 
    0x12, MP_QSTR_int & 0xff, MP_QSTR_int >> 8, 
    0x34, 0x02, 
    0x44, 0x26, 0x80, 
    0xb4, 
    0x44, 0x11, 0x80, 
    0x22, 0x83, 0x10, 
    0x22, 0x86, 0x52, 
    0x22, 0x86, 0x20, 
    0x22, 0x83, 0x42, 
    0x2a, 0x04, 
    0x42, 0x0e, 0x80, 
    0x22, 0x86, 0x20, 
    0x22, 0x8d, 0x24, 
    0x22, 0x8c, 0x40, 
    0x22, 0x87, 0x04, 
    0x2a, 0x04, 
    0x42, 0x01, 0x80, 
    0xb4, 
    0xb0, 
    0x18, MP_QSTR_timing & 0xff, MP_QSTR_timing >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_neopixel__lt_module_gt__NeoPixel___init__[5] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_pin),
    MP_ROM_QSTR(MP_QSTR_n),
    MP_ROM_QSTR(MP_QSTR_bpp),
    MP_ROM_QSTR(MP_QSTR_timing),
};
STATIC const mp_raw_code_t raw_code_neopixel__lt_module_gt__NeoPixel___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 5,
    .fun_data = fun_data_neopixel__lt_module_gt__NeoPixel___init__,
    .const_table = (mp_uint_t*)const_table_data_neopixel__lt_module_gt__NeoPixel___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 107,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 9,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 5,
        .n_kwonly_args = 0,
        .n_def_pos_args = 2,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_neopixel_dot_py,
        .line_info = fun_data_neopixel__lt_module_gt__NeoPixel___init__ + 8,
        .opcodes = fun_data_neopixel__lt_module_gt__NeoPixel___init__ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file neopixel.py, scope neopixel__lt_module_gt__NeoPixel___len__
STATIC const byte fun_data_neopixel__lt_module_gt__NeoPixel___len__[12] = {
    0x09, 0x0a,
    MP_QSTR___len__ & 0xff, MP_QSTR___len__ >> 8,
    MP_QSTR_neopixel_dot_py & 0xff, MP_QSTR_neopixel_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_n & 0xff, MP_QSTR_n >> 8, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_neopixel__lt_module_gt__NeoPixel___len__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_neopixel__lt_module_gt__NeoPixel___len__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_neopixel__lt_module_gt__NeoPixel___len__,
    .const_table = (mp_uint_t*)const_table_data_neopixel__lt_module_gt__NeoPixel___len__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 12,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___len__,
        .qstr_source_file = MP_QSTR_neopixel_dot_py,
        .line_info = fun_data_neopixel__lt_module_gt__NeoPixel___len__ + 6,
        .opcodes = fun_data_neopixel__lt_module_gt__NeoPixel___len__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file neopixel.py, scope neopixel__lt_module_gt__NeoPixel___setitem__
STATIC const byte fun_data_neopixel__lt_module_gt__NeoPixel___setitem__[52] = {
    0x53, 0x0a,
    MP_QSTR___setitem__ & 0xff, MP_QSTR___setitem__ >> 8,
    MP_QSTR_neopixel_dot_py & 0xff, MP_QSTR_neopixel_dot_py >> 8,
    0x00,
    0xb1, 
    0xb0, 
    0x13, MP_QSTR_bpp & 0xff, MP_QSTR_bpp >> 8, 
    0xf4, 
    0xc3, 
    0xb0, 
    0x13, MP_QSTR_bpp & 0xff, MP_QSTR_bpp >> 8, 
    0x80, 
    0x42, 0x14, 0x80, 
    0x57, 
    0xc1, 
    0xb2, 
    0xb1, 
    0x55, 
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0xb3, 
    0xb0, 
    0x13, MP_QSTR_ORDER & 0xff, MP_QSTR_ORDER >> 8, 
    0xb1, 
    0x55, 
    0xf2, 
    0x56, 
    0x81, 
    0xe5, 
    0x58, 
    0x5a, 
    0xd7, 
    0x43, 0xe6, 0x7f, 
    0x59, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_neopixel__lt_module_gt__NeoPixel___setitem__[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_i),
    MP_ROM_QSTR(MP_QSTR_v),
};
STATIC const mp_raw_code_t raw_code_neopixel__lt_module_gt__NeoPixel___setitem__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 3,
    .fun_data = fun_data_neopixel__lt_module_gt__NeoPixel___setitem__,
    .const_table = (mp_uint_t*)const_table_data_neopixel__lt_module_gt__NeoPixel___setitem__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 52,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___setitem__,
        .qstr_source_file = MP_QSTR_neopixel_dot_py,
        .line_info = fun_data_neopixel__lt_module_gt__NeoPixel___setitem__ + 6,
        .opcodes = fun_data_neopixel__lt_module_gt__NeoPixel___setitem__ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file neopixel.py, scope neopixel__lt_module_gt__NeoPixel___getitem___<genexpr>
STATIC const byte fun_data_neopixel__lt_module_gt__NeoPixel___getitem____lt_genexpr_gt_[39] = {
    0xdb, 0x40, 0x0a,
    MP_QSTR__lt_genexpr_gt_ & 0xff, MP_QSTR__lt_genexpr_gt_ >> 8,
    MP_QSTR_neopixel_dot_py & 0xff, MP_QSTR_neopixel_dot_py >> 8,
    0x00,
    0x53, 
    0xb2, 
    0x53, 
    0x53, 
    0x4b, 0x16, 0x00, 
    0xc3, 
    0x25, 0x00, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x25, 0x01, 
    0x25, 0x00, 
    0x13, MP_QSTR_ORDER & 0xff, MP_QSTR_ORDER >> 8, 
    0xb3, 
    0x55, 
    0xf2, 
    0x55, 
    0x67, 
    0x59, 
    0x42, 0xe7, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_neopixel__lt_module_gt__NeoPixel___getitem____lt_genexpr_gt_[3] = {
    MP_ROM_QSTR(MP_QSTR__star_),
    MP_ROM_QSTR(MP_QSTR__star_),
    MP_ROM_QSTR(MP_QSTR__star_),
};
STATIC const mp_raw_code_t raw_code_neopixel__lt_module_gt__NeoPixel___getitem____lt_genexpr_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x01,
    .n_pos_args = 3,
    .fun_data = fun_data_neopixel__lt_module_gt__NeoPixel___getitem____lt_genexpr_gt_,
    .const_table = (mp_uint_t*)const_table_data_neopixel__lt_module_gt__NeoPixel___getitem____lt_genexpr_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 39,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 12,
        .n_exc_stack = 0,
        .scope_flags = 1,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_genexpr_gt_,
        .qstr_source_file = MP_QSTR_neopixel_dot_py,
        .line_info = fun_data_neopixel__lt_module_gt__NeoPixel___getitem____lt_genexpr_gt_ + 7,
        .opcodes = fun_data_neopixel__lt_module_gt__NeoPixel___getitem____lt_genexpr_gt_ + 8,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file neopixel.py, scope neopixel__lt_module_gt__NeoPixel___getitem__
STATIC const byte fun_data_neopixel__lt_module_gt__NeoPixel___getitem__[43] = {
    0x32, 0x8a, 0x01,
    MP_QSTR___getitem__ & 0xff, MP_QSTR___getitem__ >> 8,
    MP_QSTR_neopixel_dot_py & 0xff, MP_QSTR_neopixel_dot_py >> 8,
    0x00, 0x00, 0x02,
    0xb1, 
    0x25, 0x00, 
    0x13, MP_QSTR_bpp & 0xff, MP_QSTR_bpp >> 8, 
    0xf4, 
    0x27, 0x02, 
    0x12, MP_QSTR_tuple & 0xff, MP_QSTR_tuple >> 8, 
    0xb0, 
    0xb2, 
    0x20, 0x02, 0x02, 
    0x12, MP_QSTR_range & 0xff, MP_QSTR_range >> 8, 
    0x25, 0x00, 
    0x13, MP_QSTR_bpp & 0xff, MP_QSTR_bpp >> 8, 
    0x34, 0x01, 
    0x5e, 
    0x34, 0x01, 
    0x34, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_neopixel__lt_module_gt__NeoPixel___getitem__[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_i),
    MP_ROM_PTR(&raw_code_neopixel__lt_module_gt__NeoPixel___getitem____lt_genexpr_gt_),
};
STATIC const mp_raw_code_t raw_code_neopixel__lt_module_gt__NeoPixel___getitem__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_neopixel__lt_module_gt__NeoPixel___getitem__,
    .const_table = (mp_uint_t*)const_table_data_neopixel__lt_module_gt__NeoPixel___getitem__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 43,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___getitem__,
        .qstr_source_file = MP_QSTR_neopixel_dot_py,
        .line_info = fun_data_neopixel__lt_module_gt__NeoPixel___getitem__ + 7,
        .opcodes = fun_data_neopixel__lt_module_gt__NeoPixel___getitem__ + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file neopixel.py, scope neopixel__lt_module_gt__NeoPixel_fill
STATIC const byte fun_data_neopixel__lt_module_gt__NeoPixel_fill[74] = {
    0x72, 0x0a,
    MP_QSTR_fill & 0xff, MP_QSTR_fill >> 8,
    MP_QSTR_neopixel_dot_py & 0xff, MP_QSTR_neopixel_dot_py >> 8,
    0x00,
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0xc2, 
    0xb0, 
    0x13, MP_QSTR_bpp & 0xff, MP_QSTR_bpp >> 8, 
    0x80, 
    0x42, 0x2c, 0x80, 
    0x57, 
    0xc3, 
    0xb1, 
    0xb3, 
    0x55, 
    0xc4, 
    0x12, MP_QSTR_range & 0xff, MP_QSTR_range >> 8, 
    0xb0, 
    0x13, MP_QSTR_ORDER & 0xff, MP_QSTR_ORDER >> 8, 
    0xb3, 
    0x55, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x34, 0x01, 
    0xb0, 
    0x13, MP_QSTR_bpp & 0xff, MP_QSTR_bpp >> 8, 
    0x34, 0x03, 
    0x5f, 
    0x4b, 0x08, 0x00, 
    0xc5, 
    0xb4, 
    0xb2, 
    0xb5, 
    0x56, 
    0x42, 0xf5, 0x7f, 
    0x81, 
    0xe5, 
    0x58, 
    0x5a, 
    0xd7, 
    0x43, 0xce, 0x7f, 
    0x59, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_neopixel__lt_module_gt__NeoPixel_fill[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_v),
};
STATIC const mp_raw_code_t raw_code_neopixel__lt_module_gt__NeoPixel_fill = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_neopixel__lt_module_gt__NeoPixel_fill,
    .const_table = (mp_uint_t*)const_table_data_neopixel__lt_module_gt__NeoPixel_fill,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 74,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 15,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_fill,
        .qstr_source_file = MP_QSTR_neopixel_dot_py,
        .line_info = fun_data_neopixel__lt_module_gt__NeoPixel_fill + 6,
        .opcodes = fun_data_neopixel__lt_module_gt__NeoPixel_fill + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file neopixel.py, scope neopixel__lt_module_gt__NeoPixel_write
STATIC const byte fun_data_neopixel__lt_module_gt__NeoPixel_write[28] = {
    0x29, 0x0a,
    MP_QSTR_write & 0xff, MP_QSTR_write >> 8,
    MP_QSTR_neopixel_dot_py & 0xff, MP_QSTR_neopixel_dot_py >> 8,
    0x00,
    0x12, MP_QSTR_bitstream & 0xff, MP_QSTR_bitstream >> 8, 
    0xb0, 
    0x13, MP_QSTR_pin & 0xff, MP_QSTR_pin >> 8, 
    0x80, 
    0xb0, 
    0x13, MP_QSTR_timing & 0xff, MP_QSTR_timing >> 8, 
    0xb0, 
    0x13, MP_QSTR_buf & 0xff, MP_QSTR_buf >> 8, 
    0x34, 0x04, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_neopixel__lt_module_gt__NeoPixel_write[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_neopixel__lt_module_gt__NeoPixel_write = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_neopixel__lt_module_gt__NeoPixel_write,
    .const_table = (mp_uint_t*)const_table_data_neopixel__lt_module_gt__NeoPixel_write,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 28,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_write,
        .qstr_source_file = MP_QSTR_neopixel_dot_py,
        .line_info = fun_data_neopixel__lt_module_gt__NeoPixel_write + 6,
        .opcodes = fun_data_neopixel__lt_module_gt__NeoPixel_write + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file neopixel.py, scope neopixel__lt_module_gt__NeoPixel
STATIC const byte fun_data_neopixel__lt_module_gt__NeoPixel[65] = {
    0x18, 0x0a,
    MP_QSTR_NeoPixel & 0xff, MP_QSTR_NeoPixel >> 8,
    MP_QSTR_neopixel_dot_py & 0xff, MP_QSTR_neopixel_dot_py >> 8,
    0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_NeoPixel & 0xff, MP_QSTR_NeoPixel >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x81, 
    0x80, 
    0x82, 
    0x83, 
    0x2a, 0x04, 
    0x16, MP_QSTR_ORDER & 0xff, MP_QSTR_ORDER >> 8, 
    0x83, 
    0x81, 
    0x2a, 0x02, 
    0x53, 
    0x33, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR___len__ & 0xff, MP_QSTR___len__ >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR___setitem__ & 0xff, MP_QSTR___setitem__ >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR___getitem__ & 0xff, MP_QSTR___getitem__ >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_fill & 0xff, MP_QSTR_fill >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_neopixel__lt_module_gt__NeoPixel[6] = {
    MP_ROM_PTR(&raw_code_neopixel__lt_module_gt__NeoPixel___init__),
    MP_ROM_PTR(&raw_code_neopixel__lt_module_gt__NeoPixel___len__),
    MP_ROM_PTR(&raw_code_neopixel__lt_module_gt__NeoPixel___setitem__),
    MP_ROM_PTR(&raw_code_neopixel__lt_module_gt__NeoPixel___getitem__),
    MP_ROM_PTR(&raw_code_neopixel__lt_module_gt__NeoPixel_fill),
    MP_ROM_PTR(&raw_code_neopixel__lt_module_gt__NeoPixel_write),
};
STATIC const mp_raw_code_t raw_code_neopixel__lt_module_gt__NeoPixel = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_neopixel__lt_module_gt__NeoPixel,
    .const_table = (mp_uint_t*)const_table_data_neopixel__lt_module_gt__NeoPixel,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 65,
    .n_obj = 0,
    .n_raw_code = 6,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_NeoPixel,
        .qstr_source_file = MP_QSTR_neopixel_dot_py,
        .line_info = fun_data_neopixel__lt_module_gt__NeoPixel + 6,
        .opcodes = fun_data_neopixel__lt_module_gt__NeoPixel + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file neopixel.py, scope neopixel_<module>
STATIC const byte fun_data_neopixel__lt_module_gt_[36] = {
    0x10, 0x0a,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_neopixel_dot_py & 0xff, MP_QSTR_neopixel_dot_py >> 8,
    0x00,
    0x80, 
    0x10, MP_QSTR_bitstream & 0xff, MP_QSTR_bitstream >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_machine & 0xff, MP_QSTR_machine >> 8, 
    0x1c, MP_QSTR_bitstream & 0xff, MP_QSTR_bitstream >> 8, 
    0x16, MP_QSTR_bitstream & 0xff, MP_QSTR_bitstream >> 8, 
    0x59, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_NeoPixel & 0xff, MP_QSTR_NeoPixel >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_NeoPixel & 0xff, MP_QSTR_NeoPixel >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_neopixel__lt_module_gt_[1] = {
    MP_ROM_PTR(&raw_code_neopixel__lt_module_gt__NeoPixel),
};
const mp_raw_code_t raw_code_neopixel__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_neopixel__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_neopixel__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 36,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_neopixel_dot_py,
        .line_info = fun_data_neopixel__lt_module_gt_ + 6,
        .opcodes = fun_data_neopixel__lt_module_gt_ + 7,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__Response___init__
STATIC const byte fun_data_urequests__lt_module_gt__Response___init__[30] = {
    0x1a, 0x12,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x60, 0x40, 0x25, 0x27, 0x00,
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_raw & 0xff, MP_QSTR_raw >> 8, 
    0x10, MP_QSTR_utf_hyphen_8 & 0xff, MP_QSTR_utf_hyphen_8 >> 8, 
    0xb0, 
    0x18, MP_QSTR_encoding & 0xff, MP_QSTR_encoding >> 8, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR__cached & 0xff, MP_QSTR__cached >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__Response___init__[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_f),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__Response___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_urequests__lt_module_gt__Response___init__,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__Response___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 30,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__Response___init__ + 6,
        .opcodes = fun_data_urequests__lt_module_gt__Response___init__ + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__Response_close
STATIC const byte fun_data_urequests__lt_module_gt__Response_close[41] = {
    0x11, 0x14,
    MP_QSTR_close & 0xff, MP_QSTR_close >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x0a, 0x27, 0x2a, 0x25, 0x00,
    0xb0, 
    0x13, MP_QSTR_raw & 0xff, MP_QSTR_raw >> 8, 
    0x44, 0x0f, 0x80, 
    0xb0, 
    0x13, MP_QSTR_raw & 0xff, MP_QSTR_raw >> 8, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR_raw & 0xff, MP_QSTR_raw >> 8, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR__cached & 0xff, MP_QSTR__cached >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__Response_close[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__Response_close = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__Response_close,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__Response_close,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 41,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_close,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__Response_close + 6,
        .opcodes = fun_data_urequests__lt_module_gt__Response_close + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__Response_content
STATIC const byte fun_data_urequests__lt_module_gt__Response_content[61] = {
    0x2d, 0x18,
    MP_QSTR_content & 0xff, MP_QSTR_content >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x11, 0x29, 0x23, 0x4e, 0x2a, 0x26, 0x00,
    0xb0, 
    0x13, MP_QSTR__cached & 0xff, MP_QSTR__cached >> 8, 
    0x51, 
    0xde, 
    0x44, 0x21, 0x80, 
    0x49, 0x0e, 0x00, 
    0xb0, 
    0x13, MP_QSTR_raw & 0xff, MP_QSTR_raw >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x36, 0x00, 
    0xb0, 
    0x18, MP_QSTR__cached & 0xff, MP_QSTR__cached >> 8, 
    0x51, 
    0xb0, 
    0x13, MP_QSTR_raw & 0xff, MP_QSTR_raw >> 8, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR_raw & 0xff, MP_QSTR_raw >> 8, 
    0x5d, 
    0xb0, 
    0x13, MP_QSTR__cached & 0xff, MP_QSTR__cached >> 8, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__Response_content[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__Response_content = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__Response_content,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__Response_content,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 61,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_content,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__Response_content + 6,
        .opcodes = fun_data_urequests__lt_module_gt__Response_content + 14,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__Response_text
STATIC const byte fun_data_urequests__lt_module_gt__Response_text[23] = {
    0x19, 0x0e,
    MP_QSTR_text & 0xff, MP_QSTR_text >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x1b, 0x00,
    0x12, MP_QSTR_str & 0xff, MP_QSTR_str >> 8, 
    0xb0, 
    0x13, MP_QSTR_content & 0xff, MP_QSTR_content >> 8, 
    0xb0, 
    0x13, MP_QSTR_encoding & 0xff, MP_QSTR_encoding >> 8, 
    0x34, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__Response_text[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__Response_text = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__Response_text,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__Response_text,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_text,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__Response_text + 6,
        .opcodes = fun_data_urequests__lt_module_gt__Response_text + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__Response_json
STATIC const byte fun_data_urequests__lt_module_gt__Response_json[27] = {
    0x21, 0x10,
    MP_QSTR_json & 0xff, MP_QSTR_json >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x1e, 0x46, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_ujson & 0xff, MP_QSTR_ujson >> 8, 
    0xc1, 
    0xb1, 
    0x14, MP_QSTR_loads & 0xff, MP_QSTR_loads >> 8, 
    0xb0, 
    0x13, MP_QSTR_content & 0xff, MP_QSTR_content >> 8, 
    0x36, 0x01, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__Response_json[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__Response_json = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__Response_json,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__Response_json,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 27,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_json,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__Response_json + 6,
        .opcodes = fun_data_urequests__lt_module_gt__Response_json + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__Response
STATIC const byte fun_data_urequests__lt_module_gt__Response[66] = {
    0x08, 0x1e,
    MP_QSTR_Response & 0xff, MP_QSTR_Response >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x6c, 0x20, 0x65, 0x40, 0x65, 0x60, 0x8a, 0x0a, 0x6a, 0x20, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_Response & 0xff, MP_QSTR_Response >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x11, MP_QSTR_property & 0xff, MP_QSTR_property >> 8, 
    0x32, 0x02, 
    0x34, 0x01, 
    0x16, MP_QSTR_content & 0xff, MP_QSTR_content >> 8, 
    0x11, MP_QSTR_property & 0xff, MP_QSTR_property >> 8, 
    0x32, 0x03, 
    0x34, 0x01, 
    0x16, MP_QSTR_text & 0xff, MP_QSTR_text >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_json & 0xff, MP_QSTR_json >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__Response[5] = {
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__Response___init__),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__Response_close),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__Response_content),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__Response_text),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__Response_json),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__Response = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_urequests__lt_module_gt__Response,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__Response,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 66,
    .n_obj = 0,
    .n_raw_code = 5,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_Response,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__Response + 6,
        .opcodes = fun_data_urequests__lt_module_gt__Response + 17,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__request
STATIC const byte fun_data_urequests__lt_module_gt__request[648] = {
    0xde, 0x94, 0x80, 0x01, 0x84, 0x02,
    MP_QSTR_request & 0xff, MP_QSTR_request >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x24, 0x23, 0x5c, 0x2f, 0x28, 0x28, 0x27, 0x28, 0x46, 0x47, 0x4a, 0x28, 0x2e, 0x47, 0x32, 0x44, 0x32, 0x23, 0x2a, 0x28, 0x2d, 0x2e, 0x28, 0x4b, 0x26, 0x28, 0x29, 0x2a, 0x2c, 0x27, 0x2a, 0x46, 0x28, 0x29, 0x24, 0x30, 0x29, 0x24, 0x48, 0x48, 0x2b, 0x2b, 0x25, 0x2c, 0x2b, 0x20, 0x28, 0x2d, 0x43, 0x2c, 0x28, 0x2e, 0x1f, 0x24, 0x5a, 0x27, 0x45, 0x28, 0x27, 0x27, 0x00,
    0x48, 0x13, 0x00, 
    0xb1, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0x83, 
    0x36, 0x02, 
    0x30, 0x04, 
    0xc6, 
    0xc7, 
    0xc8, 
    0xc9, 
    0x4a, 0x20, 0x00, 
    0x57, 
    0x12, MP_QSTR_ValueError & 0xff, MP_QSTR_ValueError >> 8, 
    0xdf, 
    0x44, 0x17, 0x80, 
    0x59, 
    0xb1, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x10, MP_QSTR__slash_ & 0xff, MP_QSTR__slash_ >> 8, 
    0x82, 
    0x36, 0x02, 
    0x30, 0x03, 
    0xc6, 
    0xc7, 
    0xc8, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0xc9, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0xb6, 
    0x10, MP_QSTR_http_colon_ & 0xff, MP_QSTR_http_colon_ >> 8, 
    0xd9, 
    0x44, 0x07, 0x80, 
    0x22, 0x80, 0x50, 
    0xca, 
    0x42, 0x1f, 0x80, 
    0xb6, 
    0x10, MP_QSTR_https_colon_ & 0xff, MP_QSTR_https_colon_ >> 8, 
    0xd9, 
    0x44, 0x0d, 0x80, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_ussl & 0xff, MP_QSTR_ussl >> 8, 
    0xcb, 
    0x22, 0x83, 0x3b, 
    0xca, 
    0x42, 0x0a, 0x80, 
    0x12, MP_QSTR_ValueError & 0xff, MP_QSTR_ValueError >> 8, 
    0x23, 0x06, 
    0xb6, 
    0xf2, 
    0x34, 0x01, 
    0x65, 
    0x10, MP_QSTR__colon_ & 0xff, MP_QSTR__colon_ >> 8, 
    0xb8, 
    0xdd, 
    0x44, 0x15, 0x80, 
    0xb8, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x10, MP_QSTR__colon_ & 0xff, MP_QSTR__colon_ >> 8, 
    0x81, 
    0x36, 0x02, 
    0x30, 0x02, 
    0xc8, 
    0xca, 
    0x12, MP_QSTR_int & 0xff, MP_QSTR_int >> 8, 
    0xba, 
    0x34, 0x01, 
    0xca, 
    0x12, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x14, MP_QSTR_getaddrinfo & 0xff, MP_QSTR_getaddrinfo >> 8, 
    0xb8, 
    0xba, 
    0x80, 
    0x12, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x13, MP_QSTR_SOCK_STREAM & 0xff, MP_QSTR_SOCK_STREAM >> 8, 
    0x36, 0x04, 
    0xcc, 
    0xbc, 
    0x80, 
    0x55, 
    0xcc, 
    0x12, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x14, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0xbc, 
    0x80, 
    0x55, 
    0xbc, 
    0x81, 
    0x55, 
    0xbc, 
    0x82, 
    0x55, 
    0x36, 0x03, 
    0xcd, 
    0x48, 0x66, 0x01, 
    0xbd, 
    0x14, MP_QSTR_connect & 0xff, MP_QSTR_connect >> 8, 
    0xbc, 
    0x7f, 
    0x55, 
    0x36, 0x01, 
    0x59, 
    0xb6, 
    0x10, MP_QSTR_https_colon_ & 0xff, MP_QSTR_https_colon_ >> 8, 
    0xd9, 
    0x44, 0x0d, 0x80, 
    0xbb, 
    0x14, MP_QSTR_wrap_socket & 0xff, MP_QSTR_wrap_socket >> 8, 
    0xbd, 
    0x10, MP_QSTR_server_hostname & 0xff, MP_QSTR_server_hostname >> 8, 
    0xb8, 
    0x36, 0x82, 0x01, 
    0xcd, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x07, 
    0xb0, 
    0xb9, 
    0x2a, 0x02, 
    0xf8, 
    0x36, 0x01, 
    0x59, 
    0x10, MP_QSTR_Host & 0xff, MP_QSTR_Host >> 8, 
    0xb4, 
    0xdd, 
    0x43, 0x0b, 0x80, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x08, 
    0xb8, 
    0xf8, 
    0x36, 0x01, 
    0x59, 
    0xb4, 
    0x5f, 
    0x4b, 0x28, 0x00, 
    0xce, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xbe, 
    0x36, 0x01, 
    0x59, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x09, 
    0x36, 0x01, 
    0x59, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb4, 
    0xbe, 
    0x55, 
    0x36, 0x01, 
    0x59, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x0a, 
    0x36, 0x01, 
    0x59, 
    0x42, 0xd5, 0x7f, 
    0xb3, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x21, 0x80, 
    0xb2, 
    0x51, 
    0xde, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_ujson & 0xff, MP_QSTR_ujson >> 8, 
    0xcf, 
    0xbf, 
    0x14, MP_QSTR_dumps & 0xff, MP_QSTR_dumps >> 8, 
    0xb3, 
    0x36, 0x01, 
    0xc2, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x0b, 
    0x36, 0x01, 
    0x59, 
    0xb2, 
    0x44, 0x10, 0x80, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x0c, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb2, 
    0x34, 0x01, 
    0xf8, 
    0x36, 0x01, 
    0x59, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x0d, 
    0x36, 0x01, 
    0x59, 
    0xb2, 
    0x44, 0x08, 0x80, 
    0xbd, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb2, 
    0x36, 0x01, 
    0x59, 
    0xbd, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0x26, 0x10, 
    0x24, 0x10, 
    0x14, MP_QSTR_split & 0xff, MP_QSTR_split >> 8, 
    0x51, 
    0x82, 
    0x36, 0x02, 
    0x26, 0x10, 
    0x12, MP_QSTR_int & 0xff, MP_QSTR_int >> 8, 
    0x24, 0x10, 
    0x81, 
    0x55, 
    0x34, 0x01, 
    0x26, 0x11, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x26, 0x12, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0x24, 0x10, 
    0x34, 0x01, 
    0x82, 
    0xd8, 
    0x44, 0x0b, 0x80, 
    0x24, 0x10, 
    0x82, 
    0x55, 
    0x14, MP_QSTR_rstrip & 0xff, MP_QSTR_rstrip >> 8, 
    0x36, 0x00, 
    0x26, 0x12, 
    0xbd, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0x26, 0x10, 
    0x24, 0x10, 
    0x44, 0x08, 0x80, 
    0x24, 0x10, 
    0x23, 0x0e, 
    0xd9, 
    0x44, 0x03, 0x80, 
    0x42, 0x53, 0x80, 
    0x24, 0x10, 
    0x14, MP_QSTR_startswith & 0xff, MP_QSTR_startswith >> 8, 
    0x23, 0x0f, 
    0x36, 0x01, 
    0x44, 0x16, 0x80, 
    0x23, 0x10, 
    0x24, 0x10, 
    0xdd, 
    0x44, 0x0b, 0x80, 
    0x12, MP_QSTR_ValueError & 0xff, MP_QSTR_ValueError >> 8, 
    0x23, 0x11, 
    0x24, 0x10, 
    0xf2, 
    0x34, 0x01, 
    0x65, 
    0x42, 0x2e, 0x80, 
    0x24, 0x10, 
    0x14, MP_QSTR_startswith & 0xff, MP_QSTR_startswith >> 8, 
    0x23, 0x12, 
    0x36, 0x01, 
    0x44, 0x22, 0x80, 
    0x22, 0x81, 0x48, 
    0x24, 0x11, 
    0x57, 
    0x5b, 
    0xda, 
    0x46, 0x07, 0x80, 
    0x22, 0x82, 0x2b, 
    0xda, 
    0x42, 0x02, 0x80, 
    0x5a, 
    0x59, 
    0x43, 0x0b, 0x80, 
    0x12, MP_QSTR_NotImplementedError & 0xff, MP_QSTR_NotImplementedError >> 8, 
    0x23, 0x13, 
    0x34, 0x01, 
    0x65, 
    0x42, 0x00, 0x80, 
    0x42, 0x95, 0x7f, 
    0x4a, 0x15, 0x00, 
    0x57, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x0c, 0x80, 
    0x59, 
    0xbd, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x64, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x12, MP_QSTR_Response & 0xff, MP_QSTR_Response >> 8, 
    0xbd, 
    0x34, 0x01, 
    0x26, 0x13, 
    0x24, 0x11, 
    0x24, 0x13, 
    0x18, MP_QSTR_status_code & 0xff, MP_QSTR_status_code >> 8, 
    0x24, 0x12, 
    0x24, 0x13, 
    0x18, MP_QSTR_reason & 0xff, MP_QSTR_reason >> 8, 
    0x24, 0x13, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_0 = {{&mp_type_str}, 38204, 22, (const byte*)"\x55\x6e\x73\x75\x70\x70\x6f\x72\x74\x65\x64\x20\x70\x72\x6f\x74\x6f\x63\x6f\x6c\x3a\x20"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_1 = {{&mp_type_bytes}, 47477, 17, (const byte*)"\x25\x73\x20\x2f\x25\x73\x20\x48\x54\x54\x50\x2f\x31\x2e\x30\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_2 = {{&mp_type_bytes}, 35534, 10, (const byte*)"\x48\x6f\x73\x74\x3a\x20\x25\x73\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_3 = {{&mp_type_bytes}, 26975, 2, (const byte*)"\x3a\x20"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_4 = {{&mp_type_bytes}, 27298, 2, (const byte*)"\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_5 = {{&mp_type_bytes}, 14729, 32, (const byte*)"\x43\x6f\x6e\x74\x65\x6e\x74\x2d\x54\x79\x70\x65\x3a\x20\x61\x70\x70\x6c\x69\x63\x61\x74\x69\x6f\x6e\x2f\x6a\x73\x6f\x6e\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_6 = {{&mp_type_bytes}, 8065, 20, (const byte*)"\x43\x6f\x6e\x74\x65\x6e\x74\x2d\x4c\x65\x6e\x67\x74\x68\x3a\x20\x25\x64\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_7 = {{&mp_type_bytes}, 27298, 2, (const byte*)"\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_8 = {{&mp_type_bytes}, 27298, 2, (const byte*)"\x0d\x0a"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_9 = {{&mp_type_bytes}, 59642, 18, (const byte*)"\x54\x72\x61\x6e\x73\x66\x65\x72\x2d\x45\x6e\x63\x6f\x64\x69\x6e\x67\x3a"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_10 = {{&mp_type_bytes}, 3583, 7, (const byte*)"\x63\x68\x75\x6e\x6b\x65\x64"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_11 = {{&mp_type_str}, 18832, 12, (const byte*)"\x55\x6e\x73\x75\x70\x70\x6f\x72\x74\x65\x64\x20"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_12 = {{&mp_type_bytes}, 1090, 9, (const byte*)"\x4c\x6f\x63\x61\x74\x69\x6f\x6e\x3a"};
STATIC const mp_obj_str_t const_obj_urequests__lt_module_gt__request_13 = {{&mp_type_str}, 223, 27, (const byte*)"\x52\x65\x64\x69\x72\x65\x63\x74\x73\x20\x6e\x6f\x74\x20\x79\x65\x74\x20\x73\x75\x70\x70\x6f\x72\x74\x65\x64"};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__request[20] = {
    MP_ROM_QSTR(MP_QSTR_method),
    MP_ROM_QSTR(MP_QSTR_url),
    MP_ROM_QSTR(MP_QSTR_data),
    MP_ROM_QSTR(MP_QSTR_json),
    MP_ROM_QSTR(MP_QSTR_headers),
    MP_ROM_QSTR(MP_QSTR_stream),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_0),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_1),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_2),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_3),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_4),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_5),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_6),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_7),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_8),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_9),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_10),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_11),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_12),
    MP_ROM_PTR(&const_obj_urequests__lt_module_gt__request_13),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__request = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 6,
    .fun_data = fun_data_urequests__lt_module_gt__request,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__request,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 648,
    .n_obj = 14,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 28,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 6,
        .n_kwonly_args = 0,
        .n_def_pos_args = 4,
        .qstr_block_name = MP_QSTR_request,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__request + 10,
        .opcodes = fun_data_urequests__lt_module_gt__request + 72,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__head
STATIC const byte fun_data_urequests__lt_module_gt__head[23] = {
    0xb1, 0x80, 0x40, 0x0e,
    MP_QSTR_head & 0xff, MP_QSTR_head >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x6f, 0x00,
    0x12, MP_QSTR_request & 0xff, MP_QSTR_request >> 8, 
    0x10, MP_QSTR_HEAD & 0xff, MP_QSTR_HEAD >> 8, 
    0xb0, 
    0x53, 
    0xb1, 
    0x35, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__head[1] = {
    MP_ROM_QSTR(MP_QSTR_url),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__head = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x02,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__head,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__head,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 2,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_head,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__head + 8,
        .opcodes = fun_data_urequests__lt_module_gt__head + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__get
STATIC const byte fun_data_urequests__lt_module_gt__get[23] = {
    0xb1, 0x80, 0x40, 0x0e,
    MP_QSTR_get & 0xff, MP_QSTR_get >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x73, 0x00,
    0x12, MP_QSTR_request & 0xff, MP_QSTR_request >> 8, 
    0x10, MP_QSTR_GET & 0xff, MP_QSTR_GET >> 8, 
    0xb0, 
    0x53, 
    0xb1, 
    0x35, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__get[1] = {
    MP_ROM_QSTR(MP_QSTR_url),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__get = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x02,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__get,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__get,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 2,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_get,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__get + 8,
        .opcodes = fun_data_urequests__lt_module_gt__get + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__post
STATIC const byte fun_data_urequests__lt_module_gt__post[23] = {
    0xb1, 0x80, 0x40, 0x0e,
    MP_QSTR_post & 0xff, MP_QSTR_post >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x77, 0x00,
    0x12, MP_QSTR_request & 0xff, MP_QSTR_request >> 8, 
    0x10, MP_QSTR_POST & 0xff, MP_QSTR_POST >> 8, 
    0xb0, 
    0x53, 
    0xb1, 
    0x35, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__post[1] = {
    MP_ROM_QSTR(MP_QSTR_url),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__post = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x02,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__post,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__post,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 2,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_post,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__post + 8,
        .opcodes = fun_data_urequests__lt_module_gt__post + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__put
STATIC const byte fun_data_urequests__lt_module_gt__put[23] = {
    0xb1, 0x80, 0x40, 0x0e,
    MP_QSTR_put & 0xff, MP_QSTR_put >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x7b, 0x00,
    0x12, MP_QSTR_request & 0xff, MP_QSTR_request >> 8, 
    0x10, MP_QSTR_PUT & 0xff, MP_QSTR_PUT >> 8, 
    0xb0, 
    0x53, 
    0xb1, 
    0x35, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__put[1] = {
    MP_ROM_QSTR(MP_QSTR_url),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__put = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x02,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__put,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__put,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 2,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_put,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__put + 8,
        .opcodes = fun_data_urequests__lt_module_gt__put + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__patch
STATIC const byte fun_data_urequests__lt_module_gt__patch[23] = {
    0xb1, 0x80, 0x40, 0x0e,
    MP_QSTR_patch & 0xff, MP_QSTR_patch >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x7f, 0x00,
    0x12, MP_QSTR_request & 0xff, MP_QSTR_request >> 8, 
    0x10, MP_QSTR_PATCH & 0xff, MP_QSTR_PATCH >> 8, 
    0xb0, 
    0x53, 
    0xb1, 
    0x35, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__patch[1] = {
    MP_ROM_QSTR(MP_QSTR_url),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__patch = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x02,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__patch,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__patch,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 2,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_patch,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__patch + 8,
        .opcodes = fun_data_urequests__lt_module_gt__patch + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests__lt_module_gt__delete
STATIC const byte fun_data_urequests__lt_module_gt__delete[23] = {
    0xb1, 0x80, 0x40, 0x0e,
    MP_QSTR_delete & 0xff, MP_QSTR_delete >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x80, 0x83, 0x00,
    0x12, MP_QSTR_request & 0xff, MP_QSTR_request >> 8, 
    0x10, MP_QSTR_DELETE & 0xff, MP_QSTR_DELETE >> 8, 
    0xb0, 
    0x53, 
    0xb1, 
    0x35, 0x02, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt__delete[1] = {
    MP_ROM_QSTR(MP_QSTR_url),
};
STATIC const mp_raw_code_t raw_code_urequests__lt_module_gt__delete = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x02,
    .n_pos_args = 1,
    .fun_data = fun_data_urequests__lt_module_gt__delete,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt__delete,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 2,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_delete,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt__delete + 8,
        .opcodes = fun_data_urequests__lt_module_gt__delete + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file urequests.py, scope urequests_<module>
STATIC const byte fun_data_urequests__lt_module_gt_[86] = {
    0x18, 0x28,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_urequests_dot_py & 0xff, MP_QSTR_urequests_dot_py >> 8,
    0x68, 0x8b, 0x20, 0x8d, 0x4b, 0x65, 0x20, 0x65, 0x20, 0x65, 0x20, 0x65, 0x20, 0x65, 0x20, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x16, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_Response & 0xff, MP_QSTR_Response >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_Response & 0xff, MP_QSTR_Response >> 8, 
    0x51, 
    0x51, 
    0x2c, 0x00, 
    0x51, 
    0x2a, 0x04, 
    0x53, 
    0x33, 0x01, 
    0x16, MP_QSTR_request & 0xff, MP_QSTR_request >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR_head & 0xff, MP_QSTR_head >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_get & 0xff, MP_QSTR_get >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_post & 0xff, MP_QSTR_post >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_put & 0xff, MP_QSTR_put >> 8, 
    0x32, 0x06, 
    0x16, MP_QSTR_patch & 0xff, MP_QSTR_patch >> 8, 
    0x32, 0x07, 
    0x16, MP_QSTR_delete & 0xff, MP_QSTR_delete >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_urequests__lt_module_gt_[8] = {
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__Response),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__request),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__head),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__get),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__post),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__put),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__patch),
    MP_ROM_PTR(&raw_code_urequests__lt_module_gt__delete),
};
const mp_raw_code_t raw_code_urequests__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_urequests__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_urequests__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 86,
    .n_obj = 0,
    .n_raw_code = 8,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_urequests_dot_py,
        .line_info = fun_data_urequests__lt_module_gt_ + 6,
        .opcodes = fun_data_urequests__lt_module_gt_ + 22,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__LS___repr__
STATIC const byte fun_data_upysh__lt_module_gt__LS___repr__[21] = {
    0x11, 0x10,
    MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x60, 0x60, 0x27, 0x00,
    0xb0, 
    0x14, MP_QSTR___call__ & 0xff, MP_QSTR___call__ >> 8, 
    0x36, 0x00, 
    0x59, 
    0x10, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__LS___repr__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__LS___repr__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upysh__lt_module_gt__LS___repr__,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__LS___repr__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 21,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___repr__,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__LS___repr__ + 6,
        .opcodes = fun_data_upysh__lt_module_gt__LS___repr__ + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__LS___call__
STATIC const byte fun_data_upysh__lt_module_gt__LS___call__[101] = {
    0xea, 0x01, 0x1a,
    MP_QSTR___call__ & 0xff, MP_QSTR___call__ >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x80, 0x0a, 0x2a, 0x27, 0x26, 0x31, 0x2b, 0x4d, 0x00,
    0x12, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x14, MP_QSTR_listdir & 0xff, MP_QSTR_listdir >> 8, 
    0xb1, 
    0x36, 0x01, 
    0xc2, 
    0xb2, 
    0x14, MP_QSTR_sort & 0xff, MP_QSTR_sort >> 8, 
    0x36, 0x00, 
    0x59, 
    0xb2, 
    0x5f, 
    0x4b, 0x3d, 0x00, 
    0xc3, 
    0x12, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x14, MP_QSTR_stat & 0xff, MP_QSTR_stat >> 8, 
    0x10, MP_QSTR__percent_s_slash__percent_s & 0xff, MP_QSTR__percent_s_slash__percent_s >> 8, 
    0xb1, 
    0xb3, 
    0x2a, 0x02, 
    0xf8, 
    0x36, 0x01, 
    0xc4, 
    0xb4, 
    0x80, 
    0x55, 
    0x22, 0x81, 0x80, 0x00, 
    0xef, 
    0x44, 0x0d, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x02, 
    0xb3, 
    0xf8, 
    0x34, 0x01, 
    0x59, 
    0x42, 0x10, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x10, MP_QSTR__percent__space_8d_space__percent_s & 0xff, MP_QSTR__percent__space_8d_space__percent_s >> 8, 
    0xb4, 
    0x86, 
    0x55, 
    0xb3, 
    0x2a, 0x02, 
    0xf8, 
    0x34, 0x01, 
    0x59, 
    0x42, 0xc0, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upysh__lt_module_gt__LS___call___0 = {{&mp_type_str}, 29390, 11, (const byte*)"\x20\x20\x20\x3c\x64\x69\x72\x3e\x20\x25\x73"};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__LS___call__[3] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_path),
    MP_ROM_PTR(&const_obj_upysh__lt_module_gt__LS___call___0),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__LS___call__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upysh__lt_module_gt__LS___call__,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__LS___call__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 101,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 14,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR___call__,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__LS___call__ + 7,
        .opcodes = fun_data_upysh__lt_module_gt__LS___call__ + 16,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__LS
STATIC const byte fun_data_upysh__lt_module_gt__LS[41] = {
    0x08, 0x12,
    MP_QSTR_LS & 0xff, MP_QSTR_LS >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x6c, 0x40, 0x65, 0x20, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_LS & 0xff, MP_QSTR_LS >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8, 
    0x10, MP_QSTR__dot_ & 0xff, MP_QSTR__dot_ >> 8, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x01, 
    0x16, MP_QSTR___call__ & 0xff, MP_QSTR___call__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__LS[2] = {
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__LS___repr__),
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__LS___call__),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__LS = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upysh__lt_module_gt__LS,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__LS,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 41,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_LS,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__LS + 6,
        .opcodes = fun_data_upysh__lt_module_gt__LS + 11,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__PWD___repr__
STATIC const byte fun_data_upysh__lt_module_gt__PWD___repr__[18] = {
    0x11, 0x0e,
    MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x80, 0x16, 0x00,
    0x12, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x14, MP_QSTR_getcwd & 0xff, MP_QSTR_getcwd >> 8, 
    0x36, 0x00, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__PWD___repr__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__PWD___repr__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upysh__lt_module_gt__PWD___repr__,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__PWD___repr__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 18,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___repr__,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__PWD___repr__ + 6,
        .opcodes = fun_data_upysh__lt_module_gt__PWD___repr__ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__PWD___call__
STATIC const byte fun_data_upysh__lt_module_gt__PWD___call__[16] = {
    0x11, 0x0e,
    MP_QSTR___call__ & 0xff, MP_QSTR___call__ >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x80, 0x19, 0x00,
    0xb0, 
    0x14, MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8, 
    0x36, 0x00, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__PWD___call__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__PWD___call__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upysh__lt_module_gt__PWD___call__,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__PWD___call__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 16,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___call__,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__PWD___call__ + 6,
        .opcodes = fun_data_upysh__lt_module_gt__PWD___call__ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__PWD
STATIC const byte fun_data_upysh__lt_module_gt__PWD[34] = {
    0x00, 0x10,
    MP_QSTR_PWD & 0xff, MP_QSTR_PWD >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x8c, 0x15, 0x65, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_PWD & 0xff, MP_QSTR_PWD >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR___call__ & 0xff, MP_QSTR___call__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__PWD[2] = {
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__PWD___repr__),
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__PWD___call__),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__PWD = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upysh__lt_module_gt__PWD,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__PWD,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 34,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_PWD,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__PWD + 6,
        .opcodes = fun_data_upysh__lt_module_gt__PWD + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__CLEAR___repr__
STATIC const byte fun_data_upysh__lt_module_gt__CLEAR___repr__[13] = {
    0x09, 0x0e,
    MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x80, 0x1e, 0x00,
    0x10, MP_QSTR__0x1b__bracket_open_2J_0x1b__bracket_open_H & 0xff, MP_QSTR__0x1b__bracket_open_2J_0x1b__bracket_open_H >> 8, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__CLEAR___repr__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__CLEAR___repr__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upysh__lt_module_gt__CLEAR___repr__,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__CLEAR___repr__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 13,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___repr__,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__CLEAR___repr__ + 6,
        .opcodes = fun_data_upysh__lt_module_gt__CLEAR___repr__ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__CLEAR___call__
STATIC const byte fun_data_upysh__lt_module_gt__CLEAR___call__[16] = {
    0x11, 0x0e,
    MP_QSTR___call__ & 0xff, MP_QSTR___call__ >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x80, 0x21, 0x00,
    0xb0, 
    0x14, MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8, 
    0x36, 0x00, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__CLEAR___call__[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__CLEAR___call__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upysh__lt_module_gt__CLEAR___call__,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__CLEAR___call__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 16,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___call__,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__CLEAR___call__ + 6,
        .opcodes = fun_data_upysh__lt_module_gt__CLEAR___call__ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__CLEAR
STATIC const byte fun_data_upysh__lt_module_gt__CLEAR[34] = {
    0x00, 0x10,
    MP_QSTR_CLEAR & 0xff, MP_QSTR_CLEAR >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x8c, 0x1d, 0x65, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_CLEAR & 0xff, MP_QSTR_CLEAR >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR___call__ & 0xff, MP_QSTR___call__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__CLEAR[2] = {
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__CLEAR___repr__),
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__CLEAR___call__),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__CLEAR = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upysh__lt_module_gt__CLEAR,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__CLEAR,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 34,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_CLEAR,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__CLEAR + 6,
        .opcodes = fun_data_upysh__lt_module_gt__CLEAR + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__head
STATIC const byte fun_data_upysh__lt_module_gt__head[74] = {
    0xd6, 0x01, 0x18,
    MP_QSTR_head & 0xff, MP_QSTR_head >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x80, 0x30, 0x2a, 0x27, 0x27, 0x24, 0x23, 0x00,
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0xb0, 
    0x34, 0x01, 
    0x47, 0x2e, 0x00, 
    0xc0, 
    0xb1, 
    0x80, 
    0x42, 0x1f, 0x80, 
    0x57, 
    0xc2, 
    0xb0, 
    0x14, MP_QSTR_readline & 0xff, MP_QSTR_readline >> 8, 
    0x36, 0x00, 
    0xc3, 
    0xb3, 
    0x43, 0x03, 0x80, 
    0x42, 0x15, 0x80, 
    0x12, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x13, MP_QSTR_stdout & 0xff, MP_QSTR_stdout >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb3, 
    0x36, 0x01, 
    0x59, 
    0x81, 
    0xe5, 
    0x58, 
    0x5a, 
    0xd7, 
    0x43, 0xdb, 0x7f, 
    0x59, 
    0x59, 
    0x51, 
    0x5c, 
    0x5d, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__head[2] = {
    MP_ROM_QSTR(MP_QSTR_f),
    MP_ROM_QSTR(MP_QSTR_n),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__head = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_upysh__lt_module_gt__head,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__head,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 74,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 1,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_head,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__head + 7,
        .opcodes = fun_data_upysh__lt_module_gt__head + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__cat
STATIC const byte fun_data_upysh__lt_module_gt__cat[20] = {
    0x19, 0x0e,
    MP_QSTR_cat & 0xff, MP_QSTR_cat >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x80, 0x39, 0x00,
    0x12, MP_QSTR_head & 0xff, MP_QSTR_head >> 8, 
    0xb0, 
    0x23, 0x01, 
    0x34, 0x02, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_int_t const_obj_upysh__lt_module_gt__cat_0 = {{&mp_type_int}, {.neg=0, .fixed_dig=1, .alloc=2, .len=2, .dig=(uint16_t*)(const uint16_t[]){0x0,0x4000}}};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__cat[2] = {
    MP_ROM_QSTR(MP_QSTR_f),
    MP_ROM_PTR(&const_obj_upysh__lt_module_gt__cat_0),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__cat = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upysh__lt_module_gt__cat,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__cat,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 20,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_cat,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__cat + 6,
        .opcodes = fun_data_upysh__lt_module_gt__cat + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__newfile
STATIC const byte fun_data_upysh__lt_module_gt__newfile[93] = {
    0xd1, 0x02, 0x1c,
    MP_QSTR_newfile & 0xff, MP_QSTR_newfile >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x80, 0x3d, 0x28, 0x2d, 0x20, 0x23, 0x52, 0x28, 0x28, 0x00,
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x01, 
    0x34, 0x01, 
    0x59, 
    0x12, MP_QSTR_open & 0xff, MP_QSTR_open >> 8, 
    0xb0, 
    0x10, MP_QSTR_w & 0xff, MP_QSTR_w >> 8, 
    0x34, 0x02, 
    0x47, 0x34, 0x00, 
    0xc1, 
    0x48, 0x09, 0x00, 
    0x12, MP_QSTR_input & 0xff, MP_QSTR_input >> 8, 
    0x34, 0x00, 
    0xc2, 
    0x4a, 0x11, 0x00, 
    0x57, 
    0x12, MP_QSTR_EOFError & 0xff, MP_QSTR_EOFError >> 8, 
    0xdf, 
    0x44, 0x08, 0x80, 
    0x59, 
    0x40, 0x1a, 0x80, 0x01, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0xb1, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb2, 
    0x36, 0x01, 
    0x59, 
    0xb1, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x10, MP_QSTR__0x0a_ & 0xff, MP_QSTR__0x0a_ >> 8, 
    0x36, 0x01, 
    0x59, 
    0x42, 0xce, 0x7f, 
    0x51, 
    0x5c, 
    0x5d, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upysh__lt_module_gt__newfile_0 = {{&mp_type_str}, 25826, 58, (const byte*)"\x54\x79\x70\x65\x20\x66\x69\x6c\x65\x20\x63\x6f\x6e\x74\x65\x6e\x74\x73\x20\x6c\x69\x6e\x65\x20\x62\x79\x20\x6c\x69\x6e\x65\x2c\x20\x66\x69\x6e\x69\x73\x68\x20\x77\x69\x74\x68\x20\x45\x4f\x46\x20\x28\x43\x74\x72\x6c\x2b\x44\x29\x2e"};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__newfile[2] = {
    MP_ROM_QSTR(MP_QSTR_path),
    MP_ROM_PTR(&const_obj_upysh__lt_module_gt__newfile_0),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__newfile = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upysh__lt_module_gt__newfile,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__newfile,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 93,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_newfile,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__newfile + 7,
        .opcodes = fun_data_upysh__lt_module_gt__newfile + 17,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__Man___repr__
STATIC const byte fun_data_upysh__lt_module_gt__Man___repr__[12] = {
    0x09, 0x0e,
    MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x80, 0x4a, 0x00,
    0x23, 0x01, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_upysh__lt_module_gt__Man___repr___0 = {{&mp_type_str}, 51500, 244, (const byte*)"\x0a\x75\x70\x79\x73\x68\x20\x69\x73\x20\x69\x6e\x74\x65\x6e\x64\x65\x64\x20\x74\x6f\x20\x62\x65\x20\x69\x6d\x70\x6f\x72\x74\x65\x64\x20\x75\x73\x69\x6e\x67\x3a\x0a\x66\x72\x6f\x6d\x20\x75\x70\x79\x73\x68\x20\x69\x6d\x70\x6f\x72\x74\x20\x2a\x0a\x0a\x54\x6f\x20\x73\x65\x65\x20\x74\x68\x69\x73\x20\x68\x65\x6c\x70\x20\x74\x65\x78\x74\x20\x61\x67\x61\x69\x6e\x2c\x20\x74\x79\x70\x65\x20\x22\x6d\x61\x6e\x22\x2e\x0a\x0a\x75\x70\x79\x73\x68\x20\x63\x6f\x6d\x6d\x61\x6e\x64\x73\x3a\x0a\x70\x77\x64\x2c\x20\x63\x64\x28\x22\x6e\x65\x77\x5f\x64\x69\x72\x22\x29\x2c\x20\x6c\x73\x2c\x20\x6c\x73\x28\x2e\x2e\x2e\x29\x2c\x20\x68\x65\x61\x64\x28\x2e\x2e\x2e\x29\x2c\x20\x63\x61\x74\x28\x2e\x2e\x2e\x29\x0a\x6e\x65\x77\x66\x69\x6c\x65\x28\x2e\x2e\x2e\x29\x2c\x20\x6d\x76\x28\x22\x6f\x6c\x64\x22\x2c\x20\x22\x6e\x65\x77\x22\x29\x2c\x20\x72\x6d\x28\x2e\x2e\x2e\x29\x2c\x20\x6d\x6b\x64\x69\x72\x28\x2e\x2e\x2e\x29\x2c\x20\x72\x6d\x64\x69\x72\x28\x2e\x2e\x2e\x29\x2c\x0a\x63\x6c\x65\x61\x72\x0a"};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__Man___repr__[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&const_obj_upysh__lt_module_gt__Man___repr___0),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__Man___repr__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_upysh__lt_module_gt__Man___repr__,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__Man___repr__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 12,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 2,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR___repr__,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__Man___repr__ + 6,
        .opcodes = fun_data_upysh__lt_module_gt__Man___repr__ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh__lt_module_gt__Man
STATIC const byte fun_data_upysh__lt_module_gt__Man[28] = {
    0x00, 0x0e,
    MP_QSTR_Man & 0xff, MP_QSTR_Man >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x8c, 0x49, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_Man & 0xff, MP_QSTR_Man >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR___repr__ & 0xff, MP_QSTR___repr__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt__Man[1] = {
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__Man___repr__),
};
STATIC const mp_raw_code_t raw_code_upysh__lt_module_gt__Man = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upysh__lt_module_gt__Man,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt__Man,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 28,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_Man,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt__Man + 6,
        .opcodes = fun_data_upysh__lt_module_gt__Man + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file upysh.py, scope upysh_<module>
STATIC const byte fun_data_upysh__lt_module_gt_[199] = {
    0x10, 0x3c,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_upysh_dot_py & 0xff, MP_QSTR_upysh_dot_py >> 8,
    0x28, 0x68, 0x8b, 0x10, 0x8b, 0x08, 0x8b, 0x08, 0x28, 0x28, 0x48, 0x29, 0x29, 0x29, 0x29, 0x69, 0x89, 0x09, 0x65, 0x20, 0x85, 0x0c, 0x8b, 0x0f, 0x48, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x16, MP_QSTR_sys & 0xff, MP_QSTR_sys >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x16, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_LS & 0xff, MP_QSTR_LS >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_LS & 0xff, MP_QSTR_LS >> 8, 
    0x54, 
    0x32, 0x01, 
    0x10, MP_QSTR_PWD & 0xff, MP_QSTR_PWD >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_PWD & 0xff, MP_QSTR_PWD >> 8, 
    0x54, 
    0x32, 0x02, 
    0x10, MP_QSTR_CLEAR & 0xff, MP_QSTR_CLEAR >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_CLEAR & 0xff, MP_QSTR_CLEAR >> 8, 
    0x11, MP_QSTR_PWD & 0xff, MP_QSTR_PWD >> 8, 
    0x34, 0x00, 
    0x16, MP_QSTR_pwd & 0xff, MP_QSTR_pwd >> 8, 
    0x11, MP_QSTR_LS & 0xff, MP_QSTR_LS >> 8, 
    0x34, 0x00, 
    0x16, MP_QSTR_ls & 0xff, MP_QSTR_ls >> 8, 
    0x11, MP_QSTR_CLEAR & 0xff, MP_QSTR_CLEAR >> 8, 
    0x34, 0x00, 
    0x16, MP_QSTR_clear & 0xff, MP_QSTR_clear >> 8, 
    0x11, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x13, MP_QSTR_chdir & 0xff, MP_QSTR_chdir >> 8, 
    0x16, MP_QSTR_cd & 0xff, MP_QSTR_cd >> 8, 
    0x11, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x13, MP_QSTR_mkdir & 0xff, MP_QSTR_mkdir >> 8, 
    0x16, MP_QSTR_mkdir & 0xff, MP_QSTR_mkdir >> 8, 
    0x11, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x13, MP_QSTR_rename & 0xff, MP_QSTR_rename >> 8, 
    0x16, MP_QSTR_mv & 0xff, MP_QSTR_mv >> 8, 
    0x11, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x13, MP_QSTR_remove & 0xff, MP_QSTR_remove >> 8, 
    0x16, MP_QSTR_rm & 0xff, MP_QSTR_rm >> 8, 
    0x11, MP_QSTR_os & 0xff, MP_QSTR_os >> 8, 
    0x13, MP_QSTR_rmdir & 0xff, MP_QSTR_rmdir >> 8, 
    0x16, MP_QSTR_rmdir & 0xff, MP_QSTR_rmdir >> 8, 
    0x8a, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x03, 
    0x16, MP_QSTR_head & 0xff, MP_QSTR_head >> 8, 
    0x32, 0x04, 
    0x16, MP_QSTR_cat & 0xff, MP_QSTR_cat >> 8, 
    0x32, 0x05, 
    0x16, MP_QSTR_newfile & 0xff, MP_QSTR_newfile >> 8, 
    0x54, 
    0x32, 0x06, 
    0x10, MP_QSTR_Man & 0xff, MP_QSTR_Man >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_Man & 0xff, MP_QSTR_Man >> 8, 
    0x11, MP_QSTR_Man & 0xff, MP_QSTR_Man >> 8, 
    0x34, 0x00, 
    0x16, MP_QSTR_man & 0xff, MP_QSTR_man >> 8, 
    0x11, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x11, MP_QSTR_man & 0xff, MP_QSTR_man >> 8, 
    0x34, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_upysh__lt_module_gt_[7] = {
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__LS),
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__PWD),
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__CLEAR),
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__head),
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__cat),
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__newfile),
    MP_ROM_PTR(&raw_code_upysh__lt_module_gt__Man),
};
const mp_raw_code_t raw_code_upysh__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_upysh__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_upysh__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 199,
    .n_obj = 0,
    .n_raw_code = 7,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 3,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_upysh_dot_py,
        .line_info = fun_data_upysh__lt_module_gt_ + 6,
        .opcodes = fun_data_upysh__lt_module_gt_ + 32,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTException
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTException[23] = {
    0x00, 0x0e,
    MP_QSTR_MQTTException & 0xff, MP_QSTR_MQTTException >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x6c, 0x60, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_MQTTException & 0xff, MP_QSTR_MQTTException >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTException = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTException,
    .const_table = NULL,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 1,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_MQTTException,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTException + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTException + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient___init__
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient___init__[126] = {
    0xd1, 0x80, 0x85, 0x01, 0x2e,
    MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x15, 0x26, 0x2f, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x00,
    0xb3, 
    0x80, 
    0xd9, 
    0x44, 0x0f, 0x80, 
    0xb7, 
    0x44, 0x07, 0x80, 
    0x22, 0x80, 0xc5, 0x33, 
    0x42, 0x03, 0x80, 
    0x22, 0x8e, 0x5b, 
    0xc3, 
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_client_id & 0xff, MP_QSTR_client_id >> 8, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0xb2, 
    0xb0, 
    0x18, MP_QSTR_server & 0xff, MP_QSTR_server >> 8, 
    0xb3, 
    0xb0, 
    0x18, MP_QSTR_port & 0xff, MP_QSTR_port >> 8, 
    0xb7, 
    0xb0, 
    0x18, MP_QSTR_ssl & 0xff, MP_QSTR_ssl >> 8, 
    0xb8, 
    0xb0, 
    0x18, MP_QSTR_ssl_params & 0xff, MP_QSTR_ssl_params >> 8, 
    0x80, 
    0xb0, 
    0x18, MP_QSTR_pid & 0xff, MP_QSTR_pid >> 8, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR_cb & 0xff, MP_QSTR_cb >> 8, 
    0xb4, 
    0xb0, 
    0x18, MP_QSTR_user & 0xff, MP_QSTR_user >> 8, 
    0xb5, 
    0xb0, 
    0x18, MP_QSTR_pswd & 0xff, MP_QSTR_pswd >> 8, 
    0xb6, 
    0xb0, 
    0x18, MP_QSTR_keepalive & 0xff, MP_QSTR_keepalive >> 8, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR_lw_topic & 0xff, MP_QSTR_lw_topic >> 8, 
    0x51, 
    0xb0, 
    0x18, MP_QSTR_lw_msg & 0xff, MP_QSTR_lw_msg >> 8, 
    0x80, 
    0xb0, 
    0x18, MP_QSTR_lw_qos & 0xff, MP_QSTR_lw_qos >> 8, 
    0x50, 
    0xb0, 
    0x18, MP_QSTR_lw_retain & 0xff, MP_QSTR_lw_retain >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient___init__[9] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_client_id),
    MP_ROM_QSTR(MP_QSTR_server),
    MP_ROM_QSTR(MP_QSTR_port),
    MP_ROM_QSTR(MP_QSTR_user),
    MP_ROM_QSTR(MP_QSTR_password),
    MP_ROM_QSTR(MP_QSTR_keepalive),
    MP_ROM_QSTR(MP_QSTR_ssl),
    MP_ROM_QSTR(MP_QSTR_ssl_params),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient___init__ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 9,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient___init__,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient___init__,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 126,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 9,
        .n_kwonly_args = 0,
        .n_def_pos_args = 6,
        .qstr_block_name = MP_QSTR___init__,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient___init__ + 9,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient___init__ + 28,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient__send_str
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient__send_str[50] = {
    0x42, 0x10,
    MP_QSTR__send_str & 0xff, MP_QSTR__send_str >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x28, 0x3b, 0x00,
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x12, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x14, MP_QSTR_pack & 0xff, MP_QSTR_pack >> 8, 
    0x10, MP_QSTR__bang_H & 0xff, MP_QSTR__bang_H >> 8, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb1, 
    0x34, 0x01, 
    0x36, 0x02, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient__send_str[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_s),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient__send_str = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient__send_str,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient__send_str,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 50,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 9,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__send_str,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient__send_str + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient__send_str + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient__recv_len
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient__recv_len[62] = {
    0x31, 0x1c,
    MP_QSTR__recv_len & 0xff, MP_QSTR__recv_len >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x2c, 0x22, 0x22, 0x20, 0x2d, 0x2a, 0x28, 0x22, 0x00,
    0x80, 
    0xc1, 
    0x80, 
    0xc2, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x81, 
    0x36, 0x01, 
    0x80, 
    0x55, 
    0xc3, 
    0xb1, 
    0xb3, 
    0x22, 0x80, 0x7f, 
    0xef, 
    0xb2, 
    0xf0, 
    0xe0, 
    0xc1, 
    0xb3, 
    0x22, 0x81, 0x00, 
    0xef, 
    0x43, 0x02, 0x80, 
    0xb1, 
    0x63, 
    0xb2, 
    0x87, 
    0xe5, 
    0xc2, 
    0x42, 0xd8, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient__recv_len[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient__recv_len = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient__recv_len,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient__recv_len,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 62,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 7,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__recv_len,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient__recv_len + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient__recv_len + 16,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient_set_callback
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient_set_callback[16] = {
    0x1a, 0x0e,
    MP_QSTR_set_callback & 0xff, MP_QSTR_set_callback >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x36, 0x00,
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_cb & 0xff, MP_QSTR_cb >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient_set_callback[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_f),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient_set_callback = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient_set_callback,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient_set_callback,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 16,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_set_callback,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient_set_callback + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient_set_callback + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient_set_last_will
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient_set_last_will[68] = {
    0xb9, 0x84, 0x01, 0x18,
    MP_QSTR_set_last_will & 0xff, MP_QSTR_set_last_will >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x39, 0x36, 0x28, 0x25, 0x25, 0x25, 0x00,
    0x80, 
    0xb4, 
    0x57, 
    0x5b, 
    0xda, 
    0x46, 0x05, 0x80, 
    0x82, 
    0xda, 
    0x42, 0x02, 0x80, 
    0x5a, 
    0x59, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0xb1, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0xb1, 
    0xb0, 
    0x18, MP_QSTR_lw_topic & 0xff, MP_QSTR_lw_topic >> 8, 
    0xb2, 
    0xb0, 
    0x18, MP_QSTR_lw_msg & 0xff, MP_QSTR_lw_msg >> 8, 
    0xb4, 
    0xb0, 
    0x18, MP_QSTR_lw_qos & 0xff, MP_QSTR_lw_qos >> 8, 
    0xb3, 
    0xb0, 
    0x18, MP_QSTR_lw_retain & 0xff, MP_QSTR_lw_retain >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient_set_last_will[5] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_topic),
    MP_ROM_QSTR(MP_QSTR_msg),
    MP_ROM_QSTR(MP_QSTR_retain),
    MP_ROM_QSTR(MP_QSTR_qos),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient_set_last_will = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 5,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient_set_last_will,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient_set_last_will,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 68,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 8,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 5,
        .n_kwonly_args = 0,
        .n_def_pos_args = 2,
        .qstr_block_name = MP_QSTR_set_last_will,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient_set_last_will + 8,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient_set_last_will + 16,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient_connect
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient_connect[519] = {
    0xf2, 0x01, 0x5e,
    MP_QSTR_connect & 0xff, MP_QSTR_connect >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x41, 0x2c, 0x35, 0x2b, 0x27, 0x46, 0x33, 0x28, 0x48, 0x2e, 0x26, 0x2a, 0x3a, 0x2a, 0x27, 0x30, 0x2d, 0x2f, 0x27, 0x3a, 0x3a, 0x4d, 0x22, 0x23, 0x2c, 0x24, 0x2c, 0x44, 0x2e, 0x4b, 0x2b, 0x27, 0x2b, 0x2b, 0x2a, 0x2b, 0x2b, 0x2b, 0x34, 0x28, 0x29, 0x00,
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x14, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x36, 0x00, 
    0xb0, 
    0x18, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x12, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x14, MP_QSTR_getaddrinfo & 0xff, MP_QSTR_getaddrinfo >> 8, 
    0xb0, 
    0x13, MP_QSTR_server & 0xff, MP_QSTR_server >> 8, 
    0xb0, 
    0x13, MP_QSTR_port & 0xff, MP_QSTR_port >> 8, 
    0x36, 0x02, 
    0x80, 
    0x55, 
    0x7f, 
    0x55, 
    0xc2, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_connect & 0xff, MP_QSTR_connect >> 8, 
    0xb2, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_ssl & 0xff, MP_QSTR_ssl >> 8, 
    0x44, 0x19, 0x80, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_ussl & 0xff, MP_QSTR_ussl >> 8, 
    0xc3, 
    0xb3, 
    0x14, MP_QSTR_wrap_socket & 0xff, MP_QSTR_wrap_socket >> 8, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x53, 
    0xb0, 
    0x13, MP_QSTR_ssl_params & 0xff, MP_QSTR_ssl_params >> 8, 
    0x37, 0x01, 
    0xb0, 
    0x18, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x23, 0x02, 
    0x34, 0x01, 
    0xc4, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x23, 0x03, 
    0x34, 0x01, 
    0xc5, 
    0x8a, 
    0x82, 
    0xf2, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb0, 
    0x13, MP_QSTR_client_id & 0xff, MP_QSTR_client_id >> 8, 
    0x34, 0x01, 
    0xf2, 
    0xc6, 
    0xb1, 
    0x81, 
    0xf0, 
    0xb5, 
    0x86, 
    0x56, 
    0xb0, 
    0x13, MP_QSTR_user & 0xff, MP_QSTR_user >> 8, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x24, 0x80, 
    0xb6, 
    0x82, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb0, 
    0x13, MP_QSTR_user & 0xff, MP_QSTR_user >> 8, 
    0x34, 0x01, 
    0xf2, 
    0x82, 
    0xf2, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb0, 
    0x13, MP_QSTR_pswd & 0xff, MP_QSTR_pswd >> 8, 
    0x34, 0x01, 
    0xf2, 
    0xe5, 
    0xc6, 
    0xb5, 
    0x86, 
    0x58, 
    0x55, 
    0x22, 0x81, 0x40, 
    0xe0, 
    0x5b, 
    0x56, 
    0xb0, 
    0x13, MP_QSTR_keepalive & 0xff, MP_QSTR_keepalive >> 8, 
    0x44, 0x2c, 0x80, 
    0xb0, 
    0x13, MP_QSTR_keepalive & 0xff, MP_QSTR_keepalive >> 8, 
    0x22, 0x84, 0x80, 0x00, 
    0xd7, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0xb5, 
    0x87, 
    0x58, 
    0x55, 
    0xb0, 
    0x13, MP_QSTR_keepalive & 0xff, MP_QSTR_keepalive >> 8, 
    0x88, 
    0xf1, 
    0xe0, 
    0x5b, 
    0x56, 
    0xb5, 
    0x88, 
    0x58, 
    0x55, 
    0xb0, 
    0x13, MP_QSTR_keepalive & 0xff, MP_QSTR_keepalive >> 8, 
    0x22, 0x81, 0x7f, 
    0xef, 
    0xe0, 
    0x5b, 
    0x56, 
    0xb0, 
    0x13, MP_QSTR_lw_topic & 0xff, MP_QSTR_lw_topic >> 8, 
    0x44, 0x41, 0x80, 
    0xb6, 
    0x82, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb0, 
    0x13, MP_QSTR_lw_topic & 0xff, MP_QSTR_lw_topic >> 8, 
    0x34, 0x01, 
    0xf2, 
    0x82, 
    0xf2, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb0, 
    0x13, MP_QSTR_lw_msg & 0xff, MP_QSTR_lw_msg >> 8, 
    0x34, 0x01, 
    0xf2, 
    0xe5, 
    0xc6, 
    0xb5, 
    0x86, 
    0x58, 
    0x55, 
    0x84, 
    0xb0, 
    0x13, MP_QSTR_lw_qos & 0xff, MP_QSTR_lw_qos >> 8, 
    0x81, 
    0xef, 
    0x83, 
    0xf0, 
    0xed, 
    0xb0, 
    0x13, MP_QSTR_lw_qos & 0xff, MP_QSTR_lw_qos >> 8, 
    0x82, 
    0xef, 
    0x83, 
    0xf0, 
    0xed, 
    0xe0, 
    0x5b, 
    0x56, 
    0xb5, 
    0x86, 
    0x58, 
    0x55, 
    0xb0, 
    0x13, MP_QSTR_lw_retain & 0xff, MP_QSTR_lw_retain >> 8, 
    0x85, 
    0xf0, 
    0xe0, 
    0x5b, 
    0x56, 
    0x81, 
    0xc7, 
    0x42, 0x14, 0x80, 
    0xb6, 
    0x22, 0x80, 0x7f, 
    0xef, 
    0x22, 0x81, 0x00, 
    0xed, 
    0xb4, 
    0xb7, 
    0x56, 
    0xb6, 
    0x87, 
    0xe4, 
    0xc6, 
    0xb7, 
    0x81, 
    0xe5, 
    0xc7, 
    0xb6, 
    0x22, 0x80, 0x7f, 
    0xd8, 
    0x43, 0xe4, 0x7f, 
    0xb6, 
    0xb4, 
    0xb7, 
    0x56, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb4, 
    0xb7, 
    0x82, 
    0xf2, 
    0x36, 0x02, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb5, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR__send_str & 0xff, MP_QSTR__send_str >> 8, 
    0xb0, 
    0x13, MP_QSTR_client_id & 0xff, MP_QSTR_client_id >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_lw_topic & 0xff, MP_QSTR_lw_topic >> 8, 
    0x44, 0x16, 0x80, 
    0xb0, 
    0x14, MP_QSTR__send_str & 0xff, MP_QSTR__send_str >> 8, 
    0xb0, 
    0x13, MP_QSTR_lw_topic & 0xff, MP_QSTR_lw_topic >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR__send_str & 0xff, MP_QSTR__send_str >> 8, 
    0xb0, 
    0x13, MP_QSTR_lw_msg & 0xff, MP_QSTR_lw_msg >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_user & 0xff, MP_QSTR_user >> 8, 
    0x51, 
    0xde, 
    0xd3, 
    0x44, 0x16, 0x80, 
    0xb0, 
    0x14, MP_QSTR__send_str & 0xff, MP_QSTR__send_str >> 8, 
    0xb0, 
    0x13, MP_QSTR_user & 0xff, MP_QSTR_user >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR__send_str & 0xff, MP_QSTR__send_str >> 8, 
    0xb0, 
    0x13, MP_QSTR_pswd & 0xff, MP_QSTR_pswd >> 8, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x84, 
    0x36, 0x01, 
    0xc8, 
    0xb8, 
    0x80, 
    0x55, 
    0xa0, 
    0xd9, 
    0x44, 0x08, 0x80, 
    0xb8, 
    0x81, 
    0x55, 
    0x82, 
    0xd9, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0xb8, 
    0x83, 
    0x55, 
    0x80, 
    0xdc, 
    0x44, 0x09, 0x80, 
    0x12, MP_QSTR_MQTTException & 0xff, MP_QSTR_MQTTException >> 8, 
    0xb8, 
    0x83, 
    0x55, 
    0x34, 0x01, 
    0x65, 
    0xb8, 
    0x82, 
    0x55, 
    0x81, 
    0xef, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_connect_0 = {{&mp_type_bytes}, 36565, 6, (const byte*)"\x10\x00\x00\x00\x00\x00"};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_connect_1 = {{&mp_type_bytes}, 14619, 9, (const byte*)"\x04\x4d\x51\x54\x54\x04\x02\x00\x00"};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient_connect[4] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_clean_session),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_connect_0),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_connect_1),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient_connect = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient_connect,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient_connect,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 519,
    .n_obj = 2,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 15,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_connect,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient_connect + 7,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient_connect + 50,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient_disconnect
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient_disconnect[34] = {
    0x19, 0x10,
    MP_QSTR_disconnect & 0xff, MP_QSTR_disconnect >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x71, 0x2c, 0x00,
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x01, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_close & 0xff, MP_QSTR_close >> 8, 
    0x36, 0x00, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_disconnect_0 = {{&mp_type_bytes}, 24037, 2, (const byte*)"\xe0\x00"};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient_disconnect[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_disconnect_0),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient_disconnect = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient_disconnect,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient_disconnect,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 34,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_disconnect,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient_disconnect + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient_disconnect + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient_ping
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient_ping[23] = {
    0x19, 0x0e,
    MP_QSTR_ping & 0xff, MP_QSTR_ping >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x75, 0x00,
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0x23, 0x01, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_ping_0 = {{&mp_type_bytes}, 25093, 2, (const byte*)"\xc0\x00"};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient_ping[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_ping_0),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient_ping = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient_ping,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient_ping,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 23,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_ping,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient_ping + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient_ping + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient_publish
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient_publish[314] = {
    0x81, 0x94, 0x01, 0x4c,
    MP_QSTR_publish & 0xff, MP_QSTR_publish >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x78, 0x28, 0x2c, 0x30, 0x26, 0x24, 0x2e, 0x22, 0x23, 0x2c, 0x24, 0x2c, 0x44, 0x2e, 0x28, 0x26, 0x2b, 0x25, 0x2f, 0x2c, 0x2b, 0x26, 0x20, 0x27, 0x28, 0x2b, 0x2b, 0x2b, 0x2a, 0x26, 0x28, 0x26, 0x00,
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x23, 0x05, 
    0x34, 0x01, 
    0xc5, 
    0xb5, 
    0x80, 
    0x58, 
    0x55, 
    0xb4, 
    0x81, 
    0xf0, 
    0xb3, 
    0xed, 
    0xe0, 
    0x5b, 
    0x56, 
    0x82, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb1, 
    0x34, 0x01, 
    0xf2, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb2, 
    0x34, 0x01, 
    0xf2, 
    0xc6, 
    0xb4, 
    0x80, 
    0xd8, 
    0x44, 0x04, 0x80, 
    0xb6, 
    0x82, 
    0xe5, 
    0xc6, 
    0xb6, 
    0x22, 0x81, 0x80, 0x80, 0x00, 
    0xd7, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0x81, 
    0xc7, 
    0x42, 0x14, 0x80, 
    0xb6, 
    0x22, 0x80, 0x7f, 
    0xef, 
    0x22, 0x81, 0x00, 
    0xed, 
    0xb5, 
    0xb7, 
    0x56, 
    0xb6, 
    0x87, 
    0xe4, 
    0xc6, 
    0xb7, 
    0x81, 
    0xe5, 
    0xc7, 
    0xb6, 
    0x22, 0x80, 0x7f, 
    0xd8, 
    0x43, 0xe4, 0x7f, 
    0xb6, 
    0xb5, 
    0xb7, 
    0x56, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb5, 
    0xb7, 
    0x81, 
    0xf2, 
    0x36, 0x02, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR__send_str & 0xff, MP_QSTR__send_str >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0xb4, 
    0x80, 
    0xd8, 
    0x44, 0x2b, 0x80, 
    0xb0, 
    0x57, 
    0x13, MP_QSTR_pid & 0xff, MP_QSTR_pid >> 8, 
    0x81, 
    0xe5, 
    0x5a, 
    0x18, MP_QSTR_pid & 0xff, MP_QSTR_pid >> 8, 
    0xb0, 
    0x13, MP_QSTR_pid & 0xff, MP_QSTR_pid >> 8, 
    0xc8, 
    0x12, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x14, MP_QSTR_pack_into & 0xff, MP_QSTR_pack_into >> 8, 
    0x10, MP_QSTR__bang_H & 0xff, MP_QSTR__bang_H >> 8, 
    0xb5, 
    0x80, 
    0xb8, 
    0x36, 0x04, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb5, 
    0x82, 
    0x36, 0x02, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb2, 
    0x36, 0x01, 
    0x59, 
    0xb4, 
    0x81, 
    0xd9, 
    0x44, 0x48, 0x80, 
    0xb0, 
    0x14, MP_QSTR_wait_msg & 0xff, MP_QSTR_wait_msg >> 8, 
    0x36, 0x00, 
    0xc9, 
    0xb9, 
    0x22, 0x80, 0x40, 
    0xd9, 
    0x44, 0x33, 0x80, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x81, 
    0x36, 0x01, 
    0xc6, 
    0xb6, 
    0x23, 0x06, 
    0xd9, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x82, 
    0x36, 0x01, 
    0xca, 
    0xba, 
    0x80, 
    0x55, 
    0x88, 
    0xf0, 
    0xba, 
    0x81, 
    0x55, 
    0xed, 
    0xca, 
    0xb8, 
    0xba, 
    0xd9, 
    0x44, 0x02, 0x80, 
    0x51, 
    0x63, 
    0x42, 0xbb, 0x7f, 
    0x42, 0x0d, 0x80, 
    0xb4, 
    0x82, 
    0xd9, 
    0x44, 0x07, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0x42, 0x00, 0x80, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_publish_0 = {{&mp_type_bytes}, 18805, 4, (const byte*)"\x30\x00\x00\x00"};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_publish_1 = {{&mp_type_bytes}, 46503, 1, (const byte*)"\x02"};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient_publish[7] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_topic),
    MP_ROM_QSTR(MP_QSTR_msg),
    MP_ROM_QSTR(MP_QSTR_retain),
    MP_ROM_QSTR(MP_QSTR_qos),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_publish_0),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_publish_1),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient_publish = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 5,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient_publish,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient_publish,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 314,
    .n_obj = 2,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 17,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 5,
        .n_kwonly_args = 0,
        .n_def_pos_args = 2,
        .qstr_block_name = MP_QSTR_publish,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient_publish + 8,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient_publish + 42,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient_subscribe
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient_subscribe[206] = {
    0xeb, 0x01, 0x2a,
    MP_QSTR_subscribe & 0xff, MP_QSTR_subscribe >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0x9b, 0x32, 0x28, 0x2b, 0x5e, 0x2b, 0x28, 0x34, 0x20, 0x27, 0x28, 0x4b, 0x38, 0x2a, 0x29, 0x00,
    0xb0, 
    0x13, MP_QSTR_cb & 0xff, MP_QSTR_cb >> 8, 
    0x51, 
    0xde, 
    0xd3, 
    0x43, 0x08, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x23, 0x03, 
    0x34, 0x01, 
    0x65, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x23, 0x04, 
    0x34, 0x01, 
    0xc3, 
    0xb0, 
    0x57, 
    0x13, MP_QSTR_pid & 0xff, MP_QSTR_pid >> 8, 
    0x81, 
    0xe5, 
    0x5a, 
    0x18, MP_QSTR_pid & 0xff, MP_QSTR_pid >> 8, 
    0x12, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x14, MP_QSTR_pack_into & 0xff, MP_QSTR_pack_into >> 8, 
    0x10, MP_QSTR__bang_BH & 0xff, MP_QSTR__bang_BH >> 8, 
    0xb3, 
    0x81, 
    0x82, 
    0x82, 
    0xf2, 
    0x12, MP_QSTR_len & 0xff, MP_QSTR_len >> 8, 
    0xb1, 
    0x34, 0x01, 
    0xf2, 
    0x81, 
    0xf2, 
    0xb0, 
    0x13, MP_QSTR_pid & 0xff, MP_QSTR_pid >> 8, 
    0x36, 0x05, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb3, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR__send_str & 0xff, MP_QSTR__send_str >> 8, 
    0xb1, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb2, 
    0x14, MP_QSTR_to_bytes & 0xff, MP_QSTR_to_bytes >> 8, 
    0x81, 
    0x10, MP_QSTR_little & 0xff, MP_QSTR_little >> 8, 
    0x36, 0x02, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_wait_msg & 0xff, MP_QSTR_wait_msg >> 8, 
    0x36, 0x00, 
    0xc4, 
    0xb4, 
    0x22, 0x81, 0x10, 
    0xd9, 
    0x44, 0x38, 0x80, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x84, 
    0x36, 0x01, 
    0xc5, 
    0xb5, 
    0x81, 
    0x55, 
    0xb3, 
    0x82, 
    0x55, 
    0xd9, 
    0x44, 0x0a, 0x80, 
    0xb5, 
    0x82, 
    0x55, 
    0xb3, 
    0x83, 
    0x55, 
    0xd9, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0xb5, 
    0x83, 
    0x55, 
    0x22, 0x81, 0x00, 
    0xd9, 
    0x44, 0x09, 0x80, 
    0x12, MP_QSTR_MQTTException & 0xff, MP_QSTR_MQTTException >> 8, 
    0xb5, 
    0x83, 
    0x55, 
    0x34, 0x01, 
    0x65, 
    0x51, 
    0x63, 
    0x42, 0xb6, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_subscribe_0 = {{&mp_type_str}, 9577, 29, (const byte*)"\x53\x75\x62\x73\x63\x72\x69\x62\x65\x20\x63\x61\x6c\x6c\x62\x61\x63\x6b\x20\x69\x73\x20\x6e\x6f\x74\x20\x73\x65\x74"};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_subscribe_1 = {{&mp_type_bytes}, 63431, 4, (const byte*)"\x82\x00\x00\x00"};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient_subscribe[5] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_topic),
    MP_ROM_QSTR(MP_QSTR_qos),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_subscribe_0),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_subscribe_1),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient_subscribe = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 3,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient_subscribe,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient_subscribe,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 206,
    .n_obj = 2,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 14,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 1,
        .qstr_block_name = MP_QSTR_subscribe,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient_subscribe + 7,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient_subscribe + 24,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient_wait_msg
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient_wait_msg[289] = {
    0x71, 0x48,
    MP_QSTR_wait_msg & 0xff, MP_QSTR_wait_msg >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0xb2, 0x2b, 0x2b, 0x26, 0x22, 0x27, 0x27, 0x27, 0x2d, 0x2a, 0x22, 0x24, 0x2b, 0x22, 0x27, 0x2b, 0x2a, 0x2b, 0x26, 0x26, 0x2b, 0x2a, 0x24, 0x2b, 0x29, 0x28, 0x28, 0x2f, 0x2e, 0x28, 0x00,
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x81, 
    0x36, 0x01, 
    0xc1, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_setblocking & 0xff, MP_QSTR_setblocking >> 8, 
    0x52, 
    0x36, 0x01, 
    0x59, 
    0xb1, 
    0x51, 
    0xde, 
    0x44, 0x02, 0x80, 
    0x51, 
    0x63, 
    0xb1, 
    0x23, 0x01, 
    0xd9, 
    0x44, 0x07, 0x80, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0x7f, 
    0x34, 0x01, 
    0x65, 
    0xb1, 
    0x23, 0x02, 
    0xd9, 
    0x44, 0x19, 0x80, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x81, 
    0x36, 0x01, 
    0x80, 
    0x55, 
    0xc2, 
    0xb2, 
    0x80, 
    0xd9, 
    0x43, 0x04, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0x51, 
    0x63, 
    0xb1, 
    0x80, 
    0x55, 
    0xc3, 
    0xb3, 
    0x22, 0x81, 0x70, 
    0xef, 
    0x22, 0x30, 
    0xdc, 
    0x44, 0x02, 0x80, 
    0xb3, 
    0x63, 
    0xb0, 
    0x14, MP_QSTR__recv_len & 0xff, MP_QSTR__recv_len >> 8, 
    0x36, 0x00, 
    0xc2, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x82, 
    0x36, 0x01, 
    0xc4, 
    0xb4, 
    0x80, 
    0x55, 
    0x88, 
    0xf0, 
    0xb4, 
    0x81, 
    0x55, 
    0xed, 
    0xc4, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0xb4, 
    0x36, 0x01, 
    0xc5, 
    0xb2, 
    0xb4, 
    0x82, 
    0xf2, 
    0xe6, 
    0xc2, 
    0xb3, 
    0x86, 
    0xef, 
    0x44, 0x19, 0x80, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0x82, 
    0x36, 0x01, 
    0xc6, 
    0xb6, 
    0x80, 
    0x55, 
    0x88, 
    0xf0, 
    0xb6, 
    0x81, 
    0x55, 
    0xed, 
    0xc6, 
    0xb2, 
    0x82, 
    0xe6, 
    0xc2, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_read & 0xff, MP_QSTR_read >> 8, 
    0xb2, 
    0x36, 0x01, 
    0xc7, 
    0xb0, 
    0x14, MP_QSTR_cb & 0xff, MP_QSTR_cb >> 8, 
    0xb5, 
    0xb7, 
    0x36, 0x02, 
    0x59, 
    0xb3, 
    0x86, 
    0xef, 
    0x82, 
    0xd9, 
    0x44, 0x25, 0x80, 
    0x12, MP_QSTR_bytearray & 0xff, MP_QSTR_bytearray >> 8, 
    0x23, 0x03, 
    0x34, 0x01, 
    0xc8, 
    0x12, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x14, MP_QSTR_pack_into & 0xff, MP_QSTR_pack_into >> 8, 
    0x10, MP_QSTR__bang_H & 0xff, MP_QSTR__bang_H >> 8, 
    0xb8, 
    0x82, 
    0xb6, 
    0x36, 0x04, 
    0x59, 
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_write & 0xff, MP_QSTR_write >> 8, 
    0xb8, 
    0x36, 0x01, 
    0x59, 
    0x42, 0x0f, 0x80, 
    0xb3, 
    0x86, 
    0xef, 
    0x84, 
    0xd9, 
    0x44, 0x07, 0x80, 
    0x12, MP_QSTR_AssertionError & 0xff, MP_QSTR_AssertionError >> 8, 
    0x65, 
    0x42, 0x00, 0x80, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_wait_msg_0 = {{&mp_type_bytes}, 5381, 0, (const byte*)""};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_wait_msg_1 = {{&mp_type_bytes}, 46453, 1, (const byte*)"\xd0"};
STATIC const mp_obj_str_t const_obj_umqtt_simple__lt_module_gt__MQTTClient_wait_msg_2 = {{&mp_type_bytes}, 12359, 4, (const byte*)"\x40\x02\x00\x00"};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient_wait_msg[4] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_wait_msg_0),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_wait_msg_1),
    MP_ROM_PTR(&const_obj_umqtt_simple__lt_module_gt__MQTTClient_wait_msg_2),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient_wait_msg = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient_wait_msg,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient_wait_msg,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 289,
    .n_obj = 3,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 15,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_wait_msg,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient_wait_msg + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient_wait_msg + 38,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient_check_msg
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient_check_msg[28] = {
    0x19, 0x10,
    MP_QSTR_check_msg & 0xff, MP_QSTR_check_msg >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x80, 0xd5, 0x2b, 0x00,
    0xb0, 
    0x13, MP_QSTR_sock & 0xff, MP_QSTR_sock >> 8, 
    0x14, MP_QSTR_setblocking & 0xff, MP_QSTR_setblocking >> 8, 
    0x50, 
    0x36, 0x01, 
    0x59, 
    0xb0, 
    0x14, MP_QSTR_wait_msg & 0xff, MP_QSTR_wait_msg >> 8, 
    0x36, 0x00, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient_check_msg[1] = {
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient_check_msg = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 1,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient_check_msg,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient_check_msg,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 28,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 1,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_check_msg,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient_check_msg + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient_check_msg + 10,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple__lt_module_gt__MQTTClient
STATIC const byte fun_data_umqtt_simple__lt_module_gt__MQTTClient[133] = {
    0x28, 0x3a,
    MP_QSTR_MQTTClient & 0xff, MP_QSTR_MQTTClient >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x8c, 0x0a, 0x85, 0x09, 0x8a, 0x14, 0x65, 0x20, 0x85, 0x0a, 0x65, 0x8a, 0x08, 0x89, 0x30, 0x65, 0x20, 0x65, 0x8a, 0x23, 0x89, 0x17, 0x85, 0x23, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_MQTTClient & 0xff, MP_QSTR_MQTTClient >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x80, 
    0x51, 
    0x51, 
    0x80, 
    0x50, 
    0x2c, 0x00, 
    0x2a, 0x06, 
    0x53, 
    0x33, 0x00, 
    0x16, MP_QSTR___init__ & 0xff, MP_QSTR___init__ >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR__send_str & 0xff, MP_QSTR__send_str >> 8, 
    0x32, 0x02, 
    0x16, MP_QSTR__recv_len & 0xff, MP_QSTR__recv_len >> 8, 
    0x32, 0x03, 
    0x16, MP_QSTR_set_callback & 0xff, MP_QSTR_set_callback >> 8, 
    0x50, 
    0x80, 
    0x2a, 0x02, 
    0x53, 
    0x33, 0x04, 
    0x16, MP_QSTR_set_last_will & 0xff, MP_QSTR_set_last_will >> 8, 
    0x52, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x05, 
    0x16, MP_QSTR_connect & 0xff, MP_QSTR_connect >> 8, 
    0x32, 0x06, 
    0x16, MP_QSTR_disconnect & 0xff, MP_QSTR_disconnect >> 8, 
    0x32, 0x07, 
    0x16, MP_QSTR_ping & 0xff, MP_QSTR_ping >> 8, 
    0x50, 
    0x80, 
    0x2a, 0x02, 
    0x53, 
    0x33, 0x08, 
    0x16, MP_QSTR_publish & 0xff, MP_QSTR_publish >> 8, 
    0x80, 
    0x2a, 0x01, 
    0x53, 
    0x33, 0x09, 
    0x16, MP_QSTR_subscribe & 0xff, MP_QSTR_subscribe >> 8, 
    0x32, 0x0a, 
    0x16, MP_QSTR_wait_msg & 0xff, MP_QSTR_wait_msg >> 8, 
    0x32, 0x0b, 
    0x16, MP_QSTR_check_msg & 0xff, MP_QSTR_check_msg >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt__MQTTClient[12] = {
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient___init__),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient__send_str),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient__recv_len),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient_set_callback),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient_set_last_will),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient_connect),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient_disconnect),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient_ping),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient_publish),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient_subscribe),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient_wait_msg),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient_check_msg),
};
STATIC const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt__MQTTClient = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_umqtt_simple__lt_module_gt__MQTTClient,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt__MQTTClient,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 133,
    .n_obj = 0,
    .n_raw_code = 12,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_MQTTClient,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt__MQTTClient + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt__MQTTClient + 31,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/simple.py, scope umqtt_simple_<module>
STATIC const byte fun_data_umqtt_simple__lt_module_gt_[71] = {
    0x18, 0x14,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_umqtt_slash_simple_dot_py & 0xff, MP_QSTR_umqtt_slash_simple_dot_py >> 8,
    0x28, 0x28, 0x70, 0x6e, 0x20, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_usocket & 0xff, MP_QSTR_usocket >> 8, 
    0x16, MP_QSTR_socket & 0xff, MP_QSTR_socket >> 8, 
    0x80, 
    0x51, 
    0x1b, MP_QSTR_ustruct & 0xff, MP_QSTR_ustruct >> 8, 
    0x16, MP_QSTR_struct & 0xff, MP_QSTR_struct >> 8, 
    0x80, 
    0x10, MP_QSTR_hexlify & 0xff, MP_QSTR_hexlify >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_ubinascii & 0xff, MP_QSTR_ubinascii >> 8, 
    0x1c, MP_QSTR_hexlify & 0xff, MP_QSTR_hexlify >> 8, 
    0x16, MP_QSTR_hexlify & 0xff, MP_QSTR_hexlify >> 8, 
    0x59, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_MQTTException & 0xff, MP_QSTR_MQTTException >> 8, 
    0x11, MP_QSTR_Exception & 0xff, MP_QSTR_Exception >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_MQTTException & 0xff, MP_QSTR_MQTTException >> 8, 
    0x54, 
    0x32, 0x01, 
    0x10, MP_QSTR_MQTTClient & 0xff, MP_QSTR_MQTTClient >> 8, 
    0x34, 0x02, 
    0x16, MP_QSTR_MQTTClient & 0xff, MP_QSTR_MQTTClient >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_simple__lt_module_gt_[2] = {
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTException),
    MP_ROM_PTR(&raw_code_umqtt_simple__lt_module_gt__MQTTClient),
};
const mp_raw_code_t raw_code_umqtt_simple__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_umqtt_simple__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_umqtt_simple__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 71,
    .n_obj = 0,
    .n_raw_code = 2,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_umqtt_slash_simple_dot_py,
        .line_info = fun_data_umqtt_simple__lt_module_gt_ + 6,
        .opcodes = fun_data_umqtt_simple__lt_module_gt_ + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/robust.py, scope umqtt_robust__lt_module_gt__MQTTClient_delay
STATIC const byte fun_data_umqtt_robust__lt_module_gt__MQTTClient_delay[24] = {
    0x22, 0x0e,
    MP_QSTR_delay & 0xff, MP_QSTR_delay >> 8,
    MP_QSTR_umqtt_slash_robust_dot_py & 0xff, MP_QSTR_umqtt_slash_robust_dot_py >> 8,
    0x80, 0x0a, 0x00,
    0x12, MP_QSTR_utime & 0xff, MP_QSTR_utime >> 8, 
    0x14, MP_QSTR_sleep & 0xff, MP_QSTR_sleep >> 8, 
    0xb0, 
    0x13, MP_QSTR_DELAY & 0xff, MP_QSTR_DELAY >> 8, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_robust__lt_module_gt__MQTTClient_delay[2] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_i),
};
STATIC const mp_raw_code_t raw_code_umqtt_robust__lt_module_gt__MQTTClient_delay = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_umqtt_robust__lt_module_gt__MQTTClient_delay,
    .const_table = (mp_uint_t*)const_table_data_umqtt_robust__lt_module_gt__MQTTClient_delay,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 24,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 5,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_delay,
        .qstr_source_file = MP_QSTR_umqtt_slash_robust_dot_py,
        .line_info = fun_data_umqtt_robust__lt_module_gt__MQTTClient_delay + 6,
        .opcodes = fun_data_umqtt_robust__lt_module_gt__MQTTClient_delay + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/robust.py, scope umqtt_robust__lt_module_gt__MQTTClient_log
STATIC const byte fun_data_umqtt_robust__lt_module_gt__MQTTClient_log[49] = {
    0x2b, 0x14,
    MP_QSTR_log & 0xff, MP_QSTR_log >> 8,
    MP_QSTR_umqtt_slash_robust_dot_py & 0xff, MP_QSTR_umqtt_slash_robust_dot_py >> 8,
    0x80, 0x0d, 0x27, 0x24, 0x4d, 0x00,
    0xb0, 
    0x13, MP_QSTR_DEBUG & 0xff, MP_QSTR_DEBUG >> 8, 
    0x44, 0x1c, 0x80, 
    0xb1, 
    0x44, 0x0d, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x23, 0x03, 
    0xb2, 
    0xf8, 
    0x34, 0x01, 
    0x59, 
    0x42, 0x0b, 0x80, 
    0x12, MP_QSTR_print & 0xff, MP_QSTR_print >> 8, 
    0x10, MP_QSTR_mqtt_colon__space__percent_r & 0xff, MP_QSTR_mqtt_colon__space__percent_r >> 8, 
    0xb2, 
    0xf8, 
    0x34, 0x01, 
    0x59, 
    0x51, 
    0x63, 
};
STATIC const mp_obj_str_t const_obj_umqtt_robust__lt_module_gt__MQTTClient_log_0 = {{&mp_type_str}, 17533, 18, (const byte*)"\x6d\x71\x74\x74\x20\x72\x65\x63\x6f\x6e\x6e\x65\x63\x74\x3a\x20\x25\x72"};
STATIC const mp_rom_obj_t const_table_data_umqtt_robust__lt_module_gt__MQTTClient_log[4] = {
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_in_reconnect),
    MP_ROM_QSTR(MP_QSTR_e),
    MP_ROM_PTR(&const_obj_umqtt_robust__lt_module_gt__MQTTClient_log_0),
};
STATIC const mp_raw_code_t raw_code_umqtt_robust__lt_module_gt__MQTTClient_log = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 3,
    .fun_data = fun_data_umqtt_robust__lt_module_gt__MQTTClient_log,
    .const_table = (mp_uint_t*)const_table_data_umqtt_robust__lt_module_gt__MQTTClient_log,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 49,
    .n_obj = 1,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 6,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 3,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_log,
        .qstr_source_file = MP_QSTR_umqtt_slash_robust_dot_py,
        .line_info = fun_data_umqtt_robust__lt_module_gt__MQTTClient_log + 6,
        .opcodes = fun_data_umqtt_robust__lt_module_gt__MQTTClient_log + 12,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/robust.py, scope umqtt_robust__lt_module_gt__MQTTClient_reconnect
STATIC const byte fun_data_umqtt_robust__lt_module_gt__MQTTClient_reconnect[85] = {
    0xd2, 0x02, 0x1a,
    MP_QSTR_reconnect & 0xff, MP_QSTR_reconnect >> 8,
    MP_QSTR_umqtt_slash_robust_dot_py & 0xff, MP_QSTR_umqtt_slash_robust_dot_py >> 8,
    0x80, 0x14, 0x22, 0x20, 0x23, 0x5c, 0x29, 0x24, 0x00,
    0x80, 
    0xc2, 
    0x48, 0x10, 0x00, 
    0x12, MP_QSTR_super & 0xff, MP_QSTR_super >> 8, 
    0x25, 0x00, 
    0xb1, 
    0x15, MP_QSTR_connect & 0xff, MP_QSTR_connect >> 8, 
    0x50, 
    0x36, 0x01, 
    0x63, 
    0x4a, 0x2b, 0x00, 
    0x57, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x22, 0x80, 
    0xc3, 
    0x49, 0x16, 0x00, 
    0xb1, 
    0x14, MP_QSTR_log & 0xff, MP_QSTR_log >> 8, 
    0x52, 
    0xb3, 
    0x36, 0x02, 
    0x59, 
    0xb2, 
    0x81, 
    0xe5, 
    0xc2, 
    0xb1, 
    0x14, MP_QSTR_delay & 0xff, MP_QSTR_delay >> 8, 
    0xb2, 
    0x36, 0x01, 
    0x59, 
    0x51, 
    0x51, 
    0xc3, 
    0x28, 0x03, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0x42, 0xbf, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_robust__lt_module_gt__MQTTClient_reconnect[2] = {
    MP_ROM_QSTR(MP_QSTR__star_),
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_umqtt_robust__lt_module_gt__MQTTClient_reconnect = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_umqtt_robust__lt_module_gt__MQTTClient_reconnect,
    .const_table = (mp_uint_t*)const_table_data_umqtt_robust__lt_module_gt__MQTTClient_reconnect,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 85,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 11,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_reconnect,
        .qstr_source_file = MP_QSTR_umqtt_slash_robust_dot_py,
        .line_info = fun_data_umqtt_robust__lt_module_gt__MQTTClient_reconnect + 7,
        .opcodes = fun_data_umqtt_robust__lt_module_gt__MQTTClient_reconnect + 16,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/robust.py, scope umqtt_robust__lt_module_gt__MQTTClient_publish
STATIC const byte fun_data_umqtt_robust__lt_module_gt__MQTTClient_publish[80] = {
    0xea, 0x86, 0x01, 0x16,
    MP_QSTR_publish & 0xff, MP_QSTR_publish >> 8,
    MP_QSTR_umqtt_slash_robust_dot_py & 0xff, MP_QSTR_umqtt_slash_robust_dot_py >> 8,
    0x80, 0x1e, 0x20, 0x23, 0x5f, 0x33, 0x00,
    0x48, 0x13, 0x00, 
    0x12, MP_QSTR_super & 0xff, MP_QSTR_super >> 8, 
    0x25, 0x00, 
    0xb1, 
    0x15, MP_QSTR_publish & 0xff, MP_QSTR_publish >> 8, 
    0xb2, 
    0xb3, 
    0xb4, 
    0xb5, 
    0x36, 0x04, 
    0x63, 
    0x4a, 0x1f, 0x00, 
    0x57, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x16, 0x80, 
    0xc6, 
    0x49, 0x0a, 0x00, 
    0xb1, 
    0x14, MP_QSTR_log & 0xff, MP_QSTR_log >> 8, 
    0x50, 
    0xb6, 
    0x36, 0x02, 
    0x59, 
    0x51, 
    0x51, 
    0xc6, 
    0x28, 0x06, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0xb1, 
    0x14, MP_QSTR_reconnect & 0xff, MP_QSTR_reconnect >> 8, 
    0x36, 0x00, 
    0x59, 
    0x42, 0xc1, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_robust__lt_module_gt__MQTTClient_publish[6] = {
    MP_ROM_QSTR(MP_QSTR__star_),
    MP_ROM_QSTR(MP_QSTR_self),
    MP_ROM_QSTR(MP_QSTR_topic),
    MP_ROM_QSTR(MP_QSTR_msg),
    MP_ROM_QSTR(MP_QSTR_retain),
    MP_ROM_QSTR(MP_QSTR_qos),
};
STATIC const mp_raw_code_t raw_code_umqtt_robust__lt_module_gt__MQTTClient_publish = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 6,
    .fun_data = fun_data_umqtt_robust__lt_module_gt__MQTTClient_publish,
    .const_table = (mp_uint_t*)const_table_data_umqtt_robust__lt_module_gt__MQTTClient_publish,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 80,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 14,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 6,
        .n_kwonly_args = 0,
        .n_def_pos_args = 2,
        .qstr_block_name = MP_QSTR_publish,
        .qstr_source_file = MP_QSTR_umqtt_slash_robust_dot_py,
        .line_info = fun_data_umqtt_robust__lt_module_gt__MQTTClient_publish + 8,
        .opcodes = fun_data_umqtt_robust__lt_module_gt__MQTTClient_publish + 15,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/robust.py, scope umqtt_robust__lt_module_gt__MQTTClient_wait_msg
STATIC const byte fun_data_umqtt_robust__lt_module_gt__MQTTClient_wait_msg[75] = {
    0xca, 0x02, 0x16,
    MP_QSTR_wait_msg & 0xff, MP_QSTR_wait_msg >> 8,
    MP_QSTR_umqtt_slash_robust_dot_py & 0xff, MP_QSTR_umqtt_slash_robust_dot_py >> 8,
    0x80, 0x26, 0x20, 0x23, 0x5b, 0x33, 0x00,
    0x48, 0x0f, 0x00, 
    0x12, MP_QSTR_super & 0xff, MP_QSTR_super >> 8, 
    0x25, 0x00, 
    0xb1, 
    0x15, MP_QSTR_wait_msg & 0xff, MP_QSTR_wait_msg >> 8, 
    0x36, 0x00, 
    0x63, 
    0x4a, 0x1f, 0x00, 
    0x57, 
    0x12, MP_QSTR_OSError & 0xff, MP_QSTR_OSError >> 8, 
    0xdf, 
    0x44, 0x16, 0x80, 
    0xc2, 
    0x49, 0x0a, 0x00, 
    0xb1, 
    0x14, MP_QSTR_log & 0xff, MP_QSTR_log >> 8, 
    0x50, 
    0xb2, 
    0x36, 0x02, 
    0x59, 
    0x51, 
    0x51, 
    0xc2, 
    0x28, 0x02, 
    0x5d, 
    0x4a, 0x01, 0x00, 
    0x5d, 
    0xb1, 
    0x14, MP_QSTR_reconnect & 0xff, MP_QSTR_reconnect >> 8, 
    0x36, 0x00, 
    0x59, 
    0x42, 0xc5, 0x7f, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_robust__lt_module_gt__MQTTClient_wait_msg[2] = {
    MP_ROM_QSTR(MP_QSTR__star_),
    MP_ROM_QSTR(MP_QSTR_self),
};
STATIC const mp_raw_code_t raw_code_umqtt_robust__lt_module_gt__MQTTClient_wait_msg = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 2,
    .fun_data = fun_data_umqtt_robust__lt_module_gt__MQTTClient_wait_msg,
    .const_table = (mp_uint_t*)const_table_data_umqtt_robust__lt_module_gt__MQTTClient_wait_msg,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 75,
    .n_obj = 0,
    .n_raw_code = 0,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 10,
        .n_exc_stack = 2,
        .scope_flags = 0,
        .n_pos_args = 2,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_wait_msg,
        .qstr_source_file = MP_QSTR_umqtt_slash_robust_dot_py,
        .line_info = fun_data_umqtt_robust__lt_module_gt__MQTTClient_wait_msg + 7,
        .opcodes = fun_data_umqtt_robust__lt_module_gt__MQTTClient_wait_msg + 14,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/robust.py, scope umqtt_robust__lt_module_gt__MQTTClient
STATIC const byte fun_data_umqtt_robust__lt_module_gt__MQTTClient[77] = {
    0x18, 0x21,
    MP_QSTR_MQTTClient & 0xff, MP_QSTR_MQTTClient >> 8,
    MP_QSTR_umqtt_slash_robust_dot_py & 0xff, MP_QSTR_umqtt_slash_robust_dot_py >> 8,
    0x6c, 0x60, 0x24, 0x44, 0x65, 0x85, 0x07, 0x87, 0x0a, 0x8c, 0x08, 0x00, 0x00,
    0x11, MP_QSTR___name__ & 0xff, MP_QSTR___name__ >> 8, 
    0x16, MP_QSTR___module__ & 0xff, MP_QSTR___module__ >> 8, 
    0x10, MP_QSTR_MQTTClient & 0xff, MP_QSTR_MQTTClient >> 8, 
    0x16, MP_QSTR___qualname__ & 0xff, MP_QSTR___qualname__ >> 8, 
    0x82, 
    0x16, MP_QSTR_DELAY & 0xff, MP_QSTR_DELAY >> 8, 
    0x50, 
    0x16, MP_QSTR_DEBUG & 0xff, MP_QSTR_DEBUG >> 8, 
    0x32, 0x00, 
    0x16, MP_QSTR_delay & 0xff, MP_QSTR_delay >> 8, 
    0x32, 0x01, 
    0x16, MP_QSTR_log & 0xff, MP_QSTR_log >> 8, 
    0xb0, 
    0x20, 0x02, 0x01, 
    0x16, MP_QSTR_reconnect & 0xff, MP_QSTR_reconnect >> 8, 
    0x50, 
    0x80, 
    0x2a, 0x02, 
    0x53, 
    0xb0, 
    0x21, 0x03, 0x01, 
    0x16, MP_QSTR_publish & 0xff, MP_QSTR_publish >> 8, 
    0xb0, 
    0x20, 0x04, 0x01, 
    0x16, MP_QSTR_wait_msg & 0xff, MP_QSTR_wait_msg >> 8, 
    0xb0, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_robust__lt_module_gt__MQTTClient[5] = {
    MP_ROM_PTR(&raw_code_umqtt_robust__lt_module_gt__MQTTClient_delay),
    MP_ROM_PTR(&raw_code_umqtt_robust__lt_module_gt__MQTTClient_log),
    MP_ROM_PTR(&raw_code_umqtt_robust__lt_module_gt__MQTTClient_reconnect),
    MP_ROM_PTR(&raw_code_umqtt_robust__lt_module_gt__MQTTClient_publish),
    MP_ROM_PTR(&raw_code_umqtt_robust__lt_module_gt__MQTTClient_wait_msg),
};
STATIC const mp_raw_code_t raw_code_umqtt_robust__lt_module_gt__MQTTClient = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_umqtt_robust__lt_module_gt__MQTTClient,
    .const_table = (mp_uint_t*)const_table_data_umqtt_robust__lt_module_gt__MQTTClient,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 77,
    .n_obj = 0,
    .n_raw_code = 5,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR_MQTTClient,
        .qstr_source_file = MP_QSTR_umqtt_slash_robust_dot_py,
        .line_info = fun_data_umqtt_robust__lt_module_gt__MQTTClient + 6,
        .opcodes = fun_data_umqtt_robust__lt_module_gt__MQTTClient + 19,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

// frozen bytecode for file umqtt/robust.py, scope umqtt_robust_<module>
STATIC const byte fun_data_umqtt_robust__lt_module_gt_[52] = {
    0x18, 0x0e,
    MP_QSTR__lt_module_gt_ & 0xff, MP_QSTR__lt_module_gt_ >> 8,
    MP_QSTR_umqtt_slash_robust_dot_py & 0xff, MP_QSTR_umqtt_slash_robust_dot_py >> 8,
    0x28, 0x70, 0x00,
    0x80, 
    0x51, 
    0x1b, MP_QSTR_utime & 0xff, MP_QSTR_utime >> 8, 
    0x16, MP_QSTR_utime & 0xff, MP_QSTR_utime >> 8, 
    0x81, 
    0x10, MP_QSTR_simple & 0xff, MP_QSTR_simple >> 8, 
    0x2a, 0x01, 
    0x1b, MP_QSTR_ & 0xff, MP_QSTR_ >> 8, 
    0x1c, MP_QSTR_simple & 0xff, MP_QSTR_simple >> 8, 
    0x16, MP_QSTR_simple & 0xff, MP_QSTR_simple >> 8, 
    0x59, 
    0x54, 
    0x32, 0x00, 
    0x10, MP_QSTR_MQTTClient & 0xff, MP_QSTR_MQTTClient >> 8, 
    0x11, MP_QSTR_simple & 0xff, MP_QSTR_simple >> 8, 
    0x13, MP_QSTR_MQTTClient & 0xff, MP_QSTR_MQTTClient >> 8, 
    0x34, 0x03, 
    0x16, MP_QSTR_MQTTClient & 0xff, MP_QSTR_MQTTClient >> 8, 
    0x51, 
    0x63, 
};
STATIC const mp_rom_obj_t const_table_data_umqtt_robust__lt_module_gt_[1] = {
    MP_ROM_PTR(&raw_code_umqtt_robust__lt_module_gt__MQTTClient),
};
const mp_raw_code_t raw_code_umqtt_robust__lt_module_gt_ = {
    .kind = MP_CODE_BYTECODE,
    .scope_flags = 0x00,
    .n_pos_args = 0,
    .fun_data = fun_data_umqtt_robust__lt_module_gt_,
    .const_table = (mp_uint_t*)const_table_data_umqtt_robust__lt_module_gt_,
    #if MICROPY_PERSISTENT_CODE_SAVE
    .fun_data_len = 52,
    .n_obj = 0,
    .n_raw_code = 1,
    #if MICROPY_PY_SYS_SETTRACE
    .prelude = {
        .n_state = 4,
        .n_exc_stack = 0,
        .scope_flags = 0,
        .n_pos_args = 0,
        .n_kwonly_args = 0,
        .n_def_pos_args = 0,
        .qstr_block_name = MP_QSTR__lt_module_gt_,
        .qstr_source_file = MP_QSTR_umqtt_slash_robust_dot_py,
        .line_info = fun_data_umqtt_robust__lt_module_gt_ + 6,
        .opcodes = fun_data_umqtt_robust__lt_module_gt_ + 9,
    },
    .line_of_definition = 0,
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .prelude_offset = 0,
    .n_qstr = 0,
    .qstr_link = NULL,
    #endif
    #endif
    #if MICROPY_EMIT_MACHINE_CODE
    .type_sig = 0,
    #endif
};

const char mp_frozen_mpy_names[] = {
"_boot.py\0"
"flashbdev.py\0"
"apa106.py\0"
"inisetup.py\0"
"upip.py\0"
"upip_utarfile.py\0"
"ntptime.py\0"
"dht.py\0"
"onewire.py\0"
"ds18x20.py\0"
"uasyncio/__init__.py\0"
"uasyncio/core.py\0"
"uasyncio/event.py\0"
"uasyncio/funcs.py\0"
"uasyncio/lock.py\0"
"uasyncio/stream.py\0"
"webrepl.py\0"
"webrepl_setup.py\0"
"websocket_helper.py\0"
"neopixel.py\0"
"urequests.py\0"
"upysh.py\0"
"umqtt/simple.py\0"
"umqtt/robust.py\0"
"\0"};
const mp_raw_code_t *const mp_frozen_mpy_content[] = {
    &raw_code__boot__lt_module_gt_,
    &raw_code_flashbdev__lt_module_gt_,
    &raw_code_apa106__lt_module_gt_,
    &raw_code_inisetup__lt_module_gt_,
    &raw_code_upip__lt_module_gt_,
    &raw_code_upip_utarfile__lt_module_gt_,
    &raw_code_ntptime__lt_module_gt_,
    &raw_code_dht__lt_module_gt_,
    &raw_code_onewire__lt_module_gt_,
    &raw_code_ds18x20__lt_module_gt_,
    &raw_code_uasyncio___init____lt_module_gt_,
    &raw_code_uasyncio_core__lt_module_gt_,
    &raw_code_uasyncio_event__lt_module_gt_,
    &raw_code_uasyncio_funcs__lt_module_gt_,
    &raw_code_uasyncio_lock__lt_module_gt_,
    &raw_code_uasyncio_stream__lt_module_gt_,
    &raw_code_webrepl__lt_module_gt_,
    &raw_code_webrepl_setup__lt_module_gt_,
    &raw_code_websocket_helper__lt_module_gt_,
    &raw_code_neopixel__lt_module_gt_,
    &raw_code_urequests__lt_module_gt_,
    &raw_code_upysh__lt_module_gt_,
    &raw_code_umqtt_simple__lt_module_gt_,
    &raw_code_umqtt_robust__lt_module_gt_,
};
#ifdef MICROPY_FROZEN_LIST_ITEM
MICROPY_FROZEN_LIST_ITEM("_boot", "_boot.py")
MICROPY_FROZEN_LIST_ITEM("flashbdev", "flashbdev.py")
MICROPY_FROZEN_LIST_ITEM("apa106", "apa106.py")
MICROPY_FROZEN_LIST_ITEM("inisetup", "inisetup.py")
MICROPY_FROZEN_LIST_ITEM("upip", "upip.py")
MICROPY_FROZEN_LIST_ITEM("upip_utarfile", "upip_utarfile.py")
MICROPY_FROZEN_LIST_ITEM("ntptime", "ntptime.py")
MICROPY_FROZEN_LIST_ITEM("dht", "dht.py")
MICROPY_FROZEN_LIST_ITEM("onewire", "onewire.py")
MICROPY_FROZEN_LIST_ITEM("ds18x20", "ds18x20.py")
MICROPY_FROZEN_LIST_ITEM("uasyncio", "uasyncio/__init__.py")
MICROPY_FROZEN_LIST_ITEM("uasyncio/core", "uasyncio/core.py")
MICROPY_FROZEN_LIST_ITEM("uasyncio/event", "uasyncio/event.py")
MICROPY_FROZEN_LIST_ITEM("uasyncio/funcs", "uasyncio/funcs.py")
MICROPY_FROZEN_LIST_ITEM("uasyncio/lock", "uasyncio/lock.py")
MICROPY_FROZEN_LIST_ITEM("uasyncio/stream", "uasyncio/stream.py")
MICROPY_FROZEN_LIST_ITEM("webrepl", "webrepl.py")
MICROPY_FROZEN_LIST_ITEM("webrepl_setup", "webrepl_setup.py")
MICROPY_FROZEN_LIST_ITEM("websocket_helper", "websocket_helper.py")
MICROPY_FROZEN_LIST_ITEM("neopixel", "neopixel.py")
MICROPY_FROZEN_LIST_ITEM("urequests", "urequests.py")
MICROPY_FROZEN_LIST_ITEM("upysh", "upysh.py")
MICROPY_FROZEN_LIST_ITEM("umqtt/simple", "umqtt/simple.py")
MICROPY_FROZEN_LIST_ITEM("umqtt/robust", "umqtt/robust.py")
#endif
