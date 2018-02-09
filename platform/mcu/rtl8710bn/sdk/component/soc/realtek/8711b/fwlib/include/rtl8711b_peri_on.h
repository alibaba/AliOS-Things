#ifndef __INC_RTL8710B_PERI_ON_H
#define __INC_RTL8710B_PERI_ON_H

#define MASK_ALLON                          0xFFFFFFFF

#define HAL_PERI_ON_READ32(addr)			HAL_READ32(PERI_ON_BASE, addr)
#define HAL_PERI_ON_WRITE32(addr, value)	HAL_WRITE32(PERI_ON_BASE, addr, value)
#define HAL_PERL_ON_FUNC_CTRL(addr,value,ctrl)  \
		HAL_PERI_ON_WRITE32(addr, ((HAL_PERI_ON_READ32(addr) & (~value))|((MASK_ALLON - ctrl + 1) & value)))

//2 0x200 REG_PEON_PWR_CTRL
#define BIT_SOC_UAHV_EN					BIT(2)
#define BIT_SOC_UALV_EN					BIT(1)
#define BIT_SOC_USBD_EN					BIT(0)
 
//2 0x210 REG_SOC_FUNC_EN
#define BIT_SOC_BOOT_FROM_JTAG			BIT(31)
#define BIT_SOC_UNLOCK_FROM_JTAG			BIT(30)
#define BIT_SOC_WAKE_FROM_PS				BIT(29)
#define BIT_SOC_PATCH_FUNC0				BIT(28)
#define BIT_SOC_PATCH_FUNC1				BIT(27)
#define BIT_SOC_PATCH_FUNC2				BIT(26)

#define BIT_SOC_XMODEM_PAGE_PG			BIT(25)
#define BIT_SOC_XMODEM_FLASH_EMPTY		BIT(24)

#define BIT_SOC_SECURITY_ENGINE_EN		BIT(20)
#define BIT_SOC_GTIMER_EN					BIT(16)
#define BIT_SOC_GDMA1_EN					BIT(14)
#define BIT_SOC_GDMA0_EN					BIT(13)
#define BIT_SOC_LOG_UART_EN				BIT(12)
#define BIT_SOC_CPU_EN						BIT(8)
#define BIT_SOC_MEM_CTRL_EN				BIT(6)
#define BIT_SOC_FLASH_EN					BIT(4)
#define BIT_SOC_LXBUS_EN					BIT(2)
#define BIT_SOC_OCP_EN						BIT(1)
#define BIT_SOC_FUN_EN						BIT(0)
#define LXBUS_FCTRL(ctrl)						HAL_PERL_ON_FUNC_CTRL(REG_SOC_FUNC_EN, BIT_SOC_LXBUS_EN, ctrl)

//2 0x0214 REG_SOC_HCI_COM_FUNC_EN
#define BIT_SOC_HCI_WL_MACON_EN			BIT(16)
#define BIT_SOC_HCI_SM_SEL					BIT(13)
#define BIT_SOC_HCI_OTG_RST_MUX			BIT(5)
#define BIT_SOC_HCI_OTG_EN					BIT(4)
#define BIT_SOC_HCI_SDIOD_ON_RST_MUX		BIT(3)
#define BIT_SOC_HCI_SDIOD_OFF_EN			BIT(1)
#define BIT_SOC_HCI_SDIOD_ON_EN			BIT(0)
#define WL_MACON_FCTRL(ctrl)				HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_WL_MACON_EN, ctrl)
#define SDIOD_OFF_FCTRL(ctrl)				HAL_PERL_ON_FUNC_CTRL(REG_SOC_HCI_COM_FUNC_EN, BIT_SOC_HCI_SDIOD_OFF_EN, ctrl)
									
//2 0x0218 REG_SOC_PERI_FUNC0_EN
#define BIT_PERI_I2S0_EN					BIT(24)
#define BIT_PERI_I2C1_EN					BIT(17)
#define BIT_PERI_I2C0_EN					BIT(16)
#define BIT_PERI_SPI1_EN					BIT(9)
#define BIT_PERI_SPI0_EN					BIT(8)
#define BIT_PERI_UART2_EN					BIT(2)
#define BIT_PERI_UART1_EN					BIT(1)
#define BIT_PERI_UART0_EN					BIT(0)

//2 0x021C REG_SOC_PERI_FUNC1_EN
#define BIT_PERI_GPIO_EN					BIT(8)
#define BIT_PERI_ADC0_EN					BIT(0)

