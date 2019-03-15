/**
 *****************************************************************************
 * @file     sdio.h
 * @author   Richard
 * @version  V1.0.0
 * @date     27-June-2013
 * @maintainer lilu
 * @brief    declare sdio controller driver interface
 * @note     For sdio and spi can't access memory simultaneously
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __SDIO_H__
#define __SDIO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define NO_ERR                  (0)
#define SEND_CMD_TIME_OUT_ERR   (1)


/**
 * @brief  enable sd card clk,output 0~24M
 * @param  NONE
 * @return NONE
 */
void SdioEnableClk(void);

/**
 * @brief  disable sd card clk,output 1
 * @param  NONE
 * @return NONE
 */
void SdioDisableClk(void);

/**
 * @brief  div sd card clk, four bits,0 is 2 div,max div val is 0xB(4096 div)
 * @param  ClkIndex [2,4096]
 * @return NONE
 */
void SdioSetClk(uint32_t ClkIndex);

/**
 * @brief  data transfer end
 * @param  NONE
 * @return NONE
 */
void SdioEndDatTrans(void);

/**
 * @brief  check data transfer done
 * @param  NONE
 * @return bool 1 means done
 */
bool SdioIsDatTransDone(void);

/**
 * @brief  get sdio card current transfer status
 * @param  NONE
 * @return sdio card current transfer status
 */
uint8_t SdioGetStatus(void);

/**
 * @brief  get sdio card  response status
 * @param  NONE
 * @return current sd card status
 */
uint8_t SdioGetResponseStatus(void);

/**
 * @brief  check card is busy
 * @param  NONE
 * @return 0 is busy, 1 is idle
 */
bool SdioIsBusy(void);

/**
 * @brief  Start receive data, after receiving, controller auto stops the clock.
 * @param  InBuf save data in this buffer
 * @param  DataLen recive data length,max not exceed 512 BYTES
 * @return NONE
 */
void SdioStartReciveData(uint8_t* InBuf, uint16_t DataLen);

/**
 * @brief  Start send data
 * @param  OutBuf send data from this buffer
 * @param  DataLen send data length,max not exceed 512 BYTES
 * @return NONE
 */
void SdioStartSendData(uint8_t* OutBuf, uint16_t DataLen);

/**
 * @brief  Sdio Controller Init
 * @param  NONE
 * @return NONE
 */
void SdioControllerInit(void);

/**
 * @brief  Send basic command.
 * @param  Cmd the command index will send
 * @param  Param the argument of this command
 * @param  TimeOut send cmd time out
 * @return SEND_CMD_TIME_OUT_ERR-send cmd time out,but maybe this cmd has no response,it's also return FALSE
 */
bool SdioSendCommand(uint8_t Cmd, uint32_t Param, uint16_t TimeOut);

/**
 * @brief  enable sd int, it includes and data int
 * @param  NONE
 * @return NONE
 */
void SdioDataInterruptEn(void);

/**
 * @brief  disable sd int, it includes data int
 * @param  NONE
 * @return NONE
 */
void SdioDataInterruptDis(void);

/**
 * @brief  clear sd int, it includes data int
 * @param  NONE
 * @return NONE
 */
void SdioDataInterruptClear(void);

/**
 * @brief  get command response
 * @param  RespBuf save the response
 * @param  RespBufLen the length of the response,
 * @return NONE
 * @note   totally have 8 response type, all response code length are 48bits except R2(136bits)
 *         so generally RespBufLen can set 5 or 16,more detail please see specification
 */
void SdioGetCmdResp(uint8_t *RespBuf, uint8_t RespBufLen);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__SDIO_H__
