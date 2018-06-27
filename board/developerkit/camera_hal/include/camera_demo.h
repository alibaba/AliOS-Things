#ifndef _CAMERA_IMAGES_H
#define _CAMERA_IMAGES_H


#define ST7789_WIDTH ((uint16_t)240)
#define ST7789_HEIGHT ((uint16_t)240)

/*************
The first part of the bitmap file header
The length of the structure is fixed, which is 12 bytes. The order of the fields is as follows:
4byte : entire file size
4byte : reserved word, 0
4byte : Offset bytes from the file header to the actual bitmap image data.
*************/
typedef struct
{
    uint32_t imageSize;
    uint32_t blank;
    uint32_t startPosition;
}BmpHead;

/*********************
The second part of the bitmap information header
The length of the structure is also fixed, which is 40 bytes. Each field is described in turn as follows:
4byte : length of this structure, value 40
4byte : how many pixels are the width of the image.
4byte : how many pixels are the height of the image.
2Byte : Must be 1.
2Byte : The number of bits used to represent the color. Commonly used values ??are 1 (black and white dichroic map), 4 (16 color map), 8 (256 color map), and 24 (true color map).
4byte : Specifies whether the bitmap is compressed. Valid values ??are BI_RGB, BI_RLE8, BI_RLE4, BI_BITFIELDS. The Windows bitmap can use the compression format of RLE4 and RLE8, and BI_RGB means no compression.
4byte : Specifies the number of bytes occupied by the actual bitmap image data. It can be calculated using the following formula:
	    Image data = Width' * Height * The number of bytes occupied by each pixel's color (ie, the number of color bits/8, 24 bitmap is 3, and 256 colors are 1)
	    It should be noted that the biWidth' in the above formula must be an integral multiple of 4 (not biWidth but an integer multiple of greater than or equal to the biWidth's minimum of 4).
	    If biCompression is BI_RGB, this item may be 0.
4byte : The horizontal resolution of the target device.
4byte : The vertical resolution of the target device.
4byte : The number of colors actually used in the image. If the value is 0, the number of colors used is 2 (the number of color bits). For example, the number of colors is 8, 2^8 = 256, which is 256 colors. Bitmap
4byte: Specifies the number of important colors in the image. If the value is 0, all colors are considered important.
***********************************/

typedef struct
{
    uint32_t    Length;
    uint32_t    width;
    uint32_t    height;
    uint16_t    colorPlane;
    uint16_t    bitColor;
    uint32_t    zipFormat;
    uint32_t    realSize;
    uint32_t    xPels;
    uint32_t    yPels;
    uint32_t    colorUse;
    uint32_t    colorImportant;
}InfoHead;

/***************************
The third part of the palette structure color table
For 256-color BMP bitmaps, the number of colors is 8, requiring 2^8 = 256 palettes;
 For 24bit BMP bitmaps, the RGB values of each pixel are stored directly in the image data area, no palette is required, no palette is present
rgbBlue: The blue component of this color.
rgbGreen: The green component of the color.
rgbRed: The red component of this color.
rgbReserved: Reserved value?
************************/
typedef struct
{         
	  uint8_t   rgbBlue;
         uint8_t   rgbGreen;
         uint8_t   rgbRed;
         uint8_t   rgbReserved;
}RGBMixPlate;		 

void CameraDEMO_Init(uint16_t *buff, uint32_t size);
void enable_camera_display(uint8_t on);

#endif  /* Avoid multiple inclusion */

