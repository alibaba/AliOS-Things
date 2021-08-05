#ifndef BOARD_H
#define BOARD_H
typedef enum {
    HAAS_EDU_K1 = 0,                     /**< Playback stream */
    HAAS_EDU_K1C,                         /**< Capture stream */
    HAAS_EDU_MAX
} haasedu_board_t;

extern int haasedu_is_k1c();
void soc_systick_start(void);
void soc_systick_stop(void);

#endif /* BOARD_H */
