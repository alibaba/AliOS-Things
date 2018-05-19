#include "GUIDEMO.h"
#include <stdint.h>
#include "stm32l4xx_hal.h"
#include <aos/uData.h>
#include "audio.h"
#if 0 /* Wait ATCMD adapted to AliOS master branch */
#include "atdemo.h"
#endif

#if (SHOW_GUIDEMO_UNCLASSIFIED)

#define Y_START          43
#define Y_STEP           10
#define DEC_LEN_DEF      5

#define GUIDEMO_UNCLASSIFIED_OFFSET 20

#define MAX_GRAPH_SIZE_X 440
#define MAX_GRAPH_SIZE_Y (MAX_GRAPH_SIZE_X * 11) / 20

#define DIST_TO_BORDER   10
#define DIST_TO_WIN      5

#define BORDER_TOP       0
#define BORDER_BOTTOM    9
#define BORDER_LEFT      19
#define BORDER_RIGHT     0

#define COLOR_BK         GUI_DARKGRAY
#define COLOR_BORDER     BK_COLOR_1
#define COLOR_FRAME      GUI_BLACK
#define COLOR_GRID       GUI_GRAY

#define SCALE_H_HEIGHT   4

#define TICK_DIST_H      25
#define TICK_DIST_V      20

#define TIME_RUN         5000
#define TIME_STEP        15

#define HEARTBEAT_TIME   1000

#define MAX_NUM_DATA_OBJ 5

#define GRAPH_DIV        9   // (2^9 = 512) If this value is changed _aWaves[] need to be changed too!
#define GRID_DIST_X      25
#define GRID_DIST_Y      10
#define GRID_OFF_Y       25

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

static GRAPH_SCALE_Handle _hScaleH_sensor, _hScaleV_sensor;
// static int         _Data_Sensor_Adjust;


static GUI_COLOR _aColorData_sensor[MAX_NUM_DATA_OBJ] = {
  0x50C0FF,
  0xFFC050,
  0x50FFC0,
  0x800000,
  0x000080
};

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
#if 0
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
#endif
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

static int get_acc_data(int32_t *x, int32_t *y, int32_t *z, uint32_t *step)
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
  *step = data.step;

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

