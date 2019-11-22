#include "k_api.h"
#include "stm32l4xx_hal.h"
#include "irda.h"
#include <stdlib.h>
#include <string.h>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

#define IR_SIG_REVERSE

/* in sec */
#define STUDY_TIME											12
#define SEND_TIME										  	4

#define FEATURE_TOLERANCE               80

#define MAX_TIME_DATA                   128
#define MIN_VAID_CODE                   12

#define MODULES_TOLERANCE               106 /* 38kHz * 4 */
#define MIN_DIV                         106 /* 38kHz * 4 */
#define TRY_RANGE                       27  /* 38kHz * 1 */

#define TIM_FORCED_ACTIVE               ((uint16_t)0x0050)
#define TIM_FORCED_INACTIVE             ((uint16_t)0x0040)

typedef struct {
	uint32_t lead_s_min;
	uint32_t lead_s_max;
	uint32_t lead_e_min;
	uint32_t lead_e_max;
} IR_lead_time;

typedef struct {
	uint32_t time_data_s;
	uint32_t time_data_e;
	uint8_t pwm_data_s;
	uint8_t pwm_data_e;
} IR_sig_data;

typedef enum {
	IR_feature_lead_s = 0,
	IR_feature_lead_e,
	IR_feature_data_1,
	IR_feature_data_2,
	IR_feature_data_3,
	IR_feature_extra,
	IR_feature_max
} IR_feature_code;

typedef enum {
	/* NEC with simple repeat code : uPD6121G/D6121/BU5777/D1913/LC7461M-C13 & NEC with full repeat code : uPD6121G/LC7461M-C13 */
	IR_NEC_1 = 0,
	/* NEC with simple repeat code : AEHA */
	IR_NEC_2,
	/* NEC with full repeat code : MN6024-C5D6 */
	IR_NEC_3,
	/* NEC with full repeat code : MN6014-C6D6/MN6030 */
	IR_NEC_4,
	/* NEC with full repeat code : MATNEW */
	IR_NEC_5,
	/* NEC with full repeat code : PANASONIC */
	IR_NEC_6,
	/* TC9012 : TC9012F/9243 */
	IR_TC9012,
	/* SONY : D7C5/D7C6/D7C8/D7C13 */
	IR_SONY,
	IR_ENUM_MAX
} IR_encoding_types;

static const IR_lead_time lead_arr[] = {
	{8280, 9720, 4140, 4860},
	{2700, 4050, 1350, 2030},
	{2700, 4050, 2700, 4050},
	{2790, 4190, 2790, 4190},
	{2790, 4190, 3000, 4490},
	{2820, 4220, 2820, 4220},
	{4140, 4860, 4140, 4860},
	{1920, 2880, 480 , 720 },
};

static IR_sig_data rcv_data[MAX_TIME_DATA] = {{0}};
static uint32_t rcv_count = 0;
static int lead_data_hit = -1;
static uint32_t feature_code[IR_feature_max] = {0};
static int feature_found = 0;
static uint32_t pwm_data[IR_feature_max] = {0};
static uint32_t pwm_div = 0;
static uint32_t last_rcv_time = 0;

static uint8_t *pwm_send_buffer = NULL;
static uint32_t pwm_send_max = 0;
static uint32_t pwm_send_counter = 0;

static aos_sem_t irda_sem;
static int resend_flag = 1;

/**
* @brief This function handles TIM1 update interrupt and TIM16 global interrupt.
*/
void TIM1_UP_TIM16_IRQHandler(void)
{
  /* This interrupt is time critical for IRDA */
  //krhino_intrpt_enter();
  HAL_TIM_IRQHandler(&htim1);
  HAL_TIM_IRQHandler(&htim16);
  //krhino_intrpt_exit();
}

/**
* @brief This function handles TIM1 capture compare interrupt.
*/
void TIM1_CC_IRQHandler(void)
{
  /* This interrupt is time critical for IRDA */
  //krhino_intrpt_enter();
  HAL_TIM_IRQHandler(&htim1);
  //krhino_intrpt_exit();
}

/**
  * @brief  Force new configuration to the output channel
  * @param  action: new configuration
  * @retval None
  */
