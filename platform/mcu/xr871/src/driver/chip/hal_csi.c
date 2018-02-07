/**
  * @file  hal_csi.c
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "driver/chip/hal_csi.h"
#include "driver/chip/hal_gpio.h"
#include "driver/chip/hal_dma.h"
#include "hal_base.h"

#define CSI_DEBUG 1

#define HAL_CSI_LOG(flags, fmt, arg...)	\
	do {								\
		if (flags) 						\
			printf(fmt, ##arg);		\
	} while (0)


#define HAL_CSI_DBG(fmt, arg...)	\
			HAL_CSI_LOG(CSI_DEBUG, "[HAL_CSI] "fmt, ##arg)


uint8_t csi_is_run = 0;

void CSI_ModuleEnable()
{
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_CSI);
	HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_CSI);
	HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_CSI);
	HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_CSI);
}

void CSI_ModuleDisable()
{
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_CSI);
}

void CSI_InputFormat() //raw
{
	HAL_CLR_BIT(CSI->CSI_CFG_REG , CSI_CFG_INPUT_FORMAT);
}

void CSI_Irq_Enable()
{
	HAL_NVIC_SetPriority(CSI_IRQn, 0);
	HAL_NVIC_EnableIRQ(CSI_IRQn);
}

void CSI_Irq_Disable()
{
	HAL_NVIC_DisableIRQ(CSI_IRQn);
}

CSI_Call_Back private_csi_cb;


void CSI_IRQHandler()
{
	if(private_csi_cb.callBack != NULL)
		private_csi_cb.callBack(private_csi_cb.arg);
}

/**
  *@private
  *@brief Initializes the CSI peripheral according to the specified parameters
  * in the CSI_Config *param.
  * @note This function is used to configure the source clock and pins of the CSI.
  *           The source clock freq is the mclk output freq.
  * @param param:
  *        @arg param->src_Clk.clk used for select the source clock of CSI
  *        @arg param->src_Clk.divN  *  param->src_Clk.divM = The clock division
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_CSI_Config(CSI_Config *param)
{
	if (csi_is_run) {
		HAL_WRN("%s, %d csi is busy\n", __func__, __LINE__);
		return HAL_BUSY;
	}
	CSI_ModuleEnable();
	HAL_CCM_CSI_SetMClock(param->src_Clk.clk, param->src_Clk.divN, param->src_Clk.divM);
	HAL_CCM_CSI_EnableMClock();

	HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_CSI, 0), 0);
	csi_is_run = 1;
	CSI_InputFormat();
	return HAL_OK;
}

/**
  * @brief Deinitializes all CSI peripherals registers and pins to their default reset
  *           values.
  * @param None
  * @retval None
  */
void HAL_CSI_DeInit(void)
{
	HAL_CLR_BIT(CSI->CSI_EN_REG, CSI_EN);
	HAL_CCM_CSI_DisableMClock();
	CSI_ModuleDisable();
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_CSI, 0), 0);
	csi_is_run = 0;
}

/**
  * @brief Enables or disables the CSI interface.
  * @param ctrl: Controls the enable of CSI moudle.
  *         @arg This parameter can be: CSI_ENABLE or CSI_DISABLE.
  * @retval None
  */
void HAL_CSI_Moudle_Enalbe(CSI_CTRL ctrl)
{
	if (CSI_ENABLE == ctrl)
		HAL_SET_BIT(CSI->CSI_EN_REG, CSI_EN);
	else
		HAL_CLR_BIT(CSI->CSI_EN_REG, CSI_EN);
}

/**
  * @brief Configure the CSI sync signal polarity.
  * @param signal: Set the polarity for vsync, herf,p_Clk.
  * @retval None
  */
void HAL_CSI_Sync_Signal_Polarity_Cfg(CSI_Sync_Signal *signal)
{
	uint32_t csi_sync_pol;
	csi_sync_pol = signal->vsync + (signal->herf << 1) + (signal->p_Clk << 2);
	HAL_CLR_BIT(CSI->CSI_CFG_REG, CSI_CFG_SYNC_SIGNAL_POL);
	HAL_SET_BIT(CSI->CSI_CFG_REG, csi_sync_pol);
}

/**
  * @brief Enables or disables the CSI capture data.
  * @param mode: Select the capture mode.
  *        @arg CSI_STILL_MODE: capture one picture.
  *        @arg CSI_VIDEO_MODE:Continuous capture.
  * @param  ctrl: Controls the enable of CSI capture.
  *        @arg This parameter can be: CSI_ENABLE or CSI_DISABLE.
  * @retval None
  */
void HAL_CSI_Capture_Enable(CSI_CAPTURE_MODE mode , CSI_CTRL ctrl)
{
	HAL_CLR_BIT(CSI->CSI_CAP_REG, CSI_CAP_MODE);

	if (CSI_ENABLE == ctrl) {
		if (mode == CSI_STILL_MODE) {
			HAL_SET_BIT(CSI->CSI_CAP_REG, HAL_BIT(0));
		} else if (mode == CSI_VIDEO_MODE)
			HAL_SET_BIT(CSI->CSI_CAP_REG, HAL_BIT(1));
	}
}

