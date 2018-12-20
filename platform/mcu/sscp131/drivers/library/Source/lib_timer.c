/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "lib_timer.h"

void T16Nx_BaseInit(T16N_TypeDef* T16Nx,T16Nx_BaseInitStruType* T16Nx_BaseInitStruct)
{
    T16Nx->CON0.CS = T16Nx_BaseInitStruct->T16Nx_ClkS;
    T16Nx->CON0.SYNC = T16Nx_BaseInitStruct->T16Nx_SYNC;
    T16Nx->CON0.EDGE = T16Nx_BaseInitStruct->T16Nx_EDGE;
    T16Nx->CON0.MOD = T16Nx_BaseInitStruct->T16Nx_Mode;
    T16Nx->PREMAT.PREMAT = T16Nx_BaseInitStruct->T16Nx_PREMAT-1;
}

void T16Nx_CapInit(T16N_TypeDef* T16Nx,T16Nx_CapInitStruType* T16Nx_CapInitStruct)
{
        
    T16Nx->CON1.CAPL0=T16Nx_CapInitStruct->T16Nx_CAPCAPL0;
    T16Nx->CON1.CAPL1=T16Nx_CapInitStruct->T16Nx_CAPCAPL1;
    T16Nx->CON1.CAPPE=T16Nx_CapInitStruct->T16Nx_CapRise;
    T16Nx->CON1.CAPNE=T16Nx_CapInitStruct->T16Nx_CapFall;
    T16Nx->CON1.CAPIS0=T16Nx_CapInitStruct->T16Nx_CapIS0;
    T16Nx->CON1.CAPIS1=T16Nx_CapInitStruct->T16Nx_CapIS1;
    T16Nx->CON1.CAPT=T16Nx_CapInitStruct->T16Nx_CapTime;
    
}

void T16Nx_PMWOutInit(T16N_TypeDef* T16Nx,T16Nx_PWMInitStruType* T16Nx_PWMInitStruct)
{
    T16Nx->CON2.MOE0 = T16Nx_PWMInitStruct->T16Nx_MOE0;
    T16Nx->CON2.MOE1 = T16Nx_PWMInitStruct->T16Nx_MOE1;
    T16Nx->CON2.POL0 = T16Nx_PWMInitStruct->T16Nx_POL0;
    T16Nx->CON2.POL1 = T16Nx_PWMInitStruct->T16Nx_POL1;
    T16Nx->CON2.PWMMOD = T16Nx_PWMInitStruct->T16Nx_PWMMODE;
    T16Nx->CON2.PWMDZE = T16Nx_PWMInitStruct->PWMDZE;
}

void T16Nx_PWMPDZ_Config(T16N_TypeDef* T16Nx,unsigned int PWM_PDZ_data)
{
    T16Nx->PDZ.PDZ = PWM_PDZ_data-1;
}

void T16Nx_MAT0ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type)
{  
      T16Nx->CON0.MAT0S=Type;
}
void T16Nx_MAT1ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type)
{
    T16Nx->CON0.MAT1S=Type;
}
void T16Nx_MAT2ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type)
{
    T16Nx->CON0.MAT2S=Type;
}
void T16Nx_MAT3ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATCON Type)
{
    T16Nx->CON0.MAT3S=Type;
}

void T16Nx_MAT0Out0Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type)
{
      T16Nx->CON2.MOM0=Type;
}
void T16Nx_MAT1Out0Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type)
{
    T16Nx->CON2.MOM1=Type;
}
void T16Nx_MAT2Out1Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type)
{
    T16Nx->CON2.MOM2=Type;
}
void T16Nx_MAT3Out1Config(T16N_TypeDef* T16Nx,T16Nx_TYPE_MATOUT Type)
{
    T16Nx->CON2.MOM3=Type;
}

void T16Nx_ITConfig(T16N_TypeDef* T16Nx,T16Nx_TYPE_IT Type,TYPE_FUNCEN NewState)
{
     if (NewState != DISABLE)
    {
        T16Nx->IE.Word = (T16Nx->IE.Word & (uint32_t)0x0000003FF) | (uint32_t)Type;
    }
    else
    {
        T16Nx->IE.Word = (T16Nx->IE.Word  & (uint32_t)0x0000003FF) & (~(uint32_t)Type);
    }
}