/*********************************************************************
*
*       _cbBk
*/
static void _cbBk_test(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUIDEMO_DrawBk(1);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

static void _Show_Sensor_Graph(GRAPH_Handle hGraph, GRAPH_DATA_Handle hData[])
{
  int xSize      = LCD_GetXSize();
  int Data_xSize = xSize - (DIST_TO_BORDER << 1) - (BORDER_LEFT + BORDER_RIGHT);
  int Count      = 0;

  //
  // Attach data objects
  //
  GRAPH_AttachData(hGraph, hData[0]);
  GRAPH_AttachData(hGraph, hData[1]);
  GRAPH_AttachData(hGraph, hData[2]);
  //
  // Add values before GRAPH is displayed
  //
  while (Count < Data_xSize) {
    GRAPH_DATA_YT_AddValue(hData[0], (I16)0);
    GRAPH_DATA_YT_AddValue(hData[1], (I16)0);
    GRAPH_DATA_YT_AddValue(hData[2], (I16)0);
    Count++;
  }

  sensor_all_open();

  // uint32_t lux_senser;
  // get_als_data(&lux_senser);
  int32_t x,y,z;
  uint32_t setp;
  //
  // Add values depending on time
  //
  // int TimeStart, TimeDiff, TimeStep;
  // int NextState, Flag;
  // lighten GS LED
  hal_gpio_output_low(&brd_gpio_table[GPIO_LED_GS]);

  // TimeStart = GUIDEMO_GetTime();
  int Flag = 1;
  do {
    get_acc_data(&x,&y,&z,&setp);
    // TimeDiff = GUIDEMO_GetTime() - TimeStart;
    GRAPH_DATA_YT_AddValue(hData[0], ((I16)x >> 5) + 78);
    GRAPH_DATA_YT_AddValue(hData[1], ((I16)y >> 5) + 78);
    GRAPH_DATA_YT_AddValue(hData[2], ((I16)z >> 5) + 78);

    if (Flag) {
      Flag = 0;
      GUI_Exec();
      GRAPH_DetachScale(hGraph, _hScaleH_sensor);
      GRAPH_DetachScale(hGraph, _hScaleV_sensor);
      WM_ValidateWindow(hGraph);
    }
    /*
    NextState = GUIDEMO_CheckCancel();
    TimeStep  = GUIDEMO_GetTime() - TimeStart;
    if ((TimeStep - TimeDiff) < TIME_STEP) {
      GUI_Delay(TIME_STEP - (TimeStep - TimeDiff));
    }*/
    GUI_Delay(100);
  } while (key_flag == GUI_DEMO_PAGE_3);

  GRAPH_DetachData(hGraph, hData[0]);
  GRAPH_DetachData(hGraph, hData[1]);
  GRAPH_DetachData(hGraph, hData[2]);

  // GUIDEMO_NotifyStartNext();

  // shutdown GS LED
  hal_gpio_output_high(&brd_gpio_table[GPIO_LED_GS]);
}

static void _Graph_Sensor_Demo()
{
  const WIDGET_EFFECT * pEffectOld;
  GRAPH_Handle          hGraph;
  GRAPH_DATA_Handle     hData[3];
  // GRAPH_DATA_Handle     hData;
  int                   xSize, ySize, i;
  int                   Graph_xSize, Graph_ySize, Data_ySize;
  int                   Graph_xPos, Graph_yPos;

  // set back screen black
  // GUI_SetColor(GUI_BLACK);
  // GUIDEMO_DrawBk(1);

  xSize      = LCD_GetXSize();
  ySize      = LCD_GetYSize();  // 256
  pEffectOld = WIDGET_SetDefaultEffect(&WIDGET_Effect_Simple);
  // Return a poninter to the previous callback routine
  // Set Callback function for background window
  // The given window will be invalidated. This makes sure the window will be redrawn
  WM_SetCallback(WM_HBKWIN, _cbBk_test);
  //
  // Determine size of GRAPH
  //
  Graph_xSize = xSize - (DIST_TO_BORDER << 1);            // xsize = Screen size subtracting twice the distance to the border of the screen
  Graph_ySize = ySize - INFO_SIZE_Y - (DIST_TO_WIN << 1); // ysize = Screen size subtracting the window sizes and twice the distance to the windows
  if (Graph_ySize > MAX_GRAPH_SIZE_Y) {  // 181
    Graph_ySize = MAX_GRAPH_SIZE_Y;  // 140
    if (Graph_xSize > (Graph_ySize * 20) / 11) {
      Graph_xSize = (Graph_ySize * 20) / 11;
    }
  }
  //
  // Create and configure GRAPH_WIDGET
  //
  Graph_xPos = (xSize - Graph_xSize) >> 1;
  Graph_yPos = (ySize - Graph_ySize) >> 1;
  if (Graph_yPos < INFO_SIZE_Y + DIST_TO_WIN) {
    Graph_yPos = INFO_SIZE_Y + DIST_TO_WIN;
  }
  // create a graph widget
  hGraph = GRAPH_CreateEx(Graph_xPos, Graph_yPos, Graph_xSize, Graph_ySize, WM_HBKWIN, WM_CF_SHOW | WM_CF_CONST_OUTLINE, 0, 0);
  GRAPH_SetBorder(hGraph, BORDER_LEFT, BORDER_TOP, BORDER_RIGHT, BORDER_BOTTOM);  // set width of border
  WM_SetHasTrans (hGraph);  // enbale tansparency for the given window
  GRAPH_SetColor (hGraph, COLOR_BK,     GRAPH_CI_BK);  // Sets the desired color of the given GRAPH widget
  GRAPH_SetColor (hGraph, COLOR_BORDER, GRAPH_CI_BORDER);
  GRAPH_SetColor (hGraph, COLOR_FRAME,  GRAPH_CI_FRAME);
  GRAPH_SetColor (hGraph, COLOR_GRID,   GRAPH_CI_GRID);
  //
  // Adjust grid
  //
  GRAPH_SetGridVis  (hGraph, 1);  // set grid visible
  GRAPH_SetGridDistX(hGraph, GRID_DIST_X);  // set horizontal grid spacing
  GRAPH_SetGridDistY(hGraph, GRID_DIST_Y);  // set vertical grid spacing
  WM_BringToBottom  (hGraph);  // Places hGraph underneath its siblings
  //
  // Create and configure GRAPH_DATA_YT object
  //

  for (i = 0; i < 3; i++) {
    // Create a point valude which include a X-axis value on the Y-axis
    hData[i] = GRAPH_DATA_YT_Create(_aColorData_sensor[i], xSize - (DIST_TO_BORDER << 1) - BORDER_LEFT, 0, 0);
  }

  // hData = GRAPH_DATA_YT_Create(_aColorData_sensor[0], xSize - (DIST_TO_BORDER << 1) - BORDER_LEFT, 0, 0);
  Data_ySize = Graph_ySize - BORDER_BOTTOM;  // 131
  //
  // Create and configure GRAPH_SCALE objects
  //
  _hScaleH_sensor = GRAPH_SCALE_Create(BORDER_BOTTOM >> 1, GUI_TA_VCENTER, GRAPH_SCALE_CF_HORIZONTAL, TICK_DIST_H);
  _hScaleV_sensor = GRAPH_SCALE_Create(BORDER_LEFT   >> 1, GUI_TA_HCENTER, GRAPH_SCALE_CF_VERTICAL,   TICK_DIST_V);
  GRAPH_SCALE_SetPos(_hScaleH_sensor, Graph_ySize - 4);  // Sets the vertical position for showing the scale object with in the GRAPH widget
  GRAPH_SCALE_SetOff(_hScaleH_sensor, -5);  // Sets an horizontal offset used to 'shift' the scale object in positive or negative direction

  // show sensor graph
  GUIDEMO_ShowInfo("sensor demo");
  GRAPH_AttachScale(hGraph, _hScaleH_sensor);
  GRAPH_AttachScale(hGraph, _hScaleV_sensor);
  // _Data_Sensor_Adjust = (Data_ySize * 100) >> GRAPH_DIV;
  // (Data_ySize * 21) >> GRAPH_DIV
  GRAPH_SetGridOffY (hGraph, 6);  // Add an offset used to show the horizontal grid lines
  // (((Data_ySize - BORDER_BOTTOM) * 260) >> GRAPH_DIV)
  GRAPH_SCALE_SetOff(_hScaleV_sensor, 75);

  _Show_Sensor_Graph(hGraph, hData);

  //
  // Clean up
  //
  GRAPH_DetachScale(hGraph, _hScaleH_sensor);
  GRAPH_DetachScale(hGraph, _hScaleV_sensor);
  GRAPH_SCALE_Delete(_hScaleH_sensor);
  GRAPH_SCALE_Delete(_hScaleV_sensor);

  for (i = 0; i < 3; i++) {
    GRAPH_DATA_YT_Delete(hData[i]);
  }
  // GRAPH_DATA_YT_Delete(hData);
  WM_DeleteWindow(hGraph);
  WIDGET_SetDefaultEffect(pEffectOld);

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
  uint32_t step = 0;
#if 0
	if (sensor_all_open() != 0)
		return;
#endif
  sensor_all_open();

  // lighten GS-LED and ALS-LED
  hal_gpio_output_low(&brd_gpio_table[GPIO_LED_GS]);
  hal_gpio_output_low(&brd_gpio_table[GPIO_LED_ALS]);

  // set back screen black

  // GUIDEMO_HideInfoWin();
  // GUIDEMO_ShowControlWin();
  // GUI_Exec();
  GUIDEMO_DrawBk(1);
  GUI_SetColor(GUI_BLACK);
  GUIDEMO_DrawBk(1);

  // set font
	GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font16_ASCII);

  GUI_DispStringAt("step",                GUIDEMO_UNCLASSIFIED_OFFSET, Y_START);
  GUI_DispStringAt("acc_x",               GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  2);
  GUI_DispStringAt("acc_y",               GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  4);
  GUI_DispStringAt("acc_z",               GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6);
#if 0
  GUI_DispStringAt("barometer",     GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6);
  GUI_DispStringAt("temperature",   GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8);
  GUI_DispStringAt("humidity",      GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10);
#endif
  GUI_DispStringAt("als",                  GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8);
	GUI_DispStringAt("proximity",            GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10);
  GUI_DispStringAt("sensor data upload:",  GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  12);


  // GUI_HWIN hWnd;

  do{
      // print value
      // GUI_SetOrientation(GUI_SWAP_XY);

   //   hWnd = WM_CreateWindow(10, 10, 200, 200, WM_CF_HIDE, _cbDesktop, 0);
   //   HAL_Delay(1000);
   //   WM_DeleteWindow(hWnd);

      // print value type

      if (!get_acc_data(&acc_adc_data[0], &acc_adc_data[1], &acc_adc_data[2], &step)) {
				acc_nkg[0] = (float)acc_adc_data[0] * 9.8 / 1024;
				acc_nkg[1] = (float)acc_adc_data[1] * 9.8 / 1024;
				acc_nkg[2] = (float)acc_adc_data[2] * 9.8 / 1024;
				GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START);
        GUI_DispDec(step, DEC_LEN_DEF);
				GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  2);
				GUI_DispFloatFix(acc_nkg[0], 7, 3);
				GUI_DispString(" N/kg");
				GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  4);
				GUI_DispFloatFix(acc_nkg[1], 7, 3);
				GUI_DispString(" N/kg");
				GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6);
				GUI_DispFloatFix(acc_nkg[2], 7, 3);
				GUI_DispString(" N/kg");
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  2);
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  4);
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6);
      }
