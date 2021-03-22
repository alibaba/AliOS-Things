/* -------------------------------------------------------------------------- 
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Name:    GLCD_MCB1800.c
 * Purpose: Graphic LCD interface (240x320 pixels) for Graphic LCD with
 *          SPI interface (in RGB mode) for MCB1800 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

#include "LPC18xx.h"
#include "SCU_LPC18xx.h"
#include "GPIO_LPC18xx.h"
#include "GLCD_Config.h"
#include "Board_GLCD.h"
#include "Driver_SPI.h"

/* GLCD pins:
   - LCD_VD19  : P4_1
   - LCD_VD3   : P4_2
   - LCD_VSYNC : P4_5
   - LCD_EN    : P4_6
   - LCD_DOTCLK: P4_7
   - LCD_VD11  : P4_9
   - LCD_VD10  : P4_10
   - LCD_BL_EN : P7_0  = GPIO3[8]
   - LCD_HSYNC : P7_6
   - LCD_VD12  : P8_3
   - LCD_VD7   : P8_4
   - LCD_VD6   : P8_5
   - LCD_VD5   : P8_6
   - LCD_VD4   : P8_7
   - LCD_VD23  : PB_0
   - LCD_VD22  : PB_1
   - LCD_VD21  : PB_2
   - LCD_VD20  : PB_3
   - LCD_VD15  : PB_4
   - LCD_VD14  : PB_5
   - LCD_VD13  : PB_6             */

#define     GLCD_PIN_NUMBER             21

/* GLCD GPIO definitions */
const GPIO_ID GLCD_GPIO[] = {
  { 3, 8 }          /* Only backlight control is used as GPIO pin */
};

