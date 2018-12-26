/*
 * Copyright (C) 2017 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _OLED_H_
#define _OLED_H_

int32_t oled_draw_char_1608(uint8_t x, uint8_t y, uint8_t c);
int32_t oled_draw_string_1608(uint8_t column, uint8_t page, const char *str);
int32_t oled_draw_p8xnstr(uint8_t column, uint8_t page, const uint8_t* str, uint8_t len);
void oled_clear_screen(void);

#endif

