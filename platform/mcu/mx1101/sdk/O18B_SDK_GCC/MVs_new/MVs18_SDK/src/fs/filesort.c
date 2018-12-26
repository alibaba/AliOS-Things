/////////////////////////////////////////////////////////////////////////
//                  Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc.,ShangHai,China
//                      All rights  reserved
//  Filename:filesort.c
//maintainer lilu
/////////////////////////////////////////////////////////////////////////

#include "fs_config.h"
#include "folder.h"
#include "fat_file.h"
#include "fsinfo.h"
#include "diskio.h"
#include "presearch.h"
#include "fs_filesort.h"

#if FS_FILE_SORT_EN == 1

//出于内存大小的限制，这一版文件名排序功能最大只支持整个设备上前2048个歌曲文件的排序。
//超出此范围的歌曲仍可以播放，但不再按照文件名排序，而是按照歌曲序号排序。
//最大支持的排序歌曲数，最大支持2048个，客户也可以将此最大值改小，以减小SRAM的使用量。
//保存一个文件的排序序号使用12bit，所以最大支持2048个文件时需要SRAM 1.5KB。


//在定义了FS_FILE_SORT_EN宏的前提下，可以通过该变量控制是否使能文件名排序功能。
//如果需要已使能排序，需要动态禁止文件名排序功能，则只需要将该变量设置成FALSE
bool gFileSortEnable = TRUE;


/////////////文件名排序功能需要用到的几个缓冲区//////////////////////////////////////////

//用于存放排序前的文件目录项，每个文件占用11字节
typedef uint8_t FILE_NAME_TYPE[9];	//文件名数组类型，简化下面宏定义的写法

FILE_NAME_TYPE FileNameTable[FS_MAX_FILE_SORT_NUM]__ATTRIBUTE__(AT(VMEM_ADDR + 2048));


//用于存放排序结果的数组，每个文件占用2字节
uint16_t FileSortTableTemp[FS_MAX_FILE_SORT_NUM]__ATTRIBUTE__(AT(VMEM_ADDR + FS_MAX_FILE_SORT_NUM * 9 + 2048));

//用于存放压缩的排序结果的数组，每个文件占用1.5字节
//FileSortTable[]数组中第m项表示排序后的第m个歌曲的原始文件号
//存储的个数如下：
//   ---------------------------------------
//  |              FileNum1[11:4]           |  +0
//   ---------------------------------------
//  |   FileNum2[11:8]   |   FileNum1[3:0]  |  +1
//   ---------------------------------------
//  |		       FileNum2[7:0]	        |  +2
//   ---------------------------------------
//  |			   ... ...	                |  +3
//   ---------------------------------------
static uint8_t FileSortTable[(FS_MAX_FILE_SORT_NUM * 3) / 2 + 1];
//////////////////////////////////////////////////////////////////////////////////////////



//预搜索过程中找到一个新的歌曲文件，将其文件名拷贝到缓冲区
//由PreSearch()函数调用
void FileSortSearchNewFile(FS_CONTEXT* FsContext)
{

	if(FsContext->gFsInfo.FileSum <= FS_MAX_FILE_SORT_NUM)
	{
		memcpy(FileNameTable[FsContext->gFsInfo.FileSum - 1], &FsContext->gCurrentEntry->FileName[0], 11);
	}
}



