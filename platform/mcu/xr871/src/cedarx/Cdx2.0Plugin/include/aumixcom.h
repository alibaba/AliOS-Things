
#ifndef	___aumixcommon__
#define	___aumixcommon__
typedef	struct __PcmInfo__{
//input infor
    unsigned int   SampleRate;
    short          *PCMPtr;
    unsigned int   PcmLen;
    unsigned int   Chan;
}PcmInfo;

typedef	struct ___AudioMix__{
//input para
    PcmInfo      *InputA;
    PcmInfo      *InputB;
    PcmInfo      *TempBuf;
    PcmInfo      *Output;
    unsigned int ByPassflag;
    //output para
    short        *MixbufPtr;
    unsigned int MixLen;
    unsigned int Mixch;
	void*        RESI;

}AudioMix;
#if MIX_SUPPORT
int     do_AuMIX(AudioMix	*AMX);
#endif
void*   Init_ResampleInfo();
void    Destroy_ResampleInfo(void * ri);

#endif