/* GLCD pin definitions */
const PIN_ID GLCD_PIN[] = {
  {  4,  1, (SCU_CFG_MODE_FUNC5 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  4,  2, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  4,  5, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  4,  6, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  4,  7, (SCU_CFG_MODE_FUNC0 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  4,  9, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  4, 10, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  7,  0, (SCU_CFG_MODE_FUNC0 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  7,  6, (SCU_CFG_MODE_FUNC3 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  8,  3, (SCU_CFG_MODE_FUNC3 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  8,  4, (SCU_CFG_MODE_FUNC3 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  8,  5, (SCU_CFG_MODE_FUNC3 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  8,  6, (SCU_CFG_MODE_FUNC3 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  {  8,  7, (SCU_CFG_MODE_FUNC3 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  { 11,  0, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  { 11,  1, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  { 11,  2, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  { 11,  3, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  { 11,  4, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  { 11,  5, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)},
  { 11,  6, (SCU_CFG_MODE_FUNC2 | SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)}
};

/* SPI Interface: SSP0
 
   PINS: 
   - CS  = PF_1 (GPIO7[16])
   - SCK = PF_0 (SCK0)
   - SDO = PF_2 (MISO0)
   - SDI = PF_3 (MOSI0)
*/

/* SPI Driver */
extern ARM_DRIVER_SPI         Driver_SPI0;
#define ptrSPI              (&Driver_SPI0)

#define SDRAM_BASE_ADDR       0x28000000

/*********************** Hardware specific configuration **********************/

#define SPI_START            (0x70)     /* Start byte for SPI transfer        */
#define SPI_RD               (0x01)     /* WR bit 1 within start              */
#define SPI_WR               (0x00)     /* WR bit 0 within start              */
#define SPI_DATA             (0x02)     /* RS bit 1 within start byte         */
#define SPI_INDEX            (0x00)     /* RS bit 0 within start byte         */

/* SCU pin configuration definitions */
#define LCD_PIN_SET          (SCU_PIN_CFG_INPUT_FILTER_DIS | SCU_PIN_CFG_INPUT_BUFFER_EN | SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN)
#define LCD_NPR_SET          (SCU_PIN_CFG_HIGH_SPEED_SLEW_RATE_EN | SCU_PIN_CFG_PULLUP_DIS)

/* Clock Control Unit register bits */
#define CCU_CLK_CFG_RUN      (1 << 0)
#define CCU_CLK_CFG_AUTO     (1 << 1)
#define CCU_CLK_STAT_RUN     (1 << 0)

/*---------------------------- Global variables ------------------------------*/

static uint16_t   frame_buf[GLCD_WIDTH*GLCD_HEIGHT]__attribute__((at(SDRAM_BASE_ADDR)));
static uint16_t   foreground_color = GLCD_COLOR_BLACK;
static uint16_t   background_color = GLCD_COLOR_WHITE;
static GLCD_FONT *active_font      = NULL;

/************************ Local auxiliary functions ***************************/

/**
  \fn          void delay_ms (int ms)
  \brief       Delay execution for a specified number of milliseconds
  \param[in]   ms     number of while loop iterations
*/
static void delay_ms (int ms) {
  ms *= (SystemCoreClock/10000);
  while (ms--) { __nop(); __nop(); __nop(); __nop(); __nop(); __nop(); }
}

/**
  \fn          void wr_reg (uint8_t reg, uint8_t val)
  \brief       Write  value to LCD register
  \param[in]   reg    register to write
  \param[in]   val    value to write
*/
static void wr_reg (uint8_t reg, uint8_t val) {
  uint8_t wr_data[2];

  wr_data[0] = SPI_START | SPI_WR | SPI_INDEX;    // RS = 0, RW = 0
  wr_data[1] = reg;
  ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
  ptrSPI->Send   (wr_data, 2);
  while (ptrSPI->GetStatus().busy);
  ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);

  wr_data[0] = SPI_START | SPI_WR | SPI_DATA;     // RS = 1, RW = 0
  wr_data[1] = val;
  ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
  ptrSPI->Send   (wr_data, 2);
  while (ptrSPI->GetStatus().busy);
  ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}

/**
  \fn          uint8_t rd_reg (uint8_t reg)
  \brief       Read value from LCD register
  \param[in]   reg    register to read
  \return      Read register value
*/
#if 0
static uint8_t rd_reg (uint8_t reg) {
  uint8_t  wr_data[2];
  uint8_t  rd_data;

  wr_data[0] = SPI_START | SPI_WR | SPI_INDEX;    // RS = 0, RW = 0
  wr_data[1] = reg;
  ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
  ptrSPI->Send   (wr_data, 2);
  while (ptrSPI->GetStatus().busy);
  ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);

  wr_data[0] = SPI_START | SPI_RD | SPI_DATA;     // RS = 1, RW = 1
  ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
  ptrSPI->Send   (wr_data, 1);
  while (ptrSPI->GetStatus().busy);
  ptrSPI->Receive  (&rd_data, 1);
  while (ptrSPI->GetStatus().busy);
  ptrSPI->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
  return (rd_data);
}
#endif


/************************ Module functions ************************************/

/**
  \fn          int32_t GLCD_Initialize (void)
  \brief       Initialize Graphic LCD
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_Initialize (void) {
  uint32_t i;

  SystemCoreClockUpdate();

  /* Initialize and configure SPI */
  ptrSPI->Initialize  (NULL);
  ptrSPI->PowerControl(ARM_POWER_FULL);
  ptrSPI->Control     (ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_SS_MASTER_SW | ARM_SPI_DATA_BITS(8), 10000000);

  LPC_CGU->BASE_LCD_CLK  = (1    << 11) |
                           (0x10 << 24) ; /* IDIVE is clock source            */

  /* Enable LCD clock */
  LPC_CCU1->CLK_M3_LCD_CFG   = CCU_CLK_CFG_AUTO | CCU_CLK_CFG_RUN;
  while (!(LPC_CCU1->CLK_M3_LCD_STAT & CCU_CLK_STAT_RUN));
 
  /* Enable GPIO Clock */
  GPIO_PortClock (1);

  /* Configure pins */
  for (i = 0; i < GLCD_PIN_NUMBER; i++)
    SCU_PinConfigure (GLCD_PIN[i].port,  GLCD_PIN[i].num,  GLCD_PIN[i].config_val);
  /* Configure backlight pin GPIO mode */
  GPIO_SetDir        (GLCD_GPIO[0].port, GLCD_GPIO[0].num, GPIO_DIR_OUTPUT);

  /* LCD with HX8347-D LCD Controller */
  /* Driving ability settings */
  wr_reg(0xEA, 0x00);                   /* Power control internal used (1)    */
  wr_reg(0xEB, 0x20);                   /* Power control internal used (2)    */
  wr_reg(0xEC, 0x0C);                   /* Source control internal used (1)   */
  wr_reg(0xED, 0xC7);                   /* Source control internal used (2)   */
  wr_reg(0xE8, 0x38);                   /* Source output period Normal mode   */
  wr_reg(0xE9, 0x10);                   /* Source output period Idle mode     */
  wr_reg(0xF1, 0x01);                   /* RGB 18-bit interface ;0x0110       */
  wr_reg(0xF2, 0x10);

  /* Adjust Gamma Curve */
  wr_reg(0x40, 0x01);
  wr_reg(0x41, 0x00);
  wr_reg(0x42, 0x00);
  wr_reg(0x43, 0x10);
  wr_reg(0x44, 0x0E);
  wr_reg(0x45, 0x24);
  wr_reg(0x46, 0x04);
  wr_reg(0x47, 0x50);
  wr_reg(0x48, 0x02);
  wr_reg(0x49, 0x13);
  wr_reg(0x4A, 0x19);
  wr_reg(0x4B, 0x19);
  wr_reg(0x4C, 0x16);

  wr_reg(0x50, 0x1B);
  wr_reg(0x51, 0x31);
  wr_reg(0x52, 0x2F);
  wr_reg(0x53, 0x3F);
  wr_reg(0x54, 0x3F);
  wr_reg(0x55, 0x3E);
  wr_reg(0x56, 0x2F);
  wr_reg(0x57, 0x7B);
  wr_reg(0x58, 0x09);
  wr_reg(0x59, 0x06);
  wr_reg(0x5A, 0x06);
  wr_reg(0x5B, 0x0C);
  wr_reg(0x5C, 0x1D);
  wr_reg(0x5D, 0xCC);

  /* Power voltage setting */
  wr_reg(0x1B, 0x1B);
  wr_reg(0x1A, 0x01);
  wr_reg(0x24, 0x2F);
  wr_reg(0x25, 0x57);
  wr_reg(0x23, 0x88);

  /* Power on setting */
  wr_reg(0x18, 0x36);                   /* Internal oscillator frequency adj  */
  wr_reg(0x19, 0x01);                   /* Enable internal oscillator         */
  wr_reg(0x01, 0x00);                   /* Normal mode, no scrool             */
  wr_reg(0x1F, 0x88);                   /* Power control 6 - DDVDH Off        */
  delay_ms (20);
  wr_reg(0x1F, 0x82);                   /* Power control 6 - Step-up: 3 x VCI */
  delay_ms (5);
  wr_reg(0x1F, 0x92);                   /* Power control 6 - Step-up: On      */
  delay_ms (5);
  wr_reg(0x1F, 0xD2);                   /* Power control 6 - VCOML active     */
  delay_ms (5);

  /* Color selection */
  wr_reg(0x17, 0x55);                   /* RGB, System interface: 16 Bit/Pixel*/
  wr_reg(0x00, 0x00);                   /* Scrolling off, no standby          */

  /* Interface config */
  wr_reg(0x2F, 0x11);                   /* LCD Drive: 1-line inversion        */
  wr_reg(0x31, 0x02);                   /* Value for SPI: 0x02, RBG: 0x02     */
  wr_reg(0x32, 0x00);                   /* DPL=0, HSPL=0, VSPL=0, EPL=0       */

  /* Display on setting */
  wr_reg(0x28, 0x38);                   /* PT(0,0) active, VGL/VGL            */
  delay_ms (20);
  wr_reg(0x28, 0x3C);                   /* Display active, VGL/VGL            */

  /* Miror and Swap:     MY           |         MX           |          MV         |    ML    |    BGR       */
  wr_reg (0x16, ((GLCD_MIRROR_Y << 7) | (GLCD_MIRROR_X << 6) | (GLCD_SWAP_XY << 5) | (0 << 4) | (0 << 3)));

  /* Display scrolling settings */
  wr_reg(0x0E, 0x00);                   /* TFA MSB                            */
  wr_reg(0x0F, 0x00);                   /* TFA LSB                            */
  wr_reg(0x10, 320 >> 8);               /* VSA MSB                            */
  wr_reg(0x11, 320 &  0xFF);            /* VSA LSB                            */
  wr_reg(0x12, 0x00);                   /* BFA MSB                            */
  wr_reg(0x13, 0x00);                   /* BFA LSB                            */

  /* Configure LCD controller */
  LPC_RGU->RESET_CTRL0 = (1U << 16);
  while ((LPC_RGU->RESET_ACTIVE_STATUS0 & (1U << 16)) == 0);

  LPC_LCD->CTRL &= ~(1 << 0);           /* Disable LCD                        */
  LPC_LCD->INTMSK = 0;                  /* Disable all interrupts             */

  LPC_LCD->UPBASE = SDRAM_BASE_ADDR;

  LPC_LCD->TIMH  = (7    << 24) |       /* Horizontal back porch              */
                   (3    << 16) |       /* Horizontal front porch             */
                   (3    <<  8) |       /* Horizontal sync pulse width        */
                   (14   <<  2) ;       /* Pixels-per-line                    */
  LPC_LCD->TIMV  = (3    << 24) |       /* Vertical back porch                */
                   (2    << 16) |       /* Vertical front porch               */
                   (3    << 10) |       /* Vertical sync pulse width          */
                   (319  <<  0) ;       /* Lines per panel                    */
  LPC_LCD->POL   = (1    << 26) |       /* Bypass pixel clock divider         */
                   (239  << 16) |       /* Clocks per line: num of LCDCLKs    */
                   (1    << 13) |       /* Invert panel clock                 */
                   (1    << 12) |       /* Invert HSYNC                       */
                   (1    << 11) ;       /* Invert VSYNC                       */
  LPC_LCD->LE    = (1    << 16) |       /* LCDLE Enabled: 1, Disabled: 0      */
                   (9    <<  0) ;       /* Line-end delay: LCDCLK clocks - 1  */
  LPC_LCD->CTRL  = (1    << 11) |       /* LCD Power Enable                   */
                   (1    <<  5) |       /* 0 = STN display, 1: TFT display    */
                   (6    <<  1) ;       /* Bits per pixel: 16bpp (5:6:5)      */

  for (i = 0; i < 256; i++) {
    LPC_LCD->PAL[i] = 0;                /* Clear palette                      */
  }
  LPC_LCD->CTRL |= (1 <<  0);           /* LCD enable                         */

  /* Turn backlight on */
  GPIO_PinWrite (GLCD_GPIO[0].port, GLCD_GPIO[0].num, 1);

  return 0;
}

/**
  \fn          int32_t GLCD_Uninitialize (void)
  \brief       De-initialize Graphic LCD
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_Uninitialize (void) {
  uint32_t i;

  /* Turn backlight off */
  GPIO_PinWrite (GLCD_GPIO[0].port, GLCD_GPIO[0].num, 0);

  /* Unconfigure pins */
  for (i = 0; i < GLCD_PIN_NUMBER; i++)
    SCU_PinConfigure (GLCD_PIN[i].port,  GLCD_PIN[i].num,  0);

  return 0;
}

/**
  \fn          int32_t GLCD_SetForegroundColor (uint32_t color)
  \brief       Set foreground color
  \param[in]   color  Color value
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_SetForegroundColor (uint32_t color) {
  foreground_color = color;
  return 0;
}

/**
  \fn          int32_t GLCD_SetBackgroundColor (uint32_t color)
  \brief       Set background color
  \param[in]   color  Color value
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_SetBackgroundColor (uint32_t color) {
  background_color = color;
  return 0;
}

/**
  \fn          int32_t GLCD_ClearScreen (void)
  \brief       Clear screen (with active background color)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_ClearScreen (void) {
  uint32_t  i;
  uint16_t *ptr_frame_buf;

  ptr_frame_buf = frame_buf;
  for (i = 0; i < (GLCD_WIDTH * GLCD_HEIGHT); i++) {
    *ptr_frame_buf++ = background_color;
  }
  return 0;
}

/**
  \fn          int32_t GLCD_SetFont (GLCD_FONT *font)
  \brief       Set active font
  \param[in]   font   Pointer to font structure
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_SetFont (GLCD_FONT *font) {
  active_font = font;
  return 0;
}

/**
  \fn          int32_t GLCD_DrawPixel (uint32_t x, uint32_t y)
  \brief       Draw pixel (in active foreground color)
  \param[in]   x  x position in pixels (0 = left corner)
  \param[in]   y  y position in pixels (0 = upper corner)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_DrawPixel (uint32_t x, uint32_t y) {
  frame_buf[GLCD_SWAP_XY * (x * GLCD_HEIGHT + y) + (!GLCD_SWAP_XY) * (y * GLCD_WIDTH + x)] = foreground_color;
  return 0;
}

/**
  \fn          int32_t GLCD_DrawHLine (uint32_t x, uint32_t y, uint32_t length)
  \brief       Draw horizontal line (in active foreground color)
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   length Line length
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_DrawHLine (uint32_t x, uint32_t y, uint32_t length) {
  uint32_t dot;

  dot = GLCD_SWAP_XY * (x * GLCD_HEIGHT + y) + (!GLCD_SWAP_XY) * (y * GLCD_WIDTH + x);
  while (length--) { 
    frame_buf[dot] = foreground_color;
    dot += GLCD_SWAP_XY * GLCD_HEIGHT + (!GLCD_SWAP_XY);
  }
  return 0;
}

/**
  \fn          int32_t GLCD_DrawVLine (uint32_t x, uint32_t y, uint32_t length)
  \brief       Draw vertical line (in active foreground color)
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   length Line length in pixels
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_DrawVLine (uint32_t x, uint32_t y, uint32_t length) {
  uint32_t dot;

  dot = GLCD_SWAP_XY * (x * GLCD_HEIGHT + y) + (!GLCD_SWAP_XY) * (y * GLCD_WIDTH + x);
  while (length--) { 
    frame_buf[dot] = foreground_color;
    dot += (!GLCD_SWAP_XY) * GLCD_WIDTH + (GLCD_SWAP_XY);
  }
  return 0;
}

/**
  \fn          int32_t GLCD_DrawRectangle (uint32_t x, uint32_t y, uint32_t width, uint32_t height)
  \brief       Draw rectangle (in active foreground color)
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   width  Rectangle width in pixels
  \param[in]   height Rectangle height in pixels
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_DrawRectangle (uint32_t x, uint32_t y, uint32_t width, uint32_t height) {

  GLCD_DrawHLine (x,         y,          width);
  GLCD_DrawHLine (x,         y + height, width);
  GLCD_DrawVLine (x,         y,          height);
  GLCD_DrawVLine (x + width, y,          height);

  return 0;
}

/**
  \fn          int32_t GLCD_DrawChar (uint32_t x, uint32_t y, int32_t ch)
  \brief       Draw character (in active foreground color)
  \param[in]   x      x position in pixels (0 = left corner)
  \param[in]   y      y position in pixels (0 = upper corner)
  \param[in]   ch     Character
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_DrawChar (uint32_t x, uint32_t y, int32_t ch) {
  uint32_t i, j;
  uint32_t wb, dot;
  uint8_t *ptr_ch_bmp;

  if (active_font == NULL) return -1;

  ch        -= active_font->offset;
  wb         = (active_font->width + 7)/8;
  ptr_ch_bmp = (uint8_t *)active_font->bitmap + (ch * wb * active_font->height);
  dot        = GLCD_SWAP_XY * (x * GLCD_HEIGHT + y) + (!GLCD_SWAP_XY) * (y * GLCD_WIDTH + x);

  for (i = 0; i < active_font->height; i++) {
    for (j = 0; j < active_font->width; j++) {
      frame_buf[dot] = (((*ptr_ch_bmp >> (j & 7)) & 1) ? foreground_color : background_color);
      dot += GLCD_SWAP_XY * GLCD_HEIGHT + (!GLCD_SWAP_XY);
      if (((j & 7) == 7) && (j != (active_font->width - 1))) ptr_ch_bmp++;
    }
    dot = GLCD_SWAP_XY * (x * GLCD_HEIGHT + y + (i + 1)) + (!GLCD_SWAP_XY) * ((y + i + 1) * GLCD_WIDTH + x);
    ptr_ch_bmp++;
  }
  return 0;
}

/**
  \fn          int32_t GLCD_DrawString (uint32_t x, uint32_t y, const char *str)
  \brief       Draw string (in active foreground color)
  \param[in]   x      x position in pixels (0 = left corner)
  \param[in]   y      y position in pixels (0 = upper corner)
  \param[in]   str    Null-terminated String
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_DrawString (uint32_t x, uint32_t y, const char *str) {

  while (*str) { GLCD_DrawChar(x, y, *str++); x += active_font->width; }

  return 0;
}

/**
  \fn          int32_t GLCD_DrawBargraph (uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t val)
  \brief       Draw bargraph (in active foreground color)
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   width  Full bargraph width in pixels
  \param[in]   height Full bargraph height in pixels
  \param[in]   val    Active bargraph value (maximum value is 100)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_DrawBargraph (uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t val) {
  int      i, j;
  uint32_t dot;

  val = (val * width) / 100;            /* Scale value                        */
  dot = GLCD_SWAP_XY * (x * GLCD_HEIGHT + y) + (!GLCD_SWAP_XY) * (y * GLCD_WIDTH + x);

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      frame_buf[dot] = ((j >= val) ? background_color : foreground_color);
      dot += GLCD_SWAP_XY * GLCD_HEIGHT + (!GLCD_SWAP_XY);
    }
    dot = GLCD_SWAP_XY * (x * GLCD_HEIGHT + y + (i + 1)) + (!GLCD_SWAP_XY) * ((y + i + 1) * GLCD_WIDTH + x);
  }
  return 0;
}

/**
  \fn          int32_t GLCD_DrawBitmap (uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t *bitmap)
  \brief       Draw bitmap (bitmap from BMP file without header)
  \param[in]   x      Start x position in pixels (0 = left corner)
  \param[in]   y      Start y position in pixels (0 = upper corner)
  \param[in]   width  Bitmap width in pixels
  \param[in]   height Bitmap height in pixels
  \param[in]   bitmap Bitmap data
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_DrawBitmap (uint32_t x, uint32_t y, uint32_t width, uint32_t height, const uint8_t *bitmap) {
  uint32_t  i, j;
  uint32_t  dot;
  uint16_t *ptr_bmp;

  ptr_bmp = (uint16_t *)bitmap + (width * (height - 1));
  dot     = GLCD_SWAP_XY * (x * GLCD_HEIGHT + y) + (!GLCD_SWAP_XY) * (y * GLCD_WIDTH + x);

  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      frame_buf[dot] = *ptr_bmp++;
      dot += GLCD_SWAP_XY * GLCD_HEIGHT + (!GLCD_SWAP_XY);
    }
    dot = GLCD_SWAP_XY * (x * GLCD_HEIGHT + y + (i + 1)) + (!GLCD_SWAP_XY) * ((y + i + 1) * GLCD_WIDTH + x);
    ptr_bmp -= 2 * width;
  }
  return 0;
}

/**
  \fn          int32_t GLCD_VScroll (uint32_t dy)
  \brief       Scroll picture on display vertically
  \param[in]   dy  Scroll size in pixels
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t GLCD_VScroll (uint32_t dy) {
  uint32_t i, dif, len;

  dif = GLCD_SWAP_XY * dy + (!GLCD_SWAP_XY) * (GLCD_WIDTH * dy);
  len = GLCD_WIDTH * GLCD_HEIGHT - dif;
  for (i = 0; i < len; i++) {
    frame_buf[i] = frame_buf[i+dif];
  }
  for (; i < GLCD_WIDTH * GLCD_HEIGHT; i++) {
    frame_buf[i] = background_color;
  }
  return 0;
}

/**
  \fn          int32_t GLCD_FrameBufferAccess (bool enable)
  \brief       Enable or disable direct access to FrameBuffer
  \param[in]   enable Enable or disable request
                - true (1): enable
                - false(0): disable
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t  GLCD_FrameBufferAccess   (bool enable) {
  /* Direct data access is used with direct access to frame buffer            */
  return 0;
}

/**
  \fn          uint32_t GLCD_FrameBufferAddress (void)
  \brief       Retrieve FrameBuffer address
  \returns     FrameBuffer address
*/
uint32_t GLCD_FrameBufferAddress (void) {
  return ((uint32_t)frame_buf);
}
