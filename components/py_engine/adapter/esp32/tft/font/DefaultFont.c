// Default font

// ========================================================================
// This comes with no warranty, implied or otherwise

// This data structure was designed to support Proportional fonts
// fonts. Individual characters do not have to be multiples of 8 bits wide.
// Any width is fine and does not need to be fixed.

// The data bits are packed to minimize data requirements, but the tradeoff
// is that a header is required per character.

// Header Format:
// ------------------------------------------------
// Character Width (Used as a marker to indicate use this format. i.e.: = 0x00)
// Character Height
// First Character (Reserved. 0x00)
// Number Of Characters (Reserved. 0x00)

// Individual Character Format:
// ----------------------------
// Character Code
// Adjusted Y Offset	(start Y of visible pixels)
// Width				(width of the visible pixels)
// Height				(height of the visible pixels)
// xOffset				(start X of visible pixels)
// xDelta				(the distance to move the cursor. Effective width of the character.)
// Data[n]

// NOTE: You can remove any of these characters if they are not needed in
// your application. The first character number in each Glyph indicates
// the ASCII character code. Therefore, these do not have to be sequential.
// Just remove all the content for a particular character to save space.
// ========================================================================

// dejavu
// Point Size   : 12
// Memory usage : 1158 bytes
// # characters : 95

#include "sdkconfig.h"

