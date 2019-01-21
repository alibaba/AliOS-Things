/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __ABASE64_H__
#define __ABASE64_H__

char *ABase64_Encode(uint8_t *in, int in_len, char *out, int out_len);
int ABase64_Decode(char *in, uint8_t *out, int out_len);

int ABase64_DecodeLen(char *encoded);
int ABase64_EncodeLen(int decode_len);

#endif
