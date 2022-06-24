# 1 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
# 29 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
# 1 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/pycparser/utils/fake_libc_include/string.h" 1
# 1 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/pycparser/utils/fake_libc_include/_fake_defines.h" 1
# 2 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/pycparser/utils/fake_libc_include/string.h" 2
# 1 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/pycparser/utils/fake_libc_include/_fake_typedefs.h" 1



typedef int size_t;
typedef int __builtin_va_list;
typedef int __gnuc_va_list;
typedef int va_list;
typedef int __int8_t;
typedef int __uint8_t;
typedef int __int16_t;
typedef int __uint16_t;
typedef int __int_least16_t;
typedef int __uint_least16_t;
typedef int __int32_t;
typedef int __uint32_t;
typedef int __int64_t;
typedef int __uint64_t;
typedef int __int_least32_t;
typedef int __uint_least32_t;
typedef int __s8;
typedef int __u8;
typedef int __s16;
typedef int __u16;
typedef int __s32;
typedef int __u32;
typedef int __s64;
typedef int __u64;
typedef int _LOCK_T;
typedef int _LOCK_RECURSIVE_T;
typedef int _off_t;
typedef int __dev_t;
typedef int __uid_t;
typedef int __gid_t;
typedef int _off64_t;
typedef int _fpos_t;
typedef int _ssize_t;
typedef int wint_t;
typedef int _mbstate_t;
typedef int _flock_t;
typedef int _iconv_t;
typedef int __ULong;
typedef int __FILE;
typedef int ptrdiff_t;
typedef int wchar_t;
typedef int __off_t;
typedef int __pid_t;
typedef int __loff_t;
typedef int u_char;
typedef int u_short;
typedef int u_int;
typedef int u_long;
typedef int ushort;
typedef int uint;
typedef int clock_t;
typedef int time_t;
typedef int daddr_t;
typedef int caddr_t;
typedef int ino_t;
typedef int off_t;
typedef int dev_t;
typedef int uid_t;
typedef int gid_t;
typedef int pid_t;
typedef int key_t;
typedef int ssize_t;
typedef int mode_t;
typedef int nlink_t;
typedef int fd_mask;
typedef int _types_fd_set;
typedef int clockid_t;
typedef int timer_t;
typedef int useconds_t;
typedef int suseconds_t;
typedef int FILE;
typedef int fpos_t;
typedef int cookie_read_function_t;
typedef int cookie_write_function_t;
typedef int cookie_seek_function_t;
typedef int cookie_close_function_t;
typedef int cookie_io_functions_t;
typedef int div_t;
typedef int ldiv_t;
typedef int lldiv_t;
typedef int sigset_t;
typedef int __sigset_t;
typedef int _sig_func_ptr;
typedef int sig_atomic_t;
typedef int __tzrule_type;
typedef int __tzinfo_type;
typedef int mbstate_t;
typedef int sem_t;
typedef int pthread_t;
typedef int pthread_attr_t;
typedef int pthread_mutex_t;
typedef int pthread_mutexattr_t;
typedef int pthread_cond_t;
typedef int pthread_condattr_t;
typedef int pthread_key_t;
typedef int pthread_once_t;
typedef int pthread_rwlock_t;
typedef int pthread_rwlockattr_t;
typedef int pthread_spinlock_t;
typedef int pthread_barrier_t;
typedef int pthread_barrierattr_t;
typedef int jmp_buf;
typedef int rlim_t;
typedef int sa_family_t;
typedef int sigjmp_buf;
typedef int stack_t;
typedef int siginfo_t;
typedef int z_stream;


typedef int int8_t;
typedef int uint8_t;
typedef int int16_t;
typedef int uint16_t;
typedef int int32_t;
typedef int uint32_t;
typedef int int64_t;
typedef int uint64_t;


typedef int int_least8_t;
typedef int uint_least8_t;
typedef int int_least16_t;
typedef int uint_least16_t;
typedef int int_least32_t;
typedef int uint_least32_t;
typedef int int_least64_t;
typedef int uint_least64_t;


typedef int int_fast8_t;
typedef int uint_fast8_t;
typedef int int_fast16_t;
typedef int uint_fast16_t;
typedef int int_fast32_t;
typedef int uint_fast32_t;
typedef int int_fast64_t;
typedef int uint_fast64_t;


typedef int intptr_t;
typedef int uintptr_t;


typedef int intmax_t;
typedef int uintmax_t;


typedef _Bool bool;


typedef void* MirEGLNativeWindowType;
typedef void* MirEGLNativeDisplayType;
typedef struct MirConnection MirConnection;
typedef struct MirSurface MirSurface;
typedef struct MirSurfaceSpec MirSurfaceSpec;
typedef struct MirScreencast MirScreencast;
typedef struct MirPromptSession MirPromptSession;
typedef struct MirBufferStream MirBufferStream;
typedef struct MirPersistentId MirPersistentId;
typedef struct MirBlob MirBlob;
typedef struct MirDisplayConfig MirDisplayConfig;


