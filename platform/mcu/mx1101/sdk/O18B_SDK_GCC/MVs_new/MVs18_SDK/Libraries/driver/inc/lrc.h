///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: lrc.h
//  maintainer: Sam
///////////////////////////////////////////////////////////////////////////////

#ifndef __LRC_H__
#define __LRC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus


#pragma pack(1)

typedef enum _TEXT_ENCODE_TYPE
{
    ENCODE_UNKNOWN,
    ENCODE_ANSI,
    ENCODE_UTF8,
    ENCODE_UTF16, // unicode
    ENCODE_UTF16_BIG,
    ENCODE_GBK,
} TEXT_ENCODE_TYPE;

typedef struct _LRC_ROW
{
	int32_t StartTime; // 起始时间点, ms
	uint32_t Duration;  // 当前歌词行持续时间, ms
	uint8_t* LrcText;   // 歌词指针，由外部分配内存
	int16_t MaxLrcLen; // 歌词Buffer的内存大小
} LRC_ROW;

#pragma pack()

/**
* @brief                   按文件全路径初始化Lrc Parser, 【注意】不支持同时打开多个Lrc Parser
* @param input CurFolder   当前文件夹句柄
* @param input FileName    歌词文件全路径
* @param input FileNameLen 歌词文件名长度
* @param input ReadBuffer  Lrc Parser的工作内存空间，由用户外部指定内存空间，
*                          如果设置为NULL，默认由Parser内部分配
* @param input ReadBufSize Lrc Parser工作空间的大小，如果工作空间由用户指定（即ReadBuffer != NULL），
*                          则该值必须 >= 128，如果工作空间由内部分配，该值除了可设置 >= 128的值外，
*                          还可以设置为0，表示Lrc Parser自己默认分配512Bytes的工作空间
* @return Success - TRUE, Fail - FALSE
*/
bool OpenLrcByFileName(FOLDER* CurFolder, uint8_t* FileName, uint32_t FileNameLen,
                           uint8_t* ReadBuffer, uint32_t ReadBufSize);

/**
* @brief                   按文件ID号初始化Lrc Parser, 【注意】不支持同时打开多个Lrc Parser
* @param input CurFolder   当前文件夹句柄
* @param input FileId      歌词文件ID，当CurFolder != NULL时表示全盘ID否则表示相对ID
* @param input ReadBuffer  Lrc Parser的工作内存空间，由用户外部指定内存空间，
*                          如果设置为NULL，默认由Parser内部分配
* @param input ReadBufSize Lrc Parser工作空间的大小，如果工作空间由用户指定（即ReadBuffer != NULL），
*                          则该值必须 >= 128，如果工作空间由内部分配，该值除了可设置 >= 128的值外，
*                          还可以设置为0，表示Lrc Parser自己默认分配512Bytes的工作空间
* @return Success - TRUE, Fail - FALSE
*/
bool OpenLrcByFileId(FOLDER* CurFolder, uint32_t FileId,
                         uint8_t* ReadBuffer, uint32_t ReadBufSize);

/**
* @brief 关闭Lrc Parser
* @return Success - TRUE, Fail - FALSE
*/
bool CloseLrcFile(void);

/**
* @brief 根据时间点获得某行歌词的实际长度，如果找不到该时间点对应的歌词则返回 -1
* @param input SeekTime 歌词时间点
* @param 当用户希望用动态内存的方式获得歌词时，可在调用GetLrcInfo函数前，
*        先调用本函数预先知道特定时间点的歌词长度，然后动态分配好装载歌词所需内存，
*        作为GetLrcInfo函数的输入参数
* @return 输出与时间点对应的歌词长度，如果找不到歌词，则返回-1
*/
int32_t GetLrcTextLength(uint32_t SeekTime);

/**
* @brief 根据时间点查询歌词信息，主要有歌词起始时间，持续时间，歌词内容
* @param input SeekTime 歌词时间点
* @param input TextOffset 歌词偏移 用于对很长的歌词分段获取，每次从指定偏移地址开始读一段歌词
* @param 该函数输出歌词时，会根据指定的Buffer Size(LrcRow->MaxLrcLen)，填充Buffer(LrcRow->LrcText)
*        如果Buffer Size小于实际的歌词长度，则自动截断歌词
*        【注意】如果歌词过长，截断的歌词末尾字符需要判断汉字的完整性，
*                由于该接口值输出歌词原字符串，并且编码类型较多，所以汉字完整判断由接口调用者自行处理

* @return 歌词的实际长度，如果找不到歌词，返回-1，
*        【注意】如果歌词实际长度大于歌词Buffer(LrcRow->LrcText)的最大长度(LrcRow->MaxLrcLen)，
*                则返回值 > LrcRow->MaxLrcLen，否则 返回值 <= LrcRow->MaxLrcLen
*/
int32_t GetLrcInfo(LRC_ROW* LrcRow, uint32_t SeekTime/*ms*/, uint32_t TextOffset);

/**
* @brief 获得当前歌词文件的编码格式
* @return 歌词的编码类型
*/
TEXT_ENCODE_TYPE GetLrcEncodeType(void);


#ifdef __cplusplus
}
#endif//__cplusplus

#endif





