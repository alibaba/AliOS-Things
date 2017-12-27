#include "stm32l4xx_hal.h"
#include "st7789.h"
#include <stdlib.h>
#include <string.h>

extern void LcdWriteReg(uint8_t Data);
extern void LcdWriteData(uint8_t Data);
extern void LcdWriteDataMultiple(uint8_t * pData, int NumItems);

extern SPI_HandleTypeDef hspi1;
static SPI_HandleTypeDef *hspi_lcd = NULL;

static struct st7789_function st7789_cfg_script[] = {
  {ST7789_START, ST7789_START},
  {ST7789_CMD, 0x11},
  {ST7789_DELAY, 120},
  {ST7789_CMD, 0x36},
  {ST7789_DATA, 0x00},
  {ST7789_CMD, 0x3a},
  {ST7789_DATA, 0x05},
  {ST7789_CMD, 0xb2},
  {ST7789_DATA, 0x0c},
  {ST7789_DATA, 0x0c},
  {ST7789_DATA, 0x00},
  {ST7789_DATA, 0x33},
  {ST7789_DATA, 0x33},
  {ST7789_CMD, 0xb7},
  {ST7789_DATA, 0x72},
  {ST7789_CMD, 0xbb},
  {ST7789_DATA, 0x3d},
  {ST7789_CMD, 0xc2},
  {ST7789_DATA, 0x01},
  {ST7789_CMD, 0xc3},
  {ST7789_DATA, 0x19},
  {ST7789_CMD, 0xc4},
  {ST7789_DATA, 0x20},
  {ST7789_CMD, 0xc6},
  {ST7789_DATA, 0x0f},
  {ST7789_CMD, 0xd0},
  {ST7789_DATA, 0xa4},
  {ST7789_DATA, 0xa1},
  {ST7789_CMD, 0xe0},
  {ST7789_DATA, 0x70},
  {ST7789_DATA, 0x04},
  {ST7789_DATA, 0x08},
  {ST7789_DATA, 0x09},
  {ST7789_DATA, 0x09},
  {ST7789_DATA, 0x05},
  {ST7789_DATA, 0x2a},
  {ST7789_DATA, 0x33},
  {ST7789_DATA, 0x41},
  {ST7789_DATA, 0x07},
  {ST7789_DATA, 0x13},
  {ST7789_DATA, 0x13},
  {ST7789_DATA, 0x29},
  {ST7789_DATA, 0x2f},
  {ST7789_CMD, 0xe1},
  {ST7789_DATA, 0x70},
  {ST7789_DATA, 0x03},
  {ST7789_DATA, 0x09},
  {ST7789_DATA, 0x0a},
  {ST7789_DATA, 0x09},
  {ST7789_DATA, 0x06},
  {ST7789_DATA, 0x2b},
  {ST7789_DATA, 0x34},
  {ST7789_DATA, 0x41},
  {ST7789_DATA, 0x07},
  {ST7789_DATA, 0x12},
  {ST7789_DATA, 0x14},
  {ST7789_DATA, 0x28},
  {ST7789_DATA, 0x2e},
  {ST7789_CMD, 0x21},
  {ST7789_CMD, 0x29},
  {ST7789_CMD, 0x2a},
  {ST7789_DATA, 0x00},
  {ST7789_DATA, 0x00},
  {ST7789_DATA, 0x00},
  {ST7789_DATA, 0xef},
  {ST7789_CMD, 0x2b},
  {ST7789_DATA, 0x00},
  {ST7789_DATA, 0x00},
  {ST7789_DATA, 0x00},
  {ST7789_DATA, 0xef},
  {ST7789_CMD, 0x2c},
  {ST7789_END, ST7789_END},
};

