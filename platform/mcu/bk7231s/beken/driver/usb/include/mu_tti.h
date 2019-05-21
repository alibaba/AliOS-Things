/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2006              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

/*
 * MUSB-MicroSW TTI (Transport Test Interface).
 * $Revision: 1.3 $
 */

#ifndef __MUSB_TTI_H__
#define __MUSB_TTI_H__

#include "mu_tools.h"

/**
 * Introduction to Transport Test Interface.
 */

/************************* TTI CONSTANTS **************************/

/**
 * MUSB_TtiTestType.
 * TTI Test Type.
 */
typedef enum
{
    MUSB_TTI_TYPE_NONE,
    MUSB_TTI_TYPE_SINK,
    MUSB_TTI_TYPE_SOURCE,
    MUSB_TTI_TYPE_LOOPBACK
} MUSB_TtiTestType;

/**
 * MUSB_TtiTestOption.
 * TTI Test Option.
 */
typedef enum
{
    MUSB_TTI_OPTION_NONE,
    MUSB_TTI_OPTION_INCREMENT,
    MUSB_TTI_OPTION_DECREMENT,
    MUSB_TTI_OPTION_ROTATE_LEFT,
    MUSB_TTI_OPTION_ROTATE_RIGHT
} MUSB_TtiTestOption;

/*************************** TTI TYPES ****************************/

/**
 * MUSB_TtiTestSpec.
 * Test specification.
 * @param dwIterations how many iterations of this test to do before moving
 * to the next test in the suite (if any)
 * @param dwActualIterations updated with actual iteration count
 * @param dwErrorIterations updated with iteration count that had errors
 * @field TestType the type of test
 */
typedef struct
{
    uint32_t dwIterations;
    uint32_t dwActualIterations;
    uint32_t dwErrorIterations;
    uint16_t wMaxPacketSize;
    MUSB_TtiTestType TestType;
    MUSB_TtiTestOption TestOption;
    uint8_t bProtocol;
    uint8_t bInEnd;
    uint8_t bOutEnd;
    uint8_t bStartPattern;
    uint8_t bPacketCount;
} MUSB_TtiTestSpec;

/**
 * Test progress callback.
 */
typedef void (*MUSB_pfTtiTestProgress)(MUSB_TtiTestSpec *pTestSpec);

/**
 * Test error callback.
 */
typedef void (*MUSB_pfTtiTestError)(MUSB_TtiTestSpec *pTestSpec,
                                    uint32_t dwCdiStatus, uint32_t dwFailedCompareCount);

/**
 * Notification of status of test addition request.
 * @param pTestSpec pointer to test specification
 * @param hTest non-NULL test "handle" on successful addition; NULL on failure
 */
typedef void (*MUSB_pfTtiTestAddStatus)(MUSB_TtiTestSpec *pTestSpec, void *hTest);

/**
 * Add a test
 * @param pDeviceData pPrivateData from dispatch table
 * @param pTestSpec test specification
 * @param pfTestAddStatus notification of status of this request
 * @param pfTestProgress test progress callback; NULL for none
 * @param pfTestError test error callback; NULL for none
 */
typedef void (*MUSB_pfTtiAddTest)(void *pDeviceData, MUSB_TtiTestSpec *pTestSpec,
                                  MUSB_pfTtiTestAddStatus pfTestAddStatus,
                                  MUSB_pfTtiTestProgress pfTestProgress,
                                  MUSB_pfTtiTestError pfTestError);

/**
 * Remove a test
 * @param pDeviceData pPrivateData from dispatch table
 * @param hTest test "handle" from MUSB_pfTestAddStatus callback by MUSB_pfTtiAddTest
 * @see #MUSB_pfTtiAddTest
 */
typedef void (*MUSB_pfTtiRemoveTest)(void *pDeviceData, void *hTest);

/**
 * Start tests
 * @param pDeviceData pPrivateData from dispatch table
 * @param dwIterationCount number of iterations of the complete test suite
 * (0 => infinite)
 */
typedef void (*MUSB_pfTtiStartTests)(void *pDeviceData, uint32_t dwIterationCount);

/**
 * Stop tests
 * @param pDeviceData pPrivateData from dispatch table
 */
typedef void (*MUSB_pfTtiStopTests)(void *pDeviceData);

/**
 * MUSB_TtiRunner.
 * Device driver callbacks (dispatch table).
 * @field pPrivateData device driver private data;
 * not to be interpreted by the TTI implementation
 * @field pfTtiAddTest add-test callback
 * @field pfTtiRemoveTest remove-test callback
 * @field pfTtiStartTests start-tests callback
 * @field pfTtiStopTests stop-tests callback
 */
typedef struct
{
    void *pPrivateData;
    MUSB_pfTtiAddTest pfTtiAddTest;
    MUSB_pfTtiRemoveTest pfTtiRemoveTest;
    MUSB_pfTtiStartTests pfTtiStartTests;
    MUSB_pfTtiStopTests pfTtiStopTests;
} MUSB_TtiRunner;

/************************* TTI FUNCTIONS **************************/

/**
 * Add a transport (test) device.
 * A transport device driver calls this to inform the TTI implementation
 * that a new test device is available.
 *
 * @param pTestRunner pointer to struct with dispatch table to access the device
 */
extern void MUSB_TtiAddTestRunner(MUSB_TtiRunner *pRunner);

/**
 * Remove a transport (test) device.
 * A transport device driver calls this to inform the TTI implementation
 * that a test device is no longer available.
 *
 * @param pTestRunner pointer to struct with dispatch table to access the device
 */
extern void MUSB_TtiRemoveTestRunner(MUSB_TtiRunner *pRunner);

#endif	/* multiple inclusion protection */
