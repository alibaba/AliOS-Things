#ifndef __FONT__H__
#define __FONT__H__

// To incloud uint*_t head file
#include "esp_system.h"


typedef struct { // Data stored PER GLYPH
	uint32_t bitmapOffset;     // Pointer into GFXfont->bitmap
	uint8_t  width, height;    // Bitmap dimensions in pixels
	uint8_t  xAdvance;         // Distance to advance cursor (x axis)
	int8_t   xOffset, yOffset; // Dist from cursor pos to UL corner
} GFXglyph;

// 0x7F, 0xB6, 0xD2, 0x49, 0x20, 0x1B, /* 0x0021 */
typedef struct {
	uint16_t start;
	uint16_t end;
	uint16_t base;
} EncodeRange;

typedef struct { // Data stored for FONT AS A WHOLE:
	uint8_t  *bitmap;      // Glyph bitmaps, concatenated
	GFXglyph *glyph;       // Glyph array
	uint16_t  first, last; // ASCII extents
	uint8_t   yAdvance;    // Newline distance (y axis)
	uint16_t range_num;
	EncodeRange *range;
} GFXfont;

#endif