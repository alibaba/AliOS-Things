#ifndef JOYLINK2_AES_H
#define JOYLINK2_AES_H 

#ifdef __cplusplus
extern "C"
{
#endif

#define UINT8 unsigned char
#define UINT32 unsigned int

enum
{
    JOYLINK_SUCCESS                 = 0,
    JOYLINK_BUFFER_SPACE_ERR        = -100,             /*buffer不足*/
    JOYLINK_RECV_LEN_ERR            = -101,             /*接收数据长度有误*/
    JOYLINK_CHECKSUM_ERR            = -102,             /*数据校验失败*/
    JOYLINK_GET_CMD_ERR             = -103,             /*接收命令类型有误*/
    JOYLINK_GET_DEVID_ERR           = -104,             /*设备ID不一致*/
    JOYLINK_DEVID_ERR               = -105,             /*设备ID有误*/
    JOYLINK_TOKEN_ERR               = -106,             /*设备TOKEN有误*/
    JOYLINK_ENCTYPE_ERR             = -107,             /*不支持的加密策略*/
    JOYLINK_MAGIC_ERR               = -108,             /*无效数据包,magic有误*/
    JOYLINK_ENCINFO_ERR             = -109,             /*加密信息有误*/
    
    
    JOYLINK_PARAM_INVALID           = -1000,            /*参数数据有误*/
    JOYLINK_SYSTEM_ERR              = -1001,            /*系统调用错误,例如创建socket失败*/
    JOYLINK_NETWORK_TIMEOUT         = -1002,            /*网络超时*/
    JOYLINK_RECV_DATA_ERR           = -1003,             /*接收到的数据有误*/
    JOYLINK_CANCEL_ERR              = -1004,            /*用户取消操作*/
};

// ARC4 crypt, 若有用户自己实现, 则宏定义为1
#define HARDWARE_ARC4   0
extern int joylinkEnc1Crypt(UINT8 *key, UINT32 keyLen, UINT8 *data, UINT32 len);

#define JOYLINK_ENC2_ENCRYPT     1
#define JOYLINK_ENC2_DECRYPT     0

//#define JOYLINK_ERR_ENC2_INVALID_KEY_LENGTH                -0x0020  /**< Invalid key length. */
//#define JOYLINK_ERR_ENC2_INVALID_INPUT_LENGTH              -0x0022  /**< Invalid data input length. */

//AES crypt, 若用户自己实现，则宏定义为1
#define HARDWARE_AES    0
extern int joylinkEnc2Crypt(UINT8 *key, UINT32 keyLen, UINT8 *iv, UINT8 *data, UINT32 *len, UINT32 maxLen, int isPKCS5, int type);

//MD5 crypt, 若用户自己实现，则宏定义为1
#define HARDWARE_MD5    1
extern void joylinkENC3(UINT8 *input, UINT32 inputlen, UINT8 output[16]);


#define JOYLINK_ENC4_KEY_BIT     1024
#define JOYLINK_ENC4_ENCRYPT     1
#define JOYLINK_ENC4_DECRYPT     0
//RSA crypt, 需要由用户自己实现,
#define HARDWARE_RSA    1
extern int joylinkEnc4Crypt(UINT8 *input, UINT32 inputLen, UINT8 *output, UINT32 maxLen, UINT8 *key, int type);

#ifdef __cplusplus
}
#endif
    
#endif
