/**
 *******************************************************************************
 * @file    debug_test.c
 * @author  Richard
 * @version V1.0.0
 * @date    17-12-2013
 * @maintainer Halley
 * @brief   debug
 *******************************************************************************
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

#include "type.h"
#include "os.h"
#include "debug.h"
#include "app_msg.h"
#include "rtc.h"
#include "wakeup.h"
#include "uart.h"
#include "sys_app.h"

#ifdef CFG_SHELL_DEBUG

volatile unsigned long  IntCnt = 0,IntCnt1 = 0,IntCnt2 = 0,IntCnt3 = 0;
__attribute__((section(".driver.isr")))
void FuartInterrupt(void)
{
	int status;
	/*
	 * take reception proirity over transmittion
	 */
	IntCnt++;
    status = FuartIOctl(UART_IOCTL_RXSTAT_GET,0);
	if(status & 0x1E)
	{
		/*
		 * tansfer error occurs if,give some hints
		 */
		if(status & 0x02)
		{
			DBG("Fuart RX error interrupt\r\n");
		}
		if(status & 0x04)
		{
			DBG("Fuart RX error overrun\r\n");
		}
		if(status & 0x08)
		{
			DBG("Fuart RX error Parity\r\n");
		}
		if(status & 0x10)
		{
			DBG("Fuart RX error Frame\r\n");
		}
		/*
		 * clear FIFO before clear other flags
		 */
		FuartIOctl(UART_IOCTL_RXFIFO_CLR,0);
		/*
		 * clear other error flags
		 */
		FuartIOctl(UART_IOCTL_RXINT_CLR,0);
	}
	//else
	if(status & 0x01)
	{
		/*
		 * notify the shell task to take preparation for datum coming
		 */

		IntCnt1 = *(unsigned long*)0xE000E018;
		OSQueueMsgSend(MSGDEV_FUART_DATRDY, NULL, 0, MSGPRIO_LEVEL_LO, 0);
		IntCnt2 = *(unsigned long*)0xE000E018;
		IntCnt3 = IntCnt1 - IntCnt2;
		FuartIOctl(UART_IOCTL_RXINT_CLR,0);
	}
	//else
	if(FuartIOctl(UART_IOCTL_TXSTAT_GET,0) & 0x01)
	{
		/*
		 * TO DO
		 */
		FuartIOctl(UART_IOCTL_TXINT_CLR,0);
	}
}

