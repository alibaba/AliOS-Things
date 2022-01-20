#
# Library of function for image manipulation.
# Can be used with lodepng to decode PNG images:
#
#     from imagetools import get_png_info, open_png
#     decoder = lv.img.decoder_create()
#     decoder.info_cb = get_png_info
#     decoder.open_cb = open_png
#
# Support transparency (alpha)
#
# TODO:
# - Support more color formats
#

import lvgl as lv
import lodepng as png
import struct

COLOR_SIZE = lv.color_t.__SIZE__
COLOR_IS_SWAPPED = hasattr(lv.color_t().ch,'green_h')

class lodepng_error(RuntimeError):
    def __init__(self, err):
        if type(err) is int:
            super().__init__(png.error_text(err))
        else:
            super().__init__(err)

# Parse PNG file header
# Taken from https://github.com/shibukawa/imagesize_py/blob/ffef30c1a4715c5acf90e8945ceb77f4a2ed2d45/imagesize.py#L63-L85

@micropython.native
def get_png_info(decoder, src, header):
    # Only handle variable image types

    if lv.img.src_get_type(src) != lv.img.SRC.VARIABLE:
        return lv.RES.INV

    data = lv.img_dsc_t.__cast__(src).data
    if data == None:
        return lv.RES.INV
    
    png_header = bytes(data.__dereference__(24))

    if png_header.startswith(b'\211PNG\r\n\032\n'):
        if png_header[12:16] == b'IHDR':
            start = 16
        # Maybe this is for an older PNG version.
        else:
            start = 8
        try:
            width, height = struct.unpack(">LL", png_header[start:start+8])
        except struct.error:
            return lv.RES.INV
    else:
        return lv.RES.INV

    header.always_zero = 0
    header.w = width
    header.h = height
    header.cf = lv.img.CF.TRUE_COLOR_ALPHA

    return lv.RES.OK

# Convert color formats

@micropython.viper
def convert_rgba8888_to_bgra5658(img_view):
    p = ptr32(img_view)
    p_out = ptr8(img_view)
    img_size = int(len(img_view)) // 4
    for i in range(0, img_size):
        r = p[i] & 0xFF
        g = (p[i] >> 8) & 0xFF
        b = (p[i] >> 16) & 0xFF
        a = (p[i] >> 24) & 0xFF
        i_out = i*3
        p_out[i_out] = \
            ((b & 0b11111000) >> 3) |\
            ((g & 0b00011100) << 3)
        p_out[i_out + 1] = \
            ((g & 0b11100000) >> 5) |\
            ((r & 0b11111000) )
        p_out[i_out + 2] = a

@micropython.viper
def convert_rgba8888_to_swapped_bgra5658(img_view):
    p = ptr32(img_view)
    p_out = ptr8(img_view)
    img_size = int(len(img_view)) // 4
    for i in range(0, img_size):
        r = p[i] & 0xFF
        g = (p[i] >> 8) & 0xFF
        b = (p[i] >> 16) & 0xFF
        a = (p[i] >> 24) & 0xFF
        i_out = i*3
        p_out[i_out] = \
            ((g & 0b11100000) >> 5) |\
            ((r & 0b11111000) )
        p_out[i_out + 1] = \
            ((b & 0b11111000) >> 3) |\
            ((g & 0b00011100) << 3)
        p_out[i_out + 2] = a

@micropython.viper
def convert_rgba8888_to_bgra8888(img_view):
    p = ptr32(img_view)
    img_size = int(len(img_view)) // 4
    for i in range(0, img_size):
        r = p[i] & 0xFF
        g = (p[i] >> 8) & 0xFF
        b = (p[i] >> 16) & 0xFF
        a = (p[i] >> 24) & 0xFF
        p[i] = \
            (b) |\
            (g << 8) |\
            (r << 16) |\
            (a << 24)


# Read and parse PNG file

@micropython.native
def open_png(decoder, dsc):
    img_dsc = lv.img_dsc_t.__cast__(dsc.src)
    png_data = img_dsc.data
    png_size = img_dsc.data_size
    png_decoded = png.C_Pointer()
    png_width = png.C_Pointer()
    png_height = png.C_Pointer()
    error = png.decode32(png_decoded, png_width, png_height, png_data, png_size);
    if error:
        raise lodepng_error(error)
    img_size = png_width.int_val * png_height.int_val * 4
    img_data = png_decoded.ptr_val
    img_view = img_data.__dereference__(img_size)

    if COLOR_SIZE == 4:
        convert_rgba8888_to_bgra8888(img_view)
    elif COLOR_SIZE == 2:
        if COLOR_IS_SWAPPED:
            convert_rgba8888_to_swapped_bgra5658(img_view)
        else:
            convert_rgba8888_to_bgra5658(img_view)
    else:
        raise lodepng_error("Error: Color mode not supported yet!")

    dsc.img_data = img_data
    return lv.RES.OK

