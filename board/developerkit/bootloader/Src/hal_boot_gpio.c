#include "stm32l4xx_hal.h"

#define GPIO_MODE             ((uint32_t)0x00000003)
#define ANALOG_MODE           ((uint32_t)0x00000008)
#define EXTI_MODE             ((uint32_t)0x10000000)
#define GPIO_MODE_IT          ((uint32_t)0x00010000)
#define GPIO_MODE_EVT         ((uint32_t)0x00020000)
#define RISING_EDGE           ((uint32_t)0x00100000)
#define FALLING_EDGE          ((uint32_t)0x00200000)
#define GPIO_OUTPUT_TYPE      ((uint32_t)0x00000010)

void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    uint32_t position = 0x00;
    uint32_t iocurrent = 0x00;
    uint32_t temp = 0x00;

    /* Check the parameters */
    assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
    assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
    assert_param(IS_GPIO_MODE(GPIO_Init->Mode));
    assert_param(IS_GPIO_PULL(GPIO_Init->Pull));

    /* Configure the port pins */
    while (((GPIO_Init->Pin) >> position) != RESET) {
        /* Get current io position */
        iocurrent = (GPIO_Init->Pin) & (1U << position);
        if(iocurrent) {
            /*--------------------- GPIO Mode Configuration ------------------------*/
            /* In case of Alternate function mode selection */
            if((GPIO_Init->Mode == GPIO_MODE_AF_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_OD)) {
                /* Check the Alternate function parameters */
                assert_param(IS_GPIO_AF_INSTANCE(GPIOx));
                assert_param(IS_GPIO_AF(GPIO_Init->Alternate));

                /* Configure Alternate function mapped with the current IO */
                temp = GPIOx->AFR[position >> 3];
                temp &= ~((uint32_t)0xF << ((uint32_t)(position & (uint32_t)0x07) * 4)) ;
                temp |= ((uint32_t)(GPIO_Init->Alternate) << (((uint32_t)position & (uint32_t)0x07) * 4));
                GPIOx->AFR[position >> 3] = temp;
            }

            /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
            temp = GPIOx->MODER;
            temp &= ~(GPIO_MODER_MODE0 << (position * 2));
            temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2));
            GPIOx->MODER = temp;

            /* In case of Output or Alternate function mode selection */
            if((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
            (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD)) {
                /* Check the Speed parameter */
                assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));
                /* Configure the IO Speed */
                temp = GPIOx->OSPEEDR;
                temp &= ~(GPIO_OSPEEDR_OSPEED0 << (position * 2));
                temp |= (GPIO_Init->Speed << (position * 2));
                GPIOx->OSPEEDR = temp;

                /* Configure the IO Output Type */
                temp = GPIOx->OTYPER;
                temp &= ~(GPIO_OTYPER_OT0 << position) ;
                temp |= (((GPIO_Init->Mode & GPIO_OUTPUT_TYPE) >> 4) << position);
                GPIOx->OTYPER = temp;
            }
            /* Activate the Pull-up or Pull down resistor for the current IO */
            temp = GPIOx->PUPDR;
            temp &= ~(GPIO_PUPDR_PUPD0 << (position * 2));
            temp |= ((GPIO_Init->Pull) << (position * 2));
            GPIOx->PUPDR = temp;

            /*--------------------- EXTI Mode Configuration ------------------------*/
            /* Configure the External Interrupt or event for the current IO */
            if((GPIO_Init->Mode & EXTI_MODE) == EXTI_MODE) {
                /* Enable SYSCFG Clock */
                __HAL_RCC_SYSCFG_CLK_ENABLE();

                temp = SYSCFG->EXTICR[position >> 2];
                temp &= ~(((uint32_t)0x0F) << (4 * (position & 0x03)));
                temp |= (GPIO_GET_INDEX(GPIOx) << (4 * (position & 0x03)));
                SYSCFG->EXTICR[position >> 2] = temp;

                /* Clear EXTI line configuration */
                temp = EXTI->IMR1;
                temp &= ~((uint32_t)iocurrent);
                if((GPIO_Init->Mode & GPIO_MODE_IT) == GPIO_MODE_IT) {
                    temp |= iocurrent;
                }
                EXTI->IMR1 = temp;

                temp = EXTI->EMR1;
                temp &= ~((uint32_t)iocurrent);
                if((GPIO_Init->Mode & GPIO_MODE_EVT) == GPIO_MODE_EVT) {
                    temp |= iocurrent;
                }
                EXTI->EMR1 = temp;

                /* Clear Rising Falling edge configuration */
                temp = EXTI->RTSR1;
                temp &= ~((uint32_t)iocurrent);
                if((GPIO_Init->Mode & RISING_EDGE) == RISING_EDGE) {
                    temp |= iocurrent;
                }
                EXTI->RTSR1 = temp;

                temp = EXTI->FTSR1;
                temp &= ~((uint32_t)iocurrent);
                if((GPIO_Init->Mode & FALLING_EDGE) == FALLING_EDGE) {
                    temp |= iocurrent;
                }
                EXTI->FTSR1 = temp;
            }
        }
        position++;
    }
}

