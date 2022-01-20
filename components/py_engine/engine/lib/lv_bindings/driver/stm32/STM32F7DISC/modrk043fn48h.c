#include "py/runtime.h"
#include "py/mphal.h"
#include "softtimer.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../../lvgl/lvgl.h"
#include "../../../lv_conf.h"
#include "../../include/common.h"
#include "stm32746g_discovery_ts.h"
#include "rk043fn48h.h"
#include "ports/stm32/i2c.h"


#define LCD_DISP_PIN                    GPIO_PIN_12
#define LCD_DISP_GPIO_PORT              GPIOI
#define LCD_DISP_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOI_CLK_ENABLE()
#define LCD_DISP_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOI_CLK_DISABLE()
#define LCD_BL_CTRL_PIN                 GPIO_PIN_3
#define LCD_BL_CTRL_GPIO_PORT           GPIOK
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOK_CLK_ENABLE()
#define LCD_BL_CTRL_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOK_CLK_DISABLE()


LTDC_HandleTypeDef *hltdc = NULL;           // handle to LTDC, referenced in stm32_it.c
DMA2D_HandleTypeDef *hdma2d = NULL;         // handle to DMA2D, referenced in stm32_it.c
i2c_t *i2c_ts = NULL;                       // I2C handle for touchscreen
lv_disp_drv_t *dma2d_disp_drv = NULL;  // handle to display driver
lv_color_t *fb[2] = {NULL, NULL};           // framebuffer pointers
uint32_t w = 0;                             // display width
uint32_t h = 0;                             // display height
volatile bool dma2d_pend = false;           // flag of DMA2D pending operation

static bool config_ltdc(void);
static bool config_dma2d(void);

STATIC mp_obj_t mp_rk043fn48h_framebuffer(mp_obj_t n_obj) {
	int n = mp_obj_get_int(n_obj) -1;

	if (n<0 || n>1){
		return mp_const_none;
	}

	if(fb[n]==NULL){
		// allocation on extRAM with 1KB alignment to speed up LTDC burst access on AHB
		fb[n] = MP_STATE_PORT(rk043fn48h_fb[n]) = m_malloc(sizeof(lv_color_t) * w * h  + 1024);
		fb[n] = (lv_color_t*)((uint32_t)fb[n] + 1024 - (uint32_t)fb[n] % 1024);
	}
	return mp_obj_new_bytearray_by_ref(sizeof(lv_color_t) * w * h , (void *)fb[n]);
}

STATIC mp_obj_t mp_rk043fn48h_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_w, ARG_h };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_w, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 480} },
        { MP_QSTR_h, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 272} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    w = args[ARG_w].u_int;
    h = args[ARG_h].u_int;

    mp_rk043fn48h_framebuffer(mp_obj_new_int(1));

    if (fb[0] == NULL) {
        mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Failed allocating frame buffer"));
    }

    if (!config_ltdc()) {
        mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("LTDC init error"));
    }

    if (!config_dma2d()) {
         mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("DMA2D init error"));
    }

    i2c_ts = I2C3;
    i2c_init(i2c_ts, MICROPY_HW_I2C3_SCL, MICROPY_HW_I2C3_SDA, 400000, 100);

    if (BSP_TS_Init(w, h) != TS_OK) {
        mp_obj_new_exception_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Touchscreen init error"));
    }

    return mp_const_none;
}

STATIC mp_obj_t mp_rk043fn48h_deinit() {
    if (hdma2d) {
        HAL_DMA2D_DeInit(hdma2d);
        hdma2d = NULL;
    }

    if (hltdc) {
        HAL_LTDC_DeInit(hltdc);
        HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_RESET);
        hltdc = NULL;
    }

    if(fb[0]!=NULL){
    	m_free(MP_STATE_PORT(rk043fn48h_fb[0]));
    	fb[0]=NULL;
    }

    if(fb[1]!=NULL){
    	m_free(MP_STATE_PORT(rk043fn48h_fb[1]));
    	fb[1]=NULL;
    }

    BSP_TS_DeInit();

    return mp_const_none;
}

