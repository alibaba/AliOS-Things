/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

#include "lib_timer.h"

void T16Nx_BaseInit(T16N_TypeDef* T16Nx,TIM_BaseInitStruType* TIM_BaseInitStruct)
{
    T16Nx->CON0.CS = TIM_BaseInitStruct->TIM_ClkS;
    T16Nx->CON0.SYNC = TIM_BaseInitStruct->TIM_SYNC;
    T16Nx->CON0.EDGE = TIM_BaseInitStruct->TIM_EDGE;
    T16Nx->CON0.MOD = TIM_BaseInitStruct->TIM_Mode; 
}

void T32Nx_BaseInit(T32N_TypeDef* T32Nx,TIM_BaseInitStruType* TIM_BaseInitStruct)
{
    T32Nx->CON0.CS = TIM_BaseInitStruct->TIM_ClkS;
    T32Nx->CON0.SYNC = TIM_BaseInitStruct->TIM_SYNC;
    T32Nx->CON0.EDGE = TIM_BaseInitStruct->TIM_EDGE;
    T32Nx->CON0.MOD = TIM_BaseInitStruct->TIM_Mode; 
}

void T16Nx_CapInit(T16N_TypeDef* T16Nx,TIM_CapInitStruType* TIM_CapInitStruct)
{
    T16Nx->CON1.CAPPE=TIM_CapInitStruct->TIM_CapRise;
    T16Nx->CON1.CAPNE=TIM_CapInitStruct->TIM_CapFall;
    T16Nx->CON1.CAPIS0=TIM_CapInitStruct->TIM_CapIS0;
    T16Nx->CON1.CAPIS1=TIM_CapInitStruct->TIM_CapIS1;
    T16Nx->CON1.CAPT=TIM_CapInitStruct->TIM_CapTime;

}

void T32Nx_CapInit(T32N_TypeDef* T32Nx,TIM_CapInitStruType* TIM_CapInitStruct)
{
    T32Nx->CON1.CAPPE=TIM_CapInitStruct->TIM_CapRise;
    T32Nx->CON1.CAPNE=TIM_CapInitStruct->TIM_CapFall;
    T32Nx->CON1.CAPIS0=TIM_CapInitStruct->TIM_CapIS0;
    T32Nx->CON1.CAPIS1=TIM_CapInitStruct->TIM_CapIS1;
    T32Nx->CON1.CAPT=TIM_CapInitStruct->TIM_CapTime;

}

void T16Nx_MAT0ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_MATCON Type)
{  
    T16Nx->CON0.MAT0S=Type;
}
void T16Nx_MAT1ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_MATCON Type)
{
    T16Nx->CON0.MAT1S=Type;
}
void T16Nx_MAT2ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_MATCON Type)
{
    T16Nx->CON0.MAT2S=Type;
}
void T16Nx_MAT3ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_MATCON Type)
{
    T16Nx->CON0.MAT3S=Type;
}

void T32Nx_MAT0ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_MATCON Type)
{  
    T32Nx->CON0.MAT0S=Type;
}
void T32Nx_MAT1ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_MATCON Type)
{
    T32Nx->CON0.MAT1S=Type;
}
void T32Nx_MAT2ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_MATCON Type)
{
    T32Nx->CON0.MAT2S=Type;
}
void T32Nx_MAT3ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_MATCON Type)
{
    T32Nx->CON0.MAT3S=Type;
}

void T16Nx_MAT0Out0Config(T16N_TypeDef* T16Nx,TIM_TYPE_MATOUT Type)
{
    T16Nx->CON1.MOM0=Type;
}
void T16Nx_MAT1Out0Config(T16N_TypeDef* T16Nx,TIM_TYPE_MATOUT Type)
{
    T16Nx->CON1.MOM1=Type;
}
void T16Nx_MAT2Out1Config(T16N_TypeDef* T16Nx,TIM_TYPE_MATOUT Type)
{
    T16Nx->CON1.MOM2=Type;
}
void T16Nx_MAT3Out1Config(T16N_TypeDef* T16Nx,TIM_TYPE_MATOUT Type)
{
    T16Nx->CON1.MOM3=Type;
}

void T32Nx_MAT0Out0Config(T32N_TypeDef* T32Nx,TIM_TYPE_MATOUT Type)
{
    T32Nx->CON1.MOM0=Type;
}
void T32Nx_MAT1Out0Config(T32N_TypeDef* T32Nx,TIM_TYPE_MATOUT Type)
{
    T32Nx->CON1.MOM1=Type;
}
void T32Nx_MAT2Out1Config(T32N_TypeDef* T32Nx,TIM_TYPE_MATOUT Type)
{
    T32Nx->CON1.MOM2=Type;
}
void T32Nx_MAT3Out1Config(T32N_TypeDef* T32Nx,TIM_TYPE_MATOUT Type)
{
    T32Nx->CON1.MOM3=Type;
}

void T16Nx_ITConfig(T16N_TypeDef* T16Nx,TIM_TYPE_IT Type,TYPE_FUNCEN NewState)
{
    if (NewState != DISABLE)
    {
        T16Nx->IE.Word |= (uint32_t)Type;
    }
    else
    {
        T16Nx->IE.Word &= (~(uint32_t)Type);
    }
}

