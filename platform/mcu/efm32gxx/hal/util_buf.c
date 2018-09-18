
#define __UTIL_BUF_C__
#include "util_buf.h"


#define BUF_VOLATILE_OF(A)        (*(volatile uint16_t *)(&(A)))

#define BUF_GUARANTEE_READ(a,A)    do{do{a=BUF_VOLATILE_OF(A);}while(a!=BUF_VOLATILE_OF(A));}while(0)   
#define BUF_GUARANTEE_WRITE(A,a)   do{do{BUF_VOLATILE_OF(A)=a;}while(BUF_VOLATILE_OF(A)!=a);}while(0)  
#define BUF_GUARANTEE_WRITE2(A,B)  do{do{BUF_VOLATILE_OF(A)=BUF_VOLATILE_OF(B);}while(BUF_VOLATILE_OF(A)!=BUF_VOLATILE_OF(B));}while(0) 


uint16_t BUF_Query(BUF_STRUCT *_this)
{
  uint16_t query_size;
  const uint16_t buf_size=_this->__buf_size; 
  
  uint16_t temp_head;
  uint16_t temp_tail;
  
  BUF_GUARANTEE_READ(temp_head,_this->__head);  
  BUF_GUARANTEE_READ(temp_tail,_this->__tail);
  
  if(temp_head >= temp_tail)  
  {
    query_size=temp_head - temp_tail;             
  }
  else
  {
    query_size = temp_head + buf_size - temp_tail;  
  }
  
  return query_size;
}


uint16_t BUF_QueryFreeSpace(BUF_STRUCT *_this)
{
  const uint16_t buf_size=_this->__buf_size;
  return buf_size - 1 - BUF_Query(_this) ;     
}


bool  BUF_IsEmpty(BUF_STRUCT *_this)
{
  uint16_t temp_head;
  uint16_t temp_tail;
  
  BUF_GUARANTEE_READ(temp_head,_this->__head); 
  BUF_GUARANTEE_READ(temp_tail,_this->__tail);  
  
  if(temp_head==temp_tail)   
  {
    return 1;
  }
  
  return 0;
}

void BUF_Clear(BUF_STRUCT *_this)
{  
  BUF_GUARANTEE_WRITE2(_this->__tail,_this->__head);  
}

void BUF_Discard(BUF_STRUCT *_this,uint16_t d_size)
{
  uint16_t query_size;
  const uint16_t buf_size=_this->__buf_size;  
  
  uint16_t temp_head;
  uint16_t temp_tail;
  
  BUF_GUARANTEE_READ(temp_head,_this->__head);  
  BUF_GUARANTEE_READ(temp_tail,_this->__tail);  
  
  if(temp_head>=temp_tail)
  {
    query_size=temp_head-temp_tail;
  }
  else
  {
    query_size = temp_head + buf_size - temp_tail;
  }
  
  
  if (query_size <= d_size)     
  {   
    temp_tail = temp_head;   
  }
  else                           
  {
    temp_tail += d_size;        
    
    if ( temp_tail >= buf_size)  
    {
      temp_tail -= buf_size;    
    }
  }
  
  BUF_GUARANTEE_WRITE(_this->__tail,temp_tail); 
}

void BUF_PutUnit(BUF_STRUCT *_this,uint8_t data)
{
  const uint16_t buf_size=_this->__buf_size;        
  
  uint16_t temp_head;
  uint16_t temp_tail;
  
  BUF_GUARANTEE_READ(temp_head,_this->__head);      
  BUF_GUARANTEE_READ(temp_tail,_this->__tail);      
  
  _this->__buf_room[temp_head]=data;                  
  temp_head=BUF_ABS_ITER_NEXT(temp_head,buf_size);  
  
  BUF_GUARANTEE_WRITE(_this->__head,temp_head);       
  
  if(temp_head==temp_tail)                            
  {
    temp_tail=BUF_ABS_ITER_NEXT(temp_tail,buf_size);  
    BUF_GUARANTEE_WRITE(_this->__tail,temp_tail);     
  } 
}


bool BUF_GetUnit(BUF_STRUCT *_this,uint8_t *pdata)
{
  const uint16_t buf_size=_this->__buf_size;      
  
  uint16_t temp_head;
  uint16_t temp_tail;
  
  BUF_GUARANTEE_READ(temp_head,_this->__head);      
  BUF_GUARANTEE_READ(temp_tail,_this->__tail);       
  
  if(temp_head==temp_tail)                          
  {
    return 0;
  }
  
  *pdata=_this->__buf_room[temp_tail];               
  temp_tail=BUF_ABS_ITER_NEXT(temp_tail,buf_size);   
  
  BUF_GUARANTEE_WRITE(_this->__tail,temp_tail);  
  
  return 1;
}


void  BUF_Put(BUF_STRUCT *_this,const uint8_t *pdata,uint16_t size)
{
  uint16_t i;
  for (i=0; i<size; i++)          
  {
    BUF_PutUnit(_this,*pdata);
    pdata++;
  }
}

uint16_t  BUF_Get(BUF_STRUCT *_this,uint8_t *pdata,uint16_t size)
{
  uint16_t i;
  
  for (i=0; i<size; i++)
  {
    if (!BUF_GetUnit(_this,&pdata[i]))
    {
      break;
    }
  }
  return i;
}

bool  BUF_PreGetUnit(BUF_STRUCT *_this,uint8_t * pdata)
{
  uint16_t temp_head;
  uint16_t temp_tail;
  
  BUF_GUARANTEE_READ(temp_head,_this->__head);     
  BUF_GUARANTEE_READ(temp_tail,_this->__tail);    
  
  if(temp_head==temp_tail)                         
  {
    return 0;
  }
  
  *pdata=_this->__buf_room[temp_tail];           
  
  return 1;
}


uint16_t  BUF_PreGet(BUF_STRUCT *_this,uint8_t *pdata, uint16_t size)
{
  uint16_t i;
  const uint16_t buf_size=_this->__buf_size;  
  
  uint16_t temp_head;
  uint16_t temp_tail;
  
  BUF_GUARANTEE_READ(temp_head,_this->__head);  
  BUF_GUARANTEE_READ(temp_tail,_this->__tail);  
  
  for ( i=0;i<size;i++)
  {
    if ( temp_tail==temp_head)                   
    {
      break;
    }
    
    *pdata=_this->__buf_room[temp_tail];     
    pdata++;
    
    temp_tail=BUF_ABS_ITER_NEXT(temp_tail,buf_size);  
  }
  
  return i;
}
