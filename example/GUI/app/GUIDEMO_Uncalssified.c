#include "GUIDEMO.h"
#include <stdint.h>
#include "stm32l4xx_hal.h"
#include <aos/uData.h>

#if (SHOW_GUIDEMO_UNCLASSIFIED)

#define Y_START          43
#define Y_STEP           10
#define DEC_LEN_DEF      5

#define GUIDEMO_UNCLASSIFIED_OFFSET 20
/*
static void _cbDesktop(WM_MESSAGE * pMsg)
{
  switch (pMsg->MsgId)
  {
    case GUI_KEY_FUNC_A:
    case GUI_KEY_FUNC_B:
      GUIDEMO_Intro();
	    GUIDEMO_Delay(5000);
      break;

    default:
      break;
  }

}
*/

static int fd_acc  = -1;
static int fd_baro = -1;
static int fd_temp = -1;
static int fd_humi = -1;
static int fd_als  = -1;
static int fd_ps   = -1;

extern int key_flag;

static int sensor_all_open(void)
{
	int fd = -1;

	fd = aos_open(dev_acc_path, O_RDWR);
	if (fd < 0) {
		KIDS_A10_PRT("Error: aos_open return %d.\n", fd);
		return -1;
	}
	fd_acc = fd;

	fd = aos_open(dev_baro_path, O_RDWR);
	if (fd < 0) {
		KIDS_A10_PRT("Error: aos_open return %d.\n", fd);
		return -1;
	}
	fd_baro = fd;

	fd = aos_open(dev_temp_path, O_RDWR);
	if (fd < 0) {
		KIDS_A10_PRT("Error: aos_open return %d.\n", fd);
		return -1;
	}
	fd_temp = fd;

	fd = aos_open(dev_humi_path, O_RDWR);
	if (fd < 0) {
		KIDS_A10_PRT("Error: aos_open return %d.\n", fd);
		return -1;
	}
	fd_humi = fd;

	fd = aos_open(dev_als_path, O_RDWR);
	if (fd < 0) {
		KIDS_A10_PRT("Error: aos_open return %d.\n", fd);
		return -1;
	}
	fd_als = fd;

	fd = aos_open(dev_ps_path, O_RDWR);
	if (fd < 0) {
		KIDS_A10_PRT("Error: aos_open return %d.\n", fd);
		return -1;
	}
	fd_ps = fd;

	return 0;
}

static int get_acc_data(int32_t *x, int32_t *y, int32_t *z)
{
	accel_data_t data = {0};
	ssize_t size = 0;

	size = aos_read(fd_acc, &data, sizeof(data));
	if (size != sizeof(data)) {
		KIDS_A10_PRT("aos_read return error.\n");
		return -1;
	}
	*x = data.data[0];
	*y = data.data[1];
	*z = data.data[2];

	return 0;
}

static int get_baro_data(uint32_t *pa)
{
	barometer_data_t data = {0};
	ssize_t size = 0;

	size = aos_read(fd_baro, &data, sizeof(data));
	if (size != sizeof(data)) {
		KIDS_A10_PRT("aos_read return error.\n");
		return -1;
	}
	*pa = data.p;

	return 0;
}

static int get_temp_data(int32_t *celsius)
{
	temperature_data_t data = {0};
	ssize_t size = 0;

	size = aos_read(fd_temp, &data, sizeof(data));
	if (size != sizeof(data)) {
		KIDS_A10_PRT("aos_read return error.\n");
		return -1;
	}
	*celsius = data.t;

	return 0;
}

static int get_humi_data(uint32_t *pecent)
{
	humidity_data_t data = {0};
	ssize_t size = 0;

	size = aos_read(fd_humi, &data, sizeof(data));
	if (size != sizeof(data)) {
		KIDS_A10_PRT("aos_read return error.\n");
		return -1;
	}
	*pecent = data.h;

	return 0;
}

static int get_als_data(uint32_t *lux)
{
	als_data_t data = {0};
	ssize_t size = 0;

	size = aos_read(fd_als, &data, sizeof(data));
	if (size != sizeof(data)) {
		KIDS_A10_PRT("aos_read return error.\n");
		return -1;
	}
	*lux = data.lux;

	return 0;
}

