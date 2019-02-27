## Overview

STM32L4 series shatters performance limits in the ultra-low-power world. It delivers 100 DMIPS based on its Arm® Cortex®-M4 core with FPU and ST ART Accelerator™ at 80 MHz.

STM32L4 microcontrollers offer dynamic voltage scaling to balance power consumption with processing demand, low-power peripherals (LP UART, LP timers) available in Stop mode, safety and security features, smart and numerous peripherals, advanced and low-power analog peripherals such as op amps, comparators, LCD, 12-bit DACs and 16-bit ADCs (hardware oversampling).

The series are available in different lines: STM32L4x1 (Access line), STM32L4x2 (USB Device), STM32L4x3 (USB Device, LCD), STM32L4x5 (USB OTG) and STM32L4x6 (USB OTG, LCD).

### Directories

```sh
stm32l4xx
├── aos                         # aos system interfaces
│   ├── app_runtime             # app runtime for multi-bins
│   ├── aos.c                   # aos kernel startup
│   ├── hook_impl.c             # aos kernel hooks
│   ├── soc_impl.c              # soc tick, heap, stack etc.
│   └── trace_impl.c            # aos track feature
├── Drivers                     # stm32l4xx drivers
│   ├── CMSIS
│   └── STM32L4xx_HAL_Driver
└── hal                         # aos hal interfaces
    ├── hal_xxx_stm32l4.c
    ├── hw.c                    # hardware hal interface: reboot, delay etc.
    └── app_runtime             # app runtime for multi-bins
```

## Resources

check https://www.st.com/en/microcontrollers/stm32l4-series.html?querycriteria=productId=SS1580.

## Reference

- [AliOS Things Developer Kit Quick Start Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Quick-Start-Guide)
- [stm32cubel4 resources](https://www.st.com/zh/embedded-software/stm32cubel4.html)
- [STM32CubeMX](https://www.st.com/content/st_com/zh/products/development-tools/software-development-tools/stm32-software-development-tools/stm32-configurators-and-code-generators/stm32cubemx.html)
- [stm32l4-online-training](https://www.st.com/content/st_com/zh/support/learning/stm32l4-online-training.html)