//for adc or other input device simulation from shell cmd
int sim_adc(int argc, char* argv[])
{
	DBG("%d\n", argc);
	if(argc < 2)
	{
		DBG("simulate one adc key command:\n"
		     "'1':SYS_KEY_ID_CHANGE_MODE;\n"
		     "'2':Next Song;\n"
		     "'3':PreSong\n"
		     "'4':V-\n"
		     "'5'V+\n"
		     "'6':Play/Pause\n"
		     "'7':phone nuber replay\n"
		     "'s':goto sleep\n"
		     "'r':reboot\n"
		     "Usage:sim_adc 1,2,3,...)");

		return 0;
	}

	switch(argv[1][0])
	{
		case '1':
			MsgSend(MSG_MODE);
			break;

		case '2':
			MsgSend(MSG_NEXT);
			break;

		case '3':
			MsgSend(MSG_PRE);
			break;

		case '4':
			MsgSend(MSG_VOL_DW);
			break;

		case '5':
			MsgSend(MSG_VOL_UP);
			break;

		case '6':
			MsgSend(MSG_PLAY_PAUSE);
			break;

		case '9': // set alarm after 1min
		/*
			{
				RTC_DATE_TIME DataTime;// = {2013, 8, 6, 0, 11, 00, 0};
				RtcGetCurrTime(&DataTime);

				if(DataTime.Min < 59)
				{
					DataTime.Min += 1;
				}
				else
				{
					DataTime.Min = 0;
					DataTime.Hour++;
					DataTime.Hour %= 60;
				}
				APP_DBG("Alarm Time:%d-%d-%d %d:%d:%d Week:%d",
				        DataTime.Year, DataTime.Mon, DataTime.Date,
				        DataTime.Hour, DataTime.Min, DataTime.Sec, DataTime.WDay);
				RtcSetAlarmTime(1, ALARM_MODE_ONCE_ONLY, &DataTime);
				RtcAlarmSetStatus(1, ALARM_STATUS_OPENED);
			}
		*/
			break;
		case 'a':
		/*
			{
				RTC_DATE_TIME DataTime = {2013, 8, 6, 0, 10, 55, 0};
				RtcSetCurrTime(&DataTime);
				RtcGetCurrTime(&DataTime);
				APP_DBG("Set Time:%d-%d-%d %d:%d:%d Week:%d",
				        DataTime.Year, DataTime.Mon, DataTime.Date,
				        DataTime.Hour, DataTime.Min, DataTime.Sec, DataTime.WDay);
			}
		*/
			break;

		case 's':
			SysSetWakeUpSrcInDeepSleep(WAKEUP_SRC_SLEEP_B7, WAKEUP_POLAR_B7_LOW, 0);
//            SysGotoDeepSleepCfg();
			SysGotoDeepSleep();
			break;

		case 'c':
			SysPowerKeyInit(POWERKEY_MODE_SLIDE_SWITCH, 1);
			SysGotoPowerDown();
			break;

		case 'r':
			NVIC_SystemReset();
			break;
		case 'z':
			argv[100][1024] = 0xdf;
			break;
		case 'x'://rtc power up
		/*
			{
				RTC_DATE_TIME CurTime;
				CurTime.Year = 2013;
				CurTime.Mon = 1;
				CurTime.Date = 1;
				CurTime.Hour = 1;
				CurTime.Min  = 1;
				CurTime.Sec  = 1;

				RtcSetCurrTime(&CurTime);

				CurTime.Sec  = 5;

				RtcSetAlarmTime(2, ALARM_MODE_ONCE_ONLY, &CurTime);
				RtcAlarmSetStatus(2, ALARM_STATUS_OPENED);

				SysSetWakeUpSrcInDeepSleep(WAKEUP_SRC_SLEEP_RTC, 0, 0);
				SysGotoDeepSleepCfg();
				SysGotoDeepSleep();

			}
		*/
			break;
		default:
			break;
	}
	return 0;
}
SHELL_CMD(
    sim_adc, 0, sim_adc,
    "simulate one adc key command\r\n",
    "\r\n"
    "\tsimulate one adc key command:\r\n"
    "\t'1':SYS_KEY_ID_CHANGE_MODE;\r\n"
    "\t'2':Next Song;\r\n"
    "\t'3':PreSong\r\n"
    "\t'4':V-\r\n"
    "\t'5'V+\r\n"
    "\t'6':Play/Pause\r\n"
    "\t'7':phone nuber replay\r\n"
    "\t'8':get cur time\r\n"
    "\t'9':set alarm after 1 min\r\n"
    "\tUsage:sim_adc 1,2,3,...\r\n"
);
			
int DbgUartSend(char* Buf, int BufLen)
{
	return FuartSend((unsigned char*)Buf,BufLen);
}

int DbgUuartRecv(char* Buf, int BufLen, unsigned int Wait)
{
	if(MSGDEV_FUART_DATRDY == OSQueueMsgRecv(NULL, NULL, Wait))
		return FuartRecv((unsigned char*)Buf, BufLen,0);
	return -1;		
}
	
void ShellCommand(void)
{    
    unsigned char* Argv[CFG_MAX_ARGS];
    
	MsgAddSet(MSGDEV_FUART_CLASS);
	for(;;)
	{       
        //if(MSGDEV_FUART_DATRDY == OSQueueMsgRecv(NULL, NULL, ~0UL))
            command_parse_spawn(CFG_MAX_ARGS,(char**)Argv);
	}
}

#endif//CFG_SHELL_DEBUG