typedef struct xcb_connection_t xcb_connection_t;
typedef uint32_t xcb_window_t;
typedef uint32_t xcb_visualid_t;
# 3 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/pycparser/utils/fake_libc_include/string.h" 2
# 30 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h" 2

extern const char* LODEPNG_VERSION_STRING;
# 99 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
typedef enum LodePNGColorType {
  LCT_GREY = 0,
  LCT_RGB = 2,
  LCT_PALETTE = 3,
  LCT_GREY_ALPHA = 4,
  LCT_RGBA = 6,





  LCT_MAX_OCTET_VALUE = 255
} LodePNGColorType;
# 129 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
unsigned lodepng_decode_memory(unsigned char** out, unsigned* w, unsigned* h,
                               const unsigned char* in, size_t insize,
                               LodePNGColorType colortype, unsigned bitdepth);


unsigned lodepng_decode32(unsigned char** out, unsigned* w, unsigned* h,
                          const unsigned char* in, size_t insize);


unsigned lodepng_decode24(unsigned char** out, unsigned* w, unsigned* h,
                          const unsigned char* in, size_t insize);






unsigned lodepng_decode_file(unsigned char** out, unsigned* w, unsigned* h,
                             const char* filename,
                             LodePNGColorType colortype, unsigned bitdepth);


unsigned lodepng_decode32_file(unsigned char** out, unsigned* w, unsigned* h,
                               const char* filename);


unsigned lodepng_decode24_file(unsigned char** out, unsigned* w, unsigned* h,
                               const char* filename);
# 262 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
const char* lodepng_error_text(unsigned code);




typedef struct LodePNGDecompressSettings LodePNGDecompressSettings;
struct LodePNGDecompressSettings {

  unsigned ignore_adler32;
  unsigned ignore_nlen;






  size_t max_output_size;



  unsigned (*custom_zlib)(unsigned char**, size_t*,
                          const unsigned char*, size_t,
                          const LodePNGDecompressSettings*);



  unsigned (*custom_inflate)(unsigned char**, size_t*,
                             const unsigned char*, size_t,
                             const LodePNGDecompressSettings*);

  const void* custom_context;
};

extern const LodePNGDecompressSettings lodepng_default_decompress_settings;
void lodepng_decompress_settings_init(LodePNGDecompressSettings* settings);
# 338 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
typedef struct LodePNGColorMode {

  LodePNGColorType colortype;
  unsigned bitdepth;
# 358 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
  unsigned char* palette;
  size_t palettesize;
# 372 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
  unsigned key_defined;
  unsigned key_r;
  unsigned key_g;
  unsigned key_b;
} LodePNGColorMode;


void lodepng_color_mode_init(LodePNGColorMode* info);
void lodepng_color_mode_cleanup(LodePNGColorMode* info);

unsigned lodepng_color_mode_copy(LodePNGColorMode* dest, const LodePNGColorMode* source);

LodePNGColorMode lodepng_color_mode_make(LodePNGColorType colortype, unsigned bitdepth);

void lodepng_palette_clear(LodePNGColorMode* info);

unsigned lodepng_palette_add(LodePNGColorMode* info,
                             unsigned char r, unsigned char g, unsigned char b, unsigned char a);


unsigned lodepng_get_bpp(const LodePNGColorMode* info);


unsigned lodepng_get_channels(const LodePNGColorMode* info);

unsigned lodepng_is_greyscale_type(const LodePNGColorMode* info);

unsigned lodepng_is_alpha_type(const LodePNGColorMode* info);

unsigned lodepng_is_palette_type(const LodePNGColorMode* info);


unsigned lodepng_has_palette_alpha(const LodePNGColorMode* info);







unsigned lodepng_can_have_alpha(const LodePNGColorMode* info);

size_t lodepng_get_raw_size(unsigned w, unsigned h, const LodePNGColorMode* color);



typedef struct LodePNGTime {
  unsigned year;
  unsigned month;
  unsigned day;
  unsigned hour;
  unsigned minute;
  unsigned second;
} LodePNGTime;



typedef struct LodePNGInfo {

  unsigned compression_method;
  unsigned filter_method;
  unsigned interlace_method;
  LodePNGColorMode color;
# 460 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
  unsigned background_defined;
  unsigned background_r;
  unsigned background_g;
  unsigned background_b;
# 485 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
  size_t text_num;
  char** text_keys;
  char** text_strings;
# 497 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
  size_t itext_num;
  char** itext_keys;
  char** itext_langtags;
  char** itext_transkeys;
  char** itext_strings;


  unsigned time_defined;
  LodePNGTime time;


  unsigned phys_defined;
  unsigned phys_x;
  unsigned phys_y;
  unsigned phys_unit;
# 524 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
  unsigned gama_defined;
  unsigned gama_gamma;


  unsigned chrm_defined;
  unsigned chrm_white_x;
  unsigned chrm_white_y;
  unsigned chrm_red_x;
  unsigned chrm_red_y;
  unsigned chrm_green_x;
  unsigned chrm_green_y;
  unsigned chrm_blue_x;
  unsigned chrm_blue_y;






  unsigned srgb_defined;
  unsigned srgb_intent;
# 567 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
  unsigned iccp_defined;
  char* iccp_name;





  unsigned char* iccp_profile;
  unsigned iccp_profile_size;
# 597 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
  unsigned char* unknown_chunks_data[3];
  size_t unknown_chunks_size[3];

} LodePNGInfo;