/**
  * @brief Configure the CSI interval capture.
  * @note This function is used to Configure interlaced acquisition and interlaced pixels.
  * @param vermask:  the interlaced pixels value.
  * @param hor_mask: the interlaced acquisition.
  * @retval None
  */
void HAL_CSI_Interval_Capture_Cfg(uint8_t ver_mask, uint16_t hor_mask)
{
	HAL_CLR_BIT(CSI->CSI_SCALE_REG, CSI_VER_MASK);
	HAL_CLR_BIT(CSI->CSI_SCALE_REG, CSI_HER_MASK);

	HAL_SET_BIT(CSI->CSI_SCALE_REG, ver_mask << 24);
	HAL_SET_BIT(CSI->CSI_SCALE_REG, hor_mask);
}

/**
  * @brief Select the next FIFO for cache data.
  * @param fifo_num:
  *        @arg CSI_FIFO_0_A:  Select CSI_FIFO_0_A.
  *        @arg CSI_FIFO_0_B:  Select CSI_FIFO_0_B.
  * @retval None
  */
void HAL_CSI_Selection_Next_FIFO (CSI_FIFO fifo_num)
{
	HAL_CLR_BIT(CSI->CSI_BUF_CTL_REG ,HAL_BIT(2));
	HAL_SET_BIT(CSI->CSI_BUF_CTL_REG , fifo_num << 2);
}

/**
  * @brief Gets the currently used FIFO.
  * @param None.
  * @retval CSI_FIFO: The FIFO that currently used.
  */
CSI_FIFO HAL_CSI_Current_FIFO()
{
	return  (HAL_GET_BIT(CSI->CSI_BUF_CTL_REG, HAL_BIT(1)) >> 1);
}

/**
  * @brief Enables or disables used double FIFO for cache the data.
  * @note If you disable the double FIFO mode, the CSI_FIFO_A will be always
  *           used  for cache data.
  * @param ctrl: Controls the enable of double FIFO mode.
  *         @arg This parameter can be: CSI_ENABLE or CSI_DISABLE.
  * @retval None.
  */
void HAL_CSI_Double_FIFO_Mode_Enable(CSI_CTRL ctrl)
{
	HAL_CLR_BIT(CSI->CSI_BUF_CTL_REG, HAL_BIT(0));

	if (CSI_ENABLE == ctrl)
		HAL_SET_BIT(CSI->CSI_BUF_CTL_REG, HAL_BIT(0));
}

/**
  * @brief Gets the CSI moudle status.
  * @param None.
  * @retval CSI_Status: The status of CSI.
  */
CSI_Status HAL_CSI_Status()
{
	CSI_Status sta;
	sta.luminance = HAL_GET_BIT(CSI->CSI_BUF_STA_REG, LUM_STATIS) >> 8;
	sta.still_Mode = HAL_GET_BIT(CSI->CSI_BUF_STA_REG, HAL_BIT(1)) >> 1;
	sta.video_Mode = HAL_GET_BIT(CSI->CSI_BUF_STA_REG, HAL_BIT(0));
	return sta;
}

/**
  * @brief Configure the interrupt flag for CSI.
  * @note Set the interrupt source for CSI.
  * @param irq_signel: The interrupt flag of CSI.
  * @param ctrl: Controls the enable of interrupt flag.
  *        @arg This parameter can be: CSI_ENABLE or CSI_DISABLE.
  * @retval None.
  */
void HAL_CSI_Interrupt_Cfg(CSI_INTERRUPT_SIGNAL irq_signel, CSI_CTRL ctrl)
{
	if (ctrl == CSI_ENABLE)
		HAL_SET_BIT(CSI->CSI_INT_EN_REG, irq_signel);
	else
		HAL_CLR_BIT(CSI->CSI_INT_EN_REG, irq_signel);
}

/**
  * @brief Gets the CSI IRQ status.
  * @param None.
  * @return The status of CSI IRQ.
  *        @arg CSI_CAPTURE_DONE_IRQ: The picture capture done.
  *        @arg CSI_FRAME_DONE_IRQ:     The ftame done.
  *        @arg CSI_FIFO_0_OVERFLOW_IRQ: The FIFO is overflow.
  *		      When this flag appears, you need to restart CSI.
  *        @arg CSI_ALL_FIFO_OVERFLOW_IRQ: ALL FIFO is overflow.
  *	             When this flag appears, you need to restart CSI.
  *        @arg CSI_VSYNC_IRQ: Capture a vsync signal.
  *        @arg CSI_FIFO_0_A_READY_IRQ: The CSI_FIFO_A is ready for read.
  *        @arg CSI_FIFO_0_B_READY_IRQ: The CSI_FIFO_B is ready for read.
  */
__IO uint32_t HAL_CSI_Interrupt_Sta()
{
	return CSI->CSI_INT_STA_REG;
}

/**
  * @brief Clear the CSI interrupt flag.
  * @param None.
  * @retval None.
  */
void HAL_CSI_Interrupt_Clear()
{
	HAL_SET_BIT(CSI->CSI_INT_STA_REG, CSI->CSI_INT_STA_REG);
}

