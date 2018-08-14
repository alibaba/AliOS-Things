#include <k_api.h>
#include "stm32l4xx_hal.h"
#include "soc_init.h"
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

static HAL_StatusTypeDef st7789_write(int is_cmd, uint8_t data)
{
  uint8_t pData[2] = {0};

  if (hspi_lcd == NULL) {
    _Error_Handler(__FILE__, __LINE__);
    return HAL_ERROR;
  }
  pData[0] = data;

	if (is_cmd)
		hal_gpio_output_low(&brd_gpio_table[GPIO_LCD_DCX]);
	else
		hal_gpio_output_high(&brd_gpio_table[GPIO_LCD_DCX]);

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
      krhino_task_sleep(krhino_ms_to_ticks(st7789_cfg_script[i].data));
      break;
    case ST7789_END:
      end_script = 1;
    }
    i++;
  } while (!end_script);
}

static void st7789_reset()
{
  hal_gpio_output_high(&brd_gpio_table[GPIO_LCD_PWR]);
  hal_gpio_output_high(&brd_gpio_table[GPIO_LCD_RST]);
  krhino_task_sleep(krhino_ms_to_ticks(50));
  hal_gpio_output_low(&brd_gpio_table[GPIO_LCD_RST]);
  krhino_task_sleep(krhino_ms_to_ticks(50));
  hal_gpio_output_high(&brd_gpio_table[GPIO_LCD_RST]);
  krhino_task_sleep(krhino_ms_to_ticks(150));
}

#if 1
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

static void st7789_display_picture(void)
{
  st7789_write(1, ST7789_RAMWR);

  spec_send_fb(0x0, WIDTH * HEIGHT / 4);
  spec_send_fb(0x1111, WIDTH * HEIGHT / 4);
  spec_send_fb(0x7777, WIDTH * HEIGHT / 4);
  spec_send_fb(0xeeee, WIDTH * HEIGHT / 4);
}
#endif

/*xiehj add camera display*/
void camera_dispaly(uint16_t *data, uint32_t pixel_num)
{	
	 int i;
	int count, remain;
	uint16_t *pdata = data;

	st7789_write(1, ST7789_RAMWR);

	count = pixel_num / LCD_MAX_MEM16_BLOCK;
	remain = pixel_num % LCD_MAX_MEM16_BLOCK;
	HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);

	for (i = 0; i < count; ++i) {
		st7789_write_fb(pdata , LCD_MAX_MEM16_BLOCK << 1);
		pdata += LCD_MAX_MEM16_BLOCK;
	}
	st7789_write_fb(pdata, remain << 1);
}

#define GRAY_TO_RGB565(g) ((((uint16_t)(g)>>3)<<11)| (((uint16_t)(g)>>2)<<5) | (((uint16_t)(g)>>3)))
#define RGB565_ENDIAN(h) (((uint16_t)(h)>>8)|((uint16_t)(h)&0xff)<<8)
void camera_display_gray(uint8_t *data, uint32_t pixel_num)
{	
	int i,j;
	int count, remain;
	static uint16_t pdata[LCD_MAX_MEM16_BLOCK];

	st7789_write(1, ST7789_RAMWR);

	count = pixel_num / LCD_MAX_MEM16_BLOCK;
	remain = pixel_num % LCD_MAX_MEM16_BLOCK;
	HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);

	for (i = 0; i < count; ++i) {
    for(j=0; j<LCD_MAX_MEM16_BLOCK; j++){
      pdata[j] = RGB565_ENDIAN(GRAY_TO_RGB565(data[j]));
    }
		st7789_write_fb(pdata , LCD_MAX_MEM16_BLOCK << 1);
		data += LCD_MAX_MEM16_BLOCK;
	}

  for(j=0; j<remain; j++){
      pdata[j] = RGB565_ENDIAN(GRAY_TO_RGB565(data[j]));
  }
	st7789_write_fb(pdata, remain << 1);
}

int st7789_init()
{
  hspi_lcd = &hspi1;
  st7789_reset();
  st7789_run_cfg_script();
//  st7789_display_picture();

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

void ST7789H2_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t data)
{
  uint8_t dataB = 0;

  /* Set Cursor */
  ST7789H2_SetCursor(Xpos, Ypos);

  /* Prepare to write to LCD RAM */
  ST7789H2_WriteReg(0x2C, (uint8_t*)NULL, 0);   /* RAM write data command */

  /* Write RAM data */
  dataB = (uint8_t)(data >> 8);
  LcdWriteData(dataB);
  dataB = (uint8_t)data;
  LcdWriteData(dataB);
}

uint8_t endian_buffer[480];

void ST7789H2_WriteLine(uint16_t Xpos, uint16_t Ypos, uint16_t *RGBCode, uint16_t pointNum)
{
  int i = 0;

  /* Set Cursor */
  ST7789H2_SetCursor(Xpos, Ypos);

  /* Prepare to write to LCD RAM */
  ST7789H2_WriteReg(0x2C, (uint8_t*)NULL, 0);   /* RAM write data command */

  for (i = 0; i < pointNum; i++) {
    endian_buffer[2*i] = (uint8_t)(RGBCode[i] >> 8);
    endian_buffer[2*i + 1] = (uint8_t)RGBCode[i];
  }

  /* Write RAM data */
  LcdWriteDataMultiple(endian_buffer, pointNum*2);
}

/********************************************************************
*
*       LcdWriteReg
*
* Function description:
*   Sets display register
*/
void LcdWriteReg(uint8_t Data) 
{
  hal_gpio_output_low(&brd_gpio_table[GPIO_LCD_DCX]);
  HAL_SPI_Transmit(&hspi1, &Data, 1, 10);
}

/********************************************************************
*
*       LcdWriteData
*
* Function description:
*   Writes a value to a display register
*/
void LcdWriteData(uint8_t Data) 
{
  hal_gpio_output_high(&brd_gpio_table[GPIO_LCD_DCX]);
  HAL_SPI_Transmit(&hspi1, &Data, 1, 10);
}

/********************************************************************
*
*       LcdWriteDataMultiple
*
* Function description:
*   Writes multiple values to a display register.
*/
void LcdWriteDataMultiple(uint8_t * pData, int NumItems) 
{
  HAL_GPIO_WritePin(LCD_DCX_GPIO_Port, LCD_DCX_Pin, GPIO_PIN_SET);
  HAL_SPI_Transmit(&hspi1, pData, NumItems, 10);
}
