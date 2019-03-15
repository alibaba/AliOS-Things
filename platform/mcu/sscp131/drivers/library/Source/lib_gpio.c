/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/

#include "lib_gpio.h"
#include "type.h"

void GPIO_SetFunc(GPIO_Pin Pin, GPIO_Func Func)
{
    switch (Pin) {
    case GPIO_Pin_A0:
        GPIO->FUNC0.GPIO0FUNC = Func;
        break;
    case GPIO_Pin_A1:
        GPIO->FUNC0.GPIO1FUNC = Func;
        break;
    case GPIO_Pin_A2:
        GPIO->FUNC0.GPIO2FUNC = Func;
        break;
    case GPIO_Pin_A3:
        GPIO->FUNC0.GPIO3FUNC = Func;
        break;
    case GPIO_Pin_A4:
        GPIO->FUNC0.GPIO4FUNC = Func;
        break;
    case GPIO_Pin_A5:
        GPIO->FUNC0.GPIO5FUNC = Func;
        break;
    case GPIO_Pin_A6:
        GPIO->FUNC0.GPIO6FUNC = Func;
        break;
    case GPIO_Pin_A7:
        GPIO->FUNC0.GPIO7FUNC = Func;
        break;
    case GPIO_Pin_A8:
        GPIO->FUNC1.GPIO8FUNC = Func;
        break;
    case GPIO_Pin_A9:
        GPIO->FUNC1.GPIO9FUNC = Func;
        break;
    case GPIO_Pin_A10:
        GPIO->FUNC1.GPIO10FUNC = Func;
        break;
    case GPIO_Pin_A11:
        GPIO->FUNC1.GPIO11FUNC = Func;
        break;
    case GPIO_Pin_A12:
        GPIO->FUNC1.GPIO12FUNC = Func;
        break;
    case GPIO_Pin_A13:
        GPIO->FUNC1.GPIO13FUNC = Func;
        break;
    case GPIO_Pin_A14:
        GPIO->FUNC1.GPIO14FUNC = Func;
        break;
    case GPIO_Pin_A15:
        GPIO->FUNC1.GPIO15FUNC = Func;
        break;
    case GPIO_Pin_A16:
        GPIO->FUNC2.GPIO16FUNC = Func;
        break;
    case GPIO_Pin_A17:
        GPIO->FUNC2.GPIO17FUNC = Func;
        break;
    case GPIO_Pin_A18:
        GPIO->FUNC2.GPIO18FUNC = Func;
        break;
    case GPIO_Pin_A19:
        GPIO->FUNC2.GPIO19FUNC = Func;
        break;
    case GPIO_Pin_A20:
        GPIO->FUNC2.GPIO20FUNC = Func;
        break;
    case GPIO_Pin_A21:
        GPIO->FUNC2.GPIO21FUNC = Func;
        break;
    case GPIO_Pin_A22:
        GPIO->FUNC2.GPIO22FUNC = Func;
        break;
    case GPIO_Pin_A23:
        GPIO->FUNC2.GPIO23FUNC = Func;
        break;
    case GPIO_Pin_A24:
        GPIO->FUNC3.GPIO24FUNC = Func;
        break;
    case GPIO_Pin_A25:
        GPIO->FUNC3.GPIO25FUNC = Func;
        break;
    case GPIO_Pin_A26:
        GPIO->FUNC3.GPIO26FUNC = Func;
        break;
    case GPIO_Pin_A27:
        GPIO->FUNC3.GPIO27FUNC = Func;
        break;
    case GPIO_Pin_A28:
        GPIO->FUNC3.GPIO28FUNC = Func;
        break;
    case GPIO_Pin_A29:
        GPIO->FUNC3.GPIO29FUNC = Func;
        break;   
    case GPIO_Pin_A30:
        GPIO->FUNC3.GPIO30FUNC = Func;
        break;   
    case GPIO_Pin_A31:
        GPIO->FUNC3.GPIO31FUNC = Func;
        break;       
    default:
        break;
    }
    return;
}

void GPIO_SetDir_OUT(GPIO_Pin Pin)
{

    assert_param(IS_GPIO_PIN(Pin));

    GPIO->DIRBCR.Word = (uint32_t)(1 << Pin);

    return;
}

