#include "lcd.h"
#include "esp_camera.h"
#include <string.h>
static const char *TAG = "lcd";

static scr_driver_t g_lcd;
static scr_info_t g_lcd_info;

void screen_display(int x1, int y1, int x2, int y2, uint8_t *rgb565_data)
{
     g_lcd.draw_bitmap(x1, y1, x2, y2, (uint16_t *)rgb565_data);
}

void screen_display_row(int x1, int y1, int x2, int y2, uint8_t *rgb565_data)
{
     g_lcd.draw_row_bitmap(x1, y1, x2, y2, (uint16_t *)rgb565_data);
}

esp_err_t init_lcd(const QueueHandle_t frame_i, const QueueHandle_t frame_o, const bool return_fb)
{
    spi_config_t bus_conf = {
        .miso_io_num = BOARD_LCD_MISO,
        .mosi_io_num = BOARD_LCD_MOSI,
        .sclk_io_num = BOARD_LCD_SCK,
        .max_transfer_sz = 2 * 320 * 240 + 10,
    };
    spi_bus_handle_t spi_bus = spi_bus_create(SPI2_HOST, &bus_conf);

    scr_interface_spi_config_t spi_lcd_cfg = {
        .spi_bus = spi_bus,
        .pin_num_cs = BOARD_LCD_CS,
        .pin_num_dc = BOARD_LCD_DC,
        .clk_freq = 60 * 1000000,
        .swap_data = 0,
    };

    scr_interface_driver_t *iface_drv;
    scr_interface_create(SCREEN_IFACE_SPI, &spi_lcd_cfg, &iface_drv);
    esp_err_t ret = scr_find_driver(SCREEN_CONTROLLER_ILI9341, &g_lcd);
    if (ESP_OK != ret) {
        return ret;
        ESP_LOGE(TAG, "screen find failed");
    }

    scr_controller_config_t lcd_cfg = {
        .interface_drv = iface_drv,
        .pin_num_rst = BOARD_LCD_RST,
        .pin_num_bckl = BOARD_LCD_BL,
        .rst_active_level = 0,
        .bckl_active_level = 0,
        .offset_hor = 0,
        .offset_ver = 0,
        .width = 240,
        .height = 320,
        .rotate = 4,
    };
    ret = g_lcd.init(&lcd_cfg);
    if (ESP_OK != ret) {
        return ESP_FAIL;
        ESP_LOGE(TAG, "screen initialize failed");
    }

    g_lcd.get_info(&g_lcd_info);
    ESP_LOGI(TAG, "Screen name:%s | width:%d | height:%d", g_lcd_info.name, g_lcd_info.width, g_lcd_info.height);

    return ESP_OK;
}
