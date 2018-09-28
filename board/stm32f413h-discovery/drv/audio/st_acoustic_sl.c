/*
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <k_api.h>
#include <aos/aos.h>

#include "acoustic_sl.h"
#include "st_acoustic_sl.h"



#define DEFAULT_AUDIO_IN_VOLUME		64

#define REC_BUFF_SIZE	512

#define MIC_U3		0
#define MIC_U4		1
#define MIC_U2		2
#define MIC_U5		3


DFSDM_Channel_HandleTypeDef  Dfsdm2Channel1Handle;
DFSDM_Channel_HandleTypeDef  Dfsdm2Channel0Handle;
DFSDM_Channel_HandleTypeDef  Dfsdm2Channel7Handle;
DFSDM_Channel_HandleTypeDef  Dfsdm2Channel6Handle;
DFSDM_Channel_HandleTypeDef  Dfsdm1Channel1Handle;
DFSDM_Filter_HandleTypeDef   Dfsdm2Filter1Handle;
DFSDM_Filter_HandleTypeDef   Dfsdm2Filter2Handle;
DFSDM_Filter_HandleTypeDef   Dfsdm2Filter3Handle;
DFSDM_Filter_HandleTypeDef   Dfsdm2Filter4Handle;
DFSDM_Filter_HandleTypeDef   Dfsdm1Filter1Handle;


static int32_t Dfsdm2Filter1RecBuff[REC_BUFF_SIZE];
static int32_t Dfsdm2Filter2RecBuff[REC_BUFF_SIZE];
static int32_t Dfsdm2Filter3RecBuff[REC_BUFF_SIZE];
static int32_t Dfsdm2Filter4RecBuff[REC_BUFF_SIZE];


extern int8_t mic_loudness_db;
extern int8_t sound_location;
extern int8_t display_view_index;

static uint8_t u2_rec_buff_half = 0;
static uint8_t u2_rec_buff_cplt = 0;
static uint8_t u3_rec_buff_half = 0;
static uint8_t u3_rec_buff_cplt = 0;
static uint8_t u4_rec_buff_half = 0;
static uint8_t u4_rec_buff_cplt = 0;
static uint8_t u5_rec_buff_half = 0;
static uint8_t u5_rec_buff_cplt = 0;
static int16_t u2_pcm_buff[REC_BUFF_SIZE];
static int16_t u3_pcm_buff[REC_BUFF_SIZE];
static int16_t u4_pcm_buff[REC_BUFF_SIZE];
static int16_t u5_pcm_buff[REC_BUFF_SIZE];

static ksem_t rec_buff_half_sem;
static ksem_t rec_buff_cplt_sem;

AcousticSL_Config_t *g_asl_handler;

static int8_t audio_location_running;

static HP_FilterState_TypeDef HP_Filters[4];


static int32_t DFSDM_Init(uint32_t rate)
{
  /* Initialize DFSDM1 channel 1 */
  __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&Dfsdm1Channel1Handle);
  Dfsdm1Channel1Handle.Instance                      = DFSDM1_DATIN1_INSTANCE;
  Dfsdm1Channel1Handle.Init.OutputClock.Activation   = ENABLE;
  Dfsdm1Channel1Handle.Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  Dfsdm1Channel1Handle.Init.OutputClock.Divider      = DFSDM_CLOCK_DIVIDER(rate);
  Dfsdm1Channel1Handle.Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  Dfsdm1Channel1Handle.Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
  Dfsdm1Channel1Handle.Init.Input.Pins               = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  Dfsdm1Channel1Handle.Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_RISING;
  Dfsdm1Channel1Handle.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;  
  Dfsdm1Channel1Handle.Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
  Dfsdm1Channel1Handle.Init.Awd.Oversampling         = 10;
  Dfsdm1Channel1Handle.Init.Offset                   = 0;
  Dfsdm1Channel1Handle.Init.RightBitShift            = DFSDM_MIC_BIT_SHIFT(rate);
  if(HAL_OK != HAL_DFSDM_ChannelInit(&Dfsdm1Channel1Handle))
  {
    return -1;
  }
  
  /* Initialize DFSDM2 channel 1 */  
  __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&Dfsdm2Channel1Handle);
  Dfsdm2Channel1Handle.Instance                      = DFSDM2_DATIN1_INSTANCE;
  Dfsdm2Channel1Handle.Init.OutputClock.Activation   = ENABLE;
  Dfsdm2Channel1Handle.Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  Dfsdm2Channel1Handle.Init.OutputClock.Divider      = DFSDM_CLOCK_DIVIDER(rate);
  Dfsdm2Channel1Handle.Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  Dfsdm2Channel1Handle.Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
  Dfsdm2Channel1Handle.Init.Input.Pins               = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  Dfsdm2Channel1Handle.Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_RISING;
  Dfsdm2Channel1Handle.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;  
  Dfsdm2Channel1Handle.Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
  Dfsdm2Channel1Handle.Init.Awd.Oversampling         = 10;
  Dfsdm2Channel1Handle.Init.Offset                   = 0;
  Dfsdm2Channel1Handle.Init.RightBitShift            = DFSDM_MIC_BIT_SHIFT(rate);
  if(HAL_OK != HAL_DFSDM_ChannelInit(&Dfsdm2Channel1Handle))
  {
    return -2;
  }

  /* Initialize DFSDM2 channel 0 */  
  __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&Dfsdm2Channel0Handle);
  Dfsdm2Channel0Handle.Instance                      = DFSDM2_DATIN0_INSTANCE;
  Dfsdm2Channel0Handle.Init.OutputClock.Activation   = ENABLE;
  Dfsdm2Channel0Handle.Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  Dfsdm2Channel0Handle.Init.OutputClock.Divider      = DFSDM_CLOCK_DIVIDER(rate);
  Dfsdm2Channel0Handle.Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  Dfsdm2Channel0Handle.Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE; 
  Dfsdm2Channel0Handle.Init.Input.Pins               = DFSDM_CHANNEL_FOLLOWING_CHANNEL_PINS;  
  Dfsdm2Channel0Handle.Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_FALLING; 
  Dfsdm2Channel0Handle.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;  
  Dfsdm2Channel0Handle.Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
  Dfsdm2Channel0Handle.Init.Awd.Oversampling         = 10;
  Dfsdm2Channel0Handle.Init.Offset                   = 0;
  Dfsdm2Channel0Handle.Init.RightBitShift            = DFSDM_MIC_BIT_SHIFT(rate);
  if(HAL_OK != HAL_DFSDM_ChannelInit(&Dfsdm2Channel0Handle))
  {
    return -3;
  }
  
  /* Initialize DFSDM2 channel 7 */
  __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&Dfsdm2Channel7Handle);
  Dfsdm2Channel7Handle.Instance                      = DFSDM2_DATIN7_INSTANCE;
  Dfsdm2Channel7Handle.Init.OutputClock.Activation   = ENABLE;
  Dfsdm2Channel7Handle.Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  Dfsdm2Channel7Handle.Init.OutputClock.Divider      = DFSDM_CLOCK_DIVIDER(rate);
  Dfsdm2Channel7Handle.Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  Dfsdm2Channel7Handle.Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
  Dfsdm2Channel7Handle.Init.Input.Pins               = DFSDM_CHANNEL_SAME_CHANNEL_PINS;
  Dfsdm2Channel7Handle.Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_FALLING;
  Dfsdm2Channel7Handle.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL;  
  Dfsdm2Channel7Handle.Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
  Dfsdm2Channel7Handle.Init.Awd.Oversampling         = 10;
  Dfsdm2Channel7Handle.Init.Offset                   = 0;
  Dfsdm2Channel7Handle.Init.RightBitShift            = DFSDM_MIC_BIT_SHIFT(rate);
  if(HAL_OK != HAL_DFSDM_ChannelInit(&Dfsdm2Channel7Handle))
  {
    return -4;
  }

  /* Initialize DFSDM2 channel 6 */
  __HAL_DFSDM_CHANNEL_RESET_HANDLE_STATE(&Dfsdm2Channel6Handle);
  Dfsdm2Channel6Handle.Instance                      = DFSDM2_DATIN6_INSTANCE;
  Dfsdm2Channel6Handle.Init.OutputClock.Activation   = ENABLE;
  Dfsdm2Channel6Handle.Init.OutputClock.Selection    = DFSDM_CHANNEL_OUTPUT_CLOCK_AUDIO;
  Dfsdm2Channel6Handle.Init.OutputClock.Divider      = DFSDM_CLOCK_DIVIDER(rate);
  Dfsdm2Channel6Handle.Init.Input.Multiplexer        = DFSDM_CHANNEL_EXTERNAL_INPUTS;
  Dfsdm2Channel6Handle.Init.Input.DataPacking        = DFSDM_CHANNEL_STANDARD_MODE;
  Dfsdm2Channel6Handle.Init.Input.Pins               = DFSDM_CHANNEL_FOLLOWING_CHANNEL_PINS;  

  Dfsdm2Channel0Handle.Init.SerialInterface.Type     = DFSDM_CHANNEL_SPI_RISING; 
  Dfsdm2Channel6Handle.Init.SerialInterface.SpiClock = DFSDM_CHANNEL_SPI_CLOCK_INTERNAL; 
  Dfsdm2Channel6Handle.Init.Awd.FilterOrder          = DFSDM_CHANNEL_SINC1_ORDER;
  Dfsdm2Channel6Handle.Init.Awd.Oversampling         = 10;
  Dfsdm2Channel6Handle.Init.Offset                   = 0;
  Dfsdm2Channel6Handle.Init.RightBitShift            = DFSDM_MIC_BIT_SHIFT(rate);
  if(HAL_OK != HAL_DFSDM_ChannelInit(&Dfsdm2Channel6Handle))
  {
    return -5;
  }

  /* Initialize DFSDM1 filter 0 */
  __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&Dfsdm1Filter1Handle);
  Dfsdm1Filter1Handle.Instance                          = DFSDM1_FILTER0;
  Dfsdm1Filter1Handle.Init.RegularParam.Trigger         = DFSDM_FILTER_SW_TRIGGER;
  Dfsdm1Filter1Handle.Init.RegularParam.FastMode        = ENABLE;
  Dfsdm1Filter1Handle.Init.RegularParam.DmaMode         = ENABLE;
  Dfsdm1Filter1Handle.Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
  Dfsdm1Filter1Handle.Init.InjectedParam.ScanMode       = DISABLE;
  Dfsdm1Filter1Handle.Init.InjectedParam.DmaMode        = DISABLE;
  Dfsdm1Filter1Handle.Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
  Dfsdm1Filter1Handle.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
  Dfsdm1Filter1Handle.Init.FilterParam.SincOrder        = DFSDM_FILTER_ORDER(rate);
  Dfsdm1Filter1Handle.Init.FilterParam.Oversampling     = DFSDM_OVER_SAMPLING(rate);
  Dfsdm1Filter1Handle.Init.FilterParam.IntOversampling  = 1;
  if(HAL_OK != HAL_DFSDM_FilterInit(&Dfsdm1Filter1Handle))
  {
    return -6;
  }
  
  /* Initialize DFSDM2 filter 1 */
  __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&Dfsdm2Filter1Handle);
  Dfsdm2Filter1Handle.Instance                          = DFSDM2_FILTER0;
  Dfsdm2Filter1Handle.Init.RegularParam.Trigger         = DFSDM_FILTER_SW_TRIGGER;
  Dfsdm2Filter1Handle.Init.RegularParam.FastMode        = ENABLE;
  Dfsdm2Filter1Handle.Init.RegularParam.DmaMode         = ENABLE;
  Dfsdm2Filter1Handle.Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
  Dfsdm2Filter1Handle.Init.InjectedParam.ScanMode       = DISABLE;
  Dfsdm2Filter1Handle.Init.InjectedParam.DmaMode        = DISABLE;
  Dfsdm2Filter1Handle.Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
  Dfsdm2Filter1Handle.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
  Dfsdm2Filter1Handle.Init.FilterParam.SincOrder        = DFSDM_FILTER_ORDER(rate);
  Dfsdm2Filter1Handle.Init.FilterParam.Oversampling     = DFSDM_OVER_SAMPLING(rate);
  Dfsdm2Filter1Handle.Init.FilterParam.IntOversampling  = 1;
  if(HAL_OK != HAL_DFSDM_FilterInit(&Dfsdm2Filter1Handle))
  {
    return -7;
  }
  
  /* Initialize DFSDM2 filter 2 */
  __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&Dfsdm2Filter2Handle);
  Dfsdm2Filter2Handle.Instance                          = DFSDM2_FILTER1;
  Dfsdm2Filter2Handle.Init.RegularParam.Trigger         = DFSDM_FILTER_SYNC_TRIGGER;
  Dfsdm2Filter2Handle.Init.RegularParam.FastMode        = ENABLE;
  Dfsdm2Filter2Handle.Init.RegularParam.DmaMode         = ENABLE;
  Dfsdm2Filter2Handle.Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
  Dfsdm2Filter2Handle.Init.InjectedParam.ScanMode       = DISABLE;
  Dfsdm2Filter2Handle.Init.InjectedParam.DmaMode        = DISABLE;
  Dfsdm2Filter2Handle.Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
  Dfsdm2Filter2Handle.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
  Dfsdm2Filter2Handle.Init.FilterParam.SincOrder        = DFSDM_FILTER_ORDER(rate);
  Dfsdm2Filter2Handle.Init.FilterParam.Oversampling     = DFSDM_OVER_SAMPLING(rate);
  Dfsdm2Filter2Handle.Init.FilterParam.IntOversampling  = 1;
  if(HAL_OK != HAL_DFSDM_FilterInit(&Dfsdm2Filter2Handle))
  {
    return -8;
  }

  /* Initialize DFSDM2 filter 3 */
  __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&Dfsdm2Filter3Handle);
  Dfsdm2Filter3Handle.Instance                          = DFSDM2_FILTER2;
  Dfsdm2Filter3Handle.Init.RegularParam.Trigger         = DFSDM_FILTER_SYNC_TRIGGER;
  Dfsdm2Filter3Handle.Init.RegularParam.FastMode        = ENABLE;
  Dfsdm2Filter3Handle.Init.RegularParam.DmaMode         = ENABLE;
  Dfsdm2Filter3Handle.Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
  Dfsdm2Filter3Handle.Init.InjectedParam.ScanMode       = DISABLE;
  Dfsdm2Filter3Handle.Init.InjectedParam.DmaMode        = DISABLE;
  Dfsdm2Filter3Handle.Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
  Dfsdm2Filter3Handle.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
  Dfsdm2Filter3Handle.Init.FilterParam.SincOrder        = DFSDM_FILTER_ORDER(rate);
  Dfsdm2Filter3Handle.Init.FilterParam.Oversampling     = DFSDM_OVER_SAMPLING(rate);
  Dfsdm2Filter3Handle.Init.FilterParam.IntOversampling  = 1;
  if(HAL_OK != HAL_DFSDM_FilterInit(&Dfsdm2Filter3Handle))
  {
    return -9;
  }
  
  /* Initialize DFSDM2 filter 4 */
  __HAL_DFSDM_FILTER_RESET_HANDLE_STATE(&Dfsdm2Filter4Handle);
  Dfsdm2Filter4Handle.Instance                          = DFSDM2_FILTER3;
  Dfsdm2Filter4Handle.Init.RegularParam.Trigger         = DFSDM_FILTER_SYNC_TRIGGER;
  Dfsdm2Filter4Handle.Init.RegularParam.FastMode        = ENABLE;
  Dfsdm2Filter4Handle.Init.RegularParam.DmaMode         = ENABLE;
  Dfsdm2Filter4Handle.Init.InjectedParam.Trigger        = DFSDM_FILTER_SW_TRIGGER;
  Dfsdm2Filter4Handle.Init.InjectedParam.ScanMode       = DISABLE;
  Dfsdm2Filter4Handle.Init.InjectedParam.DmaMode        = DISABLE;
  Dfsdm2Filter4Handle.Init.InjectedParam.ExtTrigger     = DFSDM_FILTER_EXT_TRIG_TIM8_TRGO;
  Dfsdm2Filter4Handle.Init.InjectedParam.ExtTriggerEdge = DFSDM_FILTER_EXT_TRIG_BOTH_EDGES;
  Dfsdm2Filter4Handle.Init.FilterParam.SincOrder        = DFSDM_FILTER_ORDER(rate);
  Dfsdm2Filter4Handle.Init.FilterParam.Oversampling     = DFSDM_OVER_SAMPLING(rate);
  Dfsdm2Filter4Handle.Init.FilterParam.IntOversampling  = 1;
  if(HAL_OK != HAL_DFSDM_FilterInit(&Dfsdm2Filter4Handle))
  {
    return -10;
  }

  /* Configure Regular channel */
  if(HAL_OK != HAL_DFSDM_FilterConfigRegChannel(&Dfsdm1Filter1Handle, DFSDM_DATIN1_CHANNEL, DFSDM_CONTINUOUS_CONV_ON))
  {
    return -11;
  } 
  /* Configure Regular channel */
  if(HAL_OK != HAL_DFSDM_FilterConfigRegChannel(&Dfsdm2Filter1Handle, DFSDM_DATIN1_CHANNEL, DFSDM_CONTINUOUS_CONV_ON))
  {
    return -12;
  }
  /* Configure Regular channel */
  if(HAL_OK != HAL_DFSDM_FilterConfigRegChannel(&Dfsdm2Filter2Handle, DFSDM_DATIN0_CHANNEL, DFSDM_CONTINUOUS_CONV_ON))
  {
    return -13;
  } 
  /* Configure Regular channel */
  if(HAL_OK != HAL_DFSDM_FilterConfigRegChannel(&Dfsdm2Filter3Handle, DFSDM_DATIN7_CHANNEL, DFSDM_CONTINUOUS_CONV_ON))
  {
    return -14;
  } 
  /* Configure Regular channel */
  if(HAL_OK != HAL_DFSDM_FilterConfigRegChannel(&Dfsdm2Filter4Handle, DFSDM_DATIN6_CHANNEL, DFSDM_CONTINUOUS_CONV_ON))
  {
    return -15;
  }
  return 0;
}

