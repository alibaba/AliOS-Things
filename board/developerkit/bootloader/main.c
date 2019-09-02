#include "main.h"
#include "stm32l4xx_hal.h"
#include "hal_boot_uart.h"
#include "hal_boot_wdg.h"
#include "hal_boot_process.h"

#include "2ndboot.h"

void HAL_PWR_EnableBkUpAccess(void)
{
    SET_BIT(PWR->CR1, PWR_CR1_DBP);
}

/**
  * @brief Disable access to the backup domain
  *        (RTC registers, RTC backup data registers).
  * @retval None
  */
void HAL_PWR_DisableBkUpAccess(void)
{
    CLEAR_BIT(PWR->CR1, PWR_CR1_DBP);
}


uint32_t HAL_PWREx_GetVoltageRange(void)
{
    return  (PWR->CR1 & PWR_CR1_VOS);
}

#define PWR_FLAG_SETTING_DELAY_US                      50
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling)
{
    uint32_t wait_loop_index = 0;

    assert_param(IS_PWR_VOLTAGE_SCALING_RANGE(VoltageScaling));

    /* If Set Range 1 */
    if (VoltageScaling == PWR_REGULATOR_VOLTAGE_SCALE1) {
        if (READ_BIT(PWR->CR1, PWR_CR1_VOS) != PWR_REGULATOR_VOLTAGE_SCALE1) {
            /* Set Range 1 */
            MODIFY_REG(PWR->CR1, PWR_CR1_VOS, PWR_REGULATOR_VOLTAGE_SCALE1);
            /* Wait until VOSF is cleared */
            wait_loop_index = (PWR_FLAG_SETTING_DELAY_US * (SystemCoreClock / 1000000));
            while ((wait_loop_index != 0) && (HAL_IS_BIT_SET(PWR->SR2, PWR_SR2_VOSF))) {
                wait_loop_index--;
            }
            if (HAL_IS_BIT_SET(PWR->SR2, PWR_SR2_VOSF)) {
                return HAL_TIMEOUT;
            }
        }
    }
    else {
        if (READ_BIT(PWR->CR1, PWR_CR1_VOS) != PWR_REGULATOR_VOLTAGE_SCALE2) {
            /* Set Range 2 */
            MODIFY_REG(PWR->CR1, PWR_CR1_VOS, PWR_REGULATOR_VOLTAGE_SCALE2);
            /* No need to wait for VOSF to be cleared for this transition */
        }
    }
    return HAL_OK;
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
    PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

static uint32_t delay_i, delay_j, delay_k;
static void delay()
{
    for(delay_i = 0; delay_i < 100; delay_i++) {
        for(delay_j = 0; delay_j < 100; delay_j++)
            for(delay_k = 0; delay_k < 100; delay_k++){}
    }
}

int main(void)
{
    int ret = 0, i = 0;
    unsigned int bank = 0;
    ota_boot_param_t ota_param = {0};
    ret = ota_patch_read_param(&ota_param);
    if(ret < 0) {
        return -1;
    }
    HAL_Init();
    SystemClock_Config();
    hal_uart_Init();
    bank = hal_get_boot_bank();
    printf("ota:0x%x bank:%d \n", ota_param.upg_flag, bank);
#if defined OTA_CONFIG_DIFF
    if(ota_param.upg_flag == OTA_UPGRADE_DIFF) {
        ret = ota_nbpatch_main();
    }
#else
    if(ota_param.upg_flag == OTA_UPGRADE_XZ){
        ret = ota_xz_main();
    }
#endif
    if(ret != 0 && ota_param.boot_count <= 3) {
        printf("fail.\n");
        ota_patch_write_param(&ota_param);
        sys_delayms(100);
        sys_reboot();
    } else if (ota_param.upg_flag != 0) {
        ota_param.upg_flag = 0;
        ota_param.boot_count = 0;
        ota_patch_write_param(&ota_param);
    }
    load_app(APP_LOAD_ADDR);
    return 0;
}

void _Error_Handler(char *file, int line)
{
    while(1)
    {

    }
}
