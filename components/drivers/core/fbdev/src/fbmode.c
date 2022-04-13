/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */




/**
 * fb_var_to_videomode - convert fb_var_screeninfo to fb_videomode
 * @mode: pointer to struct fb_videomode
 * @var: pointer to struct fb_var_screeninfo
 */
void fb_var_to_videomode(fb_videomode_t *mode,
             const fb_var_screeninfo_t *var)
{
    uint32_t pixclock, hfreq, htotal, vtotal;

    mode->name = NULL;
    mode->xres = var->xres;
    mode->yres = var->yres;
    mode->pixclock = var->pixclock;
    mode->hsync_len = var->hsync_len;
    mode->vsync_len = var->vsync_len;
    mode->left_margin = var->left_margin;
    mode->right_margin = var->right_margin;
    mode->upper_margin = var->upper_margin;
    mode->lower_margin = var->lower_margin;
    mode->sync = var->sync;
    mode->vmode = var->vmode & FB_VMODE_MASK;
    mode->flag = FB_MODE_IS_FROM_VAR;
    mode->refresh = 0;

    if (!var->pixclock)
        return;

    pixclock = PICOS2KHZ(var->pixclock) * 1000;

    htotal = var->xres + var->right_margin + var->hsync_len +
        var->left_margin;
    vtotal = var->yres + var->lower_margin + var->vsync_len +
        var->upper_margin;

    if (var->vmode & FB_VMODE_INTERLACED)
        vtotal /= 2;
    if (var->vmode & FB_VMODE_DOUBLE)
        vtotal *= 2;

    if (!htotal || !vtotal)
        return;

    hfreq = pixclock / htotal;
    mode->refresh = hfreq / vtotal;
}
