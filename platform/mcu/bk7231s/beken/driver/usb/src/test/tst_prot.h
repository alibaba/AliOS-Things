/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2004              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_TEST_PROTO_H__
#define __MUSB_TEST_PROTO_H__

/*
 * MUSBStack-S unit-test prototypes.
 * $Revision: 1.6 $
 */

#include "mu_tools.h"

/**
 * Test the array/linked-list library.
 * It is built on the micro-stdio and micro-string libraries,
 * so these are assumed already tested.
 * stdio's output uses MUSB_WriteConsole, so this is also assumed to work.
 * <p>
 * Tests include:
 * <ul>
 * <li>Fill/empty/re-fill, checking sizes at each step
 * <li>Different element sizes and alignments
 * <li>Corruption checks (using padding)
 * <li>Insertion into empty list
 * <li>Insertion into non-empty list
 * <li>Removal from empty list
 * <li>Removal from non-empty list
 * </ul>
 *
 * @param argc ANSI argc, reserved for possible future use
 * @param argv ANSI argv, reserved for possible future use
 *
 * @return the number of failures
 */
extern uint16_t MGC_ArrayListTestRun(int argc, char *argv[]);

/**
 * Test the bit-manipulation library.
 * It is built on the micro-stdio and micro-string libraries,
 * so these are assumed already tested.
 * stdio's output uses MUSB_WriteConsole, so this is also assumed to work.
 * <p>
 * Tests include:
 * <ul>
 * <li>Verifying correct bits are extracted/inserted with several
 * cases designed to cross byte boundaries
 * and representing all byte magnitudes
 * <li>Corruption check (using padding) of input buffer
 * <li>Corruption check (using padding) of all filled arguments
 * <li>Repeat with varying buffer sizes and alignments
 * </ul>
 *
 * @param argc ANSI argc, reserved for possible future use
 * @param argv ANSI argv, reserved for possible future use
 *
 * @return the number of failures
 */
extern uint16_t MGC_BitsTestRun(int argc, char *argv[]);

/**
 * Test the USB descriptor analysis library.
 * It is built on the micro-stdio and micro-string libraries,
 * so these are assumed already tested.
 * stdio's output uses MUSB_WriteConsole, so this is also assumed to work.
 * <p>
 * Tests include:
 * <ul>
 * <li>Verifying correct descriptors are found at all levels
 * in all supported cases
 * <li>Verifying descriptors outside of config length are NOT found
 * <li>Verifying arbitrary non-existent descriptors are NOT found
 * <li>Verifying different ways to find the same descriptor all agree
 * <li>Corruption check (using padding) of input descriptors
 * <li>Corruption check (using padding) of all filled arguments
 * <li>All tests repeated for (1) a complex real-world config,
 * (2) a contrived config designed to have maximum "border crossings"
 * and to catch static assumptions,
 * and (3) a malformed config to characterize behavior when wTotalLength
 * is too large and bDescriptorSize's are wrong
 * </ul>
 *
 * @param argc ANSI argc, reserved for possible future use
 * @param argv ANSI argv, reserved for possible future use
 *
 * @return the number of failures
 */
extern uint16_t MGC_DeviceTestRun(int argc, char *argv[]);

/**
 * Unit-test UCD's device-side functionality.
 *
 * @param argc ANSI argc, reserved for possible future use
 * @param argv ANSI argv, reserved for possible future use
 *
 * @return the number of failures
 */
extern uint16_t MGC_FunctionTestRun(int argc, char *argv[]);

/**
 * Unit-test the UCD's host functionality.
 * This includes testing of the behavior of address management
 * and driver matching, with corruption checking
 *
 * @param argc ANSI argc, reserved for possible future use
 * @param argv ANSI argv, reserved for possible future use
 *
 * @return the number of failures
 */
extern uint16_t MGC_HostTestRun(int argc, char *argv[]);

/**
 * Test the stack (LIFO data structure) library.
 * It is built on the micro-stdio and micro-string libraries,
 * so these are assumed already tested.
 * stdio's output uses MUSB_WriteConsole, so this is also assumed to work.
 * <p>
 * Tests include:
 * <ul>
 * <li>Fill/empty/re-fill, checking sizes at each step
 * <li>Different element sizes and alignments
 * <li>Corruption checks (using padding)
 * <li>Push onto empty stack
 * <li>Push onto non-empty stack
 * <li>Pop from empty stack
 * <li>Pop from non-empty stack
 * </ul>
 *
 * @param argc ANSI argc, reserved for possible future use
 * @param argv ANSI argv, reserved for possible future use
 *
 * @return the number of failures
 */
extern uint16_t MGC_StackTestRun(int argc, char *argv[]);

