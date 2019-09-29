/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <k_api.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"
#include "aos/yloop.h"

#include <soc_init.h>
#include "quirc.h"
#include <camera.h>
#include "qrcodegen.h"

static uint8_t *buf = 0;
static void captured();
static int frameReady = 0;
static struct quirc *pQR = 0;
static struct quirc_code qr_code;
static struct quirc_data qr_data;
static uint8_t keyB = 0;

#define IMAGE_W 240
#define IMAGE_H 240

#define RED(a)      ((((uint16_t)(a) & 0xf800) >> 11) << 3)
#define GREEN(a)    ((((uint16_t)(a) & 0x07e0) >> 5) << 2)
#define BLUE(a)     (((uint16_t)(a) & 0x001f) << 3)
#define RGB565_TO_GRAY(a) (uint8_t)((RED(a)+GREEN(a)+GREEN(a)+BLUE(a))>>2)
#define GRAY_TO_RGB565(g) ((((uint16_t)(g)>>3)<<11)| (((uint16_t)(g)>>2)<<5) | (((uint16_t)(g)>>3)))
#define RGB565_ENDIAN(h) (((uint16_t)(h)>>8)|((uint16_t)(h)&0xff)<<8)

static void capture_action(void *arg)
{
    CameraHAL_Capture_Start(buf, IMAGE_W * IMAGE_H / 4, captured);
}

void dump_cells(const struct quirc_code *code)
{
    int u, v;

    printf("    %d cells, corners:", code->size);
    for (u = 0; u < 4; u++)
        printf(" (%d,%d)", code->corners[u].x,
               code->corners[u].y);
    printf("\n");

    for (v = 0; v < code->size; v++) {
        printf("    ");
        for (u = 0; u < code->size; u++) {
            int p = v * code->size + u;

            if (code->cell_bitmap[p >> 3] & (1 << (p & 7))) {
                printf("[]");
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
}

// Prints the given QR Code to the console.
static void printQr(const uint8_t qrcode[])
{
    int size = qrcodegen_getSize(qrcode);
    int border = 4;
    for (int y = -border; y < size + border; y++) {
        for (int x = -border; x < size + border; x++) {
            printf((qrcodegen_getModule(qrcode, x, y) ? "##" : "  "));
        }
        printf("\n");
    }
    printf("\n");
}

static void qr_decode_action(input_event_t *event, void *arg)
{
    //if(event->code != EV_USER+1) return;
    int delay = 20;
    if (frameReady && !keyB) {

        //camera_dispaly(buf, 240*240);

        if (pQR) {
            uint8_t *image = buf;
            int w = 0, h = 0, i, j;

            image = quirc_begin(pQR, &w, &h);
            //LOG("Prepare QR scan on %p %dx%d\n", image, w, h);

            //TODO: we'd better let camera_hal capture lum data directly.
            // for(j=0; j<h; j++){
            //     for(i=0; i<w; i++){
            //         //uint8_t gray = RGB565_TO_GRAY(RGB565_ENDIAN(*((uint16_t *)(buf)+j*w+i)));
            //         uint8_t gray = buf[j*w+i];
            //         image[j*w+239-i] = gray;//(gray>0x80)?0xff:0;
            //         //*((uint16_t *)(buf)+j*w+i) = RGB565_ENDIAN(GRAY_TO_RGB565(gray));
            //     }
            // }

            camera_display_gray(image, IMAGE_W * IMAGE_H);

            //LOG("Start scan\n");
            quirc_end(pQR);

            int num_codes = quirc_count(pQR);
            //LOG("Scan QR count %d\n", num_codes);

            if (num_codes) {
                quirc_decode_error_t err;

                //LOG("QR Code Extracting\n");
                quirc_extract(pQR, 0, &qr_code);

                dump_cells(&qr_code);

                /* Decoding stage */
                //LOG("QR Code Decoding\n");
                err = quirc_decode(&qr_code, &qr_data);

                if (err) {
                    LOG("DECODE FAILED: %s\n", quirc_strerror(err));
                } else {
                    LOG("Data: %s\n", qr_data.payload);
                }
                //dump_image();

            }
        }

        frameReady = 0;


    }

    if (keyB && buf) {
        static uint8_t qrcode[qrcodegen_BUFFER_LEN_FOR_VERSION(26)];
        static uint8_t tempBuffer[qrcodegen_BUFFER_LEN_FOR_VERSION(26)];
        LOG("Encoding QR\n");
        bool ok = qrcodegen_encodeText("http://aliosthings.io/", tempBuffer, qrcode, qrcodegen_Ecc_LOW,
                                       qrcodegen_VERSION_MIN, 26, qrcodegen_Mask_AUTO, true);
        if (ok) {
            int size = qrcodegen_getSize(qrcode);
            int i, j;
            int rep = 0;
            while ((size < IMAGE_W - 20) && (size < IMAGE_H - 20)) {
                size <<= 1;
                rep++;
            }
            if (rep) {
                rep--;
            } else {
                LOG("QR Size is too big\n");
            }

            uint8_t *pbuf = buf;
            for (j = 0; j < IMAGE_H; j++) {
                for (i = 0; i < IMAGE_W; i++) {
                    if (i <= 10 || j <= 10) {
                        pbuf[i] = 0xff;
                    } else {
                        pbuf[i] = qrcodegen_getModule(qrcode, (i - 10) >> rep, (j - 10) >> rep) ? 0x0 : 0xff;
                    }
                }
                pbuf += IMAGE_W;
            }
            LOG("Disaply QR,size %d, rep %d\n", qrcodegen_getSize(qrcode), rep);
            //printQr(qrcode);
            camera_display_gray(buf, IMAGE_W * IMAGE_H);
            return;
        } else {
            LOG("QR Encoding error\n");
        }
    }


    aos_post_delayed_action(delay, capture_action, NULL);
}

static void captured()
{
    frameReady = 1;
    aos_post_event(EV_USER + 1, 0, 0);
}

static int handing_shake()
{
    static sys_time_t last_time = 0;
    sys_time_t now_time = 0;
    int ret = 0;

    now_time = krhino_sys_time_get();
    if (now_time - last_time < 500) {
        ret = 1;
    }
    last_time = now_time;

    return ret;
}

static void keyB_handle(void *arg)
{
    if (handing_shake()) {
        return;
    }
    keyB = !keyB;
    frameReady = 0;
    aos_post_event(EV_USER + 1, 0, 0);
}

int application_start(int argc, char *argv[])
{
    int ret;
    pQR = quirc_new();
    if (!pQR || quirc_resize(pQR, IMAGE_W, IMAGE_H) < 0 || !(buf = quirc_begin(pQR, 0, 0))) {
        LOG("No enough memory to initialize QR lib.\n");
        if (pQR) {
            quirc_destroy(pQR);
        }
    }

    LOG("QR Application started %p.", pQR);
    ret = hal_gpio_enable_irq(&brd_gpio_table[GPIO_KEY_3], IRQ_TRIGGER_RISING_EDGE, keyB_handle, NULL);
    if (ret != 0) {
        LOG("hal_gpio_enable_irq key return failed.\n");
    }

    aos_register_event_filter(EV_USER + 1, qr_decode_action, NULL);

    st7789_init();
    CameraHAL_Capture_Config(IMAGE_W, IMAGE_W);
    CameraHAL_Capture_Start(buf, IMAGE_W * IMAGE_H / 4, captured);

    aos_loop_run();

    if (pQR) {
        quirc_destroy(pQR);
    }
    return 0;
}

