# i.MX 6 Series Platform SDK

_This is a archived copy of the i.MX 6 Series Platform SDK. The last released version was 1.1 from 04-Feb-2013. Development was discontinued after this release, so the SDK does not support more recent i.MX 6 series SoCs. However, the bare metal drivers in this SDK may prove useful for developers using any i.MX device, as the IP is mostly common. There is also code that may be useful for managing Cortex-A core peripherals._

The purposes of the Platform SDK for the i.MX 6 Series are to:
- Provide easily understood driver code that supports the primary features of a
peripheral.
- Demonstrate key use cases of the chips.
- Provide unit tests for drivers to demonstrate their main features.
- Provide a simple environment for quick board validation and bring-up.
- Provide a source of entirely non-GPL example driver code.
- Includes consistent, documented register access macros for all hardware IP blocks.
- Uses the IOMUX Tool to generate mux configuration code.
- Provides register definitions for ARM RealView Debugger and Lauterbach TRACE32.

The result is an SDK that provides reusable system code and a large number of drivers with related unit tests. For board bring-up, each module can be individually tested one at a time rather than having to bring up most drives all at once. Based on a common initialization of the i.MX6 and the development board, a test for a specific peripheral is launched with the help of a serial console for human interaction.

Also included in the release package are a Firmware Guide (iMX6_Firmware_Guide.pdf) and IOMUX Tool application (IOMux.exe).

The full readme from the version 1.1 release is [README.pdf](README.pdf).


### Building

To build the SDK, use the `./tools/build_sdk` command.

Building is supported on either a Linux host or Windows host using Cygwin.

Note: It is important to run build_sdk from the SDK root directory, not from within the tools directory.


### License

Most code is licensed with a BSD 3-Clause license.

Some code, specifically the USB stack, has a different license. Please see the
License.txt file in `sdk/common/usb_stack/` for the USB license details.

Copyright (c) 2011-2013, Freescale Semiconductor, Inc.