const uint8_t marvell_logo[1][64][4] =
{
  { // marvell
    {0x01, 0x00, 0x00, 0x00}, {0x03, 0x00, 0x00, 0x00}, {0x07, 0x00, 0x00, 0x00}, {0x0f, 0x00, 0x00, 0x00},
    {0x1f, 0x00, 0x00, 0x00}, {0x3f, 0x00, 0x00, 0x00}, {0x7f, 0x00, 0x00, 0x00}, {0xff, 0x00, 0x00, 0x00},
    {0xff, 0x01, 0x00, 0x00}, {0xff, 0x03, 0x00, 0x00}, {0xff, 0x07, 0x00, 0x00}, {0xff, 0x0f, 0x00, 0x00}, // 11
    {0xff, 0x1f, 0x00, 0x00}, {0xff, 0x3f, 0x00, 0x00}, {0xfe, 0x7f, 0x00, 0x00}, {0xfc, 0xff, 0x00, 0x00},
    {0xf8, 0xff, 0x01, 0x00}, {0xf0, 0xff, 0x03, 0x00}, {0xe0, 0xff, 0x07, 0x00}, {0xc0, 0xff, 0x0f, 0x00},
    {0x80, 0xff, 0x1f, 0x00}, {0x00, 0xff, 0x3f, 0x00}, {0x00, 0xfe, 0x7f, 0x00}, {0x01, 0xfc, 0xff, 0x00},
    {0x03, 0xf8, 0xff, 0x01}, {0x77, 0xf7, 0xff, 0x03}, {0x5d, 0x15, 0x00, 0x02}, {0x5d, 0x15, 0x00, 0x02},
    {0x5d, 0x15, 0x00, 0x02}, {0x5d, 0x15, 0x00, 0x02}, {0xdd, 0x15, 0x00, 0x02}, {0xdd, 0x15, 0x00, 0x02},
    {0xdd, 0x15, 0x00, 0x02}, {0xdd, 0x15, 0x00, 0x02}, {0xdd, 0x15, 0x00, 0x02}, {0xdd, 0x1d, 0x00, 0x02},
    {0xdd, 0x1d, 0x00, 0x02}, {0xdd, 0x1d, 0x00, 0x02}, {0xff, 0xff, 0xff, 0x03}, {0xfc, 0xff, 0x00, 0x00}, // 39
    {0xf8, 0xff, 0x01, 0x00}, {0xf0, 0xff, 0x03, 0x00}, {0xe0, 0xff, 0x07, 0x00}, {0xc0, 0xff, 0x0f, 0x00},
    {0x80, 0xff, 0x1f, 0x00}, {0x00, 0xff, 0x3f, 0x00}, {0x00, 0xfe, 0x7f, 0x00}, {0x00, 0xfc, 0xff, 0x00},
    {0x00, 0xf8, 0xff, 0x01}, {0x77, 0xf7, 0xff, 0x03}, {0x55, 0x15, 0x00, 0x02}, {0x55, 0x15, 0x00, 0x02},
    {0x55, 0x15, 0x00, 0x02}, {0x55, 0x15, 0x00, 0x02}, {0x55, 0x15, 0x00, 0x02}, {0x55, 0x15, 0x00, 0x02},
    {0x55, 0x15, 0x00, 0x02}, {0x55, 0x15, 0x00, 0x02}, {0x55, 0x15, 0x00, 0x02}, {0x55, 0x15, 0x00, 0x02},
    {0x55, 0x15, 0x00, 0x02}, {0x55, 0x15, 0x00, 0x02}, {0x55, 0x15, 0x00, 0x02}, {0x77, 0xf7, 0xff, 0x03}
  }
};

static HAL_StatusTypeDef st7789_write(int is_cmd, uint8_t data)
{
  uint8_t pData[2] = {0};

  if (hspi_lcd == NULL) {
    _Error_Handler(__FILE__, __LINE__);
    return HAL_ERROR;
  }
  pData[0] = data;
  if (is_cmd)
    HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_RESET);
  else
    HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);

  return HAL_SPI_Transmit(hspi_lcd, pData, 1, HAL_MAX_DELAY);
}

static HAL_StatusTypeDef st7789_write_fb(uint16_t *data, uint16_t size)
{
  if (hspi_lcd == NULL) {
    _Error_Handler(__FILE__, __LINE__);
    return HAL_ERROR;
  }

  return HAL_SPI_Transmit(hspi_lcd, (uint8_t *)data, size, HAL_MAX_DELAY);
}

static void st7789_run_cfg_script()
{
  uint8_t data[2] = {0};
  int i = 0;
  int end_script = 0;

  do {
    switch (st7789_cfg_script[i].cmd) {
    case ST7789_START:
      break;
    case ST7789_CMD:
      data[0] = st7789_cfg_script[i].data & 0xff;
      st7789_write(1, data[0]);
      break;
    case ST7789_DATA:
      data[0] = st7789_cfg_script[i].data & 0xff;
      st7789_write(0, data[0]);
      break;
    case ST7789_DELAY:
      HAL_Delay(st7789_cfg_script[i].data);
      break;
    case ST7789_END:
      end_script = 1;
    }
    i++;
  } while (!end_script);
}

