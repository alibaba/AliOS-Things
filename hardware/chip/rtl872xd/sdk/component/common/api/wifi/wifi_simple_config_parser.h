#ifndef __SIMPLE_CONFIG_H__
#define __SIMPLE_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


  /*  This macro means user take simple config
   *  lib to another platform such as linux, and
   *  have no rom crypto libs of simple config,
   *  so we take simple_config_crypto as a sw lib 
   *  This macro is used by Realtek internal to generate simple config lib
   *  Please delete this macro after generation.
   */
#define SIMPLE_CONFIG_PLATFORM_LIB 0

#include "platform_opts.h"
#include "autoconf.h"



/* platform related settings */
#if (defined(CONFIG_PLATFORM_8195A)|| defined(CONFIG_PLATFORM_8711B)|| defined(CONFIG_PLATFORM_8721D) || defined(CONFIG_PLATFORM_8195BHP) || defined(CONFIG_PLATFORM_8710C))
#undef u32
#undef s32
#undef u8
#undef s8
#undef u16
#undef s16
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned char u8;
typedef char s8;
typedef unsigned short int u16;
typedef signed short int s16;
#else
#include "osdep_service.h"
#endif

typedef int  (*simple_config_printf_fn) (char const * fmt, ...);
typedef void* (*simple_config_memset_fn) (void *dst0, s32 Val, u32 length);
typedef void* (*simple_config_memcpy_fn) ( void *s1, const void *s2, u32 n );
typedef u32 (*simple_config_strlen_fn) (const char *s);
typedef char * (*simple_config_strcpy_fn) (char  *dest, const char  *src);
typedef void (*simple_config_free_fn) (u8 *pbuf, u32 sz);
typedef u8*  (*simple_config_zmalloc_fn) (u32 sz);
typedef u8* (*simple_config_malloc_fn) (u32 sz);
typedef int (*simple_config_memcmp_fn) (const void *av, const void *bv, u32 len);
typedef u32 (*simple_config_ntohl_fn)(u32 x);



struct simple_config_lib_config {
	simple_config_printf_fn printf_fn;
	simple_config_memset_fn memset_fn;
	simple_config_memcpy_fn memcpy_fn;
	simple_config_strlen_fn strlen_fn;
	simple_config_strcpy_fn strcpy_fn;
	simple_config_free_fn free_fn;
	simple_config_zmalloc_fn zmalloc_fn;
	simple_config_malloc_fn malloc_fn;
	simple_config_memcmp_fn memcmp_fn;
	simple_config_ntohl_fn ntohl_fn;


	int *is_promisc_callback_unlock;

};

struct fmt_info {
	u8 fmt_channel[2];
	u8 fmt_hidden[1];
	u8 fmt_bssid[6];
};

struct dsoc_info {
	unsigned char dsoc_ssid[33];
	unsigned char dsoc_length;
};


#pragma pack(1)
struct rtk_test_sc {
	/* API exposed to user */
	unsigned char		ssid[33];
	unsigned char		password[65];	
	unsigned int		ip_addr;
};
// for softAP mode
typedef enum {
    SOFTAP_ERROR = -1,
    SOFTAP_INIT,
    SOFTAP_RECV_A,
    SOFTAP_HANDSHAKE_DONE,
    SOFTAP_DECODE_SUCCESS,
} SC_softAP_status;

#pragma pack(1)
typedef struct _SC_softAP_decode_ctx {
    u8      nonceA[16];
    u8      nonceB[32];
    u8      mac[6];
    SC_softAP_status    softAP_decode_status;
} SC_softAP_decode_ctx;

/* expose data */
extern s32 is_promisc_callback_unlock;
extern u8 g_bssid[6];
extern u8 get_channel_flag;
extern u8 g_security_mode;

/* expose API */
extern s32 rtk_sc_init(char *custom_pin_code, struct simple_config_lib_config* config);
extern int rtl_pre_parse(u8 *mac_addr, u8 *buf, void *userdata, u8 **da, u8 **sa, unsigned int *len);
extern s32 rtk_start_parse_packet(u8 *da, u8 *sa, s32 len,  void * user_data, void *backup_sc);
extern SC_softAP_status softAP_simpleConfig_parse(unsigned char *buf, int len, void *backup_sc_ctx, void *psoftAP_ctx);
extern void rtk_restart_simple_config(void);
extern void rtk_sc_deinit(void);
extern void wifi_enter_promisc_mode(void);
extern void whc_fix_channel(void);
extern void whc_unfix_channel(void);


#ifdef __cplusplus
}
#endif

#endif /* __SIMPLE_CONFIG_H__*/
