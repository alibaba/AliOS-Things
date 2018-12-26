//maintainer lilu
#ifndef __FSMOUNT_BUFFERMANAGER_H__
#define	__FSMOUNT_BUFFERMANAGER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "fs_config.h"
#include "fsinfo.h"
//
//主要目的为实现多分区共享文件系统空间
//

typedef struct _FSCONTEXT_BUFFER	 //Fs buffer
{
	FS_CONTEXT FSContext[MAX_FS_CONTEXT_COUNT];
	uint8_t DeviceID[MAX_FS_CONTEXT_COUNT];
} FSCONTEXT_BUFFER;

typedef struct _FSCONTEXT_BUFFER_MANAGER	 //Fs context buffer manager
{

	FSCONTEXT_BUFFER FSContextBuffer;
	//
	//A pointer to the function to alloc a buffer.
	//
	FS_CONTEXT* (*pfnAllocFSContextBuffer)(FSCONTEXT_BUFFER* FSContextBufferp, uint8_t DeviceID);

	//
	//A pointer to the function to free a buffer.
	//
	bool (*pfnFreeFSContextBuffer)(FSCONTEXT_BUFFER* FSContextBufferp, uint8_t DeviceID);


} FSCONTEXT_BUFFER_MANAGER;

extern FSCONTEXT_BUFFER_MANAGER gFSContextBufferManager;

extern FS_CONTEXT* FSContextBufferAlloc(FSCONTEXT_BUFFER* FSContextBufferp, uint8_t DeviceID);

extern bool FSContextBufferFree(FSCONTEXT_BUFFER* FSContextBufferp, uint8_t DeviceID);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //_FSMOUNT_BUFFERMANAGER_H_
