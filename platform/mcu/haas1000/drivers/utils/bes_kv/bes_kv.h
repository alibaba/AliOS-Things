
#ifndef BES_KV_H
#define BES_KV_H


#if defined(__cplusplus)
extern "C" {
#endif

#include "cmsis_os.h"

#ifndef u8
#define u8  unsigned char
#endif
#ifndef u16
#define u16  unsigned short
#endif
#ifndef u32
#define u32  unsigned int
#endif

#define  MAX_KEY_NUMBER  32
#define  MAX_NEME_LEN    32
#define  ALIAS_BASE      64

/**
 * Key-value item description
 *
 * key_name:    Name of this key, string type.
 * real_value:  Real time value of this key, its value will be cleared after every time it is read.
 * accu_value:  Accumulate value of this key.
 * interval:    Interval time keys were dumped.
 * alias:       Alise of the key_name, 0 for not used.
 *
 */
typedef struct _bes_kv_t
{
    u8   key_name[MAX_NEME_LEN];
    u32  real_value;
    u32  accu_value;
    u32  alias;
} bes_kv_t;

/**
 * struct bes_global_stat - Statistics of the golbal kv.
 * @en: enable/disable the task which will output statistics results periodically
 * @interval_sec: period time
 * @thread_id: task's id.
 * @bes_kv_t: struct _bes_kv_t.
 *
 */
typedef struct bes_global_stat {
    u8     en;
    u8     interval_sec;
    osThreadId  thread_id;
    bes_kv_t bes_key[MAX_KEY_NUMBER];
} bes_global_stat_t;



/**
 * @brief Initialize the kv module
 *
 * @retrun 0 on success, otherwise will be failed.
 *
 */
int bes_kv_init(void);

/**
 * Add the KV pair by its key.
 *
 * @param[in]  key     the the name of the KV pair.
 *
 * @return  0 on success, negative error on failure.
 */
int bes_kv_add(char *key);

/**
 * Delete the KV pair by its key index.
 *
 * @param[in]  index  the index mapped to its key of the KV pair.
 *
 * @return  0 on success, negative error on failure.
 */
int bes_kv_item_delete(int index);

/**
 * Add a new KV pair.
 *
 * @param[in]  index  the index mapped to its key of the KV pair.
 * @param[in]  val    the value of the KV pair.
 *
 * @return  total lost data len.
 */
int bes_kv_item_set(int index, u32 val);

/**
 * This function is used to enable/disable the statistics of the global kv pair.
 *
 * @param[in]	en Set to 1 to enable the statistics ,0 to disable it.
 * @param[in]	interval_sec Time of the statistics in seconds. 
 * @return	0 ,OK; negative value,  ERROR
 * @note	If enabled, the statistics information will be output throuth a default log uart.
 */
int bes_kv_item_get(u8 en, u32 interval_sec);

#if defined(__cplusplus)
}
#endif

#endif /* BES_KV_H */
