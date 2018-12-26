/**
 *****************************************************************************
 * @file     rtc.h
 * @author   Yancy
 * @version  V1.0.0
 * @date     19-June-2013
 * @brief    rtc module driver header file
 * @maintainer: Sam
 * change log:
 *			 Modify by Sam -20140624
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

/**
 * 定义每个闹钟需占用的memory字节数,上层代码可据此以及需要支持的最大闹钟数分配memory空间
 */
#define   MEM_SIZE_PER_ALARM     6
/**
 * 定义闹钟最大数量，最多同时支持8个闹钟
 */	
#define   MAX_ALARM_NUM			2

/**
 * 闹钟状态定义
 */
#define ALARM_STATUS_OPENED      0x01  /**< 打开状态   */
#define ALARM_STATUS_CLOSED      0x02  /**< 关闭状态   */
#define ALARM_STATUS_UNUSED      0x03  /**< 未使用状态 */
/**
 * 闹钟模式定义
 */
typedef enum _RTC_ALARM_MODE
{
	ALARM_MODE_ONCE_ONLY = 1,	/**< 单次闹钟 */
	ALARM_MODE_PER_DAY,			/**< 每天闹钟 */
	ALARM_MODE_PER_WEEK,		/**< 每周闹钟 */
	ALARM_MODE_WORKDAY,			/**< 工作日闹钟 */
	ALARM_MODE_USER_DEFINED,	/**< 自定义闹钟 */
	ALARM_MODE_MAX,	
} RTC_ALARM_MODE;


#pragma pack(1)	/*Ensure this structure is byte aligned, and not use padding bytes */
typedef struct _ALARM_INFO
{
	uint32_t AlarmTimeAsSeconds;
	uint8_t  AlarmStatus : 2;
	uint8_t  AlarmMode : 4;
	uint8_t  AlarmData; //闹钟周期，bit0：星期天，bit6：星期六
} ALARM_TIME_INFO;
#pragma pack()


/**
 * 阳历时间结构体定义
 */
typedef struct _RTC_DATE_TIME
{
	uint16_t	Year;  /**< 阳历年 */
	uint8_t	Mon;   /**< 阳历月 */
	uint8_t	Date;  /**< 阳历日 */
	uint8_t	WDay;  /**< 星期号，0代表周日，1~6代表周1到周6 */
	uint8_t	Hour;  /**< 小时数 */
	uint8_t	Min;   /**< 分钟数 */
	uint8_t	Sec;   /**< 秒数 */
} RTC_DATE_TIME;



/**
 * 农历时间结构体定义
 */
typedef struct _RTC_LUNAR_DATE
{
	uint16_t Year;         /**< 农历年 */
	uint8_t Month;        /**< 农历月 */
	uint8_t Date;         /**< 农历日 */
	uint8_t MonthDays;	   /**< 农历每月的天数，30 or 29*/
	bool IsLeapMonth;  /**< 是否为闰月*/
} RTC_LUNAR_DATE;


/**
 * @brief  RTC模块初始化。
 * @param  AlarmWorkMem:上层代码为闹钟功能分配的memory空间的首地址。
 * @param  AlarmWorkMemSize：上层代码为闹钟功能分配的memory空间的大小。AlarmWorkMemSize由客户希望同时支持的最多
 *         闹钟个数决定。每个闹钟需要的memory空间在rtc.h中由宏定义MEM_SIZE_PER_ALARM确定。比如客户需要最多支持
 *		   8个闹钟，那么上层分配的空间大小应为MEM_SIZE_PER_ALARM*8。上层的样例代码为：
 *		   #define MAX_ALARM_NUM 8	//最多同时支持8个闹钟
 *		   uint8_t AlarmWorkMem[MEM_SIZE_PER_ALARM * MAX_ALARM_NUM];
 *		   RtcInit(AlarmWorkMem, MEM_SIZE_PER_ALARM * MAX_ALARM_NUM);
 * @return NONE
 */
void RtcInit(uint8_t* AlarmWorkMem, uint8_t AlarmWorkMemSize);

/**
 * @brief  获取当前时间
 * @param  时间结构体指针，用于返回当前时间
 * @return NONE
 */
void RtcGetCurrTime(RTC_DATE_TIME* CurrTime);


/**
 * @brief  设置当前时间
 * @param  时间结构体指针，为要设置的时间值
 * @return NONE
 */
void RtcSetCurrTime(RTC_DATE_TIME* CurrTime);

