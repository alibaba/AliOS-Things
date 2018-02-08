#ifndef __EXAMPLE_GET_BEACON_FRAME_H__
#define __EXAMPLE_GET_BEACON_FRAME_H__

/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/

typedef struct beacon_info_str{
	//802.11 MAC header
	unsigned char frame_ctl[2];
	unsigned char duration_id[2];
	unsigned char addr1[6];
	unsigned char addr2[6];
	unsigned char addr3[6];
	unsigned char seq_ctl[2];
	//802.11 beacon IE
	unsigned char timestamp[8];
}BEACON_INFO_T;

void example_get_beacon_frame(void);
int get_beacon_frame_func(BEACON_INFO_T beacon_frame);

#endif //#ifndef __EXAMPLE_GET_BEACON_FRAME_H__
