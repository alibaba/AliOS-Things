#ifndef _G711_CODEC_H
#define _G711_CODEC_H
#include "basic_types.h"

typedef enum {
	I2S_MODE_UNKNOWN = -1,
	I2S_MODE_G711A = 1,
	I2S_MODE_G711U = 2 
}I2S_mode;

typedef struct {
	I2S_mode mode;	//intend to encode as G711A or G711U
	int len;	//data length
//	_sema RWSema;
	unsigned char *raw_data;	//address of buffered data (not encoded
	int data_start; // a shift value to show where the data starts
	int data_end;	// a shift value shows data ending
}i2s_buf_context;

//void encoder_init(int mode);
int G711_encoder(short *source, unsigned char *des, int mode, int size);
int G711_decoder(unsigned char *source, short *des, int mode, int size);
int mono2stereo(unsigned char *src, unsigned char *dst, int size, int wl);
#endif