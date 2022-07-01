/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Image Python module.
 */
#ifndef __PY_IMAGE_H__
#define __PY_IMAGE_H__
#include "imlib.h"
mp_obj_t py_image(int width, int height, int bpp, void *pixels);
mp_obj_t py_image_from_struct(image_t *img);
void *py_image_cobj(mp_obj_t img_obj);
int py_image_descriptor_from_roi(image_t *img, const char *path, rectangle_t *roi);
#endif // __PY_IMAGE_H__