/**
 * Test the micro-string library.
 * It is built on the micro-stdio library,
 * so this is assumed already tested.
 * stdio's output uses MUSB_WriteConsole, so this is also assumed to work.
 * <p>
 * This requires a system to be modified slightly to instantiate
 * controller(s) and simulate continuous interrupts on each one.
 * If a system is designed to support only a single controller,
 * testing it as such is acceptable (NOTE: this implies a separate
 * build is necessary to test each supported controller type).
 * Otherwise, the system should request instantiation
 * of at least two controllers of any type(s) it supports.
 * <p>
 * Tests include:
 * <ul>
 * <li>StringLength functionality on NULL, empty, and different-sized
 * strings in ROMable, global RAM and stack RAM locations
 * <li>StringConcat functionality with normal and overflow conditions
 * <li>StringFind functionality at non-border and border cases,
 * with and without case-sensitivity
 * <li>StringCompare functionality at non-border and border cases,
 * with and without case-sensitivity
 * <li>StringParse functionality with properly-formed and malformed
 * input, in each base including auto-base, with implied negatives,
 * positives and zeros of varying required representation widths
 * (including overflow)
 * <li>Stringize functionality for normal and overflow conditions
 * with various bases and justification
 * </ul>
 *
 * @param argc ANSI argc, reserved for possible future use
 * @param argv ANSI argv, reserved for possible future use
 *
 * @return the number of failures
 */
extern uint16_t MGC_StringTestRun(int argc, char *argv[]);

/**
 * Test a system implementation by being fake controllers.
 * It is built on the micro-stdio and micro-string libraries,
 * so these are assumed already tested.
 * stdio's output uses MUSB_WriteConsole, so this is also assumed to work.
 * <p>
 * This requires a system to be instrumented for testing,
 * which involves implementing MUSB_SystemTestStart and
 * MUSB_SystemTestStop, conditionally-compiled #ifdef MUSB_SYSTEM_TEST.
 * <p>
 * Tests include:
 * <ul>
 * <li>Verifying correct ISRs are called with correct params,
 * and no data corruption of params, repeat with params stored
 * in different places (walk through array)
 * <li>Verifying ISR/BSR calling logic
 * <li>Verifying locks, queue, and timers operation (including boundaries)
 * <li>Stress-test locks, queue, timers
 * </ul>
 *
 * @param argc ANSI argc, reserved for possible future use
 * @param argv ANSI argv, reserved for possible future use
 *
 * @return the number of failures
 *
 * @see MUSB_SystemTestStart
 * @see MUSB_SystemTestStop
 */
extern uint16_t MGC_SystemTestRun(int argc, char *argv[]);

/**
 * Test start hook for systems instrumented for testing.
 * This function should begin simulated interrupts for each test controller
 * (from a low-priority task or with a delay,
 * or whatever is necessary to allow the background task to run).
 * For maximum testing integrity, the background task mechanism
 * should be as close as possible to the version intended for deployment.
 * <p>
 * A system under test is built with the preprocessor macro
 * MUSB_SYSTEM_TEST defined, and linked with the system test module
 * instead of the UCD library.
 * When built this way, its MUSB_InitSystem() function
 * should instantiate controllers (via MUSB_NewController),
 * preferably two of each supported type.
 * If a system is designed to support only a single controller,
 * testing it as such is acceptable (NOTE: this implies a separate
 * build is necessary to test each supported controller type).
 * The base addresses are ignored by the test module,
 * but unique values for each controller will simplify problem diagnosis.
 *
 * @param abInterruptCounts an array of suggested simulated interrupt counts.
 * The system should cycle through this array until the stop hook is called.
 * Each element represents the number of consecutive interrupts to simulate
 * on a given controller instance, before moving to the next controller.
 * For example, if two controllers were instantiated,
 * this array should have two elements, which could be { 120, 60 }.
 * This means the system should call the first instantiated
 * controller's ISR 120 consecutive times,
 * then the second controller's ISR 60 times,
 * repeating this pattern until the stop hook is called.
 *
 * @param bCountArrayLength the length, in bytes, of the count array
 * (the number of controllers instantiated)
 *
 * @return TRUE on success
 * @return FALSE on failure
 *
 * @see MUSB_InitSystem
 * @see MUSB_SystemTestStop
 */
extern uint8_t MUSB_SystemTestStart(const uint8_t *abInterruptCounts,
                                    uint8_t bCountArrayLength);

/**
 * Test stop hook for systems instrumented for testing.
 * This function should stop simulating interrupts.
 * <p>
 * A system under test is built with the preprocessor macro
 * MUSB_SYSTEM_TEST defined, and linked with the system test module
 * instead of the UCD library.
 *
 * @return TRUE on success
 * @return FALSE on failure
 *
 * @see MUSB_InitSystem
 * @see MUSB_SystemTestStart
 */
extern uint8_t MUSB_SystemTestStop(void);

/**
 * Delay calling thread.
 * Delay the calling thread (allowing other threads to run if supported).
 * @param dwTime the minimum delay, in milliseconds
 * @return status code
 */
extern uint32_t MUSB_Sleep(uint32_t dwTime);

#endif	/* multiple inclusion protection */
