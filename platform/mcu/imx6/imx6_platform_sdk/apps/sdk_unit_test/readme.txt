sdk_unit_test application
=========================

SDK unit test host application.


Description
-----------

The sdk_unit_test application is the interface for running the unit tests for SDK drivers. Depending
on how it is configured at build time, it can either display a menu for selecting the unit test
to execute, or run a single unit test automatically. The application can be a handy way to test
hardware. And the unit tests themselves are useful demonstrations of the driver APIs.


Requirements
------------

Many unit tests require extra hardware to be attached to the board being used. The details of how
to setup and run each unit test are in the SDK Unit Test Procedures.pdf document in the doc
directory.


Build options
-------------

When building sdk_unit_test, you can optionally specify the name of a single unit test. Doing so
will cause the application to automatically run the specified test rather than displaying a
menu for interactive execution of tests.

To set the name of the test to run, use the -test option of build_sdk. The value passed to the
-test option has "_test" appended to it to form the name of the function that will be run when
the application starts.

If no -test option is provided to build_sdk, then the function ALL_test() will be run at startup.
This function displays the test selection menu, and resides in all_test.c (see below).


Code organization
-----------------

The two source files in the src directory encompass the main() function and the menu interface
for all tests.

    unit_test_main.c - Contains main() and invokes the unit test or menu interface.
    
    all_test.c - Implements a menu interface showing all available unit tests for the configuration
            (i.e., chip and board).

The source code for the actual unit tests themselves is built using child makefiles that reside
in the test directories inside the sdk.