void GPIO_SetDir_IN(GPIO_Pin Pin)
{

    assert_param(IS_GPIO_PIN(Pin));

    GPIO->DIRBSR.Word = (uint32_t)(1 << Pin);

    return;
}

void GPIO_SetDir(GPIO_Pin Pin, GPIO_Direction Dir)
{
    if (Dir == GPIO_Direction_Input)
        GPIO->DIR.Word |= (1 << Pin);
    else
        GPIO->DIR.Word &= (~(1 << Pin));

    return;
}

void GPIO_DirBrr(GPIO_Pin Pin)
{
    assert_param(IS_GPIO_PIN(Pin));

    GPIO->DIRBRR.Word = (uint32_t)(1 << Pin);
    return;
}

void GPIO_SetODE(GPIO_Pin Pin, GPIO_ODE_Output ODE)
{
    if (ODE == GPIO_ODE_Output_Enable)
        GPIO->ODE.Word |= (1 << Pin);
    else
        GPIO->ODE.Word &= (~(1 << Pin));

    return;
}

void GPIO_SetDS(GPIO_Pin Pin, GPIO_DS_Output DS)
{
    if (DS == GPIO_DS_Output_Strong)
        GPIO->DS.Word |= (1 << Pin);
    else
        GPIO->DS.Word &= (~(1 << Pin));

    return;
}

void GPIO_SetPUE(GPIO_Pin Pin, GPIO_PUE_Input PUE)
{
    if (PUE == GPIO_PUE_Input_Enable)
        GPIO->PUE.Word |= (1 << Pin);
    else
        GPIO->PUE.Word &= (~(1 << Pin));

    return;
}

void GPIO_SetPDE(GPIO_Pin Pin, GPIO_PDE_Input PDE)
{
    if (PDE == GPIO_PDE_Input_Enable)
        GPIO->PDE.Word |= (1 << Pin);
    else
        GPIO->PDE.Word &= (~(1 << Pin));

    return;
}

void GPIO_SetSingalType(GPIO_Pin Pin, GPIO_Signal_Type Signal)
{
    if (Signal == GPIO_Signal_Digital)
        GPIO->INEB.Word &= (~(1 << Pin));   
    else
        GPIO->INEB.Word |= (1 << Pin);

    return;
}

void GPIO_Init(GPIO_Pin Pin, GPIO_InitSettingType *InitSet)
{
    /* check parameters */
    assert_param(IS_GPIO_PIN(Pin));
    assert_param(IS_FUNCEN_STATE(InitSet->Signal));

    /* Init GPIO */
    if (InitSet->Signal == GPIO_Signal_Analog) {
        GPIO_SetSingalType(Pin, GPIO_Signal_Analog);
        GPIO_SetDir(Pin, GPIO_Direction_Input);
        GPIO_SetFunc(Pin, GPIO_Func0);
    } else {
        assert_param(IS_FUNCEN_STATE(InitSet->Dir));
        assert_param(IS_GPIO_FUNC(InitSet->Func));

        GPIO_SetSingalType(Pin, GPIO_Signal_Digital);
        GPIO_SetDir(Pin, InitSet->Dir);
        GPIO_SetFunc(Pin, InitSet->Func);

        if (InitSet->Dir == GPIO_Direction_Output) {
            assert_param(IS_FUNCEN_STATE(InitSet->ODE));
            assert_param(IS_FUNCEN_STATE(InitSet->DS));

            GPIO_SetODE(Pin, InitSet->ODE);
            GPIO_SetDS(Pin, InitSet->DS);
        }
        else {
            assert_param(IS_FUNCEN_STATE(InitSet->PUE));
            assert_param(IS_FUNCEN_STATE(InitSet->PDE));

            GPIO_SetPUE(Pin, InitSet->PUE);
            GPIO_SetPDE(Pin, InitSet->PDE);
        }
    }

    return;
}

uint32_t GPIO_ReadBit(GPIO_Pin Pin)
{
    uint32_t value;

    assert_param(IS_GPIO_PIN(Pin));

    value   = GPIO->PORT.Word;
    value >>= Pin;
    value  &= 0x01;

    return value;
}

