/**
 ********************************************************************************************************
 Copyright (c) 2015, Realtek Semiconductor Corporation. All rights reserved.
 *********************************************************************************************************
 * @file      cycle_queue.c
 * @brief     Cycle Queue functions Implementation.
 * @details
 *
 * @author  lory_xu
 * @date        2015-07-13
 * @version v0.1
 */

#include <string.h>
#include "os_sync.h"
#include "cycle_queue.h"
#include "trace_app.h"
#include "os_mem.h"


/** @detailed  cyc_buffer stores all the printing information which will be print in the PrintTask */
uint8_t *cyc_buffer = NULL;
volatile uint16_t pRead = 0;
volatile uint16_t pWrite = 0;

bool MallocCycQueue(void);
/**
 * @brief  check if write queue empty
 *
 * @param
 *
 * @return
 */
uint8_t IsCycQueueEmpty()
{
    uint16_t tmpRead = pRead;
    uint16_t tmpWrite = pWrite;
    return (tmpRead == tmpWrite);
}

/**
 * @brief  check if write queue full
 *
 * @param
 *
 * @return
 */
uint8_t IsCycQueueFull()
{
    uint16_t tmpRead = pRead;
    uint16_t tmpWrite = pWrite;
    return ((tmpWrite + 1) & (MAX_BUFFER_SIZE - 1)) == tmpRead;
}

/**
 * @brief  get data size in cycle queue
 *
 * @param
 *
 * @return
 */
uint16_t CycQueueSize()
{
    uint16_t tmpRead = pRead;
    uint16_t tmpWrite = pWrite;
    return (tmpWrite - tmpRead + MAX_BUFFER_SIZE) & (MAX_BUFFER_SIZE - 1);
}

/**
 * @brief  remain cycle queue size
 *
 * @param
 *
 * @return
 */
uint16_t CycQueueRemainSize()
{
    uint16_t tmpRead = pRead;
    uint16_t tmpWrite = pWrite;
    return (MAX_BUFFER_SIZE - tmpWrite + tmpRead - 1) & (MAX_BUFFER_SIZE - 1);
}

/**
 * @brief  write data to cycle queue
 *
 * @param pWriteBuf: data buf
 * @param length: data length
 *
 * @return
 */
bool CycQueueWrite(uint8_t *pWriteBuf, uint16_t length)
{
    bool     ret = true;
    uint32_t s;

    if (cyc_buffer == NULL)
    {
       // DiagPutChar("cyc_buffer is init\r\n");
        MallocCycQueue();
        if(cyc_buffer == NULL)
        {
            APP_PRINT_ERROR0("cyc_buffer is NULL, malloc fail");
            return false;
        }
    }

    s = os_lock();

    if (CycQueueRemainSize() >= length)
    {
        if ((pWrite + length) <= MAX_BUFFER_SIZE)
        {
            memcpy(cyc_buffer + pWrite, pWriteBuf, length);
            pWrite = (pWrite + length) & (MAX_BUFFER_SIZE - 1);
        }
        else
        {
            uint16_t part_length = MAX_BUFFER_SIZE - pWrite;
            memcpy(cyc_buffer + pWrite, pWriteBuf, part_length);
            memcpy(cyc_buffer, (pWriteBuf + part_length), (length - part_length));
            pWrite = length - part_length;
        }
    }
    else
    {
        ret = false;
    }

    os_unlock(s);

    return ret;
}

/**
 * @brief  upate read ponit of cycle queue
 *
 * @param SendSize
 *
 * @return
 */
void UpdateQueueRead(uint16_t SendSize)
{
    uint32_t s;

    s     = os_lock();
    pRead = (pRead + SendSize) & (MAX_BUFFER_SIZE - 1);
    os_unlock(s);
}


/**
 * @brief malloc the buffer 
 *
 * @param malloc the cycle
 *
 * @return
 */
bool MallocCycQueue()
{
    if(cyc_buffer != NULL)
    {
        APP_PRINT_ERROR0("cyc_buffer is not free");
        FreeCycQueue();
    }
    cyc_buffer = os_mem_zalloc(RAM_TYPE_DATA_ON, MAX_BUFFER_SIZE);
    pRead = 0;
    pWrite = 0;
    return true;
}

/**
 * @brief free the buffer 
 *
 * @param free the buffer 
 *
 * @return
 */
void FreeCycQueue()
{
	if (cyc_buffer != NULL) {
    	os_mem_free(cyc_buffer);
	}
    pRead = 0;
    pWrite = 0;
    cyc_buffer = NULL;
}

