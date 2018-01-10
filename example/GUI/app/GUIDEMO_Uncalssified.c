#include "GUIDEMO.h"
#include <stdint.h>
#include "stm32l4xx_hal.h"
#include "soc_init.h"

#if (SHOW_GUIDEMO_UNCLASSIFIED)

#define Y_START          43
#define Y_STEP           10
#define DEC_LEN_DEF      5

typedef struct {
  int16_t    as_x;
  int16_t    as_y;
  int16_t    as_z;
}ACCE_SPEED_T;

typedef struct {
  uint16_t    channel0;
  uint16_t    channel1;
}ALS_DATA_T;

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
extern int   key_flag;

void GUIDEMO_Unclassified(void) {
  int xSize = LCD_GetXSize();
  uint16_t step_num, ps;
  ACCE_SPEED_T acc_speed;
  ALS_DATA_T als;
	float axis[3] = {0.0};

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

  GUI_DispStringAt("step", GUIDEMO_UNCLASSIFIED_OFFSET, Y_START);
  GUI_DispStringAt("as_x",   GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  2);
  GUI_DispStringAt("as_y",   GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  4);
  GUI_DispStringAt("as_z",   GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6);
  GUI_DispStringAt("als_chan0",    GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8);
  GUI_DispStringAt("als_chan1",    GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10);
  GUI_DispStringAt("proximity sensor",    GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  12);


  // GUI_HWIN hWnd;

  do{
      // print value
      // GUI_SetOrientation(GUI_SWAP_XY);

   //   hWnd = WM_CreateWindow(10, 10, 200, 200, WM_CF_HIDE, _cbDesktop, 0);
   //   HAL_Delay(1000);
   //   WM_DeleteWindow(hWnd);

      // print value type


      if (!mir3da_get_step(&step_num)) {
        GUI_DispDecAt(step_num, (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START);
      }

      if (!mir3da_read_data(&acc_speed.as_x, &acc_speed.as_y, &acc_speed.as_z)) {
				#if 0
        GUI_DispDecAt(acc_speed.as_x, (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  2, DEC_LEN_DEF);
        GUI_DispDecAt(acc_speed.as_y, (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  4, DEC_LEN_DEF);
        GUI_DispDecAt(acc_speed.as_z, (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6, DEC_LEN_DEF);
				#else
				axis[0] = (float)acc_speed.as_x * 100 / 1024;
				axis[1] = (float)acc_speed.as_y * 100 / 1024;
				axis[2] = (float)acc_speed.as_z * 100 / 1024;
				GUI_GotoXY(xSize >> 1, Y_START + Y_STEP *  2);
				GUI_DispFloatFix(axis[0], 7, 2);
				GUI_DispString("% G");
				GUI_GotoXY(xSize >> 1, Y_START + Y_STEP *  4);
				GUI_DispFloatFix(axis[1], 7, 2);
				GUI_DispString("% G");
				GUI_GotoXY(xSize >> 1, Y_START + Y_STEP *  6);
				GUI_DispFloatFix(axis[2], 7, 2);
				GUI_DispString("% G");
				#endif
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  2);
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  4);
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  6);
      }

      if (!ltr553_get_adc_als_data(&als.channel0, &als.channel1)) {
        GUI_DispDecAt(als.channel0, (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8, DEC_LEN_DEF);
        GUI_DispDecAt(als.channel1, (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  8);
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  10);
      }

      if (!ltr553_get_adc_ps_data(&ps)) {
        GUI_DispDecAt(ps, (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  12, DEC_LEN_DEF);
      }
      else {
        GUI_DispStringAt("unknow", (xSize >> 1) + GUIDEMO_UNCLASSIFIED_OFFSET, Y_START + Y_STEP *  12);
      }

      // krhino_task_sleep(100); // delay 1s
      if (key_flag == 1)
        break;

      HAL_Delay(400);

      if (key_flag == 1)
        break;

      HAL_Delay(300);

      if (key_flag == 1)
        break;

      HAL_Delay(300);
   //   WM_SetCallback(WM_HBKWIN, _cbDesktop);

  }while(1);

}

#else

void GUIDEMO_Unclassified(void) {}

#endif

