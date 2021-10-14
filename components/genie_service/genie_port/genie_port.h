
#ifndef _GENIE_PORT_H_
#define _GENIE_PORT_H_

#define GENIE_DEBUG_COLOR

#ifdef GENIE_DEBUG_COLOR
#define F_BLACK  "\033[0;30m"
#define F_RED    "\033[0;31m"
#define F_GREEN  "\033[0;32m"
#define F_YELLOW "\033[0;33m"
#define F_BLUE   "\033[0;34m"
#define F_PURPLE "\033[0;35m"
#define F_DGREEN "\033[0;36m"
#define F_WHITE  "\033[0;37m"
#define F_END    "\033[0m"
#define BT_DBG_R(fmt, ...)                    \
                                if (BT_DBG_ENABLED) {                   \
                                    LOGV("GENIE",F_RED "[%s]" fmt F_END, __func__,   \
                ##__VA_ARGS__);             \
                                }
#else
#define F_BLACK
#define F_RED
#define F_GREEN
#define F_YELLOW
#define F_BLUE
#define F_PURPLE
#define F_DGREEN
#define F_WHITE
#define F_END

#define BT_DBG_R(fmt, ...)
#endif

extern uint8_t g_mesh_log_mode;

#define printk(...) \
{ \
    if(g_mesh_log_mode) { \
      printf(__VA_ARGS__); \
    }\
}

#define k_malloc malloc

#if defined(CONFIG_BT_DEBUG_LOG)
//#define BT_DBG(fmt, ...)  SYS_LOG_DBG(fmt, ##__VA_ARGS__)
//#define BT_ERR(fmt, ...) SYS_LOG_ERR(fmt, ##__VA_ARGS__)
//#define BT_WARN(fmt, ...) SYS_LOG_WRN(fmt, ##__VA_ARGS__)
//#define BT_INFO(fmt, ...) SYS_LOG_INF(fmt, ##__VA_ARGS__)
#else
//#define BT_DBG(fmt, ...)
//#define BT_ERR BT_DBG
//#define BT_WARN BT_DBG
//#define BT_INFO BT_DBG
#endif

#ifdef GENIE_DEBUG_COLOR
//#define F_RED    "\033[0;31m"
//#define F_GREEN  "\033[0;32m"
//#define F_BLUE   "\033[0;34m"
//#define F_END    "\033[0m"
#define BT_INFO_R(fmt, ...) SYS_LOG_DBG(F_RED fmt F_END, ##__VA_ARGS__);
#define BT_INFO_G(fmt, ...) SYS_LOG_DBG(F_GREEN fmt F_END, ##__VA_ARGS__);
#define BT_INFO_B(fmt, ...) SYS_LOG_DBG(F_BLUE fmt F_END, ##__VA_ARGS__);
#else
#define F_RED
#define F_GREEN
#define F_BLUE
#define F_END
#define BT_INFO_R(fmt, ...)
#define BT_INFO_G(fmt, ...)
#define BT_INFO_B(fmt, ...)
#endif

int bt_mesh_aes_encrypt(const uint8_t key[16], const uint8_t plaintext[16],
                        uint8_t enc_data[16]);
int bt_mesh_aes_decrypt(const uint8_t key[16], const uint8_t enc_data[16],
                        uint8_t dec_data[16]);

int bt_mesh_adv_stop(void);

#define hal_malloc malloc
#define hal_free   free

void genie_mesh_setup(void);

#endif
