/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __TRANS_ADAPT_V1_H__
#define __TRANS_ADAPT_V1_H__

#if defined(__cplusplus)
extern "C" {
#endif

void BESHCI_SendData(void *packet);
int BESHCI_SendBuffer(unsigned char packet_type, unsigned char *packet, int size);
unsigned short hci_h4_receive_msg( const uint8_t *buf, uint32_t size);

#if defined(__cplusplus)
}
#endif

#endif /* __TRANS_ADAPT_V1_H__ */