STATIC void mp_rk043fn48h_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    if ((lv_area_get_width(area) == w) && (lv_area_get_height(area) == h)) {
        dma2d_disp_drv = disp_drv;
        SCB_CleanInvalidateDCache();
        HAL_LTDC_SetAddress_NoReload(hltdc, (uint32_t)color_p, 1);
        HAL_LTDC_Reload(hltdc, LTDC_RELOAD_VERTICAL_BLANKING);
    } else {
        hdma2d->Init.Mode = DMA2D_M2M;
        hdma2d->Init.OutputOffset = w - lv_area_get_width(area);
        dma2d_disp_drv = disp_drv;
        dma2d_pend = true;
        SCB_CleanInvalidateDCache();
        HAL_DMA2D_Init(hdma2d);
        HAL_DMA2D_Start_IT(hdma2d,
            (uint32_t)color_p,
            (uint32_t)(fb[0] + area->x1 + area->y1 * w),
            lv_area_get_width(area),
            lv_area_get_height(area));
    }
}

void DMA2D_TransferComplete(DMA2D_HandleTypeDef *hdma2d) {
    lv_disp_flush_ready(dma2d_disp_drv);
    dma2d_pend = false;
}

void HAL_LTDC_ReloadEventCallback(LTDC_HandleTypeDef *hltdc) {
    lv_disp_flush_ready(dma2d_disp_drv);
}

STATIC bool mp_rk043fn48h_ts_read(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    static TS_StateTypeDef ts_state = {0};
    static lv_coord_t lastX = 0;
    static lv_coord_t lastY = 0;

    BSP_TS_GetState(&ts_state);
    if (ts_state.touchDetected) {
        data->point.x = lastX = ts_state.touchX[0];
        data->point.y = lastY = ts_state.touchY[0];
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->point.x = lastX;
        data->point.y = lastY;
        data->state = LV_INDEV_STATE_REL;
    }

    return false;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(mp_rk043fn48h_init_obj, 0, mp_rk043fn48h_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_rk043fn48h_deinit_obj, mp_rk043fn48h_deinit);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_rk043fn48h_framebuffer_obj, mp_rk043fn48h_framebuffer);
DEFINE_PTR_OBJ(mp_rk043fn48h_flush);
DEFINE_PTR_OBJ(mp_rk043fn48h_ts_read);

STATIC const mp_rom_map_elem_t rk043fn48h_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rk043fn48h) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_rk043fn48h_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mp_rk043fn48h_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&PTR_OBJ(mp_rk043fn48h_flush))},
    { MP_ROM_QSTR(MP_QSTR_ts_read), MP_ROM_PTR(&PTR_OBJ(mp_rk043fn48h_ts_read))},
    { MP_ROM_QSTR(MP_QSTR_framebuffer), MP_ROM_PTR(&PTR_OBJ(mp_rk043fn48h_framebuffer))},
};

STATIC MP_DEFINE_CONST_DICT(
    mp_module_rk043fn48h_globals,
    rk043fn48h_globals_table
    );

const mp_obj_module_t mp_module_rk043fn48h = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_rk043fn48h_globals
};

MP_REGISTER_MODULE(MP_QSTR_rk043fn48h, mp_module_rk043fn48h, 1);

static bool config_ltdc(void) {
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    static LTDC_HandleTypeDef hltdc_F = {0};
    LTDC_LayerCfgTypeDef pLayerCfg = {0};

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = RK043FN48H_FREQUENCY_DIVIDER;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        return false;
    }

    hltdc_F.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hltdc_F.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hltdc_F.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hltdc_F.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hltdc_F.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
    hltdc_F.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
    hltdc_F.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hltdc_F.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hltdc_F.Init.AccumulatedActiveH = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hltdc_F.Init.AccumulatedActiveW = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hltdc_F.Init.TotalHeigh = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
    hltdc_F.Init.TotalWidth = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);
    hltdc_F.Init.Backcolor.Blue = 0;
    hltdc_F.Init.Backcolor.Green = 0;
    hltdc_F.Init.Backcolor.Red = 0;
    hltdc_F.Instance = LTDC;

    pLayerCfg.WindowX0 = 0;
    pLayerCfg.WindowX1 = w;
    pLayerCfg.WindowY0 = 0;
    pLayerCfg.WindowY1 = h;

    #if (LV_COLOR_DEPTH == 32)
    pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
    #elif (LV_COLOR_DEPTH == 16)
    pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    #else
    #error "modrk043fn48h: LV_COLOR_DEPTH not supported"
    #endif

    pLayerCfg.FBStartAdress = (uint32_t)fb[0];
    pLayerCfg.Alpha = 255;
    pLayerCfg.Alpha0 = 0;
    pLayerCfg.Backcolor.Blue = 0;
    pLayerCfg.Backcolor.Green = 0;
    pLayerCfg.Backcolor.Red = 0;
    pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
    pLayerCfg.ImageWidth = w;
    pLayerCfg.ImageHeight = h;

    hltdc = &hltdc_F;

    if (HAL_LTDC_Init(hltdc) != HAL_OK) {
        return false;
    }

    if (HAL_LTDC_ConfigLayer(hltdc, &pLayerCfg, 1) != HAL_OK) {
        return false;
    }

    return true;
}