#if 0
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
#endif
			if (!get_als_data(&als_data)) {
        GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8);
        GUI_DispDec(als_data, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8);
      }
			
			if (!get_ps_data(&ps_data)) {
        GUI_GotoXY((xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10);
        GUI_DispDec(ps_data, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10);
      }

      int time_counter = 0;
      for ( ; time_counter < 10; ++time_counter) {
        if (key_flag != GUI_DEMO_PAGE_2) {
          // shutdown GS LED
          hal_gpio_output_high(&brd_gpio_table[GPIO_LED_GS]);
          hal_gpio_output_high(&brd_gpio_table[GPIO_LED_ALS]);
          return;
        }
        krhino_task_sleep(krhino_ms_to_ticks(100));
      }
   //   WM_SetCallback(WM_HBKWIN, _cbDesktop);

  }while(1);

}

void GUIDEMO_Sensor_Graph (void)
{
  GUIDEMO_ShowInfoWin();
  _Graph_Sensor_Demo();
  // GUIDEMO_NotifyStartNext();
}

#if 0 /* Wait ATCMD adapted to AliOS master branch */
static int GUIDEMO_GET_WIFI_SSID (char buf[], int len)
{
#define AT_STR_BUF_LEN     128
#define WIFI_SSID_MAX_LEN  24

  // construct version string
  char at_str_buf[AT_STR_BUF_LEN]         = {0};
  char wifi_ssid[WIFI_SSID_MAX_LEN + 1]   = {0};
  const char* at_str_head = "+WJAP:";
  int ver_head_len = strlen(at_str_head);
  if(!at_cmd_request(AT_CMD_AT_WJAP_GET, NULL, at_str_buf, AT_STR_BUF_LEN)) {
    
    char *p_begin = strstr(at_str_buf, at_str_head);
    if (!p_begin)
      return 0;

    p_begin += ver_head_len;
    char *p_end = strstr(p_begin, ",");
    if (!p_end)
      return 0;

    int wifi_ssid_len = p_end - p_begin;
    int copy_len = wifi_ssid_len < WIFI_SSID_MAX_LEN ? wifi_ssid_len : WIFI_SSID_MAX_LEN;
    strncpy(wifi_ssid, p_begin, copy_len);
    wifi_ssid[copy_len] = 0;
    
    snprintf(buf, len, "WiFi SSID: %s", wifi_ssid);
		return 1;
  }
  return 0;
}
#endif