/**
 * @brief  Get days count in the month of the year
 * @param  Year:  the year number
 * @param  Month: the month number
 * @return days count in the month of the year
 */
 uint8_t RtcGetMonthDays(uint16_t Year, uint8_t Month);

/**
 * @brief  获取某个闹钟的闹钟模式和闹钟时间
 * @param  AlarmTime：闹钟时间结构体指针，用于保存获取的闹钟时间
 * @param  AlarmMode: 闹钟模式指针，用于保存获取的闹钟模式
 * @param  ModeData: 周闹钟模式下闹钟有效的时间（周几，可以多个同时有效）
 * @param  AlarmID：闹钟号
 * @return 如果执行成功返回TRUE，否则返回FALSE。
 */
bool RtcGetAlarmTime(uint8_t AlarmID, uint8_t* AlarmMode, uint8_t* AlarmData, RTC_DATE_TIME* AlarmTime);


/**
 * @brief  设置某个闹钟的闹钟模式和闹钟时间
 * @param  AlarmTime：闹钟时间结构体指针，用于保存闹钟时间
 * @param  AlarmMode: 闹钟模式
 * @param  ModeData: 周闹钟模式下闹钟有效的时间（周几，可以多个同时有效）
 * @param  AlarmID：闹钟号
 * @return 如果执行成功返回TRUE，否则返回FALSE。
 */
bool RtcSetAlarmTime(uint8_t AlarmID, uint8_t AlarmMode, uint8_t AlarmData, RTC_DATE_TIME* AlarmTime);


/**
 * @brief  设置闹钟状态
 * @param  AlarmID：闹钟号
 * @param  AlarmStatus：要设置的闹钟状态
 * @arg        ALARM_STATUS_OPENED
 * @arg        ALARM_STATUS_CLOSED
 * @arg        ALARM_STATUS_UNUSED
 * @return 如果执行成功返回TRUE，否则返回FALSE。
 */
bool RtcAlarmSetStatus(uint8_t AlarmID, uint8_t AlarmStatus);


/**
 * @brief  获取闹钟的状态(打开/关闭/未使用)。
 * @param  AlarmID：闹钟号
 * @return 返回闹钟状态。
 *         ALARM_STATUS_OPENED
 *         ALARM_STATUS_CLOSED
 *         ALARM_STATUS_UNUSED
 */
uint8_t RtcGetAlarmStatus(uint8_t AlarmID);


/**
 * @brief  检查是否有闹钟到了。
 * @param  NONE
 * @return 返回0表示没有闹钟到。返回大于0的值表示对应的闹钟到了
 */
uint8_t RtcCheckAlarmFlag(void);

/**
 * @brief  清楚闹钟中断标志
 * @param  NONE
 * @return NONE
 */
void RtcAlarmIntClear(void);


/**
 * @brief  闹钟到时需要做的处理，上层接收到闹钟到的消息后，必须调用此函数。
 * @param  NONE
 * @return NONE
 */
void RtcAlarmArrivedProcess(void);



/**
 * @brief  阳历日期转农历日期
 * @param  DateTime:阳历时间结构体指针，作为输入参数
 * @param  LunarDate:农历时间结构体指针，作为输出参数
 * @return NONE
 */
void SolarToLunar(RTC_DATE_TIME* DateTime, RTC_LUNAR_DATE* LunarDate);

/**
 * @brief  获取农历年份的天干
 * @param  Year:农历年份
 * @return 返回0-9, 分别对应天干: {"甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸"}
 */
uint8_t GetHeavenlyStem(uint16_t Year);

/**
 * @brief  获取农历年份的地支
 * @param  Year:农历年份
 * @return 返回0-11, 分别对应地支: {"子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥"}
 *         分别对应生肖: {"鼠", "牛", "虎", "兔", "龙", "蛇", "马", "羊", "猴", "鸡", "狗", "猪"}
 */
uint8_t GetEarthlyBranch(uint16_t Year);

/**
 * @brief  返回时钟总的秒数值
 * @param  NONE
 * @return 时钟总的秒数值
 */
uint32_t RtcGetRefCnt(void);

/**
 * @brief  设置闹钟寄存器的值
 * @param  AlarmCnt：闹钟寄存器的值
 * @return NONE
 */
void RtcSetAlarmCnt(uint32_t AlarmCnt);

uint16_t RtcDayOffsetInYear(uint16_t Year, uint8_t Month, uint8_t Date);

bool RtcIsLeapYear(uint16_t Year);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif
