
#ifndef TOUCH_TO_STRETCH_H
#define TOUCH_TO_STRETCH_H
	#ifdef  __cplusplus
	extern "C" {
	#endif
#include "STTypes.h"

typedef void STPrivData;

typedef struct RunParameter_s
{
	int   VoiceChangeFlag;
	int   sample_rate;
	int	  channel_number;
	float tempoDelta;//'t=xx'[-60.0 60.0]
    float pitchDelta;//'p=xx'[-95.0 5000.0]
    float rateDelta;//'r=xx'[-95.0 5000.0]
    int   quick;//'q','-quick'
    int   noAntiAlias;//'n','-naa'
    float goalBPM;//'b','bpm=xx'
    int  detectBPM;//'b,'bpm=xx'
    int  speech;//'s','-speech'
}RunParameters;

typedef enum SetParam_{
	ALL,
	TEMPO,
    PITCH,
    VPS,
}SetParam;
void RunParameters_RunParameters(RunParameters *params);
void RunParameters_checkLimits(RunParameters *params);
void setupSoundTouch(STPrivData* priv_data, const RunParameters *params, SetParam para);
//void processSoundTouch( FILE *inFile, FILE *outFile, const RunParameters *params);
int  processSoundTouch( SAMPLETYPE *inbufer, SAMPLETYPE *outbuffer, int nSamples,STPrivData* priv_data);
void* SouchTouchInit();
void  SouchTouchDestroy(STPrivData* priv_data);


	#ifdef  __cplusplus
	}
	#endif
#endif//TOUCH_TO_STRETCH_H

