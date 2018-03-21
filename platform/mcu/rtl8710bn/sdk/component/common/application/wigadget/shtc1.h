#ifndef SHTC1_H
#define SHTC1_H

#define NO_ERROR        0x00
#define ACK_ERROR       0x01
#define CHECKSUM_ERROR  0x02
#define NULL_ERROR      0x03

int SHTC_GetTempAndHumi(float *temp, float *humi);
int SHTC_Init(uint16_t *pID);
void example_shtc1(void);

#endif
