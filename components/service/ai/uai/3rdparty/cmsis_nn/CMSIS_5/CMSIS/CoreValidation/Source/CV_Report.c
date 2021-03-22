/*-----------------------------------------------------------------------------
 *      Name:         cv_report.c
 *      Purpose:      Report statistics and layout implementation
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2017 - 2018 Arm Limited. All rights reserved.
 *----------------------------------------------------------------------------*/
#include "CV_Report.h"
#include <stdio.h>
#include <string.h>

TEST_REPORT test_report;
static AS_STAT     current_assertions;   /* Current test case assertions statistics  */
#define TAS (&test_report.assertions)         /* Total assertions             */
#define CAS (&current_assertions)             /* Current assertions           */

#ifdef DISABLE_SEMIHOSTING
#if defined (__CC_ARM)
  #pragma import __use_no_semihosting
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  __ASM(".global __use_no_semihosting");
#endif
#define PRINT(x)
#define FLUSH()
void _sys_exit(int return_code) {}
#else
#define PRINT(x) MsgPrint x
#define FLUSH()  MsgFlush()
#endif // DISABLE_SEMIHOSTING

static uint8_t Passed[] = "PASSED";
static uint8_t Warning[] = "WARNING";
static uint8_t Failed[] = "FAILED";
static uint8_t NotExe[] = "NOT EXECUTED";


/*-----------------------------------------------------------------------------
 * Test report function prototypes
 *----------------------------------------------------------------------------*/
static BOOL     tr_Init   (void);
static BOOL     tc_Init   (void);
static uint8_t *tr_Eval   (void);
static uint8_t *tc_Eval   (void);
static BOOL     StatCount (TC_RES res);

/*-----------------------------------------------------------------------------
 * Printer function prototypes
 *----------------------------------------------------------------------------*/
static void MsgPrint (const char *msg, ...);
static void MsgFlush (void);


/*-----------------------------------------------------------------------------
 * Assert interface function prototypes
 *----------------------------------------------------------------------------*/
static BOOL As_File_Result (TC_RES res);
static BOOL As_File_Dbgi   (TC_RES res, const char *fn, uint32_t ln, char *desc);

TC_ITF tcitf = {
  As_File_Result,
  As_File_Dbgi,
};


/*-----------------------------------------------------------------------------
 * Test report interface function prototypes
 *----------------------------------------------------------------------------*/
BOOL tr_File_Init  (void);
BOOL tr_File_Open  (const char *title, const char *date, const char *time, const char *fn);
BOOL tr_File_Close (void);
BOOL tc_File_Open  (uint32_t num, const char *fn);
BOOL tc_File_Close (void);

REPORT_ITF ritf = {
  tr_File_Init,
  tr_File_Open,
  tr_File_Close,
  tc_File_Open,
  tc_File_Close
};


/*-----------------------------------------------------------------------------
 * Init test report
 *----------------------------------------------------------------------------*/
BOOL tr_File_Init (void) {
  return (tr_Init());
}


/*-----------------------------------------------------------------------------
 * Open test report
 *----------------------------------------------------------------------------*/
