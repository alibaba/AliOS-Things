#ifndef _PCM_FIFO_H_
#define _PCM_FIFO_H_

#define MAX_PCM_FIFO_DEPTH (2)

typedef enum _FIFO_STATUS {
	FIFO_EMPTY=0,
	FIFO_FULL,		
	FIFO_STAUS_MAX_NUM
} FIFO_STATUS;


typedef struct _PCM_FIFO{
        uint16_t rptr;
		uint16_t depth;
//		FIFO_STATUS 
		uint32_t *pBuf;
} PCM_FIFO;



#endif //_PCM_FIFO_H_