#ifndef COMMUNICATION_CMD_HANDLE_H
#define COMMUNICATION_CMD_HANDLE_H

#include "tool_msg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define COMMUNICATION_CMD_BUFF_TO_U32(ptr,val) do{ \
                                val =( ((uint32_t) *((uint8_t*)ptr+3) << 24)   | \
                                      ((uint32_t) *((uint8_t*)ptr+2) << 16) | \
                                      ((uint32_t) *((uint8_t*)ptr+1) << 8)  | \
                                      ((uint32_t) *((uint8_t*)ptr)) ); \
                         }while(0)

#define COMMUNICATION_CMD_U32_TO_BUFF(ptr,val) do{ \
                            *(ptr+3) = (uint8_t) (val>>24); \
                            *(ptr+2) = (uint8_t) (val>>16); \
                            *(ptr+1) = (uint8_t) (val>>8); \
                            *(ptr+0) = (uint8_t) val; \
                         }while(0)

int communication_cmd_init(int (* cb)(const unsigned char *, unsigned int));

int communication_cmd_send_reply(const unsigned char *payload, unsigned int len);

enum ERR_CODE communication_cmd_check_msg_hdr(struct message_t *msg);

enum ERR_CODE communication_cmd_handle_cmd(enum COMMUNICATION_CMD_TYPE cmd, unsigned char *param, unsigned int len);


#ifdef __cplusplus
}
#endif
#endif
