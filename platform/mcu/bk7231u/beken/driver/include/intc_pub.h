#ifndef _INTC_PUB_H_
#define _INTC_PUB_H_

#include "generic.h"

#if 1
#define FIQ_PSRAM                        (31) 
#define FIQ_BLE							 (30)
#define FIQ_JPEG_ENCODER                 (29) 
#define FIQ_DPLL_UNLOCK                  (28) 
#define FIQ_SPI_DMA                      (27) 
#define FIQ_MAC_WAKEUP                   (26)
#if (CFG_SOC_NAME == SOC_BK7221U)
#define FIQ_SECURITY                     (25) 
#define FIQ_USB_PLUG_INOUT               (24) 
#else
#define FIQ_MAILBOX1                     (25) 
#define FIQ_MAILBOX0                     (24) 
#endif
#define FIQ_SDIO_DMA                     (23) 
#define FIQ_MAC_GENERAL                  (22) 
#define FIQ_MAC_PROT_TRIGGER             (21) 
#define FIQ_MAC_TX_TRIGGER               (20) 
#define FIQ_MAC_RX_TRIGGER               (19) 
#define FIQ_MAC_TX_RX_MISC               (18) 
#define FIQ_MAC_TX_RX_TIMER              (17) 
#define FIQ_MODEM                        (16) 

#define IRQ_GENERDMA                     (15) 
#define IRQ_FFT                          (14) 
#define IRQ_USB                          (13) 
#define IRQ_SD                           (12) 
#define IRQ_SARADC                       (11) 
#define IRQ_AUDIO                        (10) 
#define IRQ_PWM                          (9) 
#if (CFG_SOC_NAME == SOC_BK7231)
#define IRQ_TL410_WATCHDOG               (8) 
#else
#define IRQ_TIMER                        (8) 
#endif
#define IRQ_GPIO                         (7) 
#define IRQ_SPI                          (6) 
#define IRQ_I2C2                         (5) 
#define IRQ_I2S_PCM                      (4) 
#define IRQ_IRDA                         (3) 
#define IRQ_I2C1                         (2) 
#define IRQ_UART2                        (1) 
#define IRQ_UART1                        (0) 
#endif 

#if 2
#define PRI_FIQ_BLE                          (31)
#define PRI_FIQ_JPEG_DECODER                 (30) 
#define PRI_FIQ_DPLL_UNLOCK                  (29) 
#define PRI_FIQ_SPI_DMA                      (7) 
#define PRI_FIQ_MAC_WAKEUP                   (9)
#if (CFG_SOC_NAME == SOC_BK7221U)
#define PRI_FIQ_SECURITY                     (12) 
#define PRI_FIQ_USB_PLUG_INOUT               (11) 
#else
#define PRI_FIQ_MAILBOX1                     (12) 
#define PRI_FIQ_MAILBOX0                     (11) 
#endif
#define PRI_FIQ_SDIO_DMA                     (8) 
#define PRI_FIQ_MAC_GENERAL                  (1) 
#define PRI_FIQ_MAC_PROT_TRIGGER             (6) 
#define PRI_FIQ_MAC_TX_TRIGGER               (3) 
#define PRI_FIQ_MAC_RX_TRIGGER               (4) 
#define PRI_FIQ_MAC_TX_RX_MISC               (5) 
#define PRI_FIQ_MAC_TX_RX_TIMER              (2) 
#define PRI_FIQ_MODEM                        (10) 

#define PRI_IRQ_GENERDMA                     (28) 
#define PRI_IRQ_FFT                          (13) 
#define PRI_IRQ_USB                          (14) 
#define PRI_IRQ_SD                           (15) 
#define PRI_IRQ_SARADC                       (16) 
#define PRI_IRQ_AUDIO                        (27) 
#define PRI_IRQ_PWM                          (17) 
#if (CFG_SOC_NAME == SOC_BK7231)
#define PRI_IRQ_TL410_WATCHDOG               (18)
#else
#define PRI_IRQ_TIMER                        (18)
#endif
#define PRI_IRQ_GPIO                         (19) 
#define PRI_IRQ_SPI                          (20) 
#define PRI_IRQ_I2C2                         (21) 
#define PRI_IRQ_I2S_PCM                      (22) 
#define PRI_IRQ_IRDA                         (23) 
#define PRI_IRQ_I2C1                         (24) 
#define PRI_IRQ_UART2                        (25) 
#define PRI_IRQ_UART1                        (26)
#endif 

extern void intc_service_register(UINT8 int_num, UINT8 int_pri, FUNCPTR isr);
extern void intc_spurious(void);
extern void intc_enable(int index);
extern void intc_disable(int index);

#endif // _INTC_PUB_H_
// eof
