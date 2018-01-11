#ifndef __LTR553_H
#define __LTR553_H

#define LTR553_ALS_CONTR            0x80 /* ALS operation mode, SW reset */
#define LTR553_PS_CONTR             0x81 /* PS operation mode */
#define LTR553_PS_LED               0x82 /* LED pulse freq, current duty, peak current */
#define LTR553_PS_MEAS_RATE         0x84 /* measurement rate*/
#define LTR553_ALS_MEAS_RATE        0x85 /* ALS integ time, measurement rate*/
#define LTR553_PART_ID              0x86
#define LTR553_MANUFAC_ID           0x87
#define LTR553_ALS_DATA1_L          0x88
#define LTR553_ALS_DATA1_H          0x89
#define LTR553_ALS_DATA0_L          0x8a
#define LTR553_ALS_DATA0_H          0x8b
#define LTR553_ALS_PS_STATUS        0x8c
#define LTR553_PS_DATA_L            0x8d
#define LTR553_PS_DATA_H            0x8e
#define LTR553_INTR                 0x8f /* output mode, polarity, mode */
#define LTR553_PS_THRESH_UP         0x90 /* 11 bit, ps upper threshold */
#define LTR553_PS_THRESH_LOW        0x92 /* 11 bit, ps lower threshold */
#define LTR553_ALS_THRESH_UP        0x97 /* 16 bit, ALS upper threshold */
#define LTR553_ALS_THRESH_LOW       0x99 /* 16 bit, ALS lower threshold */
#define LTR553_INTR_PRST            0x9e /* ps thresh, als thresh */
#define LTR553_MAX_REG              0x9f

typedef enum {
	FLAG_ALS,
	FLAG_PS,
	FLAG_BOTH
} FLAG_ALS_PS;

typedef enum {
	AG_GAIN_1X = 0x0,                 /* 1 lux to 64k lux (default) */
	AG_GAIN_2X = 0x1,                 /* 0.5 lux to 32 lux */
	AG_GAIN_4X = 0x2,                 /* 0.25 lux to 16k lux */
	AG_GAIN_8X = 0x3,                 /* 0.125 lux to 8k lux */
	AG_GAIN_48X = 0x6,                /* 0.02 lux to 1.3k lux */
	AG_GAIN_96X = 0x7,                /* 0.01 lux to 600 lux */
} CFG_ALS_Gain;

typedef enum {
	PG_GAIN_X16 = 0x0,                /* X16 (default) */
	PG_GAIN_X32 = 0x2,                /* X32 */
	PG_GAIN_X64 = 0x3,                /* X64 */
} CFG_PS_Gain;

typedef enum {
	LPMF_PERIOD_30K = 0x0,            /* LED pulse period = 30kHz */
	LPMF_PERIOD_40K = 0x1,            /* LED pulse period = 40kHz */
	LPMF_PERIOD_50K = 0x2,            /* LED pulse period = 50kHz */
	LPMF_PERIOD_60K = 0x3,            /* LED pulse period = 60kHz(default) */
	LPMF_PERIOD_70K = 0x4,            /* LED pulse period = 70kHz */
	LPMF_PERIOD_80K = 0x5,            /* LED pulse period = 80kHz */
	LPMF_PERIOD_90K = 0x6,            /* LED pulse period = 90kHz */
	LPMF_PERIOD_100K = 0x7,           /* LED pulse period = 100kHz */
} CFG_LED_pulse_Modulation_Frequency;

typedef enum {
	LCD_PER_25 = 0x0,                 /* DUTY = 25% */
	LCD_PER_50 = 0x1,                 /* DUTY = 50% */
	LCD_PER_75 = 0x2,                 /* DUTY = 75% */
	LCD_PER_100 = 0x3,                /* DUTY = 100%(default) */
} CFG_LED_Current_DUTY;

typedef enum {
	LC_LEVEL_5 = 0x0,                 /* LED pulse current level = 5mA */
	LC_LEVEL_10 = 0x1,                /* LED pulse current level = 10mA */
	LC_LEVEL_20 = 0x2,                /* LED pulse current level = 20mA */
	LC_LEVEL_50 = 0x3,                /* LED pulse current level = 50mA */
	LC_LEVEL_100 = 0x4,               /* LED pulse current level = 100mA(default) */
} CFG_LED_Current;

typedef enum {
	PMR_RATE_50 = 0x0,                /* PS Measurement Repeat Rate = 50ms */
	PMR_RATE_70 = 0x1,                /* PS Measurement Repeat Rate = 70ms */
	PMR_RATE_100 = 0x2,               /* PS Measurement Repeat Rate = 100ms(default) */
	PMR_RATE_200 = 0x3,               /* PS Measurement Repeat Rate = 200ms */
	PMR_RATE_500 = 0x4,               /* PS Measurement Repeat Rate = 500ms */
	PMR_RATE_1000 = 0x5,              /* PS Measurement Repeat Rate = 1000ms */
	PMR_RATE_2000 = 0x6,              /* PS Measurement Repeat Rate = 2000ms */
	PMR_RATE_10 = 0x8,                /* PS Measurement Repeat Rate = 10ms */
} CFG_PS_measurement_rate;

typedef enum {
	AIT_TIME_100 = 0x0,               /* ALS integration_time = 100ms(default) */
	AIT_TIME_50 = 0x1,                /* ALS integration_time = 50ms */
	AIT_TIME_200 = 0x2,               /* ALS integration_time = 200ms */
	AIT_TIME_400 = 0x3,               /* ALS integration_time = 400ms */
	AIT_TIME_150 = 0x4,               /* ALS integration_time = 150ms */
	AIT_TIME_250 = 0x5,               /* ALS integration_time = 250ms */
	AIT_TIME_300 = 0x6,               /* ALS integration_time = 300ms */
	AIT_TIME_350 = 0x7,               /* ALS integration_time = 350ms */
} CFG_ALS_integration_time;

typedef enum {
	AMR_RATE_50 = 0x0,                /* ALS Measurement Repeat Rate = 50ms */
	AMR_RATE_100 = 0x1,               /* ALS Measurement Repeat Rate = 100ms */
	AMR_RATE_200 = 0x2,               /* ALS Measurement Repeat Rate = 200ms */
	AMR_RATE_500 = 0x3,               /* ALS Measurement Repeat Rate = 500ms(default) */
	AMR_RATE_1000 = 0x4,              /* ALS Measurement Repeat Rate = 1000ms */
	AMR_RATE_2000 = 0x5,              /* ALS Measurement Repeat Rate = 2000ms */
} CFG_ALS_measurement_rate;

int ltr553_get_adc_als_data(uint16_t *ch0, uint16_t *ch1);
int ltr553_get_adc_ps_data(uint16_t *data);
int ltr553_init(FLAG_ALS_PS flag);

#endif /* __LTR553_H */
