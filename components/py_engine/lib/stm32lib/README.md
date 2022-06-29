STM32 Cube library - CMSIS and HAL for F0, F4, F7, G4, H7, L0, L4, WB and WL microcontrollers
=============================================================================================

This repository contains a copy of STMicroelectronic's STM32 Cube firmware
library, with CMSIS-device and HAL code for STM32F0xx, STM32F4xx, STM32F7xx,
STM32G4xx, STM32H7xx, STM32L0xx, STM32L4xx, STM32WBxx and STM32WLxx
microcontrollers.  Minor changes are applied to the HAL source to support
MicroPython's port to the STM32.

The "vendor" branch contains the original sources and is updated from
time-to-time when new versions become available.  The upstream sources are
provided as separate releases (and in separate archives) for each
microcontroller class (eg F4, F7, L4).  These are merged together here in
the "vendor" branch, because the idea of the HAL code is that it provides a
uniform interface to all microcontroller classes.  The commits in the "vendor"
branch are tagged as appropriate with a tag that describes all the available
versions at that commit, for example F4-1.5.0+F7-1.1.0+L4-1.3.0.

There are then working branches that branch at a given vendor tag and apply
minor patches to the vendor code, for example work-F4-1.5.0+F7-1.1.0+L4-1.3.0.
The patches/commits that form a given working branch are reapplied (with
conflict resolution) to newer vendor tags to create the next working branch.

Original sources
================

The sources are obtained from ``http://st.com``, and also from the git
repositories found at ``https://github.com/STMicroelectronics/``.

For all .c and .h files the following processing has been done to the original
files before committing them here:
- lines endings changed to unix style
- trailing spaces removed
- tabs expanded to 4 spaces
- non-ASCII chars converted to their ASCII equivalent

Directories from the original sources are mapped into this repository according
to the following:
```
Drivers/STM32F4xx_HAL_Driver/Inc ->          STM32F4xx_HAL_Driver/Inc
Drivers/STM32F4xx_HAL_Driver/Src ->          STM32F4xx_HAL_Driver/Src
Drivers/CMSIS/Device/ST/STM32F4xx/Include -> CMSIS/STM32F4xx/Include
Drivers/CMSIS/Device/ST/STM32F4xx/Source  -> CMSIS/STM32F4xx/Source
```
And similar for the other microcontroller classes.

The included `fetch_from_upstream.sh` script can automatically copy and process
new source code from an STM git repository.
