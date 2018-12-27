/**
 *****************************************************************************
 * @file     sdio.h
 * @author   Owen
 * @version  V1.0.1
 * @date     25-Feb-2016
 * @brief    sdio controller driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

/**
* @addtogroup Çý¶¯
* @{
* @defgroup SDIO SDIO
* @{
*/

#ifndef __SDIO_H__
#define __SDIO_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define NO_ERR                  (0)
#define SEND_CMD_TIME_OUT_ERR   (1)


/**
 * @brief  enable sdio controller clk
 * @param  NONE
 * @return NONE
 * @note
 */
void SdioEnableClk(void);

/**
 * @brief  disable sdio controller clk
 * @param  NONE
 * @return NONE
 * @note
 */
void SdioDisableClk(void);

/**
 * @brief  div sdio controller clk, four bits,0 is 2 div,max div val is 0xB(4096 div)
 * @param  ClkIndex [2,4096]
 * @return NONE
 * @note
 */
void SdioSetClk(uint32_t ClkIndex);

/**
 * @brief  data transfer end
 * @param  NONE
 * @return NONE
 * @note
 */
void SdioEndDatTrans(void);

/**
 * @brief  check data transfer done
 * @param  NONE
 * @return bool 1 means done
 * @note
 */
bool SdioIsDatTransDone(void);

/**
 * @brief  get sdio controller current transfer status
 * @param  NONE
 * @return sdio card current transfer status
 * @note
 */
uint8_t SdioGetStatus(void);

/**
 * @brief  get sdio controller  response status
 * @param  NONE
 * @return current sd card status
 * @note
 */
uint8_t SdioGetResponseStatus(void);

/**
 * @brief  check sdio controller is busy
 * @param  NONE
 * @return 0 is busy, 1 is idle
 * @note
 */
bool SdioIsBusy(void);

/**
 * @brief  Start receive data, after receiving, sdio controller auto stops the clock.
 * @param  InBuf save data in this buffer
 * @param  DataLen recive data length,max not exceed 512 BYTES
 * @return NONE
 * @note
 */
void SdioStartReciveData(uint8_t* InBuf, uint16_t DataLen);

/**
 * @brief  Start send data
 * @param  OutBuf send data from this buffer
 * @param  DataLen send data length,max not exceed 512 BYTES
 * @return NONE
 * @note
 */
void SdioStartSendData(uint8_t* OutBuf, uint16_t DataLen);

/**
 * @brief  Sdio Controller Init
 * @param  NONE
 * @return NONE
 * @note
 */
void SdioControllerInit(void);

/**
 * @brief  Send basic command.
 * @param  Cmd the command index will send
 * @param  Param the argument of this command
 * @param  TimeOut send cmd time out
 * @return SEND_CMD_TIME_OUT_ERR-send cmd time out,but maybe this cmd has no response,it's also return FALSE
 * @note
 */
bool SdioSendCommand(uint8_t Cmd, uint32_t Param, uint16_t TimeOut);

/**
 * @brief  enable sdio controller data transfer end interrupt
 * @param  NONE
 * @return NONE
 * @note
 */
void SdioDataInterruptEn(void);

/**
 * @brief  disable sdio controller data transfer end interrupt
 * @param  NONE
 * @return NONE
 * @note
 */
void SdioDataInterruptDis(void);

/**
 * @brief  clear sdio controller data transfer end interrupt
 * @param  NONE
 * @return NONE
 * @note
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

/**
 * @}
 * @}
 */
