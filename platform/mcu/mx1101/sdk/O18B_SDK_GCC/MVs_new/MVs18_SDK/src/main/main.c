#if 0
#include "type.h"
#include "app_config.h"
#include "os.h"
#include "msgq.h"
#include "app_msg.h"
#include "uart.h"
#include "key.h"
#include "clk.h"
#include "spi_flash.h"
#include "cache.h"
#include "gpio.h"
#include "dac.h"
#include "audio_adc.h"
#include "wakeup.h"
#include "timer.h"
#include "sys_app.h"
#include "power_management.h"
#include "rtc.h"
#include "adc.h"
#include "ir.h"
#include "host_hcd.h"
#include "host_stor.h"
#include "watchdog.h"
#include "mixer.h"
#include "breakpoint.h"
#include "dev_detect_driver.h"
#include "sys_vol.h"
#include "eq.h"
#include "lcd_seg.h"
#include "eq_params.h"
#include "sound_remind.h"
#include "sw_uart.h"
#include "debug.h"
#include "bt_stack_api.h"
#include "audio_decoder.h"
#include "get_bootup_info.h"
#include "fsinfo.h"
#include "sys_vol.h"
#include "micro_adjust.h"
#include "fat_file.h"
#include "task_decoder.h"
#include "presearch.h"
#include "delay.h"
int32_t BtTaskHandle = 0;
int32_t MainTaskHandle = 0;
xTaskHandle AudioProcessHandle;
		

#define     REG_RST_WDG_SOFT_KEY1          (*(volatile unsigned long*)0x4002010C)
#define     REG_RST_WDG_SOFT_DISABLE_KEY2  (*(volatile unsigned long*)0x40020114)


#define     REG_GPIO_B_REG_I               (*(volatile unsigned long*)0x40002028)
#define     REG_GPIO_B_REG_O               (*(volatile unsigned long*)0x4000202C)

#define     REG_GPIO_B_REG_IE              (*(volatile unsigned long*)0x40002030)
#define     REG_GPIO_B_REG_OE              (*(volatile unsigned long*)0x40002034)
#define     REG_GPIO_B_REG_DS              (*(volatile unsigned long*)0x40002038)
#define     REG_GPIO_B_REG_OUTDS           (*(volatile unsigned long*)0x4000203C)
#define     REG_GPIO_B_REG_PU              (*(volatile unsigned long*)0x40002040)
#define     REG_GPIO_B_REG_PD              (*(volatile unsigned long*)0x40002044)
#define     REG_GPIO_B_REG_PULLDOWN1       (*(volatile unsigned long*)0x40002048)
#define     REG_GPIO_B_REG_PULLDOWN2       (*(volatile unsigned long*)0x4000204C)
int val;
volatile int val2;
int32_t main(void)
{

   //val = 0x12345678;
	GpioSetRegBits(GPIO_B_OE,GPIOB1);
	GpioClrRegBits(GPIO_B_IE,GPIOB1);
	//REG_GPIO_B_REG_OE = 0xFFFFFFFF;
	//REG_GPIO_B_REG_IE = 0;
	while(1)
	{
		//REG_RST_WDG_SOFT_KEY1 = 1;
		//REG_RST_WDG_SOFT_DISABLE_KEY2 = 1;
		//REG_GPIO_B_REG_O = 0xFFFFFFFF;
		//REG_GPIO_B_REG_O = 0;
		WdgDis();
		GpioSetRegBits(GPIO_B_OUT,GPIOB1);
		GpioClrRegBits(GPIO_B_OUT,GPIOB1);
	}
}

#endif





#include "type.h"
#include "app_config.h"
#include "os.h"
#include "msgq.h"
#include "app_msg.h"
#include "uart.h"
#include "key.h"
#include "clk.h"
#include "spi_flash.h"
#include "cache.h"
#include "gpio.h"
#include "dac.h"
#include "audio_adc.h"
#include "wakeup.h"
#include "timer.h"
#include "sys_app.h"
#include "power_management.h"
#include "rtc.h"
#include "adc.h"
#include "ir.h"
#include "host_hcd.h"
#include "host_stor.h"
#include "watchdog.h"
#include "mixer.h"
#include "breakpoint.h"
#include "dev_detect_driver.h"
#include "sys_vol.h"
#include "eq.h"
#include "lcd_seg.h"
#include "eq_params.h"
#include "sound_remind.h"
#include "sw_uart.h"
#include "debug.h"
#include "bt_stack_api.h"
#include "audio_decoder.h"
#include "get_bootup_info.h"
#include "fsinfo.h"
#include "sys_vol.h"
#include "micro_adjust.h"
#include "fat_file.h"
#include "task_decoder.h"
#include "presearch.h"
#include "delay.h"
int32_t BtTaskHandle = 0;
uint32_t RecvTaskHandle = 0;
uint32_t SendTaskHandle = 0;
uint32_t   MainTaskHandle = 0;
xTaskHandle AudioProcessHandle;

#ifdef FUNC_SPI_UPDATE_EN
extern void BootUpgradeChk(void);
#endif
extern void AudioProcessTaskEntrance(void);

//extern uint8_t UpgradeFileFound;

extern void DetectMassTestCondition(void);
extern bool GetMassTestFlag(void);

extern bool FlshBTInfoAreaInit(void);