static void DFSDM_DeInit(void)
{  
  if(HAL_OK != HAL_DFSDM_ChannelDeInit(&Dfsdm1Channel1Handle))
  {
    return;
  }

  if(HAL_OK != HAL_DFSDM_ChannelDeInit(&Dfsdm2Channel1Handle))
  {
    return;
  }

  if(HAL_OK != HAL_DFSDM_ChannelDeInit(&Dfsdm2Channel0Handle))
  {
    return;
  }
  
  if(HAL_OK != HAL_DFSDM_ChannelDeInit(&Dfsdm2Channel7Handle))
  {
    return;
  }

  if(HAL_OK != HAL_DFSDM_ChannelDeInit(&Dfsdm2Channel6Handle))
  {
    return;
  }

  if(HAL_OK != HAL_DFSDM_FilterDeInit(&Dfsdm1Filter1Handle))
  {
    return;
  }

  if(HAL_OK != HAL_DFSDM_FilterDeInit(&Dfsdm2Filter1Handle))
  {
    return;
  }

  if(HAL_OK != HAL_DFSDM_FilterDeInit(&Dfsdm2Filter2Handle))
  {
    return;
  }

  if(HAL_OK != HAL_DFSDM_FilterDeInit(&Dfsdm2Filter3Handle))
  {
    return;
  }

  if(HAL_OK != HAL_DFSDM_FilterDeInit(&Dfsdm2Filter4Handle))
  {
    return;
  }
}