static void TIM_ForcedOC1Config(uint32_t action)
{
#ifndef IR_SIG_REVERSE
  uint32_t temporary = htim16.Instance->CCMR1;

  temporary &= ~TIM_CCMR1_OC1M;
  temporary |= action;
  htim16.Instance->CCMR1 = temporary;
#else
	static uint32_t last_act = TIM_FORCED_INACTIVE;
	uint32_t temporary = 0;
	
	if (last_act == action)
		return;
	if (action == TIM_FORCED_ACTIVE) {
		HAL_TIM_OC_Start(&htim17, TIM_CHANNEL_1);
		last_act = TIM_FORCED_ACTIVE;
	} else {
		HAL_TIM_OC_Stop(&htim17, TIM_CHANNEL_1);
		last_act = TIM_FORCED_INACTIVE;
	}
#endif
}

/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim : TIM IC handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	uint32_t time = 0;

	if (htim->Instance != TIM1 || rcv_count >= MAX_TIME_DATA) 
		return;

	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
		time = HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_1);
		rcv_data[rcv_count].time_data_e = time - rcv_data[rcv_count].time_data_s;
		++rcv_count;
	} else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
		time = HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_2);
		rcv_data[rcv_count].time_data_s = time;
		last_rcv_time = krhino_sys_time_get();
	}
}