void T16Nx_SetCNT0(T16N_TypeDef* T16Nx,uint16_t Value)
{
    T16Nx->CON0.ASYWEN = ENABLE;
    T16Nx->CNT0.CNT0 = Value;
    T16Nx->CON0.ASYWEN = ENABLE;
} 

void T16Nx_SetCNT1(T16N_TypeDef* T16Nx,uint16_t Value)
{
    T16Nx->CON0.ASYWEN = ENABLE;
    T16Nx->CNT1.CNT1 = Value;
    T16Nx->CON0.ASYWEN = DISABLE;
} 

void T16Nx_SetPREMAT(T16N_TypeDef* T16Nx,uint8_t Value)
{
    T16Nx->PREMAT.PREMAT = Value-1;  
}

void T16Nx_SetPRECNT(T16N_TypeDef* T16Nx,uint8_t Value)
{
        T16Nx->CON0.ASYWEN = ENABLE;
    T16Nx->PRECNT.PRECNT = Value;   
        T16Nx->CON0.ASYWEN = DISABLE;
}

void T16Nx_SetMAT0(T16N_TypeDef* T16Nx,uint16_t Value)
{
        
    T16Nx->MAT0.MAT0=Value;
}

void T16Nx_SetMAT1(T16N_TypeDef* T16Nx,uint16_t Value)
{
 
      T16Nx->MAT1.MAT1=Value;
}
void T16Nx_SetMAT2(T16N_TypeDef* T16Nx,uint16_t Value)
{
 
      T16Nx->MAT2.MAT2=Value;
}
void T16Nx_SetMAT3(T16N_TypeDef* T16Nx,uint16_t Value)
{
   
      T16Nx->MAT3.MAT3=Value;
}

void T16Nx_SetTOP0(T16N_TypeDef* T16Nx,uint16_t Value)
{
        
    T16Nx->TOP0.TOP0=Value;
}

void T16Nx_SetTOP1(T16N_TypeDef* T16Nx,uint16_t Value)
{
 
      T16Nx->TOP1.TOP1=Value;
}

uint16_t T16Nx_GetMAT0(T16N_TypeDef* T16Nx)
{
    return (T16Nx->MAT0.MAT0) ;
      
}

uint16_t T16Nx_GetMAT1(T16N_TypeDef* T16Nx)
{
    return (T16Nx->MAT1.MAT1) ;
     
}
uint16_t T16Nx_GetMAT2(T16N_TypeDef* T16Nx)
{
    return (T16Nx->MAT2.MAT2) ;
     
}
uint16_t T16Nx_GetMAT3(T16N_TypeDef* T16Nx)
{
    return (T16Nx->MAT3.MAT3) ;
     
}

uint16_t T16Nx_GetTOP0(T16N_TypeDef* T16Nx)
{
    return (T16Nx->TOP0.TOP0) ;
      
}

uint16_t T16Nx_GetTOP1(T16N_TypeDef* T16Nx)
{
    return (T16Nx->TOP1.TOP1) ;
     
}

uint16_t T16Nx_GetCNT0(T16N_TypeDef* T16Nx)
{
    return (T16Nx->CNT0.CNT0) ;
}

uint16_t T16Nx_GetCNT1(T16N_TypeDef* T16Nx)
{
    return (T16Nx->CNT1.CNT1) ;
}

