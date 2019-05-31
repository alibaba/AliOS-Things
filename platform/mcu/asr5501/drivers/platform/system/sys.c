#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////     

//////////////////////////////////////////////////////////////////////////////////  


  
__asm void WFI_SET(void)
{
    WFI;          
}

__asm void INTX_DISABLE(void)
{
    CPSID   I
    BX      LR      
}

__asm void INTX_ENABLE(void)
{
    CPSIE   I
    BX      LR  
}

__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0             //set Main Stack value
    BX r14
}


// below code is for no need to select microlib
#if 0
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
 struct __FILE 
{ 
    int handle; 
}; 

 FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
    x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{     
    while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
    return ch;
}
#endif