/**
  * @brief  Period elapsed callback in non blocking mode - timeout
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint32_t byte_index;
	uint32_t bit_index;
	
	if (htim->Instance != TIM16)
		return;
	
	byte_index = pwm_send_counter >> 3;
	bit_index = pwm_send_counter & 7;
	if ((pwm_send_buffer[byte_index] >> bit_index) & 1) {
		TIM_ForcedOC1Config(TIM_FORCED_ACTIVE);
	} else {
		TIM_ForcedOC1Config(TIM_FORCED_INACTIVE);
		if (pwm_send_counter == pwm_send_max - 1)
			aos_sem_signal(&irda_sem);
	}
	++pwm_send_counter;
}

static void ir_dbg(void)
{
	int i;
	uint32_t signal_time = 0;
	uint32_t empty_time = 0;
	uint8_t feature_s = 0;
	uint8_t feature_e = 0;
	
	for (i = 0; i < rcv_count; ++i) {
		signal_time = rcv_data[i].time_data_s;
		empty_time = rcv_data[i].time_data_e;
		feature_s = rcv_data[i].pwm_data_s;
		feature_e = rcv_data[i].pwm_data_e;
		printf("Signal hold time is %lu, feature is %u\n", signal_time, feature_s);
		printf("Empty hold time is %lu, feature is %u\n",empty_time, feature_e);
	}
	
	printf("lead_data_hit is %d\n", lead_data_hit);
	
	for (i = 0; i < feature_found; ++i) {
		printf("Feature %d indicate time is %lu\n", i, feature_code[i]);
	}
	
	printf("PWM Feature div is %lu\n", pwm_div);
	for (i = 0; i < feature_found; ++i) {
		printf("Feature %d time is %lu, PWM data is %lu, PWM time is %lu\n", i, feature_code[i], pwm_data[i], pwm_data[i] * pwm_div);
	}
	
	while (1);
}

static inline int recog_feature(uint32_t time)
{
	int i;
	int time_signed = (int)time;
	int feature_signed = 0;
	int diff = 0;
	
	for (i = feature_found - 1; i >= 0; --i) {
		feature_signed = (int)feature_code[i];
		diff = abs(feature_signed - time_signed);
		if (diff < FEATURE_TOLERANCE)
			return i;
	}
	
	return -1;
}

static int irda_classify_feature_code(void)
{
	int i, j;
	int lead_range = rcv_count - MIN_VAID_CODE;
	int lead_encoding_hit = -1;
	uint32_t feature_sum[IR_feature_max] = {0};
	uint32_t feature_freq[IR_feature_max] = {0};
	int ret = 0;
	
	for (i = 0; i < lead_range; ++i) {
		if (rcv_data[i].time_data_s == 0 || rcv_data[i].time_data_e == 0)
			continue;
		for (j = 0; j < IR_ENUM_MAX; ++j) {
			if (rcv_data[i].time_data_s > lead_arr[j].lead_s_min &&
			    rcv_data[i].time_data_s < lead_arr[j].lead_s_max &&
			    rcv_data[i].time_data_e > lead_arr[j].lead_e_min &&
			    rcv_data[i].time_data_e < lead_arr[j].lead_e_max) {
				lead_encoding_hit = j;
				break;
			}
		}
		if (lead_encoding_hit != -1) {
			lead_data_hit = i;
			break;
		}
	}
	if (lead_encoding_hit == -1 || lead_data_hit == -1) {
		KIDS_A10_PRT("IR encoding lead can not be found!\n");
		return -1;
	}
	feature_code[IR_feature_lead_s] = rcv_data[lead_data_hit].time_data_s;
	feature_code[IR_feature_lead_e] = rcv_data[lead_data_hit].time_data_e;
	feature_found = 2;
	rcv_data[lead_data_hit].pwm_data_s = IR_feature_lead_s;
	rcv_data[lead_data_hit].pwm_data_e = IR_feature_lead_e;
	feature_sum[IR_feature_lead_s] += rcv_data[lead_data_hit].time_data_s;
	feature_sum[IR_feature_lead_e] += rcv_data[lead_data_hit].time_data_e;
	++feature_freq[IR_feature_lead_s];
	++feature_freq[IR_feature_lead_e];
	for (i = lead_data_hit + 1; i < rcv_count; ++i) {
		if (rcv_data[i].time_data_s == 0 || (i != rcv_count - 1 && rcv_data[i].time_data_e == 0)) {
			KIDS_A10_PRT("Capture invalid IR time data!\n");
			return -1;
		}
		
		ret = recog_feature(rcv_data[i].time_data_s);
		if (ret >= 0) {
			rcv_data[i].pwm_data_s = ret;
			feature_sum[ret] += rcv_data[i].time_data_s;
			++feature_freq[ret];
		} else {
			if (feature_found + 1 > IR_feature_max) {
				KIDS_A10_PRT("Overflow feature time is %u\n", (unsigned int)rcv_data[i].time_data_s);
				ir_dbg();
				return -1;
			}
			++feature_found;
			feature_code[feature_found - 1] = rcv_data[i].time_data_s;
			rcv_data[i].pwm_data_s = feature_found - 1;
			feature_sum[feature_found - 1] += rcv_data[i].time_data_s;
			++feature_freq[feature_found - 1];
		}
		
		if (i == rcv_count - 1 && rcv_data[i].time_data_e == 0)
			break;
		ret = recog_feature(rcv_data[i].time_data_e);
		if (ret >= 0) {
			rcv_data[i].pwm_data_e = ret;
			feature_sum[ret] += rcv_data[i].time_data_e;
			++feature_freq[ret];
		} else {
			if (feature_found + 1 > IR_feature_max) {
				KIDS_A10_PRT("Overflow feature time is %u\n", (unsigned int)rcv_data[i].time_data_e);
				ir_dbg();
				return -1;
			}
			++feature_found;
			feature_code[feature_found - 1] = rcv_data[i].time_data_e;
			rcv_data[i].pwm_data_e = feature_found - 1;
			feature_sum[feature_found - 1] += rcv_data[i].time_data_e;
			++feature_freq[feature_found - 1];
		}
	}
	
	for (i = 0; i < IR_feature_max; ++i) {
		feature_code[i] = feature_sum[i] / feature_freq[i];
	}
	
	return 0;
}

static void fill_pwm_data(void)
{
    int i;
    if (pwm_div == 0) {
        KIDS_A10_PRT("pwm_div is zero!\n");
        return;
    }
    for (i = 0; i < feature_found; ++i) {
        pwm_data[i] = feature_code[i] / pwm_div;
        if (feature_code[i] % pwm_div > pwm_div / 2) {
            ++pwm_data[i];
        }
    }
}

static int is_div_by(uint32_t div)
{
    int i;

    for (i = 0; i < feature_found; ++i) {
        if (feature_code[i] % div > MODULES_TOLERANCE) {
            return 0;
        }
    }

    return 1;
}

static int irda_get_pwm_data(void)
{
    uint32_t i;
    uint32_t try_div = 1;
    uint32_t time_min = 0;
    uint32_t try_data_base = 0;
    uint32_t try_data = 0;

    time_min = feature_code[0];
    for (i = 0; i < feature_found; ++i) {
        time_min = feature_code[i] < time_min ? feature_code[i] : time_min;
    }

    if (time_min < MIN_DIV) {
        KIDS_A10_PRT("Capture IR time data too short: %lu\n", time_min);
        return -1;
    }

    try_data_base = time_min / try_div;

    while (try_data_base >= MIN_DIV) {
        for (i = 0; i < TRY_RANGE; ++i) {
            try_data = try_data_base + i;
            if (is_div_by(try_data)) {
                pwm_div = try_data;
                fill_pwm_data();
                return 0;
            }
            if (i == 0)
                continue;
            try_data = try_data_base - i;
            if (try_data < MIN_DIV)
                continue;
            if (is_div_by(try_data)) {
                pwm_div = try_data;
                fill_pwm_data();
                return 0;
            }
        }
        try_data_base = time_min / ++try_div;
    }
    KIDS_A10_PRT("Try div failed, min try_data_base is %lu\n", try_data_base);
		ir_dbg();

    return -1;
}

int irda_study_code(void)
{
	int i;
	uint32_t now_time = 0;
	int ret = 0;
	
	last_rcv_time = 0;
	rcv_count = 0;
	lead_data_hit = -1;
	feature_found = 0;
	pwm_div = 0;
	memset(rcv_data, 0, sizeof(rcv_data));
	memset(feature_code, 0, sizeof(feature_code));
	memset(pwm_data, 0, sizeof(pwm_data));
	
	HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
	
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
	
#if 1
	for (i = 0; i < STUDY_TIME; ++i) {
		krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
		now_time = krhino_sys_time_get();
		if (last_rcv_time != 0 && now_time - last_rcv_time > 1000)
			break;
	}
#else
	krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND * STUDY_TIME);
#endif
	
	HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_1);
	HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_2);
	
	HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
	
	if (rcv_count < MIN_VAID_CODE || rcv_count >= MAX_TIME_DATA) {
		KIDS_A10_PRT("Receive data is invalid!\n");
		return -1;
	}
	
	if (rcv_data[rcv_count].time_data_s != 0){
		++rcv_count;
	}
	
	ret = irda_classify_feature_code();
	if (ret != 0)
		return -1;
	
	ret = irda_get_pwm_data();
	if (ret != 0)
		return -1;
	
	resend_flag = 1;
	
	return 0;
}

static int reconfig_pwm_basic_time(uint32_t div)
{
	TIM_OC_InitTypeDef sConfigOC;
	uint32_t period = 80 * div;

	htim16.Init.Period = period;
	if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
	{
		return -1;
	}

	if (HAL_TIM_OC_Init(&htim16) != HAL_OK)
	{
		return -1;
	}

	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	sConfigOC.Pulse = period;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_OC_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		return -1;
	}

	return 0;
}

static void fill_once(uint32_t pwm_index, uint32_t pwm_width)
{
	uint32_t i;
	uint32_t byte_index = 0;
	uint32_t bit_index = 0;
	uint32_t width_left = 0;
	uint32_t width_bytes = 0;
	uint32_t width_extra = 0;
	
	byte_index = pwm_index / 8;
	bit_index = pwm_index % 8;
	
	if (pwm_width > (8 - bit_index)) {
		width_left = 8 - bit_index;
		pwm_width -= width_left;
		width_bytes = pwm_width / 8;
		width_extra = pwm_width % 8;
		pwm_send_buffer[byte_index] |= (1 << width_left) - 1 << bit_index;
		for (i = 1; i <= width_bytes; ++i) {
			pwm_send_buffer[byte_index + i] = 0xff;
		}
		if (width_extra != 0) {
			pwm_send_buffer[byte_index + width_bytes + 1] |= (1 << width_extra) - 1;
		}
	} else {
		pwm_send_buffer[byte_index] |= (1 << pwm_width) - 1 << bit_index;
	}
}

static int irda_fill_send_buffer(void)
{
	int ret = 0;
	int i;
	uint32_t total_period = 0;
	uint32_t alloc_bytes = 0;
	uint32_t pwm_index = 0;
	
	for (i = lead_data_hit; i < rcv_count; ++i) {
		total_period += pwm_data[rcv_data[i].pwm_data_s];
		total_period += pwm_data[rcv_data[i].pwm_data_e];
	}
	pwm_send_max = total_period;
	alloc_bytes = total_period / 8;
	alloc_bytes += (total_period % 8 == 0 ? 0 : 1);
	
	pwm_send_buffer = (uint8_t *)aos_malloc(alloc_bytes);
	if (pwm_send_buffer == NULL) {
		KIDS_A10_PRT("aos_malloc return NULL.\n");
		return -1;
	}
	memset(pwm_send_buffer, 0, alloc_bytes);
	
	for (i = lead_data_hit; i < rcv_count; ++i) {
		fill_once(pwm_index, pwm_data[rcv_data[i].pwm_data_s]);
		pwm_index += pwm_data[rcv_data[i].pwm_data_s];
		pwm_index += pwm_data[rcv_data[i].pwm_data_e];
	}
	
	return 0;
}

int irda_send_code(void)
{
	int ret = 0;
	
	if (pwm_div == 0) {
		KIDS_A10_PRT("Please study code before.\n");
		return -1;
	}
	if (pwm_send_buffer != NULL || lead_data_hit == -1 || !aos_sem_is_valid(&irda_sem)) {
		KIDS_A10_PRT("unknow error.\n");
		return -1;
	}
	
	ret = irda_fill_send_buffer();
	if (ret != 0) {
		ret = -1;
		goto send_exit;
	}
	
	if (resend_flag) {
		ret = reconfig_pwm_basic_time(pwm_div);
		if (ret != 0) {
			KIDS_A10_PRT("reconfig_pwm_basic_time return failed.\n");
			ret = -1;
			goto send_exit;
		}
	}
	
	pwm_send_counter = 0;
	
	HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
	
	/* Reset the counter to ensure accurate timing */
  __HAL_TIM_SET_COUNTER(&htim16, 0);

  /* TIM IT Enable */
  HAL_TIM_Base_Start_IT(&htim16);
	
	ret = aos_sem_wait(&irda_sem, SEND_TIME * 1000);
	if (ret != 0) {
		KIDS_A10_PRT("IR send data failed.\n");
		ret = -1;
		goto send_exit;
	}
	
	resend_flag = 0;
	