void GPIO_SetBit(GPIO_Pin Pin)
{
    assert_param(IS_GPIO_PIN(Pin));

    GPIO->DATABSR.Word = 1<<Pin;

    return;
}

void GPIO_ResetBit(GPIO_Pin Pin)
{
    assert_param(IS_GPIO_PIN(Pin));

    GPIO->DATABCR.Word = 1<<Pin;

    return;
}

void GPIO_WriteBit(GPIO_Pin Pin, uint32_t bit)
{
    uint32_t value;

    assert_param(IS_GPIO_PIN(Pin));

    value  = GPIO->DATA.Word;
    value &= (~(1 << Pin));
    value |= ((bit & 0x01) << Pin);

    GPIO->DATA.Word = value;
    return;
}

void GPIO_DataBrr(GPIO_Pin Pin)
{
    assert_param(IS_GPIO_PIN(Pin));

    GPIO->DATABRR.Word = (uint32_t)(1 << Pin);
    return;
}

void PINT_SetPMASK(GPIO_PINT PINT, PINT_PMASK_Set x)
{
    assert_param(IS_PINT(PINT));
    assert_param(IS_FUNCEN_STATE(x));
    if (x == PINT_PMASK_Enable)
        GPIO->PINTIE.PMASK |= (1 << PINT);
    else
        GPIO->PINTIE.PMASK &= (~(1 << PINT));

    return;
}

void PINT_SetEnable(GPIO_PINT PINT, PINT_IE_Set IE)
{
    assert_param(IS_PINT(PINT));
    assert_param(IS_FUNCEN_STATE(IE));
    if (IE == PINT_IE_Enable)
        GPIO->PINTIE.PINTIE |= (1 << PINT);
    else
        GPIO->PINTIE.PINTIE &= (~(1 << PINT));

    return;
}

ErrorStatus PINT_SetPINTSource(GPIO_PINT PINT, GPIO_Pin Pin)
{
    assert_param(IS_PINT(PINT));
    assert_param(IS_GPIO_PIN(Pin));
    if( (Pin-PINT)%8 !=0 )
    {
        return ERROR;
    }
    switch(PINT)
    {
        case GPIO_PINT0:
            GPIO->PINTSEL.PINT0_SEL = Pin >> 3;
            break;
        case GPIO_PINT1:
            GPIO->PINTSEL.PINT1_SEL = Pin >> 3;
            break;
        case GPIO_PINT2:
            GPIO->PINTSEL.PINT2_SEL = Pin >> 3;
            break;
        case GPIO_PINT3:
            GPIO->PINTSEL.PINT3_SEL = Pin >> 3;
            break;
        case GPIO_PINT4:
            GPIO->PINTSEL.PINT4_SEL = Pin >> 3;
            break;
        case GPIO_PINT5:
            GPIO->PINTSEL.PINT5_SEL = Pin >> 3;
            break;
        case GPIO_PINT6:
            GPIO->PINTSEL.PINT6_SEL = Pin >> 3;
            break;
        case GPIO_PINT7:
            GPIO->PINTSEL.PINT7_SEL = Pin >> 3;
            break;
        default:
            break;
    }

    return SUCCESS;
}

void PINT_SetTriggerStyle(GPIO_PINT PINT, PINT_Trigger_Style Style)
{
    assert_param(IS_PINT(PINT));
    assert_param(IS_PINT_TRIGGER(Style));
    switch (PINT) {
    case GPIO_PINT0:
        GPIO->PINTCFG.PINT0_CFG = Style;
        break;
    case GPIO_PINT1:
        GPIO->PINTCFG.PINT1_CFG = Style;
        break;
    case GPIO_PINT2:
        GPIO->PINTCFG.PINT2_CFG = Style;
        break;
    case GPIO_PINT3:
        GPIO->PINTCFG.PINT3_CFG = Style;
        break;
    case GPIO_PINT4:
        GPIO->PINTCFG.PINT4_CFG = Style;
        break;
    case GPIO_PINT5:
        GPIO->PINTCFG.PINT5_CFG = Style;
        break;
    case GPIO_PINT6:
        GPIO->PINTCFG.PINT6_CFG = Style;
        break;
    case GPIO_PINT7:
        GPIO->PINTCFG.PINT7_CFG = Style;
        break;
    
    default:
        break;
    }

    return;
}