void GUIDEMO_Version_Info (void)
{
  #define VERSION_X_OFFSET     20
  #define VERSION_Y_START      45
  #define VERSION_Y_STEP       30
  #define WIFI_Y_OFFSET        40
  // GUIDEMO_HideInfoWin();
  // GUIDEMO_ShowControlWin();
  // GUI_Exec();
  GUIDEMO_DrawBk(1);
  GUI_SetColor(GUI_BLACK);
  GUIDEMO_DrawBk(1);

  // set font
	GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font20_ASCII);

#define WIFI_SSID_DISP_LEN 40
	const char* wifi_error_info = "not connected";
  char wifi_ssid_disp[WIFI_SSID_DISP_LEN] = {0};
	char wifi_old_ssid[WIFI_SSID_DISP_LEN] = {0};
  int xSize = LCD_GetXSize();

  // display version info
  GUI_DispStringAt("HW version: A10_1_11",     VERSION_X_OFFSET, VERSION_Y_START);
  GUI_DispStringAt("FW version: A10_V0.97",    VERSION_X_OFFSET, VERSION_Y_START + VERSION_Y_STEP);
  GUI_DispStringAt("Slogan: Aliot Things",           VERSION_X_OFFSET, VERSION_Y_START + VERSION_Y_STEP * 2);
  
  GUI_DispStringHCenterAt("WiFi SSID:",        (xSize >> 1), VERSION_Y_START + VERSION_Y_STEP * 2 + WIFI_Y_OFFSET);
  
  while(1) {
#if 0 /* Wait ATCMD adapted to AliOS master branch */
    if (GUIDEMO_GET_WIFI_SSID(wifi_ssid_disp, WIFI_SSID_DISP_LEN)) {
#else
    if (0) {
#endif
      if (strcmp(wifi_ssid_disp, wifi_old_ssid)) {
        GUI_GotoXY(0, VERSION_Y_START + VERSION_Y_STEP * 3 + WIFI_Y_OFFSET);
        GUI_DispCEOL();
        GUI_DispStringHCenterAt(wifi_ssid_disp, (xSize >> 1), VERSION_Y_START + VERSION_Y_STEP * 3 + WIFI_Y_OFFSET);
        snprintf(wifi_old_ssid, WIFI_SSID_DISP_LEN, "%s", wifi_ssid_disp);
      }
    }
    else {
      if (strcmp(wifi_error_info, wifi_old_ssid)) {
          GUI_GotoXY(0, VERSION_Y_START + VERSION_Y_STEP * 3 + WIFI_Y_OFFSET);
          GUI_DispCEOL();
          GUI_DispStringHCenterAt(wifi_error_info, (xSize >> 1), VERSION_Y_START + VERSION_Y_STEP * 3 + WIFI_Y_OFFSET);
          snprintf(wifi_old_ssid, WIFI_SSID_DISP_LEN, "%s", wifi_error_info);
        } 
    }

    for (int i = 0; i < 10; i++) {
      if (key_flag != GUI_DEMO_PAGE_1)
        return;

      krhino_task_sleep(krhino_ms_to_ticks(100));
    }
  }
}

