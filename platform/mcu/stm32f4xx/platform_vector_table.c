/** @file
 * STM32F4xx vector table
 */
#include <stdint.h>
#include "platform_assert.h"
//#include "platform_constants.h"
#include "platform_isr.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Static Function Declarations
 ******************************************************/

extern void UnhandledInterrupt( void );
extern void Reset_Handler     ( void );

extern void vPortSVCHandler     ( void );
extern void xPortPendSVHandler     ( void );
extern void xPortSysTickHandler     ( void );

#ifdef NO_MICO_RTOS
extern void SVC_Handler             ( void );
extern void PendSV_Handler          ( void );
extern void SysTick_Handler         ( void );
#else
#define SVC_Handler     vPortSVCHandler
#define PendSV_Handler  xPortPendSVHandler
#define SysTick_Handler xPortSysTickHandler
#endif

extern void NMI_Handler           ( void );  // Non Maskable Interrupt
extern void HardFault_Handler     ( void );  // Hard Fault interrupt
extern void MemManage_Handler     ( void );  // Memory Management Fault interrupt
extern void BusFault_Handler      ( void );  // Bus Fault interrupt
extern void UsageFault_Handler    ( void );  // Usage Fault interrupt
extern void DebugMon_Handler           ( void );  // Debug Monitor interrupt
extern void WWDG_IRQHandler               ( void );  // Window WatchDog
extern void PVD_IRQHandler                ( void );  // PVD through EXTI Line detection
extern void TAMP_STAMP_IRQHandler         ( void );  // Tamper and TimeStamps through the EXTI line
extern void RTC_WKUP_irq           ( void );  // RTC Wakeup through the EXTI line
extern void FLASH_IRQHandler              ( void );  // FLASH
extern void RCC_IRQHandler                ( void );  // RCC
extern void EXTI0_IRQHandler              ( void );  // EXTI Line0
extern void EXTI1_IRQHandler              ( void );  // EXTI Line1
extern void EXTI2_IRQHandler              ( void );  // EXTI Line2
extern void EXTI3_IRQHandler              ( void );  // EXTI Line3
extern void EXTI4_IRQHandler              ( void );  // EXTI Line4
extern void DMA1_Stream0_IRQHandler       ( void );  // DMA1 Stream 0
extern void DMA1_Stream1_IRQHandler       ( void );  // DMA1 Stream 1
extern void DMA1_Stream2_IRQHandler       ( void );  // DMA1 Stream 2
extern void DMA1_Stream3_IRQHandler       ( void );  // DMA1 Stream 3
extern void DMA1_Stream4_IRQHandler       ( void );  // DMA1 Stream 4
extern void DMA1_Stream5_IRQHandler       ( void );  // DMA1 Stream 5
extern void DMA1_Stream6_IRQHandler       ( void );  // DMA1 Stream 6
extern void ADC_IRQHandler                ( void );  // ADC1, ADC2 and ADC3s
extern void CAN1_TX_IRQHandler            ( void );  // CAN1 TX
extern void CAN1_RX0_IRQHandler           ( void );  // CAN1 RX0
extern void CAN1_RX1_IRQHandler           ( void );  // CAN1 RX1
extern void CAN1_SCE_IRQHandler           ( void );  // CAN1 SCE
extern void EXTI9_5_IRQHandler            ( void );  // External Line[9:5]s
extern void TIM1_BRK_TIM9_IRQHandler      ( void );  // TIM1 Break and TIM9
extern void TIM1_UP_TIM10_IRQHandler      ( void );  // TIM1 Update and TIM10
extern void TIM1_TRG_COM_TIM11_IRQHandler ( void );  // TIM1 Trigger and Commutation and TIM11
extern void TIM1_CC_IRQHandler            ( void );  // TIM1 Capture Compare
extern void TIM2_IRQHandler               ( void );  // TIM2
extern void TIM3_IRQHandler               ( void );  // TIM3
extern void TIM4_IRQHandler               ( void );  // TIM4
extern void I2C1_EV_IRQHandler            ( void );  // I2C1 Event
extern void I2C1_ER_IRQHandler            ( void );  // I2C1 Error
extern void I2C2_EV_IRQHandler            ( void );  // I2C2 Event
extern void I2C2_ER_IRQHandler            ( void );  // I2C2 Error
extern void SPI1_IRQHandler               ( void );  // SPI1
extern void SPI2_IRQHandler               ( void );  // SPI2
extern void USART1_IRQHandler             ( void );  // USART1
extern void USART2_IRQHandler             ( void );  // USART2
extern void USART3_IRQHandler             ( void );  // USART3
extern void EXTI15_10_IRQHandler          ( void );  // External Line[15:10]s
extern void RTC_Alarm_IRQHandler          ( void );  // RTC Alarm (A and B) through EXTI Line
extern void OTG_FS_WKUP_IRQHandler        ( void );  // USB OTG FS Wakeup through EXTI line
extern void TIM8_BRK_TIM12_IRQHandler     ( void );  // TIM8 Break and TIM12
extern void TIM8_UP_TIM13_IRQHandler      ( void );  // TIM8 Update and TIM13
extern void TIM8_TRG_COM_TIM14_IRQHandler ( void );  // TIM8 Trigger and Commutation and TIM14
extern void TIM8_CC_IRQHandler            ( void );  // TIM8 Capture Compare
extern void DMA1_Stream7_IRQHandler       ( void );  // DMA1 Stream7
extern void FMC_IRQHandler                ( void );  // FMC
extern void FSMC_IRQHandler               ( void );  // FSMC
extern void SDIO_IRQHandler               ( void );  // SDIO
extern void TIM5_IRQHandler               ( void );  // TIM5
extern void SPI3_IRQHandler               ( void );  // SPI3
extern void UART4_IRQHandler              ( void );  // UART4
extern void UART5_IRQHandler              ( void );  // UART5
extern void TIM6_DAC_IRQHandler           ( void );  // TIM6 and DAC1&2 underrun errors
extern void TIM7_IRQHandler               ( void );  // TIM7
extern void DMA2_Stream0_IRQHandler       ( void );  // DMA2 Stream 0
extern void DMA2_Stream1_IRQHandler       ( void );  // DMA2 Stream 1
extern void DMA2_Stream2_IRQHandler       ( void );  // DMA2 Stream 2
extern void DMA2_Stream3_IRQHandler       ( void );  // DMA2 Stream 3
extern void DMA2_Stream4_IRQHandler       ( void );  // DMA2 Stream 4
extern void ETH_IRQHandler                ( void );  // Ethernet
extern void ETH_WKUP_IRQHandler           ( void );  // Ethernet Wakeup through EXTI line
extern void CAN2_TX_IRQHandler            ( void );  // CAN2 TX
extern void CAN2_RX0_IRQHandler           ( void );  // CAN2 RX0
extern void CAN2_RX1_IRQHandler           ( void );  // CAN2 RX1
extern void CAN2_SCE_IRQHandler           ( void );  // CAN2 SCE
extern void OTG_FS_IRQHandler             ( void );  // USB OTG FS
extern void DMA2_Stream5_IRQHandler       ( void );  // DMA2 Stream 5
extern void DMA2_Stream6_IRQHandler       ( void );  // DMA2 Stream 6
extern void DMA2_Stream7_IRQHandler       ( void );  // DMA2 Stream 7
extern void USART6_IRQHandler             ( void );  // USART6
extern void I2C3_EV_IRQHandler            ( void );  // I2C3 event
extern void I2C3_ER_IRQHandler            ( void );  // I2C3 error
extern void OTG_HS_EP1_OUT_IRQHandler     ( void );  // USB OTG HS End Point 1 Out
extern void OTG_HS_EP1_IN_IRQHandler      ( void );  // USB OTG HS End Point 1 In
extern void OTG_HS_WKUP_IRQHandler        ( void );  // USB OTG HS Wakeup through EXTI
extern void OTG_HS_IRQHandler             ( void );  // USB OTG HS
extern void DCMI_IRQHandler               ( void );  // DCMI
extern void CRYP_IRQHandler               ( void );  // CRYP crypto
extern void HASH_RNG_IRQHandler           ( void );  // Hash and Rng
extern void FPU_IRQHandler                ( void );  // FPU
extern void SPI4_IRQHandler               ( void );  // SPI4
extern void SPI5_IRQHandler               ( void );  // SPI5
extern void QUADSPI_IRQHandler            ( void );  // QuadSPI
extern void FMPI2C1_EV_IRQHandler         ( void );  // FMPI2C1 Event
extern void FMPI2C1_ER_IRQHandler         ( void );  // FMPI2C1 Error