//先对所有文件名扫描一遍，将文件名中的数字子串转成可比较大小的压缩BCD格式。
//如果数字子串为个位数，则数字子串第一个字节就是该数字值0--9
//如果数字子串不是个位数，则数字子串第一个字节为数字位数，0x0A表示2位数，0x0B表示3位数，...
//从第二个字节开始存储数字子串压缩BCD格式，后续多余字节填充0x00
//例1: 4D 50 33 5F 30 34 XX XX: 4D 50 03 5F 04 00 XX XX
//例2: 4D 50 33 5F 30 30 35 XX: 4D 50 03 5F 05 00 00 XX
//例3: 41 30 42 30 30 43 30 35: 41 00 42 00 00 43 05 00
static void FileSortConvertName(FS_CONTEXT* FsContext)
{
	uint16_t Cnt = (FsContext->gFsInfo.FileSum > FS_MAX_FILE_SORT_NUM) ? FS_MAX_FILE_SORT_NUM : FsContext->gFsInfo.FileSum;
	uint16_t i;
	uint8_t j;
	uint8_t k;

	uint8_t * p;
	uint8_t NumCnt;
	uint8_t ZeroCnt;
	uint8_t Temp;

	for(i = 0; i < Cnt; i++)
	{
		p = FileNameTable[i];

		for(j = 0; j < 8; j++)
		{
			if((p[j] < '0') || (p[j] > '9'))
			{
				continue;	//当前位置非数字
			}

			//找到一个数字子串
			//计算子串开头的'0'个数
			for(ZeroCnt = 0; j + ZeroCnt < 8; ZeroCnt++)
			{
				if(p[j + ZeroCnt] != '0')
				{
					break;
				}
				p[j + ZeroCnt] = 0x00;
			}

			//计算子串有效数字个数
			for(NumCnt = 0; j + ZeroCnt + NumCnt < 8; NumCnt++)
			{
				if((p[j + ZeroCnt + NumCnt] < '0') || (p[j + ZeroCnt + NumCnt] > '9'))
				{
					break;
				}
				Temp = p[j + ZeroCnt + NumCnt];
				p[j + ZeroCnt + NumCnt] = 0x00;
				p[j + NumCnt] = Temp - '0';
			}

			//如果是2位数或更大的数，则将数字子串转换成压缩BCD格式
			//否则，如果是个位数，数组中已经是预定格式
			if(NumCnt > 1)
			{
				for(k = 0; k < (NumCnt + 1) / 2; k++)
				{
					if(2 * k + 1 == NumCnt)	//如果是3位数、5位数、7位数，并且是取最后一位数，则只取1个字节，否则取2个字节
					{
						Temp = (p[j + 2 * k] << 4);
						p[j + 2 * k] = 0;
						p[j + k + 1] = Temp;

					}
					else
					{
						Temp = (p[j + 2 * k] << 4) | p[j + 2 * k + 1];
						p[j + 2 * k] = 0;
						p[j + 2 * k + 1] = 0;
						p[j + k + 1] = Temp;
					}
				}
				p[j] = NumCnt + 8;	//数字子串的第1个字节表示该数字子串的位数。1位数：00H-09H，2位数：0AH，3位数：0BH，8位数：10H
			}

			j += (NumCnt + ZeroCnt);
		}

	}
}


//二分法插入排序算法，所花时间与(n*logn)成正比
static void FileSortByName(FS_CONTEXT* FsContext)
{
	uint16_t Cnt = (FsContext->gFsInfo.FileSum > FS_MAX_FILE_SORT_NUM) ? FS_MAX_FILE_SORT_NUM : FsContext->gFsInfo.FileSum;
	uint16_t i;
	uint16_t j;

	int16_t Start;
	int16_t End;
	int16_t Cur;

	for(i = 0; i < Cnt; i++)
	{
		Start = -1;
		End = i;

		//二分法查找
		while(Start + 1 < End)
		{
			Cur = (Start + End) / 2;
			if(memcmp(FileNameTable[i], FileNameTable[FileSortTableTemp[Cur]], 9) < 0)
			{
				End = Cur;
			}
			else
			{
				Start = Cur;
			}
		}

		//将新节点插入到End位置，之后的所有节点后移
		for(j = i; j > End; j--)
		{
			FileSortTableTemp[j] = FileSortTableTemp[j - 1];
		}

		FileSortTableTemp[End] = i;
	}
}


