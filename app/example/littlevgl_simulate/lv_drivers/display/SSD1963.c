/**
 * @file SSD1963.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "SSD1963.h"
#if USE_SSD1963

#include <stdbool.h>
#include "lvgl/lv_core/lv_vdb.h"
#include LV_DRV_DISP_INCLUDE
#include LV_DRV_DELAY_INCLUDE

/*********************
 *      DEFINES
 *********************/
#define SSD1963_CMD_MODE     0
#define SSD1963_DATA_MODE    1

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static inline void ssd1963_cmd_mode(void);
static inline void ssd1963_data_mode(void);
static inline void ssd1963_cmd(uint8_t cmd);
static inline void ssd1963_data(uint8_t data);
static void ssd1963_io_init(void);
static void ssd1963_reset(void);
static void ssd1963_set_clk(void);
static void ssd1963_set_tft_spec(void);
static void ssd1963_init_bl(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool cmd_mode = true;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ssd1963_init(void)
{
    ssd1963_io_init();
    ssd1963_reset();
    ssd1963_set_clk();
    ssd1963_set_tft_spec();
    ssd1963_init_bl();
    
    ssd1963_cmd(0x13);		//SET display on

    ssd1963_cmd(0x29);		//SET display on
    LV_DRV_DELAY_MS(30);
    
}


void ssd1963_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{

    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > SSD1963_HOR_RES - 1) return;
    if(y1 > SSD1963_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > SSD1963_HOR_RES - 1 ? SSD1963_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > SSD1963_VER_RES - 1 ? SSD1963_VER_RES - 1 : y2;

    //Set the rectangular area
    ssd1963_cmd(0x002A);
    ssd1963_data(act_x1 >> 8);
    ssd1963_data(0x00FF & act_x1);
    ssd1963_data(act_x2 >> 8);
    ssd1963_data(0x00FF & act_x2);

    ssd1963_cmd(0x002B);
    ssd1963_data(act_y1 >> 8);
    ssd1963_data(0x00FF & act_y1);
    ssd1963_data(act_y2 >> 8);
    ssd1963_data(0x00FF & act_y2);

    ssd1963_cmd(0x2c);
     int16_t i;
    uint16_t full_w = x2 - x1 + 1;

    ssd1963_data_mode();

#if LV_COLOR_DEPTH == 16
    uint16_t act_w = act_x2 - act_x1 + 1;
    for(i = act_y1; i <= act_y2; i++) {
        LV_DRV_DISP_PAR_WR_ARRAY((uint16_t*)color_p, act_w);
        color_p += full_w;
    }
#else
    int16_t j;
    for(i = act_y1; i <= act_y2; i++) {
        for(j = 0; j <= act_x2 - act_x1 + 1; j++) {
            LV_DRV_DISP_PAR_WR_WORD(color_p[j]);
            color_p += full_w;
        }
    }
#endif

    lv_flush_ready();
}

void ssd1963_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > SSD1963_HOR_RES - 1) return;
    if(y1 > SSD1963_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > SSD1963_HOR_RES - 1 ? SSD1963_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > SSD1963_VER_RES - 1 ? SSD1963_VER_RES - 1 : y2;
   
    //Set the rectangular area
    ssd1963_cmd(0x002A);
    ssd1963_data(act_x1 >> 8);
    ssd1963_data(0x00FF & act_x1);
    ssd1963_data(act_x2 >> 8);
    ssd1963_data(0x00FF & act_x2);

    ssd1963_cmd(0x002B);
    ssd1963_data(act_y1 >> 8);
    ssd1963_data(0x00FF & act_y1);
    ssd1963_data(act_y2 >> 8);
    ssd1963_data(0x00FF & act_y2);

    ssd1963_cmd(0x2c);
    
    ssd1963_data_mode();

    uint16_t color16 = lv_color_to16(color);
    uint32_t size = (act_x2 - act_x1 + 1) * (act_y2 - act_y1 + 1);
    uint32_t i;
    for(i = 0; i < size; i++) {
        LV_DRV_DISP_PAR_WR_WORD(color16);
    }
}

