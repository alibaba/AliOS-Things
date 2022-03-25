# Changelog

All notable changes to this project will be documented in this file, in reverse chronological order by release.

## [0.6.0-dev](https://github.com/tuupola/axp192/compare/0.5.0...master) - unreleased

### Added

- Support for reading raw register values ([#24](https://github.com/tuupola/axp192/pull/24))
- Support for manually setting the rail voltages ([#27](https://github.com/tuupola/axp192/pull/27))
- Support for manually enablind and disabling DCDC1 ([#29](https://github.com/tuupola/axp192/pull/29))
- Support for manually enablind and disabling DCDC2 ([#30](https://github.com/tuupola/axp192/pull/30))
- Support for manually enablind and disabling EXTEN ([#31](https://github.com/tuupola/axp192/pull/31))
- Support for manually enablind and disabling LDOIO0 ([#32](https://github.com/tuupola/axp192/pull/32))
- Support for writing a raw byte directly to a register([#33](https://github.com/tuupola/axp192/pull/33))

### Removed

- All moot ioctl commands ([#25](https://github.com/tuupola/axp192/pull/25)).
    ```c
    /* Removed */
    axp192_ioctl(&axp, AXP192_READ_POWER_STATUS, &power);
    axp192_ioctl(&axp, AXP192_READ_CHARGE_STATUS, &charge);

    /* Use this instead. */
    axp192_read(&axp, AXP192_POWER_STATUS, &power);
    axp192_read(&axp, AXP192_CHARGE_STATUS, &charge);
    ```

### Changed

- Using menuconfig is now optional ([#28](https://github.com/tuupola/axp192/pull/28)).
- Third parameter of `axp192_ioctl()` is now optional ([#26](https://github.com/tuupola/axp192/pull/26)).
- Splitted AXP192_LDO2_SET_CONTROL to separate commands ([#26](https://github.com/tuupola/axp192/pull/26)).
    ```c
    axp192_ioctl(&axp, AXP192_LDO2_ENABLE);
    axp192_ioctl(&axp, AXP192_LDO2_DISABLE);
    ```
- Splitted AXP192_LDO3_SET_CONTROL to separate commands ([#26](https://github.com/tuupola/axp192/pull/26)).
    ```c
    axp192_ioctl(&axp, AXP192_LDO3_ENABLE);
    axp192_ioctl(&axp, AXP192_LDO3_DISABLE);
    ```
- Splitted AXP192_DCDC3_SET_CONTROL to separate commands ([#26](https://github.com/tuupola/axp192/pull/26)).
    ```c
    axp192_ioctl(&axp, AXP192_DCDC3_ENABLE);
    axp192_ioctl(&axp, AXP192_DCDC3_DISABLE);
    ```


## [0.5.0](https://github.com/tuupola/axp192/compare/0.4.0...0.5.0) - 2021-09-03

This version adds features needed for using with M5STack Core2.

### Added

- IOCTL support for setting GPIO0 level ([#20](https://github.com/tuupola/axp192/issues/20))
- IOCTL support for setting GPIO2 level ([#19](https://github.com/tuupola/axp192/issues/19))
- IOCTL support to enable and disable LDO2 ([#19](https://github.com/tuupola/axp192/issues/18))
- IOCTL support to enable and disable DCDC3 ([#17](https://github.com/tuupola/axp192/issues/17))
- IOCTL support for setting GPIO4 level ([#15](https://github.com/tuupola/axp192/issues/15))
- IOCTL support for setting GPIO4 level ([#13](https://github.com/tuupola/axp192/issues/13))
- IOCTL support for setting GPIO1 level ([#12](https://github.com/tuupola/axp192/issues/12))
- IOCTL support to enable and disable LDO3 ([#11](https://github.com/tuupola/axp192/issues/11))
- Support for AXP192_GPIO43_FUNCTION_CONTROL ([#10](https://github.com/tuupola/axp192/issues/10))
- Support for AXP192_GPIO2_CONTROL ([#9](https://github.com/tuupola/axp192/issues/9))
- Support for AXP192_GPIO1_CONTROL ([#8](https://github.com/tuupola/axp192/issues/8))
- Support for AXP192_DCDC3_VOLTAGE ([#6](https://github.com/tuupola/axp192/issues/6))
- Support for AXP192_DCDC1_VOLTAGE ([#5](https://github.com/tuupola/axp192/issues/5))
- Support for AXP192_BATTERY_CHARGE_CONTROL ([#4](https://github.com/tuupola/axp192/issues/4))

### Changed

- `AXP192_GPIO40_FUNCTION_CONTROL` is now `AXP192_GPIO43_FUNCTION_CONTROL`
- `AXP192_GPIO40_SIGNAL_STATUS` is now `AXP192_GPIO43_SIGNAL_STATUS`

## [0.4.0](https://github.com/tuupola/axp192/compare/0.3.0...0.4.0) - 2020-11-09

### Added

- DOIO0, LDO23 and charge control default values in menuconfig ([#2](https://github.com/tuupola/axp192/issues/2))

## [0.3.0](https://github.com/tuupola/axp192/compare/0.2.0...0.3.0) - 2020-05-20
### Added

- Optional handle parameter for the I2C HAL ([#1](https://github.com/tuupola/axp192/issues/1))

### Changed

- `AXP192_ERROR_OK` is now `AXP192_OK`
- All LDO output defaults to 3V0 in menuconfig

## [0.2.0](https://github.com/tuupola/axp192/compare/0.1.0...0.2.0) - 2020-04-21
### Changed

- I2C HAL config is now a struct

    ```c
    axp192_t axp;

    axp.read = &i2c_read;
    axp.write = &i2c_write;

    axp192_init(&axp);
    ```

## 0.1.0 - 2020-04-12

Initial realese.