__attribute__((section(".driver.isr"))) void Timer1Interrupt(void)
{
	Timer1IntClr();
#if (defined(FUNC_USB_EN) || defined(FUNC_USB_AUDIO_EN) || defined(FUNC_USB_READER_EN) || defined(FUNC_USB_AUDIO_READER_EN))
	OTGLinkCheck();
#endif
	SystemPowerOffDetect();
}
DECLARE_SEMAPHORE(xMutex,1);
void RecvTask(void)
{
unsigned i,Msg;

MsgAddSet(MSG_DEV_CLASS);
MsgAddSet(MSG_BT_CLASS);

//OSDOWN(xMutex);
while(1)
{
   //Msg = (uint16_t)OSQueueMsgRecv(NULL, NULL, 0xFFFFFFFF);
   //APP_DBG("\n msg=0x%x",Msg);

   //schedule_timeout(100);
   //APP_DBG("\nRecvTask");

   OSDOWN(xMutex);
  // __OSDOWN_TIMEOUT(&xMutex,0xFFFF);

  // APP_DBG("\nRecvTask");
}
}

void SendTask(void)
{
	unsigned i=0,j;
	//__OSDOWN_TIMEOUT(&xMutex, 5);
	while(1)
	{
	 //for(i=0;i<10000;i++);
	 //APP_DBG("\nSendTask");
	 OSUP(xMutex);

    //i++;
    //j=i%10;
	//OSQueueMsgSend(0x1107+j, NULL, 0, 1, 0);
	}
}

int val;
volatile int val2;
int32_t main(void)
{
	extern void GuiTaskEntrance(void);
//	extern void DecoderTaskEntrance(void);
	extern void BTEntrance(void);
	extern void ShellCommand(void);
   val = 0x12345678;
#ifdef FUNC_5VIN_TRIM_LDO3V3   	
	SarAdcTrimLdo3V3();   //attention! only used in Power = 5V
#endif
		
	ClkModuleEn(ALL_MODULE_CLK_SWITCH);	
	ClkModuleGateEn(ALL_MODULE_CLK_GATE_SWITCH);        //open all clk gating
	CacheInit();

//	SysGotoPowerDown();
	val2 = 0x1234cccc;
	SysGetWakeUpFlag(); //get wake up flag, DO NOT remove this!!

#ifdef USE_POWERKEY_SLIDE_SWITCH
	SysPowerKeyInit(POWERKEY_MODE_SLIDE_SWITCH, 500);//500ms
#endif
#ifdef USE_POWERKEY_SOFT_PUSH_BUTTON
	SysPowerKeyInit(POWERKEY_MODE_PUSH_BUTTON, 2000); //2s
#endif

	SpiFlashInfoInit();		//Flash RD/WR/ER/LOCK initialization
	
	ClkPorRcToDpll(0); 		//clock src is 32768hz OSC
	
	//GD flash，选择SYS CLK或DPLL80Mhz，可以开启HPM，其他型号建议关闭HPM
	//SpiFlashClkSet(FLASHCLK_SYSCLK_SEL, TRUE);
	
#ifdef FUNC_RADIO_DIV_CLK_EN
	ClkDpllClkGatingEn(1);
#endif
	SarAdcLdoinVolInit();    //do NOT remove this
	//LcdCtrlRegInit();        //do NOT remove this
	/*
	 * Now, uart has been initialized automatically by OS.  print
	 * if you want to customize debug uart setting(defaut 115200-8-N-1),
	 * you can invoke the function at any place you want(for example here).
	 */
#ifdef FUNC_DEBUG_EN
//	SwUartTxInit(SWUART_GPIO_PORT_A, 24, 115200);
//	EnableSwUartAsFuart(TRUE);

	OsSetDebugFlag(1);
	GpioFuartRxIoConfig(FUART_RX_PORT);
	GpioFuartTxIoConfig(FUART_TX_PORT);
	OSDBGUartInit(115200, 8, 0, 1);
#else
	OsSetDebugFlag(0);
#endif

	//SysTickInit();

	WdgDis();



#if (!defined(FUNC_USB_AUDIO_EN) && !defined(FUNC_USB_READER_EN) && !defined(FUNC_USB_AUDIO_READER_EN))
#undef PC_PORT_NUM
#define PC_PORT_NUM    0
#endif

#ifdef FUNC_SPI_UPDATE_EN
	BootUpgradeChk();	//boot upgrade check upgrade for the last time
#endif

	Osc32kExtCapCalibrate();//32KHz external oscillator calibration


#ifdef FUNC_BREAKPOINT_EN
	BP_LoadInfo();// 检查、装载（如果第一次上电）基本配置信息，DO NOT remove this code!!!
#endif

	OSStartKernel();                //start initialize os kernel

#ifdef MASS_PRODUCTION_TEST_FUNC
	DetectMassTestCondition();
#endif

	WdgDis();

	//OSTaskCreate(SendTask, "SendTask", 1280, NULL, 2, &SendTaskHandle);
	//OSTaskCreate(RecvTask, "RecvTask", 1280, NULL, 2, &RecvTaskHandle);
	//OSTaskCreate(ShellCommand, "SHELL", 720, NULL, 3, NULL);

	OSStartSchedule();              //Start the scheduler.

	OSTaskPrioSet(NULL, 3);

	OSTaskCreate(RecvTask, "RecvTask", 1280, NULL, 2, &RecvTaskHandle);
	OSTaskCreate(SendTask, "SendTask", 1280, NULL, 1, &SendTaskHandle);
	OSTaskCreate(ShellCommand, "SHELL", 720, NULL, 3, NULL);

	//OSTaskCreate(FUARTtask, "FUARTtask", 720, NULL, 3, NULL);

	while(1)
	{
		APP_DBG("MainTask\r\n");

		//OSQueueMsgSend(0x1478, NULL, 0, 1, 0);

		OSRescheduleTimeout(0xFFFF);

	}
}
