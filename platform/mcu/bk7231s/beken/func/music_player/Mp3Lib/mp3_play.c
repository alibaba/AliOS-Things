/**
  ******************************************************************************
  * @file    Project/play.c 
  * @version V1.0.0
  * @brief   this file provides all audio play relevant function
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  */ 

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "mp3dec.h"
#include "mp3common.h"

#if (CONFIG_APP_MP3PLAYER == 1)
/************************************************************************************/
/*
* File Name  : Convert_Stereo  单声道到立体声转换
* Description    : I have do some modification in Subband() function due to the long time of
*                  decode.Using PolyphaseMono() function which is used in decode mono mp3,to decode
*                  Stereo,so must use this funtion to convert mono to Stereo.
* Input          : Adress of buffer data
* Output         : None
* Return         : None
*/
/************************************************************************************/
void  Convert_Stereo(short *buffer)
{
	int i,j,k=0;
	for(i=0;i<(2304/64);i++)	   //36
	{
		for(j=31+i*64,k=j+32;j>=0+i*64;j--,k-=2)
		{
			buffer[k]=buffer[j];
			buffer[k-1]=buffer[j]; 
		}
	}
}

/***********************************************************************************/
/*
* File Name  : Convert_Mono
* Description    : 缓冲后的单声道MP3解码器的框架，将填补1152半字，这个功能*将改变单声道到立体声。
* Input          : Adress of buffer data
* Output         : None
* Return         : None
*/
/***********************************************************************************/
void  Convert_Mono(short *buffer, int outputSamps)
{
	int i;
	if(outputSamps > 1152)
	{
	    return;
	}
    for (i = outputSamps - 1; i >= 0; i--)
	{
		buffer[i * 2 + 1] = buffer[i];
        buffer[i * 2] = buffer[i];
	}
}
#endif /* CONFIG_APP_MP3PLAYER */

