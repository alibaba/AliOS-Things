/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#ifndef __SWM320_DMA_H__
#define __SWM320_DMA_H__


#define DMA_CH0     0
#define DMA_CH1     1
#define DMA_CH2     2


void DMA_CHM_Config(uint32_t chn, uint32_t src_addr, uint32_t src_addr_incr, uint32_t dst_addr, uint32_t dst_addr_incr, uint32_t num_word, uint32_t int_en);    //DMA通道配置，用于存储器间（如Flash和RAM间）搬运数据
void DMA_CH_Open(uint32_t chn);
void DMA_CH_Close(uint32_t chn);
void DMA_CH_INTEn(uint32_t chn);
void DMA_CH_INTDis(uint32_t chn);
void DMA_CH_INTClr(uint32_t chn);
uint32_t DMA_CH_INTStat(uint32_t chn);


#endif //__SWM320_DMA_H__