static int acoustic_sl_init(AcousticSL_Handler_t *asl_handler)
{
  AcousticSL_Config_t asl_config;
  char asl_version[32];

  __CRC_CLK_ENABLE();

  AcousticSL_GetLibVersion(asl_version);
  printf("AcousticSL Version: %s\n", asl_version);

  asl_handler->algorithm = ACOUSTIC_SL_ALGORITHM_GCCP;
  asl_handler->sampling_frequency = AUDIO_FREQUENCY_16K;
  asl_handler->channel_number = 4;
  asl_handler->ptr_M1_channels = 1;
  asl_handler->ptr_M2_channels = 1;
  asl_handler->ptr_M3_channels = 1;
  asl_handler->ptr_M4_channels = 1;
  asl_handler->M12_distance = 100;
  asl_handler->M34_distance = 100;
  asl_handler->samples_to_process = REC_BUFF_SIZE;

  if (AcousticSL_getMemorySize(asl_handler))
	return -1;

  printf("AcousticSL memory size: %u\n", asl_handler->internal_memory_size);

  asl_handler->pInternalMemory = (uint32_t *)aos_malloc(asl_handler->internal_memory_size);
  if (!asl_handler->pInternalMemory)
	return -2;

  if (AcousticSL_Init(asl_handler))
	return -3;

  asl_config.resolution = 10;
  asl_config.threshold = 30;
  if (AcousticSL_setConfig(asl_handler, &asl_config))
	return -4;

  printf("AcousticSL init success\n");
  return 0;
}