static int get_ps_data(uint32_t *present)
{
	proximity_data_t data = {0};
	ssize_t size = 0;

	size = aos_read(fd_ps, &data, sizeof(data));
	if (size != sizeof(data)) {
		KIDS_A10_PRT("aos_read return error.\n");
		return -1;
	}
	*present = data.present;

	return 0;
}

void GUIDEMO_Unclassified(void) {
  int xSize = LCD_GetXSize();
	int32_t acc_adc_data[3] = {0};
	float acc_nkg[3] = {0};
	uint32_t baro_data = 0;
	int32_t temp_data = 0;
	uint32_t humi_data = 0;
	uint32_t als_data = 0;
	uint32_t ps_data = 0;

	if (sensor_all_open() != 0)
		return;

  // set back screen black
  GUIDEMO_HideInfoWin();
  GUIDEMO_ShowControlWin();
  GUI_Exec();
  GUIDEMO_DrawBk(1);
  GUI_SetColor(GUI_BLACK);
  GUIDEMO_DrawBk(1);

  // set font
	GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font16_ASCII);

  GUI_DispStringAt("acc_x",         GUIDEMO_UNCLASSIFIED_OFFSET, Y_START);
  GUI_DispStringAt("acc_y",         GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  2);
  GUI_DispStringAt("acc_z",         GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  4);
  GUI_DispStringAt("barometer",     GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6);
  GUI_DispStringAt("temperature",   GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8);
  GUI_DispStringAt("humidity",      GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10);
  GUI_DispStringAt("als",           GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  12);
	GUI_DispStringAt("proximity",     GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  14);


  // GUI_HWIN hWnd;

  do{
      // print value
      // GUI_SetOrientation(GUI_SWAP_XY);

   //   hWnd = WM_CreateWindow(10, 10, 200, 200, WM_CF_HIDE, _cbDesktop, 0);
   //   HAL_Delay(1000);
   //   WM_DeleteWindow(hWnd);

      // print value type

      if (!get_acc_data(&acc_adc_data[0], &acc_adc_data[1], &acc_adc_data[2])) {
				acc_nkg[0] = (float)acc_adc_data[0] * 9.8 / 1024;
				acc_nkg[1] = (float)acc_adc_data[1] * 9.8 / 1024;
				acc_nkg[2] = (float)acc_adc_data[2] * 9.8 / 1024;
				GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  0);
				GUI_DispFloatFix(acc_nkg[0], 7, 3);
				GUI_DispString("N/kg");
				GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  2);
				GUI_DispFloatFix(acc_nkg[1], 7, 3);
				GUI_DispString("N/kg");
				GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  4);
				GUI_DispFloatFix(acc_nkg[2], 7, 3);
				GUI_DispString("N/kg");
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  0);
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  2);
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  4);
      }

      if (!get_baro_data(&baro_data)) {
        GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6);
        GUI_DispDec(baro_data, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6);
      }

      if (!get_temp_data(&temp_data)) {
        GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8);
        GUI_DispDec(temp_data, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8);
      }
			
			if (!get_humi_data(&humi_data)) {
        GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10);
        GUI_DispDec(humi_data, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10);
      }
			
			if (!get_als_data(&als_data)) {
        GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  12);
        GUI_DispDec(als_data, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  12);
      }
			
			if (!get_ps_data(&ps_data)) {
        GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  14);
        GUI_DispDec(ps_data, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  14);
      }

      if (key_flag == 1)
        break;

      krhino_task_sleep(krhino_ms_to_ticks(400));

      if (key_flag == 1)
        break;

      krhino_task_sleep(krhino_ms_to_ticks(300));

      if (key_flag == 1)
        break;

      krhino_task_sleep(krhino_ms_to_ticks(300));
   //   WM_SetCallback(WM_HBKWIN, _cbDesktop);

  }while(1);

}

#else

void GUIDEMO_Unclassified(void) {}

#endif

