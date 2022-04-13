#ifndef __COMMUNICATION_CMD_MSG_H__
#define __COMMUNICATION_CMD_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

enum COMMUNICATION_CMD_TYPE {
    COMMUNICATION_CMD_EQ_OP = 0xA1,
    COMMUNICATION_CMD_DRC_OP  = 0xA2,
    COMMUNICATION_CMD_HF_OP  = 0xA3,
};


#ifdef __cplusplus
}
#endif

#endif