static int acoustic_sl_deinit(AcousticSL_Handler_t *asl_handler)
{
  __CRC_CLK_DISABLE();
  return 0;
}

int acoustic_sl_process_callback(void)
{
  int32_t angle = 0;
  static uint8_t u3_count = 1;
  AcousticSL_Process(&angle, g_asl_handler);
  if (angle != -100) {
  	printf("angle %d\n", angle);

	if ((angle >= 0 && angle <= 60) || angle >= 290) {
		if (angle == 45) {
			if (u3_count++ >= 5) {
				sound_location = MIC_U3;
				u3_count = 0;
			}
		} else {
			sound_location = MIC_U3;
			u3_count = 0;
		}
	} else if (angle > 60 && angle <= 170) {
		sound_location = MIC_U4;
		u3_count = 0;
	} else if (angle > 170 && angle <= 224) {
		sound_location = MIC_U5;
		u3_count = 0;
	} else {
		sound_location = MIC_U2;
		u3_count = 0;
	}
  }
}

int32_t acoustic_sl_start(void)
{
  AcousticSL_Handler_t *asl_handler;
  int32_t ret;
  int32_t i;

  uint32_t sample_level_sum = 0;

  asl_handler = (AcousticSL_Handler_t *)aos_malloc(sizeof(AcousticSL_Handler_t));
  if (!asl_handler) {
  	printf("%s: alloc AcousticSL_Handler_t failed !\n", __func__);
	return -1;
  }
  memset(asl_handler, 0, sizeof(AcousticSL_Handler_t));

  ret = acoustic_sl_init(asl_handler);
  if (ret) {
  	printf("%s: init AcousticSL failed %d !\n", __func__, ret);
    return -2;
  }
  g_asl_handler = asl_handler;

  if (krhino_sem_create(&rec_buff_half_sem, "recHalfSem", 0)) {
	printf("%s: create recHalfSem failed !\n", __func__);
    return -3;
  }

  if (krhino_sem_create(&rec_buff_cplt_sem, "recCpltSem", 0)) {
	printf("%s: create recCpltSem failed !\n", __func__);
    return -4;
  }

  ret = DFSDM_Init(AUDIO_FREQUENCY_16K);
  if (ret) {
  	printf("%s: init DFSDM failed %d !\n", __func__, ret);
    return -5;
  }


  if (HAL_DFSDM_FilterRegularStart_DMA(&Dfsdm2Filter2Handle, Dfsdm2Filter2RecBuff, REC_BUFF_SIZE)) {
  	printf("%s: start DFSDM2Filter2 failed !\n", __func__);
    return -6;
  }

  if (HAL_DFSDM_FilterRegularStart_DMA(&Dfsdm2Filter3Handle, Dfsdm2Filter3RecBuff, REC_BUFF_SIZE)) {
  	printf("%s: start DFSDM2Filter3 failed !\n", __func__);
    return -7;
  }

  if (HAL_DFSDM_FilterRegularStart_DMA(&Dfsdm2Filter4Handle, Dfsdm2Filter4RecBuff, REC_BUFF_SIZE)) {
    printf("%s: start DFSDM2Filter4 failed !\n", __func__);
    return -8;
  }

  if (HAL_DFSDM_FilterRegularStart_DMA(&Dfsdm2Filter1Handle, Dfsdm2Filter1RecBuff, REC_BUFF_SIZE)) {
    printf("%s: start DFSDM2Filter1 failed !\n", __func__);
    return -9;
  }

  HAL_NVIC_SetPriority((IRQn_Type)EXTI2_IRQn, 0x0D, 0);
  HAL_NVIC_EnableIRQ((IRQn_Type)EXTI2_IRQn);

  audio_location_running = 1;
  printf("%s: sound location test start\n", __func__);

  while (audio_location_running) {
    if (krhino_sem_take(&rec_buff_half_sem, 200) != RHINO_SUCCESS)
		return -10;

	for(i = 0; i < REC_BUFF_SIZE/2; i++) {
		HP_Filters[0].Z = ((Dfsdm2Filter2RecBuff[i] >> 8) * DEFAULT_AUDIO_IN_VOLUME) >> 7;
		HP_Filters[0].oldOut = (0xFC * (HP_Filters[0].oldOut + HP_Filters[0].Z - HP_Filters[0].oldIn)) / 256;
		HP_Filters[0].oldIn = HP_Filters[0].Z;
		u2_pcm_buff[i] = SaturaLH(HP_Filters[0].oldOut, -32760, 32760);

		HP_Filters[1].Z = ((Dfsdm2Filter1RecBuff[i] >> 8) * DEFAULT_AUDIO_IN_VOLUME) >> 7;
		HP_Filters[1].oldOut = (0xFC * (HP_Filters[1].oldOut + HP_Filters[1].Z - HP_Filters[1].oldIn)) / 256;
		HP_Filters[1].oldIn = HP_Filters[1].Z;
		u3_pcm_buff[i] = SaturaLH(HP_Filters[1].oldOut, -32760, 32760);

		HP_Filters[2].Z = ((Dfsdm2Filter3RecBuff[i] >> 8) * DEFAULT_AUDIO_IN_VOLUME) >> 7;
		HP_Filters[2].oldOut = (0xFC * (HP_Filters[2].oldOut + HP_Filters[2].Z - HP_Filters[2].oldIn)) / 256;
		HP_Filters[2].oldIn = HP_Filters[2].Z;
		u4_pcm_buff[i] = SaturaLH(HP_Filters[2].oldOut, -32760, 32760);

		HP_Filters[3].Z = ((Dfsdm2Filter4RecBuff[i] >> 8) * DEFAULT_AUDIO_IN_VOLUME) >> 7;
		HP_Filters[3].oldOut = (0xFC * (HP_Filters[3].oldOut + HP_Filters[3].Z - HP_Filters[3].oldIn)) / 256;
		HP_Filters[3].oldIn = HP_Filters[3].Z;
		u5_pcm_buff[i] = SaturaLH(HP_Filters[3].oldOut, -32760, 32760);
	}

    u3_rec_buff_half = 0;
    u2_rec_buff_half = 0;
    u4_rec_buff_half = 0;
    u5_rec_buff_half = 0;

    if (krhino_sem_take(&rec_buff_cplt_sem, 200) != RHINO_SUCCESS)
		return -11;

    for (i = REC_BUFF_SIZE/2; i < REC_BUFF_SIZE; i++) {
      HP_Filters[0].Z = ((Dfsdm2Filter2RecBuff[i] >> 8) * DEFAULT_AUDIO_IN_VOLUME) >> 7;
      HP_Filters[0].oldOut = (0xFC * (HP_Filters[0].oldOut + HP_Filters[0].Z - HP_Filters[0].oldIn)) / 256;
      HP_Filters[0].oldIn = HP_Filters[0].Z;
      u2_pcm_buff[i] = SaturaLH(HP_Filters[0].oldOut, -32760, 32760);

      HP_Filters[1].Z = ((Dfsdm2Filter1RecBuff[i] >> 8) * DEFAULT_AUDIO_IN_VOLUME) >> 7;
      HP_Filters[1].oldOut = (0xFC * (HP_Filters[1].oldOut + HP_Filters[1].Z - HP_Filters[1].oldIn)) / 256;
      HP_Filters[1].oldIn = HP_Filters[1].Z;
      u3_pcm_buff[i] = SaturaLH(HP_Filters[1].oldOut, -32760, 32760);

      HP_Filters[2].Z = ((Dfsdm2Filter3RecBuff[i] >> 8) * DEFAULT_AUDIO_IN_VOLUME) >> 7;
      HP_Filters[2].oldOut = (0xFC * (HP_Filters[2].oldOut + HP_Filters[2].Z - HP_Filters[2].oldIn)) / 256;
      HP_Filters[2].oldIn = HP_Filters[2].Z;
      u4_pcm_buff[i] = SaturaLH(HP_Filters[2].oldOut, -32760, 32760);

      HP_Filters[3].Z = ((Dfsdm2Filter4RecBuff[i] >> 8) * DEFAULT_AUDIO_IN_VOLUME) >> 7;
      HP_Filters[3].oldOut = (0xFC * (HP_Filters[3].oldOut + HP_Filters[3].Z - HP_Filters[3].oldIn)) / 256;
      HP_Filters[3].oldIn = HP_Filters[3].Z;
      u5_pcm_buff[i] = SaturaLH(HP_Filters[3].oldOut, -32760, 32760);
    }

	if (display_view_index == 1) {
		for (i = 0; i < 32; i++) {
			ret = AcousticSL_Data_Input(&u2_pcm_buff[i*16], &u4_pcm_buff[i*16],
				&u5_pcm_buff[i*16], &u3_pcm_buff[i*16], asl_handler);
			if (ret) {
				HAL_NVIC_SetPendingIRQ(EXTI2_IRQn);
			}
		}
	} else if (display_view_index == 2) {
		sample_level_sum = 0;
		for (i = 0; i < REC_BUFF_SIZE; i++) {
		  sample_level_sum += abs(u2_pcm_buff[i]);
		}
	    mic_loudness_db = (int)(18 * log10((double)(sample_level_sum / REC_BUFF_SIZE)));
	}

    u3_rec_buff_cplt = 0;
    u2_rec_buff_cplt = 0;
    u4_rec_buff_cplt = 0;
    u5_rec_buff_cplt = 0;
  }
  HAL_DFSDM_FilterRegularStop_DMA(&Dfsdm2Filter1Handle);
  HAL_DFSDM_FilterRegularStop_DMA(&Dfsdm2Filter4Handle);
  HAL_DFSDM_FilterRegularStop_DMA(&Dfsdm2Filter3Handle);
  HAL_DFSDM_FilterRegularStop_DMA(&Dfsdm2Filter2Handle);
  HAL_NVIC_DisableIRQ((IRQn_Type)EXTI2_IRQn);
  DFSDM_DeInit();
  krhino_sem_count_set(&rec_buff_cplt_sem, 0);
  krhino_sem_count_set(&rec_buff_half_sem, 0);
  krhino_sem_del(&rec_buff_cplt_sem);
  krhino_sem_del(&rec_buff_half_sem);
  g_asl_handler = NULL;
  aos_free(asl_handler);

  printf("%s: sound location test stop\n", __func__);
  return 0;
}