//2 0x0220 REG_SOC_PERI_BD_FUNC0_EN
#define BIT_PERI_UART2_BD_EN				BIT(2)
#define BIT_PERI_UART1_BD_EN				BIT(1)
#define BIT_PERI_UART0_BD_EN				BIT(0)

//2 0x0230 REG_PESOC_CLK_CTRL
#define BIT_SOC_SLPCK_BTCMD_EN			BIT(29)
#define BIT_SOC_ACTCK_BTCMD_EN			BIT(28)
#define BIT_SOC_ACTCK_GPIO_EN				BIT(24)
#define BIT_SOC_ACTCK_GDMA1_EN			BIT(18)
#define BIT_SOC_ACTCK_GDMA0_EN			BIT(16)
#define BIT_SOC_ACTCK_TIMER_EN			BIT(14)
#define BIT_SOC_ACTCK_LOG_UART_EN		BIT(12)
#define BIT_SOC_ACTCK_FLASH_EN			BIT(8)
#define BIT_SOC_ACTCK_VENDOR_REG_EN		BIT(6)
#define BIT_SOC_ACTCK_TRACE_EN			BIT(4)
#define BIT_SOC_CKE_PLFM					BIT(2)
#define BIT_SOC_CKE_OCP					BIT(0)

//2 0x0234 REG_PESOC_PERI_CLK_CTRL0
#define BIT_SOC_ACTCK_SPI1_EN				BIT(18)
#define BIT_SOC_ACTCK_SPI0_EN				BIT(16)
#define BIT_SOC_ACTCK_UART1_EN			BIT(2)
#define BIT_SOC_ACTCK_UART0_EN			BIT(0)

//2 0x0238 REG_PESOC_PERI_CLK_CTRL1
#define BIT_SOC_ACTCK_ADC_EN				BIT(24)
#define BIT_SOC_ACTCK_I2S_EN				BIT(16)
#define BIT_SOC_ACTCK_I2C1_EN				BIT(2)
#define BIT_SOC_ACTCK_I2C0_EN				BIT(0)

//2 0x0240 REG_PESOC_HCI_CLK_CTRL0
#define BIT_SOC_ACTCK_OTG_EN				BIT(4)
#define BIT_SOC_ACTCK_SDIO_HST_EN			BIT(2)		// SDIO_HST clock enable in CPU run mode 
#define BIT_SOC_ACTCK_SDIO_DEV_EN			BIT(0)		// SDIO_DEV clock enable in CPU run mode

/* REG_PESOC_CLK_SEL 0x0250 */
#define BIT_SHIFT_PESOC_UART1_SCLK_SEL	26
#define BIT_MASK_PESOC_UART1_SCLK_SEL	0x03	/* [27:26] uart1 rx clock, 01: osc 8m;   00: xtal;    10: xtal nco */
#define BIT_SHIFT_PESOC_UART0_SCLK_SEL	19
#define BIT_MASK_PESOC_UART0_SCLK_SEL	0x03	/* [20:19] uart0 rx clock, 01: osc 8m;   00: xtal;    10: xtal nco */

#define BIT_FLASH_CK_PS_DIV_EN				BIT(25)	/* 1: enable to generate flash clock (with phase shift) divided by 500M pll clock, HW detect this signal¡¯s rising edge to start the phase shift clock division circuit. */
#define BIT_FLASH_CK_DIV_EN				BIT(24)	/* 1: enable to generate flash clock (no phase shift) divided by 500M pll clock, HW detect this signal¡¯s rising edge to start the no phase shift division circuit. */
#define BIT_FLASH_CAL_EN					BIT(23)	/* 1: delay flash sout for calibration;  0: bypass flash sout to spic */
#define BIT_SHIFT_FLASH_CK_PS_INT			12
#define BIT_MASK_FLASH_CK_PS_INT			0x03	/* [14:12] Flash clock phase shift in units of 500M pll clock cycels */
#define BIT_FLASH_PS_DIV_RDY				BIT(7)	/* ready flag of Flash clock with phase shift, Read only */
#define BIT_FLASH_DIV_RDY					BIT(6)	/* ready flag of Flash clock, Read only */
#define BIT_SHIFT_PESOC_TRACE_CK_SEL 		4
#define BIT_MASK_PESOC_TRACE_CK_SEL 		0x03	/* [5:4] "Trace clock select0: 12.5MH1: 25MHz2: 50MHz3: 100MHz" */
#define BIT_FLASH_DIV_HIGH_FRAC			BIT(3)	/* "Only valid when r_FLASH_DIV_FRAC= 1, it decides the duty cycle of flash clock when not divided by integer1: duty cycle > 50% ; 0: duty cycle < 50%" */
#define BIT_FLASH_DIV_FRAC					BIT(2)	/* "Flash clock division ratio, fractional part0: no fraction, only divided by integer set by bit[1:0],  1: 0.5" */
#define BIT_SHIFT_DIV_INT					0
#define BIT_MASK_FLASH_DIV_INT			0x03	/* [1:0] "Flash clock division ratio, integrate part0: divided by 21: divided by 32: divided by 43: divided by 5" */


