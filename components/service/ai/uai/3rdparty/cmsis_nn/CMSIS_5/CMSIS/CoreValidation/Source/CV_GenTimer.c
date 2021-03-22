/*-----------------------------------------------------------------------------
 *      Name:         CV_GenTimer.c 
 *      Purpose:      CMSIS CORE validation tests implementation
 *-----------------------------------------------------------------------------
 *      Copyright (c) 2017 ARM Limited. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "CV_Framework.h"
#include "cmsis_cv.h"

/*-----------------------------------------------------------------------------
 *      Test implementation
 *----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 *      Test cases
 *----------------------------------------------------------------------------*/

 
/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_GenTimer_CNTFRQ(void) {
  const uint32_t cntfrq1 = __get_CNTFRQ();
  __set_CNTFRQ(cntfrq1 + 1U);
  const uint32_t cntfrq2 = __get_CNTFRQ();

  ASSERT_TRUE((cntfrq1 + 1U) == cntfrq2);
  
  __set_CNTFRQ(cntfrq1);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_GenTimer_CNTP_TVAL(void) {
  const uint32_t cntp_tval1 = __get_CNTP_TVAL();
  __set_CNTP_TVAL(cntp_tval1 + 1U);
  const uint32_t cntp_tval2 = __get_CNTP_TVAL();

  ASSERT_TRUE((cntp_tval2 - cntp_tval1) >= 1ULL);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_GenTimer_CNTP_CTL(void) {
  static const uint32_t CNTP_CTL_ENABLE = 0x01U;
  const uint32_t cntp_ctl = __get_CNTP_CTL();
  const uint32_t cntp_ctl_toggled = (cntp_ctl & (~CNTP_CTL_ENABLE)) | ((~cntp_ctl) & CNTP_CTL_ENABLE);
  __set_CNTP_CTL(cntp_ctl_toggled);

  const uint32_t cntp_ctl_new = __get_CNTP_CTL();

  ASSERT_TRUE((cntp_ctl_toggled & CNTP_CTL_ENABLE) == (cntp_ctl_new & CNTP_CTL_ENABLE));
  
  __set_CNTP_CTL(cntp_ctl);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_GenTimer_CNTPCT(void) {
  const uint64_t cntpct1 = __get_CNTPCT();
  for(int i=0; i<10; i++);
  const uint64_t cntpct2 = __get_CNTPCT();

  ASSERT_TRUE((cntpct2 - cntpct1) <= 120ULL);
}

/*=======0=========1=========2=========3=========4=========5=========6=========7=========8=========9=========0=========1====*/
void TC_GenTimer_CNTP_CVAL(void) {
  const uint64_t cntp_cval1 = __get_CNTP_CVAL();
  __set_CNTP_CVAL(cntp_cval1 + 1ULL);
  const uint64_t cntp_cval2 = __get_CNTP_CVAL();

  ASSERT_TRUE((cntp_cval2 - cntp_cval1) >= 1ULL);
}