int acoustic_sl_stop(void)
{
	audio_location_running = 0;
	return 0;
}


/**
  * @brief  Half regular conversion complete callback.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvHalfCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  if (hdfsdm_filter == &Dfsdm2Filter1Handle)
    u3_rec_buff_half = 1;
  else if (hdfsdm_filter == &Dfsdm2Filter2Handle)
    u2_rec_buff_half = 1;
  else if (hdfsdm_filter == &Dfsdm2Filter3Handle)
    u4_rec_buff_half = 1;
  else if (hdfsdm_filter == &Dfsdm2Filter4Handle)
    u5_rec_buff_half = 1;

  if (u2_rec_buff_half && u3_rec_buff_half && u4_rec_buff_half && u5_rec_buff_half)
	krhino_sem_give(&rec_buff_half_sem);
}

/**
  * @brief  Regular conversion complete callback.
  * @note   In interrupt mode, user has to read conversion value in this function
            using HAL_DFSDM_FilterGetRegularValue.
  * @param  hdfsdm_filter : DFSDM filter handle.
  * @retval None
  */
void HAL_DFSDM_FilterRegConvCpltCallback(DFSDM_Filter_HandleTypeDef *hdfsdm_filter)
{
  if (hdfsdm_filter == &Dfsdm2Filter1Handle)
    u3_rec_buff_cplt = 1;
  else if (hdfsdm_filter == &Dfsdm2Filter2Handle)
    u2_rec_buff_cplt = 1;
  else if (hdfsdm_filter == &Dfsdm2Filter3Handle)
    u4_rec_buff_cplt = 1;
  else if (hdfsdm_filter == &Dfsdm2Filter4Handle)
    u5_rec_buff_cplt = 1;

  if (u3_rec_buff_cplt && u2_rec_buff_cplt && u4_rec_buff_cplt && u5_rec_buff_cplt)
	krhino_sem_give(&rec_buff_cplt_sem);
}

void DMA2_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Dfsdm2Filter1Handle.hdmaReg);
}

void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Dfsdm2Filter2Handle.hdmaReg);
}

void DMA2_Stream2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Dfsdm2Filter3Handle.hdmaReg);
}

void DMA2_Stream3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Dfsdm2Filter4Handle.hdmaReg);
}

void DMA2_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Dfsdm1Filter1Handle.hdmaReg);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

