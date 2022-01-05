#ifndef _GFX_H__
#define _GFX_H__

#include "string.h"
#include "py/obj.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "../tft/tftspi.h"
#include "font/font.h"

extern int unicode_printProportionalChar(uint32_t code, int x, int y);
extern const GFXfont *gfx_font;
extern int16_t GetUnicodeWidth(uint32_t code);
extern int unicode_rotateProChar(uint32_t code, int x, int y, uint16_t offset);

#endif