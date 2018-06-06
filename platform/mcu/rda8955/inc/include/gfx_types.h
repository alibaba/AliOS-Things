/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _GFX_TYPES_H_
#define _GFX_TYPES_H_



// =============================================================================
//  MACROS
// =============================================================================

// ============================================================================
// GFX_WIDESCREEN_ASPECT_T
// -----------------------------------------------------------------------------
/// Describe the display aspect
// =============================================================================
typedef enum
{
    GFX_WIDESCREEN_ASPECT_FULLSCREEN            = 0x00000000,
    GFX_WIDESCREEN_ASPECT_LETTERBOX             = 0x00000001,
    GFX_WIDESCREEN_ASPECT_PANSCAN               = 0x00000002
} GFX_WIDESCREEN_ASPECT_T;


// ============================================================================
// GFX_COLOR_FORMAT_T
// -----------------------------------------------------------------------------
/// This type describes how the color are encoded within a frame buffer.
// =============================================================================
typedef enum
{
/// to track not initialized structures.
    GFX_COLOR_FORMAT_INVALID                    = 0x00000000,
/// Regular RGB 565 format.
    GFX_COLOR_FORMAT_RGB_565                    = 0x00000001,
/// RGB565 format with swapped pixels (for Greenstone Camera module).
    GFX_COLOR_FORMAT_RGB_565_PIXELSWAP          = 0x00000002,
/// RVG Alpha 888 8 format.
    GFX_COLOR_FORMAT_RGBA                       = 0x00000003,
/// YUV 4:2:2 pixel packed (UYVY).
    GFX_COLOR_FORMAT_UYVY                       = 0x00000004,
/// YUV 4:2:2 pixel packed (YUYV).
    GFX_COLOR_FORMAT_YUYV                       = 0x00000005,
/// YUV 4:2:0 planar (IYUV).
    GFX_COLOR_FORMAT_IYUV                       = 0x00000006,
    GFX_COLOR_FORMAT_QTY                        = 0x00000007
} GFX_COLOR_FORMAT_T;


// ============================================================================
// GFX_ROTATION_T
// -----------------------------------------------------------------------------
/// This type describes the rotations.
// =============================================================================
typedef enum
{
/// No Rotation.
    GFX_ROTATION_NONE                           = 0x00000000,
/// Rotation of 90 degrees Counter Clockwise.
    GFX_ROTATION_90                             = 0x0000005A,
/// Rotation of 180 degrees Counter Clockwise.
    GFX_ROTATION_180                            = 0x000000B4,
/// Rotation of 270 degrees Counter Clockwise.
    GFX_ROTATION_270                            = 0x0000010E
} GFX_ROTATION_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// GFX_POINT_T
// -----------------------------------------------------------------------------
/// This type describes a graphical point.
// =============================================================================
typedef struct
{
    /// x coordinate of the point.
    INT16                          x;                            //0x00000000
    /// y coordinate of the point.
    INT16                          y;                            //0x00000002
} GFX_POINT_T; //Size : 0x4



// ============================================================================
// GFX_RECT_T
// -----------------------------------------------------------------------------
/// This type describes a graphical rectangular region described by its corner points.
// =============================================================================
typedef struct
{
    /// x coordinate of the top left corner of the region.
    INT16                          left;                         //0x00000000
    /// y coordinate of the top left corner of the region.
    INT16                          top;                          //0x00000002
    /// x coordinate of the bottom right corner of the region.
    INT16                          right;                        //0x00000004
    /// y coordinate of the bottom right corner of the region.
    INT16                          bottom;                       //0x00000006
} GFX_RECT_T; //Size : 0x8



// ============================================================================
// GFX_ROI_T
// -----------------------------------------------------------------------------
/// Region Of Interest: This type describes a graphical rectangular region described
/// by its start point and size.
// =============================================================================
typedef struct
{
    /// starting point of the region.
    GFX_POINT_T                    start;                        //0x00000000
    /// width of the region.
    UINT16                         width;                        //0x00000004
    /// height of the region.
    UINT16                         height;                       //0x00000006
} GFX_ROI_T; //Size : 0x8



// ============================================================================
// GFX_FB_T
// -----------------------------------------------------------------------------
/// Frame Buffer: This type describes a frame buffer through its dimensions and a
/// pointer to the data of the buffer. It is mandatory for the buffer to be able
/// to hold the data describing all the pixels at the color format chosen.
///  (ie sizeof(buffer) = width*height*NbOfBytesPerPixelAtTheFormat(format))
// =============================================================================
typedef struct
{
    UINT16*                        buffer;                       //0x00000000
    /// width of the buffer.
    UINT16                         width;                        //0x00000004
    /// height of the buffer.
    UINT16                         height;                       //0x00000006
    /// Color encoding of the buffer.
    GFX_COLOR_FORMAT_T             colorFormat;                  //0x00000008
} GFX_FB_T; //Size : 0xC



// ============================================================================
// GFX_FBW_T
// -----------------------------------------------------------------------------
/// Frame Buffer Window: This type describes a window in a frame buffer as a region
/// of interest within this frame buffer. When the region of interest starts at (0,0)
/// and has the same dimensions as the frame buffer, the window is in fact the whole
/// frame buffer.
// =============================================================================
typedef struct
{
    /// Frame buffer.
    GFX_FB_T                       fb;                           //0x00000000
    /// Region of interest.
    GFX_ROI_T                      roi;                          //0x0000000C
} GFX_FBW_T; //Size : 0x14





// ============================================================================
// GFX_FB_T IYUV buffer accessors
// -----------------------------------------------------------------------------
// The macros are assuming that the width is a multiple of 4.
// ============================================================================
/// Access macros for IYUV format to access the Y buffer
#define GFX_FB_IYUV_Y(fb) ((UINT8)((fb).buffer))
/// Access macros for IYUV format to access the U buffer
#define GFX_FB_IYUV_U(fb) ((UINT8)((fb).buffer)+((fb).width*(fb).height))
/// Access macros for IYUV format to access the V buffer
#define GFX_FB_IYUV_V(fb) ((UINT8)((fb).buffer)+(((fb).width*(fb).height*5)>>2))


#endif

