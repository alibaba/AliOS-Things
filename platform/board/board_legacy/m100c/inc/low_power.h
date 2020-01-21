
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LOW_POWER_H__
#define __LOW_POWER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
/* Exported types ------------------------------------------------------------*/

typedef enum {
    e_LOW_POWER_RTC = (1 << 0),
    e_LOW_POWER_GPS = (1 << 1),
    e_LOW_POWER_UART = (1 << 2), /*  can be used to forbid stop mode in case of uart Xfer*/
} e_LOW_POWER_State_Id_t;


/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/*!
 * @brief API to set flag allowing power mode
 *
 * @param [IN] enum e_LOW_POWER_State_Id_t
 */
void LowPower_Disable( e_LOW_POWER_State_Id_t state );

/*!
 * @brief API to reset flag allowing power mode
 *
 * @param [IN] enum e_LOW_POWER_State_Id_t
 */

void LowPower_Enable( e_LOW_POWER_State_Id_t state );

/* !
 * @brief API to get flag allowing power mode
 * @note When flag is 0, low power mode is allowed
 * @param [IN] non
 * @retval flag state
 */
uint32_t LowPower_GetState( void );

/* !
 * @brief Manages the entry into ARM cortex deep-sleep mode
 * @param none
 * @retval none
 */
void LowPower_Handler( void );

#ifdef __cplusplus
}
#endif

#endif /*  __LOW_POWER_H__ */

/* *********************** (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