ErrorStatus PINT_Init(GPIO_PINT PINT,GPIO_Pin Pin, PINT_InitSettingType *InitSet)
{
    assert_param(IS_PINT(PINT));
    assert_param(IS_GPIO_PIN(Pin));
    assert_param(IS_PINT_TRIGGER(InitSet->Trigger_Style));
    assert_param(IS_FUNCEN_STATE(InitSet->IE_Set));
    assert_param(IS_FUNCEN_STATE(InitSet->PMASK_Set));
    
    if(PINT_SetPINTSource(PINT,Pin) == ERROR)
        return ERROR;   
    PINT_SetTriggerStyle(PINT, InitSet->Trigger_Style);
    PINT_SetEnable(PINT, InitSet->IE_Set);
    PINT_SetPMASK(PINT,InitSet->PMASK_Set);
    return SUCCESS;
}

void PINT_ClearITFlag(GPIO_PINT PINT)
{
    assert_param(IS_PINT(PINT));

    GPIO->PINTIF.Word = (1 << PINT);
    return;
}

uint32_t PINT_GetITFlag(GPIO_PINT PINT)
{
    int32_t value;

    assert_param(IS_PINT(PINT));

    value = GPIO->PINTIF.Word;
    value >>= PINT;
    value &= 0x01;

    return value;
}

void KINT_SetKMASK(GPIO_KINT KINT, KINT_KMASK_Set x)
{
    assert_param(IS_KINT(PINT));
    assert_param(IS_FUNCEN_STATE(x));
    if (x == KINT_KMASK_Enable)
        GPIO->KINTIE.KMASK |= (1 << KINT);
    else
        GPIO->PINTIE.PMASK &= (~(1 << KINT));

    return;
}

void KINT_SetEnable(GPIO_KINT KINT, KINT_IE_Set IE)
{
    assert_param(IS_KINT(KINT));
    assert_param(IS_FUNCEN_STATE(IE));
    if (IE == KINT_IE_Enable)
        GPIO->KINTIE.KINTIE |= (1 << KINT);
    else
        GPIO->KINTIE.KINTIE &= (~(1 << KINT));

    return;
}

ErrorStatus KINT_SetKINTSource(GPIO_KINT KINT, GPIO_Pin Pin)
{
    assert_param(IS_PINT(PINT));
    assert_param(IS_GPIO_PIN(Pin));
    if( (Pin-KINT)%8 !=0 )
    {
        return ERROR;
    }
    switch(KINT)
    {
        case GPIO_KINT0:
            GPIO->KINTSEL.KINT0_SEL = Pin >> 3;
            break;
        case GPIO_KINT1:
            GPIO->KINTSEL.KINT1_SEL = Pin >> 3;
            break;
        case GPIO_KINT2:
            GPIO->KINTSEL.KINT2_SEL = Pin >> 3;
            break;
        case GPIO_KINT3:
            GPIO->KINTSEL.KINT3_SEL = Pin >> 3;
            break;
        case GPIO_KINT4:
            GPIO->KINTSEL.KINT4_SEL = Pin >> 3;
            break;
        case GPIO_KINT5:
            GPIO->KINTSEL.KINT5_SEL = Pin >> 3;
            break;
        case GPIO_KINT6:
            GPIO->KINTSEL.KINT6_SEL = Pin >> 3;
            break;
        case GPIO_KINT7:
            GPIO->KINTSEL.KINT7_SEL = Pin >> 3;
            break;
        default:
            break;
    }

    return SUCCESS;
}

