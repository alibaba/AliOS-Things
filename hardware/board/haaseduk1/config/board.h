#ifndef BOARD_H
#define BOARD_H

#if (CONFIG_A7_DSP_ENABLE == 1)
#define DEBUG_LASTWORD_RAM_ADDR 0x20166000
#else
#define DEBUG_LASTWORD_RAM_ADDR 0x201e6000
#endif

typedef enum {
    HAAS_EDU_K1 = 0,                     /**< Playback stream */
    HAAS_EDU_K1C,                         /**< Capture stream */
    HAAS_EDU_MAX
} haasedu_board_t;

extern int haasedu_is_k1c();
void soc_systick_start(void);
void soc_systick_stop(void);

#endif /* BOARD_H */