static void st7789_reset()
{
  HAL_GPIO_WritePin(LCD_PWR_GPIO_Port, LCD_PWR_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(50);
  /* Reset controller */
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(50);
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(150);
}

static void st7789_set_addr_win(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
  uint8_t col_data[4] = {0};
  uint8_t row_data[4] = {0};

  col_data[0] = xs >> 8 & 0xff;
  col_data[1] = xs & 0xff;
  col_data[2] = xe >> 8 & 0xff;
  col_data[3] = xe & 0xff;
  row_data[0] = ys >> 8 & 0xff;
  row_data[1] = ys & 0xff;
  row_data[2] = ye >> 8 & 0xff;
  row_data[3] = ye & 0xff;
  st7789_write(1, ST7789_CASET);
  st7789_write(0, col_data[0]);
  st7789_write(0, col_data[1]);
  st7789_write(0, col_data[2]);
  st7789_write(0, col_data[3]);
  st7789_write(1, ST7789_RASET);
  st7789_write(0, row_data[0]);
  st7789_write(0, row_data[1]);
  st7789_write(0, row_data[2]);
  st7789_write(0, row_data[3]);
}

#define LCD_MAX_MEM16_BLOCK             (1 << 6)
#define LCD_PIXEL_PER_BLOCK             (LCD_MAX_MEM16_BLOCK >> 1)

static void spec_send_fb(uint16_t color, uint16_t pixel_num)
{
  int i;
  int count, remain;
  uint16_t real_mem[LCD_MAX_MEM16_BLOCK] = {0};

  for (i = 0; i < LCD_MAX_MEM16_BLOCK; ++i) {
    real_mem[i] = color;
  }
  HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);
  if (pixel_num <= LCD_MAX_MEM16_BLOCK) {
    st7789_write_fb(real_mem, pixel_num << 1);
  } else {
    count = pixel_num / LCD_MAX_MEM16_BLOCK;
    remain = pixel_num % LCD_MAX_MEM16_BLOCK;
    for (i = 0; i < count; ++i) {
      st7789_write_fb(real_mem, LCD_MAX_MEM16_BLOCK << 1);
    }
    st7789_write_fb(real_mem, remain << 1);
  }
}

static void st7789_display_picture(uint8_t p_x, uint8_t line, uint8_t W, uint8_t H,
  uint8_t *buf, uint16_t txt_color, uint16_t bg_color){
  int i,j,k;
  uint8_t c;
  int index, size;

//  st7789_set_addr_win(line, p_x, (line + H - 1), (p_x + W - 1));
//  st7789_set_addr_win(0, 0, 0xef, 0xef);
  st7789_write(1, ST7789_RAMWR);
  index = 0;
#if 0
  for (i = 0; i < W; i++) {
    for (k = (H / 8 - 1); k >= 0; k--) {
      c = *buf++;
      for (j = (8 - 1); j >= 0; j--) {
        if ((1 << (7 - j)) & c) {
          spec_send_fb(txt_color, 1);
        } else {
          spec_send_fb(bg_color, 1);
        }
        index++;
      }
    }
  }
  size = index;
  st7789_write_fb(real_mem, size);
#else
  spec_send_fb(0x0, WIDTH * HEIGHT / 4);
  spec_send_fb(0x1111, WIDTH * HEIGHT / 4);
  spec_send_fb(0x7777, WIDTH * HEIGHT / 4);
  spec_send_fb(0xeeee, WIDTH * HEIGHT / 4);
#endif
}

static void st7789_draw_logo() {
  st7789_display_picture(0x20, 0x30, 0x40, 0x20, (uint8_t *)marvell_logo, 0xffff, 0x0000);
}

int st7789_init()
{
  hspi_lcd = &hspi1;
  st7789_reset();
  st7789_run_cfg_script();
  st7789_draw_logo();

  return HAL_OK;
}

void ST7789H2_WriteReg(uint8_t Command, uint8_t *Parameters, uint8_t NbParameters)
{
  uint8_t   i;

  /* Send command */
  LcdWriteReg(Command);
  
  /* Send command's parameters if any */
  for (i=0; i<NbParameters; i++)
  {
    LcdWriteData(Parameters[i]);
  }
}

void ST7789H2_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
  uint8_t   parameter[4];
  /* CASET: Comumn Addrses Set */
  parameter[0] = 0x00;     
  parameter[1] = 0x00 + Xpos;
  parameter[2] = 0x00;
  parameter[3] = 0xEF + Xpos;
  ST7789H2_WriteReg(0x2A, parameter, 4);
  /* RASET: Row Addrses Set */  
  parameter[0] = 0x00;
  parameter[1] = 0x00 + Ypos;
  parameter[2] = 0x00;
  parameter[3] = 0xEF + Ypos;
  ST7789H2_WriteReg(0x2B, parameter, 4);
}

uint8_t black_gui[480] = {0};

void BSP_LCD_Clear(uint16_t Color)
{
  uint32_t counter = 0;
  uint32_t y_size = 0;

	memset(black_gui, 0xFF, sizeof(black_gui));

  for (counter = 0; counter < 240; counter++)
  {
		/* Set Cursor */
		ST7789H2_SetCursor(0, counter); 
		
		/* Prepare to write to LCD RAM */
		ST7789H2_WriteReg(0x2C, (uint8_t*)NULL, 0);   /* RAM write data command */
		
		LcdWriteDataMultiple(black_gui, 480);	 
  }
}
