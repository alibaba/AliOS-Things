/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Framebuffer functions.
 */
#include <stdio.h>
#include "mpprint.h"
#include "framebuffer.h"
#include "omv_boardconfig.h"

#define FB_ALIGN_SIZE_ROUND_DOWN(x) (((x) / FRAMEBUFFER_ALIGNMENT) * FRAMEBUFFER_ALIGNMENT)
#define FB_ALIGN_SIZE_ROUND_UP(x)   FB_ALIGN_SIZE_ROUND_DOWN(((x) + FRAMEBUFFER_ALIGNMENT - 1))
#define CONSERVATIVE_JPEG_BUF_SIZE  (OMV_JPEG_BUF_SIZE-64)

framebuffer_t *framebuffer = NULL;
jpegbuffer_t *jpeg_framebuffer = NULL;

void fb_set_streaming_enabled(bool enable)
{
    framebuffer->streaming_enabled = enable;
}

bool fb_get_streaming_enabled()
{
    return framebuffer->streaming_enabled;
}

int fb_encode_for_ide_new_size(image_t *img)
{
    return (((img->bpp * 8) + 5) / 6) + 2;
}

void fb_encode_for_ide(uint8_t *ptr, image_t *img)
{
    *ptr++ = 0xFE;

    for(int i = 0, j = (img->bpp / 3) * 3; i < j; i += 3) {
        int x = 0;
        x |= img->data[i + 0] << 0;
        x |= img->data[i + 1] << 8;
        x |= img->data[i + 2] << 16;
        *ptr++ = 0x80 | ((x >> 0) & 0x3F);
        *ptr++ = 0x80 | ((x >> 6) & 0x3F);
        *ptr++ = 0x80 | ((x >> 12) & 0x3F);
        *ptr++ = 0x80 | ((x >> 18) & 0x3F);
    }

    if((img->bpp % 3) == 2) { // 2 bytes -> 16-bits -> 24-bits sent
        int x = 0;
        x |= img->data[img->bpp - 2] << 0;
        x |= img->data[img->bpp - 1] << 8;
        *ptr++ = 0x80 | ((x >> 0) & 0x3F);
        *ptr++ = 0x80 | ((x >> 6) & 0x3F);
        *ptr++ = 0x80 | ((x >> 12) & 0x3F);
    }

    if((img->bpp % 3) == 1) { // 1 byte -> 8-bits -> 16-bits sent
        int x = 0;
        x |= img->data[img->bpp - 1] << 0;
        *ptr++ = 0x80 | ((x >> 0) & 0x3F);
        *ptr++ = 0x80 | ((x >> 6) & 0x3F);
    }

    *ptr++ = 0xFE;
}