//0x0244 REG_PESOC_COM_CLK_CTRL1
#define BIT_SOC_ACTCK_SECURITY_ENG_EN	BIT(4)
#define BIT_SOC_ACTCK_LXBUS_EN			BIT(0) //spec name is wrong (BIT_SOC_ACTCK_WL_EN)

//0x02E0 REG_PON_PINMUX_CTRL
#define BIT_HCI_SDIOD_PIN_EN				BIT(0)
#define SDIOD_PIN_FCTRL(ctrl)				HAL_PERL_ON_FUNC_CTRL(REG_PON_PINMUX_CTRL, BIT_HCI_SDIOD_PIN_EN, ctrl)

//0x0304 REG_PESOC_SOC_CTRL
#define BIT_PESOC_LX_SLV_SWAP_SEL			BIT(10)
#define BIT_PESOC_LX_MST_SWAP_SEL		BIT(9)
#define BIT_PESOC_LX_WL_SWAP_SEL			BIT(8)
#define LX_WL_SWAP_CTRL(ctrl)				HAL_PERL_ON_FUNC_CTRL(REG_PESOC_SOC_CTRL, BIT_PESOC_LX_WL_SWAP_SEL, ctrl)
#define LX_MST_SWAP_CTRL(ctrl)				HAL_PERL_ON_FUNC_CTRL(REG_PESOC_SOC_CTRL, BIT_PESOC_LX_MST_SWAP_SEL, ctrl)

//0x2FC REG_FW_PPROTECT_KEY_CTRL
#define BIT_RDP_EN							BIT(3) /* load from efuse */
#define BIT_RDP_EN_LOAD					BIT(2)
#define BIT_RDP_KEY_REQ					BIT(1)
#define BIT_OTF_KEY_REQ					BIT(0)

//==========PERION Register Address Definition ==================//
#define REG_PEON_PWR_CTRL					0x0200
#define REG_PON_ISO_CTRL					0x0204
#define REG_SOC_FUNC_EN					0x0210
#define REG_SOC_HCI_COM_FUNC_EN			0x0214
#define REG_SOC_PERI_FUNC0_EN				0x0218
#define REG_SOC_PERI_FUNC1_EN				0x021C
#define REG_SOC_PERI_BD_FUNC0_EN			0x0220
#define REG_PESOC_CLK_CTRL					0x0230
#define REG_PESOC_PERI_CLK_CTRL0			0x0234
#define REG_PESOC_PERI_CLK_CTRL1			0x0238
#define REG_PESOC_CLK_CTRL3				0x023C
#define REG_PESOC_HCI_CLK_CTRL0			0x0240
#define REG_PESOC_COM_CLK_CTRL1			0x0244
#define REG_PESOC_HW_ENG_CLK_CTRL		0x0248
#define REG_PESOC_CLK_SEL					0x0250
#define REG_UART_NCO_CTRL					0x026C

//#define REG_OSC32K_CTRL 0x0270 /* AmebaZ move to 0xf0 */
#define REG_OSC32K_REG_CTRL0				0x0274
#define REG_OSC32K_REG_CTRL1				0x0278

#define REG_THERMAL_METER_CTRL			0x027C /* AmebaZ no change */

/* 0x0280~0x2D0: 41 16bits for pinmux pad control */
#define REG_GPIO_PINMUX_CTRL				0x0280

#define REG_PON_PINMUX_CTRL				0x02E0

/* request OFT Key autoload */
#define REG_FW_PPROTECT_KEY_CTRL			0x02FC

#define REG_PESOC_SOC_CTRL					0x0304

#endif  // end of "#ifndef __INC_RTL8710B_PERI_ON_H"