void T32Nx_ITConfig(T32N_TypeDef* T32Nx,TIM_TYPE_IT Type,TYPE_FUNCEN NewState)
{
    if (NewState != DISABLE)
    {
        T32Nx->IE.Word|= (uint32_t)Type;
    }
    else
    {
        T32Nx->IE.Word &= (~(uint32_t)Type);
    }
}

void T16Nx_PWMOutConfig(TIM_TYPE_PWMOC Pwms,PWM_TYPE_OUT PwmType,TYPE_LEVEL PwmLevel)
{
    uint32_t Prot_Temp;
    //Prot_Temp = GPIOPROT->PROT;
    Prot_Temp=GPIO->PROT.Word;

    if(Prot_Temp != 0)
    {GPIO->PROT.Word = 0x78879669;}

    switch (Pwms)
    {
        case T16N0_OUT :
            GPIO->TXPC.TX0PLV=PwmLevel;
            GPIO->TXPC.TX0PS=PwmType;
            break;

        case T16N1_OUT :
            GPIO->TXPC.TX1PLV=PwmLevel;
            GPIO->TXPC.TX1PS=PwmType;
            break;

        case T16N2_OUT :
            GPIO->TXPC.TX2PLV=PwmLevel;
            GPIO->TXPC.TX2PS=PwmType;
            break;  

        case T16N3_OUT :
            GPIO->TXPC.TX3PLV=PwmLevel;
            GPIO->TXPC.TX3PS=PwmType;
            break; 
        default:
            break;            
    }
    if(Prot_Temp != 0)
    {GPIO->PROT.Word  = 0;}

}

void T16Nx_SetCNT(T16N_TypeDef* T16Nx,uint16_t Value)
{
    T16Nx->CNT.CNT = Value;
} 

void T32Nx_SetCNT(T32N_TypeDef* T32Nx,uint32_t Value)
{
    T32Nx->CNT.CNT = Value;
} 

void T16Nx_SetPRECNT(T16N_TypeDef* T16Nx,uint8_t Value)
{
    T16Nx->PRECNT.PRECNT = Value;
} 

void T32Nx_SetPRECNT(T32N_TypeDef* T32Nx,uint8_t Value)
{
    T32Nx->PRECNT.PRECNT = Value;
} 

void T16Nx_SetPREMAT(T16N_TypeDef* T16Nx,uint8_t Value)
{
    T16Nx->PREMAT.PREMAT = Value;

} 

void T32Nx_SetPREMAT(T32N_TypeDef* T32Nx,uint8_t Value)
{
    T32Nx->PREMAT.PREMAT = Value;

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

uint16_t T16Nx_GetCNT(T16N_TypeDef* T16Nx)
{
    return (T16Nx->CNT.CNT) ;
}

uint32_t T32Nx_GetCNT(T32N_TypeDef* T32Nx)
{
    return (T32Nx->CNT.CNT) ;
}

uint8_t T16Nx_GetPRECNT(T16N_TypeDef* T16Nx)
{
    return (uint8_t)(T16Nx->PRECNT.PRECNT) ;
}

uint8_t T32Nx_GetPRECNT(T32N_TypeDef* T32Nx)
{
    return (uint8_t)(T32Nx->PRECNT.PRECNT) ;
}

FlagStatus T16Nx_GetFlagStatus(T16N_TypeDef* T16Nx,TIM_TYPE_IT TIM_Flag)
{
    FlagStatus bitstatus = RESET;

    if((T16Nx->IF.Word  & (uint32_t)TIM_Flag) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else   
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

FlagStatus T32Nx_GetFlagStatus(T32N_TypeDef* T32Nx,TIM_TYPE_IT TIM_Flag)
{
    FlagStatus bitstatus = RESET;

    if((T32Nx->IF.Word  & (uint32_t)TIM_Flag) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else   
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

ITStatus T16Nx_GetITStatus(T16N_TypeDef* T16Nx,TIM_TYPE_IT TIM_Flag)
{
    ITStatus bitstatus = RESET;

    uint32_t itstatus = 0x0, itenable = 0x0;

    itstatus = T16Nx->IF.Word  & (uint32_t)TIM_Flag;
    itenable = T16Nx->IE.Word & (uint32_t)TIM_Flag;
    if ((itstatus != (uint32_t)RESET) && (itenable != (uint32_t)RESET))
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return  bitstatus;    
}

ITStatus T32Nx_GetITStatus(T32N_TypeDef* T32Nx,TIM_TYPE_IT TIM_Flag)
{
    ITStatus bitstatus = RESET;

    uint32_t itstatus = 0x0, itenable = 0x0;

    itstatus = T32Nx->IF.Word  & (uint32_t)TIM_Flag;
    itenable = T32Nx->IE.Word  & (uint32_t)TIM_Flag;
    if ((itstatus != (uint32_t)RESET) && (itenable != (uint32_t)RESET))
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return  bitstatus;    
}

void T16Nx_ClearITPendingBit(T16N_TypeDef* T16Nx,TIM_TYPE_IT TIM_Flag)
{
    T16Nx->IF.Word = (uint32_t)TIM_Flag;
}  

void T32Nx_ClearITPendingBit(T32N_TypeDef* T32Nx,TIM_TYPE_IT TIM_Flag)
{
    T32Nx->IF.Word = (uint32_t)TIM_Flag;
}  

/*************************END OF FILE**********************/