FlagStatus T16Nx_GetFlagStatus(T16N_TypeDef* T16Nx,T16Nx_TYPE_IT T16Nx_Flag)
{
    FlagStatus bitstatus = RESET;
    
    if((T16Nx->IF.Word  & (uint32_t)T16Nx_Flag) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else   
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

void T16Nx_ClearITPendingBit(T16N_TypeDef* T16Nx,T16Nx_TYPE_IT T16Nx_Flag)
{
    T16Nx->IF.Word = (T16Nx->IF.Word &(uint32_t)0x000003FF)|(uint32_t)T16Nx_Flag;
}  

void T16Nx_Enable(T16N_TypeDef* T16Nx)
{
    T16Nx->CON0.EN = 1;
}  

void T16Nx_Disable(T16N_TypeDef* T16Nx)
{
    T16Nx->CON0.EN = 0;
}  

void T16Nx_PWMBK_Config(T16N_TypeDef* T16Nx,T16Nx_PWMBK_Type* type)
{
    
        T16Nx->CON2.PWMBKL0 = type->T16Nx_PWMBK_L0;
        T16Nx->CON2.PWMBKL1 = type->T16Nx_PWMBK_L1;
        T16Nx->CON2.PWMBKP0 = type->T16Nx_PWMBK_P0;
        T16Nx->CON2.PWMBKP1 = type->T16Nx_PWMBK_P1;
        T16Nx->CON2.PWMBKE0 = type->T16Nx_PWMBK_EN0;
        T16Nx->CON2.PWMBKE1 = type->T16Nx_PWMBK_EN1;
}

FlagStatus T16Nx_GetPWMBKF(T16N_TypeDef* T16Nx)
{
    if(T16Nx->CON2.PWMBKF == 1)
        return SET;
    else
        return RESET;
}

void T16Nx_ResetPWMBKF(T16N_TypeDef* T16Nx)
{
    T16Nx->CON2.PWMBKF = 1;
}

void T16Nx_PTR_Config(T16N_TypeDef* T16Nx,T16Nx_PWMTRE_type Type,TYPE_FUNCEN NewState )
{
    if (NewState != DISABLE)
    {
                T16Nx->PTR.Word = (T16Nx->PTR.Word & (uint32_t)0x0000000EE) | (uint32_t)Type;
    }
    else
    {
                T16Nx->PTR.Word = (T16Nx->PTR.Word & (uint32_t)0x0000000EE) & (~(uint32_t)Type);
    }
}

void T32Nx_BaseInit(T32N_TypeDef* T32Nx,T32Nx_BaseInitStruType* T32Nx_BaseInitStruct)
{
    
    T32Nx->CON0.CS         = T32Nx_BaseInitStruct->T32Nx_ClkS;
    T32Nx->CON0.SYNC       = T32Nx_BaseInitStruct->T32Nx_SYNC;
    T32Nx->CON0.EDGE       = T32Nx_BaseInitStruct->T32Nx_EDGE;
    T32Nx->CON0.MOD        = T32Nx_BaseInitStruct->T32Nx_Mode;
    T32Nx->PREMAT.PREMAT   = T32Nx_BaseInitStruct->T32Nx_PREMAT-1;
}

void T32Nx_CapInit(T32N_TypeDef* T32Nx,T32Nx_CapInitStruType* T32Nx_CapInitStruct)
{
        
    T32Nx->CON1.CAPL0=T32Nx_CapInitStruct->T32Nx_CAPCAPL0;
    T32Nx->CON1.CAPL1=T32Nx_CapInitStruct->T32Nx_CAPCAPL1;
    T32Nx->CON1.CAPPE=T32Nx_CapInitStruct->T32Nx_CapRise;
    T32Nx->CON1.CAPNE=T32Nx_CapInitStruct->T32Nx_CapFall;
    T32Nx->CON1.CAPIS0=T32Nx_CapInitStruct->T32Nx_CapIS0;
    T32Nx->CON1.CAPIS1=T32Nx_CapInitStruct->T32Nx_CapIS1;
    T32Nx->CON1.CAPT=T32Nx_CapInitStruct->T32Nx_CapTime;
}

void T32Nx_PMWOutInit(T32N_TypeDef* T32Nx,T32Nx_PWMInitStruType* T32Nx_PWMInitStruct)
{
    T32Nx->CON1.MOE0 = T32Nx_PWMInitStruct->T32Nx_MOE0;
    T32Nx->CON1.MOE1 = T32Nx_PWMInitStruct->T32Nx_MOE1;

}

void T32Nx_MAT0ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type)
{  
      T32Nx->CON0.MAT0S=Type;
}
void T32Nx_MAT1ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type)
{
    T32Nx->CON0.MAT1S=Type;
}
void T32Nx_MAT2ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type)
{
    T32Nx->CON0.MAT2S=Type;
}
void T32Nx_MAT3ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATCON Type)
{
    T32Nx->CON0.MAT3S=Type;
}

