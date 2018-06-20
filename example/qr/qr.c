/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include "quirc.h"
#include "camera.h"

static uint8_t buf[240*240*2];
static void captured();
static int frameReady = 0;
static struct quirc * pQR = 0;
static struct quirc_code qr_code;
static struct quirc_data qr_data;

#define RED(a)      ((((uint16_t)(a) & 0xf800) >> 11) << 3)
#define GREEN(a)    ((((uint16_t)(a) & 0x07e0) >> 5) << 2)
#define BLUE(a)     (((uint16_t)(a) & 0x001f) << 3)
#define RGB565_TO_GRAY(a) (uint8_t)((RED(a)+GREEN(a)+GREEN(a)+BLUE(a))>>2)
#define GRAY_TO_RGB565(g) ((((uint16_t)(g)>>3)<<11)| (((uint16_t)(g)>>2)<<5) | (((uint16_t)(g)>>3)))
#define RGB565_ENDIAN(h) (((uint16_t)(h)>>8)|((uint16_t)(h)&0xff)<<8)

static void capture_action(void *arg)
{
    CameraHAL_Capture_Start(buf, sizeof(buf)/4, captured);
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

			if (code->cell_bitmap[p >> 3] & (1 << (p & 7)))
				printf("[]");
			else
				printf("  ");
		}
		printf("\n");
	}
}

static void qr_decode_action(input_event_t *event, void *arg)
{
    //if(event->code != EV_USER+1) return;
    int delay = 20;
    if(frameReady){

       camera_dispaly(buf, 240*240);
        
        if(pQR){
            uint8_t *image;
            int w = 0, h = 0, i, j;

            image = quirc_begin(pQR, &w, &h);
            //LOG("Prepare QR scan on %p %dx%d\n", image, w, h);

            //TODO: we'd better let camera_hal capture lum data directly.
            for(j=0; j<h; j++){
                for(i=0; i<w; i++){
                    uint8_t gray = RGB565_TO_GRAY(RGB565_ENDIAN(*((uint16_t *)(buf)+j*w+i)));
                    image[j*w+239-i] = gray;//(gray>0x80)?0xff:0;
                    //*((uint16_t *)(buf)+j*w+i) = RGB565_ENDIAN(GRAY_TO_RGB565(gray));
                }
            }

            //LOG("Start scan\n");
            quirc_end(pQR);

            int num_codes = quirc_count(pQR);
            //LOG("Scan QR count %d\n", num_codes);

            if(num_codes){
                quirc_decode_error_t err;

                //LOG("QR Code Extracting\n");
                quirc_extract(pQR, 0, &qr_code);

                dump_cells(&qr_code);

                /* Decoding stage */
                //LOG("QR Code Decoding\n");
                err = quirc_decode(&qr_code, &qr_data);
    
                if (err)
                    LOG("DECODE FAILED: %s\n", quirc_strerror(err));
                else
                {
                    LOG("Data: %s\n", qr_data.payload);
                }
                //dump_image();
                
            }
        }

        frameReady = 0;
        
              
    }


    aos_post_delayed_action(delay, capture_action, NULL);
}

static void captured()
{
    frameReady = 1;
    aos_post_event(EV_USER+1, 0, 0);
}

int application_start(int argc, char *argv[])
{
    pQR = quirc_new();
    if(!pQR || quirc_resize(pQR, 240, 240) < 0){
        LOG("No enough memory to initialize QR lib.\n");
        if(pQR) quirc_destroy(pQR);
    }
    
    LOG("QR Application started %p.", pQR);

    aos_register_event_filter(EV_USER+1, qr_decode_action, NULL);
    
    st7789_init();
    CameraHAL_Capture_Config(240, 240);
    CameraHAL_Capture_Start(buf, sizeof(buf)/4, captured);

    aos_loop_run();

    if(pQR) quirc_destroy(pQR);
    return 0;
}

