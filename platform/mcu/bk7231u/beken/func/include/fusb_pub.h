#ifndef _FUSB_PUB_H_
#define _FUSB_PUB_H_

//#define FMSC_TEST
//#define FHID_TEST
//#define FUVC_TEST

#define FUSB_FAILURE        ((UINT32)-1)
#define FUSB_SUCCESS        (0)

extern UINT32 fusb_init(void);
void fmsc_fiddle_process(void);

#endif 
// eof

