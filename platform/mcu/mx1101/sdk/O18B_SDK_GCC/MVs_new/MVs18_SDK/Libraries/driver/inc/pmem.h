/**
 *****************************************************************************
 * @file     pmem.c
 * @author   Ingrid
 * @version  V1.0.0
 * @date     03-June-2013
  *@maintainer Halley
 * @brief    PMEM module driver interface
 *****************************************************************************
 * @attention
 *
 * This module support clk driver, all the other module need to consider how to
 * use these function
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */


#ifndef __PMEM_H__
#define __PMEM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

/**<PATH SEL*/
#define PMEM_ADC_MIX_PATH           (1 << 0)       /**<adc and phub module switch*/
#define PMEM_SPI_PATH               (1 << 2)       /**<SPIS module switch*/
#define PMEM_UART_PATH              (1 << 3)       /**<buart module switch*/
#define PMEM_I2S_PATH               (1 << 4)       /**<I2S module switch*/

/**<ERR CODE DEFINE*/
#define PMEM_SPIS_BUART_ERR         (-1)           /**<spis and buart can't work in the same time*/

/**<PMEM SPACE allocation*/
#define PMEM_OFIFO_START_ADDR       (0x0000)       /**<start address of PMEM for output fifo */
#define PMEM_OFIFO_MAX_LENGTH       (0x8000)       /**<depth of PMEM for output fifo */
#define DEF_PCM_FIFO_LENGTH         (0x3000)       /**<depth of PMEM for output fifo */
#define BT_FIFO_MAX_LENGTH          (0x1C00)       /**<depth of PMEM for output fifo */



/**
 * @brief  there are five master can write data into pmem, ADC,phub,SPIS,UART,I2S
 *         but adc and phub, spis and buart can't work in the same time
 * @param  Mode master read/write pmem path sel
 * @return err code
 *         @arg PMEM_SPIS_BUART_ERR  spis and buart set at the same time
 *         >0 is current val in reg
 */
uint32_t PmemDataPathSel(uint8_t Mode);

/**
 * @brief  close the path for each master r/w pmem
 * @param  Mode master read/write pmem path sel
 * @return NONE
 */
void PmemDataPathClose(uint8_t Mode);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
