
#ifndef	___AUGAINCOM_H__
#define	___AUGAINCOM_H__

typedef	struct ___AudioGain__{
//input para
	int    preamp;//-20 -- 20 db	
	int    InputChan;
	short  *InputPtr;
	int    InputLen;//total byte
	int    OutputChan;//0 输出左 1: 输出右声道only 2：输出左右声道  3: double left 4:double right
	short  *OutputPtr;
}AudioGain;
int	do_AudioGain(AudioGain *AGX);
#endif//___AUGAINCOM_H__