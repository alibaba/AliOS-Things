/**
 *****************************************************************************
 * @file     sd_card.h
 * @author   Richard
 * @version  V1.0.0
 * @date     09-June-2013
 * @brief    sd card module driver interface
 * @maintainer lilu
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __CARD_H__
#define __CARD_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * SD CARD CID definition
 */
typedef struct _SD_CARD_ID
{
	uint8_t	MID;			/**< Manufacturer ID CID[127:120]*/
	uint8_t	OID[2];			/**< OEM/Application ID CID[119:104]*/
	uint8_t	PNM[5];			/**< Product name CID[103:64]*/
	uint8_t	PRV;			/**< Product revision CID[63:56]*/
	uint8_t	PSN[4];			/**< Product serial number CID[55:24]*/
	uint16_t	Rsv : 4;		/**< reserved CID[23:20]*/
	uint16_t	MDT : 12;		/**< Manufacturing date CID[19:8]*/
	uint8_t	CRC : 7;		/**< CRC7 checksum CID[7:1]*/
	uint8_t	NoUse : 1;		/**< not used, always 1  CID[0:0]*/

} SD_CARD_ID;

/**
 * err code definition
 */
typedef enum _SD_CARD_ERR_CODE
{
    CMD_SEND_TIME_OUT_ERR = -255,   /**<cmd send time out*/
    GET_RESPONSE_STATUS_ERR,        /**<get surrent transfer status err*/
    READ_SD_CARD_TIME_OUT_ERR,      /**<sd card read time out*/
    WRITE_SD_CARD_TIME_OUT_ERR,     /**<sd card write time out*/
    SD_CARD_IS_BUSY_TIME_OUT_ERR,   /**<sd card is busy time out*/
    NOCARD_LINK_ERR,                /**<sd card link err*/
    ACMD41_SEND_ERR,                /**<send ACMD41 err*/
    CMD1_SEND_ERR,                  /**<send CMD1 err*/
    CMD2_SEND_ERR,                  /**<get CID err*/
    ACMD6_SEND_ERR,                 /**<set bus witdh err*/
    CMD7_SEND_ERR,                  /**<select and deselect card err*/
    CMD9_SEND_ERR,                  /**<get CSD err*/
    CMD12_SEND_ERR,                 /**<stop cmd send err*/
    CMD13_SEND_ERR,                 /**<CMD13 read card status err*/
    CMD16_SEND_ERR,                 /**<set block length err*/
    CMD18_SEND_ERR,                 /**<CMD18 send err*/
    CMD25_SEND_ERR,                 /**<multi block write cmd send err*/
    CMD55_SEND_ERR,                 /**<send cmd55 err*/
    GET_SD_CARD_INFO_ERR,           /**<get sd card info err*/
    BLOCK_NUM_EXCEED_BOUNDARY,      /**<read block exceed boundary err*/
    NONE_ERR = 0,
} SD_CARD_ERR_CODE;

/**
 * @brief  Sd Controller Init
 * @param  NONE
 * @return NONE
 */
void SdControllerInit(void);

/**
 * @brief  identify sd card
 * @param  NONE
 * @return SD_CARD_ERR_CODE
 */
SD_CARD_ERR_CODE SdCardInit(void);


/**
 * @brief  detect sdio card.
 * @param  NONE
 * @return SD_CARD_ERR_CODE
 *         @arg  CMD13_SEND_ERR  read card status err
 *         @arg  NOCARD_LINK_ERR undetect card insert
 *         @arg  NONE_ERR
 */
SD_CARD_ERR_CODE SdCardDetect(void);

/**
 * @brief  get the capacity of the card, unit is block
 * @param  NONE
 * @return the card capacity
 */
uint32_t SdCardCapacityGet(void);

/**
 * @brief  read data from sd card
 * @param  Block specified block num
 * @param  Buffer save data into this buffer
 * @param  Size how many blocks will read
 * @return SD_CARD_ERR_CODE
 */
SD_CARD_ERR_CODE SdReadBlock(uint32_t Block, uint8_t* Buffer, uint8_t Size);

/**
 * @brief  write data into sd card
 * @param  Block specified block num
 * @param  Buffer read data from this buffer
 * @param  Size how many blocks will write
 * @return SD_CARD_ERR_CODE
 */
SD_CARD_ERR_CODE SdWriteBlock(uint32_t Block, const uint8_t* Buffer, uint8_t Size);



/**
 * @brief  start read sd card, each time read one block
 * @param  Block specified block num
 * @param  Buffer save data into this buffer
 * @return SD_CARD_ERR_CODE
 */
SD_CARD_ERR_CODE SdCardReadBlockStart(uint32_t block, uint8_t* buffer);

/**
 * @brief  start read next block, before read must wait last block transfer finished
 * @param  Buffer save data into this buffer
 * @return SD_CARD_ERR_CODE
 */
SD_CARD_ERR_CODE SdCardReadBlockNext(uint8_t* buffer);

/**
 * @brief  end read sd card
 * @param  NONE
 * @return SD_CARD_ERR_CODE
 */
SD_CARD_ERR_CODE SdCardReadBlockEnd(void);

/**
 * @brief  start write block
 * @param  block  Block specified block num
 * @return SD_CARD_ERR_CODE
 */
SD_CARD_ERR_CODE SdCardWriteBlockStart(uint32_t Block);

/**
 * @brief  ready to write next block, before write function must wait last block write finished
 * @param  Buffer read data from this buffer
 * @return SD_CARD_ERR_CODE
 */
SD_CARD_ERR_CODE SdCardWriteBlockNext(uint8_t* Buffer);

/**
 * @brief  end write sd card
 * @param  NONE
 * @return SD_CARD_ERR_CODE
 */
SD_CARD_ERR_CODE SdCardWriteBlockEnd(void);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