void framebuffer_init0()
{
    jpeg_framebuffer = heap_caps_malloc(OMV_JPEG_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_32BIT);
    printf("jpeg_framebuffer(%p), OMV_JPEG_BUF_SIZE[%d]\n",jpeg_framebuffer, OMV_JPEG_BUF_SIZE);
    if(jpeg_framebuffer == NULL)
    {
        printf("malloc heap for mp task failed, heap size if %d\n", OMV_JPEG_BUF_SIZE);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    framebuffer = heap_caps_malloc(OMV_FB_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_32BIT);
    printf("framebuffer(%p), OMV_FB_SIZE[%d]\n",framebuffer, OMV_FB_SIZE);
    if(framebuffer == NULL)
    {
        printf("malloc heap for mp task failed, heap size if %d\n", OMV_FB_SIZE);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    
    // Save fb_enabled flag state
    int fb_enabled = JPEG_FB()->enabled;
  
    // Clear framebuffers
    memset(MAIN_FB(), 0, sizeof(*MAIN_FB()));
    memset(JPEG_FB(), 0, sizeof(*JPEG_FB()));

    // Enable streaming.
    MAIN_FB()->streaming_enabled = true; // controlled by the OpenMV Cam.

    // Set default quality
    JPEG_FB()->quality = ((JPEG_QUALITY_HIGH - JPEG_QUALITY_LOW) / 2) + JPEG_QUALITY_LOW;

    // Set fb_enabled
    JPEG_FB()->enabled = fb_enabled; // controlled by the IDE.

    // Setup buffering.
    framebuffer_set_buffers(1);
}

void framebuffer_initialize_image(image_t *img)
{
    img->w = framebuffer->w;
    img->h = framebuffer->h;
    img->bpp = framebuffer->bpp;
    img->data = framebuffer_get_buffer(framebuffer->head)->data;
}

static void initialize_jpeg_buf_from_image(image_t *img)
{
    if (!img) {
        jpeg_framebuffer->w = 0;
        jpeg_framebuffer->h = 0;
        jpeg_framebuffer->size = 0;
    } else {
        jpeg_framebuffer->w = img->w;
        jpeg_framebuffer->h = img->h;
        jpeg_framebuffer->size = img->bpp;
    }
}

void framebuffer_update_jpeg_buffer()
{
    static int overflow_count = 0;

    image_t main_fb_src;
    framebuffer_initialize_image(&main_fb_src);
    image_t *src = &main_fb_src;

    if (framebuffer->streaming_enabled && jpeg_framebuffer->enabled) {
        if (src->bpp > 4) {
            bool does_not_fit = false;

            if (mutex_try_lock(&jpeg_framebuffer->lock, MUTEX_TID_OMV)) {
                if(CONSERVATIVE_JPEG_BUF_SIZE < src->bpp) {
                    initialize_jpeg_buf_from_image(NULL);
                    does_not_fit = true;
                } else {
                    initialize_jpeg_buf_from_image(src);
                    memcpy(jpeg_framebuffer->pixels, src->pixels, src->bpp);
                }

                mutex_unlock(&jpeg_framebuffer->lock, MUTEX_TID_OMV);
            }

            if (does_not_fit) {
                printf("Warning: JPEG too big! Trying framebuffer transfer using fallback method!\n");
                int new_size = fb_encode_for_ide_new_size(src);
                fb_alloc_mark();
                uint8_t *temp = fb_alloc(new_size, FB_ALLOC_NO_HINT);
                fb_encode_for_ide(temp, src);
                (MP_PYTHON_PRINTER)->print_strn((MP_PYTHON_PRINTER)->data, (const char *) temp, new_size);
                fb_alloc_free_till_mark();
            }
        } else if (src->bpp >= 0) {
            if (mutex_try_lock(&jpeg_framebuffer->lock, MUTEX_TID_OMV)) {
                image_t dst = {.w=src->w, .h=src->h, .bpp=CONSERVATIVE_JPEG_BUF_SIZE, .pixels=jpeg_framebuffer->pixels};
                // Note: lower quality saves USB bandwidth and results in a faster IDE FPS.
                bool overflow = jpeg_compress(src, &dst, jpeg_framebuffer->quality, false);

                if (overflow) {
                    // JPEG buffer overflowed, reduce JPEG quality for the next frame
                    // and skip the current frame. The IDE doesn't receive this frame.
                    if (jpeg_framebuffer->quality > 1) {
                        // Keep this quality for the next n frames
                        overflow_count = 60;
                        jpeg_framebuffer->quality = IM_MAX(1, (jpeg_framebuffer->quality/2));
                    }

                    initialize_jpeg_buf_from_image(NULL);
                } else {
                    if (overflow_count) {
                        overflow_count--;
                    }

                    // Dynamically adjust our quality if the image is huge.
                    bool big_frame_buffer = image_size(src) > JPEG_QUALITY_THRESH;
                    int jpeg_quality_max = big_frame_buffer ? JPEG_QUALITY_LOW : JPEG_QUALITY_HIGH;

                    // No buffer overflow, increase quality up to max quality based on frame size...
                    if ((!overflow_count) && (jpeg_framebuffer->quality < jpeg_quality_max)) {
                        jpeg_framebuffer->quality++;
                    }

                    initialize_jpeg_buf_from_image(&dst);
                }

                mutex_unlock(&jpeg_framebuffer->lock, MUTEX_TID_OMV);
            }
        }
    }
}

int32_t framebuffer_get_x()
{
    return framebuffer->x;
}

int32_t framebuffer_get_y()
{
    return framebuffer->y;
}

int32_t framebuffer_get_u()
{
    return framebuffer->u;
}

int32_t framebuffer_get_v()
{
    return framebuffer->v;
}

int32_t framebuffer_get_width()
{
    return framebuffer->w;
}

int32_t framebuffer_get_height()
{
    return framebuffer->h;
}

int32_t framebuffer_get_depth()
{
    return framebuffer->bpp;
}

// Returns the number of bytes the frame buffer could be at the current moment it time.
static uint32_t framebuffer_raw_buffer_size()
{
    uint32_t size = (uint32_t) (fb_alloc_stack_pointer() - ((char *) framebuffer->data));
    // We don't want to give all of the frame buffer RAM to the frame buffer. So, we will limit
    // the maximum amount of RAM we return.
    return IM_MIN(size, OMV_RAW_BUF_SIZE);
}

uint32_t framebuffer_get_buffer_size()
{
    uint32_t size;

    if (framebuffer->n_buffers == 1) {
        // With only 1 vbuffer it's fine to allow the frame buffer size to change given fb_alloc().
        size = framebuffer_raw_buffer_size();
    } else {
        // Whatever the raw size was when the number of buffers were set is locked in...
        size = framebuffer->raw_buffer_size;
    }

    // Remove the size of the state header plus alignment padding.
    size -= sizeof(vbuffer_t);

    // Do we have an estimate on the frame size with mutliple buffers? If so, we can reduce the
    // RAM each buffer takes up giving some space back to fb_alloc().
    if ((framebuffer->n_buffers != 1) && framebuffer->u && framebuffer->v) {
        // Typically a framebuffer will not need more than u*v*2 bytes.
        uint32_t size_guess = framebuffer->u * framebuffer->v * 2;
        // Add in extra bytes to prevent round down from shrinking buffer too small.
        size_guess += FRAMEBUFFER_ALIGNMENT - 1;
        // Limit the frame buffer size.
        size = IM_MIN(size, size_guess);
    }

    // Needs to be a multiple of FRAMEBUFFER_ALIGNMENT for DMA transfers...
    return FB_ALIGN_SIZE_ROUND_DOWN(size);
}

// Each raw frame buffer is split into two parts. The vbuffer_t struct followed by
// padding and then the pixel array starting at the next 32-byte offset.
vbuffer_t *framebuffer_get_buffer(int32_t index)
{
    uint32_t offset = (sizeof(vbuffer_t) + framebuffer_get_buffer_size()) * index;
    return (vbuffer_t *) (framebuffer->data + offset);
}

void framebuffer_set(int32_t w, int32_t h, int32_t bpp)
{
    framebuffer->w = w;
    framebuffer->h = h;
    framebuffer->bpp = bpp;
}

void framebuffer_flush_buffers()
{
    // Move the tail pointer to the head which empties the virtual fifo while keeping the same
    // position of the current frame for the rest of the code.
    framebuffer->tail = framebuffer->head;
    framebuffer->check_head = true;
    framebuffer->sampled_head = 0;
}

void framebuffer_reset_buffers()
{
    for (int32_t i = 0; i < framebuffer->n_buffers; i++) {
        memset(framebuffer_get_buffer(i), 0, sizeof(vbuffer_t));
    }

    framebuffer_flush_buffers();
}

int framebuffer_set_buffers(int32_t n_buffers)
{
    uint32_t total_size = framebuffer_raw_buffer_size();
    uint32_t size = total_size / n_buffers;

    // Error out if frame buffers are smaller than this...
    if (size < (sizeof(vbuffer_t) + FRAMEBUFFER_ALIGNMENT)) {
        return -1;
    }

    // Invalidate frame.
    framebuffer->bpp = -1;

    // Cache the maximum size we can allocate for the frame buffer when vbuffers are greater than 1.
    framebuffer->raw_buffer_size = size;
    framebuffer->n_buffers = n_buffers;
    framebuffer->head = 0;

    framebuffer_reset_buffers();

    return 0;
}

// Returns the real size of bytes in the frame buffer.
static uint32_t framebuffer_total_buffer_size()
{
    if (framebuffer->n_buffers == 1) {
        // Allow fb_alloc to use frame buffer space up until the image size.
        image_t img;
        framebuffer_initialize_image(&img);
        return sizeof(vbuffer_t) + FB_ALIGN_SIZE_ROUND_UP(image_size(&img));
    } else {
        // fb_alloc may only use up to the size of all the virtual buffers...
        return (sizeof(vbuffer_t) + framebuffer_get_buffer_size()) * framebuffer->n_buffers;
    }
}

void framebuffer_auto_adjust_buffers()
{
     // Keep same buffer count in video fifo mode but resize buffer sizes.
    if (framebuffer->n_buffers > 3) {
        framebuffer_set_buffers(framebuffer->n_buffers);
        return;
    }

    for (int i = 3; i > 0; i--) {
        framebuffer_set_buffers(i);

        // Find a buffering size automatically that doesn't use more than half.
        if (fb_avail() >= framebuffer_total_buffer_size()) {
            return;
        }
    }
}

void framebuffer_free_current_buffer()
{
    // Invalidate frame.
    framebuffer->bpp = -1;

    // Allow frame to be updated in single buffer mode...
    if (framebuffer->n_buffers == 1) {
        framebuffer_get_buffer(framebuffer->head)->waiting_for_data = true;
    }
}

vbuffer_t *framebuffer_get_head(framebuffer_flags_t flags)
{
    int32_t new_head = (framebuffer->head + 1) % framebuffer->n_buffers;

    // Single Buffer Mode.
    if (framebuffer->n_buffers == 1) {
        if (framebuffer_get_buffer(framebuffer->head)->waiting_for_data) {
            return NULL;
        }
    // Double Buffer Mode.
    } else if (framebuffer->n_buffers == 2) {
        if (framebuffer->head == framebuffer->tail) {
            return NULL;
        }
    // Triple Buffer Mode.
    } else if (framebuffer->n_buffers == 3) {
        int32_t sampled_tail = framebuffer->tail;
        if (framebuffer->head == sampled_tail) {
            return NULL;
        } else {
            new_head = sampled_tail;
        }
    // Video FIFO Mode.
    } else {
        if (framebuffer->head == framebuffer->tail) {
            return NULL;
        }
    }

    if (!(flags & FB_PEEK)) {
        framebuffer->head = new_head;
    }

    return framebuffer_get_buffer(new_head);
}

vbuffer_t *framebuffer_get_tail(framebuffer_flags_t flags)
{
    // Sample head on the first line of a new frame.
    if (framebuffer->check_head) {
        framebuffer->check_head = false;
        framebuffer->sampled_head = framebuffer->head;
    }

    int32_t new_tail = (framebuffer->tail + 1) % framebuffer->n_buffers;

    // Single Buffer Mode.
    if (framebuffer->n_buffers == 1) {
        if (!framebuffer_get_buffer(new_tail)->waiting_for_data) {
            // Setup to check head again.
            framebuffer->check_head = true;
            return NULL;
        }
    // Double Buffer Mode.
    } else if (framebuffer->n_buffers == 2) {
        if (new_tail == framebuffer->sampled_head) {
            // Setup to check head again.
            framebuffer->check_head = true;
            return NULL;
        }
    // Triple Buffer Mode.
    } else if (framebuffer->n_buffers == 3) {
        // For triple buffering we are never writing where tail or head
        // (which may instantly update to be equal to tail) is.
        if (new_tail == framebuffer->sampled_head) {
            new_tail = (new_tail + 1) % framebuffer->n_buffers;
        }
    // Video FIFO Mode.
    } else {
        if (new_tail == framebuffer->sampled_head) {
            // Setup to check head again.
            framebuffer->check_head = true;
            return NULL;
        }
    }

    vbuffer_t *buffer = framebuffer_get_buffer(new_tail);

    // Reset on start versus the end so offset and jpeg_buffer_overflow are valid after FB_COMMIT.
    if (buffer->reset_state) {
        buffer->reset_state = false;
        buffer->offset = 0;
        buffer->jpeg_buffer_overflow = false;
    }

    if (!(flags & FB_PEEK)) {
        // Trigger reset on the frame buffer the next time it is used.
        buffer->reset_state = true;

        // Mark the frame buffer ready in single buffer mode.
        if (framebuffer->n_buffers == 1) {
            buffer->waiting_for_data = false;
        }

        framebuffer->tail = new_tail;

        // Setup to check head again.
        framebuffer->check_head = true;
    }

    return buffer;
}

char *framebuffer_get_buffers_end()
{
    return (char *) (framebuffer->data + framebuffer_total_buffer_size());
}
