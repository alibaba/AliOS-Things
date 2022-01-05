CryptoAuthLib - Microchip CryptoAuthentication Library
====================================================

Introduction
------------------------
This code base implements an object-oriented C library which supports
Microchip CryptoAuth devices.  The family of devices supported currently are:

- [ATSHA204A](http://www.microchip.com/ATSHA204A)
- [ATECC108A](http://www.microchip.com/ATECC108A)
- [ATECC508A](http://www.microchip.com/ATECC508A)
- [ATECC608A](http://www.microchip.com/ATECC608A)

Online documentation is at https://microchiptech.github.io/cryptoauthlib/

Latest software and examples can be found at:
  - http://www.microchip.com/SWLibraryWeb/product.aspx?product=CryptoAuthLib
  - https://github.com/MicrochipTech/cryptoauthtools

Prerequisite skills:
  - strong C programming and code reading
  - Atmel Studio familiarity
  - Knowledge of flashing microcontrollers with new code
  - Familiarity with Microchip CryptoAuth device functionality

Prerequisite hardware to run CryptoAuthLib examples:
  - [ATSAMR21 Xplained Pro]( http://www.microchip.com/atsamr21-xpro )
    or [ATSAMD21 Xplained Pro]( http://www.microchip.com/ATSAMD21-XPRO )
  - [CryptoAuth Xplained Pro Extension](http://www.microchip.com/developmenttools/productdetails.aspx?partno=atcryptoauth-xpro-b )
    or [CryptoAuthentication SOIC Socket Board](http://www.microchip.com/developmenttools/productdetails.aspx?partno=at88ckscktsoic-xpro )
    to accept SOIC parts

For most development, using socketed top-boards is preferable until your
configuration is well tested, then you can commit it to a CryptoAuth Xplained
Pro Extension, for example. Keep in mind that once you lock a device, it will
not be changeable.

There are two major compiler defines that affect the operation of the library.
  - ATCA_NO_POLL can be used to revert to a non-polling mechanism for device
    responses. Normally responses are polled for after sending a command,
    giving quicker response times. However, if ATCA_NO_POLL is defined, then
    the library will simply delay the max execution time of a command before
    reading the response.
  - ATCA_NO_HEAP can be used to remove the use of malloc/free from the main
    library. This can be helpful for smaller MCUs that don't have a heap
    implemented. If just using the basic API, then there shouldn't be any code
    changes required. The lower-level API will no longer use the new/delete
    functions and the init/release functions should be used directly.

Examples
-----------

  - Watch [CryptoAuthLib Documents](http://www.microchip.com/design-centers/security-ics/cryptoauthentication/overview )
    for new examples coming online.
  - Node Authentication Example Using Asymmetric PKI is a complete, all-in-one
    example demonstrating all the stages of crypto authentication starting from
    provisioning the Crypto Authentication device ATECC608A/ATECC508A with keys
    and certificates to demonstrating an authentication sequence using
    asymmetric techniques.
    http://www.microchip.com/SWLibraryWeb/product.aspx?product=CryptoAuthLib

Release notes
-----------
11/22/2019
  - Patches for CVE-2019-16128 & CVE-2019-16129: Ensure reported packet length
    is valid for the packet being processed.
  - Improvement to encrypted read operations to allow supply of a host nonce
    (prevent replay of a read sequence to the host). Default API is changed
    but can be reverted by setting the option ATCA_USE_CONSTANT_HOST_NONCE
  - Added Azure compatible TNGTLS and TNGLORA certificates. Use the TNG client
    API to retrieve the proper certificate based on the device.
  - Misc Python updates (updated APIs for encrypted reads to match the C-API change)
    atcacert_cert_element_t now initializes properly 

08/30/2019
  - Added big-endian architecture support
  - Fixes to atcah_gen_dig() and atcah_nonce()

05/17/2019
  - Added support for TNG devices (cert transforms, new API)
  - atcab_write_pub_key() now works when the data zone is unlocked

03/04/2019
  - mbed TLS wrapper added
  - Minor bug fixes

01/25/2019
  - Python JWT support
  - Python configuration structures added
  - Restructure of secure boot app

01/04/2019
  - Added GCM functions
  - Split AES modes into separate files
  - Bug fix in SWI START driver

10/25/2018
  - Added basic certificate functions to the python wrapper.
  - Added Espressif ESP32 I2C driver.
  - Made generic Atmel START drivers to support most MCUs in START.
  - Added AES-CTR mode functions.
  - Python wrapper functions now return single values with AtcaReference.
  - Added mutex support to HAL and better support for freeRTOS.
  
08/17/2018
  - Better support for multiple kit protocol devices

07/25/2018
  - Clean up python wrapper

07/18/2018
  - Added ATCA_NO_HEAP define to remove use of malloc/free.
  - Moved PEM functions to their own file in atcacert.
  - Added wake retry to accomodate power on self test delay.
  - Added ca_cert_def member to atcacert_def_s so cert chains can be traversed
    as a linked list.

03/29/2018
  - Added support for response polling by default, which will make commands
    return faster (define ATCA_NO_POLL to use old delay method).
  - Removed atcatls related files as they were of limited value.
  - Test framework generates a prompt before locking test configuration.
  - Test framework puts device to sleep between tests.
  - Fixed mode parameter issue in atcah_gen_key_msg().
  - ATECC608A health test error code added.

01/15/2018
  - Added AES-128 CBC implementation using AES command
  - Added AES-128 CMAC implementation using AES command
  
11/22/2017
  - Added support for FLEXCOM6 on SAMG55 driver
  
11/17/2017
  - Added library support for the ATECC608A device
  - Added support for Counter command
  - atca_basic functions and tests now split into multiple files based on
    command
  - Added support for multiple base64 encoding rules
  - Added support for JSON Web Tokens (jwt)
  - Fixed atcab_write_enc() function to encrypt the data even when the device
    is unlocked
  - Fixed atcab_base64encode_() for the extra newline
  - Updated atcab_ecdh_enc() to work more consistently

07/01/2017
  - Removed assumption of SN[0:1]=0123, SN[8]=EE. SN now needs to be passed in
    for functions in atca_host and atca_basic functions will now read the
    config zone for the SN if needed.
  - Renamed atcab_gendig_host() to atcab_gendig() since it's not a host
    function. Removed original atcab_gendig(), which had limited scope.
  - Fixed atcah_hmac() for host side HMAC calculations. Added atcab_hmac().
  - Removed unnecessary ATCADeviceType parameters from some atca_basic
    functions.
  - Added atcacert_create_csr() to create a signed CSR.
  - New HAL implementation for Kit protocol over HID on Linux. Please see the
    Incorporating CryptoAuthLib in a Linux project using USB HID devices
    section in this file for more information.
  - Added atcacert_write_cert() for writing certificates to the device.
  - Added support for dynamic length certificate serial numbers in atcacert.
  - Added atcab_write() for lower level write commands.
  - Fixed atcah_write_auth_mac(), which had wrong OpCode.
  - Added atcab_verify() command for lower level verify commands.
  - Added atcab_verify_stored() for verifying data with a stored public key.
  - Removed atcab_write_bytes_slot(). Use atcab_write_bytes_zone() instead.
  - Modified atcab_write_bytes_zone() and atcab_read_bytes_zone() to specify a
    slot
  - Added atcab_verify_validate() and atcab_verify_invalidate()
  - Improvements to host functions to handle more cases.
  - Added atcab_updateextra(), atcab_derive_key()
  - Added support for more certificate formats.
  - Added general purpose hardware SHA256 functions. See atcab_hw_sha2_256().
  - Removed device specific config read/write. Generic now handles both.
  - Removed unnecessary response parameter from lock commands.
  - Enhanced and added unit tests.
  - Encrypted read and write functions now handle keys with SlotConfig.NoMac
    set
  - atcab_cmp_config_zone() handles all devices now.
  - Fixed some edge cases in atcab_read_bytes_zone().
  - Updated atSHA() to work with all devices.
  - Fixed atcacert_get_device_locs() when using stored sn.
  
01/08/2016
  - New HAL implementations for
    - Single Wire interface for SAMD21 / SAMR21
    - SAMV71 I2C HAL implementation
    - XMega A3Bu HAL implementation
  - Added atcab_version() method to return current version string of libary to
    application
  - New Bus and Discovery API 
    - returns a list of ATCA device configurations for each CryptoAuth device
      found
    - currently implemented on SAMD21/R21 I2C, SAMV71
    - additional discovery implementations to come
  - TLS APIs solidified and documented
  - Added missing doxygen documentation for some CryptoAuthLib methods 
  - Stubs for HAL SPI removed as they are unused for SHA204A and ECC508A
    support
  - bug fixes
  - updated atcab_sha() to accept a variable length message that is > 64 bytes
    and not a multiple of 64 bytes (the SHA block size).
  - refactored Cert I/O and Cert Data tests to be smaller
  - 'uncrustify' source formatting
  - published on GitHub

9/19/2015
  - Kit protocol over HID on Windows
  - Kit protocol over CDC on Linux
  - TLS integration with ATECC508A
  - Certificate I/O and reconstruction 
  - New SHA2 implementation
  - Major update to API docs, Doxygen files found in cryptoauthlib/docs
  - load cryptoauthlib/docs/index.html with your browser

Host Device Support
---------------

CryptoAuthLib will run on a variety of platforms from small micro-controllers
to desktop host systems.  The current list of hardware abstraction layer
support includes:

Rich OS Hosts:
  - Linux Kit Protocol over CDC USB
  - Linux Kit Protocol over HID USB
  - Linux I2C protocol.
  - Windows Kit Protocol over CDC USB
  - Windows Kit Protocol over HID USB

Microcontrollers:
  - SAMD21 (I2C, SWI, and Bit Banging)
  - SAMR21 (I2C and SWI)
  - SAM4S  (I2C)
  - SAMV71 (I2C)
  - SAMB11 (I2C)
  - SAMG55 (I2C)
  - AVR XMEGA A3BU  (I2C and SWI)
  - AVR AT90USB1287 (I2C and SWI)
  - PIC32MX695F512H (I2C)

If you have specific microcontrollers or Rich OS platforms you need support
for, please contact us through the Microchip portal with your request.

CryptoAuthLib Architecture
----------------------------
See the 'docs' directory of CryptoAuthLib for supporting documentation
including architecture diagrams and more detailed usage docs.

The library is structured to support portability to:
  - multiple hardware/microcontroller platforms
  - multiple environments including bare-metal, Windows, and Linux OS
  - multiple chip communication protocols (I2C, SPI, UART, and SWI)

All platform dependencies are contained within the HAL (hardware abstraction
layer).

![CryptoAuthLib Architecture](./docs/CryptoAuthLib-Architecture.png "CryptoAuthLib Architecture" )

There are three primary object types in CryptoAuthLib:
  - Device (ATCADevice)
  - Command (ATCACommand)
  - Interface (ATCAIface)
  
ATCADevice is a composite object made up of ATCACommand ATCAIface.

![ATCADevice](./docs/cryptoauthlib-uml/ATCADevice.png "ATCADevice and ATCACommand object" )
![ATCAIface](./docs/cryptoauthlib-uml/ATCAIFace.png "ATCAIface object" )
![Hardware abstraction layer](./docs/cryptoauthlib-uml/ATCAHal.png "CryptoAuth hardware abstraction layer" )

Currently, the vast majority of testing has been performed on:

  - ATSAMR21 Xplained Pro
  - ATSAMD21 Xplained Pro
  - ATSAMV71 Xplained Pro
  - Windows (kit protocol HID)

These host containers implement a host test environment and
test console to exercise tests.  They presume that a CryptoAuth Xplained Pro
or other I2C socket for an ATECC608A/ATECC508A/ATECC108A/ATSHA204A are
connected to the I2C pins of the host Xplained Pro development board or in the
case of windows is using a HID connection to an  ATCK101 or ATCK590.

The unit tests and basic tests exercise the core datasheet commands of
the device as well as the more convenient, basic API methods.

If you need an example of how to use a command, these hosts and tests are a
good place to reference.

Object Architecture
--------------------------
Even though this is a C library, it follows object-oriented design patterns.

An object is minimally defined to be data and the actions which operate on that
data.

Each CryptoAuth device is a composite object, a structure which includes
the command table (list of commands) which are valid for the device, and the
data used to hold the state of that device.

ATCADevice is the object which represents the Microchip CryptAuth device

ATCACommand is the object which represents the valid methods of the Device.

ATCAInterface is the physical interface object (I2C or SWI instance).
Currently, each Device may have a single OATCAInterface.

ATCADevice represents an ATSHA or ATECC family device.

In order to add new protocol support for a platform, you provide a HAL
(hardware abstraction layer) C file for the protocol and target.  In your
project's IDE or Makefile, you select which HAL support you need for the 
hardware configuration.  Generally, there are separate files for each 
protocol and platform combination - (ie: samd21_i2c_asf.c would target SAMD21 
MCUs with I2C using the ASF low-level driver support.)

Directory Structure
-----------------------
```bash
docs - AppNotes and Doxygen HTML documentation for the library API. Load "docs/html/index.html" in your browser
lib - primary library source code
lib/atcacert - certificate data and i/o methods
lib/basic - the Basic API way to access the core classes
lib/crypto - Software crypto implementations (primarily SHA1 and SHA256)
lib/hal - hardware abstraction layer code for supporting specific platforms
lib/host - support functions for common host-side calculations
lib/jwt - json web token functions
test - unit tests. See test/cmd-processor.c for main() implementation.

For production code, test directories should be excluded by not compiling it
into a project, so it is up to the developer to include or not as needed.  Test
code adds significant bulk to an application - it's not intended to be included
in production code.
```

Tests
------------

There is a set of unit tests found in the test directory which will at least
partially demonstrate the use of the objects.  Some tests may depend upon a 
certain device being configured in a certain way and may not work for all
devices or specific configurations of the device.

The test/cmd-processor.c file contains a main() function for running the tests.
It implements a command-line interface. Typing help will bring up the list of
commands available.

One first selects a device type, with one of the following commands:
  - 204 (ATSHA204A)
  - 108 (ATECC108A)
  - 508 (ATECC508A)
  - 608 (ATECC608A)
  
From there the following unit test sweets are available:
  - unit (test command builder functions)
  - basic (test basic API functions)
  - cio (test certification i/o functions)
  - cd (test certificate data functions)
  - util (test utility functions)
  - crypto (test software crypto functions)
  
Unit tests available depend on the lock level of the device. The unit tests
won't lock the config or data zones automatically to allow retesting at desired
lock levels. Therefore, some commands will need to be repeated after locking
to exercise all available tests.

Starting from a blank device, the sequence of commands to exercise all unit
tests is:
```text
unit
basic
lockcfg
unit
basic
lockdata
unit
basic
cio
cd
util
crypto
```

Using CryptoAuthLib (Microchip CryptoAuth Library)
===========================================

Using a new library is often easier when you can load an example and see how 
it works.  We've provided examples in the form of "host containers" which are
host projects that incorporate CryptoAuthLib and target various processors
or communication APIs.

We maintain host test containers for each of the HAL layers we support.
We've published the host container for SAMD21 which demonstrates a simple
console interface to invoke test runners.

Look for SAMD21 Unit Tests CryptoAuthLib at
http://www.microchip.com/SWLibraryWeb/product.aspx?product=CryptoAuthLib

The best way to learn how to use CryptoAuthLib is to study the host test
projects that exercise the library and ATECC and ATSHA devices.

New examples will be forthcoming as the software matures.  Continue checking 
the [CryptoAuthentication](http://www.microchip.com/design-centers/security-ics/cryptoauthentication/overview) 
web page for new updates.

Using Git to Incorporate CryptoAuthLib as a Submodule
-----------------------------------
You can include this project in your own project under git.

Using CryptoAuthLib as a git submodule, you can maintain your application
separately from CryptoAuthLib.

If your project is already in git but you haven't yet intergrated 
CryptoAuthLib, change to the directory where you want to put CryptoAuthLib

```bash
git submodule add -b master <giturl to CryptoAuthLib>
```

This adds CryptoAuthLib as a subdirectory and separate git repo within your
own project.  Changes and commits to your project vs CryptoAuthLib will remain
separated into each respective repository.

If there is a project you want to checkout that already incorporates
CryptoAuthLib as a submodule if you clone the repo that incorporates
CryptoAuthLib, after cloning, you'll still need to fill out the CryptoAuthLib
submodule after cloning:
```bash
git submodule init
git submodule update --remote
cd cryptoauthlib
git checkout master
```

Now that CryptoAuthLib is a full-fledged submodule in your git project, in
order to easily add it to your project within Atmel Studio, please see this
[tip](http://avrstudio5.wordpress.com/2011/07/12/tip-add-existing-multiple-files-and-folders-to-an-avr-studio-project-quickly/)

Incorporating CryptoAuthLib in a project
-----------------------------------------
1) In your Makefile or IDE, choose the HAL support you need from the HAL
   directory and exclude other HAL files from your project.

2) For I2C interfaces, define the symbol ATCA_HAL_I2C in your compiler's symbol
   definitions.  This will hook up the CryptoAuthLib interface class with your
   HAL implementation of I2C methods.

3) HAL implementations for CDC and HID interfaces to the ATCK101 are also
   included for use with Windows or Linux versions of the test host.

Incorporating CryptoAuthLib in a Linux project using USB HID devices
-----------------------------------------
The Linux HID HAL files use the Linux udev development software package.

To install the udev development package under Ubuntu Linux, please type the
following command at the terminal window:

```bash
sudo apt-get install libudev-dev
```

This adds the udev development development software package to the Ubuntu Linux
installation.

The Linux HID HAL files also require a udev rule to be added to change the
permissions of the USB HID Devices.  Please add a new udev rule for the
Microchip CryptoAuth USB devices.

```bash
cd /etc/udev/rules.d
sudo touch mchp-cryptoauth.rules
```

Edit the mchp-cryptoauth.rules file and add the following line to the file:
```text
SUBSYSTEM=="hidraw", ATTRS{idVendor}=="03eb", ATTRS{idProduct}=="2312", MODE="0666"
```