send_exit:
	HAL_TIM_Base_Stop_IT(&htim16);
	
	HAL_NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
	
	if (pwm_send_buffer) {
		aos_free(pwm_send_buffer);
		pwm_send_buffer = NULL;
	}
	
	return ret;
}

int irda_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	int ret = 0;
	
	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin : PB9 */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
#ifndef IR_SIG_REVERSE
	GPIO_InitStruct.Alternate = GPIO_AF1_IR;
#else
	GPIO_InitStruct.Alternate = GPIO_AF14_TIM17;
#endif
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	__HAL_TIM_URS_ENABLE(&htim1);
	__HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_FLAG_UPDATE | TIM_IT_CC1 | TIM_IT_CC2);
	__HAL_TIM_ENABLE(&htim1);
	
	/* Setting also the opposing polarity */
  TIM_SET_CAPTUREPOLARITY(&htim17, TIM_CHANNEL_1, TIM_CCxN_ENABLE | TIM_CCx_ENABLE );

#ifndef IR_SIG_REVERSE
  /* Start the modulation frequency */
  HAL_TIM_OC_Start(&htim17, TIM_CHANNEL_1);
#endif
	
	/* Setting also the opposing polarity */
  TIM_SET_CAPTUREPOLARITY(&htim16, TIM_CHANNEL_1, TIM_CCxN_ENABLE | TIM_CCx_ENABLE );
	
  /* TIM16 Main Output Enable */
  __HAL_TIM_MOE_ENABLE(&htim16);
	
  /* TIM Disable */
  __HAL_TIM_DISABLE(&htim16);
	
	ret = aos_sem_new(&irda_sem, 0);
	if (ret != 0) {
		KIDS_A10_PRT("aos_sem_new return failed.\n");
		return -1;
	}
	
	resend_flag = 1;
	
	return 0;
}
