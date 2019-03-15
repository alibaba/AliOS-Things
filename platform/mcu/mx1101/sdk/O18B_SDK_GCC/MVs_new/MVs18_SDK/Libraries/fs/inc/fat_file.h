//maintainer lilu
#ifndef __FAT_FILE_H__
#define __FAT_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fs_config.h"
#include "folder.h"
#include "fsinfo.h"


//open mode

#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08

//file status
//#define FA__WRITTEN			0x20
//#define FA__DIRTY			0x40
//#define FA__ERROR			0x80

//define file seek position.
#define	SEEK_FILE_SET				0					//seek file from the first sector of file
#define	SEEK_FILE_CUR				1					//seek file from the current sector of file
#define	SEEK_FILE_END				2					//seek file from the last sector of file 

typedef enum _FILE_TYPE
{
    FILE_TYPE_MP3,
    FILE_TYPE_WMA,
    FILE_TYPE_WAV,
    FILE_TYPE_SBC,
    FILE_TYPE_FLAC,
    FILE_TYPE_AAC,
    FILE_TYPE_AIF,
    FILE_TYPE_AMR,
    FILE_TYPE_UNKNOWN = 0xFF

} FILE_TYPE;



//文件的描述结构
typedef struct _FAT_FILE
{
	uint16_t	FileNumInDisk;			//在整个设备上的文件号
	uint16_t	FileNumInFolder;		//在当前文件夹中的文件号
	uint16_t	FolderNum;				//所在文件夹的编号
//	uint16_t	ValidFolderNum;			//所在文件夹的有效文件夹编号

	uint32_t	ParentFoldStartSecNum; //父目录起始扇区号
	uint32_t 	DirSecNum;             //FDI 所在扇区号
	uint32_t 	DirOffset;			   //FDI 所在扇区中的偏移量

	uint8_t	ShortName[11];			//短文件名
#if FAT_USE_LFN == 1
	//uint8_t	LongName[FAT_NAME_MAX];			//长文件名
#endif
	uint8_t	    FileType;				//文件夹类型
	uint32_t	Size;					//文件的总字节数

	uint32_t 	FptrIndex;
	uint32_t 	FptrCluster;
	uint32_t	StartClusNum;			//文件第一个簇号
	uint32_t	SecterOffset;			// Sector offset in the cluster

	uint32_t    Fptr;					//当前位置在文件中偏移字节数
	//uint32_t    DataSec;				//当前缓冲区中的数据扇区号
	//uint8_t* 	IOBuffer;				//文件读写缓冲区
	uint8_t     Flag;				    //标记文件状态

	FS_CONTEXT* FsContext;
} FAT_FILE;


//设置、获取文件时间的结构体
typedef struct _FILE_TIME
{
	uint16_t Year;	//1980～2107
	uint8_t  Month;	//1～12
	uint8_t  Date;	//1～31
	uint8_t  Hour;	//0～23 
	uint8_t  Min;	//0～59
	uint8_t  Sec;	//0～59

} FILE_TIME;


//
// @brief  Open a file
// @param  file_name    file name   forexample: \\mp3\\short.mp3,\\mp3\\short.mp3
// @param  mode         open mode   FA_READ、FA_WRITE、FA_CREATE_NEW、FA_CREATE_ALWAYS
// @return File Handle
// 字符串打开文件不支持中文名的文件夹或文件
// 支持长文件名
bool FileOpen(FAT_FILE* File, const uint8_t* FileName, const uint8_t Mode);

//判断文件的类型，由文件系统底层调用
uint8_t FileGetType(FS_CONTEXT* FsContext);

//
// @brief  Check End-of-File indicator.
// @param  File      Pointer to a FILE object that specifies stream.
// @return The feof function returns a nonzero value if a read operation has attempted to read past the end of the file; it returns 0 otherwise.
//
int32_t FileEOF(FAT_FILE* File);

//返回到文件开头
void FileRewind(FAT_FILE* File);

//按文件序号打开文件
//打开成功：返回TRUE，目标文件信息填充到File所指的结构体。
//打开失败：返回FALSE。
//Folder == NULL: 打开整个设备上的第FileNum个文件。
//Folder != NULL: 打开Folder文件夹中的第FileNum个文件夹。
bool FileOpenByNum(FAT_FILE* File, FOLDER* Folder, uint16_t FileNum);

//按文件名称打开指定文件夹中的文件。
//打开成功：返回TRUE，目标文件信息填充到File所指的结构体。
//打开失败：返回FALSE。
//Folder指针不能为空指针。
bool FileOpenByName(FAT_FILE* File, FOLDER* Folder, uint8_t* FileName);

