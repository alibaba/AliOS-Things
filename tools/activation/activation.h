/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ACTIVATION_H
#define ACTIVATION_H

#ifndef UINT32
#define UINT32 unsigned int
#endif

#ifndef UINT8
#define UINT8 unsigned char
#endif

#define VERSION_NUM_SIZE    4
#define RANDOM_NUM_SIZE     4
#define MAC_ADDRESS_SIZE    8
#define CHIP_CODE_SIZE      4
#define INPUT_SIZE          (VERSION_NUM_SIZE+RANDOM_NUM_SIZE+MAC_ADDRESS_SIZE+CHIP_CODE_SIZE)
#define OUTPUT_SPACE_SIZE   (INPUT_SIZE*2*2+1)


/*
	input: version 4byte + random 4 byte + mac 4byte + chip_code 4byte
	output: output_buffer store the version info process. length at least OUTPUT_SPACE_SIZE
	return: 0 success, 1 failed	
*/
extern UINT32 aos_get_version_info (UINT8 version_num[VERSION_NUM_SIZE], UINT8 random_num[RANDOM_NUM_SIZE], UINT8 mac_address[MAC_ADDRESS_SIZE], UINT8 chip_code[CHIP_CODE_SIZE], UINT8 *output_buffer, UINT32 output_buffer_size);

#endif