const unsigned char tft_DefaultFont[] =
{
0x00, 0x0B, 0x86, 0x04,

// ' '
0x20,0x0A,0x00,0x00,0x00,0x04,

// '!'
0x21,0x01,0x01,0x09,0x02,0x05,
0xFD,0x80,
// '"'
0x22,0x01,0x03,0x03,0x01,0x05,
0xB6,0x80,
// '#'
0x23,0x02,0x08,0x08,0x01,0x0A,
0x12,0x14,0x7F,0x24,0x24,0xFE,0x28,0x48,
// '$'
0x24,0x01,0x06,0x0B,0x02,0x08,
0x21,0xCA,0xA8,0xE0,0xE2,0xAA,0x70,0x82,0x00,
// '%'
0x25,0x01,0x0A,0x09,0x00,0x0B,
0x61,0x24,0x89,0x22,0x50,0x6D,0x82,0x91,0x24,0x49,0x21,0x80,
// '&'
0x26,0x01,0x09,0x09,0x01,0x0A,
0x30,0x24,0x10,0x0C,0x05,0x14,0x4A,0x19,0x8C,0x7B,0x00,
// '''
0x27,0x01,0x01,0x03,0x01,0x03,
0xE0,
// '('
0x28,0x00,0x03,0x0B,0x01,0x05,
0x69,0x49,0x24,0x48,0x80,
// ')'
0x29,0x00,0x03,0x0B,0x01,0x05,
0x89,0x12,0x49,0x4A,0x00,
// '*'
0x2A,0x01,0x05,0x06,0x01,0x06,
0x25,0x5C,0xEA,0x90,
// '+'
0x2B,0x03,0x07,0x07,0x01,0x0A,
0x10,0x20,0x47,0xF1,0x02,0x04,0x00,
// ','
0x2C,0x08,0x01,0x03,0x01,0x04,
0xE0,
// '-'
0x2D,0x06,0x03,0x01,0x01,0x04,
0xE0,
// '.'
0x2E,0x08,0x01,0x02,0x01,0x04,
0xC0,
// '/'
0x2F,0x01,0x04,0x0A,0x00,0x04,
0x11,0x22,0x24,0x44,0x88,
// '0'
0x30,0x01,0x06,0x09,0x01,0x08,
0x79,0x28,0x61,0x86,0x18,0x52,0x78,
// '1'
0x31,0x01,0x05,0x09,0x01,0x08,
0xE1,0x08,0x42,0x10,0x84,0xF8,
// '2'
0x32,0x01,0x07,0x09,0x01,0x08,
0x79,0x18,0x10,0x20,0x82,0x08,0x20,0xFC,
// '3'
0x33,0x01,0x06,0x09,0x01,0x08,
0x7A,0x10,0x41,0x38,0x30,0x63,0x78,
// '4'
0x34,0x01,0x06,0x09,0x01,0x08,
0x18,0x62,0x92,0x4A,0x2F,0xC2,0x08,
// '5'
0x35,0x01,0x06,0x09,0x01,0x08,
0xFA,0x08,0x3C,0x0C,0x10,0x63,0x78,
// '6'
0x36,0x01,0x06,0x09,0x01,0x08,
0x39,0x18,0x3E,0xCE,0x18,0x53,0x78,
// '7'
0x37,0x01,0x06,0x09,0x01,0x08,
0xFC,0x10,0x82,0x10,0x42,0x08,0x40,
// '8'
0x38,0x01,0x06,0x09,0x01,0x08,
0x7B,0x38,0x73,0x7B,0x38,0x73,0x78,
// '9'
0x39,0x01,0x06,0x09,0x01,0x08,
0x7B,0x28,0x61,0xCD,0xD0,0x62,0x70,
// ':'
0x3A,0x04,0x01,0x06,0x01,0x04,
0xCC,
// ';'
0x3B,0x04,0x01,0x07,0x01,0x04,
0xCE,
// '<'
0x3C,0x03,0x08,0x06,0x01,0x0A,
0x03,0x1E,0xE0,0xE0,0x1E,0x03,
// '='
0x3D,0x05,0x08,0x03,0x01,0x0A,
0xFF,0x00,0xFF,
// '>'
0x3E,0x03,0x08,0x06,0x01,0x0A,
0xC0,0x78,0x07,0x07,0x78,0xC0,
// '?'
0x3F,0x01,0x05,0x09,0x00,0x06,
0x74,0x42,0x22,0x10,0x04,0x20,
// '@'
0x40,0x01,0x0B,0x0B,0x01,0x0D,
0x1F,0x06,0x19,0x01,0x46,0x99,0x13,0x22,0x64,0x54,0x6C,0x40,0x04,0x10,0x7C,0x00,
// 'A'
0x41,0x01,0x08,0x09,0x00,0x08,
0x18,0x18,0x24,0x24,0x24,0x42,0x7E,0x42,0x81,
// 'B'
0x42,0x01,0x06,0x09,0x01,0x08,
0xFA,0x18,0x61,0xFA,0x18,0x61,0xF8,
// 'C'
0x43,0x01,0x06,0x09,0x01,0x08,
0x39,0x18,0x20,0x82,0x08,0x11,0x38,
// 'D'
0x44,0x01,0x07,0x09,0x01,0x09,
0xF9,0x0A,0x0C,0x18,0x30,0x60,0xC2,0xF8,
// 'E'
0x45,0x01,0x06,0x09,0x01,0x08,
0xFE,0x08,0x20,0xFE,0x08,0x20,0xFC,
// 'F'
0x46,0x01,0x05,0x09,0x01,0x07,
0xFC,0x21,0x0F,0xC2,0x10,0x80,
// 'G'
0x47,0x01,0x07,0x09,0x01,0x09,
0x3C,0x86,0x04,0x08,0xF0,0x60,0xA1,0x3C,
// 'H'
0x48,0x01,0x07,0x09,0x01,0x09,
0x83,0x06,0x0C,0x1F,0xF0,0x60,0xC1,0x82,
// 'I'
0x49,0x01,0x01,0x09,0x01,0x03,
0xFF,0x80,
// 'J'
0x4A,0x01,0x03,0x0B,0xFF,0x03,
0x24,0x92,0x49,0x27,0x00,
// 'K'
0x4B,0x01,0x07,0x09,0x01,0x07,
0x85,0x12,0x45,0x0C,0x14,0x24,0x44,0x84,
// 'L'
0x4C,0x01,0x05,0x09,0x01,0x06,
0x84,0x21,0x08,0x42,0x10,0xF8,
// 'M'
0x4D,0x01,0x08,0x09,0x01,0x0A,
0x81,0xC3,0xC3,0xA5,0xA5,0x99,0x99,0x81,0x81,
// 'N'
0x4E,0x01,0x07,0x09,0x01,0x09,
0xC3,0x86,0x8D,0x19,0x31,0x62,0xC3,0x86,
// 'O'
0x4F,0x01,0x07,0x09,0x01,0x09,
0x38,0x8A,0x0C,0x18,0x30,0x60,0xA2,0x38,
// 'P'
0x50,0x01,0x06,0x09,0x01,0x08,
0xFA,0x38,0x63,0xFA,0x08,0x20,0x80,
// 'Q'
0x51,0x01,0x07,0x0B,0x01,0x09,
0x38,0x8A,0x0C,0x18,0x30,0x60,0xA2,0x38,0x10,0x10,
// 'R'
0x52,0x01,0x07,0x09,0x01,0x08,
0xF9,0x1A,0x14,0x6F,0x91,0x21,0x42,0x82,
// 'S'
0x53,0x01,0x06,0x09,0x01,0x08,
0x7B,0x18,0x30,0x78,0x30,0x63,0x78,
// 'T'
0x54,0x01,0x07,0x09,0x00,0x07,
0xFE,0x20,0x40,0x81,0x02,0x04,0x08,0x10,
// 'U'
0x55,0x01,0x07,0x09,0x01,0x09,
0x83,0x06,0x0C,0x18,0x30,0x60,0xA2,0x38,
// 'V'
0x56,0x01,0x0A,0x09,0xFF,0x08,
0x40,0x90,0x22,0x10,0x84,0x21,0x04,0x81,0x20,0x30,0x0C,0x00,
// 'W'
0x57,0x01,0x0B,0x09,0x00,0x0B,
0x84,0x28,0x89,0x11,0x27,0x22,0xA8,0x55,0x0E,0xE0,0x88,0x11,0x00,
// 'X'
0x58,0x01,0x07,0x09,0x00,0x07,
0xC6,0x88,0xA1,0xC1,0x07,0x0A,0x22,0x82,
// 'Y'
0x59,0x01,0x07,0x09,0x00,0x07,
0x82,0x89,0x11,0x43,0x82,0x04,0x08,0x10,
// 'Z'
0x5A,0x01,0x07,0x09,0x01,0x09,
0xFE,0x04,0x10,0x41,0x04,0x10,0x40,0xFE,
// '['
0x5B,0x01,0x02,0x0B,0x02,0x05,
0xEA,0xAA,0xAC,
// '\'
0x5C,0x01,0x04,0x0A,0x00,0x04,
0x88,0x44,0x42,0x22,0x11,
// ']'
0x5D,0x01,0x02,0x0B,0x01,0x05,
0xD5,0x55,0x5C,
// '^'
0x5E,0x01,0x08,0x03,0x01,0x0A,
0x18,0x24,0x42,
// '_'
0x5F,0x0C,0x06,0x01,0x00,0x06,
0xFC,
// '`'
0x60,0x00,0x03,0x02,0x01,0x06,
0x44,
// 'a'
0x61,0x03,0x06,0x07,0x01,0x08,
0x7A,0x30,0x5F,0x86,0x37,0x40,
// 'b'
0x62,0x00,0x06,0x0A,0x01,0x08,
0x82,0x08,0x2E,0xCA,0x18,0x61,0xCE,0xE0,
// 'c'
0x63,0x03,0x05,0x07,0x01,0x07,
0x72,0x61,0x08,0x25,0xC0,
// 'd'
0x64,0x00,0x06,0x0A,0x01,0x08,
0x04,0x10,0x5D,0xCE,0x18,0x61,0xCD,0xD0,
// 'e'
0x65,0x03,0x06,0x07,0x01,0x08,
0x39,0x38,0x7F,0x81,0x13,0x80,
// 'f'
0x66,0x00,0x04,0x0A,0x00,0x04,
0x34,0x4F,0x44,0x44,0x44,
// 'g'
0x67,0x03,0x06,0x0A,0x01,0x08,
0x77,0x38,0x61,0x87,0x37,0x41,0x4C,0xE0,
// 'h'
0x68,0x00,0x06,0x0A,0x01,0x08,
0x82,0x08,0x2E,0xC6,0x18,0x61,0x86,0x10,
// 'i'
0x69,0x01,0x01,0x09,0x01,0x03,
0xBF,0x80,
// 'j'
0x6A,0x01,0x02,0x0C,0x00,0x03,
0x45,0x55,0x56,
// 'k'
0x6B,0x00,0x06,0x0A,0x01,0x07,
0x82,0x08,0x22,0x92,0x8E,0x28,0x92,0x20,
// 'l'
0x6C,0x00,0x01,0x0A,0x01,0x03,
0xFF,0xC0,
// 'm'
0x6D,0x03,0x09,0x07,0x01,0x0B,
0xB3,0x66,0x62,0x31,0x18,0x8C,0x46,0x22,
// 'n'
0x6E,0x03,0x06,0x07,0x01,0x08,
0xBB,0x18,0x61,0x86,0x18,0x40,
// 'o'
0x6F,0x03,0x06,0x07,0x01,0x08,
0x7B,0x38,0x61,0x87,0x37,0x80,
// 'p'
0x70,0x03,0x06,0x0A,0x01,0x08,
0xBB,0x28,0x61,0x87,0x3B,0xA0,0x82,0x00,
// 'q'
0x71,0x03,0x06,0x0A,0x01,0x08,
0x77,0x38,0x61,0x87,0x37,0x41,0x04,0x10,
// 'r'
0x72,0x03,0x04,0x07,0x01,0x05,
0xBC,0x88,0x88,0x80,
// 's'
0x73,0x03,0x06,0x07,0x01,0x07,
0x72,0x28,0x1C,0x0A,0x27,0x00,
// 't'
0x74,0x01,0x04,0x09,0x00,0x05,
0x44,0xF4,0x44,0x44,0x30,
// 'u'
0x75,0x03,0x06,0x07,0x01,0x08,
0x86,0x18,0x61,0x86,0x37,0x40,
// 'v'
0x76,0x03,0x08,0x07,0xFF,0x06,
0x42,0x42,0x24,0x24,0x24,0x18,0x18,
// 'w'
0x77,0x03,0x09,0x07,0x00,0x09,
0x88,0xC4,0x57,0x4A,0xA5,0x51,0x10,0x88,
// 'x'
0x78,0x03,0x06,0x07,0x00,0x06,
0x85,0x24,0x8C,0x49,0x28,0x40,
// 'y'
0x79,0x03,0x08,0x0A,0xFF,0x06,
0x42,0x42,0x24,0x24,0x14,0x18,0x08,0x08,0x10,0x60,
// 'z'
0x7A,0x03,0x05,0x07,0x00,0x05,
0xF8,0x44,0x44,0x43,0xE0,
// '{'
0x7B,0x01,0x05,0x0B,0x02,0x08,
0x19,0x08,0x42,0x60,0x84,0x21,0x06,
// '|'
0x7C,0x01,0x01,0x0C,0x02,0x04,
0xFF,0xF0,
// '}'
0x7D,0x01,0x05,0x0B,0x01,0x08,
0xC1,0x08,0x42,0x0C,0x84,0x21,0x30,
// '~'
0x7E,0x04,0x08,0x03,0x01,0x0A,
0x00,0x71,0x8E,

// Terminator
0xFF
};