/******************************************************
 *               Variable Definitions
 ******************************************************/

/* Pointer to stack location */
extern void* _estack;


uint32_t interrupt_vector_table[] =
{
    (uint32_t)&_estack                     , // Initial stack location
    (uint32_t)Reset_Handler                , // Reset vector
    (uint32_t)NMI_Handler                  , // Non Maskable Interrupt
    (uint32_t)HardFault_Handler            , // Hard Fault interrupt
    (uint32_t)MemManage_Handler            , // Memory Management Fault interrupt
    (uint32_t)BusFault_Handler             , // Bus Fault interrupt
    (uint32_t)UsageFault_Handler           , // Usage Fault interrupt
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)SVC_Handler                  , // SVC interrupt
    (uint32_t)DebugMon_Handler             , // Debug Monitor interrupt
    (uint32_t)0                            , // Reserved
    (uint32_t)PendSV_Handler               , // PendSV interrupt
    (uint32_t)SysTick_Handler              , // Sys Tick Interrupt
    (uint32_t)WWDG_IRQHandler              , // Window WatchDog
    (uint32_t)PVD_IRQHandler               , // PVD through EXTI Line detection
    (uint32_t)TAMP_STAMP_IRQHandler        , // Tamper and TimeStamps through the EXTI line
    (uint32_t)RTC_WKUP_irq                 , // RTC Wakeup through the EXTI line
    (uint32_t)FLASH_IRQHandler             , // FLASH
    (uint32_t)RCC_IRQHandler               , // RCC
    (uint32_t)EXTI0_IRQHandler             , // EXTI Line0
    (uint32_t)EXTI1_IRQHandler             , // EXTI Line1
    (uint32_t)EXTI2_IRQHandler             , // EXTI Line2
    (uint32_t)EXTI3_IRQHandler             , // EXTI Line3
    (uint32_t)EXTI4_IRQHandler             , // EXTI Line4
    (uint32_t)DMA1_Stream0_IRQHandler      , // DMA1 Stream 0
    (uint32_t)DMA1_Stream1_IRQHandler      , // DMA1 Stream 1
    (uint32_t)DMA1_Stream2_IRQHandler      , // DMA1 Stream 2
    (uint32_t)DMA1_Stream3_IRQHandler      , // DMA1 Stream 3
    (uint32_t)DMA1_Stream4_IRQHandler      , // DMA1 Stream 4
    (uint32_t)DMA1_Stream5_IRQHandler      , // DMA1 Stream 5
    (uint32_t)DMA1_Stream6_IRQHandler      , // DMA1 Stream 6
    (uint32_t)ADC_IRQHandler               , // ADC1, ADC2 and ADC3s
    (uint32_t)CAN1_TX_IRQHandler           , // CAN1 TX
    (uint32_t)CAN1_RX0_IRQHandler          , // CAN1 RX0
    (uint32_t)CAN1_RX1_IRQHandler          , // CAN1 RX1
    (uint32_t)CAN1_SCE_IRQHandler          , // CAN1 SCE
    (uint32_t)EXTI9_5_IRQHandler           , // External Line[9:5]s
    (uint32_t)TIM1_BRK_TIM9_IRQHandler     , // TIM1 Break and TIM9
    (uint32_t)TIM1_UP_TIM10_IRQHandler     , // TIM1 Update and TIM10
    (uint32_t)TIM1_TRG_COM_TIM11_IRQHandler, // TIM1 Trigger and Commutation and TIM11
    (uint32_t)TIM1_CC_IRQHandler           , // TIM1 Capture Compare
    (uint32_t)TIM2_IRQHandler              , // TIM2
    (uint32_t)TIM3_IRQHandler              , // TIM3
    (uint32_t)TIM4_IRQHandler              , // TIM4
    (uint32_t)I2C1_EV_IRQHandler           , // I2C1 Event
    (uint32_t)I2C1_ER_IRQHandler           , // I2C1 Error
    (uint32_t)I2C2_EV_IRQHandler           , // I2C2 Event
    (uint32_t)I2C2_ER_IRQHandler           , // I2C2 Error
    (uint32_t)SPI1_IRQHandler              , // SPI1
    (uint32_t)SPI2_IRQHandler              , // SPI2
    (uint32_t)USART1_IRQHandler            , // USART1
    (uint32_t)USART2_IRQHandler            , // USART2
    (uint32_t)USART3_IRQHandler            , // USART3
    (uint32_t)EXTI15_10_IRQHandler         , // External Line[15:10]s
    (uint32_t)RTC_Alarm_IRQHandler         , // RTC Alarm (A and B) through EXTI Line
    (uint32_t)OTG_FS_WKUP_IRQHandler       , // USB OTG FS Wakeup through EXTI line
    (uint32_t)TIM8_BRK_TIM12_IRQHandler    , // TIM8 Break and TIM12
    (uint32_t)TIM8_UP_TIM13_IRQHandler     , // TIM8 Update and TIM13
    (uint32_t)TIM8_TRG_COM_TIM14_IRQHandler, // TIM8 Trigger and Commutation and TIM14
    (uint32_t)TIM8_CC_IRQHandler           , // TIM8 Capture Compare
    (uint32_t)DMA1_Stream7_IRQHandler      , // DMA1 Stream7
    (uint32_t)FMC_IRQHandler               , // FSMC
    (uint32_t)SDIO_IRQHandler              , // SDIO
    (uint32_t)TIM5_IRQHandler              , // TIM5
    (uint32_t)SPI3_IRQHandler              , // SPI3
    (uint32_t)0                            , // UART4
    (uint32_t)0                            , // UART5
    (uint32_t)0                            , // TIM6 and DAC1&2 underrun errors
    (uint32_t)0                            , // TIM7
    (uint32_t)DMA2_Stream0_IRQHandler      , // DMA2 Stream 0
    (uint32_t)DMA2_Stream1_IRQHandler      , // DMA2 Stream 1
    (uint32_t)DMA2_Stream2_IRQHandler      , // DMA2 Stream 2
    (uint32_t)DMA2_Stream3_IRQHandler      , // DMA2 Stream 3
    (uint32_t)DMA2_Stream4_IRQHandler      , // DMA2 Stream 4
    (uint32_t)0                            , // Ethernet
    (uint32_t)0                            , // Ethernet Wakeup through EXTI line
    (uint32_t)0                            , // CAN2 TX
    (uint32_t)0                            , // CAN2 RX0
    (uint32_t)0                            , // CAN2 RX1
    (uint32_t)0                            , // CAN2 SCE
    (uint32_t)OTG_FS_IRQHandler            , // USB OTG FS
    (uint32_t)DMA2_Stream5_IRQHandler      , // DMA2 Stream 5
    (uint32_t)DMA2_Stream6_IRQHandler      , // DMA2 Stream 6
    (uint32_t)DMA2_Stream7_IRQHandler      , // DMA2 Stream 7
    (uint32_t)USART6_IRQHandler            , // USART6
    (uint32_t)I2C3_EV_IRQHandler           , // I2C3 event
    (uint32_t)I2C3_ER_IRQHandler           , // I2C3 error
    (uint32_t)0                            , // USB OTG HS End Point 1 Out
    (uint32_t)0                            , // USB OTG HS End Point 1 In
    (uint32_t)0                            , // USB OTG HS Wakeup through EXTI
    (uint32_t)0                            , // USB OTG HS
    (uint32_t)0                            , // DCMI
    (uint32_t)0                            , // CRYP crypto
    (uint32_t)0                            , // Hash and Rng
    (uint32_t)FPU_IRQHandler               , //FPU
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)SPI4_IRQHandler              , // SPI4
    (uint32_t)SPI5_IRQHandler              , // SPI5
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)QUADSPI_IRQHandler           , // QuadSPI
    (uint32_t)0                            , // Reserved
    (uint32_t)0                            , // Reserved
    (uint32_t)FMPI2C1_EV_IRQHandler        , // FMPI2C1 Event
    (uint32_t)FMPI2C1_ER_IRQHandler          // FMPI2C1 Error
};

/******************************************************
 *               Function Definitions
 ******************************************************/
