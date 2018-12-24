/**
  ******************************************************************************
  * @file    fm33a00xx_lcd.c
  * @author  FM33a0xx Application Team
  * @version V2.0.2
  * @date    11-1-2018
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of....:
  *
*/
#include "fm33A0xx_lcd.h" 

//Code_Start
//2018-04-02-13-21-29
//FM33A0XX_Driver_Gen_V1.4

/*********************************
LCD 显示控制函数 
功能：SEG/COM在LCD关闭情况下接地控制，防极化
输入: ENABLE/DISABLE	COM和SEG在LCD关闭下接地/浮空
输出: 无
*********************************/
void LCD_DISPCTRL_ANTIPOLAR_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->DISPCTRL |= (LCD_DISPCTRL_ANTIPOLAR_Msk);
	}
	else
	{
		LCD->DISPCTRL &= ~(LCD_DISPCTRL_ANTIPOLAR_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：获取SEG/COM在LCD关闭情况下接地状态
输入: 无
输出: ENABLE/DISABLE	COM和SEG在LCD关闭下接地/浮空
*********************************/
FunState LCD_DISPCTRL_ANTIPOLAR_Getable(void)
{
	if (LCD->DISPCTRL & (LCD_DISPCTRL_ANTIPOLAR_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：LCD显示功能使能控制
输入: ENABLE/DISABLE	LCD显示使能/LCD显示禁止
输出: 无
*********************************/
void LCD_DISPCTRL_LCDEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->DISPCTRL |= (LCD_DISPCTRL_LCDEN_Msk);
	}
	else
	{
		LCD->DISPCTRL &= ~(LCD_DISPCTRL_LCDEN_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：获取LCD显示功能使能控制
输入: 无
输出: ENABLE/DISABLE	LCD显示使能/LCD显示禁止
*********************************/
FunState LCD_DISPCTRL_LCDEN_Getable(void)
{
	if (LCD->DISPCTRL & (LCD_DISPCTRL_LCDEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：LCD显示闪烁使能控制
输入: ENABLE/DISABLE	LCD显示闪烁使能/LCD显示闪烁禁止
输出: 无
*********************************/
void LCD_DISPCTRL_FLICK_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->DISPCTRL |= (LCD_DISPCTRL_FLICK_Msk);
	}
	else
	{
		LCD->DISPCTRL &= ~(LCD_DISPCTRL_FLICK_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：获取LCD显示闪烁功能使能控制
输入: 无
输出: ENABLE/DISABLE	LCD显示闪烁使能/LCD显示闪烁禁止
*********************************/
FunState LCD_DISPCTRL_FLICK_Getable(void)
{
	if (LCD->DISPCTRL & (LCD_DISPCTRL_FLICK_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：LCD测试模式下显示控制(仅在DISPMD=1的情况下有效)
输入: ENABLE/DISABLE	LCD测试模式下全亮/LCD测试模式下全灭
输出: 无
*********************************/
void LCD_DISPCTRL_TEST_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->DISPCTRL |= (LCD_DISPCTRL_TEST_Msk);
	}
	else
	{
		LCD->DISPCTRL &= ~(LCD_DISPCTRL_TEST_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：获取LCD测试模式下显示控制(仅在DISPMD=1的情况下有效)
输入: 无
输出: ENABLE/DISABLE	LCD测试模式下全亮/LCD测试模式下全灭
*********************************/
FunState LCD_DISPCTRL_TEST_Getable(void)
{
	if (LCD->DISPCTRL & (LCD_DISPCTRL_TEST_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：LCD模式选择
输入: ENABLE/DISABLE	测试模式/正常模式
输出: 无
*********************************/
void LCD_DISPCTRL_DISPMD_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->DISPCTRL |= (LCD_DISPCTRL_DISPMD_Msk);
	}
	else
	{
		LCD->DISPCTRL &= ~(LCD_DISPCTRL_DISPMD_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：LCD模式选择
输入: 无
输出: ENABLE/DISABLE	测试模式/正常模式
*********************************/
FunState LCD_DISPCTRL_DISPMD_Getable(void)
{
	if (LCD->DISPCTRL & (LCD_DISPCTRL_DISPMD_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：LCD测试控制位(仅在DISPMD=1的情况下有效)
输入: LCD_LCDTEST_LCCTRL_0/LCD_LCDTEST_LCCTRL_1 LCD电平由正常寄存器设置/LCD电平由测试寄存器设置
输出: 无
*********************************/
void LCD_LCDTEST_LCCTRL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDTEST;
	tmpreg &= ~(LCD_LCDTEST_LCCTRL_Msk);
	tmpreg |= (SetValue & LCD_LCDTEST_LCCTRL_Msk);
	LCD->LCDTEST = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD测试控制位(仅在DISPMD=1的情况下有效)
输入: 无
输出: LCD_LCDTEST_LCCTRL_0/LCD_LCDTEST_LCCTRL_1 LCD电平由正常寄存器设置/LCD电平由测试寄存器设置
*********************************/
uint32_t LCD_LCDTEST_LCCTRL_Get(void)
{
	return (LCD->LCDTEST & LCD_LCDTEST_LCCTRL_Msk);
}

/*********************************
LCD 显示控制函数 
功能：LCD测试模式使能控制(仅在DISPMD=1的情况下有效)
输入: ENABLE/DISABLE 	LCD测试模式使能/LCD测试模式禁止
输出: 无
*********************************/
void LCD_LCDTEST_TESTEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->LCDTEST |= (LCD_LCDTEST_TESTEN_Msk);
	}
	else
	{
		LCD->LCDTEST &= ~(LCD_LCDTEST_TESTEN_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：LCD测试模式使能控制(仅在DISPMD=1的情况下有效)
输入: 无
输出: ENABLE/DISABLE 	LCD测试模式使能/LCD测试模式禁止
*********************************/
FunState LCD_LCDTEST_TESTEN_Getable(void)
{
	if (LCD->LCDTEST & (LCD_LCDTEST_TESTEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：配置LCD显示频率
输入: 00-FF
输出: 无
*********************************/
void LCD_DF_Write(uint32_t SetValue)
{
	LCD->DF = (SetValue & LCD_DF_DF_Msk);
}
/*********************************
LCD 显示控制函数 
功能：读取LCD显示频率
输入: 无
输出: 00-FF
*********************************/
uint32_t LCD_DF_Read(void)
{
	return (LCD->DF & LCD_DF_DF_Msk);
}
/*********************************
LCD 显示控制函数 
功能：配置LCD显示点亮时间
输入: 00-FF  ton*0.25s
输出: 无
*********************************/
void LCD_TON_Write(uint32_t SetValue)
{
	LCD->TON = (SetValue & LCD_TON_TON_Msk);
}
/*********************************
LCD 显示控制函数 
功能：读取LCD显示点亮时间
输入: 无
输出: 00-FF ton*0.25s
*********************************/
uint32_t LCD_TON_Read(void)
{
	return (LCD->TON & LCD_TON_TON_Msk);
}

/*********************************
LCD 显示控制函数 
功能：配置LCD显示熄灭时间
输入: 00-FF  toff*0.25s
输出: 无
*********************************/
void LCD_TOFF_Write(uint32_t SetValue)
{
	LCD->TOFF = (SetValue & LCD_TOFF_TOFF_Msk);
}
/*********************************
LCD 显示控制函数 
功能：读取LCD显示熄灭时间
输入: 无
输出: 00-FF toff*0.25s
*********************************/
uint32_t LCD_TOFF_Read(void)
{
	return (LCD->TOFF & LCD_TOFF_TOFF_Msk);
}

/*********************************
LCD 显示控制函数 
功能：配置LCD点亮中断使能
输入: ENABLE/DISABLE	点亮中断使能/禁止
输出: 无
*********************************/
void LCD_DISPIE_DONIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->DISPIE |= (LCD_DISPIE_DONIE_Msk);
	}
	else
	{
		LCD->DISPIE &= ~(LCD_DISPIE_DONIE_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：读取LCD点亮中断使能
输入: 无
输出: ENABLE/DISABLE	点亮中断使能/禁止
*********************************/
FunState LCD_DISPIE_DONIE_Getable(void)
{
	if (LCD->DISPIE & (LCD_DISPIE_DONIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：配置LCD熄灭中断使能
输入: ENABLE/DISABLE	熄灭中断使能/禁止
输出: 无
*********************************/
void LCD_DISPIE_DOFFIE_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->DISPIE |= (LCD_DISPIE_DOFFIE_Msk);
	}
	else
	{
		LCD->DISPIE &= ~(LCD_DISPIE_DOFFIE_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：读取LCD熄灭中断使能
输入: 无
输出: ENABLE/DISABLE	熄灭中断使能/禁止
*********************************/
FunState LCD_DISPIE_DOFFIE_Getable(void)
{
	if (LCD->DISPIE & (LCD_DISPIE_DOFFIE_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：清LCD点亮中断标志
输入: 无
输出: 无
*********************************/
void LCD_DISPIF_DONIF_Clr(void)
{
	LCD->DISPIF = LCD_DISPIF_DONIF_Msk;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD点亮中断标志
输入: 无
输出: SET/RESET		LCD点亮中断已发生/未发生
*********************************/
FlagStatus LCD_DISPIF_DONIF_Chk(void)
{
	if (LCD->DISPIF & LCD_DISPIF_DONIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/*********************************
LCD 显示控制函数 
功能：清LCD熄灭中断标志
输入: 无
输出: 无
*********************************/
void LCD_DISPIF_DOFFIF_Clr(void)
{
	LCD->DISPIF = LCD_DISPIF_DOFFIF_Msk;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD熄灭中断标志
输入: 无
输出: SET/RESET		LCD熄灭中断已发生/未发生
*********************************/
FlagStatus LCD_DISPIF_DOFFIF_Chk(void)
{
	if (LCD->DISPIF & LCD_DISPIF_DOFFIF_Msk)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}
/*********************************
LCD 显示控制函数 
功能：设置LCD偏置
输入: LCD_LCDSET_BIASMD_4BIAS/LCD_LCDSET_BIASMD_3BIAS	1/4bias / 1/3bias
输出: 无
*********************************/
void LCD_LCDSET_BIASMD_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDSET;
	tmpreg &= ~(LCD_LCDSET_BIASMD_Msk);
	tmpreg |= (SetValue & LCD_LCDSET_BIASMD_Msk);
	LCD->LCDSET = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD偏置
输入: 无
输出: LCD_LCDSET_BIASMD_4BIAS/LCD_LCDSET_BIASMD_3BIAS	1/4bias / 1/3bias
*********************************/
uint32_t LCD_LCDSET_BIASMD_Get(void)
{
	return (LCD->LCDSET & LCD_LCDSET_BIASMD_Msk);
}

/*********************************
LCD 显示控制函数 
功能：设置LCD驱动类型
输入: LCD_LCDSET_WFT_ATYPE/LCD_LCDSET_WFT_BTYPE		A类波形 / B类波形
输出: 无
*********************************/
void LCD_LCDSET_WFT_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDSET;
	tmpreg &= ~(LCD_LCDSET_WFT_Msk);
	tmpreg |= (SetValue & LCD_LCDSET_WFT_Msk);
	LCD->LCDSET = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD驱动类型
输入: 无
输出: LCD_LCDSET_WFT_ATYPE/LCD_LCDSET_WFT_BTYPE		A类波形 / B类波形
*********************************/
uint32_t LCD_LCDSET_WFT_Get(void)
{
	return (LCD->LCDSET & LCD_LCDSET_WFT_Msk);
}


/*********************************
LCD 显示控制函数 
功能：设置LCD duty值
输入: LCD_LCDSET_LMUX_4COM/LCD_LCDSET_LMUX_6COM/LCD_LCDSET_LMUX_8COM	4com/6com/8com
输出: 无
*********************************/
void LCD_LCDSET_LMUX_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDSET;
	tmpreg &= ~(LCD_LCDSET_LMUX_Msk);
	tmpreg |= (SetValue & LCD_LCDSET_LMUX_Msk);
	LCD->LCDSET = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD duty值
输入: 无
输出: LCD_LCDSET_LMUX_4COM/LCD_LCDSET_LMUX_6COM/LCD_LCDSET_LMUX_8COM	4com/6com/8com
*********************************/
uint32_t LCD_LCDSET_LMUX_Get(void)
{
	return (LCD->LCDSET & LCD_LCDSET_LMUX_Msk);
}

/*********************************
LCD 显示控制函数 
功能：设置LCD SC频率值
输入: 	LCD_LCDDRV_SCFSEL_X1		频率为disp_clk/1
	LCD_LCDDRV_SCFSEL_X8		频率为disp_clk/8
	LCD_LCDDRV_SCFSEL_X16		频率为disp_clk/16
	LCD_LCDDRV_SCFSEL_X32		频率为disp_clk/32
	LCD_LCDDRV_SCFSEL_X64		频率为disp_clk/64
	LCD_LCDDRV_SCFSEL_X128		频率为disp_clk/128
	LCD_LCDDRV_SCFSEL_X256		频率为disp_clk/256
	LCD_LCDDRV_SCFSEL_X512		频率为disp_clk/512
输出: 无
*********************************/
void LCD_LCDDRV_SCFSEL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDDRV;
	tmpreg &= ~(LCD_LCDDRV_SCFSEL_Msk);
	tmpreg |= (SetValue & LCD_LCDDRV_SCFSEL_Msk);
	LCD->LCDDRV = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD SC频率值
输入: 无
输出: 	LCD_LCDDRV_SCFSEL_X1		频率为disp_clk/1
	LCD_LCDDRV_SCFSEL_X8		频率为disp_clk/8
	LCD_LCDDRV_SCFSEL_X16		频率为disp_clk/16
	LCD_LCDDRV_SCFSEL_X32		频率为disp_clk/32
	LCD_LCDDRV_SCFSEL_X64		频率为disp_clk/64
	LCD_LCDDRV_SCFSEL_X128		频率为disp_clk/128
	LCD_LCDDRV_SCFSEL_X256		频率为disp_clk/256
	LCD_LCDDRV_SCFSEL_X512		频率为disp_clk/512
*********************************/
uint32_t LCD_LCDDRV_SCFSEL_Get(void)
{
	return (LCD->LCDDRV & LCD_LCDDRV_SCFSEL_Msk);
}


/*********************************
LCD 显示控制函数 
功能：设置LCD 驱动方式控制(片外电容驱动模式下)
输入: 	LCD_LCDDRV_SC_CTRL_ONE		单次驱动
	LCD_LCDDRV_SC_CTRL_TWO		连续驱动两次
	LCD_LCDDRV_SC_CTRL_FOUR		连续驱动四次(当SC>4HKz时，此选项也为连续驱动)
	LCD_LCDDRV_SC_CTRL_CONTINUE	连续驱动
输出: 无
*********************************/
void LCD_LCDDRV_SC_CTRL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDDRV;
	tmpreg &= ~(LCD_LCDDRV_SC_CTRL_Msk);
	tmpreg |= (SetValue & LCD_LCDDRV_SC_CTRL_Msk);
	LCD->LCDDRV = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD 驱动方式控制(片外电容驱动模式下)
输入: 无
输出: 	LCD_LCDDRV_SC_CTRL_ONE		单次驱动
	LCD_LCDDRV_SC_CTRL_TWO		连续驱动两次
	LCD_LCDDRV_SC_CTRL_FOUR		连续驱动四次(当SC>4HKz时，此选项也为连续驱动)
	LCD_LCDDRV_SC_CTRL_CONTINUE	连续驱动
*********************************/
uint32_t LCD_LCDDRV_SC_CTRL_Get(void)
{
	return (LCD->LCDDRV & LCD_LCDDRV_SC_CTRL_Msk);
}

/*********************************
LCD 显示控制函数 
功能：设置LCD 驱动电流
输入: 	LCD_LCDDRV_IC_CTRL_L3		驱动电流最大
	LCD_LCDDRV_IC_CTRL_L2		驱动电流次大
	LCD_LCDDRV_IC_CTRL_L1		驱动电流次小
	LCD_LCDDRV_IC_CTRL_L0		驱动电流最小
输出: 无
*********************************/
void LCD_LCDDRV_IC_CTRL_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDDRV;
	tmpreg &= ~(LCD_LCDDRV_IC_CTRL_Msk);
	tmpreg |= (SetValue & LCD_LCDDRV_IC_CTRL_Msk);
	LCD->LCDDRV = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD 驱动电流
输入: 无
输出: 	LCD_LCDDRV_IC_CTRL_L3		驱动电流最大
	LCD_LCDDRV_IC_CTRL_L2		驱动电流次大
	LCD_LCDDRV_IC_CTRL_L1		驱动电流次小
	LCD_LCDDRV_IC_CTRL_L0		驱动电流最小
*********************************/
uint32_t LCD_LCDDRV_IC_CTRL_Get(void)
{
	return (LCD->LCDDRV & LCD_LCDDRV_IC_CTRL_Msk);
}

/*********************************
LCD 显示控制函数 
功能：设置LCD 驱动模式
输入: 	LCD_LCDDRV_ENMODE_EXTERNALCAP		片外电容驱动
	LCD_LCDDRV_ENMODE_INNERRESISTER		片内电阻驱动
输出: 无
*********************************/
void LCD_LCDDRV_ENMODE_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDDRV;
	tmpreg &= ~(LCD_LCDDRV_ENMODE_Msk);
	tmpreg |= (SetValue & LCD_LCDDRV_ENMODE_Msk);
	LCD->LCDDRV = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD 驱动模式
输入: 无
输出: 	LCD_LCDDRV_ENMODE_EXTERNALCAP		片外电容驱动
	LCD_LCDDRV_ENMODE_INNERRESISTER		片内电阻驱动
*********************************/
uint32_t LCD_LCDDRV_ENMODE_Get(void)
{
	return (LCD->LCDDRV & LCD_LCDDRV_ENMODE_Msk);
}

/*********************************
LCD 显示控制函数 
功能：设置LCD 偏置电平选择(灰度)
输入: 	00-0F
输出: 无
*********************************/
void LCD_LCDBIAS_Write(uint32_t SetValue)
{
	LCD->LCDBIAS = (SetValue & LCD_LCDBIAS_LCDBIAS_Msk);
}
/*********************************
LCD 显示控制函数 
功能：获取LCD 偏置电平选择(灰度)
输入: 无
输出: 	00-0F
*********************************/
uint32_t LCD_LCDBIAS_Read(void)
{
	return (LCD->LCDBIAS & LCD_LCDBIAS_LCDBIAS_Msk);
}

/*********************************
LCD 显示控制函数 
功能：设置LCD BOOST环振频率配置信号
输入: 	LCD_LCDBSTCON_OSC_CFG_GRADE0		0级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE1		1级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE2		2级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE3		3级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE4		4级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE5		5级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE6		6级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE7		7级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE8		8级 值越大频率越高，驱动越强，功耗越高
	
输出: 无
*********************************/
void LCD_LCDBSTCON_OSC_CFG_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDBSTCON;
	tmpreg &= ~(LCD_LCDBSTCON_OSC_CFG_Msk);
	tmpreg |= (SetValue & LCD_LCDBSTCON_OSC_CFG_Msk);
	LCD->LCDBSTCON = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD BOOST环振频率配置信号
输入: 无
输出: 	LCD_LCDBSTCON_OSC_CFG_GRADE0		0级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE1		1级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE2		2级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE3		3级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE4		4级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE5		5级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE6		6级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE7		7级 值越大频率越高，驱动越强，功耗越高
	LCD_LCDBSTCON_OSC_CFG_GRADE8		8级 值越大频率越高，驱动越强，功耗越高
*********************************/
uint32_t LCD_LCDBSTCON_OSC_CFG_Get(void)
{
	return (LCD->LCDBSTCON & LCD_LCDBSTCON_OSC_CFG_Msk);
}


/*********************************
LCD 显示控制函数 
功能：配置LCD 基准源Buffer使能控制
输入: ENABLE/DISABLE	使能基准源/禁止
输出: 无
*********************************/
void LCD_LCDBSTCON_BUFENB_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->LCDBSTCON &= ~(LCD_LCDBSTCON_BUFENB_Msk);
	}
	else
	{
		LCD->LCDBSTCON |= (LCD_LCDBSTCON_BUFENB_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：获取LCD 基准源Buffer使能控制
输入: 无
输出: ENABLE/DISABLE	使能基准源/禁止
*********************************/
FunState LCD_LCDBSTCON_BUFENB_Getable(void)
{
	if (LCD->LCDBSTCON & (LCD_LCDBSTCON_BUFENB_Msk))
	{
		return DISABLE;
	}
	else
	{
		return ENABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：配置LCD 基准源Buffer旁路控制
输入: ENABLE/DISABLE	使能基准源旁路/禁止
输出: 无
*********************************/
void LCD_LCDBSTCON_BUFBYP_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->LCDBSTCON |= (LCD_LCDBSTCON_BUFBYP_Msk);
	}
	else
	{
		LCD->LCDBSTCON &= ~(LCD_LCDBSTCON_BUFBYP_Msk);
	}
}

/*********************************
LCD 显示控制函数 
功能：获取LCD 基准源Buffer旁路控制
输入: 无
输出: ENABLE/DISABLE	使能基准源旁路/禁止
*********************************/
FunState LCD_LCDBSTCON_BUFBYP_Getable(void)
{
	if (LCD->LCDBSTCON & (LCD_LCDBSTCON_BUFBYP_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：配置LCD VLCD下拉
输入: ENABLE/DISABLE	使能VLCD下拉/禁止
输出: 无
*********************************/
void LCD_LCDBSTCON_VLCDPD_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->LCDBSTCON |= (LCD_LCDBSTCON_VLCDPD_Msk);
	}
	else
	{
		LCD->LCDBSTCON &= ~(LCD_LCDBSTCON_VLCDPD_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：获取LCD VLCD下拉状态
输入: 无
输出: ENABLE/DISABLE	使能VLCD下拉/禁止
*********************************/
FunState LCD_LCDBSTCON_VLCDPD_Getable(void)
{
	if (LCD->LCDBSTCON & (LCD_LCDBSTCON_VLCDPD_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：设置LCD VLCD输出电压
输入: 	LCD_LCDBSTCON_VLCDCFG_2P5V		VLCD = 2.5V
	LCD_LCDBSTCON_VLCDCFG_3P0V		VLCD = 3.0V
	LCD_LCDBSTCON_VLCDCFG_3P5V		VLCD = 3.5V
	LCD_LCDBSTCON_VLCDCFG_4P0V		VLCD = 4.0V
	LCD_LCDBSTCON_VLCDCFG_4P5V		VLCD = 4.5V
	LCD_LCDBSTCON_VLCDCFG_5P0V		VLCD = 5.0V
	LCD_LCDBSTCON_VLCDCFG_5P5V		VLCD = 5.5V
	
输出: 无
*********************************/
void LCD_LCDBSTCON_VLCDCFG_Set(uint32_t SetValue)
{
	uint32_t tmpreg;
	tmpreg = LCD->LCDBSTCON;
	tmpreg &= ~(LCD_LCDBSTCON_VLCDCFG_Msk);
	tmpreg |= (SetValue & LCD_LCDBSTCON_VLCDCFG_Msk);
	LCD->LCDBSTCON = tmpreg;
}
/*********************************
LCD 显示控制函数 
功能：获取LCD VLCD输出电压
输入: 无
输出: 	LCD_LCDBSTCON_VLCDCFG_2P5V		VLCD = 2.5V
	LCD_LCDBSTCON_VLCDCFG_3P0V		VLCD = 3.0V
	LCD_LCDBSTCON_VLCDCFG_3P5V		VLCD = 3.5V
	LCD_LCDBSTCON_VLCDCFG_4P0V		VLCD = 4.0V
	LCD_LCDBSTCON_VLCDCFG_4P5V		VLCD = 4.5V
	LCD_LCDBSTCON_VLCDCFG_5P0V		VLCD = 5.0V
	LCD_LCDBSTCON_VLCDCFG_5P5V		VLCD = 5.5V	
*********************************/
uint32_t LCD_LCDBSTCON_VLCDCFG_Get(void)
{
	return (LCD->LCDBSTCON & LCD_LCDBSTCON_VLCDCFG_Msk);
}

/*********************************
LCD 显示控制函数 
功能：配置LCD Booster测试信号
输入: ENABLE/DISABLE	Booster测试信号使能/禁止
输出: 无
*********************************/
void LCD_LCDBSTCON_BOOST_TEN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->LCDBSTCON |= (LCD_LCDBSTCON_BOOST_TEN_Msk);
	}
	else
	{
		LCD->LCDBSTCON &= ~(LCD_LCDBSTCON_BOOST_TEN_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：获取LCD Booster测试信号状态
输入: 无
输出:  ENABLE/DISABLE	Booster测试信号使能/禁止
*********************************/
FunState LCD_LCDBSTCON_BOOST_TEN_Getable(void)
{
	if (LCD->LCDBSTCON & (LCD_LCDBSTCON_BOOST_TEN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：配置LCD Booster使能控制
输入: ENABLE/DISABLE	Booster使能/禁止
输出: 无
*********************************/
void LCD_LCDBSTCON_BOOST_EN_Setable(FunState NewState)
{
	if (NewState == ENABLE)
	{
		LCD->LCDBSTCON |= (LCD_LCDBSTCON_BOOST_EN_Msk);
	}
	else
	{
		LCD->LCDBSTCON &= ~(LCD_LCDBSTCON_BOOST_EN_Msk);
	}
}
/*********************************
LCD 显示控制函数 
功能：获取LCD Booster使能状态
输入: 无
输出:  ENABLE/DISABLE	Booster使能/禁止
*********************************/
FunState LCD_LCDBSTCON_BOOST_EN_Getable(void)
{
	if (LCD->LCDBSTCON & (LCD_LCDBSTCON_BOOST_EN_Msk))
	{
		return ENABLE;
	}
	else
	{
		return DISABLE;
	}
}

/*********************************
LCD 显示控制函数 
功能：配置LCD COM0~3输出使能控制
输入: 00-0F COM0-COM3
输出: 无
*********************************/
void LCD_COM_EN_Write(uint32_t SetValue)
{
	LCD->COM_EN = (SetValue & LCD_COM_EN_COMEN_Msk);
}
/*********************************
LCD 显示控制函数 
功能：获取LCD COM0~3输出使能状态
输入: 无
输出: 00-0F COM0-COM3
*********************************/
uint32_t LCD_COM_EN_Read(void)
{
	return (LCD->COM_EN & LCD_COM_EN_COMEN_Msk);
}


/*********************************
LCD 显示控制函数 
功能：配置LCD SEG0-31输出使能控制
输入: 00000000-ffffffff	SEG0-SEG31
输出: 无
*********************************/
void LCD_SEG_EN0_Write(uint32_t SetValue)
{
	LCD->SEG_EN0 = (SetValue);
}
/*********************************
LCD 显示控制函数 
功能：获取LCD SEG0-31输出使能状态
输入: 无
输出: 00000000-ffffffff	SEG0-SEG31
*********************************/
uint32_t LCD_SEG_EN0_Read(void)
{
	return (LCD->SEG_EN0);
}

/*********************************
LCD 显示控制函数 
功能：配置LCD SEG32~43(COM4-7)输出使能控制
输入: 00000000-00000fff	SEG32~43(COM4-7)
输出: 无
*********************************/
void LCD_SEG_EN1_Write(uint32_t SetValue)
{
	LCD->SEG_EN1 = (SetValue & LCD_SEG_EN1_SEGENx_Msk);
}
/*********************************
LCD 显示控制函数 
功能：获取LCD SEG32~43(COM4-7)输出使能状态
输入: 无
输出: 00000000-00000fff	SEG32~43(COM4-7)
*********************************/
uint32_t LCD_SEG_EN1_Read(void)
{
	return (LCD->SEG_EN1 & LCD_SEG_EN1_SEGENx_Msk);
}
/*********************************
LCD 显示控制函数 
功能：关闭所有LCD(缺省)
输入: 无
输出: 无
*********************************/
void LCD_Deinit(void)
{
	LCD->DISPCTRL = 0x00000000;
	LCD->LCDTEST = 0x00000000;
	LCD->DF = 0x00000000;
	LCD->TON = 0x00000000;
	LCD->TOFF = 0x00000000;
	LCD->DISPIE = 0x00000000;
	LCD->DISPIF = 0x00000000;
	LCD->LCDSET = 0x00000000;
	LCD->LCDDRV = 0x00000002;
	LCD->LCDBIAS = 0x0000000E;
	LCD->LCDBSTCON = 0x00000000;
	LCD->COM_EN = 0x00000000;
	LCD->SEG_EN0 = 0x00000000;
	LCD->SEG_EN1 = 0x00000000;
}
//Code_End

/*DISPDATAx显示数据寄存器刷新*/
void LCD_DISPDATAx_Refresh(uint32_t* DispBuf)
{
    LCD->DISPDATA0 = DispBuf[0];
    LCD->DISPDATA1 = DispBuf[1];
    LCD->DISPDATA2 = DispBuf[2];
    LCD->DISPDATA3 = DispBuf[3];
    LCD->DISPDATA4 = DispBuf[4];
    LCD->DISPDATA5 = DispBuf[5];
    LCD->DISPDATA6 = DispBuf[6];
    LCD->DISPDATA7 = DispBuf[7];
    LCD->DISPDATA8 = DispBuf[8];
    LCD->DISPDATA9 = DispBuf[9];
}

/*lcd boost功能配置*/
void LCD_Boost_Init(LCD_Boost_InitTypeDef* para)
{
	LCD_LCDBSTCON_OSC_CFG_Set(para->OSC_CFG);
	LCD_LCDBSTCON_VLCDCFG_Set(para->VLCDCFG);
	LCD_LCDBSTCON_BUFENB_Setable(para->BUFENB);
	LCD_LCDBSTCON_BUFBYP_Setable(para->BUFBYP);
	LCD_LCDBSTCON_VLCDPD_Setable(para->VLCDPD);	
	LCD_LCDBSTCON_BOOST_TEN_Setable(para->BOOST_TEN);
	LCD_LCDBSTCON_BOOST_EN_Setable(para->BOOST_EN);
}

/*LCD显示功能配置*/
void LCD_Init(LCD_InitTypeDef* para)
{
	LCD_LCDSET_LMUX_Set(para->LMUX);
	LCD_LCDDRV_ENMODE_Set(para->ENMODE);
	LCD_LCDSET_WFT_Set(para->WFT);
	LCD_DF_Write(para->DF);
	LCD_LCDSET_BIASMD_Set(para->BIASMD);
	LCD_LCDDRV_SCFSEL_Set(para->SCFSEL);
	LCD_LCDDRV_SC_CTRL_Set(para->SC_CTRL);
	LCD_LCDDRV_IC_CTRL_Set(para->IC_CTRL);	
	LCD_LCDBIAS_Write(para->LCDBIAS);
	LCD_DISPCTRL_ANTIPOLAR_Setable(para->ANTIPOLAR);
	
	LCD_DISPCTRL_TEST_Setable(para->TEST);
	LCD_DISPCTRL_DISPMD_Setable(para->DISPMD);

	LCD_LCDTEST_LCCTRL_Set(para->LCCTRL);
	LCD_LCDTEST_TESTEN_Setable(para->TESTEN);
	
	LCD_DISPCTRL_FLICK_Setable(para->FLICK);	
	LCD_TON_Write(para->TON);
	LCD_TOFF_Write(para->TOFF);
	LCD_DISPIE_DONIE_Setable(para->DONIE);
	LCD_DISPIE_DOFFIE_Setable(para->DOFFIE);
	LCD_DISPIF_DONIF_Clr();
	LCD_DISPIF_DOFFIF_Clr();
	
	LCD_DISPCTRL_LCDEN_Setable(para->LCDEN);	
}


