#ifndef _SECURITY_H
#define _SECURITY_H

#ifdef __LINUX_UB2__
#include <stdint.h>
#endif

#include "joylink.h"
#include "joylink_json.h"

/**
 * brief: 
 *
 * @Param: pParam
 * @Param: pIn
 * @Param: length
 * @Param: pOut
 * @Param: maxlen
 *
 * @Returns: 
 */
int 
joylink_dencypt_lan_req(JLPacketParam_t *pParam, 
                        const uint8_t *pIn, 
                        int length, 
                        uint8_t* pOut, 
                        int maxlen);

/**
 * brief: 
 *
 * @Param: pBuf
 * @Param: buflen
 * @Param: enctype
 * @Param: cmd
 * @Param: key
 * @Param: payload
 * @Param: length
 *
 * @Returns: 
 */
int 
joylink_encrypt_lan_basic(uint8_t* pBuf, 
                          int buflen, 
                          E_EncType enctype, 
                          E_PacketType cmd, 
                          uint8_t* key, 
                          const uint8_t* payload, 
                          int length);

/**
 * brief: 
 *
 * @Param: pBuf
 * @Param: buflen
 * @Param: cmd
 * @Param: key
 * @Param: payload
 * @Param: payloadLen
 *
 * @Returns: 
 */
int 
joylink_encypt_server_rsp(uint8_t* pBuf, 
                          int buflen, 
                          E_PacketType cmd, 
                          uint8_t* key, 
                          const uint8_t* payload, 
                          int payloadLen);

/**
 * brief: 
 *
 * @Param: pParam
 * @Param: pIn
 * @Param: length
 * @Param: pOut
 * @Param: maxlen
 *
 * @Returns: 
 */
int 
joylink_dencypt_server_req(JLPacketParam_t *pParam, 
                           const uint8_t *pIn, 
                           int length, 
                           uint8_t* pOut, 
                           int maxlen);

/**
 * brief: 
 *
 * @Param: pBuf
 * @Param: buflen
 * @Param: enctype
 * @Param: key
 * @Param: payload
 * @Param: length
 *
 * @Returns: 
 */
int 
joylink_encrypt_sub_dev_data(uint8_t* pBuf, 
                             int buflen, 
                             E_EncType enctype, 
                             uint8_t* key, 
                             const uint8_t* payload, 
                             int length);

/**
 * brief: 
 *
 * @Param: p_in
 * @Param: in_len
 * @Param: key
 * @Param: key_type
 * @Param: p_out
 * @Param: maxlen
 *
 * @Returns: 
 */
int 
joylink_dencrypt_sub_dev_data(const uint8_t *p_in, 
                              int in_len, 
                              const uint8_t *key, 
                              int key_type,
                              uint8_t* p_out, 
                              int maxlen);
#endif