static void FileSortSaveResult(FS_CONTEXT* FsContext)
{
	uint16_t Cnt = (FsContext->gFsInfo.FileSum > FS_MAX_FILE_SORT_NUM) ? FS_MAX_FILE_SORT_NUM : FsContext->gFsInfo.FileSum;
	uint16_t i;
	uint16_t Temp;

	//将排序列表压缩到FileSortTable[]数组中
	for(i = 1; i <= Cnt; i++)
	{
		Temp = ((i - 1) / 2) * 3;

		FileSortTableTemp[i - 1]++;

		if(i % 2)	//奇数
		{
			FileSortTable[Temp + 0] = (uint8_t)(FileSortTableTemp[i - 1] >> 4);
			FileSortTable[Temp + 1] = (uint8_t)(FileSortTableTemp[i - 1] & 0x0F);
		}
		else		//偶数
		{
			FileSortTable[Temp + 1] |= (uint8_t)((FileSortTableTemp[i - 1] >> 4) & 0xF0);
			FileSortTable[Temp + 2] = (uint8_t)FileSortTableTemp[i - 1];
		}
	}
}

//预搜索结束，完成文件名排序
//待排序的文件名在FileNameTable[]数组中
//排序后的序号临时存放在FileSortTableTemp[]数组中
//然后将排序后的列表压缩后存放在FileSortTable[]数组中
//由PreSearch()函数调用
void FileSortSearchEnd(FS_CONTEXT* FsContext)
{
	TIMER TestTimer;	//用于测试时统计执行时间

//	DBG("FileSortSearchEnd()\n");

	//软件生成文件名，用于软件仿真
//	gFsInfo.FileSum = 10;
//	memcpy(FileNameTable[0], "1        M", 9);

	TimeOutSet(&TestTimer, 0);
	FileSortConvertName(FsContext);
	DBG("time1: %d ms\n", (uint16_t)PastTimeGet(&TestTimer));

	TimeOutSet(&TestTimer, 0);
	FileSortByName(FsContext);
	DBG("time2: %d ms\n", (uint16_t)PastTimeGet(&TestTimer));

	TimeOutSet(&TestTimer, 0);
	FileSortSaveResult(FsContext);
	DBG("time3: %d ms\n", (uint16_t)PastTimeGet(&TestTimer));
}


//该函数在FileOpenByNum()函数之前调用
//计算排序后的第FileSortNum个文件所对应的原始全盘文件号
//忽略原始文件号不在[StartFileNum + 1, EndFileNum]范围内的文件
//1.计算全盘上排序后的第m个文件的原始全盘文件号：
//		GetFileNumInDisk(FsContext, m, 0, 0xFFFF)，得到全盘排序后第m个文件对应的原始全盘文件号
//2.计算文件夹内排序后的第m个文件的原始全盘文件号：
//		GetFileNumInDisk(FsContext, m, Folder->StartFileNum, Folder->StartFileNum + Folder->IncFileCnt)，
uint16_t GetFileNumInDisk(FS_CONTEXT* FsContext, uint16_t FileSortNum, uint16_t StartFileNum, uint16_t EndFileNum)
{
	uint16_t i;
	uint16_t FileNumInDisk;
	uint16_t Index;

	//当前为禁止文件名排序功能
	if(!gFileSortEnable)
	{
		return (StartFileNum + FileSortNum);
	}

	//如果目标文件超出最大排序文件数，则直接使用原始文件号做为排序后的文件号
	if(FileSortNum + StartFileNum > FS_MAX_FILE_SORT_NUM)
	{
		return (StartFileNum + FileSortNum);
	}

	//在排序列表中逐个检查是否为目标文件
	for(i = 1; i <= FS_MAX_FILE_SORT_NUM; i++)
	{
		//计算排序后的第i个文件对应的原始全盘文件号
		Index = ((i - 1) / 2) * 3;
		if(i % 2)	//奇数
		{
			FileNumInDisk = ((FileSortTable[Index] << 4) + (FileSortTable[Index + 1] & 0x0F));
		}
		else		//偶数
		{
			FileNumInDisk = (((FileSortTable[Index + 1] & 0xF0) << 4) + FileSortTable[Index + 2]);		//(FileSortTable[Index + 1] & 0xF0)左移4位，而非8位
		}

		//判断文件号是否在在目标范围内
		if((FileNumInDisk > StartFileNum) && (FileNumInDisk <= EndFileNum))
		{
			FileSortNum--;
			if(FileSortNum == 0)
			{
				//找到目标文件
				return FileNumInDisk;
			}
		}
	}
}

#endif//FS_FILE_SORT_EN