//按文件长名称打开指定文件夹中的文件。
//打开成功：返回TRUE，目标文件信息填充到File所指的结构体。
//打开失败：返回FALSE。
//Folder不能为空，FileName为Unicode 16编码
bool FileOpenByLfName(FAT_FILE* File, FOLDER* Folder, uint8_t* FileName, uint8_t Len);

//
// @brief  Moves the file pointer to a specified location.
// @param  File      Pointer to a FILE object that specifies stream.
// @param  Offset  Number of bytes from origin.
// @param  Base  Initial position.
// @return If successful, the function returns a zero value. Otherwise, it returns nonzero value.
//
int32_t FileSeek(FAT_FILE* File, int32_t Offset, uint8_t Base);

//
// @brief  Read data from stream
// @param  buffer  Pointer to a block of buffer with a minimum size of (size*count) bytes.
// @param  size    Size in bytes of each element to be read.
// @param  count   Number of elements, each one with a size of size bytes.
// @param  File      Pointer to a FILE object that specifies an input stream.
// @return The total number of elements successfully read.
//
uint32_t FileRead(void* buffer, uint32_t size, uint32_t count, FAT_FILE* File);

//
// @brief  Write data to stream
// @param  buffer  Pointer to a block of buffer with a minimum size of (size*count) bytes.
// @param  size    Size in bytes of each element to be write.
// @param  count   Number of elements, each one with a size of size bytes.
// @param  File      Pointer to a FILE object that specifies an output stream.
// @return The total number of elements successfully write.
//
uint32_t FileWrite(const void* buffer, uint32_t size, uint32_t count, FAT_FILE* File);

//
// @brief  Get size of the opened file.
// @param  File      Pointer to a FILE object that specifies an output stream.
// @return size of the opened file.
//
int32_t FileSof(FAT_FILE* File);

//
// @brief  Gets the current position of a file pointer.
// @param  File      Pointer to a FILE object that specifies stream.
// @return code
//
int32_t FileTell(FAT_FILE* File);

//
// @brief  File save Fat and Dir
// @param  File      Pointer to a FILE object that specifies an output stream.
// @return fclose returns 0 if the stream is successfully closed.
//
int32_t FileSave(FAT_FILE* File);

//
// @brief  Closes a stream.
// @param  File      Pointer to a FILE object that specifies an output stream.
// @return fclose returns 0 if the stream is successfully closed.
//
int32_t FileClose(FAT_FILE* File);

//获取指定文件的长文件名。
//长文件名LongFileName不能小于GetMaxLength
//获取到长文件名，返回TRUE。
//无长文件名，返回FALSE。
bool FileGetLongName(FAT_FILE* File, uint8_t* LongFileName, uint8_t GetMaxLength);

//文件清空。
//成功:返回TRUE，失败:返回FALSE。
bool FileFlush(FAT_FILE* File);

//文件删除。成功:返回TRUE，失败:返回FALSE。
bool FileDelete(FAT_FILE* File);

//按文件名称在指定文件夹中新建一个文件。
//目标文件信息填充到File所指的结构体。
//FileName[]为短文件名，例如："123.TXT", "ABC123.MP3"。
//短文件名长度不能超过8+3字节，短文件名中不能同时出现大小写字母。
bool FileCreate(FAT_FILE* File, FOLDER* Folder, uint8_t* FileName);

//按长文件名称在指定文件夹中新建一个文件。
//目标文件信息填充到File所指的结构体。
//FileName[]为长文件名
//Folder 不能为空，FileName为Unicode 16编码，LongFileName比为128 字节空闲位置为0xFF
bool FileCreateByLfName(FAT_FILE* File, FOLDER* Folder, uint8_t* LongFileName, uint8_t Len);

bool FSFormat(void);

//修改文件的时间信息。 
//CreateTime: 创建时间，精度为1秒
//ModifyTime: 最后修改时间，精度为2秒
//AccessTime: 最后访问时间，只有日期，忽略时间部分
//如果某个时间信息传入的指针为NULL，则表示不改变该时间信息
//成功:返回TRUE， 失败:返回FALSE。
bool FileSetTime(FAT_FILE* File, FILE_TIME* CreateTime, FILE_TIME* ModifyTime, FILE_TIME* AccessTime);

//获取文件的时间信息。 
//CreateTime: 创建时间，精度为1秒
//ModifyTime: 最后修改时间，精度为2秒
//AccessTime: 最后访问时间，只有日期，忽略时间部分
//如果某个时间信息传入的指针为NULL，则表示不获取该时间信息
//成功:返回TRUE， 失败:返回FALSE。
bool FileGetTime(FAT_FILE* File, FILE_TIME* CreateTime, FILE_TIME* ModifyTime, FILE_TIME* AccessTime);

void SetFsTerminateFuc(TerminateFunc func);

extern TerminateFunc terminate_cur_fs_proc;

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //_FAT_FILE_H_
