#ifndef __APP_FACTORY_CDC_COMM__H__
#define __APP_FACTORY_CDC_COMM__H__
#ifdef __USB_COMM__
#ifdef __cplusplus
extern "C" {
#endif

void comm_loop(void);

int send_reply(const unsigned char *payload, unsigned int len);

#ifdef __cplusplus
}
#endif
#endif
#endif

