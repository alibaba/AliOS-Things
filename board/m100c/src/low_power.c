
/* Includes ------------------------------------------------------------------*/
#include "low_power.h"
#include "lorawan_port.h"
#include "debug.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 ** \brief Flag to indicate if MCU can go to low power mode
 **        When 0, MCU is authorized to go in low power mode
 **/

/* Private function prototypes -----------------------------------------------*/


/* Exported functions
 ** ---------------------------------------------------------*/

/**
 ** \brief API to set flag allowing power mode
 **
 ** \param [IN] enum e_LOW_POWER_State_Id_t
 **/
void LowPower_Disable(e_LOW_POWER_State_Id_t state)
{
}

/**
 ** \brief API to reset flag allowing power mode
 **
 ** \param [IN] enum e_LOW_POWER_State_Id_t
 **/
void LowPower_Enable(e_LOW_POWER_State_Id_t state)
{
}

/**
 ** \brief API to get flag allowing power mode
 ** \note When flag is 0, low power mode is allowed
 ** \param [IN] state
 ** \retval flag state
 **/
uint32_t LowPower_GetState(void)
{
        return 0;
}

/**
 ** @brief  Handle Low Power
 ** @param  None
 ** @retval None
 **/

void BoardLowPowerHandler(void)
{
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