/**
  * @brief Sets the size of the captured image.
  * @param size: Set the start capture position and the number of pixels per row.
  *        @arg size->start: start capture position, unit byte..
  *        @arg size->hor_len: the number of bytes per row.
  * @retval HAL_Status:
  *             The status of driver.
  */
HAL_Status HAL_CSI_Set_Picture_Size(CSI_Picture_Size *size)
{
	if (size->hor_start > (HAL_BIT(14) - 1)) {
		HAL_WRN("%s, %d csi Picture size error hor_start = %d\n",
					__func__, __LINE__, size->hor_start);
		return HAL_ERROR;
	}

	if (size->hor_len > (HAL_BIT(14) - 1)) {
		HAL_WRN("%s, %d csi Picture size error hor_len = %d\n",
					__func__, __LINE__, size->hor_len);
		return HAL_ERROR;
	}

	HAL_CLR_BIT(CSI->CSI_HSIZE_REG, CSI_SIZE_REG);

	HAL_SET_BIT(CSI->CSI_HSIZE_REG, size->hor_len << 16);
	HAL_SET_BIT(CSI->CSI_HSIZE_REG, size->hor_start);
	return HAL_OK;
}

/**
  * @brief  Cached data length of FIFO.
  * @param None
  * @retval CSI_FIFO_Data_Len: The data length of CSI_FIFO_A and CSI_FIFO_B.
  *        @arg FIFO_0_A_Data_Len: The data length of CSI_FIFO_A.
  *        @arg FIFO_0_B_Data_Len: The data length of CSI_FIFO_B.
  */
CSI_FIFO_Data_Len HAL_CSI_FIFO_Data_Len()
{
	CSI_FIFO_Data_Len len;
	len.FIFO_0_B_Data_Len = HAL_GET_BIT(CSI->CSI_TRUE_DATA_NUM, (CSI_VALID_DATA_LEN << 16)) >> 16;
	len.FIFO_0_A_Data_Len = HAL_GET_BIT(CSI->CSI_TRUE_DATA_NUM, CSI_VALID_DATA_LEN);
	return len;
}

/**
  * @private
  * @brief Enables or disables the CSI JPEG mode.
  * @param ctrl: Controls the enable of CSI JPEG mode.
  *         @arg This parameter can be: CSI_ENABLE or CSI_DISABLE.
  * @retval None
  */
void HAL_CIS_JPEG_Mode_Enable(CSI_CTRL ctrl)
{
	if (CSI_ENABLE == ctrl)
		HAL_SET_BIT(CSI->CSI_JPEG_MOD_SEL,  HAL_BIT(0));
	else
		HAL_CLR_BIT(CSI->CSI_JPEG_MOD_SEL, HAL_BIT(0));
}

/**
  * @brief Enables or disables the CSI interrupt.
  * @param cb: The CSI interrupt callback function.
  * @param ctrl: Controls the enable of CSI interrupt.
  *        @arg This parameter can be: CSI_ENABLE or CSI_DISABLE.
  * @retval None
  */
void HAL_CSI_Interrupt_Enable(CSI_Call_Back *cb, CSI_CTRL ctrl)
{
	if (CSI_ENABLE == ctrl) {
		if (cb == NULL) {
			private_csi_cb.callBack = NULL;
			private_csi_cb.arg = NULL;
		} else
			private_csi_cb = *cb;
		CSI_Irq_Enable();
	} else {
		private_csi_cb.callBack = NULL;
		private_csi_cb.arg = NULL;
		CSI_Irq_Disable();
	}
}

void CSI_Printf()
{
	printf("CSI_EN_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_EN_REG, CSI->CSI_EN_REG);
	printf("CSI_CFG_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_CFG_REG, CSI->CSI_CFG_REG);
	printf("CSI_CAP_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_CAP_REG, CSI->CSI_CAP_REG);
	printf("CSI_SCALE_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_SCALE_REG, CSI->CSI_SCALE_REG);
	printf("CSI_BUF_CTL_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_BUF_CTL_REG, CSI->CSI_BUF_CTL_REG);
	printf("CSI_BUF_STA_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_BUF_STA_REG, CSI->CSI_BUF_STA_REG);
	printf("CSI_INT_EN_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_INT_EN_REG, CSI->CSI_INT_EN_REG);
	printf("CSI_INT_STA_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_INT_STA_REG, CSI->CSI_INT_STA_REG);
	printf("CSI_HSIZE_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_HSIZE_REG, CSI->CSI_HSIZE_REG);
	printf("CSI_BF_LEN_REG 0x%x 0x%x\n", (uint32_t)&CSI->CSI_BF_LEN_REG, CSI->CSI_BF_LEN_REG);
	printf("CSI_TRUE_DATA_NUM 0x%x 0x%x\n", (uint32_t)&CSI->CSI_TRUE_DATA_NUM, CSI->CSI_TRUE_DATA_NUM);
	printf("CSI_JPEG_MOD_SEL 0x%x 0x%x\n", (uint32_t)&CSI->CSI_JPEG_MOD_SEL, CSI->CSI_JPEG_MOD_SEL);
}