void KINT_SetTriggerStyle(GPIO_KINT KINT, KINT_Trigger_Style Style)
{
    assert_param(IS_KIN(KINT));
    assert_param(IS_KINT_TRIGGER(Style));
    switch (KINT) {
    case GPIO_KINT0:
        GPIO->KINTCFG.KINT0_CFG = Style;
        break;
    case GPIO_KINT1:
        GPIO->KINTCFG.KINT1_CFG = Style;
        break;
    case GPIO_KINT2:
        GPIO->KINTCFG.KINT2_CFG = Style;
        break;
    case GPIO_KINT3:
        GPIO->KINTCFG.KINT3_CFG = Style;
        break;
    case GPIO_KINT4:
        GPIO->KINTCFG.KINT4_CFG = Style;
        break;
    case GPIO_KINT5:
        GPIO->KINTCFG.KINT5_CFG = Style;
        break;
    case GPIO_KINT6:
        GPIO->KINTCFG.KINT6_CFG = Style;
        break;
    case GPIO_KINT7:
        GPIO->KINTCFG.KINT7_CFG = Style;
        break;
    
    default:
        break;
    }

    return;
}

ErrorStatus KINT_Init(GPIO_KINT KINT,GPIO_Pin Pin, KINT_InitSettingType *InitSet)
{
    assert_param(IS_KINT(KINT));
    assert_param(IS_GPIO_PIN(Pin));
    assert_param(IS_KINT_TRIGGER(InitSet->Trigger_Style));
    assert_param(IS_FUNCEN_STATE(InitSet->IE_Set));
    assert_param(IS_FUNCEN_STATE(InitSet->KMASK_Set));
    
    if(KINT_SetKINTSource(KINT,Pin) == ERROR)
        return ERROR;   
    KINT_SetTriggerStyle(KINT, InitSet->Trigger_Style);
    KINT_SetEnable(KINT, InitSet->IE_Set);
    KINT_SetKMASK(KINT,InitSet->KMASK_Set);
    return SUCCESS;
}

void KINT_ClearITFlag(GPIO_KINT KINT)
{
    assert_param(IS_KINT(KINT));

    GPIO->KINTIF.Word = (1 << KINT);
    return;
}

uint32_t KINT_GetITFlag(GPIO_KINT KINT)
{
    int32_t value;

    assert_param(IS_KINT(KINT));

    value = GPIO->KINTIF.Word;
    value >>= KINT;
    value &= 0x01;

    return value;
}

void GPIO_SetFLT(GPIO_Pin Pin,GPIO_FLT_Set FLT_Set)
{
    uint32_t value;
    assert_param(IS_GPIO_PIN(Pin));
    value  = GPIO->IOFLTSEL.Word;
    if(FLT_Set == FLE_20ns)
        value &= (~(1 << Pin));
    else
        value |= ((FLT_Set & 0x01) << Pin);
    GPIO->IOFLTSEL.Word = value;
}

void GPIO_TXPWMInit(TXPWM_InitSettingType *InitSet)
{
    if(InitSet->TXPWMType == TXPWM0)
    {
        GPIO->TXPWM.TX0PS = InitSet->ModulationType;
        GPIO->TXPWM.TX0_PLV = InitSet->Modulation_level;
        GPIO->TXPWM.TX0_S0 = InitSet->TXD0_TXPWMOUT_EN;
        GPIO->TXPWM.TX0_S1 = InitSet->T16N1OUT0_TXPWMOUT_EN;
        GPIO->TXPWM.TX0_S2 = InitSet->T16N1OUT1_TXPWMOUT_EN;
        GPIO->TXPWM.TX0_S3 = InitSet->BUZ_TXPWMOUT_EN;
    }
    else
    {
        GPIO->TXPWM.TX1PS = InitSet->ModulationType;
        GPIO->TXPWM.TX1_PLV = InitSet->Modulation_level;
        GPIO->TXPWM.TX1_S1 = InitSet->T16N1OUT0_TXPWMOUT_EN;
        GPIO->TXPWM.TX1_S2 = InitSet->T16N1OUT1_TXPWMOUT_EN;
        GPIO->TXPWM.TX1_S3 = InitSet->BUZ_TXPWMOUT_EN;
    }   
    return;
}

void GPIO_setBUZ_LOAD(uint32_t load)
{
    GPIO->BUZC.BUZ_LOAD = load;
}

void GPIO_setBUZ_EN(TYPE_FUNCEN EN)
{
    if(EN== ENABLE)
        GPIO->BUZC.BUZEN = 1;
    else
        GPIO->BUZC.BUZEN = 0;
}
