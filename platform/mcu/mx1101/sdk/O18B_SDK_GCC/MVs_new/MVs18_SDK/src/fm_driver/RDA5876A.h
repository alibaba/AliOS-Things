/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			RDA5876A.h
* Description:		RDA5876A FM driver header file.
* maintainer: Sam
******************************************************************************
*/
#ifndef __RDA5876A_H__
#define	__RDA5876A_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

extern uint16_t Rda5876hpReadID(void);
extern bool Rda5876hpInit(void);
extern void Rda5876hpMute(bool MuteFg);
extern bool Rda5876hpPatchPowerOn(void);
extern bool Rda5876hpPatchPowerOff(void);
extern void Rda5876hpFreqSet(uint16_t Freq);
extern bool Rda5876hpGetTuneResult(uint16_t Freq);

extern bool Rda5876hpIsStereo(void);
extern void Rda5876hpSetStereo(bool StereoFlag);
extern void Rda5876hpSetHighImpedance(bool Flag);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif //__RDA5876A_H__