#if (PRINT_XML_REPORT==1)
BOOL tr_File_Open (const char *title, const char *date, const char *time, const char *fn) {
  PRINT(("<?xml version=\"1.0\"?>\n"));
  PRINT(("<?xml-stylesheet href=\"TR_Style.xsl\" type=\"text/xsl\" ?>\n"));
  PRINT(("<report>\n"));
  PRINT(("<test>\n"));
  PRINT(("<title>%s</title>\n", title));
  PRINT(("<date>%s</date>\n",   date));
  PRINT(("<time>%s</time>\n",   time));
  PRINT(("<file>%s</file>\n",   fn));
  PRINT(("<test_cases>\n"));
#else
BOOL tr_File_Open (const char *title, const char *date, const char *time, const char __attribute__((unused)) *fn) {
  PRINT(("%s   %s   %s \n\n", title, date, time));
#endif
  return (__TRUE);
}


/*-----------------------------------------------------------------------------
 * Open test case
 *----------------------------------------------------------------------------*/
BOOL tc_File_Open (uint32_t num, const char *fn) {
  (void)tc_Init ();
#if (PRINT_XML_REPORT==1)
  PRINT(("<tc>\n"));
  PRINT(("<no>%d</no>\n",     num));
  PRINT(("<func>%s</func>\n", fn));
  PRINT(("<req></req>"));
  PRINT(("<meth></meth>"));
  PRINT(("<dbgi>\n"));
#else
  PRINT(("TEST %02d: %-42s ", num, fn));
#endif
  return (__TRUE);
}


/*-----------------------------------------------------------------------------
 * Close test case
 *----------------------------------------------------------------------------*/
BOOL tc_File_Close (void) {
  uint8_t *res = tc_Eval();
#if (PRINT_XML_REPORT==1)
  PRINT(("</dbgi>\n"));
  PRINT(("<res>%s</res>\n", res));
  PRINT(("</tc>\n"));
#else
  if ((res==Passed)||(res==NotExe)) {
    PRINT(("%s\n", res));
  } else {
    PRINT(("\n"));
  }
#endif
  FLUSH();
  return (__TRUE);
}


/*-----------------------------------------------------------------------------
 * Close test report
 *----------------------------------------------------------------------------*/
BOOL tr_File_Close (void) {
#if (PRINT_XML_REPORT==1)
  PRINT(("</test_cases>\n"));
  PRINT(("<summary>\n"));
  PRINT(("<tcnt>%d</tcnt>\n", test_report.tests));
  PRINT(("<exec>%d</exec>\n", test_report.executed));
  PRINT(("<pass>%d</pass>\n", test_report.passed));
  PRINT(("<fail>%d</fail>\n", test_report.failed));
  PRINT(("<warn>%d</warn>\n", test_report.warnings));
  PRINT(("<tres>%s</tres>\n", tr_Eval()));
  PRINT(("</summary>\n"));
  PRINT(("</test>\n"));
  PRINT(("</report>\n"));
#else
  PRINT(("\nTest Summary: %d Tests, %d Executed, %d Passed, %d Failed, %d Warnings.\n",
         test_report.tests,
         test_report.executed,
         test_report.passed,
         test_report.failed,
         test_report.warnings));
  PRINT(("Test Result: %s\n", tr_Eval()));
#endif
  FLUSH();
  return (__TRUE);
}


/*-----------------------------------------------------------------------------
 * Assertion result counter
 *----------------------------------------------------------------------------*/
static BOOL As_File_Result (TC_RES res) {
  return (StatCount (res));
}


/*-----------------------------------------------------------------------------
 * Set debug information state
 *----------------------------------------------------------------------------*/
#if (PRINT_XML_REPORT==1)
static BOOL As_File_Dbgi (TC_RES __attribute__((unused)) res, const char *fn, uint32_t ln, char *desc) {
  PRINT(("<detail>\n"));
  if (desc!=NULL) PRINT(("<desc>%s</desc>\n", desc));
  PRINT(("<module>%s</module>\n", fn));
  PRINT(("<line>%d</line>\n", ln));
  PRINT(("</detail>\n"));
#else
static BOOL As_File_Dbgi (TC_RES res, const char *fn, uint32_t ln, char *desc) {
  PRINT(("\n  %s (%d)", fn, ln));
  if (res==WARNING){ PRINT((" [WARNING]")); }
  if (res==FAILED) { PRINT((" [FAILED]"));  }
  if (desc!=NULL)  { PRINT((" %s", desc));  }
#endif
  return (__TRUE);
}


/*-----------------------------------------------------------------------------
 * Init test report
 *----------------------------------------------------------------------------*/
static BOOL tr_Init (void) {
  TAS->passed = 0;
  TAS->failed = 0;
  TAS->warnings = 0;
  return (__TRUE);
}


/*-----------------------------------------------------------------------------
 * Init test case
 *----------------------------------------------------------------------------*/
static BOOL tc_Init (void) {
  CAS->passed = 0;
  CAS->failed = 0;
  CAS->warnings = 0;
  return (__TRUE);
}


/*-----------------------------------------------------------------------------
 * Evaluate test report results
 *----------------------------------------------------------------------------*/
static uint8_t *tr_Eval (void) {
  if (test_report.failed > 0U) {
    /* Test fails if any test case failed */
    return (Failed);
  }
  else if (test_report.warnings > 0U) {
    /* Test warns if any test case warnings */
    return (Warning);
  }
  else if (test_report.passed > 0U) {
    /* Test passes if at least one test case passed */
    return (Passed);
  }
  else {
    /* No test cases were executed */
    return (NotExe);
  }
}


/*-----------------------------------------------------------------------------
 * Evaluate test case results
 *----------------------------------------------------------------------------*/
static uint8_t *tc_Eval (void) {
  test_report.tests++;
  test_report.executed++;

  if (CAS->failed > 0U) {
    /* Test case fails if any failed assertion recorded */
    test_report.failed++;
    return Failed;
  }
  else if (CAS->warnings > 0U) {
    /* Test case warns if any warnings assertion recorded */
    test_report.warnings++;
    return Warning;
  }
  else if (CAS->passed > 0U) {
    /* Test case passes if at least one assertion passed */
    test_report.passed++;
    return Passed;
  }
  else {
    /* Assert was not invoked - nothing to evaluate */
    test_report.executed--;
    return NotExe;
  }
}


/*-----------------------------------------------------------------------------
 * Statistics result counter
 *----------------------------------------------------------------------------*/
static BOOL StatCount (TC_RES res) {
  switch (res) {
    case PASSED:
      CAS->passed++;
      TAS->passed++;
      break;

    case WARNING:
      CAS->warnings++;
      TAS->warnings++;
      break;

    case FAILED:
      CAS->failed++;
      TAS->failed++;
      break;

    case NOT_EXECUTED:
      return (__FALSE);

    default:
      break;
  }
  return (__TRUE);
}


/*-----------------------------------------------------------------------------
 * Set result
 *----------------------------------------------------------------------------*/
TC_RES __set_result (const char *fn, uint32_t ln, TC_RES res, char* desc) {

  // save assertion result
  switch (res) {
    case PASSED:
      if (TAS->passed < BUFFER_ASSERTIONS) {
        test_report.assertions.info.passed[TAS->passed].module = fn;
        test_report.assertions.info.passed[TAS->passed].line = ln;
      }
      break;
    case FAILED:
      if (TAS->failed < BUFFER_ASSERTIONS) {
        test_report.assertions.info.failed[TAS->failed].module = fn;
        test_report.assertions.info.failed[TAS->failed].line = ln;
      }
      break;
    case WARNING:
      if (TAS->warnings < BUFFER_ASSERTIONS) {
        test_report.assertions.info.warnings[TAS->warnings].module = fn;
        test_report.assertions.info.warnings[TAS->warnings].line = ln;
      }
      break;
    case NOT_EXECUTED:
      break;

    default:
      break;
  }

  // set debug info (if the test case didn't pass)
  if (res != PASSED) { (void)tcitf.Dbgi (res, fn, ln, desc); }
  // set result
  (void)tcitf.Result (res);
  return (res);
}

/*-----------------------------------------------------------------------------
 * Assert true
 *----------------------------------------------------------------------------*/
TC_RES __assert_true (const char *fn, uint32_t ln, uint32_t cond) {
  TC_RES res = FAILED;
  if (cond != 0U) { res = PASSED; }
  (void)__set_result(fn, ln, res, NULL);
  return (res);
}

#ifndef DISABLE_SEMIHOSTING
/*-----------------------------------------------------------------------------
 *       MsgFlush:  Flush the standard output
 *----------------------------------------------------------------------------*/
static void MsgFlush(void) {
  (void)fflush(stdout);
}

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif
/*-----------------------------------------------------------------------------
 *       MsgPrint:  Print a message to the standard output
 *----------------------------------------------------------------------------*/
static void MsgPrint (const char *msg, ...) {
  va_list args;
  va_start(args, msg);
  vprintf(msg, args);
  va_end(args);
}
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic pop
#endif
#endif // DISABLE_SEMIHOSTING

/*-----------------------------------------------------------------------------
 * End of file
 *----------------------------------------------------------------------------*/
