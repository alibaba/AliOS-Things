/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_FORMAT_H__
#define __UVOICE_FORMAT_H__


int mp3_id3v1_parse(media_info_t *info, unsigned char *data, int size);
int format_parse_byname(char *name, media_format_t *format);
bool wav_format_check(char *buffer, int size);
bool mp3_format_check(char *data, int size);
bool aac_format_check(char *buffer, int size);
bool m4a_format_check(char *buffer, int size);
bool ogg_format_check(char *data, int size);
bool wma_format_check(char *buffer, int size);
bool amr_format_check(char *buffer, int size);
bool amrwb_format_check(char *data, int size);
bool flac_format_check(char *buffer, int size);


#endif /* __UVOICE_FORMAT_H__ */
