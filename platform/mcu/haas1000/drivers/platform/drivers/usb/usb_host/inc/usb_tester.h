/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#ifndef USB_TESTER_H
#define USB_TESTER_H

#include "FATFileSystem.h"
#include <stdint.h>


class USBFileSystem : public FATFileSystem
{

public:
    USBFileSystem();
    
    void SetDevice(int device);

    int GetDevice(void);

    virtual int disk_initialize();
    
    virtual int disk_write(const uint8_t * buffer, uint64_t sector, uint8_t count);
    
    virtual int disk_read(uint8_t * buffer, uint64_t sector, uint8_t count);
        
    virtual uint64_t disk_sectors();

protected:	
	int _device;
	u32 _blockSize;
	u32 _blockCount;
	
};


#endif