void lodepng_info_init(LodePNGInfo* info);
void lodepng_info_cleanup(LodePNGInfo* info);

unsigned lodepng_info_copy(LodePNGInfo* dest, const LodePNGInfo* source);


unsigned lodepng_add_text(LodePNGInfo* info, const char* key, const char* str);
void lodepng_clear_text(LodePNGInfo* info);

unsigned lodepng_add_itext(LodePNGInfo* info, const char* key, const char* langtag,
                           const char* transkey, const char* str);
void lodepng_clear_itext(LodePNGInfo* info);


unsigned lodepng_set_icc(LodePNGInfo* info, const char* name, const unsigned char* profile, unsigned profile_size);
void lodepng_clear_icc(LodePNGInfo* info);
# 632 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
unsigned lodepng_convert(unsigned char* out, const unsigned char* in,
                         const LodePNGColorMode* mode_out, const LodePNGColorMode* mode_in,
                         unsigned w, unsigned h);






typedef struct LodePNGDecoderSettings {
  LodePNGDecompressSettings zlibsettings;


  unsigned ignore_crc;
  unsigned ignore_critical;
  unsigned ignore_end;





  unsigned color_convert;


  unsigned read_text_chunks;


  unsigned remember_unknown_chunks;




  size_t max_text_size;




  size_t max_icc_size;

} LodePNGDecoderSettings;

void lodepng_decoder_settings_init(LodePNGDecoderSettings* settings);
# 765 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
typedef struct LodePNGState {

  LodePNGDecoderSettings decoder;




  LodePNGColorMode info_raw;
  LodePNGInfo info_png;
  unsigned error;
} LodePNGState;


void lodepng_state_init(LodePNGState* state);
void lodepng_state_cleanup(LodePNGState* state);
void lodepng_state_copy(LodePNGState* dest, const LodePNGState* source);







unsigned lodepng_decode(unsigned char** out, unsigned* w, unsigned* h,
                        LodePNGState* state,
                        const unsigned char* in, size_t insize);






unsigned lodepng_inspect(unsigned* w, unsigned* h,
                         LodePNGState* state,
                         const unsigned char* in, size_t insize);
# 814 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
unsigned lodepng_inspect_chunk(LodePNGState* state, size_t pos,
                               const unsigned char* in, size_t insize);
# 853 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
unsigned lodepng_chunk_length(const unsigned char* chunk);


void lodepng_chunk_type(char type[5], const unsigned char* chunk);


unsigned char lodepng_chunk_type_equals(const unsigned char* chunk, const char* type);


unsigned char lodepng_chunk_ancillary(const unsigned char* chunk);


unsigned char lodepng_chunk_private(const unsigned char* chunk);


unsigned char lodepng_chunk_safetocopy(const unsigned char* chunk);


unsigned char* lodepng_chunk_data(unsigned char* chunk);
const unsigned char* lodepng_chunk_data_const(const unsigned char* chunk);


unsigned lodepng_chunk_check_crc(const unsigned char* chunk);


void lodepng_chunk_generate_crc(unsigned char* chunk);
# 891 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
unsigned char* lodepng_chunk_next(unsigned char* chunk, unsigned char* end);
const unsigned char* lodepng_chunk_next_const(const unsigned char* chunk, const unsigned char* end);


unsigned char* lodepng_chunk_find(unsigned char* chunk, unsigned char* end, const char type[5]);
const unsigned char* lodepng_chunk_find_const(const unsigned char* chunk, const unsigned char* end, const char type[5]);






unsigned lodepng_chunk_append(unsigned char** out, size_t* outsize, const unsigned char* chunk);







unsigned lodepng_chunk_create(unsigned char** out, size_t* outsize, unsigned length,
                              const char* type, const unsigned char* data);



unsigned lodepng_crc32(const unsigned char* buf, size_t len);
# 929 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
unsigned lodepng_inflate(unsigned char** out, size_t* outsize,
                         const unsigned char* in, size_t insize,
                         const LodePNGDecompressSettings* settings);







unsigned lodepng_zlib_decompress(unsigned char** out, size_t* outsize,
                                 const unsigned char* in, size_t insize,
                                 const LodePNGDecompressSettings* settings);
# 980 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
unsigned lodepng_load_file(unsigned char** out, size_t* outsize, const char* filename);
# 990 "/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng/lodepng.h"
unsigned lodepng_save_file(const unsigned char* buffer, size_t buffersize, const char* filename);
