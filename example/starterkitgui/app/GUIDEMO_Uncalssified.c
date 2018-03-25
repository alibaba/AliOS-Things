#include "GUIDEMO.h"
#include <stdint.h>
#include "stm32l4xx_hal.h"
#include <aos/uData.h>

#define SHOW_GUIDEMO_UNCLASSIFIED 1

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
static int         _Data_Sensor_Adjust;


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
  //
  // Add values depending on time
  //
  // int TimeStart, TimeDiff, TimeStep;
  // int NextState, Flag;

  // TimeStart = GUIDEMO_GetTime();
  int Flag = 1;
  do {
    get_acc_data(&x,&y,&z);
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
    GUI_Delay(200);
  } while (1);
}

static void _Graph_Sensor_Demo()
{
  const WIDGET_EFFECT * pEffectOld;
  GRAPH_Handle          hGraph;
  GRAPH_DATA_Handle     hData[MAX_NUM_DATA_OBJ];
  // GRAPH_DATA_Handle     hData;
  int                   xSize, ySize, i;
  int                   Graph_xSize, Graph_ySize, Data_ySize;
  int                   Graph_xPos, Graph_yPos;

  // set back screen black
  GUI_SetColor(GUI_BLACK);
  GUIDEMO_DrawBk(1);

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

  for (i = 0; i < MAX_NUM_DATA_OBJ; i++) {
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

  for (i = 0; i < MAX_NUM_DATA_OBJ; i++) {
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

void GUIDEMO_Sensor_Graph(void)
{
  GUIDEMO_ShowInfoWin();
  _Graph_Sensor_Demo();
  GUIDEMO_NotifyStartNext();
}

#else

void GUIDEMO_Unclassified(void) {}

#endif