#define SOUND_FUN_IDLE       0
#define SOUND_FUN_EXECUTE    1

int sound_record;
int sound_play;
void GUIDEMO_Sound_record (void)
{
  #define INFO_START_X_OFFSET    40
  #define INFO_START_Y_START     40
  #define INFO_Y_STEP            20

  // GUIDEMO_HideInfoWin();
  // GUIDEMO_ShowControlWin();
  // GUI_Exec();
  GUIDEMO_DrawBk(1);
  GUI_SetColor(GUI_BLACK);
  GUIDEMO_DrawBk(1);

  // set font
	GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font20_ASCII);

  // display notes
  GUI_DispStringAt("Press key A to record",     INFO_START_X_OFFSET, INFO_START_Y_START);
  GUI_DispStringAt("Press key B to play",       INFO_START_X_OFFSET, INFO_START_Y_START + INFO_Y_STEP);

  sound_record = SOUND_FUN_IDLE;
  sound_play = SOUND_FUN_IDLE;

  while (key_flag == GUI_DEMO_PAGE_4) {
    if (sound_record == SOUND_FUN_EXECUTE) {
      // display on LED
      GUI_DispStringAt("Recording...",     INFO_START_X_OFFSET, INFO_START_Y_START + INFO_Y_STEP * 3);

#ifdef STARTERKIT_AUDIO
      // execute sound record
      record_to_flash();
#endif
      // krhino_task_sleep(krhino_ms_to_ticks(2000));
      // clean display
      GUI_GotoXY(INFO_START_X_OFFSET, INFO_START_Y_START + INFO_Y_STEP * 3);
      GUI_DispCEOL();

      // set sound_record to idle
      sound_record = SOUND_FUN_IDLE;
    }

    if (sound_play == SOUND_FUN_EXECUTE) {
      // display on LED
      GUI_DispStringAt("Playing...",       INFO_START_X_OFFSET, INFO_START_Y_START + INFO_Y_STEP * 3);

#ifdef STARTERKIT_AUDIO
      // execute sound play
      playback_from_flash();
#endif

      // clean display
      GUI_GotoXY(INFO_START_X_OFFSET, INFO_START_Y_START + INFO_Y_STEP * 3);
      GUI_DispCEOL();

      // set sound_play to idle
      sound_play = SOUND_FUN_IDLE;
    }

    // sleep 100ms
    krhino_task_sleep(krhino_ms_to_ticks(100));
  }

}

#else

void GUIDEMO_Unclassified(void) {}

#endif

