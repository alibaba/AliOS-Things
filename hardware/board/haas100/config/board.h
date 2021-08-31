#ifndef BOARD_H
#define BOARD_H

#if (CONFIG_A7_DSP_ENABLE == 1)
#define DEBUG_LASTWORD_RAM_ADDR 0x20166000
#else
#define DEBUG_LASTWORD_RAM_ADDR 0x201e6000
#endif

void soc_systick_start(void);
void soc_systick_stop(void);

#endif /* BOARD_H */
