/**
 **************************************************************************************
 * @file    dlut.h
 *
 * @author  Aissen Li
 * @version V1.0.0
 * @date    2012-07-17
 *
 * @brief   PCM FIFO Control Functions API
 **************************************************************************************
 *
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __DLUT_H__
#define __DLUT_H__

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * @brief  Set MP3 sfbwidth tables status
 * @param  Status 0:DISABLE, 1:ENABLE
 * @return NONE
 */
void DlutSetMP3SfbwidthTableStatus(bool Status);

/**
 * @brief  Set MP3 huffman tables status
 * @param  Status 0:DISABLE, 1:ENABLE
 * @return NONE
 */
void DlutSetMP3HuffmanTableStatus(bool Status);

/**
 * @brief  Set WMA huffman tables status
 * @param  Status 0:DISABLE, 1:ENABLE
 * @return NONE
 */
void DlutSetWMAHuffmanTableStatus(bool Status);

/**
 * @brief  Set COS table status
 * @param  Status 0:DISABLE, 1:ENABLE
 * @return NONE
 */
void DlutSetCosTableStatus(bool Status);

/**
 * @brief  Lookup MP3 sfbwidth tables
 * @param  Sfbi             scale factor band index
 * @param  BlockType        block type
 * @param  SampleRate       sample rate
 * @param  MixedBlockFlag   mixed block flag
 * @return MP3 sfbwidth table entry value
 */
uint8_t DlutLookupMP3SfbwidthTable(uint8_t Sfbi, uint8_t BlockType, uint16_t SampleRate, uint8_t MixedBlockFlag);

/**
 * @brief  Lookup MP3 huffman tables
 * @param  BigValueSel   big value table select signal
 * @param  Count1TabSel  count1 table select signal
 * @param  TabIdx        index of looktable,max value is 401 in big value table16
 * @param  TabType       0,means use big value table, 1 means use count1 tab
 * @return MP3 huffman entry value
 */
uint32_t DlutLookupMP3HuffmanTable(uint8_t BigValueSel, uint8_t Count1TabSel, uint16_t TabIdx, uint8_t TabType);

/**
 * @brief  Lookup WMA huffman tables
 * @param  TabIdx table entry index
 * @param  TabSel selected table index
 * @return WMA huffman entry value
 */
uint32_t DlutLookupWMAHuffmanTable(uint16_t TabIdx, uint8_t TabSel);

/**
 * @brief  Lookup COS(x) tables
 * @param  Index must be 0~1024
 * @return COS(Index), data format is Q2.14
 */
uint32_t DlutLookupCosTable(uint32_t Index);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//__DLUT_H__
