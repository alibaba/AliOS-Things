# Acoustic Algorithm Introduction

Acoustic algorithms provided in esp-sr include voice activity detection (VAD), adaptive gain control (AGC), acoustic echo cancellation (AEC), noise suppression (NS), and mic-array speech enhancement (MASE). VAD, AGC, AEC, and NS are supported with either single-mic and multi-mic development board, MASE is supported with multi-mic board only.

## VAD

### Overview

VAD takes an audio stream as input, and outputs the prediction that a frame of the stream contains audio or not.  

### API Reference

#### Header

- esp_vad.h

#### Function

- `vad_handle_t vad_create(vad_mode_t vad_mode, int sample_rate_hz, int one_frame_ms)`

	**Definition**

	Initialization of VAD handle. 

	**Parameter**

	- vad_mode: operating mode of VAD, integer ranging from 1 to 5, larger value indicates more aggressive VAD.
    - sample_rate_hz: audio sampling rate in Hz.
    - one_frame_ms: frame length in ms.

    **Return**

    Handle to VAD.

- `vad_state_t vad_process(vad_handle_t inst, int16_t *data)`

    **Definition**

    Processing of VAD for one frame.

    **Parameter**

    - inst: VAD handle.
    - data: buffer to save both input and output audio stream.

    **Return**

    VAD handle.

- `void vad_destroy(vad_handle_t inst)`

    **Definition**

    Destruction of a VAD handle.

    **Parameter**

    - inst: the VAD handle to be destroyed.

## AGC

### Overview

AGC keeps the volume of audio signal at a stable level to avoid the situation that the signal is so loud that gets clipped or too quiet to trigger the speech recognizer.

### API Reference

- `void *esp_agc_open(int agc_mode, int sample_rate)`

    **Definition**

    Initialization of AGC handle.

    **Parameter**

    - agc_mode: operating mode of AGC, 3 to enable AGC and 0 to disable it.
    - sample_rate: sampling rate of audio signal.

    **Return**

    - AGC handle.

- `int esp_agc_process(void *agc_handle, short *in_pcm, short *out_pcm, int frame_size, int sample_rate)`

    **Definition**

    Pocessing of AGC for one frame.

    **Parameter**

    - agc_handle: AGC handle.
    - in_pcm: input audio stream.
    - out_pcm: output audio stream.
    - frame_size: signal frame length in ms.
    - sample_rate: signal sampling rate in Hz.

    **Return**

    Return 0 if AGC processing succeeds, -1 if fails; -2 and -3 indicate invalid input of sample_rate and frame_size, respectively.

- `void esp_agc_clse(void *agc_handle)`

    **Definition**

    Destruction of an AGC handle.

    **Parameter**

    - agc_handle: the AGC handle to be destroyed.

## AEC

### Overview

AEC suppresses echo of the sound played by the speaker of the board.

### API Reference

- `aec_handle_t aec_create(int sample_rate, int frame_length, int filter_length)`

    **Definition**

    Initialization of AEC handle.

    **Parameter**

    - sample_rate: audio signal sampling rate.
    - frame_length: audio frame length in ms.
    - filter_length: the length of adaptive filter in AEC.

    **Return**

    Handle to AEC.

- `aec_create_t aec_create_multimic(int sample_rate, int frame_length, int filter_length, int nch)`

    **Definition**

    Initialization of AEC handle.

    **Parameter**

    - sample_rate: audio signal sampling rate.
    - frame_length: audio frame length in ms.
    - filter_length: the length of adaptive filter in AEC.
    - nch: number of channels of the signal to be processed.

    **Return**

    Handle to AEC.

- `void aec_process(aec_handle_t inst, int16_t *indata, int16_t *refdata, int16_t *outdata)`
    
    **Definition**

    Processing of AEC for one frame.

    **Parameter**

    - inst: AEC handle.
    - indata: input audio stream, which could be single- or multi-channel, depending on the channel number defined on initialization.
    - refdata: reference signal to be cancelled from the input.
    - outdata: output audio stream, the number of channels is the same as indata.

- `void aec_destroy(aec_handle_t inst)`

    **Definition**

    Destruction of an AEC handle.

    **Parameter**

    - inst: the AEC handle to be destroyed.

## NS

### Overview

Single-channel speech enhancement. If multiple mics are available with the board, MASE is recommened for noise suppression.

### API Reference

- `ns_handle_t ns_create(int frame_length_ms)`

    **Definition**

    Initialization of NS handle.

    **Parameter**

    - frame_length_ms: audio frame length in ms.

    **Return**

    Handle to NS.

- `void ns_process(ns_handle_t inst, int16_t *indata, int16_t *outdata)`

    **Definition**

    Prodessing of NS for one frame.

    **Parameter**

    - inst: NS handle.
    - indata: input audio stream.
    - outdata: output audio stream.

- `void ns_destroy(ns_handle_t inst)`

    **Definition**

    Destruction of a NS handle.

    **Parameter**

    - inst: the NS handle to be destroyed.

## MASE

### Overview

Multi-channel speech enhancement. Currently, 2-mic linear array and 3-mic circular array are supported.

### API Reference

- `mase_handle_t mase_create(int sample_rate, int frame_size, int array_type, float mic_distance, int operating_mode, int filter_strength)`

    **Definition**

    Initialization of MASE handle.

    **Parameter**

    - sample_rate: signal sampling rate in Hz.
    - frame_size: signal frame length in ms.
    - array_type: 0 for 2-mic linear array and 1 for 3-mic circular array.
    - mic_distance: distance between two microphones in mm.
    - operating_mode: 0 for normal mode and 1 for wake-up enhanced mode.
    - filter_strength: strength of the mic-array speech enhancement, must be 0, 1, 2 or 3 (smaller number indicates better performance and larger hardware cost).
    - aec_on: true to enable and false to disable AEC.
    - filter_length: the length of adaptive filter in AEC.

    **Return**

    Handle to MASE.

- `void mase_process(mase_handle_t st, int16_t *in, int16_t *dsp_out)`

    **Definition**

    Processing of MASE for one frame.

    **Parameter**

    - st: MASE handle.
    - in: input multi-channel audio stream.
    - dsp_out: output single-channel audio stream.

- `void mase_destory(mase_handle_t st)`

    **Definition**

    Destruction of a MASE handle.

    **Parameter**

    - inst: the MASE handle to be destroyed.