void T32Nx_MAT0Out0Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type)
{
      T32Nx->CON1.MOM0=Type;
}
void T32Nx_MAT1Out0Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type)
{
    T32Nx->CON1.MOM1=Type;
}
void T32Nx_MAT2Out1Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type)
{
    T32Nx->CON1.MOM2=Type;
}
void T32Nx_MAT3Out1Config(T32N_TypeDef* T32Nx,T32Nx_TYPE_MATOUT Type)
{
    T32Nx->CON1.MOM3=Type;
}

void T32Nx_ITConfig(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT Type,TYPE_FUNCEN NewState)
{
     if (NewState != DISABLE)
    {
        T32Nx->IE.Word = (T32Nx->IE.Word & (uint32_t)0x00000007F) | (uint32_t)Type;
    }
    else
    {
        T32Nx->IE.Word = (T32Nx->IE.Word  & (uint32_t)0x00000007F) & (~(uint32_t)Type);
    }
}

void T32Nx_SetCNT(T32N_TypeDef* T32Nx,uint32_t Value)
{
    T32Nx->CON0.ASYNC_WREN = ENABLE;  
    T32Nx->CNT.CNT0 = Value;
    T32Nx->CON0.ASYNC_WREN = DISABLE;  
} 

void T32Nx_SetPREMAT(T32N_TypeDef* T32Nx,uint8_t Value)
{
    T32Nx->PREMAT.PREMAT = Value-1;  
}

void T32Nx_SetPRECNT(T32N_TypeDef* T32Nx,uint8_t Value)
{
    T32Nx->CON0.ASYNC_WREN = ENABLE; 
    T32Nx->PRECNT.PRECNT = Value;    
    T32Nx->CON0.ASYNC_WREN = DISABLE; 
}

void T32Nx_SetMAT0(T32N_TypeDef* T32Nx,uint32_t Value)
{
        
    T32Nx->MAT0.MAT0=Value;
}

void T32Nx_SetMAT1(T32N_TypeDef* T32Nx,uint32_t Value)
{
 
      T32Nx->MAT1.MAT1=Value;
}
void T32Nx_SetMAT2(T32N_TypeDef* T32Nx,uint32_t Value)
{
 
      T32Nx->MAT2.MAT2=Value;
}
void T32Nx_SetMAT3(T32N_TypeDef* T32Nx,uint32_t Value)
{
   
      T32Nx->MAT3.MAT3=Value;
}

uint32_t T32Nx_GetMAT0(T32N_TypeDef* T32Nx)
{
    return (T32Nx->MAT0.MAT0) ;
      
}

uint32_t T32Nx_GetMAT1(T32N_TypeDef* T32Nx)
{
    return (T32Nx->MAT1.MAT1) ;
     
}
uint32_t T32Nx_GetMAT2(T32N_TypeDef* T32Nx)
{
    return (T32Nx->MAT2.MAT2) ;
     
}
uint32_t T32Nx_GetMAT3(T32N_TypeDef* T32Nx)
{
    return (T32Nx->MAT3.MAT3) ;
     
}

uint16_t T32Nx_GetCNT(T32N_TypeDef* T32Nx)
{
    return (T32Nx->CNT.CNT0) ;
}

FlagStatus T32Nx_GetFlagStatus(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT T32Nx_Flag)
{
    FlagStatus bitstatus = RESET;
    
    if((T32Nx->IF.Word  & (uint32_t)T32Nx_Flag) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else   
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

void T32Nx_ClearITPendingBit(T32N_TypeDef* T32Nx,T32Nx_TYPE_IT TIM_Flag)
{
    T32Nx->IF.Word = (T32Nx->IF.Word &(uint32_t)0x0000007F)|(uint32_t)TIM_Flag;
}  

void T32Nx_Enable(T32N_TypeDef* T32Nx)
{
    T32Nx->CON0.EN = 1;
}  

void T32Nx_Disable(T32N_TypeDef* T32Nx)
{
    T32Nx->CON0.EN = 0;
}  

