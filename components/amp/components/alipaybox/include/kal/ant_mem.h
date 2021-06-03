/**
 * ant_mem.h
 *
 * ÄÚ´æ¹ÜÀí½Ó¿Ú
 */
#ifndef __ANT_MEM_H__
#define __ANT_MEM_H__

#include "ant_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ÄÚ´æÄ£¿é³õÊ¼»¯
 * @return  0£º ³õÊ¼»¯³É¹¦
 *         -1£º ³õÊ¼»¯Ê§°Ü
 */
ant_s32 ant_malloc_init(void);

/**
 * ·ÖÅäÄÚ´æ
 *
 * @param size ÄÚ´æ´óĞ¡
 *
 * @return   ·Ç0: ³É¹¦£¬·µ»Ø·ÖÅäµÄÄÚ´æÖ¸Õë
 *         NULL: Ê§°Ü£¬·µ»Ø¿ÕÖ¸Õë
 */
void *  ant_malloc(ant_u32 size);

/**
 * ·ÖÅäÄÚ´æ¿é²¢Çå0
 *
 * @param n ÄÚ´æ¿é¸öÊı
 * @param size ÄÚ´æ¿é´óĞ¡
 * @return   ·Ç0: ³É¹¦£¬·µ»Ø·ÖÅäµÄÄÚ´æÖ¸Õë
 *         NULL: Ê§°Ü£¬·µ»Ø¿ÕÖ¸Õë
 */
void *  ant_calloc(ant_u32 n, ant_u32 size);

/**
 * ÖØĞÂ·ÖÅäÄÚ´æ
 *
 * @param p ÒÑ·ÖÅäÄÚ´æÖ¸Õë
 * @param new_size ĞÂÄÚ´æ´óĞ¡
 *
 * @return   ·Ç0: ³É¹¦£¬·µ»Ø·ÖÅäµÄÄÚ´æÖ¸Õë
 *         NULL: Ê§°Ü£¬·µ»Ø¿ÕÖ¸Õë
 */
void *  ant_realloc(void *p, ant_u32 new_size);

/**
 * ÊÍ·ÅÄÚ´æ
 *
 * @param p ĞèÒªÊÍ·ÅµÄÄÚ´æÖ¸Õë
 */
void    ant_free(void *p);

/**
 * ·ÖÅä¶ÔÆëÄÚ´æ
 *
 * @param size Ğè·ÖÅäÄÚ´æµÄ´óĞ¡
 * @param align ¶ÔÆë×Ö½ÚÊı£¬±ØĞëÊÇ4µÄ±¶Êı
 * @return   ·Ç0: ³É¹¦£¬·µ»Ø·ÖÅäµÄÄÚ´æÖ¸Õë
 *         NULL: Ê§°Ü£¬·µ»Ø¿ÕÖ¸Õë
 */
void *  ant_malloc_align(ant_u32 size, ant_u32 align);

/**
 * è·å–å†…å­˜çŠ¶æ€
 *
 * @param total è¿”å›æ€»å†…å­˜å¤§å°
 * @param avail è¿”å›å¯ç”¨å†…å­˜å¤§å°
 * @param max_block_size è¿”å›å¯åˆ†é…æœ€å¤§å†…å­˜å—å¤§å°
 *
 * @return 0 : è·å–æˆåŠŸ
 *        -1 : è·å–å¤±è´¥
 */
ant_s32 ant_mem_get_stats(ant_u32 *total, ant_u32 *avail, ant_u32 *max_block_size);

/**
 * è¿”å›å†…å­˜æ˜¯å¦å­˜åœ¨æ³„æ¼
 *
 *
 * @return ANT_TRUE  : å­˜åœ¨å†…å­˜æ³„æ¼
 *         ANT_FALSE : ä¸å­˜åœ¨å†…å­˜æ³„æ¼
 */
ant_bool ant_mem_is_leak(void);


/**
 * »ñÈ¡ÏµÍ³Ê£ÓàÄÚ´æ
 *
 * @return ·µ»ØÏµÍ³Ê£ÓàÄÚ´æ
 */
ant_u32 ant_get_remain_memory_size(void);

ant_u32 ant_get_total_memory_size(void);

#define ALLOC_TYPE(p, type) type *p = (type *)ant_malloc(sizeof(type)); memset(p, 0, sizeof(type))
#define ALLOC_TYPE_ARRAY(p, type, n) type *p = (type *)ant_calloc(sizeof(type), (n))

#ifdef __cplusplus
}
#endif

#endif

