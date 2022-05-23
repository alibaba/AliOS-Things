/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "display_service.h"
#include "periph_ws2812.h"
#include "board_def.h"
#include "string.h"
#include "esp_log.h"

const struct periph_ws2812_ctrl_cfg ws2812_display_pattern[DISPLAY_PATTERN_MAX][WS2812_LED_BAR_NUMBERS] = {
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_RED,
            .loop = 1000,
            .time_off_ms = 200,
            .time_on_ms = 400,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_RED,
            .loop = 1000,
            .time_off_ms = 200,
            .time_on_ms = 400,
        } // 0
    },
    {
        {
            .mode = PERIPH_WS2812_FADE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 0xFFFFFFFF,
            .time_off_ms = 1500,
            .time_on_ms = 1500,
        },
        {
            .mode = PERIPH_WS2812_FADE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 0xFFFFFFFF,
            .time_off_ms = 1500,
            .time_on_ms = 1500,
        } // 1
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_WHITE,
            .loop = 0xFFFFFFFF,
            .time_off_ms = 200,
            .time_on_ms = 800,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_WHITE,
            .loop = 0xFFFFFFFF,
            .time_off_ms = 200,
            .time_on_ms = 800,
        } // 2
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK ,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 3
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_RED,
            .loop = 1,
            .time_off_ms = 1000,
            .time_on_ms = 4000,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_RED,
            .loop = 1,
            .time_off_ms = 1000,
            .time_on_ms = 4000,
        } // 4
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_WHITE,
            .loop = 5,
            .time_off_ms = 200,
            .time_on_ms = 1000,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_WHITE,
            .loop = 5,
            .time_off_ms = 200,
            .time_on_ms = 1000,
        } // 5
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_BLUE,
            .loop = 100,
            .time_off_ms = 200,
            .time_on_ms = 800,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_BLUE,
            .loop = 100,
            .time_off_ms = 200,
            .time_on_ms = 800,
        } // 6
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 4,
            .time_off_ms = 100,
            .time_on_ms = 100,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 4,
            .time_off_ms = 100,
            .time_on_ms = 100,
        } // 7
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 4,
            .time_off_ms = 100,
            .time_on_ms = 100,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 4,
            .time_off_ms = 100,
            .time_on_ms = 100,
        } // 8
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 9
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 2,
            .time_off_ms = 200,
            .time_on_ms = 400,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 2,
            .time_off_ms = 200,
            .time_on_ms = 400,
        } // 10
    },
    {
        {
            .mode = PERIPH_WS2812_FADE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 100,
            .time_off_ms = 2000,
            .time_on_ms = 2000,
        },
        {
            .mode = PERIPH_WS2812_FADE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 100,
            .time_off_ms = 2000,
            .time_on_ms = 2000,
        } // 11
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 2,
            .time_off_ms = 200,
            .time_on_ms = 400,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 2,
            .time_off_ms = 200,
            .time_on_ms = 400,
        } // 12
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLUE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLUE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 13
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        }  // 14
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_YELLOW,
            .loop = 2000,
            .time_off_ms = 200,
            .time_on_ms = 600,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_YELLOW,
            .loop = 2000,
            .time_off_ms = 200,
            .time_on_ms = 600,
        }  // 15
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 16
    },
    {
        {
            .mode = PERIPH_WS2812_FADE,
            .color = LED2812_COLOR_GREEN,
            .loop = 1,
            .time_off_ms = 0,
            .time_on_ms = 1000,
        },
        {
            .mode = PERIPH_WS2812_FADE,
            .color = LED2812_COLOR_GREEN,
            .loop = 1,
            .time_off_ms = 0,
            .time_on_ms = 1000,
        } // 17
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_ORANGE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_ORANGE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 18
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 19
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_WHITE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_WHITE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 20
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 21
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_RED,
            .loop = 1000,
            .time_off_ms = 200,
            .time_on_ms = 800,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_RED,
            .loop = 1000,
            .time_off_ms = 200,
            .time_on_ms = 800,
        } // 22
    },
    {
        {
            .mode = PERIPH_WS2812_FADE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 1000,
            .time_off_ms = 1500,
            .time_on_ms = 1500,
        },
        {
            .mode = PERIPH_WS2812_FADE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 1000,
            .time_off_ms = 1500,
            .time_on_ms = 1500,
        } // 23
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 1000,
            .time_off_ms = 1000,
            .time_on_ms = 2000,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_PURPLE,
            .loop = 1000,
            .time_off_ms = 1000,
            .time_on_ms = 2000,
        } // 24
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_PURPLE,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 25
    }
    ,
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_RED,
            .loop = 0xFFFFFFFF,
            .time_off_ms = 2000,
            .time_on_ms = 2000,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_RED,
            .loop = 0xFFFFFFFF,
            .time_off_ms = 2000,
            .time_on_ms = 2000,
        } // 26
    },
    {
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_BLUE,
            .loop = 0xFFFFFFFF,
            .time_off_ms = 500,
            .time_on_ms = 500,
        },
        {
            .mode = PERIPH_WS2812_BLINK,
            .color = LED2812_COLOR_BLUE,
            .loop = 0xFFFFFFFF,
            .time_off_ms = 500,
            .time_on_ms = 500,
        } // 27,
    },
    {
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        },
        {
            .mode = PERIPH_WS2812_ONE,
            .color = LED2812_COLOR_BLACK,
            .loop = 0,
            .time_off_ms = 0,
            .time_on_ms = 0,
        } // 28,
    }
};

int8_t get_ws2812_gpio_pin(void)
{
    return WS2812_LED_GPIO_PIN;
}

int get_ws2812_num(void)
{
    return WS2812_LED_BAR_NUMBERS;
}

void ws2812_pattern_copy(struct periph_ws2812_ctrl_cfg *p)
{
    ESP_LOGD("ws2812_pattern_copy", "has been called, %s %d", __FILE__, __LINE__);
    memcpy(p, ws2812_display_pattern, sizeof(ws2812_display_pattern));
}
