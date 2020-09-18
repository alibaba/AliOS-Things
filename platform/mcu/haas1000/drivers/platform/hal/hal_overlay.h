/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef __HAL_OVERLAY_H__
#define __HAL_OVERLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_OVERLAY_TEXT0_LOC       __attribute__((section(".overlay_text0")))
#define HAL_OVERLAY_TEXT1_LOC       __attribute__((section(".overlay_text1")))
#define HAL_OVERLAY_TEXT2_LOC       __attribute__((section(".overlay_text2")))
#define HAL_OVERLAY_TEXT3_LOC       __attribute__((section(".overlay_text3")))
#define HAL_OVERLAY_TEXT4_LOC       __attribute__((section(".overlay_text4")))
#define HAL_OVERLAY_TEXT5_LOC       __attribute__((section(".overlay_text5")))
#define HAL_OVERLAY_TEXT6_LOC       __attribute__((section(".overlay_text6")))
#define HAL_OVERLAY_TEXT7_LOC       __attribute__((section(".overlay_text7")))

#define HAL_OVERLAY_RODATA0_LOC     __attribute__((section(".overlay_rodata0")))
#define HAL_OVERLAY_RODATA1_LOC     __attribute__((section(".overlay_rodata1")))
#define HAL_OVERLAY_RODATA2_LOC     __attribute__((section(".overlay_rodata2")))
#define HAL_OVERLAY_RODATA3_LOC     __attribute__((section(".overlay_rodata3")))
#define HAL_OVERLAY_RODATA4_LOC     __attribute__((section(".overlay_rodata4")))
#define HAL_OVERLAY_RODATA5_LOC     __attribute__((section(".overlay_rodata5")))
#define HAL_OVERLAY_RODATA6_LOC     __attribute__((section(".overlay_rodata6")))
#define HAL_OVERLAY_RODATA7_LOC     __attribute__((section(".overlay_rodata7")))

#define HAL_OVERLAY_DATA0_LOC       __attribute__((section(".overlay_data0")))
#define HAL_OVERLAY_DATA1_LOC       __attribute__((section(".overlay_data1")))
#define HAL_OVERLAY_DATA2_LOC       __attribute__((section(".overlay_data2")))
#define HAL_OVERLAY_DATA3_LOC       __attribute__((section(".overlay_data3")))
#define HAL_OVERLAY_DATA4_LOC       __attribute__((section(".overlay_data4")))
#define HAL_OVERLAY_DATA5_LOC       __attribute__((section(".overlay_data5")))
#define HAL_OVERLAY_DATA6_LOC       __attribute__((section(".overlay_data6")))
#define HAL_OVERLAY_DATA7_LOC       __attribute__((section(".overlay_data7")))

#define  INVALID_OVERLAY_ADDR 0xffffffff
enum HAL_OVERLAY_ID_T {
    HAL_OVERLAY_ID_0,
    HAL_OVERLAY_ID_1,
    HAL_OVERLAY_ID_2,
    HAL_OVERLAY_ID_3,
    HAL_OVERLAY_ID_4,
    HAL_OVERLAY_ID_5,
    HAL_OVERLAY_ID_6,
    HAL_OVERLAY_ID_7,

    HAL_OVERLAY_ID_QTY,
    HAL_OVERLAY_ID_IN_CFG,
};

enum HAL_OVERLAY_RET_T {
    HAL_OVERLAY_RET_OK,
    HAL_OVERLAY_RET_BAD_ID,
    HAL_OVERLAY_RET_IN_CFG,
    HAL_OVERLAY_RET_IN_USE,
};

#ifndef NO_OVERLAY
enum HAL_OVERLAY_RET_T hal_overlay_load(enum HAL_OVERLAY_ID_T id);

enum HAL_OVERLAY_RET_T hal_overlay_unload(enum HAL_OVERLAY_ID_T id);

/*
 * get the overlay's text start address
 */
uint32_t hal_overlay_get_text_address(void);

/*
 * get the whole size of the overlay text
 */
uint32_t hal_overlay_get_text_all_size(void);
/*
 * get the segment size of one overlay text
 */
uint32_t hal_overlay_get_text_size(enum HAL_OVERLAY_ID_T id);
/*
 * Use the free space of one segement, this function 
 * return the free address of space
 */
uint32_t hal_overlay_get_text_free_addr(enum HAL_OVERLAY_ID_T id);
/*
 * get the free size for one overlay text
 */
uint32_t hal_overlay_get_text_free_size(enum HAL_OVERLAY_ID_T id);
/*
 * acquire one overlay segment
 */
void hal_overlay_acquire(enum HAL_OVERLAY_ID_T id);
/*
 * release one overlay segment
 */
void hal_overlay_release(enum HAL_OVERLAY_ID_T id);
/*
 * check if any overlay segment is used
 */
bool hal_overlay_is_used(void);

#else

static inline enum HAL_OVERLAY_RET_T hal_overlay_load(enum HAL_OVERLAY_ID_T id)
{ return HAL_OVERLAY_RET_OK; }

static inline enum HAL_OVERLAY_RET_T hal_overlay_unload(enum HAL_OVERLAY_ID_T id)
{ return HAL_OVERLAY_RET_OK; }

static inline uint32_t hal_overlay_get_text_address(void)
{ return INVALID_OVERLAY_ADDR;}

static inline uint32_t hal_overlay_get_text_all_size(void)
{ return 0;}

static inline uint32_t hal_overlay_get_text_size(enum HAL_OVERLAY_ID_T id)
{ return 0;}

static inline uint32_t hal_overlay_get_text_free_addr(enum HAL_OVERLAY_ID_T id)
{ return INVALID_OVERLAY_ADDR;}

static inline uint32_t hal_overlay_get_text_free_size(enum HAL_OVERLAY_ID_T id)
{ return 0;}

static inline void hal_overlay_acquire(enum HAL_OVERLAY_ID_T id) { return;}

static inline void hal_overlay_release(enum HAL_OVERLAY_ID_T id) { return;}

static inline bool hal_overlay_is_used(void) { return false;}

#endif /*NO_OVERLAY*/

#ifdef __cplusplus
}
#endif

#endif /*__HAL_OVERLAY_H__*/

