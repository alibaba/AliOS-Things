#ifndef __UTIL_BUF_H__
#define __UTIL_BUF_H__

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C"{
#endif


#define BUF_ROOM(A)           A##_ROOM

#define BUF_ABS_ITER_NEXT(iter,buf_size) ((((iter)+1)<buf_size)?((iter)+1):(0))
#define BUF_ABS_ITER_PREV(iter,buf_size) (((iter)==0)?((buf_size)-1):((iter)-1))
    
#define BUF_ITER_NEXT(iter,buf_size)\
  {\
      iter++;\
      if(iter>=buf_size)\
      {\
          iter=0;\
      }\
}

#define BUF_ITER_PREV(iter,buf_size)\
  {\
      iter--;\
       if(iter>=buf_size)\
       {\
          iter=buf_size-1;\
      }\
}

#define BUF_INDEX_TO_ABS_ITER(index,tail,buf_size)\
  ((index+tail)>=buf_size)?((index+tail)-buf_size):((index+tail))

#define BUF_QUERY(temp_head,temp_tail,temp_size)\
  (temp_head>=temp_tail)?(temp_head-temp_tail):(temp_head + temp_size - temp_tail)

#define BUF_DATA_AT_ABS(A,B) (((A)->__buf_room)[(B)])

typedef  struct __BUF_STRUCT
{
  uint16_t __head;
  uint16_t __tail;
  uint8_t * const  __buf_room;   
  const uint16_t __buf_size;
}BUF_STRUCT;


inline static uint16_t BUF__get_head(BUF_STRUCT *_this)
{
  return _this->__head;
}

inline static uint16_t BUF__get_tail(BUF_STRUCT *_this)
{
  return _this->__tail;
}

inline static uint8_t *BUF__get_buf_room(BUF_STRUCT *_this)
{
  return _this->__buf_room;
}

inline static uint16_t BUF__get_buf_size(BUF_STRUCT *_this)
{
  return _this->__buf_size;
}
//==============================================================================


#define BUF_Extern(A)         extern BUF_STRUCT A;

#define BUF_Define(A,SIZE)   \
  static uint8_t BUF_ROOM(A)[SIZE+1]; \
    BUF_STRUCT A={0,0,&BUF_ROOM(A)[0],(SIZE)+1};
  
#define BUF_PRIVATE_Define(A,SIZE)   \
  static uint8_t BUF_ROOM(A)[SIZE+1]; \
  static BUF_STRUCT A={0,0,&BUF_ROOM(A)[0],(SIZE)+1};
  
  
uint16_t BUF_Query(BUF_STRUCT *_this);
uint16_t BUF_QueryFreeSpace(BUF_STRUCT *_this);
bool  BUF_IsEmpty(BUF_STRUCT *_this);
void BUF_Clear(BUF_STRUCT *_this);
void BUF_Discard(BUF_STRUCT *_this,uint16_t d_size);
void BUF_PutUnit(BUF_STRUCT *_this,uint8_t data);
bool BUF_GetUnit(BUF_STRUCT *_this,uint8_t *pdata);
void  BUF_Put(BUF_STRUCT *_this,const uint8_t *pdata,uint16_t size);
uint16_t  BUF_Get(BUF_STRUCT *_this,uint8_t *pdata,uint16_t size);
bool  BUF_PreGetUnit(BUF_STRUCT *_this,uint8_t * pdata);
uint16_t  BUF_PreGet(BUF_STRUCT *_this,uint8_t *pdata, uint16_t size);


#ifdef __cplusplus
}
#endif

#endif