static bool config_dma2d(void) {
    static DMA2D_HandleTypeDef Dma2dHandle = {0};

    #if (LV_COLOR_DEPTH == 32)
    uint32_t OutColorMode = DMA2D_OUTPUT_ARGB8888;
    uint32_t InColorMode = DMA2D_INPUT_ARGB8888;
    #elif (LV_COLOR_DEPTH == 16)
    uint32_t OutColorMode = DMA2D_OUTPUT_RGB565;
    uint32_t InColorMode = DMA2D_INPUT_RGB565;
    #else
    #error "modrk043fn48h: LV_COLOR_DEPTH not supported"
    #endif

    Dma2dHandle.Init.Mode = DMA2D_M2M_BLEND;
    Dma2dHandle.Init.ColorMode = OutColorMode;
    Dma2dHandle.Init.OutputOffset = 0x0;
    Dma2dHandle.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
    Dma2dHandle.LayerCfg[1].InputAlpha = 0xFF;
    Dma2dHandle.LayerCfg[1].InputColorMode = InColorMode;
    Dma2dHandle.LayerCfg[1].InputOffset = 0x0;
    Dma2dHandle.LayerCfg[0].AlphaMode = DMA2D_REPLACE_ALPHA;
    Dma2dHandle.LayerCfg[0].InputAlpha = 0xFF;
    Dma2dHandle.LayerCfg[0].InputColorMode = InColorMode;
    Dma2dHandle.LayerCfg[0].InputOffset = 0x0;
    Dma2dHandle.Instance = DMA2D;
    Dma2dHandle.XferCpltCallback = DMA2D_TransferComplete;

    if (HAL_DMA2D_Init(&Dma2dHandle) != HAL_OK) {
        return false;
    }

    hdma2d = &Dma2dHandle;

    HAL_DMA2D_ConfigLayer(&Dma2dHandle, 0);
    HAL_DMA2D_ConfigLayer(&Dma2dHandle, 1);

    return true;
}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc) {
    GPIO_InitTypeDef GPIO_Init_Structure;

    __HAL_RCC_LTDC_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    __HAL_RCC_GPIOK_CLK_ENABLE();

    GPIO_Init_Structure.Pin = GPIO_PIN_4;
    GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull = GPIO_NOPULL;
    GPIO_Init_Structure.Speed = GPIO_SPEED_FAST;
    GPIO_Init_Structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);

    GPIO_Init_Structure.Pin = GPIO_PIN_12;
    GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOG, &GPIO_Init_Structure);

    GPIO_Init_Structure.Pin = GPIO_PIN_9 | GPIO_PIN_10 | \
        GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);

    GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | \
        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | \
        GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | \
        GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOJ, &GPIO_Init_Structure);

    GPIO_Init_Structure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
        GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOK, &GPIO_Init_Structure);

    GPIO_Init_Structure.Pin = GPIO_PIN_12;
    GPIO_Init_Structure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOI, &GPIO_Init_Structure);

    GPIO_Init_Structure.Pin = GPIO_PIN_3;
    GPIO_Init_Structure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOK, &GPIO_Init_Structure);

    HAL_GPIO_WritePin(LCD_DISP_GPIO_PORT, LCD_DISP_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

    HAL_NVIC_SetPriority(LTDC_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
}


void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef *hltdc) {
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();
}


void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef *hdma2d) {
    __HAL_RCC_DMA2D_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA2D_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);
}

void TS_IO_Init(void) {
}

void TS_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value) {
    static uint8_t buff[2];
    buff[0] = Reg;
    buff[1] = Value;
    uint16_t addr = Addr >> 1;
    i2c_writeto(i2c_ts, addr, buff, 2, true);
}

uint8_t TS_IO_Read(uint8_t Addr, uint8_t Reg) {
    uint8_t Value = 0xff;
    uint16_t addr = Addr >> 1;
    i2c_writeto(i2c_ts, addr, &Reg, 1, true);
    i2c_readfrom(i2c_ts, addr, &Value, 1, true);
    return Value;
}

void TS_IO_Delay(uint32_t Delay) {
    HAL_Delay(Delay);
}

