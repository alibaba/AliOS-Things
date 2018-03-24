#ifndef __ST7789_H
#define __ST7789_H

#define WIDTH		240
#define HEIGHT		240
#define BPP		16

/* Init script function */
struct st7789_function {
  uint8_t cmd;
  uint16_t data;
};

/* Init script commands */
enum st7789_cmd {
  ST7789_START,
  ST7789_END,
  ST7789_CMD,
  ST7789_DATA,
  ST7789_DELAY
};

/* ST7789 Commands */
#define ST7789_CASET	0x2A
#define ST7789_RASET	0x2B
#define ST7789_RAMWR	0x2C
#define ST7789_RAMRD	0x2E

int st7789_init();
#endif /* __ST7789_H */