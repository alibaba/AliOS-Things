#ifndef _ALC5679_H_
#define _ALC5679_H_

#define RT5679_DEVICE_ID			0x6385
#define RT5679_VENDOR_ID2			0x00ff

/////////////////////////////////////////////////////
void rt5679_i2c_init(void);
int check_id();
void rt5679_linein_to_i2s(void);
void rt5679_i2s_to_hp(void);
void rt5679_mic_to_i2s(void);
#endif