void ssd1963_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    
    /*Return if the area is out the screen*/
    if(x2 < 0) return;
    if(y2 < 0) return;
    if(x1 > SSD1963_HOR_RES - 1) return;
    if(y1 > SSD1963_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > SSD1963_HOR_RES - 1 ? SSD1963_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > SSD1963_VER_RES - 1 ? SSD1963_VER_RES - 1 : y2;
   
    //Set the rectangular area
    ssd1963_cmd(0x002A);
    ssd1963_data(act_x1 >> 8);
    ssd1963_data(0x00FF & act_x1);
    ssd1963_data(act_x2 >> 8);
    ssd1963_data(0x00FF & act_x2);

    ssd1963_cmd(0x002B);
    ssd1963_data(act_y1 >> 8);
    ssd1963_data(0x00FF & act_y1);
    ssd1963_data(act_y2 >> 8);
    ssd1963_data(0x00FF & act_y2);

    ssd1963_cmd(0x2c);
     int16_t i;
    uint16_t full_w = x2 - x1 + 1;
    
    ssd1963_data_mode();
    
#if LV_COLOR_DEPTH == 16
    uint16_t act_w = act_x2 - act_x1 + 1;
    for(i = act_y1; i <= act_y2; i++) {
        LV_DRV_DISP_PAR_WR_ARRAY((uint16_t*)color_p, act_w);
        color_p += full_w;
    }
#else
    int16_t j;
    for(i = act_y1; i <= act_y2; i++) {
        for(j = 0; j <= act_x2 - act_x1 + 1; j++) {
            LV_DRV_DISP_PAR_WR_WORD(color_p[j]);
            color_p += full_w;
        }
    }
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ssd1963_io_init(void)
{
    LV_DRV_DISP_CMD_DATA(SSD1963_CMD_MODE);
    cmd_mode = true;
}

static void ssd1963_reset(void)
{
    /*Hardware reset*/
    LV_DRV_DISP_RST(1);
    LV_DRV_DELAY_MS(50);
    LV_DRV_DISP_RST(0);
    LV_DRV_DELAY_MS(50);
    LV_DRV_DISP_RST(1);
    LV_DRV_DELAY_MS(50);

    /*Chip enable*/
    LV_DRV_DISP_PAR_CS(0);
    LV_DRV_DELAY_MS(10);
    LV_DRV_DISP_PAR_CS(1);
    LV_DRV_DELAY_MS(5);
    
    /*Software reset*/
    ssd1963_cmd(0x01);
    LV_DRV_DELAY_MS(20);

    ssd1963_cmd(0x01);
    LV_DRV_DELAY_MS(20);

    ssd1963_cmd(0x01);
    LV_DRV_DELAY_MS(20);
    
}

static void ssd1963_set_clk(void)
{
    /* Set PLL*/    
    ssd1963_cmd(0xe2);          
    ssd1963_data(0x23);
    ssd1963_data(0x05);
    ssd1963_data(0x54);

    /*Enable PLL*/
    ssd1963_cmd(0xe0);     
    ssd1963_data(0x01);
    LV_DRV_DELAY_MS(20);

    /*Lock PLL*/
    ssd1963_cmd(0xe0);        
    ssd1963_data(0x03);

    /*Software reset*/
    ssd1963_cmd(0x01);
    LV_DRV_DELAY_MS(20);

    /*Set PCLK freq*/
    ssd1963_cmd(0xe6); 
    ssd1963_data(0x04);
    ssd1963_data(0x93);
    ssd1963_data(0xe0);
}

static void ssd1963_set_tft_spec(void)
{
    ssd1963_cmd(0xB0);	//LCD SPECIFICATION
    ssd1963_data(0x20);

    ssd1963_data(0x00);
    ssd1963_data(((SSD1963_HOR_RES - 1) >> 8) & 0XFF);  //Set HDP
    ssd1963_data((SSD1963_HOR_RES - 1) & 0XFF);
    ssd1963_data(((SSD1963_VER_RES - 1) >> 8) & 0XFF);  //Set VDP
    ssd1963_data((SSD1963_VER_RES - 1) & 0XFF);
    ssd1963_data(0x00);

    ssd1963_cmd(0xB4);	//HSYNC
    ssd1963_data((SSD1963_HT >> 8) & 0XFF);  //Set HT
    ssd1963_data(SSD1963_HT & 0XFF);
    ssd1963_data((SSD1963_HPS >> 8) & 0XFF);  //Set HPS
    ssd1963_data(SSD1963_HPS & 0XFF);
    ssd1963_data(SSD1963_HPW);			   //Set HPW
    ssd1963_data((SSD1963_LPS >> 8) & 0XFF);  //Set HPS
    ssd1963_data(SSD1963_LPS & 0XFF);
    ssd1963_data(0x00);

    ssd1963_cmd(0xB6);	//VSYNC
    ssd1963_data((SSD1963_VT >> 8) & 0XFF);   //Set VT
    ssd1963_data(SSD1963_VT & 0XFF);
    ssd1963_data((SSD1963_VPS >> 8) & 0XFF);  //Set VPS
    ssd1963_data(SSD1963_VPS & 0XFF);
    ssd1963_data(SSD1963_VPW);			   //Set VPW
    ssd1963_data((SSD1963_FPS >> 8) & 0XFF);  //Set FPS
    ssd1963_data(SSD1963_FPS & 0XFF);
    
    ssd1963_cmd(0xf0);            //SET pixel data I/F format=16bit
    ssd1963_data(0x03);
//    ssd1963_cmd(0x3a);      //RGB pixel format
//    ssd1963_data(0x66);      //24bpp (0x60: 18bpp)

    ssd1963_cmd(0x36);     	 //SET address mode=flip vertical
#if SSD1963_ORI == 0
    ssd1963_data(0x03);
#else
    ssd1963_data(0x00);
#endif


//     ssd1963_cmd(0x26);      //gamma curve
//    ssd1963_data(0x08);

}

static void ssd1963_init_bl(void)
{

    ssd1963_cmd(0xBE);      /*Set PWM configuration for back light control*/

    ssd1963_data(0x02);		/*PWMF[7:0] = 2, PWM base freq = PLL/(256*(1+5))/256 =
                                                             300Hz for a PLL freq = 120MHz */
    ssd1963_data(0x20);		/*Set duty cycle, from 0x00 (total pull-down) to 0xFF
                                                              (99% pull-up , 255/256) */
    ssd1963_data(0x01);		/*PWM enabled and controlled by host (mcu) */
    ssd1963_data(0x00);
    ssd1963_data(0x00);
    ssd1963_data(0x00);
}


/**
 * Command mode
 */
static inline void ssd1963_cmd_mode(void)
{
    if(cmd_mode == false) {
        LV_DRV_DISP_CMD_DATA(SSD1963_CMD_MODE);
        cmd_mode = true;
    }
}

/**
 * Data mode
 */
static inline void ssd1963_data_mode(void)
{
    if(cmd_mode != false) {
        LV_DRV_DISP_CMD_DATA(SSD1963_DATA_MODE);
        cmd_mode = false;
    }
}

/**
 * Write command
 * @param cmd the command
 */
static inline void ssd1963_cmd(uint8_t cmd)
{    
    ssd1963_cmd_mode();
    LV_DRV_DISP_PAR_WR_WORD(cmd);
}

/**
 * Write data
 * @param data the data
 */
static inline void ssd1963_data(uint8_t data)
{    
    ssd1963_data_mode();
    LV_DRV_DISP_PAR_WR_WORD(data);
}

#endif
